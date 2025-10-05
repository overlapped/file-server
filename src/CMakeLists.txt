# Server library
add_library(file_server_lib
    server/ConnectionManager.cpp
    server/DynamicFileGenerator.cpp
    server/FileServer.cpp
    server/NetworkUtils.cpp
#    common/Protocol.cpp
)

# Client library  
add_library(file_client_lib
    client/FileClient.cpp
    client/ClientConfig.cpp
    server/NetworkUtils.cpp
#    common/Protocol.cpp
)

# Server executable
add_executable(file_server
    server/main.cpp
)

target_link_libraries(file_server
    file_server_lib
    pthread
)

# Client executable
add_executable(file_client
    client/main.cpp
)

target_link_libraries(file_client
    file_client_lib
    pthread
)
