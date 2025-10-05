#include "server/NetworkUtils.h"
#include "common/Constants.h"
#include <cstring>
#include <errno.h>
#include <netinet/tcp.h> // For TCP_NODELAY
#include <sys/socket.h>

void NetworkUtils::setSocketOptions(int socket_fd) {
    int opt = 1;
    int buf_size = constants::SOCKET_BUFFER_SIZE;
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVBUF, &buf_size, sizeof(buf_size));
    setsockopt(socket_fd, SOL_SOCKET, SO_SNDBUF, &buf_size, sizeof(buf_size));

    int timeout = 30000;
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    setsockopt(socket_fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof(opt));
}

void NetworkUtils::setReuseAddress(int socket_fd) {
    int opt = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#ifdef SO_REUSEPORT
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt));
#endif
}

std::string NetworkUtils::getLastError() {
    return std::strerror(errno);
}

bool NetworkUtils::wouldBlock() {
    return errno == EWOULDBLOCK || errno == EAGAIN;
}

bool NetworkUtils::setNonBlocking(int socket_fd) {
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags == -1) return false;
    return fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK) != -1;
}
