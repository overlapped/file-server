#ifndef __FILE_SERVER_H__
#define __FILE_SERVER_H__

#include <atomic>
#include <memory>
#include <thread>
#include <vector>

class FileServer {
public:
    FileServer(uint16_t port);
    ~FileServer();

    bool start();
    void stop();
    bool isRunning() const { return running_.load(); }

private:
    FileServer(const FileServer &) = delete;
    FileServer &operator=(const FileServer &) = delete;

    void run();
    void handleClient(int client_socket);
    void cleanupThreads();
    void statsMonitor();

    int server_fd_;
    uint16_t port_;
    std::atomic<bool> running_{false};
    std::thread main_thread_;
    std::thread stats_thread_;
    std::vector<std::thread> worker_threads_;
};

#endif // #ifndef __FILE_SERVER_H__
