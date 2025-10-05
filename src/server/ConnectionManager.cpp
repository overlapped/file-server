#include "server/ConnectionManager.h"
#include "common/Constants.h"
#include <iomanip>
#include <iostream>

ConnectionManager &ConnectionManager::getInstance() {
    static ConnectionManager instance;
    return instance;
}

uint64_t ConnectionManager::assignClientId() {
    // Простой атомарный счётчик обеспечивает уникальные идентификаторы.
    // В распределённой системе могут использоваться более сложные схемы.
    // Но для одного сервера атомарный счётчик обеспечивает хорошее распределение.
    return next_client_id_.fetch_add(1, std::memory_order_relaxed);
}

bool ConnectionManager::canAcceptConnection() {
    // Новые подключения только при отсутствии высокой нагрузки.
    // Это предотвращает перегрузку системы и поддерживает качество обслуживания.
    // bool under_max_limit = active_connections_.load() < constants::MAX_CONNECTIONS;
    // bool not_under_heavy_load = !isUnderHeavyLoad();

    // return under_max_limit && not_under_heavy_load;

    return active_connections_.load(std::memory_order_acquire) < constants::MAX_CONNECTIONS;
}

void ConnectionManager::connectionEstablished(uint64_t client_id) {
    active_connections_.fetch_add(1, std::memory_order_acq_rel);
    total_clients_.fetch_add(1, std::memory_order_relaxed);

    std::lock_guard<std::mutex> lock(stats_mutex_);
    connection_start_times_[client_id] = std::chrono::steady_clock::now();

    std::cout << "Client " << client_id << " connected. Active: "
              << active_connections_.load() << std::endl;
}

void ConnectionManager::connectionCompleted(uint64_t client_id) {
    active_connections_.fetch_sub(1, std::memory_order_acq_rel);

    std::lock_guard<std::mutex> lock(stats_mutex_);
    auto it = connection_start_times_.find(client_id);
    if (it != connection_start_times_.end()) {
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(
            end_time - it->second);
        connection_start_times_.erase(it);

        std::cout << "Client " << client_id << " completed. Duration: "
                  << duration.count() << "s. Active: "
                  << active_connections_.load() << std::endl;
    }
}

bool ConnectionManager::isUnderHeavyLoad() const {
    // Когда загрузка системы превышает пороговое значение, замедлить
    // новые соединения. Это гарантирует сохранение высокой
    // производительности существующих соединений. И предотвращает каскадные
    // сбои при экстремальной нагрузке.
    return getConnectionUtilization() > HEAVY_LOAD_THRESHOLD;
}

double ConnectionManager::getConnectionUtilization() const {
    return static_cast<double>(active_connections_.load()) / constants::MAX_CONNECTIONS;
}

void ConnectionManager::printStats() const {
    std::lock_guard<std::mutex> lock(stats_mutex_);
    std::cout << "=== SERVER STATS ===" << std::endl;
    std::cout << "Active connections: " << active_connections_.load() << std::endl;
    std::cout << "Total clients served: " << total_clients_.load() << std::endl;
    std::cout << "Connection start times tracked: " << connection_start_times_.size() << std::endl;
}
