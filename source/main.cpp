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
#include "util/libretro.h"
#include <emulator/emulator.hpp>
#include <emulator/config/config.hpp>
#include <emulator/device/audio_device.hpp>
#include <emulator/device/input_device.hpp>
#include "io/video.h"
#include "io/input.h"
#include "io/audio.h"
#include "util/common.h"
#include "util/fshelper.h"
#include "util/fs.h"
#include "meta/nba_libretro_defs.h"

static struct retro_log_callback logging;
static retro_log_printf_t log_cb;


std::shared_ptr<nba::Config> nbaConfig;
std::unique_ptr<nba::Emulator> nbaEmulator;
std::shared_ptr<nba_libretro::NBACoreVideoDevice> nbaVideoDevice;
std::shared_ptr<nba_libretro::NBACoreInputDevice> nbaInputDevice;
std::shared_ptr<nba_libretro::NBACoreAudioDevice> nbaAudioDevice;

#ifdef NBA_LIBRETRO_ASYNC_AUDIO
#include <thread>
std::unique_ptr<std::thread> audioThread;
static bool nba_audio_running = false;
static bool nba_audio_new_frame = false;
#endif

#ifdef NBA_LIBRETRO_LIMIT_FRAMES
#include <common/framelimiter.hpp>
std::unique_ptr<common::Framelimiter> frameLimiter;
#endif

unsigned current_controller_port;
unsigned current_controller_device;

static void fallback_log(enum retro_log_level level, const char *fmt, ...)
{
   (void)level;
   va_list va;
   va_start(va, fmt);
   vfprintf(stderr, fmt, va);
   va_end(va);
}

