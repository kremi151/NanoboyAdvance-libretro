/*
 * NanoboyAdvance for libretro
 * Copyright (C) 2020  Michel Kremer (kremi151)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "fshelper.h"
#include "common.h"
#include <string>

#if __cpp_lib_filesystem
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#endif

std::string nba_libretro::concatPaths(const std::string &p1, const std::string &p2) {
#ifdef OS_Windows
    char sep = '\\';
#else
    char sep = '/';
#endif
    if (p1[p1.length()] != sep) {
        std::string tmp = p1;
        tmp += sep;
        return tmp + p2;
    } else {
        return p1 + p2;
    }
}

std::string nba_libretro::findFirstMatchingFile(const std::string &name, const std::string *paths, size_t len) {
    auto *end = const_cast<std::string*>(paths) + len;
    auto *current = const_cast<std::string*>(paths);
    while (current < end) {
        if (current == nullptr || current->empty()) {
            current++;
            continue;
        }

        std::string filePath = concatPaths(*current, name);
        if (fs::exists(filePath)) {
            return filePath;
        }

        current++;
    }

    return std::string();
}

bool nba_libretro::ensureDirExists(const std::string &path) {
    if (!fs::exists(path)) {
        fs::create_directories(path);
        return true;
    }
    return fs::is_directory(path);
}