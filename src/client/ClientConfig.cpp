#include "client/ClientConfig.h"
#include "common/Constants.h"

ClientConfig::ClientConfig() 
    : server_host("127.0.0.1")
    , server_port(8080)
    , output_file("download.bin")
    , show_progress(true)
    , buffer_size(constants::CHUNK_SIZE) {}
