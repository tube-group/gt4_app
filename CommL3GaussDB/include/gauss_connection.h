#pragma once

#include <libpq-fe.h>

#include <charconv>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace GaussDB
{

    namespace TypeOid
    {
        constexpr Oid Int4 = 23;
        constexpr Oid Int8 = 20;
        constexpr Oid Text = 25;
        constexpr Oid Float8 = 701;
        constexpr Oid Numeric = 1700;
    }

    class ResultSet
    {
    public:
        class Row
        {
        public:
            Row(const ResultSet &resultSet, int row)
                : m_resultSet(resultSet), m_row(row)
            {
                m_resultSet.validateRowIndex(m_row);
            }

            bool isNull(int col) const
            {
                return m_resultSet.isNull(m_row, col);
            }

            bool isNull(std::string_view colName) const
            {
                return m_resultSet.isNull(m_row, colName);
            }

            std::optional<std::string> getOptionalValue(int col) const
            {
                return m_resultSet.getOptionalValue(m_row, col);
            }

            std::optional<std::string> getOptionalValue(std::string_view colName) const
            {
                return m_resultSet.getOptionalValue(m_row, colName);
            }

            std::string getValue(int col) const
            {
                return m_resultSet.getValue(m_row, col);
            }

            std::string getValue(std::string_view colName) const
            {
                return m_resultSet.getValue(m_row, colName);
            }

            std::string getString(int col) const
            {
                return getValue(col);
            }

            std::string getString(std::string_view colName) const
            {
                return getValue(colName);
            }

            std::optional<std::int32_t> getOptionalInt32(int col) const
            {
                return m_resultSet.getOptionalInt32(m_row, col);
            }

            std::optional<std::int32_t> getOptionalInt32(std::string_view colName) const
            {
                return m_resultSet.getOptionalInt32(m_row, colName);
            }

            std::optional<std::int32_t> getOptionalInt(int col) const
            {
                return getOptionalInt32(col);
            }

            std::optional<std::int32_t> getOptionalInt(std::string_view colName) const
            {
                return getOptionalInt32(colName);
            }

            std::int32_t getInt32(int col) const
            {
                return m_resultSet.getInt32(m_row, col);
            }

            std::int32_t getInt32(std::string_view colName) const
            {
                return m_resultSet.getInt32(m_row, colName);
            }

            std::int32_t getInt(int col) const
            {
                return getInt32(col);
            }

            std::int32_t getInt(std::string_view colName) const
            {
                return getInt32(colName);
            }

            std::optional<std::int64_t> getOptionalInt64(int col) const
            {
                return m_resultSet.getOptionalInt64(m_row, col);
            }

            std::optional<std::int64_t> getOptionalInt64(std::string_view colName) const
            {
                return m_resultSet.getOptionalInt64(m_row, colName);
            }

            std::int64_t getInt64(int col) const
            {
                return m_resultSet.getInt64(m_row, col);
            }

            std::int64_t getInt64(std::string_view colName) const
            {
                return m_resultSet.getInt64(m_row, colName);
            }

            std::optional<double> getOptionalDouble(int col) const
            {
                return m_resultSet.getOptionalDouble(m_row, col);
            }

            std::optional<double> getOptionalDouble(std::string_view colName) const
            {
                return m_resultSet.getOptionalDouble(m_row, colName);
            }

            double getDouble(int col) const
            {
                return m_resultSet.getDouble(m_row, col);
            }

            double getDouble(std::string_view colName) const
            {
                return m_resultSet.getDouble(m_row, colName);
            }

        private:
            const ResultSet &m_resultSet;
            int m_row = 0;
        };

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

        bool isNull(int row, int col) const
        {
            validateIndex(row, col);
            return PQgetisnull(m_res.get(), row, col) != 0;
        }

        bool isNull(int row, std::string_view colName) const
        {
            return isNull(row, getColumnIndex(colName));
        }

        std::optional<std::string> getOptionalValue(int row, int col) const
        {
            validateIndex(row, col);
            if (PQgetisnull(m_res.get(), row, col) != 0)
            {
                return std::nullopt;
            }
            return std::string(PQgetvalue(m_res.get(), row, col));
        }

        std::optional<std::string> getOptionalValue(int row, std::string_view colName) const
        {
            return getOptionalValue(row, getColumnIndex(colName));
        }

        std::string getValue(int row, int col) const
        {
            return requireValue(getOptionalValue(row, col), "TEXT");
        }

        std::string getValue(int row, std::string_view colName) const
        {
            return requireValue(getOptionalValue(row, colName), "TEXT");
        }

        std::string getString(int row, int col) const
        {
            return getValue(row, col);
        }

        std::string getString(int row, std::string_view colName) const
        {
            return getValue(row, colName);
        }

        std::optional<std::int32_t> getOptionalInt32(int row, int col) const
        {
            return parseOptionalInteger<std::int32_t>(row, col, "INT4");
        }

        std::optional<std::int32_t> getOptionalInt32(int row, std::string_view colName) const
        {
            return parseOptionalInteger<std::int32_t>(row, getColumnIndex(colName), "INT4");
        }

        std::optional<std::int32_t> getOptionalInt(int row, int col) const
        {
            return getOptionalInt32(row, col);
        }

        std::optional<std::int32_t> getOptionalInt(int row, std::string_view colName) const
        {
            return getOptionalInt32(row, colName);
        }

        std::int32_t getInt32(int row, int col) const
        {
            return requireValue(getOptionalInt32(row, col), "INT4");
        }

        std::int32_t getInt32(int row, std::string_view colName) const
        {
            return requireValue(getOptionalInt32(row, colName), "INT4");
        }

        std::int32_t getInt(int row, int col) const
        {
            return getInt32(row, col);
        }

        std::int32_t getInt(int row, std::string_view colName) const
        {
            return getInt32(row, colName);
        }

        std::optional<std::int64_t> getOptionalInt64(int row, int col) const
        {
            return parseOptionalInteger<std::int64_t>(row, col, "INT8");
        }

        std::optional<std::int64_t> getOptionalInt64(int row, std::string_view colName) const
        {
            return parseOptionalInteger<std::int64_t>(row, getColumnIndex(colName), "INT8");
        }

        std::int64_t getInt64(int row, int col) const
        {
            return requireValue(getOptionalInt64(row, col), "INT8");
        }

        std::int64_t getInt64(int row, std::string_view colName) const
        {
            return requireValue(getOptionalInt64(row, colName), "INT8");
        }

        std::optional<double> getOptionalDouble(int row, int col) const
        {
            auto value = getOptionalValue(row, col);
            if (!value.has_value())
            {
                return std::nullopt;
            }

            return parseDouble(*value);
        }

        std::optional<double> getOptionalDouble(int row, std::string_view colName) const
        {
            auto value = getOptionalValue(row, colName);
            if (!value.has_value())
            {
                return std::nullopt;
            }

            return parseDouble(*value);
        }

        double getDouble(int row, int col) const
        {
            return requireValue(getOptionalDouble(row, col), "FLOAT8");
        }

        double getDouble(int row, std::string_view colName) const
        {
            return requireValue(getOptionalDouble(row, colName), "FLOAT8");
        }

        Row getRow(int row) const
        {
            return Row(*this, row);
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
        template <typename ValueType>
        ValueType requireValue(std::optional<ValueType> value, const char *typeName) const
        {
            if (!value.has_value())
            {
                throw std::runtime_error(std::string("结果集字段值为 NULL，无法读取为 ") + typeName);
            }
            return *value;
        }

        template <typename IntegerType>
        std::optional<IntegerType> parseOptionalInteger(int row, int col, const char *typeName) const
        {
            auto value = getOptionalValue(row, col);
            if (!value.has_value())
            {
                return std::nullopt;
            }

            IntegerType parsedValue{};
            const char *begin = value->data();
            const char *end = begin + value->size();
            auto [ptr, error] = std::from_chars(begin, end, parsedValue);
            if (error != std::errc() || ptr != end)
            {
                throw std::runtime_error(std::string("结果集字段值无法转换为 ") + typeName + ": " + *value);
            }

            return parsedValue;
        }

        double parseDouble(const std::string &value) const
        {
            errno = 0;
            char *end = nullptr;
            const double parsedValue = std::strtod(value.c_str(), &end);
            if (errno == ERANGE || end != value.c_str() + value.size())
            {
                throw std::runtime_error("结果集字段值无法转换为 FLOAT8: " + value);
            }

            return parsedValue;
        }

        int getColumnIndex(std::string_view colName) const
        {
            for (int col = 0; col < getColCount(); ++col)
            {
                const char *columnName = PQfname(m_res.get(), col);
                if (columnName != nullptr && std::string_view(columnName) == colName)
                {
                    return col;
                }
            }

            throw std::out_of_range(buildColumnNameError(colName));
        }

        std::string buildColumnNameError(std::string_view colName) const
        {
            std::string message = "结果集不存在列名: ";
            message.append(colName.data(), colName.size());

            const int colCount = getColCount();
            if (colCount > 0)
            {
                message += "，可用列名: ";
                for (int col = 0; col < colCount; ++col)
                {
                    if (col > 0)
                    {
                        message += ", ";
                    }

                    const char *columnName = PQfname(m_res.get(), col);
                    if (columnName != nullptr)
                    {
                        message += columnName;
                    }
                }
            }

            return message;
        }

        void validateRowIndex(int row) const
        {
            if (row < 0 || row >= getRowCount())
            {
                throw std::out_of_range("结果集行索引越界");
            }
        }

        void validateColIndex(int col) const
        {
            if (col < 0 || col >= getColCount())
            {
                throw std::out_of_range("结果集列索引越界");
            }
        }

        void validateIndex(int row, int col) const
        {
            validateRowIndex(row);
            validateColIndex(col);
        }

        std::unique_ptr<PGresult, decltype(&PQclear)> m_res;
    };

    class Connection
    {
    public:
        struct ConnectParam
        {
            std::string keyword;
            std::string value;
        };

        struct Param
        {
            std::optional<std::string> value;
            Oid type = 0;
            int format = 0;

            Param() = default;

            Param(std::nullptr_t)
                : value(std::nullopt)
            {
            }

            Param(std::string text, Oid typeOid = 0, int valueFormat = 0)
                : value(std::move(text)), type(typeOid), format(valueFormat)
            {
            }

            Param(const char *text)
                : value(text != nullptr ? std::optional<std::string>(text) : std::nullopt)
            {
            }

            static Param null(Oid typeOid = 0)
            {
                Param param;
                param.type = typeOid;
                return param;
            }

            static Param text(std::string text)
            {
                return Param(std::move(text), TypeOid::Text);
            }

            static Param int4(std::int32_t value)
            {
                return Param(std::to_string(value), TypeOid::Int4);
            }

            static Param int8(std::int64_t value)
            {
                return Param(std::to_string(value), TypeOid::Int8);
            }

            static Param float8(double value)
            {
                return Param(std::to_string(value), TypeOid::Float8);
            }

            static Param numeric(std::string value)
            {
                return Param(std::move(value), TypeOid::Numeric);
            }
        };

        explicit Connection(const std::string &connInfo)
            : m_conn(nullptr, &PQfinish)
        {
            openWithConninfo(connInfo);
        }

        explicit Connection(const std::vector<ConnectParam> &connParams)
            : m_conn(nullptr, &PQfinish)
        {
            openWithParams(connParams);
        }

        ResultSet execute(const std::string &sql)
        {
            return consumeResult(PQexec(m_conn.get(), sql.c_str()), sql, "SQL 执行失败");
        }

        ResultSet executeParams(const std::string &sql, const std::vector<Param> &params)
        {
            std::vector<const char *> paramValues;
            std::vector<int> paramLengths;
            std::vector<int> paramFormats;
            std::vector<Oid> paramTypes;

            paramValues.reserve(params.size());
            paramLengths.reserve(params.size());
            paramFormats.reserve(params.size());
            paramTypes.reserve(params.size());

            for (const auto &param : params)
            {
                paramValues.push_back(param.value.has_value() ? param.value->c_str() : nullptr);
                paramLengths.push_back(param.value.has_value() ? static_cast<int>(param.value->size()) : 0);
                paramFormats.push_back(param.format);
                paramTypes.push_back(param.type);
            }

            return consumeResult(
                PQexecParams(
                    m_conn.get(),
                    sql.c_str(),
                    static_cast<int>(params.size()),
                    paramTypes.empty() ? nullptr : paramTypes.data(),
                    paramValues.empty() ? nullptr : paramValues.data(),
                    paramLengths.empty() ? nullptr : paramLengths.data(),
                    paramFormats.empty() ? nullptr : paramFormats.data(),
                    0),
                sql,
                "参数化 SQL 执行失败");
        }

        ResultSet executeParams(const std::string &sql, const std::vector<std::string> &params) = delete;

    private:
        void openWithConninfo(const std::string &connInfo)
        {
            PGconn *conn = PQconnectdb(connInfo.c_str());
            m_conn.reset(conn);
            ensureConnected();
        }

        void openWithParams(const std::vector<ConnectParam> &connParams)
        {
            std::vector<const char *> keywords;
            std::vector<const char *> values;
            keywords.reserve(connParams.size() + 1);
            values.reserve(connParams.size() + 1);

            for (const auto &param : connParams)
            {
                keywords.push_back(param.keyword.c_str());
                values.push_back(param.value.c_str());
            }

            keywords.push_back(nullptr);
            values.push_back(nullptr);

            PGconn *conn = PQconnectdbParams(keywords.data(), values.data(), 0);
            m_conn.reset(conn);
            ensureConnected();
        }

        void ensureConnected()
        {
            if (m_conn == nullptr || PQstatus(m_conn.get()) != CONNECTION_OK)
            {
                std::string errorMsg = m_conn != nullptr
                                           ? std::string(PQerrorMessage(m_conn.get()))
                                           : std::string("数据库连接返回空连接");
                throw std::runtime_error("高斯数据库连接失败: " + errorMsg);
            }
        }

        ResultSet consumeResult(PGresult *rawResult, const std::string &sql, const char *errorPrefix)
        {
            if (rawResult == nullptr)
            {
                throw std::runtime_error(std::string(errorPrefix) + ": " + std::string(PQerrorMessage(m_conn.get())) + "\nSQL: " + sql);
            }

            std::unique_ptr<PGresult, decltype(&PQclear)> result(rawResult, &PQclear);
            ExecStatusType status = PQresultStatus(result.get());
            if (status != PGRES_COMMAND_OK && status != PGRES_TUPLES_OK)
            {
                throw std::runtime_error(buildResultError(errorPrefix, result.get(), sql));
            }

            return ResultSet(result.release());
        }

        std::string buildResultError(const char *errorPrefix, PGresult *result, const std::string &sql) const
        {
            std::string message = std::string(errorPrefix) + ": ";

            const char *resultMessage = PQresultErrorMessage(result);
            if (resultMessage != nullptr && resultMessage[0] != '\0')
            {
                message += resultMessage;
            }
            else
            {
                message += PQerrorMessage(m_conn.get());
            }

            const char *sqlState = PQresultErrorField(result, PG_DIAG_SQLSTATE);
            if (sqlState != nullptr && sqlState[0] != '\0')
            {
                message += "SQLSTATE: ";
                message += sqlState;
                message += '\n';
            }

            message += "SQL: ";
            message += sql;
            return message;
        }

        std::unique_ptr<PGconn, decltype(&PQfinish)> m_conn;
    };

} // namespace GaussDB