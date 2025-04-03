#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/statement.h>
#include <cppconn/resultset.h>
#include <cppconn/exception.h>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <iostream>
#include <string>
#include <set>
#include <mutex>
#include <memory>
#include <nlohmann/json.hpp>
#include <fstream>
#include <thread>    
#include <regex>     
#include <sys/wait.h> 
#include <fcntl.h>   
#include "DatabaseConnector.h"
#include "SharedDocument.h"
#include "MessageHandler.h"

namespace beast = boost::beast;
namespace websocket = beast::websocket;
namespace net = boost::asio;
using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;


void handle_session(tcp::socket socket, SharedDocument& doc) {
    websocket::stream<tcp::socket>* ws_ptr = nullptr;

    try {
        websocket::stream<tcp::socket> ws{ std::move(socket) };
        ws_ptr = &ws;

        ws.accept();

        doc.add_client(ws_ptr, "");

        beast::flat_buffer buffer;
        while (ws.is_open()) {
            ws.read(buffer);
            std::string msg = beast::buffers_to_string(buffer.data());

            MessageHandler messageHandler(ws, doc);
            messageHandler.handle(msg);

            buffer.consume(buffer.size());
        }
    }
    catch (const beast::system_error& se) {
        if (se.code() != websocket::error::closed) {
            std::cerr << "WebSocket Error: " << se.what() << std::endl;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    if (ws_ptr) {
        doc.remove_client(ws_ptr);
    }
}

int main() {
    try {
        net::io_context ioc;
        tcp::acceptor acceptor{ ioc, {tcp::v4(), 3001} };
        SharedDocument document;

        std::cout << "Server started on port 3001" << std::endl;

        while (true) {
            tcp::socket socket{ ioc };
            acceptor.accept(socket);
            std::thread{ handle_session, std::move(socket), std::ref(document) }.detach();
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal Error: " << e.what() << std::endl;
        return 1;
    }
}