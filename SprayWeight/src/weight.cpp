#include "weight.h"
#include "logging.h"
#include <array>
#include <cerrno>
#include <chrono>
#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <thread>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include "usercmd.h"

namespace
{

	constexpr auto kWeightStableDelay = std::chrono::seconds(2);
	constexpr auto kWeightConnectTimeout = std::chrono::seconds(3);
	constexpr auto kWeightReadTimeout = std::chrono::seconds(3);
	constexpr std::size_t kWeightFrameDistance = 16;
	constexpr std::size_t kWeightDigitsOffset = 4;
	constexpr std::size_t kWeightDigitsLength = 6;
	constexpr std::size_t kWeightSecondStatusOffset = 2;
	constexpr unsigned char kWeightStableMask = 0x08;
	constexpr std::size_t kMaxPendingPayloadBytes = 1024;
 
	enum class WeightReadStatus
	{
		Success,
		Timeout,
		ConnectionError,
	};

	bool TryParseWeightFromPayload(std::string &payload, int &weightValue);

	std::string FormatTcpPayload(const char *data, std::size_t size)
	{
		std::string formatted;
		formatted.reserve(size * 2);

		constexpr char hexDigits[] = "0123456789ABCDEF";
		for (std::size_t index = 0; index < size; ++index)
		{
			unsigned char ch = static_cast<unsigned char>(data[index]);
			if (std::isprint(ch) != 0 && ch != '\r' && ch != '\n' && ch != '\t')
			{
				formatted.push_back(static_cast<char>(ch));
				continue;
			}

			if (ch == '\r' || ch == '\n' || ch == '\t')
			{
				formatted.push_back('\\');
				formatted.push_back(ch == '\r' ? 'r' : (ch == '\n' ? 'n' : 't'));
				continue;
			}

			formatted.push_back('\\');
			formatted.push_back('x');
			formatted.push_back(hexDigits[(ch >> 4) & 0x0F]);
			formatted.push_back(hexDigits[ch & 0x0F]);
		}

		return formatted;
	}

	std::string TrimAsciiWhitespace(const std::string &text)
	{
		std::size_t begin = 0;
		while (begin < text.size() && std::isspace(static_cast<unsigned char>(text[begin])) != 0)
		{
			++begin;
		}

		std::size_t end = text.size();
		while (end > begin && std::isspace(static_cast<unsigned char>(text[end - 1])) != 0)
		{
			--end;
		}

		return text.substr(begin, end - begin);
	}

	using SocketHandle = int;
	constexpr SocketHandle kInvalidSocket = -1;

	void CloseSocket(SocketHandle socketFd)
	{
		close(socketFd);
	}

	int GetSocketError()
	{
		return errno;
	}

	bool IsWouldBlockError(int socketErr)
	{
		return socketErr == EAGAIN || socketErr == EWOULDBLOCK;
	}

	bool SetSocketNonBlocking(SocketHandle socketFd, bool nonBlocking)
	{
		const int flags = fcntl(socketFd, F_GETFL, 0);
		if (flags < 0)
		{
			return false;
		}

		const int nextFlags = nonBlocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
		return fcntl(socketFd, F_SETFL, nextFlags) == 0;
	}

