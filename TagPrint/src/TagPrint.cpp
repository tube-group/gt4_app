#include "TagPrint.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cerrno>
#include <chrono>
#include <cmath>
#include <csignal>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <optional>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include "usercmd.h"
#include "TagPrintContext.h"

extern volatile sig_atomic_t g_running;

namespace
{
    namespace fs = std::filesystem;

    constexpr std::chrono::milliseconds kPollInterval{2000};

    const std::array<std::string, 25> kBanci = {
        "", "", "", "", "", "", "", "", "", "", "", "夜甲", "夜乙", "夜丙", "夜丁",
        "", "", "", "", "", "", "早甲", "早乙", "早丙", "早丁"};

    struct BundleDataFull
    {
        std::string orderNo;
        std::string bundleNo;
        std::string itemNo;
        std::string rollNo;
        std::string lotNo;
        std::string meltNo;
        std::string zoneNo;
        double diameter = 0.0;
        double wallThickness = 0.0;
        int tube = 0;
        int weight = 0;
        int grossweight = 0;
        double lengthTo = 0.0;
        double lengthFrom = 0.0;
        double totalLength = 0.0;
        std::string ponoIDCoupling;
        std::string lotNoThread;
        std::string banci;
        std::string productDate;
        std::string matNo;
        std::string sgText;
        std::string matText;
        std::string stdText;
        std::string kxText;
        int labLengthType = 0;
        int labWeightType = 0;
        int emType = 0;
        std::array<std::string, 8> labelReqManual;
        int count = 1;
        std::string bundleType;
        std::string directionCode;
        std::string roomNo;
    };

    std::string Trim(std::string value)
    {
        auto notSpace = [](unsigned char ch)
        { return !std::isspace(ch); };
        value.erase(value.begin(), std::find_if(value.begin(), value.end(), notSpace));
        value.erase(std::find_if(value.rbegin(), value.rend(), notSpace).base(), value.end());
        return value;
    }

    std::string ToUpperCopy(std::string value)
    {
        std::transform(value.begin(), value.end(), value.begin(), [](unsigned char ch)
                       { return static_cast<char>(std::toupper(ch)); });
        return value;
    }

    std::string RowString(const pqxx::row &row, const char *field)
    {
        const auto cell = row[field];
        return cell.is_null() ? std::string() : std::string(cell.c_str());
    }

    template <typename T>
    T RowNumber(const pqxx::row &row, const char *field, T fallback)
    {
        const auto cell = row[field];
        return cell.is_null() ? fallback : cell.as<T>();
    }

