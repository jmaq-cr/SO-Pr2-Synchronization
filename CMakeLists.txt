cmake_minimum_required(VERSION 3.0.0)
project(SO-Pr2-Synchronization VERSION 0.0.0)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
include(CTest)
enable_testing()
INCLUDE_DIRECTORIES(lib)
add_subdirectory(lib) 
INCLUDE_DIRECTORIES(include)
add_subdirectory(include)

#set (CMAKE_CXX_FLAGS "-lcollectc")

# Initializer
set(SOURCES_INI     initializer.c)
add_executable(Initializer ${SOURCES_INI})
target_link_libraries(Initializer Threads::Threads lib include)

# Producer
set(SOURCES_PRO     producer.c)
add_executable(Producer ${SOURCES_PRO})
target_link_libraries(Producer Threads::Threads lib include)

# Test
set(SOURCES_SRV     server.c)
add_executable(Server ${SOURCES_SRV})
target_link_libraries(Server Threads::Threads)

set(SOURCES_CLI     client.c)
add_executable(Client ${SOURCES_CLI})
target_link_libraries(Client Threads::Threads)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
