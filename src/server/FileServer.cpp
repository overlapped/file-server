#include "server/FileServer.h"
#include "common/Constants.h"
#include "server/ConnectionManager.h"
#include "server/DynamicFileGenerator.h"
#include "server/NetworkUtils.h"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

FileServer::FileServer(uint16_t port) : server_fd_(-1), port_(port) {}

FileServer::~FileServer() {
    stop();
}

bool FileServer::start() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        std::cerr << "Socket creation failed: " << NetworkUtils::getLastError() << std::endl;
        return false;
    }

    NetworkUtils::setReuseAddress(server_fd_);
    NetworkUtils::setSocketOptions(server_fd_);

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (bind(server_fd_, reinterpret_cast<sockaddr *>(&address), sizeof(address)) < 0) {
        std::cerr << "Bind failed: " << NetworkUtils::getLastError() << std::endl;
        close(server_fd_);
        return false;
    }

    if (listen(server_fd_, constants::BACKLOG_SIZE) < 0) {
        std::cerr << "Listen failed: " << NetworkUtils::getLastError() << std::endl;
        close(server_fd_);
        return false;
    }

    running_.store(true);
    main_thread_ = std::thread(&FileServer::run, this);
    stats_thread_ = std::thread(&FileServer::statsMonitor, this);

    std::cout << "Server started on port " << port_ << std::endl;
    return true;
}

void FileServer::stop() {
    running_.store(false);

    if (server_fd_ >= 0) {
        shutdown(server_fd_, SHUT_RDWR);
        close(server_fd_);
        server_fd_ = -1;
    }

    if (main_thread_.joinable()) {
        main_thread_.join();
    }

    if (stats_thread_.joinable()) {
        stats_thread_.join();
    }

    for (auto &thread : worker_threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    worker_threads_.clear();
}

void FileServer::run() {
    while (running_.load()) {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);

        int client_socket = accept(server_fd_,
                                   reinterpret_cast<sockaddr *>(&client_addr),
                                   &client_len);

        if (client_socket < 0) {
            if (running_.load()) {
                std::cerr << "Accept failed: " << NetworkUtils::getLastError() << std::endl;
            }
            continue;
        }

        if (!ConnectionManager::getInstance().canAcceptConnection()) {
            std::cout << "Maximum connections reached. Rejecting client." << std::endl;
            close(client_socket);
            continue;
        }

        NetworkUtils::setSocketOptions(client_socket);

        worker_threads_.emplace_back(&FileServer::handleClient, this, client_socket);

        if (worker_threads_.size() > 1000) {
            cleanupThreads();
        }
    }
}

void FileServer::handleClient(int client_socket) {
    auto &connection_manager = ConnectionManager::getInstance();
    uint64_t client_id = connection_manager.assignClientId();
    connection_manager.connectionEstablished(client_id);

    DynamicFileGenerator generator;
    bool success = generator.generateAndSendFile(client_socket, client_id);

    close(client_socket);
    connection_manager.connectionCompleted(client_id);

    if (!success) {
        std::cerr << "File transfer failed for client " << client_id << std::endl;
    }
}

void FileServer::cleanupThreads() {
    worker_threads_.erase(
        std::remove_if(worker_threads_.begin(), worker_threads_.end(),
                       [](std::thread &t) {
                           if (t.joinable()) {
                               // TODO: в продакшн коде возможно надо изменить.
                               t.detach();
                               return true;
                           }
                           return false;
                       }),
        worker_threads_.end());
}

void FileServer::statsMonitor() {
    while (running_.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        ConnectionManager::getInstance().printStats();
    }
}
