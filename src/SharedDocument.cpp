#include "SharedDocument.h"

int SharedDocument::get_version(const std::string& board)
{
	std::lock_guard<std::mutex> lock(mtx);
	return board_versions[board].second;
}

void SharedDocument::version_to_null(const std::string& board)
{
	std::lock_guard<std::mutex> lock(mtx);
	if (board_versions.find(board) != board_versions.end()) {
		board_versions[board].second = 0;
	}
}

std::string SharedDocument::get_content(const std::string& board)
{
	std::lock_guard<std::mutex> lock(mtx);
	return board_versions[board].first;
}

void SharedDocument::add_client(websocket::stream<tcp::socket>* ws, const std::string& board, bool edit)
{
    std::lock_guard<std::mutex> lock(mtx);
    clients[ws] = board;

    boardManager.incrementAccessCount(board);

    if ((board_versions.find(board) == board_versions.end())&&(clients[ws]!="")) {
        board_versions[board] = { "", 0 };
        if (boardManager.boardExists(board) && (boardManager.getAccessCount(board) == 1)) {
            DatabaseConnector* db = DatabaseConnector::getInstance();

            int id;
            if (board == "board1") id = 1;
            else if (board == "board2") id = 2;

            std::cout << "id = " << id << std::endl;
            std::string query = "SELECT * FROM boards WHERE id = " + std::to_string(id);
            std::unique_ptr<sql::ResultSet> res = db->executeQuery(query);


            if (res->next()) {
                board_versions[board].first = res->getString("content");
                board_versions[board].second = std::stoi(res->getString("version"));
                std::cout << "board_versions[board].first = " << board_versions[board].first << std::endl;
                std::cout << "board_versions[board].second = " << board_versions[board].second << std::endl;
            }
            else {
                std::cout << "No data found for the given id." << std::endl;
            }

            json response;
            response["action"] = "update";
            response["content"] = board_versions[board].first;
            response["version"] = board_versions[board].second;
            std::string response_str = response.dump();
            ws->write(net::buffer(response_str));
        }
    }
    else if (edit) {
        if (boardManager.boardExists(board) && (boardManager.getAccessCount(board) == 1)) {
            DatabaseConnector* db = DatabaseConnector::getInstance();

            int id;
            if (board == "board1") id = 1;
            else if (board == "board2") id = 2;

            std::cout << "id = " << id << std::endl;
            std::string query = "SELECT * FROM boards WHERE id = " + std::to_string(id);
            std::unique_ptr<sql::ResultSet> res = db->executeQuery(query);


            if (res->next()) {
                board_versions[board].first = res->getString("content");
                board_versions[board].second = std::stoi(res->getString("version"));
                std::cout << "board_versions[board].first = " << board_versions[board].first << std::endl;
                std::cout << "board_versions[board].second = " << board_versions[board].second << std::endl;
            }
            else {
                std::cout << "No data found for the given id." << std::endl;
            }
        }

        json response;
        response["action"] = "update";
        response["content"] = board_versions[board].first;
        response["version"] = board_versions[board].second;
        std::string response_str = response.dump();
        ws->write(net::buffer(response_str));
    }
}

void SharedDocument::change_server(websocket::stream<tcp::socket>* ws, const std::string& board)
{
    std::lock_guard<std::mutex> lock(mtx);
    clients[ws] = board;
}

void SharedDocument::remove_client(websocket::stream<tcp::socket>* ws)
{
    const std::string board = clients[ws];
    boardManager.decrementAccessCount(board);
    std::cout << "decrementAccessCount: " << board << std::endl;
    if (!boardManager.getAccessCount(board)) {
        DatabaseConnector* db = DatabaseConnector::getInstance();

        int id;
        if (board == "board1") id = 1;
        else if (board == "board2") id = 2;
        std::string updateQuery = "UPDATE boards SET version = '" + std::to_string(board_versions[board].second) + "', content = '" + board_versions[board].first + "' WHERE id = " + std::to_string(id);

        if (db->executeUpdate(updateQuery)) {
            std::cout << "Data changed successfully!" << std::endl;
        }
    }
    std::lock_guard<std::mutex> lock(mtx);
    clients.erase(ws);
}

bool SharedDocument::is_client(websocket::stream<tcp::socket>* ws)
{
    return clients.find(ws) != clients.end();
}

void SharedDocument::update_content(const std::string& new_content, int client_version, const std::string& board, websocket::stream<tcp::socket>* client_ws)
{
    std::lock_guard<std::mutex> lock(mtx);
    auto& board_data = board_versions[board];
    std::string& content = board_data.first;
    int& version = board_data.second;

    if (client_version < version) {
        std::cout << "Client version is outdated. Sending sync response." << std::endl;

        json sync_response;
        sync_response["action"] = "update";
        sync_response["content"] = content;
        sync_response["version"] = version;

        if (client_ws && client_ws->is_open()) {
            try {
                client_ws->write(net::buffer(sync_response.dump()));
            }
            catch (const beast::system_error& e) {
                std::cerr << "Error writing sync response to client: " << e.what() << std::endl;
            }
        }
        return;
    }

    version++;
    content = new_content;

    json response;
    response["action"] = "update";
    response["content"] = content;
    response["version"] = version;

    std::string response_str = response.dump();
    for (const auto& client_pair : clients) {
        if (client_pair.second == board) {
            auto* client = client_pair.first;
            if (client && client->is_open()) {
                try {
                    client->write(net::buffer(response_str));
                }
                catch (const beast::system_error& e) {
                    std::cerr << "Error writing to client: " << e.what() << std::endl;
                }
            }
        }
    }
}

std::set<websocket::stream<tcp::socket>*> SharedDocument::get_clients_on_board(const std::string& board)
{
    std::lock_guard<std::mutex> lock(mtx);
    std::set<websocket::stream<tcp::socket>*> result;
    for (const auto& client_pair : clients) {
        if (client_pair.second == board) {
            result.insert(client_pair.first);
        }
    }
    return result;
}

bool SharedDocument::has_clients_on_board(const std::string& board)
{
    std::lock_guard<std::mutex> lock(mtx);
    for (const auto& client_pair : clients) {
        if (client_pair.second == board) {
            return true;
        }
    }
    return false;
}



