#include "server/DynamicFileGenerator.h"
#include <gtest/gtest.h>

TEST(DynamicFileGeneratorTest, ChunkGeneration) {
    DynamicFileGenerator generator;

    // Разные клиенты получают разные фрагменты файлов
    auto chunk1 = generator.generateChunk(1, 0, 1024);
    auto chunk2 = generator.generateChunk(2, 0, 1024);

    EXPECT_EQ(chunk1.size(), 1024);
    EXPECT_EQ(chunk2.size(), 1024);

    EXPECT_NE(memcmp(chunk1.data(), chunk2.data(), 1024), 0);
}

TEST(DynamicFileGeneratorTest, ConsistentChunkGeneration) {
    DynamicFileGenerator generator;

    auto chunk1 = generator.generateChunk(100, 50, 512);
    auto chunk2 = generator.generateChunk(100, 50, 512);

    EXPECT_EQ(memcmp(chunk1.data(), chunk2.data(), 512), 0);
}
