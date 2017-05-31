cmake_minimum_required(VERSION 3.0.0)
project(SO-Pr2-Synchronization VERSION 0.0.0)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
include(CTest)
enable_testing()
INCLUDE_DIRECTORIES(lib)
add_subdirectory(lib) 

#set (CMAKE_CXX_FLAGS "-lcollectc")

set(SOURCES     mapreadjson.c)
set (HEADERS    template.h)
add_executable(SO-Pr2-Synchronization ${SOURCES} ${HEADERS})
target_link_libraries(SO-Pr2-Synchronization Threads::Threads lib)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
