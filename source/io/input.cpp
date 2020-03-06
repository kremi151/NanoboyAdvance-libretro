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

#include "input.h"

using namespace nba_libretro;

auto NBACoreInputDevice::Poll(nba::InputDevice::Key key) -> bool {
    if (input_state_cb == nullptr) {
        return false;
    }
    switch (key) {
        case nba::InputDevice::Key::A:
            return input_state_cb(controller_port, controller_device, 0, RETRO_DEVICE_ID_JOYPAD_A);
        case nba::InputDevice::Key::B:
            return input_state_cb(controller_port, controller_device, 0, RETRO_DEVICE_ID_JOYPAD_B);
        case nba::InputDevice::Key::L:
            return input_state_cb(controller_port, controller_device, 0, RETRO_DEVICE_ID_JOYPAD_L);
        case nba::InputDevice::Key::R:
            return input_state_cb(controller_port, controller_device, 0, RETRO_DEVICE_ID_JOYPAD_R);
        case nba::InputDevice::Key::Up:
            return input_state_cb(controller_port, controller_device, 0, RETRO_DEVICE_ID_JOYPAD_UP);
        case nba::InputDevice::Key::Right:
            return input_state_cb(controller_port, controller_device, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT);
        case nba::InputDevice::Key::Down:
            return input_state_cb(controller_port, controller_device, 0, RETRO_DEVICE_ID_JOYPAD_DOWN);
        case nba::InputDevice::Key::Left:
            return input_state_cb(controller_port, controller_device, 0, RETRO_DEVICE_ID_JOYPAD_LEFT);
        case nba::InputDevice::Key::Start:
            return input_state_cb(controller_port, controller_device, 0, RETRO_DEVICE_ID_JOYPAD_START);
        case nba::InputDevice::Key::Select:
            return input_state_cb(controller_port, controller_device, 0, RETRO_DEVICE_ID_JOYPAD_SELECT);
        default:
            return false;
    }
}

void NBACoreInputDevice::setInputCallback(retro_input_state_t cb, unsigned port, unsigned device) {
    this->input_state_cb = cb;
    this->controller_port = port;
    this->controller_device = device;
}