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

#include <cstdlib>
#include "audio.h"
#include "common.h"

using namespace nba_libretro;

NBACoreAudioDevice::NBACoreAudioDevice() {
    this->nba_audio_cb = nullptr;
    this->stream = (std::int16_t*) calloc(EMULATOR_AUDIO_BLOCK * 2, sizeof(std::int16_t));
    this->byte_len = EMULATOR_AUDIO_BLOCK * 2;
}

NBACoreAudioDevice::~NBACoreAudioDevice() {
    free(this->stream);
    this->stream = nullptr;
    this->nba_audio_cb = nullptr;
}

bool NBACoreAudioDevice::Open(void* userdata, nba::AudioDevice::Callback callback) {
    this->nba_userdata = userdata;
    this->nba_audio_cb = callback;
    return true;
}

void NBACoreAudioDevice::Close() {
    this->nba_audio_cb = nullptr;
}

auto NBACoreAudioDevice::GetBlockSize() -> int {
    return EMULATOR_AUDIO_BLOCK;
}

auto NBACoreAudioDevice::GetSampleRate() -> int {
    return EMULATOR_AUDIO_SAMPLING_RATE;
}

void NBACoreAudioDevice::updateAndGetBuffer(const std::int16_t **outStream, int *outByteLen) {
    nba_audio_cb(nba_userdata, stream, byte_len);
    *outStream = stream;
    *outByteLen = byte_len;
}