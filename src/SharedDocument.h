#pragma once
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/exception.h>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>
#include <set>
#include <mutex>
#include <nlohmann/json.hpp>     
#include <regex>     
#include <sys/wait.h> 
#include <fcntl.h>  
#include "BoardManager.h"
#include "DatabaseConnector.h"

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;

class SharedDocument {
private:
    std::map<std::string, std::pair<std::string, int>> board_versions; // board -> {content, version}
    std::map<websocket::stream<tcp::socket>*, std::string> clients; // socket -> board
    std::mutex mtx;
    BoardManager boardManager;

public:

    bool clientOnBoard(websocket::stream<tcp::socket>* ws);

    int get_version(const std::string& board);

    void version_to_null(const std::string& board);

    std::string get_content(const std::string& board);

    void add_client(websocket::stream<tcp::socket>* ws, const std::string& board, bool edit = false);

    void change_server(websocket::stream<tcp::socket>* ws, const std::string& board);

    void remove_client(websocket::stream<tcp::socket>* ws);

    bool is_client(websocket::stream<tcp::socket>* ws);

    void update_content(const std::string& new_content, int client_version, const std::string& board, websocket::stream<tcp::socket>* client_ws = nullptr);

    std::set<websocket::stream<tcp::socket>*> get_clients_on_board(const std::string& board);

    bool has_clients_on_board(const std::string& board);
};

