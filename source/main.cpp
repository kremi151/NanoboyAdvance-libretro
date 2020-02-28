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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <memory>
#include "libretro.h"
#include "../NanoboyAdvance/source/emulator/emulator.hpp"
#include "../NanoboyAdvance/source/config/config.hpp"
#include "../NanoboyAdvance/source/device/audio_device.hpp"
#include "../NanoboyAdvance/source/device/input_device.hpp"
#include "video.h"
// #include <emulator/emulator.hpp>

static struct retro_log_callback logging;
static retro_log_printf_t log_cb;

//auto config = std::make_shared<nba::Config>();
//auto emulator = std::make_unique<nba::Emulator>(config);

std::shared_ptr<nba::Config> nbaConfig;
std::unique_ptr<nba::Emulator> nbaEmulator;
std::shared_ptr<nba_libretro::NBACoreVideoDevice> nbaVideoDevice;

#define EMULATOR_DISPLAY_WIDTH 240
#define EMULATOR_DISPLAY_HEIGHT 160

#define DEBUG_BIOS_PATH "D:\\Retro\\GBA\\bios\\gba_bios.bin"

static void fallback_log(enum retro_log_level level, const char *fmt, ...)
{
   (void)level;
   va_list va;
   va_start(va, fmt);
   vfprintf(stderr, fmt, va);
   va_end(va);
}

