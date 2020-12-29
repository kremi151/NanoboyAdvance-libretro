# This is from https://github.com/vector-of-bool/pitchfork/blob/develop/cmake/FindFilesystem.cmake

include(CMakePushCheckState)
include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)

cmake_push_check_state(RESET)
set(CMAKE_CXX_STANDARD 17)

set(have_fs FALSE)

check_include_file_cxx("filesystem" HAVE_STD_FILESYSTEM)
check_include_file_cxx("experimental/filesystem" HAVE_STD_EXPERIMENTAL_FILESYSTEM)

if(HAVE_STD_FILESYSTEM)
    set(have_fs TRUE)
    set(header filesystem)
    set(namespace std::filesystem)
elseif(HAVE_STD_EXPERIMENTAL_FILESYSTEM)
    set(have_fs TRUE)
    set(header experimental/filesystem)
    set(namespace std::experimental::filesystem)
endif()

string(CONFIGURE [[
    #include <@header@>

    int main() {
        auto cwd = @namespace@::current_path();
        return cwd.string().size();
    }
]] code @ONLY)

check_cxx_source_compiles("${code}" CAN_COMPILE_FS_WITHOUT_LINK)

if(NOT CAN_COMPILE_FS_WITHOUT_LINK)
    set(CMAKE_REQUIRED_LIBRARIES -lstdc++fs)
    check_cxx_source_compiles("${code}" CAN_COMPILE_FS_WITH_LINK)
endif()

cmake_pop_check_state()

if(have_fs)
    add_library(CXX::Filesystem INTERFACE IMPORTED)
    target_compile_definitions(CXX::Filesystem INTERFACE STD_FS_IS_EXPERIMENTAL=$<NOT:$<BOOL:${HAVE_STD_FILESYSTEM}>>)
    if(CAN_COMPILE_FS_WITHOUT_LINK)
        # Nothing to add...
    elseif(CAN_COMPILE_FS_WITH_LINK)
        target_link_libraries(CXX::Filesystem INTERFACE -lstdc++fs)
    else()
        message(WARNING "Failed to link a filesystem library, although we found the headers...?")
    endif()
    set(Filesystem_FOUND TRUE CACHE BOOL "" FORCE)
else()
    set(Filesystem_FOUND FALSE CACHE BOOL "" FORCE)
    if(Filesystem_FIND_REQUIRED)
        message(FATAL_ERROR "No C++ support for std::filesystem")
    endif()
endif()