    std::string FormatFixed(double value, int precision)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << value;
        return oss.str();
    }

    std::string FormatPadded(double value, int width, int precision)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(precision) << std::setw(width) << std::setfill('0') << value;
        return oss.str();
    }

    std::string FormatDateCn(const std::string &yyyymmddhhmmss)
    {
        if (yyyymmddhhmmss.size() < 8)
        {
            return yyyymmddhhmmss;
        }
        return yyyymmddhhmmss.substr(0, 4) + "年" + yyyymmddhhmmss.substr(4, 2) + "月" + yyyymmddhhmmss.substr(6, 2) + "日";
    }

    std::string FormatDateIso(const std::string &yyyymmddhhmmss)
    {
        if (yyyymmddhhmmss.size() < 8)
        {
            return yyyymmddhhmmss;
        }
        return yyyymmddhhmmss.substr(0, 4) + "-" + yyyymmddhhmmss.substr(4, 2) + "-" + yyyymmddhhmmss.substr(6, 2);
    }

    std::string ResolveBanciText(const std::string &banci)
    {
        try
        {
            const auto index = std::stoi(banci);
            if (index >= 0 && static_cast<std::size_t>(index) < kBanci.size())
            {
                return kBanci[static_cast<std::size_t>(index)];
            }
        }
        catch (...)
        {
        }
        return std::string();
    }

    void ReplaceTokenIgnoreCase(std::string &text, const std::string &token, const std::string &value)
    {
        text = std::regex_replace(text, std::regex(token, std::regex::icase), value);
    }

    void ApplyManualTokens(std::string &line, const BundleDataFull &bundle)
    {
        ReplaceTokenIgnoreCase(line, "%K", bundle.bundleNo);
        ReplaceTokenIgnoreCase(line, "%G", std::to_string(bundle.tube));
        ReplaceTokenIgnoreCase(line, "%M", bundle.meltNo);
        ReplaceTokenIgnoreCase(line, "%S", bundle.lotNo);
        ReplaceTokenIgnoreCase(line, "%L", FormatFixed(bundle.totalLength, 2));
        ReplaceTokenIgnoreCase(line, "%W", std::to_string(bundle.weight));
        ReplaceTokenIgnoreCase(line, "%T", std::to_string(bundle.grossweight));
    }

    std::vector<std::uint8_t> ReadBinaryFile(const fs::path &filePath)
    {
        std::ifstream input(filePath, std::ios::binary);
        if (!input)
        {
            throw std::runtime_error("无法打开模板文件: " + filePath.string());
        }
        return std::vector<std::uint8_t>(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
    }

    std::string BinarySliceToString(const std::vector<std::uint8_t> &buffer, std::size_t offset, std::size_t length)
    {
        if (offset >= buffer.size() || length == 0)
        {
            return std::string();
        }

        const std::size_t actualLength = std::min(length, buffer.size() - offset);
        return std::string(reinterpret_cast<const char *>(buffer.data() + offset), actualLength);
    }

    void ReplaceFirst(std::vector<std::uint8_t> &buffer, const std::string &oldValue, const std::string &newValue)
    {
        const std::vector<std::uint8_t> oldBytes(oldValue.begin(), oldValue.end());
        const std::vector<std::uint8_t> newBytes(newValue.begin(), newValue.end());
        const auto it = std::search(buffer.begin(), buffer.end(), oldBytes.begin(), oldBytes.end());
        if (it == buffer.end())
        {
            return;
        }

        const auto offset = static_cast<std::size_t>(std::distance(buffer.begin(), it));
        std::vector<std::uint8_t> replaced;
        replaced.reserve(buffer.size() + newBytes.size() - oldBytes.size());
        replaced.insert(replaced.end(), buffer.begin(), buffer.begin() + static_cast<std::ptrdiff_t>(offset));
        replaced.insert(replaced.end(), newBytes.begin(), newBytes.end());
        replaced.insert(replaced.end(), it + static_cast<std::ptrdiff_t>(oldBytes.size()), buffer.end());
        buffer.swap(replaced);
    }

    std::string BuildQrText(const BundleDataFull &bundle)
    {
        std::ostringstream qr;
        qr << bundle.bundleNo << ' '
           << bundle.orderNo << ' '
           << bundle.sgText << ' '
           << FormatPadded(bundle.diameter, 6, 2) << '*' << FormatFixed(bundle.wallThickness, 2) << ' '
           << bundle.meltNo << ' '
           << bundle.lotNo << ' '
           << FormatPadded(bundle.weight, 4, 0) << ' '
           << FormatPadded(bundle.tube, 3, 0) << ' '
           << FormatPadded(bundle.lengthFrom, 6, 3) << '-' << FormatPadded(bundle.lengthTo, 6, 3) << ' '
           << FormatPadded(bundle.totalLength, 7, 3) << ' '
           << FormatDateIso(bundle.productDate);
        return qr.str();
    }

    fs::path FindTemplateDirectory()
    {
        const auto hasRequiredTemplates = [](const fs::path &dir)
        {
            return fs::exists(dir) && fs::is_directory(dir) &&
                   fs::exists(dir / "固定格式.prn") &&
                   fs::exists(dir / "自由格式.prn") &&
                   fs::exists(dir / "废管格式.prn");
        };

        fs::path current = fs::current_path();
        for (int depth = 0; depth < 10; ++depth)
        {
            const std::array<fs::path, 4> candidates = {
                current / "assets",
                current / "标签模板",
                current / "TagPrint" / "assets",
                current / "TagPrint" / "标签模板",
            };

            for (const auto &candidate : candidates)
            {
                if (hasRequiredTemplates(candidate))
                {
                    return candidate;
                }
            }

            if (!current.has_parent_path())
            {
                break;
            }
            current = current.parent_path();
        }
        throw std::runtime_error("未找到标签模板目录，已尝试 assets/标签模板 以及 TagPrint/assets/TagPrint/标签模板");
    }

    [[maybe_unused]] bool ConnectAndSend(const std::string &ip, int port, const std::vector<std::uint8_t> &payload)
    {
        if (ip.empty() || port <= 0)
        {
            spdlog::error("打印机配置无效: ip='{}', port={}", ip, port);
            return false;
        }

        const int sock = ::socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            spdlog::error("创建打印机 socket 失败: {}", std::strerror(errno));
            return false;
        }

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(static_cast<std::uint16_t>(port));
        if (::inet_pton(AF_INET, ip.c_str(), &addr.sin_addr) != 1)
        {
            spdlog::error("打印机 IP 非法: {}", ip);
            ::close(sock);
            return false;
        }

        if (::connect(sock, reinterpret_cast<const sockaddr *>(&addr), sizeof(addr)) != 0)
        {
            spdlog::error("连接打印机失败: {}:{} ({})", ip, port, std::strerror(errno));
            ::close(sock);
            return false;
        }

        std::size_t sent = 0;
        while (sent < payload.size())
        {
            const auto rc = ::send(sock,
                                   reinterpret_cast<const char *>(payload.data() + sent),
                                   payload.size() - sent,
                                   0);
            if (rc <= 0)
            {
                spdlog::error("发送打印数据失败: {}", std::strerror(errno));
                ::close(sock);
                return false;
            }
            sent += static_cast<std::size_t>(rc);
        }

        ::shutdown(sock, SHUT_WR);
        ::close(sock);
        return true;
    }

    [[maybe_unused]] std::vector<std::uint8_t> BuildWastePayload(const fs::path &templateDir, const BundleDataFull &bundle)
    {
        auto buffer = ReadBinaryFile(templateDir / "废管格式.prn");
        ReplaceFirst(buffer, "bundle_no", bundle.bundleNo);
        ReplaceFirst(buffer, "order_no", bundle.orderNo);
        ReplaceFirst(buffer, "std_text", bundle.stdText);
        ReplaceFirst(buffer, "sg_text", bundle.sgText);
        ReplaceFirst(buffer, "guige", FormatFixed(bundle.diameter, 2) + " mm × " + FormatFixed(bundle.wallThickness, 2) + " mm");
        ReplaceFirst(buffer, "melt_no", bundle.meltNo);
        ReplaceFirst(buffer, "lot_no", bundle.lotNo);
        ReplaceFirst(buffer, "weight", std::to_string(bundle.weight) + " kg");
        ReplaceFirst(buffer, "tube", std::to_string(bundle.tube));
        ReplaceFirst(buffer, "changdu", FormatFixed(bundle.lengthFrom, 2) + " - " + FormatFixed(bundle.lengthTo, 2) + " m");
        ReplaceFirst(buffer, "total_length", FormatFixed(bundle.totalLength, 2) + " m");
        ReplaceFirst(buffer, "product_time", FormatDateCn(bundle.productDate));
        ReplaceFirst(buffer, "banbie", ResolveBanciText(bundle.banci));
        ReplaceFirst(buffer, "tiaoxingma", bundle.bundleNo);
        ReplaceFirst(buffer, "bundle_type", bundle.bundleType);
        ReplaceFirst(buffer, "direction_code", bundle.directionCode);
        ReplaceFirst(buffer, "room_no", bundle.roomNo);
        ReplaceFirst(buffer, "0000C5201", "0002C5201");

        if (bundle.count >= 2 && bundle.count <= 4)
        {
            ReplaceFirst(buffer, "XS;I,0001", "XS;I,000" + std::to_string(bundle.count));
        }

        return buffer;
    }

    std::vector<std::uint8_t> BuildFixedPayload(const fs::path &templateDir, const BundleDataFull &bundle)
    {
        auto buffer = ReadBinaryFile(templateDir / "固定格式.prn");
        ReplaceFirst(buffer, "bundle_no", bundle.bundleNo);
        ReplaceFirst(buffer, "order_no", bundle.orderNo);
        ReplaceFirst(buffer, "std_text", bundle.stdText);
        ReplaceFirst(buffer, "sg_text", bundle.sgText);
        ReplaceFirst(buffer, "guige", FormatFixed(bundle.diameter, 2) + " mm × " + FormatFixed(bundle.wallThickness, 2) + " mm");
        ReplaceFirst(buffer, "melt_no", bundle.meltNo);
        ReplaceFirst(buffer, "lot_no", bundle.lotNo);
        ReplaceFirst(buffer, "weight", std::to_string(bundle.weight) + " kg");
        ReplaceFirst(buffer, "tube", std::to_string(bundle.tube));
        ReplaceFirst(buffer, "changdu", FormatFixed(bundle.lengthFrom, 2) + " - " + FormatFixed(bundle.lengthTo, 2) + " m");
        ReplaceFirst(buffer, "total_length", FormatFixed(bundle.totalLength, 2) + " m");
        ReplaceFirst(buffer, "product_time", FormatDateCn(bundle.productDate));
        ReplaceFirst(buffer, "banbie", ResolveBanciText(bundle.banci));
        ReplaceFirst(buffer, "tiaoxingma", bundle.bundleNo);
        ReplaceFirst(buffer, "kx_text", bundle.kxText);
        ReplaceFirst(buffer, "erweima|", BuildQrText(bundle));
        ReplaceFirst(buffer, "0000C5201", "0002C5201");

        if (bundle.count >= 2 && bundle.count <= 4)
        {
            ReplaceFirst(buffer, "XS;I,0001", "XS;I,000" + std::to_string(bundle.count));
        }

        return buffer;
    }

    std::vector<std::uint8_t> BuildFreePayload(const fs::path &templateDir, BundleDataFull bundle)
    {
        for (auto &line : bundle.labelReqManual)
        {
            ApplyManualTokens(line, bundle);
        }

        auto buffer = ReadBinaryFile(templateDir / "自由格式.prn");
        for (std::size_t i = 0; i < bundle.labelReqManual.size(); ++i)
        {
            ReplaceFirst(buffer, "line" + std::to_string(i + 1), bundle.labelReqManual[i]);
        }
        ReplaceFirst(buffer, "tiaoxingma", bundle.bundleNo + "(" + bundle.orderNo + ")");
        ReplaceFirst(buffer, "information", bundle.bundleNo + "(" + bundle.orderNo + ")");
        ReplaceFirst(buffer, "erweima|", BuildQrText(bundle));
        ReplaceFirst(buffer, "0000C5201", "0002C5201");

        if (bundle.count >= 2 && bundle.count <= 4)
        {
            ReplaceFirst(buffer, "XS;I,0001", "XS;I,000" + std::to_string(bundle.count));
        }

        return buffer;
    }

    bool PrintTag(TagPrintContext &ctx, const BundleDataFull &bundle)
    {
        const fs::path templateDir = FindTemplateDirectory();
        std::vector<std::uint8_t> payload;

        if (!bundle.bundleNo.empty() && bundle.bundleNo.front() == '9')
        {
            // payload = BuildWastePayload(templateDir, bundle);
            spdlog::info("检测到废管标签，暂不执行打印");
            return true;
        }
        else if (bundle.emType == 1)
        {
            payload = BuildFreePayload(templateDir, bundle);
        }
        else
        {
            payload = BuildFixedPayload(templateDir, bundle);
        }

        // return ConnectAndSend(ctx.printerIp, ctx.printerPort, payload);

        spdlog::info("打印数据大小: {} 字节", payload.size());
        constexpr std::size_t kTailBytes = 806;
        spdlog::info("打印数据预览(结尾{}字节):\n{}",
                     std::min(kTailBytes, payload.size()),
                     BinarySliceToString(payload, payload.size() > kTailBytes ? payload.size() - kTailBytes : 0,
                                         std::min(kTailBytes, payload.size())));

        spdlog::info("检测到打印请求，暂不执行实际发送");
        return true;
    }

    std::optional<BundleDataFull> LoadBundleData(TagPrintContext &ctx, const TagPrintEvent &tagPrint)
    {
        if (!ctx.pgConn || !ctx.pgConn->is_open())
        {
            spdlog::error("PostgreSQL 连接不可用，无法查询打印数据");
            return std::nullopt;
        }

        const std::string orderNo = Trim(tagPrint.order_no.c_str());
        const std::string itemNo = Trim(tagPrint.item_no.c_str());
        const std::string bundleNo = Trim(tagPrint.bundle_no.c_str());
        const int count = tagPrint.count > 0 ? tagPrint.count : 1;

        try
        {
            pqxx::work tx(*ctx.pgConn);

            const auto bundleRows = tx.exec(
                "SELECT roll_no, lot_no, melt_no, product_job_point, diameter, wall_thickness, tube, weight, gross_weight, "
                "length_to, length_from, total_length, pono_id_coupling, lot_no_thread, ban_ci, produce_time, mat_no, sg_text, "
                "mat_text, std_text, thread_type_sign, bundle_type, direction_code, room_no "
                "FROM api_bundle_data_t WHERE order_no = $1 AND bundle_no = $2 AND item_no = $3",
                pqxx::params{orderNo, bundleNo, itemNo});

            if (bundleRows.empty())
            {
                spdlog::warn("无法查询到管捆数据: order_no={}, bundle_no={}, item_no={}", orderNo, bundleNo, itemNo);
                tx.commit();
                return std::nullopt;
            }

            BundleDataFull bundle;
            bundle.orderNo = orderNo;
            bundle.itemNo = itemNo;
            bundle.bundleNo = bundleNo;
            bundle.count = count;

            const auto &bundleRow = bundleRows.front();
            bundle.rollNo = RowString(bundleRow, "roll_no");
            bundle.lotNo = RowString(bundleRow, "lot_no");
            bundle.meltNo = RowString(bundleRow, "melt_no");
            bundle.zoneNo = RowString(bundleRow, "product_job_point");
            bundle.diameter = RowNumber<double>(bundleRow, "diameter", 0.0);
            bundle.wallThickness = RowNumber<double>(bundleRow, "wall_thickness", 0.0);
            bundle.tube = RowNumber<int>(bundleRow, "tube", 0);
            bundle.weight = static_cast<int>(std::lround(RowNumber<double>(bundleRow, "weight", 0.0)));
            bundle.grossweight = RowNumber<int>(bundleRow, "gross_weight", 0);
            bundle.lengthTo = RowNumber<double>(bundleRow, "length_to", 0.0);
            bundle.lengthFrom = RowNumber<double>(bundleRow, "length_from", 0.0);
            bundle.totalLength = RowNumber<double>(bundleRow, "total_length", 0.0);
            bundle.ponoIDCoupling = RowString(bundleRow, "pono_id_coupling");
            bundle.lotNoThread = RowString(bundleRow, "lot_no_thread");
            bundle.banci = RowString(bundleRow, "ban_ci");
            bundle.productDate = RowString(bundleRow, "produce_time");
            bundle.matNo = RowString(bundleRow, "mat_no");
            bundle.sgText = ToUpperCopy(RowString(bundleRow, "sg_text"));
            bundle.matText = ToUpperCopy(RowString(bundleRow, "mat_text"));
            bundle.stdText = Trim(RowString(bundleRow, "std_text"));
            bundle.kxText = Trim(RowString(bundleRow, "thread_type_sign"));
            bundle.bundleType = Trim(RowString(bundleRow, "bundle_type"));
            bundle.directionCode = Trim(RowString(bundleRow, "direction_code"));
            bundle.roomNo = Trim(RowString(bundleRow, "room_no"));

            const auto orderRows = tx.exec(
                "SELECT label_type, label_req_1_manual, label_req_2_manual, label_req_3_manual, label_req_4_manual, "
                "label_req_5_manual, label_req_6_manual, label_req_7_manual, label_req_8_manual, label_length_type, label_weight_type "
                "FROM api_order_data_t WHERE order_no = $1 AND item_no = $2",
                pqxx::params{orderNo, itemNo});

            if (orderRows.empty())
            {
                spdlog::warn("无法查询到合同数据: order_no={}, item_no={}", orderNo, itemNo);
                tx.commit();
                return std::nullopt;
            }

            const auto &orderRow = orderRows.front();
            bundle.emType = RowNumber<int>(orderRow, "label_type", 0);
            bundle.labelReqManual[0] = Trim(RowString(orderRow, "label_req_1_manual"));
            bundle.labelReqManual[1] = Trim(RowString(orderRow, "label_req_2_manual"));
            bundle.labelReqManual[2] = Trim(RowString(orderRow, "label_req_3_manual"));
            bundle.labelReqManual[3] = Trim(RowString(orderRow, "label_req_4_manual"));
            bundle.labelReqManual[4] = Trim(RowString(orderRow, "label_req_5_manual"));
            bundle.labelReqManual[5] = Trim(RowString(orderRow, "label_req_6_manual"));
            bundle.labelReqManual[6] = Trim(RowString(orderRow, "label_req_7_manual"));
            bundle.labelReqManual[7] = Trim(RowString(orderRow, "label_req_8_manual"));
            bundle.labLengthType = RowNumber<int>(orderRow, "label_length_type", 0);
            bundle.labWeightType = RowNumber<int>(orderRow, "label_weight_type", 0);

            if (bundle.emType == 1)
            {
                if (bundle.labLengthType == 1)
                {
                    bundle.totalLength *= 3.280839;
                }
                if (bundle.labWeightType == 1)
                {
                    bundle.weight = static_cast<int>(std::lround(bundle.weight * 2.2046));
                }
            }

            tx.commit();
            return bundle;
        }
        catch (const std::exception &e)
        {
            spdlog::error("查询打印数据失败: order_no={}, item_no={}, bundle_no={}, error={}",
                          orderNo,
                          itemNo,
                          bundleNo,
                          e.what());
            return std::nullopt;
        }
    }

} // namespace

