cmake_minimum_required(VERSION 3.21)
project(Lzss_Encoder_Cpp)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0) # look into FindPkgConfig.cmake

set(CMAKE_CXX_STANDARD 17)

link_directories(
        ${GTKMM_LIBRARY_DIRS}  )
include_directories(
        .
        ${GTKMM_INCLUDE_DIRS}  )

add_executable(Lzss_Encoder_Cpp main.cpp Lzss.cpp Lzss.h GuiWindow.cpp GuiWindow.h)

target_link_libraries(Lzss_Encoder_Cpp
        ${GTKMM_LIBRARIES}  )