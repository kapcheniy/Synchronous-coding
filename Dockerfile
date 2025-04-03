FROM ubuntu:22.04


RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libboost-all-dev \
    libmysqlcppconn-dev \
    nlohmann-json3-dev \
    && rm -rf /var/lib/apt/lists/*


WORKDIR /app


COPY CMakeLists.txt .
COPY ./src /app/src


RUN echo "Proverka src:" && ls -la src/


RUN mkdir -p build && cd build && \
    cmake .. && make

EXPOSE 3001
CMD ["./build/WebSocketServer"]