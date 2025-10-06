#include "server/ConnectionManager.h"
#include <atomic>
#include <gtest/gtest.h>
#include <thread>
#include <vector>

TEST(LoadDistributionTest, ConcurrentConnections) {
    auto &manager = ConnectionManager::getInstance();

    const int num_threads = 100;
    std::vector<std::thread> threads;
    std::atomic<int> successful_connections{0};

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back([&manager, &successful_connections]() {
            if (manager.canAcceptConnection()) {
                uint64_t client_id = manager.assignClientId();
                manager.connectionEstablished(client_id);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                manager.connectionCompleted(client_id);
                successful_connections.fetch_add(1);
            }
        });
    }

    for (auto &thread : threads) {
        thread.join();
    }

    EXPECT_GE(successful_connections.load(), num_threads - 10);
}

TEST(LoadDistributionTest, ConnectionManagerLoadAwareness) {
    auto &manager = ConnectionManager::getInstance();

    // Тест что менеджер подключений правильно выдает статистику нагрузки.
    double utilization = manager.getConnectionUtilization();
    EXPECT_GE(utilization, 0.0);
    EXPECT_LE(utilization, 1.0);

    // Сначала нет нагрузки.
    EXPECT_FALSE(manager.isUnderHeavyLoad());
}
