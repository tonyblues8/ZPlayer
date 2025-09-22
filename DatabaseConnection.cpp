// DatabaseConnection.cpp
#include "DatabaseConnection.h"
DatabaseConnection::DatabaseConnection(const std::string& host, const std::string& database)
    : host(host), database(database)
{
    reconnect();  // 初始化连接
}

void DatabaseConnection::checkAndReconnect()
{
    try {
        // 如果连接无效，重新建立连接
        if (!conn || !conn->isValid()) {
            wxLogError("数据库连接已断开，正在重新连接...");
            reconnect();
        }
    } catch (sql::SQLException& e) {
        wxString message = wxString::Format(wxString::FromUTF8("数据库连接失败: %s"), e.what());
        //wxMessageBox(message, "错误", wxOK | wxICON_ERROR);
    }
}

std::unique_ptr<sql::ResultSet> DatabaseConnection::executeQuery(const std::string& sqlQuery)
{
    checkAndReconnect();  // 每次查询前先检查连接

    std::vector<std::map<std::string, std::string>> results;

    try {
        std::unique_ptr<sql::PreparedStatement> pstmt;
        pstmt.reset(conn->prepareStatement(sqlQuery));


        std::unique_ptr<sql::ResultSet> res(pstmt->executeQuery());

        // 获取列数
        sql::ResultSetMetaData* meta = res->getMetaData();
        int columnCount = meta->getColumnCount();

        // 遍历查询结果
        while (res->next()) {
            std::map<std::string, std::string> row;

            // 获取每一列的名称和值
            for (int i = 1; i <= columnCount; ++i) {
                std::string columnName = meta->getColumnName(i);  // 使用 operator const char*() 转换
                std::string columnValue = res->getString(i);  // 使用 operator const char*() 转换
                row[columnName] = columnValue;  // 存入map中
            }

            results.push_back(row);  // 将每一行的map存入results
        }
    } catch (sql::SQLException& e) {
        wxString message = wxString::Format(wxString::FromUTF8("查询执行失败: %s"), e.what());
        wxMessageBox(message, "错误", wxOK | wxICON_ERROR);
    }

    return results;  // 返回存储查询结果的vector
}

void DatabaseConnection::reconnect()
{
            try {
                sql::Driver* driver = sql::mariadb::get_driver_instance();
                sql::SQLString user("root");
                sql::SQLString pwd("xiongqian");
                conn = std::unique_ptr<sql::Connection>(driver->connect(host,  user, pwd));

            } catch (sql::SQLException& e) {
                wxString message = wxString::Format(wxString::FromUTF8("数据库连接失败: %s"), e.what());
                //wxMessageBox(message, "错误", wxOK | wxICON_ERROR);
            }
}

