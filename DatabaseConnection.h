#ifndef DATABASE_CONNECTION_H
#define DATABASE_CONNECTION_H


#include <mariadb/conncpp.hpp>
#include <memory>
#include <string>
#include <vector>
#include <wx/wx.h>
#include <wx/mstream.h>

class DatabaseConnection {
public:
    DatabaseConnection(const std::string& host, const std::string& database);

    // 
    void checkAndReconnect();

    // 
    std::unique_ptr<sql::ResultSet> executeQuery(const std::string& sqlQuery);

private:
    std::unique_ptr<sql::Connection> conn;

    std::string host, database;

    // 
    void reconnect();
};

#endif // DATABASE_CONNECTION_H
