#include "client/FileClient.h"
#include "common/Constants.h"
#include "common/Protocol.h"
#include "server/NetworkUtils.h"
#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <vector>

FileClient::FileClient(const ClientConfig &config)
    : config_(config), socket_fd_(-1), connected_(false) {}

FileClient::~FileClient() {
    disconnect();
}

bool FileClient::connectToServer() {
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd_ < 0) {
        std::cerr << "Socket creation failed: " << NetworkUtils::getLastError() << std::endl;
        return false;
    }

    NetworkUtils::setSocketOptions(socket_fd_);

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(config_.server_port);

    if (inet_pton(AF_INET, config_.server_host.c_str(), &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address: " << config_.server_host << std::endl;
        return false;
    }

    if (connect(socket_fd_, reinterpret_cast<sockaddr *>(&serv_addr), sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failed: " << NetworkUtils::getLastError() << std::endl;
        return false;
    }

    connected_ = true;
    std::cout << "Connected to server " << config_.server_host
              << ":" << config_.server_port << std::endl;
    return true;
}

void FileClient::disconnect() {
    if (socket_fd_ >= 0) {
        close(socket_fd_);
        socket_fd_ = -1;
    }
    connected_ = false;
}

bool FileClient::receiveAll(char *buffer, size_t length) {
    size_t total_received = 0;
    while (total_received < length) {
        ssize_t received = recv(socket_fd_, buffer + total_received,
                                length - total_received, 0);
        if (received < 0) {
            if (NetworkUtils::wouldBlock()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            return false;
        } else if (received == 0) {
            return false; // Подключение закрыто
        }
        total_received += received;
    }
    return true;
}

void FileClient::printProgress(uint64_t received, uint64_t total, uint64_t client_id) {
    if (!config_.show_progress) return;

    double progress = (static_cast<double>(received) / total) * 100;
    double mb_received = static_cast<double>(received) / (1024 * 1024);

    std::cout << "Client " << client_id << " - "
              << std::fixed << std::setprecision(1) << mb_received << "MB ("
              << progress << "%)" << std::endl;
}

bool FileClient::downloadFile() {
    if (!connected_) {
        std::cerr << "Not connected to server" << std::endl;
        return false;
    }

    auto start_time = std::chrono::steady_clock::now();

    FileMetadata metadata;
    if (!receiveAll(reinterpret_cast<char *>(&metadata), sizeof(metadata))) {
        std::cerr << "Failed to receive file metadata" << std::endl;
        return false;
    }

    std::cout << "File metadata received - Client ID: " << metadata.client_id
              << ", Size: " << (metadata.file_size / (1024 * 1024 * 1024)) << "GB"
              << ", Seed: " << metadata.seed << std::endl;

    std::ofstream file(config_.output_file, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Failed to create local file: " << config_.output_file << std::endl;
        return false;
    }

    file.write(reinterpret_cast<const char *>(&metadata), sizeof(metadata));

    // Получение файла
    std::vector<char> buffer(config_.buffer_size);
    uint64_t total_received = sizeof(metadata);
    uint64_t expected_size = metadata.file_size + sizeof(metadata);
    int chunk_count = 0;

    while (total_received < expected_size) {
        size_t remaining = expected_size - total_received;
        size_t receive_size = std::min(buffer.size(), remaining);

        if (!receiveAll(buffer.data(), receive_size)) {
            std::cerr << "Receive error at " << total_received << " bytes" << std::endl;
            return false;
        }

        file.write(buffer.data(), receive_size);
        total_received += receive_size;
        chunk_count++;

        if (chunk_count % 100 == 0) {
            printProgress(total_received - sizeof(metadata), metadata.file_size, metadata.client_id);
        }
    }

    file.close();

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time);

    std::cout << "Download completed for Client " << metadata.client_id << "!" << std::endl;
    std::cout << "Total received: " << (total_received / (1024 * 1024 * 1024)) << "GB" << std::endl;
    std::cout << "Time taken: " << duration.count() << " seconds" << std::endl;

    if (duration.count() > 0) {
        double speed = (total_received / (1024.0 * 1024.0)) / duration.count();
        std::cout << "Average speed: " << speed << " MB/s" << std::endl;
    }

    return true;
}
