#include "server/ConnectionManager.h"
#include <gtest/gtest.h>

TEST(ConnectionManagerTest, SingletonInstance) {
    auto &manager1 = ConnectionManager::getInstance();
    auto &manager2 = ConnectionManager::getInstance();
    EXPECT_EQ(&manager1, &manager2);
}

TEST(ConnectionManagerTest, ClientIdAssignment) {
    auto &manager = ConnectionManager::getInstance();
    uint64_t id1 = manager.assignClientId();
    uint64_t id2 = manager.assignClientId();
    EXPECT_GT(id2, id1);
}

TEST(ConnectionManagerTest, ConnectionLimits) {
    auto &manager = ConnectionManager::getInstance();

    // Первые подключения всегда работают.
    EXPECT_TRUE(manager.canAcceptConnection());

    // NOTE: Мы не можем протестировать лимит в 10 000 в
    // модульных тестах без глобальных настроек.
}
