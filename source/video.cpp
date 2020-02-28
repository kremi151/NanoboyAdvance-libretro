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

#include "video.h"
#include <cstdlib>

using namespace nba_libretro;

NBACoreVideoDevice::NBACoreVideoDevice(uint32_t width, uint32_t height): width(width), height(height) {
    //frameBuffer = (uint32_t*) calloc(width * height, sizeof(uint32_t));
    this->video_cb = nullptr;
}

NBACoreVideoDevice::~NBACoreVideoDevice() {
    /*free(frameBuffer);
    frameBuffer = nullptr;*/
}

void NBACoreVideoDevice::Draw(uint32_t *buffer) {
    if (video_cb == nullptr) {
        // We're currently not in a render cycle of libretro
        return;
    }
    video_cb(buffer, width, height, width * sizeof(short));
}

void NBACoreVideoDevice::setVideoCallback(retro_video_refresh_t cb) {
    this->video_cb = cb;
}