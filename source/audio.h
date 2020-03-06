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

#ifndef NANOBOYADVANCE_LIBRETRO_CORE_AUDIO_H
#define NANOBOYADVANCE_LIBRETRO_CORE_AUDIO_H

#include <emulator/device/audio_device.hpp>

namespace nba_libretro {

    class NBACoreAudioDevice : public nba::AudioDevice {
    private:
        void* nba_userdata;
        Callback nba_audio_cb;
        std::int16_t* stream;
        int byte_len;
    public:
        NBACoreAudioDevice();
        ~NBACoreAudioDevice() override;

        auto GetSampleRate() -> int override;
        auto GetBlockSize() -> int override;
        bool Open(void* userdata, Callback callback) override;
        void Close() override;

        void updateAndGetBuffer(const std::int16_t **outStream, int *outByteLen);
    };

}

#endif //NANOBOYADVANCE_LIBRETRO_CORE_AUDIO_H
