cmake_minimum_required(VERSION 3.12)
project(project_part1)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_CXX_FLAGS "${CMAKE_CPP_FLAGS} -Wall -Wno-conversion -Wno-deprecated-register")

find_package(FLEX)
FLEX_TARGET(Flexer flexer.l ${CMAKE_CURRENT_BINARY_DIR}/../flexer.cpp )

include_directories(../../projectpart1)

set(SOURCE_FILES
        main.cpp
        flexer.h
        hlexer.cpp
        hlexer.h
        lexer.h
        token.h
        ${FLEX_Flexer_OUTPUTS})

add_executable(project_part1 ${SOURCE_FILES})
