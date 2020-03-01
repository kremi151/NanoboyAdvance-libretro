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

#ifndef NANOBOYADVANCE_LIBRETRO_CORE_FSHELPER_H
#define NANOBOYADVANCE_LIBRETRO_CORE_FSHELPER_H

#include <string>

namespace nba_libretro {

    std::string concatPaths(const std::string &p1, const std::string &p2);

    std::string findFirstMatchingFile(const std::string &name, const std::string *paths, size_t len);

    bool ensureDirExists(const std::string &path);

}

#endif //NANOBOYADVANCE_LIBRETRO_CORE_FSHELPER_H
