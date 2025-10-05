#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <cstdint>

#pragma pack(push, 1)
struct FileMetadata {
    uint64_t client_id;
    uint64_t file_size;
    uint64_t seed;        // Уникальный файл для каждого клиента.
    uint64_t chunks_sent; // Для отображения статистики.
    uint8_t checksum[16];
};
#pragma pack(pop)

#endif // #ifndef __PROTOCOL_H__