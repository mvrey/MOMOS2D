#define MINIAUDIO_IMPLEMENTATION
#include "../include/miniaudio.h"

#include <MOMOS/audio.h>
#include <cstdio>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace {

ma_engine g_audio_engine;
bool g_audio_initialized = false;
ma_sound g_music_sound;
bool g_music_active = false;
std::vector<std::unique_ptr<ma_sound>> g_active_sounds;

std::string ResolveAudioPath(const char* requested) {
	if (!requested) {
		return std::string();
	}

	auto exists = [](const std::string& path) -> bool {
		std::ifstream f(path, std::ios::binary);
		return f.good();
	};

	std::string candidate = requested;
	if (exists(candidate)) {
		return candidate;
	}

	const char* prefixes[] = {
		"",            // as-is (already tried)
		"data/",
		"../data/",
		"deps/",
		"../"
	};

	for (const char* prefix : prefixes) {
		std::string prefixed = std::string(prefix) + requested;
		if (exists(prefixed)) {
			return prefixed;
		}
	}

	return candidate; // fallback
}

void CleanupFinishedSounds() {
	for (auto it = g_active_sounds.begin(); it != g_active_sounds.end();) {
		if (!ma_sound_is_playing(it->get())) {
			ma_sound_uninit(it->get());
			it = g_active_sounds.erase(it);
		} else {
			++it;
		}
	}
}

} // namespace

namespace MOMOS {

bool AudioInit() {
	if (g_audio_initialized) {
		return true;
	}

	ma_result result = ma_engine_init(nullptr, &g_audio_engine);
	if (result != MA_SUCCESS) {
		printf("[Audio] Failed to initialize engine (code %d)\n", result);
		return false;
	}

	g_audio_initialized = true;
	return true;
}

void StopMusic() {
	if (g_music_active) {
		ma_sound_uninit(&g_music_sound);
		g_music_active = false;
	}
}

void AudioShutdown() {
	if (!g_audio_initialized) {
		return;
	}

	StopMusic();
	for (auto& sound : g_active_sounds) {
		if (sound) {
			ma_sound_uninit(sound.get());
		}
	}
	g_active_sounds.clear();
	ma_engine_uninit(&g_audio_engine);
	g_audio_initialized = false;
}

bool PlaySound(const char* file, float volume) {
	if (!g_audio_initialized && !AudioInit()) {
		return false;
	}

	CleanupFinishedSounds();

	std::string path = ResolveAudioPath(file);
	auto sound = std::make_unique<ma_sound>();
	ma_result result = ma_sound_init_from_file(&g_audio_engine, path.c_str(), 0, nullptr, nullptr, sound.get());
	if (result != MA_SUCCESS) {
		printf("[Audio] Failed to play sound '%s' (code %d)\n", path.c_str(), result);
		return false;
	}

	ma_sound_set_volume(sound.get(), volume);
	result = ma_sound_start(sound.get());
	if (result != MA_SUCCESS) {
		printf("[Audio] Failed to start sound '%s' (code %d)\n", path.c_str(), result);
		ma_sound_uninit(sound.get());
		return false;
	}

	g_active_sounds.push_back(std::move(sound));
	return true;
}

bool PlayMusicLoop(const char* file, float volume) {
	if (!g_audio_initialized && !AudioInit()) {
		return false;
	}

	StopMusic();

	std::string path = ResolveAudioPath(file);
	ma_uint32 flags = MA_SOUND_FLAG_STREAM; // stream for large files (mp3)
	ma_result result = ma_sound_init_from_file(&g_audio_engine, path.c_str(), flags, nullptr, nullptr, &g_music_sound);
	if (result != MA_SUCCESS) {
		printf("[Audio] Failed to load music '%s' (code %d)\n", path.c_str(), result);
		return false;
	}

	ma_sound_set_looping(&g_music_sound, MA_TRUE);
	ma_sound_set_volume(&g_music_sound, volume);

	result = ma_sound_start(&g_music_sound);
	if (result != MA_SUCCESS) {
		printf("[Audio] Failed to start music '%s' (code %d)\n", path.c_str(), result);
		ma_sound_uninit(&g_music_sound);
		return false;
	}

	g_music_active = true;
	return true;
}

} // namespace MOMOS