extern "C" {

    static retro_video_refresh_t video_cb;
    static retro_audio_sample_t audio_cb;
    static retro_audio_sample_batch_t audio_batch_cb;
    static retro_environment_t environ_cb;
    static retro_input_poll_t input_poll_cb;
    static retro_input_state_t input_state_cb;

    void retro_init(void) {
        nbaConfig = std::make_shared<nba::Config>();

        nbaAudioDevice = std::make_shared<nba_libretro::NBACoreAudioDevice>();
        nbaConfig->audio_dev = nbaAudioDevice;

        nbaInputDevice = std::make_shared<nba_libretro::NBACoreInputDevice>();
        nbaConfig->input_dev = nbaInputDevice;

        nbaVideoDevice = std::make_shared<nba_libretro::NBACoreVideoDevice>(EMULATOR_DISPLAY_WIDTH, EMULATOR_DISPLAY_HEIGHT);
        nbaConfig->video_dev = nbaVideoDevice;

        nbaEmulator = std::make_unique<nba::Emulator>(nbaConfig);

#ifdef NBA_LIBRETRO_LIMIT_FRAMES
        frameLimiter = std::make_unique<common::Framelimiter>(59.7275);
        frameLimiter->Reset();
#endif

        current_controller_device = RETRO_DEVICE_JOYPAD;
        current_controller_port = 0;

        audio_cb = nullptr;
        audio_batch_cb = nullptr;
    }

    void retro_deinit(void) {
        nbaConfig.reset();
        nbaEmulator.reset();
        nbaVideoDevice.reset();

#ifdef NBA_LIBRETRO_ASYNC_AUDIO
        audioThread.reset();
#endif
#ifdef NBA_LIBRETRO_LIMIT_FRAMES
        frameLimiter.reset();
#endif
    }

    unsigned retro_api_version(void) {
        return RETRO_API_VERSION;
    }

    void retro_set_controller_port_device(unsigned port, unsigned device) {
        log_cb(RETRO_LOG_INFO, "Plugging device %u into port %u.\n", device, port);

        // TODO: Filter supported devices (like JOYPAD or KEYBOARD)
        //current_controller_port = port;
        //current_controller_device = device;
    }

    void retro_get_system_info(struct retro_system_info *info) {
        memset(info, 0, sizeof(*info));
        info->library_name = NBA_LIBRETRO_CORE_NAME;
        info->library_version = NBA_LIBRETRO_CORE_VERSION;
        info->need_fullpath = true;
        info->valid_extensions = NBA_LIBRETRO_CORE_FILE_EXTS;
    }

    void retro_get_system_av_info(struct retro_system_av_info *info) {
        float aspect = 3.0f / 2.0f;
        float sampling_rate = EMULATOR_AUDIO_SAMPLING_RATE;

        info->timing.fps = 16777216.0 / 280896.0;
        info->timing.sample_rate = sampling_rate;

        info->geometry.base_width = EMULATOR_DISPLAY_WIDTH;
        info->geometry.base_height = EMULATOR_DISPLAY_HEIGHT;
        info->geometry.max_width = EMULATOR_DISPLAY_WIDTH;
        info->geometry.max_height = EMULATOR_DISPLAY_HEIGHT;
        info->geometry.aspect_ratio = aspect;
    }

    void retro_set_environment(retro_environment_t cb) {
        environ_cb = cb;

        if (cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &logging))
            log_cb = logging.log;
        else
            log_cb = fallback_log;
    }

    void retro_set_audio_sample(retro_audio_sample_t cb) {
        audio_cb = cb;
    }

    void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb) {
        audio_batch_cb = cb;
    }

    void retro_set_input_poll(retro_input_poll_t cb) {
        input_poll_cb = cb;
    }

    void retro_set_input_state(retro_input_state_t cb) {
        input_state_cb = cb;
    }

    void retro_set_video_refresh(retro_video_refresh_t cb) {
        video_cb = cb;
    }

    void retro_reset(void) {
        if (nbaEmulator) {
            nbaEmulator->Reset();
        }
#ifdef NBA_LIBRETRO_LIMIT_FRAMES
        if (frameLimiter) {
            frameLimiter->Reset();
        }
#endif
    }

    void render_audio(retro_audio_sample_batch_t cb) {
        const int16_t *stream;
        int byte_len;

#ifdef NBA_LIBRETRO_ASYNC_AUDIO
        if (!nba_audio_new_frame) {
            return;
        }
#endif
        nbaAudioDevice->updateAndGetBuffer(&stream, &byte_len);
#ifdef NBA_LIBRETRO_ASYNC_AUDIO
        nba_audio_new_frame = false;
#endif

        cb(stream, EMULATOR_AUDIO_BLOCK);
    }

    void render_frame() {
        nbaEmulator->Frame();
#ifdef NBA_LIBRETRO_ASYNC_AUDIO
        nba_audio_new_frame = true;
#endif
    }

    /**
     * libretro callback; Called every game tick.
     */
    void retro_run(void) {
        input_poll_cb();

        nbaVideoDevice->setLogCallback(log_cb);
        nbaVideoDevice->setVideoCallback(video_cb);
        nbaInputDevice->setInputCallback(input_state_cb, current_controller_port, current_controller_device);

#ifdef NBA_LIBRETRO_LIMIT_FRAMES
        frameLimiter->Run([&] {
            render_frame();
        }, [&](int) {
            // Allows to display current FPS somewhere
        });
#else
        render_frame();
#endif

#ifndef NBA_LIBRETRO_ASYNC_AUDIO
        render_audio(audio_batch_cb);
#endif

        nbaVideoDevice->setVideoCallback(nullptr);
        nbaInputDevice->setInputCallback(nullptr, 0, 0);
    }

    /**
     * libretro callback; Called when a game is to be loaded.
     */
    bool retro_load_game(const struct retro_game_info *info) {
        nba_libretro::fs::path biosLookupPaths[2];

        const char *systemDir = nullptr;
        if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &systemDir)) {
            nba_libretro::fs::path path = nba_libretro::fs::path(systemDir) / "NanoboyAdvance";
            biosLookupPaths[0] = path;
            nba_libretro::ensureDirExists(path);
        } else {
            biosLookupPaths[0] = nba_libretro::fs::path();
        }

        const char *homeFolder = getenv("HOME");
        log_cb(retro_log_level::RETRO_LOG_INFO, "Home folder: %s\n", homeFolder);
        if (homeFolder != nullptr && strlen(homeFolder) > 0) {
            nba_libretro::fs::path path = nba_libretro::fs::path(homeFolder) / ".NanoboyAdvance";
            biosLookupPaths[1] = path;
            nba_libretro::ensureDirExists(biosLookupPaths[1]);
        } else {
            biosLookupPaths[1] = nba_libretro::fs::path();
        }

        nba_libretro::fs::path biosPath = nba_libretro::findFirstMatchingFile("gba_bios.bin", biosLookupPaths, 2);
        if (biosPath.empty()) {
            log_cb(retro_log_level::RETRO_LOG_ERROR, "No bios found\n");
            return false;
        }
        nbaConfig->bios_path = biosPath.string();

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
                break;
            }
            default: {
                log_cb(retro_log_level::RETRO_LOG_ERROR, "An unexpected error occurred\n");
                return false;
            }
        }

#ifdef NBA_LIBRETRO_ASYNC_AUDIO
        if (!nba_audio_running) {
            nba_audio_running = true;
            audioThread = std::make_unique<std::thread>([]() {
                while (nba_audio_running) {
                    auto local_audio_cb = audio_batch_cb;
                    if (local_audio_cb == nullptr) {
                        // Skip
                        continue;
                    }
                    render_audio(local_audio_cb);
                }
            });
        }
#endif

        return true;
    }

    void retro_unload_game(void) {
#ifdef NBA_LIBRETRO_ASYNC_AUDIO
        nba_audio_running = false;
        audioThread->join();
#endif
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