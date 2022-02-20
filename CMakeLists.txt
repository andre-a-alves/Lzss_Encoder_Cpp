cmake_minimum_required(VERSION 3.21)
project(Lzss_Encoder_Cpp)

set(CMAKE_CXX_STANDARD 17)

add_executable(Lzss_Encoder_Cpp main.cpp Lzss.cpp Lzss.h)
