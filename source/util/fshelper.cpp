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
#include <string>

nba_libretro::fs::path nba_libretro::findFirstMatchingFile(const std::string &name, const fs::path *paths, size_t len) {
    auto *end = const_cast<fs::path*>(paths) + len;
    auto *current = const_cast<fs::path*>(paths);
    while (current < end) {
        if (current == nullptr || current->empty()) {
            current++;
            continue;
        }

        fs::path filePath = *current / name;
        if (fs::exists(filePath)) {
            return filePath;
        }

        current++;
    }

    return std::string();
}

bool nba_libretro::ensureDirExists(const fs::path &path) {
    if (!fs::exists(path)) {
        fs::create_directories(path);
        return true;
    }
    return fs::is_directory(path);
}