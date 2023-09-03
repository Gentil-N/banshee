cmake_minimum_required(VERSION 3.0)

project(samples)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

add_subdirectory(kernel)#build orion framework

set(SMPL_SRC_DIR  ${PROJECT_SOURCE_DIR}/samples)
set(SMPL_SRC_FILES   ${SMPL_SRC_DIR}/game.cpp
                     ${SMPL_SRC_DIR}/global.cpp
                     ${SMPL_SRC_DIR}/stage.cpp)

add_executable(samples ${SMPL_SRC_FILES})

target_link_libraries(samples orion)
file(COPY ${PROJECT_SOURCE_DIR}/samples/data DESTINATION ${CMAKE_BINARY_DIR})