	SocketHandle ConnectWeightSocket(const SprayWeightContext &ctx)
	{
		if (ctx.weightTcpHost.empty())
		{
			spdlog::warn("称重 TCP 客户端未启动: 配置的主机地址为空");
			return kInvalidSocket;
		}
		if (ctx.weightTcpPort <= 0)
		{
			spdlog::warn("称重 TCP 客户端未启动: 配置的端口无效, port={}", ctx.weightTcpPort);
			return kInvalidSocket;
		}

		SocketHandle socketFd = socket(AF_INET, SOCK_STREAM, 0);
		if (socketFd == kInvalidSocket)
		{
			spdlog::error("创建称重 TCP socket 失败, err={}", GetSocketError());
			return kInvalidSocket;
		}

		sockaddr_in serverAddr{};
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port = htons(static_cast<uint16_t>(ctx.weightTcpPort));

		if (inet_pton(AF_INET, ctx.weightTcpHost.c_str(), &serverAddr.sin_addr) != 1)
		{
			spdlog::error("称重 TCP 地址无效: {}", ctx.weightTcpHost);
			CloseSocket(socketFd);
			return kInvalidSocket;
		}

		if (!SetSocketNonBlocking(socketFd, true))
		{
			spdlog::error("设置称重 TCP socket 非阻塞失败, err={}", GetSocketError());
			CloseSocket(socketFd);
			return kInvalidSocket;
		}

		if (connect(socketFd, reinterpret_cast<const sockaddr *>(&serverAddr), sizeof(serverAddr)) != 0)
		{
			const int socketErr = GetSocketError();
			if (!IsWouldBlockError(socketErr) && socketErr != EINPROGRESS)
			{
				spdlog::warn("连接称重 TCP 服务失败, target={}:{}, err={}", ctx.weightTcpHost, ctx.weightTcpPort, socketErr);
				CloseSocket(socketFd);
				return kInvalidSocket;
			}

			fd_set writeSet;
			FD_ZERO(&writeSet);
			FD_SET(socketFd, &writeSet);

			timeval timeout{};
			timeout.tv_sec = static_cast<long>(kWeightConnectTimeout.count());

			const int ready = select(socketFd + 1, nullptr, &writeSet, nullptr, &timeout);
			if (ready <= 0)
			{
				if (ready == 0)
				{
					spdlog::warn("连接称重 TCP 服务超时, target={}:{}", ctx.weightTcpHost, ctx.weightTcpPort);
				}
				else
				{
					spdlog::warn("等待称重 TCP 连接完成失败, err={}, target={}:{}", GetSocketError(), ctx.weightTcpHost, ctx.weightTcpPort);
				}
				CloseSocket(socketFd);
				return kInvalidSocket;
			}

			int connectErr = 0;
			socklen_t connectErrLen = sizeof(connectErr);
			const int getSocketOptRet = getsockopt(socketFd, SOL_SOCKET, SO_ERROR, &connectErr, &connectErrLen);
			if (getSocketOptRet != 0 || connectErr != 0)
			{
				spdlog::warn("称重 TCP 连接建立失败, err={}, target={}:{}",
					getSocketOptRet == 0 ? connectErr : GetSocketError(),
					ctx.weightTcpHost,
					ctx.weightTcpPort);
				CloseSocket(socketFd);
				return kInvalidSocket;
			}
		}

		if (!SetSocketNonBlocking(socketFd, false))
		{
			spdlog::error("恢复称重 TCP socket 阻塞模式失败, err={}", GetSocketError());
			CloseSocket(socketFd);
			return kInvalidSocket;
		}

		spdlog::info("已连接称重 TCP 服务, target={}:{}", ctx.weightTcpHost, ctx.weightTcpPort);
		return socketFd;
	}

