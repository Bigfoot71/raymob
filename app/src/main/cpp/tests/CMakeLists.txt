cmake_minimum_required(VERSION 3.16)
project(nbapp_core_tests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(Threads REQUIRED)

add_executable(world_test world_test.cpp ../game/world.cpp)
target_include_directories(world_test PRIVATE ../game)

add_executable(network_client_test
    network_client_test.cpp
    ../game/world.cpp
    ../game/network.cpp)
target_include_directories(network_client_test PRIVATE ../game)
target_link_libraries(network_client_test PRIVATE Threads::Threads)
if(WIN32)
    target_link_libraries(network_client_test PRIVATE ws2_32)
endif()

enable_testing()
add_test(NAME world_test COMMAND world_test)
add_test(NAME network_client_test COMMAND network_client_test)
