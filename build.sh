#!/bin/sh

# Сборка проекта
mkdir build && cd build
cmake ..
cmake --build . -- -j$(nproc)
cmake --install .

cd ..

# Запуск сервера
# ./file_server 8080

# Запуск клиента (в другом терминале)
# ./file_client 127.0.0.1 8080 my_download.bin

# Запуск нескольких клиентов
# for i in {1..3}; do
#    echo Run client $i...
#    ./file_client 127.0.0.1 4000 "download_$i.bin" &
# done

# wait

# Запуск тестов
# ./file_server_tests
# ./file_server_benchmarks
