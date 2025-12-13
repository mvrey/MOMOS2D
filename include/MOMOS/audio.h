/* Copyright 2017-2025. All rights reserved.           _____   ________      _____   ________     _________
* Audio utilities (powered by miniaudio)             /     \  \_____  \    /     \  \_____  \   /   _____/
*                                                   /  \ /  \  /   |   \  /  \ /  \  /   |   \  \_____  \
* Author: Marcos Vazquez <mrkvr84@gmail.com>       /    Y    \/    |    \/    Y    \/    |    \ /        \
*                                                \____|____/\_________/\____|____/\_________//_________/
*/

#ifndef MOMOS_AUDIO_H
#define MOMOS_AUDIO_H 1

namespace MOMOS {

// Initialize audio engine (safe to call multiple times)
bool AudioInit();
// Shutdown and release audio resources
void AudioShutdown();

// Play a one-shot sound effect (non-looping). Returns false on failure.
bool PlaySound(const char* file, float volume = 1.0f);

// Play (or restart) a looping music track. Returns false on failure.
bool PlayMusicLoop(const char* file, float volume = 1.0f);
// Stop current music if any.
void StopMusic();

} // namespace MOMOS

#endif // MOMOS_AUDIO_H
