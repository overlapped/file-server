#ifndef __FILE_CLIENT_H__
#define __FILE_CLIENT_H__

#include "client/ClientConfig.h"
#include <cstdint>

class FileClient {
public:
    FileClient(const ClientConfig &config);
    ~FileClient();

    bool connectToServer();
    bool downloadFile();
    void disconnect();

private:
    bool receiveAll(char *buffer, size_t length);
    void printProgress(uint64_t received, uint64_t total, uint64_t client_id);

    ClientConfig config_;
    int socket_fd_;
    bool connected_;
};

#endif // #ifndef __FILE_CLIENT_H__
