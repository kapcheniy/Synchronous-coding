cmake_minimum_required(VERSION 3.10)
project(WebSocketServer)

# Настройка стандарта C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Поиск необходимых пакетов
find_package(Boost REQUIRED COMPONENTS system thread)
find_package(nlohmann_json REQUIRED)

# Поиск MySQL Connector
find_library(MYSQLCPPCONN_LIB mysqlcppconn)
find_path(MYSQLCPPCONN_INCLUDE mysql_driver.h PATH_SUFFIXES jdbc)

# Список исходных файлов
set(SOURCES
    src/main.cpp
    src/BoardManager.cpp
    src/DatabaseConnector.cpp
    src/MessageHandler.cpp
    src/SharedDocument.cpp
)

# Список заголовочных файлов
set(HEADERS
    src/BoardManager.h
    src/DatabaseConnector.h
    src/MessageHandler.h
    src/SharedDocument.h
)

# Создание исполняемого файла
add_executable(WebSocketServer ${SOURCES} ${HEADERS})

target_include_directories(WebSocketServer PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${MYSQLCPPCONN_INCLUDE}
)

target_link_libraries(WebSocketServer
    PRIVATE
        Boost::boost
        Boost::system
        Boost::thread
        nlohmann_json::nlohmann_json
        ${MYSQLCPPCONN_LIB}
        pthread
)

if(WIN32)
    target_compile_definitions(WebSocketServer PRIVATE _WIN32)
    target_link_libraries(WebSocketServer PRIVATE ws2_32)
endif()

if(UNIX AND NOT APPLE)
    target_compile_definitions(WebSocketServer PRIVATE __linux__)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

if(MSVC)
    target_compile_options(WebSocketServer PRIVATE /W4 /WX)
else()
    target_compile_options(WebSocketServer PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()