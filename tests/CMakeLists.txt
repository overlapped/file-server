# Tests
find_package(GTest REQUIRED)

# Test executable
add_executable(file_server_tests
    TestConnectionManager.cpp
    TestDynamicFileGenerator.cpp
#    TestFileServer.cpp
#    TestFileClient.cpp
    TestLoadDistribution.cpp
)

target_link_libraries(file_server_tests
    GTest::gtest
    GTest::gtest_main
    file_server_lib
    file_client_lib
    pthread
)

# Add test
gtest_add_tests(file_server_tests)
