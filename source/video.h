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

#ifndef NANOBOYADVANCE_LIBRETRO_CORE_VIDEO_H
#define NANOBOYADVANCE_LIBRETRO_CORE_VIDEO_H

#include "../NanoboyAdvance/source/device/video_device.hpp"
#include "libretro.h"

#include <cstdint>

namespace nba_libretro {

    class NBACoreVideoDevice: public nba::VideoDevice {
    private:
        const uint32_t width;
        const uint32_t height;
        retro_video_refresh_t video_cb;
    public:
        NBACoreVideoDevice(uint32_t width, uint32_t height);
        ~NBACoreVideoDevice() override;

        void Draw(std::uint32_t* buffer) override;

        void setVideoCallback(retro_video_refresh_t cb);
    };

}

#endif //NANOBOYADVANCE_LIBRETRO_CORE_VIDEO_H
