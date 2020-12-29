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

#ifndef FB_CORE_INCLUDE_HELPER_FS_H
#define FB_CORE_INCLUDE_HELPER_FS_H

#if STD_FS_IS_EXPERIMENTAL

#include <experimental/filesystem>

namespace nba_libretro {
    namespace fs = std::experimental::filesystem;
}

#else

#include <filesystem>

namespace nba_libretro {
    namespace fs = std::filesystem;
}

#endif

#endif //FB_CORE_INCLUDE_HELPER_FS_H
