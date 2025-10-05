#include "server/DynamicFileGenerator.h"
#include "common/Constants.h"
#include "common/Protocol.h"
#include "server/NetworkUtils.h"
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sys/socket.h>
#include <thread>

DynamicFileGenerator::DynamicFileGenerator() : rd_() {}

uint64_t DynamicFileGenerator::generateSeed() {
    std::lock_guard<std::mutex> lock(rng_mutex_);
    return rd_();
}

bool DynamicFileGenerator::sendAll(int socket, const char *data, size_t length) {
    size_t total_sent = 0;
    while (total_sent < length) {
        ssize_t sent = send(socket, data + total_sent, length - total_sent, MSG_NOSIGNAL);
        if (sent < 0) {
            if (NetworkUtils::wouldBlock()) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                continue;
            }
            return false;
        }
        total_sent += sent;
    }
    return true;
}

bool DynamicFileGenerator::generateAndSendFile(int client_socket, uint64_t client_id) {
    uint64_t seed = generateSeed();

    FileMetadata metadata{};
    metadata.client_id = client_id;
    metadata.file_size = constants::FILE_SIZE;
    metadata.seed = seed;
    metadata.chunks_sent = 0;

    if (!sendAll(client_socket, reinterpret_cast<const char *>(&metadata), sizeof(metadata))) {
        return false;
    }

    std::vector<char> buffer(constants::CHUNK_SIZE);
    size_t total_sent = 0;
    uint64_t chunk_counter = 0;

    while (total_sent < constants::FILE_SIZE) {
        size_t remaining = constants::FILE_SIZE - total_sent;
        size_t current_chunk_size = std::min(constants::CHUNK_SIZE, remaining);

        auto chunk = generateChunk(client_id, chunk_counter, current_chunk_size);

        if (!sendAll(client_socket, chunk.data(), current_chunk_size)) {
            return false;
        }

        total_sent += current_chunk_size;
        chunk_counter++;

        if (chunk_counter % 100 == 0) {
            double progress = (static_cast<double>(total_sent) / constants::FILE_SIZE) * 100;
            std::cout << "Client " << client_id << ": "
                      << (total_sent / (1024 * 1024)) << "MB ("
                      << std::fixed << std::setprecision(1) << progress << "%)" << std::endl;
        }
    }

    std::cout << "Completed file for client " << client_id
              << " (Seed: " << seed << ", Chunks: " << chunk_counter << ")" << std::endl;
    return true;
}

std::vector<char> DynamicFileGenerator::generateChunk(uint64_t client_id, uint64_t chunk_index, size_t size) {
    std::vector<char> chunk(size);
    std::mt19937_64 gen(client_id + chunk_index);

    for (size_t i = 0; i < size; i += sizeof(uint64_t)) {
        uint64_t value = gen();
        size_t copy_size = std::min(sizeof(uint64_t), size - i);
        std::memcpy(chunk.data() + i, &value, copy_size);
    }

    return chunk;
}