TagPrint::TagPrint(TagPrintContext &ctx) : ctx_(ctx)
{
}

void TagPrint::Run()
{
    spdlog::info("标签打印程序启动");

    try
    {
        unsigned int err = 0;
        subscribe(ctx_.gplatConn, "TAG_PRINT_EVENT", &err);
        subscribe(ctx_.gplatConn, "timer_500ms", &err);
    }
    catch (const std::exception &e)
    {
        spdlog::error("订阅gPlat事件失败: {}", e.what());
        throw; // 让Run函数的调用者决定如何处理订阅失败的情况
    }

    while (g_running)
    {
        unsigned int err = 0;
        char value[1024] = {0};
        std::string tagname;

        try
        {
            bool ret = waitpostdata(ctx_.gplatConn, tagname, value, 1024, -1, &err);
            if (!ret)
            {
                if (!g_running)
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

            if (tagname == "TAG_PRINT_EVENT")
            {
                TagPrintEvent tagPrint = read_value<TagPrintEvent>(value);

                const std::string orderNo = Trim(tagPrint.order_no.c_str());
                const std::string itemNo = Trim(tagPrint.item_no.c_str());
                const std::string bundleNo = Trim(tagPrint.bundle_no.c_str());
                const int count = tagPrint.count > 0 ? tagPrint.count : 1;

                if (orderNo.empty() || itemNo.empty() || bundleNo.empty())
                {
                    spdlog::warn("收到无效打印事件: order_no='{}', item_no='{}', bundle_no='{}', count={}",
                                 orderNo,
                                 itemNo,
                                 bundleNo,
                                 count);
                    continue;
                }

                spdlog::info("收到打印事件: order_no={}, item_no={}, bundle_no={}, count={}",
                             orderNo,
                             itemNo,
                             bundleNo,
                             count);

                auto bundle = LoadBundleData(ctx_, tagPrint);
                if (!bundle)
                {
                    continue;
                }

                try
                {
                    if (!PrintTag(ctx_, *bundle))
                    {
                        spdlog::error("打印失败: order_no={}, bundle_no={}, item_no={}",
                                      bundle->orderNo,
                                      bundle->bundleNo,
                                      bundle->itemNo);
                        std::this_thread::sleep_for(kPollInterval);
                        continue;
                    }

                    spdlog::info("打印完成: order_no={}, bundle_no={}, item_no={}, count={}",
                                 bundle->orderNo,
                                 bundle->bundleNo,
                                 bundle->itemNo,
                                 bundle->count);
                }
                catch (const std::exception &e)
                {
                    spdlog::error("打印流程异常: order_no={}, bundle_no={}, item_no={}, error={}",
                                  bundle->orderNo,
                                  bundle->bundleNo,
                                  bundle->itemNo,
                                  e.what());
                }
            }
        }
        catch (const std::exception &ex)
        {
            // 捕获异常后的处理逻辑，以打印异常内容到 cerr 为例
            std::cerr << "error: " << ex.what() << std::endl;
            throw; // 让Run函数的调用者决定如何处理waitpostdata异常
        }
    }

    spdlog::info("标签打印程序结束");
}

void TagPrint::test()
{

    spdlog::info("标签打印程序启动");

    TagPrintEvent tagPrint;
    tagPrint.order_no = "G2A2201255";
    tagPrint.item_no = "0";
    tagPrint.bundle_no = "1016284";
    tagPrint.count = 5;

    auto bundle = LoadBundleData(ctx_, tagPrint);
    if (!bundle)
    {
        spdlog::error("测试数据加载失败");
        return;
    }

    try
    {
        if (!PrintTag(ctx_, *bundle))
        {
            spdlog::error("打印失败: order_no={}, bundle_no={}, item_no={}",
                          bundle->orderNo,
                          bundle->bundleNo,
                          bundle->itemNo);
            std::this_thread::sleep_for(kPollInterval);
            return;
        }

        spdlog::info("打印完成: order_no={}, bundle_no={}, item_no={}, count={}",
                     bundle->orderNo,
                     bundle->bundleNo,
                     bundle->itemNo,
                     bundle->count);
    }
    catch (const std::exception &e)
    {
        spdlog::error("打印流程异常: order_no={}, bundle_no={}, item_no={}, error={}",
                      bundle->orderNo,
                      bundle->bundleNo,
                      bundle->itemNo,
                      e.what());
    }
}