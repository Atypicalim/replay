
# Replay

### 0. Description

> a simple wrap for miniaudio to play sounds and music, you can use it in your game project to play audio files.

### 1. Usage

> load a music and play

```c
// init audio device
replay_InitAudioDevice();
// load and play
RMusic music = replay_LoadMusic("resources/country.mp3");
replay_PlayMusic(music);
// loop and update
while (1) {
    replay_UpdateMusic(music);
}
// free after finish
replay_UnloadMusic(music);
replay_CloseAudioDevice();
```

> for more info please visit `replay.h` file
 
### 2. alternatives
 
- [raudio: simple and easy-to-use audio library based on miniaudio](https://github.com/raysan5/raudio)

- [miniaudio: a single file library for audio playback and capture.](https://github.com/mackron/miniaudio)
