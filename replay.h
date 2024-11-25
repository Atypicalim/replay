
#ifndef REPLAY_HEADER_H
#define REPLAY_HEADER_H

/////////////////////////////////////////////////////////////////////
// Defines and Macros
/////////////////////////////////////////////////////////////////////

#define RA_SUPPORT_FILEFORMAT_WAV
#define RA_SUPPORT_FILEFORMAT_OGG
#define RA_SUPPORT_FILEFORMAT_MP3

#ifndef RA_DEBUG_ENABLED
#define RA_DEBUG_MODE TRUE
#else
#define RA_DEBUG_MODE FALSE
#endif

/////////////////////////////////////////////////////////////////////
// memory allocators
/////////////////////////////////////////////////////////////////////

#ifndef REPLAY_MALLOC
    #define REPLAY_MALLOC(sz)           malloc(sz)
#endif
#ifndef REPLAY_CALLOC
    #define REPLAY_CALLOC(n,sz)         calloc(n,sz)
#endif
#ifndef REPLAY_REALLOC
    #define REPLAY_REALLOC(ptr,sz)      realloc(ptr,sz)
#endif
#ifndef REPLAY_FREE
    #define REPLAY_FREE(ptr)            free(ptr)
#endif

#ifndef REPLAY_TRACE
    #define REPLAY_TRACE(level, ...)    printf(__VA_ARGS__)
#endif

/////////////////////////////////////////////////////////////////////
// Types and Structures Definition
/////////////////////////////////////////////////////////////////////

#if (defined(__STDC__) && __STDC_VERSION__ >= 199901L) || (defined(_MSC_VER) && _MSC_VER >= 1800)
    #include <stdbool.h>
#elif !defined(__cplusplus) && !defined(bool)
    typedef enum bool { false = 0, true = !false } bool;
    #define RL_BOOL_TYPE
#endif

/////////////////////////////////////////////////////////////////////
// types definitions
/////////////////////////////////////////////////////////////////////

typedef void (*ReplayCallback)(void *bufferData, unsigned int frames);
typedef struct RSound RSound;
typedef struct RMusic RMusic;


/////////////////////////////////////////////////////////////////////
// interface definitions
/////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

// device
void replay_InitAudioDevice(void);                                     // Initialize audio device and context
void replay_CloseAudioDevice(void);                                    // Close the audio device and context
bool replay_IsAudioDeviceReady(void);                                  // Check if audio device has been initialized successfully
void replay_SetMasterVolume(float volume);                             // Set master volume (listener)
float replay_GetMasterVolume(void);                                    // Get master volume (listener)

// sound
RSound replay_LoadSound(const char *fileName);                          // Load sound from file
bool replay_IsSoundReady(RSound sound);                                 // Checks if a sound is ready
void replay_UpdateSound(RSound sound, const void *data, int frameCount);// Update sound buffer with new data
void replay_UnloadSound(RSound sound);                                  // Unload sound

// music
RMusic replay_LoadMusic(const char *fileName);                    // Load music stream from file
bool replay_IsMusicReady(RMusic music);                                 // Checks if a music stream is ready
void replay_UpdateMusic(RMusic music);                            // Updates buffers for music streaming
void replay_UnloadMusic(RMusic music);                            // Unload music stream

// Sound management functions
void replay_PlaySound(RSound sound);                                    // Play a sound
void replay_StopSound(RSound sound);                                    // Stop playing a sound
void replay_PauseSound(RSound sound);                                   // Pause a sound
void replay_ResumeSound(RSound sound);                                  // Resume a paused sound
bool replay_IsSoundPlaying(RSound sound);                               // Check if a sound is currently playing
void replay_SetSoundVolume(RSound sound, float volume);                 // Set volume for a sound (1.0 is max level)

// Music management functions
void replay_PlayMusic(RMusic music);                              // Start music playing
void replay_StopMusic(RMusic music);                              // Stop music playing
void replay_PauseMusic(RMusic music);                             // Pause music playing
void replay_ResumeMusic(RMusic music);                            // Resume playing paused music
bool replay_IsMusicPlaying(RMusic music);                         // Check if music is playing
void replay_SetMusicVolume(RMusic music, float volume);                 // Set volume for music (1.0 is max level)

// time related
float replay_GetMusicLength(RMusic music);                          // Get music time length (in seconds)
float replay_GetMusicTime(RMusic music);                          // Get current music time played (in seconds)
void replay_SetMusicTime(RMusic music, float position);              // Seek music to a position (in seconds)

// sound related
void replay_SetSoundPitch(RSound sound, float pitch);                   // Set pitch for a sound (1.0 is base level)
void replay_SetSoundPan(RSound sound, float pan);                       // Set pan for a sound (0.0 to 1.0, 0.5=center)
// music related
void replay_SetMusicPitch(RMusic music, float pitch);                   // Set pitch for a music (1.0 is base level)
void replay_SetMusicPan(RMusic music, float pan);                       // Set pan for a music (0.0 to 1.0, 0.5=center)

/////////////////////////////////////////////////////////////////////

// include implementation

#ifndef REPLAY_NO_IMLEMENT
    #include "replay.c"
#endif

#ifdef __cplusplus
}
#endif

#endif // REPLAY_HEADER_H
