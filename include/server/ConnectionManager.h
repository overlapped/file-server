#ifndef __CONNECTION_MANAGER_H__
#define __CONNECTION_MANAGER_H__

#include <atomic>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <unordered_map>

class ConnectionManager {
public:
    static ConnectionManager &getInstance();

    // Уникальный идентификатор клиента с учетом распределения нагрузки.
    uint64_t assignClientId();

    // Проверка, можно ли мы принять новое соединение с учетом текущей нагрузки.
    // Использует регулирование на основе нагрузки для предотвращения перегрузки системы.
    bool canAcceptConnection();
    void connectionEstablished(uint64_t client_id);
    void connectionCompleted(uint64_t client_id);

    // Проверка для высокой нагрузки.
    bool isUnderHeavyLoad() const;

    // Возврашает текущую загрузку соединения (от 0.0 до 1.0).
    double getConnectionUtilization() const;

    // Для статистики
    int getActiveConnections() const { return active_connections_.load(); }
    uint64_t getTotalClients() const { return total_clients_.load(); }
    void printStats() const;

    ConnectionManager(const ConnectionManager &) = delete;
    ConnectionManager &operator=(const ConnectionManager &) = delete;

private:
    ConnectionManager() = default;

    std::atomic<uint64_t> next_client_id_{1};
    std::atomic<int> active_connections_{0};
    std::atomic<uint64_t> total_clients_{0};
    mutable std::mutex stats_mutex_;
    std::unordered_map<uint64_t, std::chrono::steady_clock::time_point> connection_start_times_;

    // Порог высокой нагрузки (80% от максимального количества подключений)
    // при превышении порога новые подключения ограничиваются для поддержания качества обслуживания.
    static constexpr double HEAVY_LOAD_THRESHOLD = 0.8;
};

#endif // #ifndef __CONNECTION_MANAGER_H__
