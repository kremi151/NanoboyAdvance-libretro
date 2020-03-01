project(NanoboyAdvance-libretro-core CXX)
cmake_minimum_required(VERSION 3.13.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Set default compile flags for GCC
if (CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "-Wall -Wextra")
endif (CMAKE_COMPILER_IS_GNUCXX)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if (MSVC)
	message("We're running in Visual Studio!")
	if (MSVC_VERSION GREATER_EQUAL "1900")
		include(CheckCXXCompilerFlag)
		CHECK_CXX_COMPILER_FLAG("/std:c++latest" _cpp_latest_flag_supported)
		if (_cpp_latest_flag_supported)
			message("/std:c++latest is supported")
			add_compile_options("/std:c++latest")
		endif()
	endif()
endif (MSVC)

# Disable NanoboyAdvance's default platforms
option(PLATFORM_QT "Enable Qt5 frontend" OFF)
option(PLATFORM_SDL "Enable SDL2 frontend" OFF)

set(SOURCES
        # Core source
        source/main.cpp
        source/video.cpp
		source/input.cpp
		source/audio.cpp
		source/fshelper.cpp)

set(HEADERS
        # libretro
        source/libretro.h
        source/video.h
		source/input.h
		source/audio.h
		source/common.h
		source/fshelper.h)

# Provide NBA umbrella headers
include_directories("${CMAKE_SOURCE_DIR}/NanoboyAdvance/source")

add_subdirectory(${CMAKE_SOURCE_DIR}/NanoboyAdvance)
add_library(nba_libretro SHARED ${SOURCES} ${HEADERS})
set_target_properties(nba_libretro PROPERTIES LINKER_LANGUAGE CXX)

target_link_libraries(nba_libretro nba)
