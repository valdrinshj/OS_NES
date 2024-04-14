cmake_minimum_required(VERSION 3.27)
project(Project C)

set(CMAKE_C_STANDARD 11)

add_executable(Project main.c
        cpu.h
        cpu.c
        bus.h
        bus.c
)
