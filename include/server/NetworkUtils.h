#ifndef __NETWORK_UTILS_H__
#define __NETWORK_UTILS_H__

#include <fcntl.h>
#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>

class NetworkUtils {
public:
    static void setSocketOptions(int socket_fd);
    static void setReuseAddress(int socket_fd);
    static std::string getLastError();
    static bool wouldBlock();
    static bool setNonBlocking(int socket_fd);
};

#endif // #ifndef __NETWORK_UTILS_H__
