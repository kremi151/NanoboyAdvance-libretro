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
        source/io/video.cpp
        source/io/input.cpp
        source/io/audio.cpp
		source/util/fshelper.cpp)

set(HEADERS
        # libretro
		source/util/libretro.h
        source/io/video.h
        source/io/input.h
        source/io/audio.h
		source/util/common.h
		source/util/fshelper.h
		source/meta/nba_libretro_defs.h)

# Provide NBA umbrella headers
include_directories("${CMAKE_SOURCE_DIR}/NanoboyAdvance/source")

# Read core name from nba_libretro_defs.h
set(TMP_REGEX "#define NBA_LIBRETRO_CORE_NAME[ \t]+\"(.+)\"")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/source/meta/nba_libretro_defs.h" NBA_LIBRETRO_CORE_NAME REGEX ${TMP_REGEX})
string(REGEX REPLACE ${TMP_REGEX} "\\1" NBA_LIBRETRO_CORE_NAME "${NBA_LIBRETRO_CORE_NAME}")

# Read core version from nba_libretro_defs.h
set(TMP_REGEX "#define NBA_LIBRETRO_CORE_VERSION[ \t]+\"(.+)\"")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/source/meta/nba_libretro_defs.h" NBA_LIBRETRO_CORE_VERSION REGEX ${TMP_REGEX})
string(REGEX REPLACE ${TMP_REGEX} "\\1" NBA_LIBRETRO_CORE_VERSION "${NBA_LIBRETRO_CORE_VERSION}")

# Read supported file extensions from nba_libretro_defs.h
set(TMP_REGEX "#define NBA_LIBRETRO_CORE_FILE_EXTS[ \t]+\"(.+)\"")
file(STRINGS "${CMAKE_CURRENT_SOURCE_DIR}/source/meta/nba_libretro_defs.h" NBA_LIBRETRO_CORE_FILE_EXTS REGEX ${TMP_REGEX})
string(REGEX REPLACE ${TMP_REGEX} "\\1" NBA_LIBRETRO_CORE_FILE_EXTS "${NBA_LIBRETRO_CORE_FILE_EXTS}")

# Write nba_libretro.info in build directory
configure_file(source/meta/nba_libretro.info.in ${CMAKE_BINARY_DIR}/nba_libretro.info @ONLY)

add_subdirectory(${CMAKE_SOURCE_DIR}/NanoboyAdvance)
add_library(nba_libretro SHARED ${SOURCES} ${HEADERS})
set_target_properties(nba_libretro PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(nba_libretro PROPERTIES PREFIX "") # no "lib" prefix for built binary on Linux

set_property(TARGET nba PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(nba_libretro nba)
