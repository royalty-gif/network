# cmake最低版本要求
cmake_minimum_required(VERSION 3.7)

# 项目名称
project(net)
set(CMAKE_C_STANDARD 99)
set(CMAKE_BUILD_TYPE Debug)

# 链接库路径
LINK_DIRECTORIES(
    ${PROJECT_SOURCE_DIR}/pcap/lib
    ${PROJECT_SOURCE_DIR}/3rdparty/zlog/lib
)

# 头文件路径
include_directories(
    ${PROJECT_SOURCE_DIR}/pcap/include
    ${PROJECT_SOURCE_DIR}/pcap/include/pcap
    ${PROJECT_SOURCE_DIR}/net/base
    ${PROJECT_SOURCE_DIR}/net/protocol
    ${PROJECT_SOURCE_DIR}/net/plat
    ${PROJECT_SOURCE_DIR}/3rdparty/zlog/include
)

file(GLOB_RECURSE SRC_FILES
    ${PROJECT_SOURCE_DIR}/net/*.c
    ${PROJECT_SOURCE_DIR}/net/base/*.c
    ${PROJECT_SOURCE_DIR}/net/protocol/*.c
    ${PROJECT_SOURCE_DIR}/net/plat/*.c
)

add_executable(${PROJECT_NAME} main.c ${SRC_FILES})

# 链接库
target_link_libraries(${PROJECT_NAME} pcap pthread zlog)
