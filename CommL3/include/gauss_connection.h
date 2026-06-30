#pragma once

#include <libpq-fe.h>

#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace GaussDB
{

    class ResultSet
    {
    public:
        explicit ResultSet(PGresult *res)
            : m_res(res, &PQclear)
        {
            if (m_res == nullptr)
            {
                throw std::runtime_error("高斯数据库返回空结果集");
            }
        }

        int getRowCount() const
        {
            return PQntuples(m_res.get());
        }

        int getColCount() const
        {
            return PQnfields(m_res.get());
        }

        std::string getValue(int row, int col) const
        {
            if (row < 0 || row >= getRowCount() || col < 0 || col >= getColCount())
            {
                throw std::out_of_range("结果集索引越界");
            }
            return std::string(PQgetvalue(m_res.get(), row, col));
        }

        bool isCmdSuccess() const
        {
            ExecStatusType status = PQresultStatus(m_res.get());
            return status == PGRES_COMMAND_OK || status == PGRES_TUPLES_OK;
        }

        std::string cmdTuples() const
        {
            const char *value = PQcmdTuples(m_res.get());
            return value != nullptr ? std::string(value) : std::string();
        }

    private:
        std::unique_ptr<PGresult, decltype(&PQclear)> m_res;
    };

    class Connection
    {
    public:
        explicit Connection(const std::string &connInfo)
            : m_conn(nullptr, &PQfinish)
        {
            PGconn *conn = PQconnectdb(connInfo.c_str());
            m_conn.reset(conn);

            if (m_conn == nullptr || PQstatus(m_conn.get()) != CONNECTION_OK)
            {
                std::string errorMsg = m_conn != nullptr
                                           ? std::string(PQerrorMessage(m_conn.get()))
                                           : std::string("PQconnectdb 返回空连接");
                throw std::runtime_error("高斯数据库连接失败: " + errorMsg);
            }
        }

        ResultSet execute(const std::string &sql)
        {
            PGresult *res = PQexec(m_conn.get(), sql.c_str());
            if (res == nullptr)
            {
                throw std::runtime_error("SQL 执行失败: " + std::string(PQerrorMessage(m_conn.get())) + "\nSQL: " + sql);
            }

            // 1. 先验证状态
            ExecStatusType status = PQresultStatus(res);
            if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK)
            {
                // 2. 失败则立即手动释放，然后抛异常
                PQclear(res);
                throw std::runtime_error("SQL 执行失败: " + std::string(PQerrorMessage(m_conn.get())) + "\nSQL: " + sql);
            }

            // 3. 只有成功的结果才交给 ResultSet 托管
            return ResultSet(res);
        }

        ResultSet executeParams(const std::string &sql, const std::vector<std::string> &params)
        {
            std::vector<const char *> paramValues;
            paramValues.reserve(params.size());
            for (const auto &param : params)
            {
                paramValues.push_back(param.c_str());
            }

            PGresult *res = PQexecParams(
                m_conn.get(),
                sql.c_str(),
                static_cast<int>(params.size()),
                nullptr,
                paramValues.data(),
                nullptr,
                nullptr,
                0);

            // 1. 先校验是否返回空指针
            if (res == nullptr)
            {
                throw std::runtime_error("参数化 SQL 执行返回空指针: " + std::string(PQerrorMessage(m_conn.get())) + "\nSQL: " + sql);
            }

            // 2. 校验执行状态是否成功
            ExecStatusType status = PQresultStatus(res);
            if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK)
            {
                // 失败则立即手动释放该 PGresult 内存，防止内存泄漏
                PQclear(res);
                throw std::runtime_error("参数化 SQL 执行失败: " + std::string(PQerrorMessage(m_conn.get())) + "\nSQL: " + sql);
            }

            // 3. 只有状态成功的结果，才放心地交给 ResultSet 托管并返回
            return ResultSet(res);
        }

    private:
        std::unique_ptr<PGconn, decltype(&PQfinish)> m_conn;
    };

} // namespace GaussDB