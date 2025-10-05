#ifndef __CLIENT_CONFIG_H__
#define __CLIENT_CONFIG_H__

#include <cstdint>
#include <string>

struct ClientConfig {
    std::string server_host;
    uint16_t server_port;
    std::string output_file;
    bool show_progress;
    size_t buffer_size;

    ClientConfig();
};

#endif // #ifndef __CLIENT_CONFIG_H__
