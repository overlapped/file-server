#ifndef __DYNAMIC_FILE_GENERATOR_H__
#define __DYNAMIC_FILE_GENERATOR_H__

#include <cstdint>
#include <mutex>
#include <random>
#include <vector>

class DynamicFileGenerator {
public:
    DynamicFileGenerator();
    bool generateAndSendFile(int client_socket, uint64_t client_id);

    // Генерация уникального фрагмента данных для конкретного клиента и индекса фрагмента.
    // Это гарантирует, что каждый клиент получит разный контент, сохраняя при этом
    // равную вычислительную нагрузку при генерации, что критически важно для равномерной загрузки ЦП.
    std::vector<char> generateChunk(uint64_t client_id, uint64_t chunk_index, size_t size);

private:
    // Генерация уникального начального значения для клиента, чтобы гарантировать уникальность
    // содержимого файлов. Каждый клиент получает уникальный файл.
    uint64_t generateSeed();
    bool sendAll(int socket, const char *data, size_t length);

    std::mutex rng_mutex_;
    std::random_device rd_;
};

#endif // #ifndef __DYNAMIC_FILE_GENERATOR_H__
