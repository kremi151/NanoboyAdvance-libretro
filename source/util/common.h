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

#ifndef NANOBOYADVANCE_LIBRETRO_CORE_COMMON_H
#define NANOBOYADVANCE_LIBRETRO_CORE_COMMON_H

#define EMULATOR_DISPLAY_WIDTH 240
#define EMULATOR_DISPLAY_HEIGHT 160
#define EMULATOR_AUDIO_SAMPLING_RATE 32768
#define EMULATOR_AUDIO_BLOCK 2048

#define NBA_LIBRETRO_ASYNC_AUDIO
#define NBA_LIBRETRO_LIMIT_FRAMES

#if defined(_WIN32) || defined(WIN32) || defined(_WIN64) || defined(WIN64)
#define OS_Windows
#elif defined(__unix__)
#define OS_Unix
#else
#error "This platform is currently not supported"
#endif

#endif //NANOBOYADVANCE_LIBRETRO_CORE_COMMON_H
