project(NanoboyAdvance-libretro-core CXX)
cmake_minimum_required(VERSION 3.13.0)
set(CMAKE_CXX_STANDARD 17)

option(PLATFORM_QT "Enable Qt5 frontend" OFF)
option(PLATFORM_SDL "Enable SDL2 frontend" OFF)

set(SOURCES
        # Core source
        source/main.cpp
        source/video.cpp)

set(HEADERS
        # libretro
        source/libretro.h
        source/video.h)

# Provide NBA umbrella headers
include_directories("${CMAKE_SOURCE_DIR}/NanoboyAdvance/source")

add_subdirectory(${CMAKE_SOURCE_DIR}/NanoboyAdvance)
add_library(nba_libretro_core STATIC ${SOURCES} ${HEADERS})
set_target_properties(nba_libretro_core PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(nba_libretro_core nba)
