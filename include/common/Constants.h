#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <cstddef>
#include <cstdint>

namespace constants {
    constexpr size_t FILE_SIZE = 10ULL * 1024 * 1024 * 1024; // Размер файла 10 ГБ для каждого клиента
                                                             // — обеспечивает равномерную нагрузку на каждого клиента.
    constexpr size_t CHUNK_SIZE = 1024 * 1024;               // 1MB: Размер фрагмента для передачи данных — оптимизирован для пропускной
                                                             // способности сети. Более мелкие фрагменты обеспечивают лучшее чередование между клиентами.
    constexpr size_t MAX_CONNECTIONS = 10000;                // Максимальное количество одновременных подключений, поддерживаемых сервером
    constexpr uint16_t DEFAULT_PORT = 8080;
    constexpr int SOCKET_BUFFER_SIZE = 1024 * 1024; // 1MB
    constexpr int BACKLOG_SIZE = 1024;              // Определяет максимальную длину, до которой может вырасти
                                                    // очередь ожидающих соединений для прослушиваемого сокета
}

#endif // #ifndef __CONSTANTS_H__