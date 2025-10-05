#include "common/Constants.h"
#include "server/FileServer.h"
#include <atomic>
#include <csignal>
#include <iostream>

std::atomic<bool> shutdown_flag{false};

void signalHandler(int signal) {
    std::cout << "Received signal " << signal << ", shutting down..." << std::endl;
    shutdown_flag.store(true);
}

int main(int argc, char *argv[]) {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);

    uint16_t port = constants::DEFAULT_PORT;
    if (argc > 1) {
        try {
            port = static_cast<uint16_t>(std::stoi(argv[1]));
        } catch (const std::exception &e) {
            std::cerr << "Invalid port: " << argv[1] << std::endl;
            return EXIT_FAILURE;
        }
    }

    std::cout << "Starting file server on port " << port << "..." << std::endl;
    std::cout << "Maximum connections: " << constants::MAX_CONNECTIONS << std::endl;
    std::cout << "File size per client: " << (constants::FILE_SIZE / (1024 * 1024 * 1024)) << "GB" << std::endl;
    std::cout << "Press Ctrl+C to stop" << std::endl;

    FileServer server(port);
    if (!server.start()) {
        return EXIT_FAILURE;
    }

    while (!shutdown_flag.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    server.stop();
    std::cout << "Server stopped gracefully" << std::endl;

    return EXIT_SUCCESS;
}