	WeightReadStatus ReadAvailableWeightPayload(SocketHandle socketFd, const SprayWeightContext &ctx, std::string &payload, int &weightValue, bool &receivedAnyPayload)
	{
		std::array<char, 1024> buffer{};
		const auto deadline = std::chrono::steady_clock::now() + kWeightReadTimeout;

		while (std::chrono::steady_clock::now() < deadline)
		{
			fd_set readSet;
			FD_ZERO(&readSet);
			FD_SET(socketFd, &readSet);

			const auto remaining = deadline - std::chrono::steady_clock::now();
			timeval timeout{};
			const auto timeoutMicros = std::chrono::duration_cast<std::chrono::microseconds>(remaining.count() > 0 ? remaining : std::chrono::steady_clock::duration::zero());
			timeout.tv_sec = static_cast<long>(timeoutMicros.count() / 1000000);
			timeout.tv_usec = static_cast<long>(timeoutMicros.count() % 1000000);

			int ready = select(socketFd + 1, &readSet, nullptr, nullptr, &timeout);
			if (ready < 0)
			{
				spdlog::warn("称重 TCP select 失败, err={}, target={}:{}", GetSocketError(), ctx.weightTcpHost, ctx.weightTcpPort);
				return WeightReadStatus::ConnectionError;
			}
			if (ready == 0)
			{
				break;
			}

			int received = static_cast<int>(recv(socketFd, buffer.data(), static_cast<int>(buffer.size()), 0));
			if (received > 0)
			{
				receivedAnyPayload = true;
				payload.append(buffer.data(), static_cast<std::size_t>(received));
				spdlog::info("称重 TCP 收到数据: {}", FormatTcpPayload(buffer.data(), static_cast<std::size_t>(received)));

				if (payload.size() > kMaxPendingPayloadBytes)
				{
					payload.erase(0, payload.size() - kMaxPendingPayloadBytes);
				}

				if (TryParseWeightFromPayload(payload, weightValue))
				{
					return WeightReadStatus::Success;
				}
				continue;
			}

			if (received == 0)
			{
				spdlog::warn("称重 TCP 连接被远端关闭, target={}:{}", ctx.weightTcpHost, ctx.weightTcpPort);
				return WeightReadStatus::ConnectionError;
			}

			int socketErr = GetSocketError();
			if (IsWouldBlockError(socketErr))
			{
				continue;
			}

			spdlog::warn("称重 TCP 接收失败, err={}, target={}:{}", socketErr, ctx.weightTcpHost, ctx.weightTcpPort);
			return WeightReadStatus::ConnectionError;
		}

		return WeightReadStatus::Timeout;
	}

	bool TryExtractWeightText(std::string &payload, std::string &weightText)
	{
		if (payload.size() < kWeightFrameDistance + 1)
		{
			return false;
		}

		std::size_t frameStart = 0;
		while (frameStart < payload.size())
		{
			frameStart = payload.find(static_cast<char>(0x02), frameStart);
			if (frameStart == std::string::npos)
			{
				payload.clear();
				return false;
			}

			const std::size_t frameEnd = payload.find(static_cast<char>(0x0D), frameStart);
			if (frameEnd == std::string::npos)
			{
				if (frameStart > 0)
				{
					payload.erase(0, frameStart);
				}
				return false;
			}

			if (frameEnd - frameStart != kWeightFrameDistance)
			{
				spdlog::warn("称重 TCP 帧长度异常: {}", FormatTcpPayload(payload.data() + frameStart, frameEnd - frameStart + 1));
				payload.erase(0, frameEnd + 1);
				frameStart = 0;
				continue;
			}

			if (frameEnd + 1 > payload.size())
			{
				return false;
			}

			const unsigned char secondStatus = static_cast<unsigned char>(payload[frameStart + kWeightSecondStatusOffset]);
			if ((secondStatus & kWeightStableMask) != 0U)
			{
				spdlog::info("称重数据不稳定，继续等待下一帧");
				payload.erase(0, frameEnd + 1);
				frameStart = 0;
				continue;
			}

			// 按协议从 STX(0x02) 到 CR(0x0D) 的固定长度帧中提取 6 位 ASCII 重量字段。
			weightText = TrimAsciiWhitespace(payload.substr(frameStart + kWeightDigitsOffset, kWeightDigitsLength));
			payload.erase(0, frameEnd + 1);
			return true;
		}

		payload.clear();
		return false;
	}

	bool TryConvertWeightTextToCentiKg(const std::string &weightText, int &weightValue)
	{
		char *parseEnd = nullptr;
		errno = 0;
		const double rawWeight = std::strtod(weightText.c_str(), &parseEnd);
		if (parseEnd != weightText.c_str() + weightText.size() || errno == ERANGE)
		{
			return false;
		}

		weightValue = rawWeight < 0.001 ? 0 : static_cast<int>(rawWeight / 10.0 + 0.5);
		return true;
	}

