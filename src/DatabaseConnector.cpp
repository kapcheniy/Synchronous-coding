#include "DatabaseConnector.h"

DatabaseConnector* DatabaseConnector::instance = nullptr;
std::mutex DatabaseConnector::mtx;

DatabaseConnector::DatabaseConnector() {
    try {
        driver = sql::mysql::get_mysql_driver_instance();
        con.reset(driver->connect("tcp://host.docker.internal:3306", "root", "Password"));
        con->setSchema("boards_db");
        std::cout << "Connected to MySQL server successfully!" << std::endl;
    }
    catch (sql::SQLException& e) {
        std::cerr << "Connection error: " << e.what() << std::endl;
        exit(1);
    }
}

DatabaseConnector* DatabaseConnector::getInstance() {
    if (!instance) {
        std::lock_guard<std::mutex> lock(mtx);
        if (!instance) {
            instance = new DatabaseConnector();
        }
    }
    return instance;
}

void DatabaseConnector::destroyInstance() {
    if (instance) {
        delete instance; 
        instance = nullptr;
    }
}

std::unique_ptr<sql::ResultSet> DatabaseConnector::executeQuery(const std::string& query) {
    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        return std::unique_ptr<sql::ResultSet>(stmt->executeQuery(query));
    }
    catch (sql::SQLException& e) {
        std::cerr << "Request execution error: " << e.what() << std::endl;
        return nullptr;
    }
}

bool DatabaseConnector::executeUpdate(const std::string& query) {
    try {
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        stmt->execute(query);
        return true;
    }
    catch (sql::SQLException& e) {
        std::cerr << "Update execution error: " << e.what() << std::endl;
        return false;
    }
}