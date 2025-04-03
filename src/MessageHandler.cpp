#include "MessageHandler.h"
#include <fstream>
int MessageHandler::handle(const std::string& messange)
{
    json data = json::parse(messange);
    std::string action = data["action"];

    if (action == "message") {
        handle_message(data);
    }
    else if (action == "changes") {
        handle_changes(data);
    }
    else if (action == "connection") {
        handle_connection(data);
    }
    else if (action == "compile") {
        handle_compile(data);
    }
    else {
        return 1;
    }
    return 0;
}

std::string MessageHandler::create_response(bool success, const std::string& user, const std::string& board)
{
        json response;
        response["status"] = success ? "success" : "error";
        response["user"] = user;
        response["board"] = board;
        return response.dump();
}

void MessageHandler::handle_message(const json& data)
{
    std::cout << "JSON message" << std::endl;
    if (!data.contains("user") ||
        !data.contains("board") ||
        !data["auth"].contains("password")) {
        ws.write(net::buffer(create_response(false, "Invalid message format", "")));
        return;
    }

    std::string user = data["user"];
    std::string board = data["board"];
    std::string password = data["auth"]["password"];

    if (boardManager.validatePassword(board, password)) {
        std::cout << "Valid message from: " << user << std::endl;
        ws.write(net::buffer(create_response(true, user, board)));
    }
    else {
        ws.write(net::buffer(create_response(false, user, board)));
    }
}

void MessageHandler::handle_changes(const json& data)
{
    std::cout << "JSON changes" << std::endl;
    if (!data.contains("content")) {
        ws.write(net::buffer(create_response(false, "Missing 'content' field", "")));
        return;
    }
    std::string new_content = data["content"];
    std::string board = data["board"];
    int client_version = data["version"];
    doc.update_content(new_content, client_version, board, &ws);
}

void MessageHandler::handle_connection(const json& data)
{
    std::string board = data["board"];
    std::string content = "";
    std::cout << "JSON connection to " << board << std::endl;
    if (!doc.has_clients_on_board(board)) {
        doc.version_to_null(board);
    }
    doc.add_client(&ws, board, true);
}

void MessageHandler::handle_compile(const json& data)
{
#ifdef _WIN32
    std::string code = data["content"];

    std::string filename = "temp_code.cpp";
    std::ofstream file(filename);

    if (!file) {
        std::cout << "Mistake: failed to create a file temp_code.cpp" << std::endl;
        ws.write(net::buffer("Mistake: failed to create a file temp_code.cpp"));
        return;
    }

    file << code;
    file.close();

    std::string compile_cmd = "g++ " + filename + " -o temp_exec 2> error.log";
    int compile_result = system(compile_cmd.c_str());

    if (compile_result != 0) {
        std::ifstream error_file("error.log");
        std::stringstream buffer;
        buffer << error_file.rdbuf();
        error_file.close();
        std::cout << "Compilation error" << std::endl;
        ws.write(net::buffer("Compilation error:\n" + buffer.str()));
        return;
    }

    std::string run_cmd = "temp_exec.exe > output.log";
    int run_result = system(run_cmd.c_str());

    if (run_result != 0) {
        std::cout << "ws.write(net::buffer(Code execution error : run_result));" << std::endl;
        ws.write(net::buffer("Code execution error: run_result"));
        return;
    }

    std::ifstream output_file("output.log");
    std::stringstream output_buffer;
    output_buffer << output_file.rdbuf();
    output_file.close();

    std::string result = output_buffer.str();
    std::cout << "Result compile: " << result << std::endl;

    json response;
    response["action"] = "compile_result";
    response["content"] = result;
    std::string response_str = response.dump();
    ws.write(net::buffer(response_str));
    std::cout << " ws.write(net::buffer(response_str));" << std::endl;
#elif __linux__

    std::string code = data["content"];
    json response;
    response["action"] = "compile_result";

    std::regex system_regex(R"(system\s*\()");
    if (std::regex_search(code, system_regex)) {
        response["content"] = "Error: using the system function is prohibited";
        std::string res_str = response.dump();
        ws.write(net::buffer(res_str));
        std::cout << "Error: using the system function is prohibited" << std::endl;
        return;
    }

    std::string filename = "temp_code.cpp";
    std::ofstream file(filename);

    if (!file) {
        std::cout << "Error: failed to create a file" << std::endl;
            return;
    }

    file << code;
    file.close();

    pid_t compile_pid = fork();
    if (compile_pid == 0) {

        int error_fd = open("error.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        dup2(error_fd, STDERR_FILENO);
        close(error_fd);

        execlp("g++", "g++", filename.c_str(), "-o", "temp_exec", nullptr);
        exit(1);
    }
    else if (compile_pid > 0) {

        int compile_status;
        waitpid(compile_pid, &compile_status, 0);

        if (WIFEXITED(compile_status) && WEXITSTATUS(compile_status) != 0) {

            std::ifstream error_file("error.log");
            std::stringstream buffer;
            buffer << error_file.rdbuf();
            error_file.close();
            response["content"] = "Compilation error:\n" + buffer.str();
            std::string res_str = response.dump();
            ws.write(net::buffer(res_str));
            std::cout << "Compilation error:\n" + buffer.str() << std::endl;
            return;
        }
    }
    else {
        std::cout << "Error: failed to create a compilation process" << std::endl;
        return;
    }


    pid_t run_pid = fork();
    if (run_pid == 0) {

        int output_fd = open("output.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        dup2(output_fd, STDOUT_FILENO);
        dup2(output_fd, STDERR_FILENO);
        close(output_fd);

        execlp("./temp_exec", "./temp_exec", nullptr);
        exit(1);
    }
    else if (run_pid > 0) {

        int run_status;
        waitpid(run_pid, &run_status, 0);

        if (WIFEXITED(run_status) && WEXITSTATUS(run_status) != 0) {
            std::cout << "Code execution error" << std::endl;
            return;
        }
    }
    else {
        std::cout << "Error: couldn't create a process to execute" << std::endl;
        return;
    }


    std::ifstream output_file("output.log");
    std::stringstream output_buffer;
    output_buffer << output_file.rdbuf();
    output_file.close();

    std::string result = output_buffer.str();
    std::cout << "Result compile: " << result << std::endl;

    response["content"] = result;
    std::string response_str = response.dump();
    ws.write(net::buffer(response_str));

#else
    std::cout << "The OS does not hold up" << std::endl;
    return 69;
#endif
}

