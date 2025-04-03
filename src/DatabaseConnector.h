#pragma once
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <mutex> 
class DatabaseConnector {
private:
    static DatabaseConnector* instance;
    sql::mysql::MySQL_Driver* driver;
    std::unique_ptr<sql::Connection> con;
    static std::mutex mtx;

    DatabaseConnector();

public:
    DatabaseConnector(const DatabaseConnector&) = delete;
    DatabaseConnector& operator=(const DatabaseConnector&) = delete;
    DatabaseConnector(DatabaseConnector&&) = delete;
    DatabaseConnector& operator=(DatabaseConnector&&) = delete;

    static DatabaseConnector* getInstance();

    std::unique_ptr<sql::ResultSet> executeQuery(const std::string& query);

    bool executeUpdate(const std::string& query);

    static void destroyInstance();
};
