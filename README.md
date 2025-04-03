# Synchronous Coding

## Описание
Этот проект представляет собой сервер и веб-сайт, которые позволяют нескольким пользователям одновременно писать и компилировать код. Все изменения автоматически записываются в базу данных, что предотвращает потерю данных при отключении сервера.

## Основные возможности
- **Многопользовательский режим**: несколько пользователей могут одновременно работать с кодом.
- **Автоматическое сохранение**: все изменения фиксируются в базе данных.
- **Запуск в контейнере Docker**: позволяет избежать проблем, связанных с компиляцией на разных системах.
- **Использование WebSockets**: обеспечивает мгновенную синхронизацию между пользователями.
- **Поддержка MySQL**: для хранения данных пользователей и изменений кода.

## Используемые технологии
- **Сервер**: реализован с использованием `boost::beast` и `boost::asio`.
- **База данных**: MySQL через `cppconn`.
- **Фронтенд**: взаимодействует с сервером через WebSockets.
- **Docker**: контейнеризация всей системы для обеспечения портативности.

## Зависимости
Проект использует следующие библиотеки:
```cpp
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
```

### Ссылки для скачивания зависимостей
- [MySQL Server](https://dev.mysql.com/downloads/mysql/)
- [Boost](https://www.boost.org/users/download/)
- [nlohmann/json](https://github.com/nlohmann/json)

## Установка и запуск
1. Установите [Docker](https://www.docker.com/).
2. Склонируйте репозиторий:
   ```sh
   git clone https://github.com/yourusername/your-repository.git
   ```
3. Перейдите в каталог проекта:
   ```sh
   cd your-repository
   ```
4. Запустите сервер в контейнере:
   ```sh
   docker build -t my_app .
   docker rm -f my_websocket
   docker run -p 3001:3001 --add-host=host.docker.internal:host-gateway --name my_websocket -d my_app
   ```
5. Подключитесь к базе данных:
   ```sh
   \connect root@localhost:3306
   ```
6. Запустите веб-сервер:
   ```sh
   python -m http.server 3000
   ```

## Контакты
- Email: kllookin@gmail.com
- Telegram: [@KonstantinLukin](https://t.me/KonstantinLukin)

