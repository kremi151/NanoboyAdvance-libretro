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

#ifndef NANOBOYADVANCE_LIBRETRO_CORE_INPUT_H
#define NANOBOYADVANCE_LIBRETRO_CORE_INPUT_H

#include <emulator/device/input_device.hpp>
#include "../util/libretro.h"

namespace nba_libretro {

    class NBACoreInputDevice: public nba::InputDevice {
    private:
        retro_input_state_t input_state_cb;
        unsigned controller_device;
        unsigned controller_port;
    public:
        auto Poll(Key key) -> bool override;
        void SetOnChangeCallback(std::function<void(void)> callback) override;

        void setInputCallback(retro_input_state_t cb, unsigned port, unsigned device);
    };

}

#endif //NANOBOYADVANCE_LIBRETRO_CORE_INPUT_H