extern "C" {

    void retro_init(void) {
        nbaConfig = std::make_shared<nba::Config>();

        nbaConfig->audio_dev = std::make_shared<nba::NullAudioDevice>(); // TODO: Re-implement
        nbaConfig->input_dev = std::make_shared<nba::NullInputDevice>(); // TODO: Re-implement

        nbaVideoDevice = std::make_shared<nba_libretro::NBACoreVideoDevice>(EMULATOR_DISPLAY_WIDTH, EMULATOR_DISPLAY_HEIGHT);
        nbaConfig->video_dev = nbaVideoDevice;

        // TODO: Make configurable
        nbaConfig->bios_path = DEBUG_BIOS_PATH;

        nbaEmulator = std::make_unique<nba::Emulator>(nbaConfig);
    }

    void retro_deinit(void) {
        nbaConfig.reset();
        nbaEmulator.reset();
        nbaVideoDevice.reset();
    }

    unsigned retro_api_version(void) {
        return RETRO_API_VERSION;
    }

    void retro_set_controller_port_device(unsigned port, unsigned device) {
        //TODO
        log_cb(RETRO_LOG_INFO, "Plugging device %u into port %u.\n", device, port);
    }

    void retro_get_system_info(struct retro_system_info *info) {
        //TODO
        memset(info, 0, sizeof(*info));
        info->library_name = "NanoboyAdvance libretro Core";
        info->library_version = "0.0.1";
        info->need_fullpath = true;
        info->valid_extensions = "bin|gba";
    }

    static retro_video_refresh_t video_cb;
    static retro_audio_sample_t audio_cb;
    static retro_audio_sample_batch_t audio_batch_cb;
    static retro_environment_t environ_cb;
    static retro_input_poll_t input_poll_cb;
    static retro_input_state_t input_state_cb;

    void retro_get_system_av_info(struct retro_system_av_info *info) {
        float aspect = 3.0f / 2.0f;
        float sampling_rate = 30000.0f;

        info->timing.fps = 16777216.0 / 280896.0;
        info->timing.sample_rate = sampling_rate;

        info->geometry.base_width = EMULATOR_DISPLAY_WIDTH;
        info->geometry.base_height = EMULATOR_DISPLAY_HEIGHT;
        info->geometry.max_width = EMULATOR_DISPLAY_WIDTH;
        info->geometry.max_height = EMULATOR_DISPLAY_HEIGHT;
        info->geometry.aspect_ratio = aspect;
    }

    void retro_set_environment(retro_environment_t cb) {
        //TODO
        environ_cb = cb;

        if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
            log_cb = logging.log;
        else
            log_cb = fallback_log;
    }

    void retro_set_audio_sample(retro_audio_sample_t cb) {
        //TODO
        audio_cb = cb;
    }

    void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) {
        //TODO
        audio_batch_cb = cb;
    }

    void retro_set_input_poll(retro_input_poll_t cb) {
        //TODO
        input_poll_cb = cb;
    }

    void retro_set_input_state(retro_input_state_t cb) {
        //TODO
        input_state_cb = cb;
    }

    void retro_set_video_refresh(retro_video_refresh_t cb) {
        video_cb = cb;
    }

    void retro_reset(void) {
        //TODO
        // Nothing needs to happen when the game is reset.
    }

    /**
     * libretro callback; Called every game tick.
     */
    void retro_run(void) {
        nbaVideoDevice->setLogCallback(log_cb);
        nbaVideoDevice->setVideoCallback(video_cb);
        nbaEmulator->Frame();
        nbaVideoDevice->setVideoCallback(nullptr);
    }

    /**
     * libretro callback; Called when a game is to be loaded.
     */
    bool retro_load_game(const struct retro_game_info *info) {
        auto pixel_fmt = RETRO_PIXEL_FORMAT_XRGB8888;
        if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &pixel_fmt)) {
            log_cb(retro_log_level::RETRO_LOG_ERROR, "Required pixel format XRGB8888 is not supported\n");
            return false;
        }

        if (info == nullptr || info->path == nullptr || info->path[0] == '\0') {
            log_cb(retro_log_level::RETRO_LOG_ERROR, "No ROM path specified\n");
            return false;
        }

        auto status = nbaEmulator->LoadGame(info->path);
        switch (status) {
            case nba::Emulator::StatusCode::GameNotFound: {
                log_cb(retro_log_level::RETRO_LOG_ERROR, "Cannot open ROM: %s\n", info->path);
                return false;
            }
            case nba::Emulator::StatusCode::BiosNotFound: {
                log_cb(retro_log_level::RETRO_LOG_ERROR, "Cannot open BIOS: %s\n", nbaConfig->bios_path.c_str());
                return false;
            }
            case nba::Emulator::StatusCode::GameWrongSize: {
                log_cb(retro_log_level::RETRO_LOG_ERROR, "The provided ROM file is larger than the allowed 32 MiB.\n");
                return false;
            }
            case nba::Emulator::StatusCode::BiosWrongSize: {
                log_cb(retro_log_level::RETRO_LOG_ERROR, "The provided BIOS file does not match the expected size of 16 KiB.\n");
                return false;
            }
            case nba::Emulator::StatusCode::Ok: {
                return true;
            }
            default: {
                log_cb(retro_log_level::RETRO_LOG_ERROR, "An unexpected error occurred\n");
                return false;
            }
        }
    }

    void retro_unload_game(void) {
        //TODO
        // Nothing needs to happen when the game unloads.
    }

    unsigned retro_get_region(void) {
        //TODO
        return RETRO_REGION_NTSC;
    }

    bool retro_load_game_special(unsigned game_type, const struct retro_game_info *info, size_t num_info) {
        //TODO
        return retro_load_game(info);
    }

    size_t retro_serialize_size(void) {
        //TODO
        return 0;
    }

    bool retro_serialize(void *data_, size_t size) {
        //TODO
        return true;
    }

    bool retro_unserialize(const void *data_, size_t size) {
        //TODO
        return true;
    }

    void *retro_get_memory_data(unsigned id) {
        //TODO
        (void) id;
        return NULL;
    }

    size_t retro_get_memory_size(unsigned id) {
        //TODO
        (void) id;
        return 0;
    }

    void retro_cheat_reset(void) {
        //TODO
        // Nothing.
    }

    void retro_cheat_set(unsigned index, bool enabled, const char *code) {
        //TODO
        (void) index;
        (void) enabled;
        (void) code;
    }

}