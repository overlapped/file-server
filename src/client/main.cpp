#include "client/FileClient.h"
#include "client/ClientConfig.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    ClientConfig config;
    
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <server_host> [port] [output_file]" << std::endl;
        std::cout << "Default: 127.0.0.1:8080 download.bin" << std::endl;
        return 1;
    }
    
    config.server_host = argv[1];
    
    if (argc > 2) {
        try {
            config.server_port = static_cast<uint16_t>(std::stoi(argv[2]));
        } catch (const std::exception& e) {
            std::cerr << "Invalid port: " << argv[2] << std::endl;
            return 1;
        }
    }
    
    if (argc > 3) {
        config.output_file = argv[3];
    }

    FileClient client(config);
    
    if (!client.connectToServer()) {
        return 1;
    }

    std::cout << "Starting file download..." << std::endl;
    if (!client.downloadFile()) {
        std::cerr << "File download failed" << std::endl;
        return 1;
    }

    std::cout << "Download completed successfully!" << std::endl;
    return 0;
}