	bool TryParseWeightFromPayload(std::string &payload, int &weightValue)
	{
		std::string weightText;
		while (TryExtractWeightText(payload, weightText))
		{
			// 设备上送的是十进制 ASCII 字符串，沿用现有规则换算为 centi-kg 整数值。
			if (TryConvertWeightTextToCentiKg(weightText, weightValue))
			{
				spdlog::info("称重结果解析完成, raw='{}', weight={}", weightText, weightValue);
				return true;
			}

			spdlog::warn("称重数字转换失败, raw='{}'", weightText);
		}

		return false;
	}

} // namespace

WeightWorker::WeightWorker(SprayWeightContext &ctx)
	: ctx_(ctx)
{
}

void WeightWorker::Run()
{
	spdlog::info("称重线程启动");

	try
	{
		unsigned int err = 0;
		subscribe(ctx_.gplatConn, "START_WEIGHT_EVENT", &err); // 尚未定义
		subscribe(ctx_.gplatConn, "timer_500ms", &err);
	}
	catch (const std::exception &e)
	{
		spdlog::error("订阅gPlat事件失败: {}", e.what());
		throw; // 让Run函数的调用者决定如何处理订阅失败的情况
	}


	while (ctx_.running.load())
	{

		unsigned int err = 0;
		char value[1024] = {0};
		std::string tagname;
		try
		{
			bool ret = waitpostdata(ctx_.gplatConn, tagname, value, 1024, -1, &err);
			if (!ret)
			{
				if (!ctx_.running.load())
				{
					break;
				}
				spdlog::warn("waitpostdata failed, err={}, reconnecting gPlat...", err);
				// 断线重连逻辑
				continue;
			}

			
			if (tagname == "WAIT_TIMEOUT")
			{
				continue;
			}
			
			if (tagname == "timer_500ms")
			{
				continue;
			}
			
			if (tagname == "START_WEIGHT_EVENT" )
			{
			        int  startWeightEvent = read_value<int>(value);
				if (startWeightEvent == 1)
				{
					int weight = ReadWeightCentiKg();
					spdlog::info("称重结果: {}", weight);
					writeb(ctx_.gplatConn, "FINISH_WEIGHT_EVENT", &weight, sizeof(weight), &err);
				}
			}
		}
		catch (const std::exception &ex)
		{
			// std::cerr << "error: " << ex.what() << std::endl;
			spdlog::error("waitpostdata threw an exception: {}", ex.what());
			throw; // 让Run函数的调用者决定如何处理waitpostdata异常
		}
	}
	spdlog::info("称重线程退出");
}

int WeightWorker::ReadWeightCentiKg() const
{
	constexpr int kConnectTimeoutError = -1;
	constexpr int kReadTimeoutError = -2;
	constexpr int kParseError = -3;

	int weight_data = kReadTimeoutError;
	SocketHandle socketFd = ConnectWeightSocket(ctx_); // 连接称重 TCP socket
	if (socketFd == kInvalidSocket)
	{
		return kConnectTimeoutError;
	}

	std::string pendingPayload;
	bool receivedAnyPayload = false;
	int parsedWeight = 0;

	try
	{
		std::this_thread::sleep_for(kWeightStableDelay);

		const WeightReadStatus readStatus = ReadAvailableWeightPayload(socketFd, ctx_, pendingPayload, parsedWeight, receivedAnyPayload);
		if (readStatus == WeightReadStatus::ConnectionError)
		{
			CloseSocket(socketFd);
			return kConnectTimeoutError;
		}

		if (readStatus == WeightReadStatus::Success)
		{
			weight_data = parsedWeight;
		}

		if (weight_data >= 0)
		{
			spdlog::info("本次连接称重结果返回首个有效重量, weight={}", weight_data);
		}
		else if (readStatus == WeightReadStatus::Timeout)
		{
			weight_data = receivedAnyPayload ? kParseError : kReadTimeoutError;
			spdlog::warn(receivedAnyPayload ? "称重数据已接收但未解析出有效重量" : "称重数据读取超时");
		}
		else
		{
			weight_data = kParseError;
			spdlog::warn("称重数据已接收但未解析出有效重量");
		}
	}
	catch (...)
	{
		CloseSocket(socketFd);
		throw;
	}

	CloseSocket(socketFd);

	return weight_data;
}
