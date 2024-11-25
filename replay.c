
/////////////////////////////////////////////////////////////////////

#ifndef REPLAY_SOURCE_H
#define REPLAY_SOURCE_H

#include "replay.h"

/////////////////////////////////////////////////////////////////////

#include <stdlib.h>                     // Required for: malloc(), free()
#include <stdio.h>                      // Required for: FILE, fopen(), fclose(), fread()
#include <string.h>                     // Required for: strcmp() [Used in _replay_IsFileExtension(), rwave_LoadWaveFromMemory(), _replay_LoadMusicFromMemory()]

#if defined(_WIN32)
// To avoid conflicting windows.h symbols with raylib, some flags are defined
// WARNING: Those flags avoid inclusion of some Win32 headers that could be required
// by user at some point and won't be included...
//-------------------------------------------------------------------------------------

#ifndef RA_NO_WIN_EXTRA
// If defined, the following flags inhibit definition of the indicated items.
#define NOGDICAPMASKS     // CC_*, LC_*, PC_*, CP_*, TC_*, RC_
#define NOVIRTUALKEYCODES // VK_*
#define NOWINMESSAGES     // WM_*, EM_*, LB_*, CB_*
#define NOWINSTYLES       // WS_*, CS_*, ES_*, LBS_*, SBS_*, CBS_*
#define NOSYSMETRICS      // SM_*
#define NOMENUS           // MF_*
#define NOICONS           // IDI_*
#define NOKEYSTATES       // MK_*
#define NOSYSCOMMANDS     // SC_*
#define NORASTEROPS       // Binary and Tertiary raster ops
#define NOSHOWWINDOW      // SW_*
#define OEMRESOURCE       // OEM Resource values
#define NOATOM            // Atom Manager routines
#define NOCLIPBOARD       // Clipboard routines
#define NOCOLOR           // Screen colors
#define NOCTLMGR          // Control and Dialog routines
#define NODRAWTEXT        // DrawText() and DT_*
#define NOGDI             // All GDI defines and routines
#define NOKERNEL          // All KERNEL defines and routines
#define NOUSER            // All USER defines and routines
//#define NONLS             // All NLS defines and routines
#define NOMB              // MB_* and MessageBox()
#define NOMEMMGR          // GMEM_*, LMEM_*, GHND, LHND, associated routines
#define NOMETAFILE        // typedef METAFILEPICT
#define NOMINMAX          // Macros min(a,b) and max(a,b)
#define NOMSG             // typedef MSG and associated routines
#define NOOPENFILE        // OpenFile(), OemToAnsi, AnsiToOem, and OF_*
#define NOSCROLL          // SB_* and scrolling routines
#define NOSERVICE         // All Service Controller routines, SERVICE_ equates, etc.
#define NOSOUND           // Sound driver routines
#define NOTEXTMETRIC      // typedef TEXTMETRIC and associated routines
#define NOWH              // SetWindowsHook and WH_*
#define NOWINOFFSETS      // GWL_*, GCL_*, associated routines
#define NOCOMM            // COMM driver routines
#define NOKANJI           // Kanji support stuff.
#define NOHELP            // Help engine interface.
#define NOPROFILER        // Profiler interface.
#define NODEFERWINDOWPOS  // DeferWindowPos routines
#define NOMCX             // Modem Configuration Extensions
#endif

// Type required before windows.h inclusion
#ifndef RA_NO_WIN_USER_MSG
#if !defined(_WINUSER_) || defined(NOUSER) || defined(NOMSG)
typedef struct tagMSG *LPMSG;
#endif
#endif

#include <windows.h>        // Windows functionality (miniaudio)

// Type required by some unused function...
#ifndef RA_NO_WIN_BIRMAP_INFO
#if !defined(_WINGDI_) || defined(NOGDI)
typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
#endif
#endif

#ifndef RA_NO_WIN_MEDIA_INCLUDE
#include <objbase.h>        // Component Object Model (COM) header
#include <mmreg.h>          // Windows Multimedia, defines some WAVE structs
#include <mmsystem.h>       // Windows Multimedia, used by Windows GDI, defines DIBINDEX macro
#endif

// Some required types defined for MSVC/TinyC compiler
#if defined(_MSC_VER) || defined(__TINYC__)
    #include "propidl.h"
#endif
#endif

/////////////////////////////////////////////////////////////////////

#define MA_MALLOC REPLAY_MALLOC
#define MA_FREE REPLAY_FREE

#define MA_NO_JACK
#define MA_NO_WAV
#define MA_NO_FLAC
#define MA_NO_MP3
#define MA_NO_RESOURCE_MANAGER
#define MA_NO_NODE_GRAPH
#define MA_NO_ENGINE
#define MA_NO_GENERATION

#define MA_NO_DSOUND
#define MA_NO_WASAPI

#define MA_NO_DECODING
#define MA_NO_ENCODING

#define MA_NO_SSE2
#define MA_NO_AVX2
#define MA_NO_NEON

#if RA_DEBUG_MODE
#define MA_DEBUG_OUTPUT
#endif

#ifndef MINIAUDIO_IMPLEMENTATION
#define MINIAUDIO_IMPLEMENTATION
#include "others/miniaudio.h"         // Audio device initialization and management
#endif

/////////////////////////////////////////////////////////////////////

#if defined(RA_SUPPORT_FILEFORMAT_WAV)
    #define DRWAV_MALLOC REPLAY_MALLOC
    #define DRWAV_REALLOC REPLAY_REALLOC
    #define DRWAV_FREE REPLAY_FREE

    #define DR_WAV_IMPLEMENTATION
    #include "others/dr_wav.h"        // WAV loading functions
#endif

#if defined(RA_SUPPORT_FILEFORMAT_OGG)
    // TODO: Remap stb_vorbis malloc()/free() calls to REPLAY_MALLOC/REPLAY_FREE
    #include "others/stb_vorbis.c"    // OGG loading functions
#endif

#if defined(RA_SUPPORT_FILEFORMAT_MP3)
    #define DRMP3_MALLOC REPLAY_MALLOC
    #define DRMP3_REALLOC REPLAY_REALLOC
    #define DRMP3_FREE REPLAY_FREE

    #define DR_MP3_IMPLEMENTATION
    #include "others/dr_mp3.h"        // MP3 loading functions
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#ifndef RA_AUDIO_DEVICE_FORMAT
    #define RA_AUDIO_DEVICE_FORMAT    ma_format_f32    // Device output format (float-32bit)
#endif
#ifndef RA_AUDIO_DEVICE_CHANNELS
    #define RA_AUDIO_DEVICE_CHANNELS              2    // Device output channels: stereo
#endif
#ifndef RA_AUDIO_DEVICE_SAMPLE_RATE
    #define RA_AUDIO_DEVICE_SAMPLE_RATE           0    // Device output sample rate
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// Music context type
// NOTE: Depends on data structure provided by the library
// in charge of reading the different file types
typedef enum {
    RMUSIC_AUDIO_NONE = 0,   // No audio context loaded
    RMUSIC_AUDIO_WAV,        // WAV audio context
    RMUSIC_AUDIO_OGG,        // OGG audio context
    RMUSIC_AUDIO_MP3,        // MP3 audio context
} RMusicContextType;

// NOTE: Different logic is used when feeding data to the playback device
// depending on whether data is streamed (RMusic vs Sound)
typedef enum {
    REPLAY_BUFFER_STATIC = 0,
    REPLAY_BUFFER_STREAM
} AudioBufferUsage;


// Opaque structs declaration
typedef struct ReplayBuffer ReplayBuffer;
typedef struct ReplayProcessor ReplayProcessor;

// Audio processor struct
// NOTE: Useful to apply effects to an ReplayBuffer
struct ReplayProcessor {
    ReplayCallback process;          // Processor callback function
    ReplayProcessor *next;          // Next audio processor on the list
    ReplayProcessor *prev;          // Previous audio processor on the list
};

// Audio buffer struct
struct ReplayBuffer {
    ma_data_converter converter;    // Audio data converter

    ReplayCallback callback;         // Audio buffer callback for buffer filling on audio threads
    ReplayProcessor *processor;     // Audio processor

    float volume;                   // Audio buffer volume
    float pitch;                    // Audio buffer pitch
    float pan;                      // Audio buffer pan (0.0f to 1.0f)

    bool playing;                   // Audio buffer state: AUDIO_PLAYING
    bool paused;                    // Audio buffer state: AUDIO_PAUSED
    bool looping;                   // Audio buffer looping, default to true for AudioStreams
    int usage;                      // Audio buffer usage mode: STATIC or STREAM

    bool isSubBufferProcessed[2];   // SubBuffer processed (virtual double buffer)
    unsigned int sizeInFrames;      // Total buffer size in frames
    unsigned int frameCursorPos;    // Frame cursor position
    unsigned int framesProcessed;   // Total frames processed in this buffer (required for play timing)

    unsigned char *data;            // Data buffer, on music stream keeps filling

    ReplayBuffer *next;             // Next audio buffer on the list
    ReplayBuffer *prev;             // Previous audio buffer on the list
};

// ReplayWave, audio wave data
typedef struct ReplayWave {
    unsigned int frameCount;    // Total number of frames (considering channels)
    unsigned int sampleRate;    // Frequency (samples per second)
    unsigned int sampleSize;    // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
    unsigned int channels;      // Number of channels (1-mono, 2-stereo, ...)
    void *data;                 // Buffer data pointer
} ReplayWave;

// ReplayStream, custom audio stream
typedef struct ReplayStream {
    ReplayBuffer *buffer;       // Pointer to internal data used by the audio system
    ReplayProcessor *processor; // Pointer to internal data processor, useful for audio effects

    unsigned int sampleRate;    // Frequency (samples per second)
    unsigned int sampleSize;    // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
    unsigned int channels;      // Number of channels (1-mono, 2-stereo, ...)
} ReplayStream;

// Sound
typedef struct RSound {
    ReplayStream stream;         // Audio stream
    unsigned int frameCount;    // Total number of frames (considering channels)
} RSound;

// Music, audio stream, anything longer than ~10 seconds should be streamed
typedef struct RMusic {
    ReplayStream stream;         // Audio stream
    unsigned int frameCount;    // Total number of frames (considering channels)
    bool looping;               // Music looping enable
    int ctxType;                // Type of music context (audio filetype)
    void *ctxData;              // Audio context data, depends on type
} RMusic;

// Audio data context
typedef struct ReplayData {
    struct {
        ma_context context;         // miniaudio context data
        ma_device device;           // miniaudio device
        ma_mutex lock;              // miniaudio mutex lock
        bool isReady;               // Check if audio device is ready
        size_t pcmBufferSize;       // Pre-allocated buffer size
        void *pcmBuffer;            // Pre-allocated buffer to read audio data from file/memory
    } System;
    struct {
        ReplayBuffer *first;         // Pointer to first ReplayBuffer in the list
        ReplayBuffer *last;          // Pointer to last ReplayBuffer in the list
        int defaultSize;            // Default audio buffer size for audio streams
    } Buffer;
    ReplayProcessor *mixedProcessor;
} ReplayData;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static ReplayData REPLAY = {          // Global REPLAY context

    // NOTE: Music buffer size is defined by number of samples, independent of sample size and channels number
    // After some math, considering a sampleRate of 48000, a buffer refill rate of 1/60 seconds and a
    // standard double-buffering system, a 4096 samples buffer has been chosen, it should be enough
    // In case of music-stalls, just increase this number
    .Buffer.defaultSize = 0,
    .mixedProcessor = NULL
};


//----------------------------------------------------------------------------------
// Func Definition
//----------------------------------------------------------------------------------

// ReplayWave/Sound loading/unloading functions
ReplayWave rwave_LoadWave(const char *fileName);                            // Load wave data from file
ReplayWave rwave_LoadWaveFromMemory(const char *fileType, const unsigned char *fileData, int dataSize); // Load wave from memory buffer, fileType refers to extension: i.e. ".wav"
void rwave_UnloadWave(ReplayWave wave);                                     // Unload wave data

/////////////////////////////////////////////////////////////////////

// ReplayBuffer management functions declaration
ReplayBuffer *rbuffer_LoadBuffer(ma_format format, ma_uint32 channels, ma_uint32 sampleRate, ma_uint32 sizeInFrames, int usage);
void rbuffer_UnloadBuffer(ReplayBuffer *buffer);
void rbuffer_PlayBuffer(ReplayBuffer *buffer);
void rbuffer_StopBuffer(ReplayBuffer *buffer);
void rbuffer_PauseBuffer(ReplayBuffer *buffer);
void rbuffer_ResumeBuffer(ReplayBuffer *buffer);
bool rbuffer_IsBufferPlaying(ReplayBuffer *buffer);
void rbuffer_SetBufferVolume(ReplayBuffer *buffer, float volume);
void rbuffer_SetBufferPitch(ReplayBuffer *buffer, float pitch);
void rbuffer_SetBufferPan(ReplayBuffer *buffer, float pan);
//
void rbuffer_TrackBuffer(ReplayBuffer *buffer);
void rbuffer_UntrackBuffer(ReplayBuffer *buffer);

/////////////////////////////////////////////////////////////////////

// ReplayStream management functions
ReplayStream rstream_LoadStream(unsigned int sampleRate, unsigned int sampleSize, unsigned int channels); // Load audio stream (to stream raw audio pcm data)
bool rstream_IsStreamReady(ReplayStream stream);                    // Checks if an audio stream is ready
void rstream_UpdateStream(ReplayStream stream, const void *data, int samplesCount); // Update audio stream buffers with data
void rstream_UnloadStream(ReplayStream stream);                     // Unload audio stream and free memory
// 
void rstream_playStream(ReplayStream stream);                       // Play audio stream
void rstream_StopStream(ReplayStream stream);                       // Stop audio stream
void rstream_PauseStream(ReplayStream stream);                      // Pause audio stream
void rstream_ResumeStream(ReplayStream stream);                     // Resume audio stream
bool rstream_IsStreamPlaying(ReplayStream stream);                  // Check if audio stream is playing
void rstream_SetStreamVolume(ReplayStream stream, float volume);    // Set volume for audio stream (1.0 is max level)
void rstream_SetStreamPitch(ReplayStream stream, float pitch);      // Set pitch for audio stream (1.0 is base level)
void rstream_SetStreamPan(ReplayStream strean, float pan);          // Set pan for audio stream  (0.0 to 1.0, 0.5=center)
//
bool rstream_IsStreamProcessed(ReplayStream stream);                // Check if any audio stream buffers requires refill

/////////////////////////////////////////////////////////////////////

// other
RSound _replay_LoadSoundFromWave(ReplayWave wave);                             // Load sound from wave data
RMusic _replay_LoadMusicFromMemory(const char *fileType, const unsigned char* data, int dataSize); // Load music stream from data

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
static void _replay_OnAudioLog(void *pUserData, ma_uint32 level, const char *pMessage);
static void _replay_OnSendData(ma_device *pDevice, void *pFramesOut, const void *pFramesInput, ma_uint32 frameCount);
static void _replay_MixAudioFrames(float *framesOut, const float *framesIn, ma_uint32 frameCount, ReplayBuffer *buffer);

static bool _replay_IsFileExtension(const char *fileName, const char *ext); // Check file extension
static const char *_replay_GetFileExtension(const char *fileName);          // Get pointer to extension for a filename string (includes the dot: .png)
static unsigned char *_replay_LoadFileData(const char *fileName, int *dataSize);    // Load file data as byte array (read)

//----------------------------------------------------------------------------------
// Module Functions Definition - Audio Device initialization and Closing
//----------------------------------------------------------------------------------
// Initialize audio device
void replay_InitAudioDevice(void)
{
    // Init audio context
    ma_context_config ctxConfig = ma_context_config_init();
    ma_log_callback_init(_replay_OnAudioLog, NULL);

    ma_result result = ma_context_init(NULL, 0, &ctxConfig, &REPLAY.System.context);
    if (result != MA_SUCCESS)
    {
        REPLAY_TRACE(LOG_WARNING, "REPLAY: Failed to initialize context");
        return;
    }

    // Init audio device
    // NOTE: Using the default device. Format is floating point because it simplifies mixing.
    ma_device_config config = ma_device_config_init(ma_device_type_playback);
    config.playback.pDeviceID = NULL;  // NULL for the default playback REPLAY.System.device.
    config.playback.format = RA_AUDIO_DEVICE_FORMAT;
    config.playback.channels = RA_AUDIO_DEVICE_CHANNELS;
    config.capture.pDeviceID = NULL;  // NULL for the default capture REPLAY.System.device.
    config.capture.format = ma_format_s16;
    config.capture.channels = 1;
    config.sampleRate = RA_AUDIO_DEVICE_SAMPLE_RATE;
    config.dataCallback = _replay_OnSendData;
    config.pUserData = NULL;

    result = ma_device_init(&REPLAY.System.context, &config, &REPLAY.System.device);
    if (result != MA_SUCCESS)
    {
        REPLAY_TRACE(LOG_WARNING, "REPLAY: Failed to initialize playback device");
        ma_context_uninit(&REPLAY.System.context);
        return;
    }

    // Mixing happens on a separate thread which means we need to synchronize. I'm using a mutex here to make things simple, but may
    // want to look at something a bit smarter later on to keep everything real-time, if that's necessary.
    if (ma_mutex_init(&REPLAY.System.lock) != MA_SUCCESS)
    {
        REPLAY_TRACE(LOG_WARNING, "REPLAY: Failed to create mutex for mixing");
        ma_device_uninit(&REPLAY.System.device);
        ma_context_uninit(&REPLAY.System.context);
        return;
    }

    // Keep the device running the whole time. May want to consider doing something a bit smarter and only have the device running
    // while there's at least one sound being played.
    result = ma_device_start(&REPLAY.System.device);
    if (result != MA_SUCCESS)
    {
        REPLAY_TRACE(LOG_WARNING, "REPLAY: Failed to start playback device");
        ma_device_uninit(&REPLAY.System.device);
        ma_context_uninit(&REPLAY.System.context);
        return;
    }

    REPLAY_TRACE(LOG_INFO, "REPLAY: Device initialized successfully");
    REPLAY_TRACE(LOG_INFO, "    > Backend:       miniaudio / %s", ma_get_backend_name(REPLAY.System.context.backend));
    REPLAY_TRACE(LOG_INFO, "    > Format:        %s -> %s", ma_get_format_name(REPLAY.System.device.playback.format), ma_get_format_name(REPLAY.System.device.playback.internalFormat));
    REPLAY_TRACE(LOG_INFO, "    > Channels:      %d -> %d", REPLAY.System.device.playback.channels, REPLAY.System.device.playback.internalChannels);
    REPLAY_TRACE(LOG_INFO, "    > Sample rate:   %d -> %d", REPLAY.System.device.sampleRate, REPLAY.System.device.playback.internalSampleRate);
    REPLAY_TRACE(LOG_INFO, "    > Periods size:  %d", REPLAY.System.device.playback.internalPeriodSizeInFrames*REPLAY.System.device.playback.internalPeriods);

    REPLAY.System.isReady = true;
}

// Close the audio device for all contexts
void replay_CloseAudioDevice(void)
{
    if (REPLAY.System.isReady)
    {
        ma_mutex_uninit(&REPLAY.System.lock);
        ma_device_uninit(&REPLAY.System.device);
        ma_context_uninit(&REPLAY.System.context);

        REPLAY.System.isReady = false;
        REPLAY_FREE(REPLAY.System.pcmBuffer);
        REPLAY.System.pcmBuffer = NULL;
        REPLAY.System.pcmBufferSize = 0;

        REPLAY_TRACE(LOG_INFO, "REPLAY: Device closed successfully");
    }
    else REPLAY_TRACE(LOG_WARNING, "REPLAY: Device could not be closed, not currently initialized");
}

// Check if device has been initialized successfully
bool replay_IsAudioDeviceReady(void)
{
    return REPLAY.System.isReady;
}

// Set master volume (listener)
void replay_SetMasterVolume(float volume)
{
    ma_device_set_master_volume(&REPLAY.System.device, volume);
}

// Get master volume (listener)
float replay_GetMasterVolume(void)
{
    float volume = 0.0f;
    ma_device_get_master_volume(&REPLAY.System.device, &volume);
    return volume;
}

//----------------------------------------------------------------------------------
// Module Functions Definition - Audio Buffer management
//----------------------------------------------------------------------------------

// Initialize a new audio buffer (filled with silence)
ReplayBuffer *rbuffer_LoadBuffer(ma_format format, ma_uint32 channels, ma_uint32 sampleRate, ma_uint32 sizeInFrames, int usage)
{
    ReplayBuffer *audioBuffer = (ReplayBuffer *)REPLAY_CALLOC(1, sizeof(ReplayBuffer));

    if (audioBuffer == NULL)
    {
        REPLAY_TRACE(LOG_WARNING, "REPLAY: Failed to allocate memory for buffer");
        return NULL;
    }

    if (sizeInFrames > 0) audioBuffer->data = REPLAY_CALLOC(sizeInFrames*channels*ma_get_bytes_per_sample(format), 1);

    // Audio data runs through a format converter
    ma_data_converter_config converterConfig = ma_data_converter_config_init(format, RA_AUDIO_DEVICE_FORMAT, channels, RA_AUDIO_DEVICE_CHANNELS, sampleRate, REPLAY.System.device.sampleRate);
    converterConfig.allowDynamicSampleRate = true;

    ma_result result = ma_data_converter_init(&converterConfig, NULL, &audioBuffer->converter);

    if (result != MA_SUCCESS)
    {
        REPLAY_TRACE(LOG_WARNING, "REPLAY: Failed to create data conversion pipeline");
        REPLAY_FREE(audioBuffer);
        return NULL;
    }

    // Init audio buffer values
    audioBuffer->volume = 1.0f;
    audioBuffer->pitch = 1.0f;
    audioBuffer->pan = 0.5f;

    audioBuffer->callback = NULL;
    audioBuffer->processor = NULL;

    audioBuffer->playing = false;
    audioBuffer->paused = false;
    audioBuffer->looping = false;

    audioBuffer->usage = usage;
    audioBuffer->frameCursorPos = 0;
    audioBuffer->sizeInFrames = sizeInFrames;

    // Buffers should be marked as processed by default so that a call to
    // rstream_UpdateStream() immediately after initialization works correctly
    audioBuffer->isSubBufferProcessed[0] = true;
    audioBuffer->isSubBufferProcessed[1] = true;

    // Track audio buffer to linked list next position
    rbuffer_TrackBuffer(audioBuffer);

    return audioBuffer;
}

// Delete an audio buffer
void rbuffer_UnloadBuffer(ReplayBuffer *buffer)
{
    if (buffer != NULL)
    {
        ma_data_converter_uninit(&buffer->converter, NULL);
        rbuffer_UntrackBuffer(buffer);
        REPLAY_FREE(buffer->data);
        REPLAY_FREE(buffer);
    }
}

// Play an audio buffer
// NOTE: Buffer is restarted to the start.
// Use rbuffer_PauseBuffer() and rbuffer_ResumeBuffer() if the playback position should be maintained.
void rbuffer_PlayBuffer(ReplayBuffer *buffer)
{
    if (buffer != NULL)
    {
        buffer->playing = true;
        buffer->paused = false;
        buffer->frameCursorPos = 0;
    }
}

// Stop an audio buffer
void rbuffer_StopBuffer(ReplayBuffer *buffer)
{
    if (buffer != NULL)
    {
        if (rbuffer_IsBufferPlaying(buffer))
        {
            buffer->playing = false;
            buffer->paused = false;
            buffer->frameCursorPos = 0;
            buffer->framesProcessed = 0;
            buffer->isSubBufferProcessed[0] = true;
            buffer->isSubBufferProcessed[1] = true;
        }
    }
}

// Pause an audio buffer
void rbuffer_PauseBuffer(ReplayBuffer *buffer)
{
    if (buffer != NULL) buffer->paused = true;
}

// Resume an audio buffer
void rbuffer_ResumeBuffer(ReplayBuffer *buffer)
{
    if (buffer != NULL) buffer->paused = false;
}

// Check if an audio buffer is playing
bool rbuffer_IsBufferPlaying(ReplayBuffer *buffer)
{
    bool result = false;

    if (buffer != NULL) result = (buffer->playing && !buffer->paused);

    return result;
}

// Set volume for an audio buffer
void rbuffer_SetBufferVolume(ReplayBuffer *buffer, float volume)
{
    if (buffer != NULL) buffer->volume = volume;
}

// Set pitch for an audio buffer
void rbuffer_SetBufferPitch(ReplayBuffer *buffer, float pitch)
{
    if ((buffer != NULL) && (pitch > 0.0f))
    {
        // Pitching is just an adjustment of the sample rate.
        // Note that this changes the duration of the sound:
        //  - higher pitches will make the sound faster
        //  - lower pitches make it slower
        ma_uint32 outputSampleRate = (ma_uint32)((float)buffer->converter.sampleRateOut/pitch);
        ma_data_converter_set_rate(&buffer->converter, buffer->converter.sampleRateIn, outputSampleRate);

        buffer->pitch = pitch;
    }
}

// Set pan for an audio buffer
void rbuffer_SetBufferPan(ReplayBuffer *buffer, float pan)
{
    if (pan < 0.0f) pan = 0.0f;
    else if (pan > 1.0f) pan = 1.0f;

    if (buffer != NULL) buffer->pan = pan;
}

// Track audio buffer to linked list next position
void rbuffer_TrackBuffer(ReplayBuffer *buffer)
{
    ma_mutex_lock(&REPLAY.System.lock);
    {
        if (REPLAY.Buffer.first == NULL) REPLAY.Buffer.first = buffer;
        else
        {
            REPLAY.Buffer.last->next = buffer;
            buffer->prev = REPLAY.Buffer.last;
        }

        REPLAY.Buffer.last = buffer;
    }
    ma_mutex_unlock(&REPLAY.System.lock);
}

// Untrack audio buffer from linked list
void rbuffer_UntrackBuffer(ReplayBuffer *buffer)
{
    ma_mutex_lock(&REPLAY.System.lock);
    {
        if (buffer->prev == NULL) REPLAY.Buffer.first = buffer->next;
        else buffer->prev->next = buffer->next;

        if (buffer->next == NULL) REPLAY.Buffer.last = buffer->prev;
        else buffer->next->prev = buffer->prev;

        buffer->prev = NULL;
        buffer->next = NULL;
    }
    ma_mutex_unlock(&REPLAY.System.lock);
}

//----------------------------------------------------------------------------------
// Module Functions Definition - Wave loading and playing
//----------------------------------------------------------------------------------

// Load wave data from file
ReplayWave rwave_LoadWave(const char *fileName)
{
    ReplayWave wave = { 0 };

    // Loading file to memory
    int dataSize = 0;
    unsigned char *fileData = _replay_LoadFileData(fileName, &dataSize);

    // Loading wave from memory data
    if (fileData != NULL) wave = rwave_LoadWaveFromMemory(_replay_GetFileExtension(fileName), fileData, dataSize);

    REPLAY_FREE(fileData);

    return wave;
}

// Load wave from memory buffer, fileType refers to extension: i.e. ".wav"
// WARNING: File extension must be provided in lower-case
ReplayWave rwave_LoadWaveFromMemory(const char *fileType, const unsigned char *fileData, int dataSize)
{
    ReplayWave wave = { 0 };

    if (false) { }
#if defined(RA_SUPPORT_FILEFORMAT_WAV)
    else if ((strcmp(fileType, ".wav") == 0) || (strcmp(fileType, ".WAV") == 0))
    {
        drwav wav = { 0 };
        bool success = drwav_init_memory(&wav, fileData, dataSize, NULL);

        if (success)
        {
            wave.frameCount = (unsigned int)wav.totalPCMFrameCount;
            wave.sampleRate = wav.sampleRate;
            wave.sampleSize = 16;
            wave.channels = wav.channels;
            wave.data = (short *)REPLAY_MALLOC(wave.frameCount*wave.channels*sizeof(short));

            // NOTE: We are forcing conversion to 16bit sample size on reading
            drwav_read_pcm_frames_s16(&wav, wav.totalPCMFrameCount, wave.data);
        }
        else REPLAY_TRACE(LOG_WARNING, "WAVE: Failed to load WAV data");

        drwav_uninit(&wav);
    }
#endif
#if defined(RA_SUPPORT_FILEFORMAT_OGG)
    else if ((strcmp(fileType, ".ogg") == 0) || (strcmp(fileType, ".OGG") == 0))
    {
        stb_vorbis *oggData = stb_vorbis_open_memory((unsigned char *)fileData, dataSize, NULL, NULL);

        if (oggData != NULL)
        {
            stb_vorbis_info info = stb_vorbis_get_info(oggData);

            wave.sampleRate = info.sample_rate;
            wave.sampleSize = 16;       // By default, ogg data is 16 bit per sample (short)
            wave.channels = info.channels;
            wave.frameCount = (unsigned int)stb_vorbis_stream_length_in_samples(oggData);  // NOTE: It returns frames!
            wave.data = (short *)REPLAY_MALLOC(wave.frameCount*wave.channels*sizeof(short));

            // NOTE: Get the number of samples to process (be careful! we ask for number of shorts, not bytes!)
            stb_vorbis_get_samples_short_interleaved(oggData, info.channels, (short *)wave.data, wave.frameCount*wave.channels);
            stb_vorbis_close(oggData);
        }
        else REPLAY_TRACE(LOG_WARNING, "WAVE: Failed to load OGG data");
    }
#endif
#if defined(RA_SUPPORT_FILEFORMAT_MP3)
    else if ((strcmp(fileType, ".mp3") == 0) || (strcmp(fileType, ".MP3") == 0))
    {
        drmp3_config config = { 0 };
        unsigned long long int totalFrameCount = 0;

        // NOTE: We are forcing conversion to 32bit float sample size on reading
        wave.data = drmp3_open_memory_and_read_pcm_frames_f32(fileData, dataSize, &config, &totalFrameCount, NULL);
        wave.sampleSize = 32;

        if (wave.data != NULL)
        {
            wave.channels = config.channels;
            wave.sampleRate = config.sampleRate;
            wave.frameCount = (int)totalFrameCount;
        }
        else REPLAY_TRACE(LOG_WARNING, "WAVE: Failed to load MP3 data");

    }
#endif
    else REPLAY_TRACE(LOG_WARNING, "WAVE: Data format not supported");

    REPLAY_TRACE(LOG_INFO, "WAVE: Data loaded successfully (%i Hz, %i bit, %i channels)", wave.sampleRate, wave.sampleSize, wave.channels);

    return wave;
}

// Unload wave data
void rwave_UnloadWave(ReplayWave wave)
{
    REPLAY_FREE(wave.data);
    //REPLAY_TRACE(LOG_INFO, "WAVE: Unloaded wave data from RAM");
}

//----------------------------------------------------------------------------------
// Module Functions Definition - Stream loading and playing
//----------------------------------------------------------------------------------

// Load audio stream (to stream audio pcm data)
ReplayStream rstream_LoadStream(unsigned int sampleRate, unsigned int sampleSize, unsigned int channels)
{
    ReplayStream stream = { 0 };

    stream.sampleRate = sampleRate;
    stream.sampleSize = sampleSize;
    stream.channels = channels;

    ma_format formatIn = ((stream.sampleSize == 8)? ma_format_u8 : ((stream.sampleSize == 16)? ma_format_s16 : ma_format_f32));

    // The size of a streaming buffer must be at least double the size of a period
    unsigned int periodSize = REPLAY.System.device.playback.internalPeriodSizeInFrames;

    // If the buffer is not set, compute one that would give us a buffer good enough for a decent frame rate
    unsigned int subBufferSize = (REPLAY.Buffer.defaultSize == 0)? REPLAY.System.device.sampleRate/30 : REPLAY.Buffer.defaultSize;

    if (subBufferSize < periodSize) subBufferSize = periodSize;

    // Create a double audio buffer of defined size
    stream.buffer = rbuffer_LoadBuffer(formatIn, stream.channels, stream.sampleRate, subBufferSize*2, REPLAY_BUFFER_STREAM);

    if (stream.buffer != NULL)
    {
        stream.buffer->looping = true;    // Always loop for streaming buffers
        REPLAY_TRACE(LOG_INFO, "STREAM: Initialized successfully (%i Hz, %i bit, %s)", stream.sampleRate, stream.sampleSize, (stream.channels == 1)? "Mono" : "Stereo");
    }
    else REPLAY_TRACE(LOG_WARNING, "STREAM: Failed to load audio buffer, stream could not be created");

    return stream;
}

// Checks if an audio stream is ready
bool rstream_IsStreamReady(ReplayStream stream)
{
    return ((stream.buffer != NULL) &&    // Validate stream buffer
            (stream.sampleRate > 0) &&    // Validate sample rate is supported
            (stream.sampleSize > 0) &&    // Validate sample size is supported
            (stream.channels > 0));       // Validate number of channels supported
}

// Update audio stream buffers with data
// NOTE 1: Only updates one buffer of the stream source: dequeue -> update -> queue
// NOTE 2: To dequeue a buffer it needs to be processed: rstream_IsStreamProcessed()
void rstream_UpdateStream(ReplayStream stream, const void *data, int frameCount)
{
    if (stream.buffer != NULL)
    {
        if (stream.buffer->isSubBufferProcessed[0] || stream.buffer->isSubBufferProcessed[1])
        {
            ma_uint32 subBufferToUpdate = 0;

            if (stream.buffer->isSubBufferProcessed[0] && stream.buffer->isSubBufferProcessed[1])
            {
                // Both buffers are available for updating.
                // Update the first one and make sure the cursor is moved back to the front.
                subBufferToUpdate = 0;
                stream.buffer->frameCursorPos = 0;
            }
            else
            {
                // Just update whichever sub-buffer is processed.
                subBufferToUpdate = (stream.buffer->isSubBufferProcessed[0])? 0 : 1;
            }

            ma_uint32 subBufferSizeInFrames = stream.buffer->sizeInFrames/2;
            unsigned char *subBuffer = stream.buffer->data + ((subBufferSizeInFrames*stream.channels*(stream.sampleSize/8))*subBufferToUpdate);

            // Total frames processed in buffer is always the complete size, filled with 0 if required
            stream.buffer->framesProcessed += subBufferSizeInFrames;

            // Does this API expect a whole buffer to be updated in one go?
            // Assuming so, but if not will need to change this logic.
            if (subBufferSizeInFrames >= (ma_uint32)frameCount)
            {
                ma_uint32 framesToWrite = (ma_uint32)frameCount;

                ma_uint32 bytesToWrite = framesToWrite*stream.channels*(stream.sampleSize/8);
                memcpy(subBuffer, data, bytesToWrite);

                // Any leftover frames should be filled with zeros.
                ma_uint32 leftoverFrameCount = subBufferSizeInFrames - framesToWrite;

                if (leftoverFrameCount > 0) memset(subBuffer + bytesToWrite, 0, leftoverFrameCount*stream.channels*(stream.sampleSize/8));

                stream.buffer->isSubBufferProcessed[subBufferToUpdate] = false;
            }
            else REPLAY_TRACE(LOG_WARNING, "STREAM: Attempting to write too many frames to buffer");
        }
        else REPLAY_TRACE(LOG_WARNING, "STREAM: Buffer not available for updating");
    }
}

// Unload audio stream and free memory
void rstream_UnloadStream(ReplayStream stream)
{
    rbuffer_UnloadBuffer(stream.buffer);

    REPLAY_TRACE(LOG_INFO, "STREAM: Unloaded audio stream data from RAM");
}

// Play audio stream
void rstream_playStream(ReplayStream stream)
{
    rbuffer_PlayBuffer(stream.buffer);
}

// Stop audio stream
void rstream_StopStream(ReplayStream stream)
{
    rbuffer_StopBuffer(stream.buffer);
}

// Play audio stream
void rstream_PauseStream(ReplayStream stream)
{
    rbuffer_PauseBuffer(stream.buffer);
}

// Resume audio stream playing
void rstream_ResumeStream(ReplayStream stream)
{
    rbuffer_ResumeBuffer(stream.buffer);
}

// Check if audio stream is playing.
bool rstream_IsStreamPlaying(ReplayStream stream)
{
    return rbuffer_IsBufferPlaying(stream.buffer);
}

// Set volume for audio stream (1.0 is max level)
void rstream_SetStreamVolume(ReplayStream stream, float volume)
{
    rbuffer_SetBufferVolume(stream.buffer, volume);
}

// Set pitch for audio stream (1.0 is base level)
void rstream_SetStreamPitch(ReplayStream stream, float pitch)
{
    rbuffer_SetBufferPitch(stream.buffer, pitch);
}

// Set pan for audio stream
void rstream_SetStreamPan(ReplayStream stream, float pan)
{
    rbuffer_SetBufferPan(stream.buffer, pan);
}

// Check if any audio stream buffers requires refill
bool rstream_IsStreamProcessed(ReplayStream stream)
{
    if (stream.buffer == NULL) return false;

    return (stream.buffer->isSubBufferProcessed[0] || stream.buffer->isSubBufferProcessed[1]);
}

//----------------------------------------------------------------------------------
// Module Functions Definition - Sounds loading and playing (.WAV)
//----------------------------------------------------------------------------------

// Load sound from file
// NOTE: The entire file is loaded to memory to be played (no-streaming)
RSound replay_LoadSound(const char *fileName)
{
    ReplayWave wave = rwave_LoadWave(fileName);

    RSound sound = _replay_LoadSoundFromWave(wave);

    rwave_UnloadWave(wave);       // Sound is loaded, we can unload wave

    return sound;
}

// Load sound from wave data
// NOTE: ReplayWave data must be unallocated manually
RSound _replay_LoadSoundFromWave(ReplayWave wave)
{
    RSound sound = { 0 };

    if (wave.data != NULL)
    {
        // When using miniaudio we need to do our own mixing.
        // To simplify this we need convert the format of each sound to be consistent with
        // the format used to open the playback REPLAY.System.device. We can do this two ways:
        //
        //   1) Convert the whole sound in one go at load time (here).
        //   2) Convert the audio data in chunks at mixing time.
        //
        // First option has been selected, format conversion is done on the loading stage.
        // The downside is that it uses more memory if the original sound is u8 or s16.
        ma_format formatIn = ((wave.sampleSize == 8)? ma_format_u8 : ((wave.sampleSize == 16)? ma_format_s16 : ma_format_f32));
        ma_uint32 frameCountIn = wave.frameCount;

        ma_uint32 frameCount = (ma_uint32)ma_convert_frames(NULL, 0, RA_AUDIO_DEVICE_FORMAT, RA_AUDIO_DEVICE_CHANNELS, REPLAY.System.device.sampleRate, NULL, frameCountIn, formatIn, wave.channels, wave.sampleRate);
        if (frameCount == 0) REPLAY_TRACE(LOG_WARNING, "SOUND: Failed to get frame count for format conversion");

        ReplayBuffer *audioBuffer = rbuffer_LoadBuffer(RA_AUDIO_DEVICE_FORMAT, RA_AUDIO_DEVICE_CHANNELS, REPLAY.System.device.sampleRate, frameCount, REPLAY_BUFFER_STATIC);
        if (audioBuffer == NULL)
        {
            REPLAY_TRACE(LOG_WARNING, "SOUND: Failed to create buffer");
            return sound; // early return to avoid dereferencing the audioBuffer null pointer
        }

        frameCount = (ma_uint32)ma_convert_frames(audioBuffer->data, frameCount, RA_AUDIO_DEVICE_FORMAT, RA_AUDIO_DEVICE_CHANNELS, REPLAY.System.device.sampleRate, wave.data, frameCountIn, formatIn, wave.channels, wave.sampleRate);
        if (frameCount == 0) REPLAY_TRACE(LOG_WARNING, "SOUND: Failed format conversion");

        sound.frameCount = frameCount;
        sound.stream.sampleRate = REPLAY.System.device.sampleRate;
        sound.stream.sampleSize = 32;
        sound.stream.channels = RA_AUDIO_DEVICE_CHANNELS;
        sound.stream.buffer = audioBuffer;
    }

    return sound;
}

// Checks if a sound is ready
bool replay_IsSoundReady(RSound sound)
{
    return ((sound.frameCount > 0) &&           // Validate frame count
            (sound.stream.buffer != NULL) &&    // Validate stream buffer
            (sound.stream.sampleRate > 0) &&    // Validate sample rate is supported
            (sound.stream.sampleSize > 0) &&    // Validate sample size is supported
            (sound.stream.channels > 0));       // Validate number of channels supported
}

// Update sound buffer with new data
void replay_UpdateSound(RSound sound, const void *data, int frameCount)
{
    if (sound.stream.buffer != NULL)
    {
        rbuffer_StopBuffer(sound.stream.buffer);

        // TODO: May want to lock/unlock this since this data buffer is read at mixing time
        memcpy(sound.stream.buffer->data, data, frameCount*ma_get_bytes_per_frame(sound.stream.buffer->converter.formatIn, sound.stream.buffer->converter.channelsIn));
    }
}

// Unload sound
void replay_UnloadSound(RSound sound)
{
    rbuffer_UnloadBuffer(sound.stream.buffer);
    //REPLAY_TRACE(LOG_INFO, "SOUND: Unloaded sound data from RAM");
}

// Play a sound
void replay_PlaySound(RSound sound)
{
    rbuffer_PlayBuffer(sound.stream.buffer);
}

// Stop reproducing a sound
void replay_StopSound(RSound sound)
{
    rbuffer_StopBuffer(sound.stream.buffer);
}

// Pause a sound
void replay_PauseSound(RSound sound)
{
    rbuffer_PauseBuffer(sound.stream.buffer);
}

// Resume a paused sound
void replay_ResumeSound(RSound sound)
{
    rbuffer_ResumeBuffer(sound.stream.buffer);
}

// Check if a sound is playing
bool replay_IsSoundPlaying(RSound sound)
{
    return rbuffer_IsBufferPlaying(sound.stream.buffer);
}

// Set volume for a sound
void replay_SetSoundVolume(RSound sound, float volume)
{
    rbuffer_SetBufferVolume(sound.stream.buffer, volume);
}

// Set pitch for a sound
void replay_SetSoundPitch(RSound sound, float pitch)
{
    rbuffer_SetBufferPitch(sound.stream.buffer, pitch);
}

// Set pan for a sound
void replay_SetSoundPan(RSound sound, float pan)
{
    rbuffer_SetBufferPan(sound.stream.buffer, pan);
}

//----------------------------------------------------------------------------------
// Module Functions Definition - Music loading and stream playing
//----------------------------------------------------------------------------------

// Load music stream from file
RMusic replay_LoadMusic(const char *fileName)
{
    RMusic music = { 0 };
    bool musicLoaded = false;

    if (false) { }
#if defined(RA_SUPPORT_FILEFORMAT_WAV)
    else if (_replay_IsFileExtension(fileName, ".wav"))
    {
        drwav *ctxWav = REPLAY_CALLOC(1, sizeof(drwav));
        bool success = drwav_init_file(ctxWav, fileName, NULL);

        music.ctxType = RMUSIC_AUDIO_WAV;
        music.ctxData = ctxWav;

        if (success)
        {
            int sampleSize = ctxWav->bitsPerSample;
            if (ctxWav->bitsPerSample == 24) sampleSize = 16;   // Forcing conversion to s16 on UpdateMusicStream()

            music.stream = rstream_LoadStream(ctxWav->sampleRate, sampleSize, ctxWav->channels);
            music.frameCount = (unsigned int)ctxWav->totalPCMFrameCount;
            music.looping = true;   // Looping enabled by default
            musicLoaded = true;
        }
    }
#endif
#if defined(RA_SUPPORT_FILEFORMAT_OGG)
    else if (_replay_IsFileExtension(fileName, ".ogg"))
    {
        // Open ogg audio stream
        music.ctxType = RMUSIC_AUDIO_OGG;
        music.ctxData = stb_vorbis_open_filename(fileName, NULL, NULL);

        if (music.ctxData != NULL)
        {
            stb_vorbis_info info = stb_vorbis_get_info((stb_vorbis *)music.ctxData);  // Get Ogg file info

            // OGG bit rate defaults to 16 bit, it's enough for compressed format
            music.stream = rstream_LoadStream(info.sample_rate, 16, info.channels);

            // WARNING: It seems this function returns length in frames, not samples, so we multiply by channels
            music.frameCount = (unsigned int)stb_vorbis_stream_length_in_samples((stb_vorbis *)music.ctxData);
            music.looping = true;   // Looping enabled by default
            musicLoaded = true;
        }
    }
#endif
#if defined(RA_SUPPORT_FILEFORMAT_MP3)
    else if (_replay_IsFileExtension(fileName, ".mp3"))
    {
        drmp3 *ctxMp3 = REPLAY_CALLOC(1, sizeof(drmp3));
        int result = drmp3_init_file(ctxMp3, fileName, NULL);

        music.ctxType = RMUSIC_AUDIO_MP3;
        music.ctxData = ctxMp3;

        if (result > 0)
        {
            music.stream = rstream_LoadStream(ctxMp3->sampleRate, 32, ctxMp3->channels);
            music.frameCount = (unsigned int)drmp3_get_pcm_frame_count(ctxMp3);
            music.looping = true;   // Looping enabled by default
            musicLoaded = true;
        }
    }
#endif
    else REPLAY_TRACE(LOG_WARNING, "STREAM: [%s] File format not supported", fileName);

    if (!musicLoaded)
    {
        if (false) { }
    #if defined(RA_SUPPORT_FILEFORMAT_WAV)
        else if (music.ctxType == RMUSIC_AUDIO_WAV) drwav_uninit((drwav *)music.ctxData);
    #endif
    #if defined(RA_SUPPORT_FILEFORMAT_OGG)
        else if (music.ctxType == RMUSIC_AUDIO_OGG) stb_vorbis_close((stb_vorbis *)music.ctxData);
    #endif
    #if defined(RA_SUPPORT_FILEFORMAT_MP3)
        else if (music.ctxType == RMUSIC_AUDIO_MP3) { drmp3_uninit((drmp3 *)music.ctxData); REPLAY_FREE(music.ctxData); }
    #endif

        music.ctxData = NULL;
        REPLAY_TRACE(LOG_WARNING, "FILEIO: [%s] Music file could not be opened", fileName);
    }
    else
    {
        // Show some music stream info
        REPLAY_TRACE(LOG_INFO, "FILEIO: [%s] Music file loaded successfully", fileName);
        REPLAY_TRACE(LOG_INFO, "    > Sample rate:   %i Hz", music.stream.sampleRate);
        REPLAY_TRACE(LOG_INFO, "    > Sample size:   %i bits", music.stream.sampleSize);
        REPLAY_TRACE(LOG_INFO, "    > Channels:      %i (%s)", music.stream.channels, (music.stream.channels == 1)? "Mono" : (music.stream.channels == 2)? "Stereo" : "Multi");
        REPLAY_TRACE(LOG_INFO, "    > Total frames:  %i", music.frameCount);
    }

    return music;
}

// Load music stream from memory buffer, fileType refers to extension: i.e. ".wav"
// WARNING: File extension must be provided in lower-case
RMusic _replay_LoadMusicFromMemory(const char *fileType, const unsigned char *data, int dataSize)
{
    RMusic music = { 0 };
    bool musicLoaded = false;

    if (false) { }
#if defined(RA_SUPPORT_FILEFORMAT_WAV)
    else if ((strcmp(fileType, ".wav") == 0) || (strcmp(fileType, ".WAV") == 0))
    {
        drwav *ctxWav = REPLAY_CALLOC(1, sizeof(drwav));

        bool success = drwav_init_memory(ctxWav, (const void *)data, dataSize, NULL);

        music.ctxType = RMUSIC_AUDIO_WAV;
        music.ctxData = ctxWav;

        if (success)
        {
            int sampleSize = ctxWav->bitsPerSample;
            if (ctxWav->bitsPerSample == 24) sampleSize = 16;   // Forcing conversion to s16 on UpdateMusicStream()

            music.stream = rstream_LoadStream(ctxWav->sampleRate, sampleSize, ctxWav->channels);
            music.frameCount = (unsigned int)ctxWav->totalPCMFrameCount;
            music.looping = true;   // Looping enabled by default
            musicLoaded = true;
        }
    }
#endif
#if defined(RA_SUPPORT_FILEFORMAT_OGG)
    else if ((strcmp(fileType, ".ogg") == 0) || (strcmp(fileType, ".OGG") == 0))
    {
        // Open ogg audio stream
        music.ctxType = RMUSIC_AUDIO_OGG;
        //music.ctxData = stb_vorbis_open_filename(fileName, NULL, NULL);
        music.ctxData = stb_vorbis_open_memory((const unsigned char *)data, dataSize, NULL, NULL);

        if (music.ctxData != NULL)
        {
            stb_vorbis_info info = stb_vorbis_get_info((stb_vorbis *)music.ctxData);  // Get Ogg file info

            // OGG bit rate defaults to 16 bit, it's enough for compressed format
            music.stream = rstream_LoadStream(info.sample_rate, 16, info.channels);

            // WARNING: It seems this function returns length in frames, not samples, so we multiply by channels
            music.frameCount = (unsigned int)stb_vorbis_stream_length_in_samples((stb_vorbis *)music.ctxData);
            music.looping = true;   // Looping enabled by default
            musicLoaded = true;
        }
    }
#endif
#if defined(RA_SUPPORT_FILEFORMAT_MP3)
    else if ((strcmp(fileType, ".mp3") == 0) || (strcmp(fileType, ".MP3") == 0))
    {
        drmp3 *ctxMp3 = REPLAY_CALLOC(1, sizeof(drmp3));
        int success = drmp3_init_memory(ctxMp3, (const void*)data, dataSize, NULL);

        music.ctxType = RMUSIC_AUDIO_MP3;
        music.ctxData = ctxMp3;

        if (success)
        {
            music.stream = rstream_LoadStream(ctxMp3->sampleRate, 32, ctxMp3->channels);
            music.frameCount = (unsigned int)drmp3_get_pcm_frame_count(ctxMp3);
            music.looping = true;   // Looping enabled by default
            musicLoaded = true;
        }
    }
#endif
    else REPLAY_TRACE(LOG_WARNING, "STREAM: Data format not supported");

    if (!musicLoaded)
    {
        if (false) { }
#if defined(RA_SUPPORT_FILEFORMAT_WAV)
        else if (music.ctxType == RMUSIC_AUDIO_WAV) drwav_uninit((drwav *)music.ctxData);
#endif
#if defined(RA_SUPPORT_FILEFORMAT_OGG)
        else if (music.ctxType == RMUSIC_AUDIO_OGG) stb_vorbis_close((stb_vorbis *)music.ctxData);
#endif
#if defined(RA_SUPPORT_FILEFORMAT_MP3)
        else if (music.ctxType == RMUSIC_AUDIO_MP3) { drmp3_uninit((drmp3 *)music.ctxData); REPLAY_FREE(music.ctxData); }
#endif

        music.ctxData = NULL;
        REPLAY_TRACE(LOG_WARNING, "FILEIO: Music data could not be loaded");
    }
    else
    {
        // Show some music stream info
        REPLAY_TRACE(LOG_INFO, "FILEIO: Music data loaded successfully");
        REPLAY_TRACE(LOG_INFO, "    > Sample rate:   %i Hz", music.stream.sampleRate);
        REPLAY_TRACE(LOG_INFO, "    > Sample size:   %i bits", music.stream.sampleSize);
        REPLAY_TRACE(LOG_INFO, "    > Channels:      %i (%s)", music.stream.channels, (music.stream.channels == 1)? "Mono" : (music.stream.channels == 2)? "Stereo" : "Multi");
        REPLAY_TRACE(LOG_INFO, "    > Total frames:  %i", music.frameCount);
    }

    return music;
}

// Checks if a music stream is ready
bool replay_IsMusicReady(RMusic music)
{
    return ((music.ctxData != NULL) &&          // Validate context loaded
            (music.frameCount > 0) &&           // Validate audio frame count
            (music.stream.sampleRate > 0) &&    // Validate sample rate is supported
            (music.stream.sampleSize > 0) &&    // Validate sample size is supported
            (music.stream.channels > 0));       // Validate number of channels supported
}

// Update (re-fill) music buffers if data already processed
void replay_UpdateMusic(RMusic music)
{
    if (music.stream.buffer == NULL) return;

    unsigned int subBufferSizeInFrames = music.stream.buffer->sizeInFrames/2;

    // On first call of this function we lazily pre-allocated a temp buffer to read audio files/memory data in
    int frameSize = music.stream.channels*music.stream.sampleSize/8;
    unsigned int pcmSize = subBufferSizeInFrames*frameSize;

    if (REPLAY.System.pcmBufferSize < pcmSize)
    {
        REPLAY_FREE(REPLAY.System.pcmBuffer);
        REPLAY.System.pcmBuffer = REPLAY_CALLOC(1, pcmSize);
        REPLAY.System.pcmBufferSize = pcmSize;
    }

    // Check both sub-buffers to check if they require refilling
    for (int i = 0; i < 2; i++)
    {
        if ((music.stream.buffer != NULL) && !music.stream.buffer->isSubBufferProcessed[i]) continue; // No refilling required, move to next sub-buffer

        unsigned int framesLeft = music.frameCount - music.stream.buffer->framesProcessed;  // Frames left to be processed
        unsigned int framesToStream = 0;                 // Total frames to be streamed

        if ((framesLeft >= subBufferSizeInFrames) || music.looping) framesToStream = subBufferSizeInFrames;
        else framesToStream = framesLeft;

        int frameCountStillNeeded = framesToStream;
        int frameCountReadTotal = 0;

        switch (music.ctxType)
        {
        #if defined(RA_SUPPORT_FILEFORMAT_WAV)
            case RMUSIC_AUDIO_WAV:
            {
                if (music.stream.sampleSize == 16)
                {
                    while (true)
                    {
                        int frameCountRead = (int)drwav_read_pcm_frames_s16((drwav *)music.ctxData, frameCountStillNeeded, (short *)((char *)REPLAY.System.pcmBuffer + frameCountReadTotal*frameSize));
                        frameCountReadTotal += frameCountRead;
                        frameCountStillNeeded -= frameCountRead;
                        if (frameCountStillNeeded == 0) break;
                        else drwav_seek_to_first_pcm_frame((drwav *)music.ctxData);
                    }
                }
                else if (music.stream.sampleSize == 32)
                {
                    while (true)
                    {
                        int frameCountRead = (int)drwav_read_pcm_frames_f32((drwav *)music.ctxData, frameCountStillNeeded, (float *)((char *)REPLAY.System.pcmBuffer + frameCountReadTotal*frameSize));
                        frameCountReadTotal += frameCountRead;
                        frameCountStillNeeded -= frameCountRead;
                        if (frameCountStillNeeded == 0) break;
                        else drwav_seek_to_first_pcm_frame((drwav *)music.ctxData);
                    }
                }
            } break;
        #endif
        #if defined(RA_SUPPORT_FILEFORMAT_OGG)
            case RMUSIC_AUDIO_OGG:
            {
                while (true)
                {
                    int frameCountRead = stb_vorbis_get_samples_short_interleaved((stb_vorbis *)music.ctxData, music.stream.channels, (short *)((char *)REPLAY.System.pcmBuffer + frameCountReadTotal*frameSize), frameCountStillNeeded*music.stream.channels);
                    frameCountReadTotal += frameCountRead;
                    frameCountStillNeeded -= frameCountRead;
                    if (frameCountStillNeeded == 0) break;
                    else stb_vorbis_seek_start((stb_vorbis *)music.ctxData);
                }
            } break;
        #endif
        #if defined(RA_SUPPORT_FILEFORMAT_MP3)
            case RMUSIC_AUDIO_MP3:
            {
                while (true)
                {
                    int frameCountRead = (int)drmp3_read_pcm_frames_f32((drmp3 *)music.ctxData, frameCountStillNeeded, (float *)((char *)REPLAY.System.pcmBuffer + frameCountReadTotal*frameSize));
                    frameCountReadTotal += frameCountRead;
                    frameCountStillNeeded -= frameCountRead;
                    if (frameCountStillNeeded == 0) break;
                    else drmp3_seek_to_start_of_stream((drmp3 *)music.ctxData);
                }
            } break;
        #endif
            default: break;
        }

        rstream_UpdateStream(music.stream, REPLAY.System.pcmBuffer, framesToStream);

        music.stream.buffer->framesProcessed = music.stream.buffer->framesProcessed%music.frameCount;

        if (framesLeft <= subBufferSizeInFrames)
        {
            if (!music.looping)
            {
                // Streaming is ending, we filled latest frames from input
                replay_StopMusic(music);
                return;
            }
        }
    }

    // NOTE: In case window is minimized, music stream is stopped,
    // just make sure to play again on window restore
    if (replay_IsMusicPlaying(music)) replay_PlayMusic(music);
}

// Unload music stream
void replay_UnloadMusic(RMusic music)
{
    rstream_UnloadStream(music.stream);

    if (music.ctxData != NULL)
    {
        if (false) { }
#if defined(RA_SUPPORT_FILEFORMAT_WAV)
        else if (music.ctxType == RMUSIC_AUDIO_WAV) drwav_uninit((drwav *)music.ctxData);
#endif
#if defined(RA_SUPPORT_FILEFORMAT_OGG)
        else if (music.ctxType == RMUSIC_AUDIO_OGG) stb_vorbis_close((stb_vorbis *)music.ctxData);
#endif
#if defined(RA_SUPPORT_FILEFORMAT_MP3)
        else if (music.ctxType == RMUSIC_AUDIO_MP3) { drmp3_uninit((drmp3 *)music.ctxData); REPLAY_FREE(music.ctxData); }
#endif
    }
}

// Start music playing (open stream)
void replay_PlayMusic(RMusic music)
{
    if (music.stream.buffer != NULL)
    {
        // For music streams, we need to make sure we maintain the frame cursor position
        // This is a hack for this section of code in UpdateMusicStream()
        // NOTE: In case window is minimized, music stream is stopped, just make sure to
        // play again on window restore: if (replay_IsMusicPlaying(music)) replay_PlayMusic(music);
        ma_uint32 frameCursorPos = music.stream.buffer->frameCursorPos;
        rstream_playStream(music.stream);  // WARNING: This resets the cursor position.
        music.stream.buffer->frameCursorPos = frameCursorPos;
    }
}

// Stop music playing (close stream)
void replay_StopMusic(RMusic music)
{
    rstream_StopStream(music.stream);

    switch (music.ctxType)
    {
#if defined(RA_SUPPORT_FILEFORMAT_WAV)
        case RMUSIC_AUDIO_WAV: drwav_seek_to_first_pcm_frame((drwav *)music.ctxData); break;
#endif
#if defined(RA_SUPPORT_FILEFORMAT_OGG)
        case RMUSIC_AUDIO_OGG: stb_vorbis_seek_start((stb_vorbis *)music.ctxData); break;
#endif
#if defined(RA_SUPPORT_FILEFORMAT_MP3)
        case RMUSIC_AUDIO_MP3: drmp3_seek_to_start_of_stream((drmp3 *)music.ctxData); break;
#endif
        default: break;
    }
}

// Pause music playing
void replay_PauseMusic(RMusic music)
{
    rstream_PauseStream(music.stream);
}

// Resume music playing
void replay_ResumeMusic(RMusic music)
{
    rstream_ResumeStream(music.stream);
}

// Check if any music is playing
bool replay_IsMusicPlaying(RMusic music)
{
    return rstream_IsStreamPlaying(music.stream);
}

// Set volume for music
void replay_SetMusicVolume(RMusic music, float volume)
{
    rstream_SetStreamVolume(music.stream, volume);
}

// Get music time length (in seconds)
float replay_GetMusicLength(RMusic music)
{
    float totalSeconds = 0.0f;

    totalSeconds = (float)music.frameCount/music.stream.sampleRate;

    return totalSeconds;
}

// Get current music time played (in seconds)
float replay_GetMusicTime(RMusic music)
{
    float secondsPlayed = 0.0f;
    if (music.stream.buffer != NULL)
    {
        {
            //ma_uint32 frameSizeInBytes = ma_get_bytes_per_sample(music.stream.buffer->dsp.formatConverterIn.config.formatIn)*music.stream.buffer->dsp.formatConverterIn.config.channels;
            int framesProcessed = (int)music.stream.buffer->framesProcessed;
            int subBufferSize = (int)music.stream.buffer->sizeInFrames/2;
            int framesInFirstBuffer = music.stream.buffer->isSubBufferProcessed[0]? 0 : subBufferSize;
            int framesInSecondBuffer = music.stream.buffer->isSubBufferProcessed[1]? 0 : subBufferSize;
            int framesSentToMix = music.stream.buffer->frameCursorPos%subBufferSize;
            int framesPlayed = (framesProcessed - framesInFirstBuffer - framesInSecondBuffer + framesSentToMix)%(int)music.frameCount;
            if (framesPlayed < 0) framesPlayed += music.frameCount;
            secondsPlayed = (float)framesPlayed/music.stream.sampleRate;
        }
    }

    return secondsPlayed;
}

// Seek music to a certain position (in seconds)
void replay_SetMusicTime(RMusic music, float position)
{

    unsigned int positionInFrames = (unsigned int)(position*music.stream.sampleRate);

    switch (music.ctxType)
    {
#if defined(RA_SUPPORT_FILEFORMAT_WAV)
        case RMUSIC_AUDIO_WAV: drwav_seek_to_pcm_frame((drwav *)music.ctxData, positionInFrames); break;
#endif
#if defined(RA_SUPPORT_FILEFORMAT_OGG)
        case RMUSIC_AUDIO_OGG: stb_vorbis_seek_frame((stb_vorbis *)music.ctxData, positionInFrames); break;
#endif
#if defined(RA_SUPPORT_FILEFORMAT_MP3)
        case RMUSIC_AUDIO_MP3: drmp3_seek_to_pcm_frame((drmp3 *)music.ctxData, positionInFrames); break;
#endif
        default: break;
    }

    music.stream.buffer->framesProcessed = positionInFrames;
}

// Set pitch for music
void replay_SetMusicPitch(RMusic music, float pitch)
{
    rbuffer_SetBufferPitch(music.stream.buffer, pitch);
}

// Set pan for a music
void replay_SetMusicPan(RMusic music, float pan)
{
    rbuffer_SetBufferPan(music.stream.buffer, pan);
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

// Log callback function
static void _replay_OnAudioLog(void *pUserData, ma_uint32 level, const char *pMessage)
{
    REPLAY_TRACE(LOG_WARNING, "miniaudio: %s", pMessage);   // All log messages from miniaudio are errors
}

// Reads audio data from an ReplayBuffer object in internal format.
static ma_uint32 ReadAudioBufferFramesInInternalFormat(ReplayBuffer *audioBuffer, void *framesOut, ma_uint32 frameCount)
{
    // Using audio buffer callback
    if (audioBuffer->callback)
    {
        audioBuffer->callback(framesOut, frameCount);
        audioBuffer->framesProcessed += frameCount;

        return frameCount;
    }

    ma_uint32 subBufferSizeInFrames = (audioBuffer->sizeInFrames > 1)? audioBuffer->sizeInFrames/2 : audioBuffer->sizeInFrames;
    ma_uint32 currentSubBufferIndex = audioBuffer->frameCursorPos/subBufferSizeInFrames;

    if (currentSubBufferIndex > 1) return 0;

    // Another thread can update the processed state of buffers, so
    // we just take a copy here to try and avoid potential synchronization problems
    bool isSubBufferProcessed[2] = { 0 };
    isSubBufferProcessed[0] = audioBuffer->isSubBufferProcessed[0];
    isSubBufferProcessed[1] = audioBuffer->isSubBufferProcessed[1];

    ma_uint32 frameSizeInBytes = ma_get_bytes_per_frame(audioBuffer->converter.formatIn, audioBuffer->converter.channelsIn);

    // Fill out every frame until we find a buffer that's marked as processed. Then fill the remainder with 0
    ma_uint32 framesRead = 0;
    while (1)
    {
        // We break from this loop differently depending on the buffer's usage
        //  - For static buffers, we simply fill as much data as we can
        //  - For streaming buffers we only fill half of the buffer that are processed
        //    Unprocessed halves must keep their audio data in-tact
        if (audioBuffer->usage == REPLAY_BUFFER_STATIC)
        {
            if (framesRead >= frameCount) break;
        }
        else
        {
            if (isSubBufferProcessed[currentSubBufferIndex]) break;
        }

        ma_uint32 totalFramesRemaining = (frameCount - framesRead);
        if (totalFramesRemaining == 0) break;

        ma_uint32 framesRemainingInOutputBuffer;
        if (audioBuffer->usage == REPLAY_BUFFER_STATIC)
        {
            framesRemainingInOutputBuffer = audioBuffer->sizeInFrames - audioBuffer->frameCursorPos;
        }
        else
        {
            ma_uint32 firstFrameIndexOfThisSubBuffer = subBufferSizeInFrames*currentSubBufferIndex;
            framesRemainingInOutputBuffer = subBufferSizeInFrames - (audioBuffer->frameCursorPos - firstFrameIndexOfThisSubBuffer);
        }

        ma_uint32 framesToRead = totalFramesRemaining;
        if (framesToRead > framesRemainingInOutputBuffer) framesToRead = framesRemainingInOutputBuffer;

        memcpy((unsigned char *)framesOut + (framesRead*frameSizeInBytes), audioBuffer->data + (audioBuffer->frameCursorPos*frameSizeInBytes), framesToRead*frameSizeInBytes);
        audioBuffer->frameCursorPos = (audioBuffer->frameCursorPos + framesToRead)%audioBuffer->sizeInFrames;
        framesRead += framesToRead;

        // If we've read to the end of the buffer, mark it as processed
        if (framesToRead == framesRemainingInOutputBuffer)
        {
            audioBuffer->isSubBufferProcessed[currentSubBufferIndex] = true;
            isSubBufferProcessed[currentSubBufferIndex] = true;

            currentSubBufferIndex = (currentSubBufferIndex + 1)%2;

            // We need to break from this loop if we're not looping
            if (!audioBuffer->looping)
            {
                rbuffer_StopBuffer(audioBuffer);
                break;
            }
        }
    }

    // Zero-fill excess
    ma_uint32 totalFramesRemaining = (frameCount - framesRead);
    if (totalFramesRemaining > 0)
    {
        memset((unsigned char *)framesOut + (framesRead*frameSizeInBytes), 0, totalFramesRemaining*frameSizeInBytes);

        // For static buffers we can fill the remaining frames with silence for safety, but we don't want
        // to report those frames as "read". The reason for this is that the caller uses the return value
        // to know whether a non-looping sound has finished playback.
        if (audioBuffer->usage != REPLAY_BUFFER_STATIC) framesRead += totalFramesRemaining;
    }

    return framesRead;
}

// Reads audio data from an ReplayBuffer object in device format. Returned data will be in a format appropriate for mixing.
static ma_uint32 ReadAudioBufferFramesInMixingFormat(ReplayBuffer *audioBuffer, float *framesOut, ma_uint32 frameCount)
{
    // What's going on here is that we're continuously converting data from the ReplayBuffer's internal format to the mixing format, which
    // should be defined by the output format of the data converter. We do this until frameCount frames have been output. The important
    // detail to remember here is that we never, ever attempt to read more input data than is required for the specified number of output
    // frames. This can be achieved with ma_data_converter_get_required_input_frame_count().
    ma_uint8 inputBuffer[4096] = { 0 };
    ma_uint32 inputBufferFrameCap = sizeof(inputBuffer)/ma_get_bytes_per_frame(audioBuffer->converter.formatIn, audioBuffer->converter.channelsIn);

    ma_uint32 totalOutputFramesProcessed = 0;
    while (totalOutputFramesProcessed < frameCount)
    {
        ma_uint64 outputFramesToProcessThisIteration = frameCount - totalOutputFramesProcessed;
        ma_uint64 inputFramesToProcessThisIteration = 0;

        (void)ma_data_converter_get_required_input_frame_count(&audioBuffer->converter, outputFramesToProcessThisIteration, &inputFramesToProcessThisIteration);
        if (inputFramesToProcessThisIteration > inputBufferFrameCap)
        {
            inputFramesToProcessThisIteration = inputBufferFrameCap;
        }

        float *runningFramesOut = framesOut + (totalOutputFramesProcessed*audioBuffer->converter.channelsOut);

        /* At this point we can convert the data to our mixing format. */
        ma_uint64 inputFramesProcessedThisIteration = ReadAudioBufferFramesInInternalFormat(audioBuffer, inputBuffer, (ma_uint32)inputFramesToProcessThisIteration);    /* Safe cast. */
        ma_uint64 outputFramesProcessedThisIteration = outputFramesToProcessThisIteration;
        ma_data_converter_process_pcm_frames(&audioBuffer->converter, inputBuffer, &inputFramesProcessedThisIteration, runningFramesOut, &outputFramesProcessedThisIteration);

        totalOutputFramesProcessed += (ma_uint32)outputFramesProcessedThisIteration; /* Safe cast. */

        if (inputFramesProcessedThisIteration < inputFramesToProcessThisIteration)
        {
            break;  /* Ran out of input data. */
        }

        /* This should never be hit, but will add it here for safety. Ensures we get out of the loop when no input nor output frames are processed. */
        if (inputFramesProcessedThisIteration == 0 && outputFramesProcessedThisIteration == 0)
        {
            break;
        }
    }

    return totalOutputFramesProcessed;
}

// Sending audio data to device callback function
// This function will be called when miniaudio needs more data
// NOTE: All the mixing takes place here
static void _replay_OnSendData(ma_device *pDevice, void *pFramesOut, const void *pFramesInput, ma_uint32 frameCount)
{
    (void)pDevice;

    // Mixing is basically just an accumulation, we need to initialize the output buffer to 0
    memset(pFramesOut, 0, frameCount*pDevice->playback.channels*ma_get_bytes_per_sample(pDevice->playback.format));

    // Using a mutex here for thread-safety which makes things not real-time
    // This is unlikely to be necessary for this project, but may want to consider how you might want to avoid this
    ma_mutex_lock(&REPLAY.System.lock);
    {
        for (ReplayBuffer *audioBuffer = REPLAY.Buffer.first; audioBuffer != NULL; audioBuffer = audioBuffer->next)
        {
            // Ignore stopped or paused sounds
            if (!audioBuffer->playing || audioBuffer->paused) continue;

            ma_uint32 framesRead = 0;

            while (1)
            {
                if (framesRead >= frameCount) break;

                // Just read as much data as we can from the stream
                ma_uint32 framesToRead = (frameCount - framesRead);

                while (framesToRead > 0)
                {
                    float tempBuffer[1024] = { 0 }; // Frames for stereo

                    ma_uint32 framesToReadRightNow = framesToRead;
                    if (framesToReadRightNow > sizeof(tempBuffer)/sizeof(tempBuffer[0])/RA_AUDIO_DEVICE_CHANNELS)
                    {
                        framesToReadRightNow = sizeof(tempBuffer)/sizeof(tempBuffer[0])/RA_AUDIO_DEVICE_CHANNELS;
                    }

                    ma_uint32 framesJustRead = ReadAudioBufferFramesInMixingFormat(audioBuffer, tempBuffer, framesToReadRightNow);
                    if (framesJustRead > 0)
                    {
                        float *framesOut = (float *)pFramesOut + (framesRead*REPLAY.System.device.playback.channels);
                        float *framesIn = tempBuffer;

                        // Apply processors chain if defined
                        ReplayProcessor *processor = audioBuffer->processor;
                        while (processor)
                        {
                            processor->process(framesIn, framesJustRead);
                            processor = processor->next;
                        }

                        _replay_MixAudioFrames(framesOut, framesIn, framesJustRead, audioBuffer);

                        framesToRead -= framesJustRead;
                        framesRead += framesJustRead;
                    }

                    if (!audioBuffer->playing)
                    {
                        framesRead = frameCount;
                        break;
                    }

                    // If we weren't able to read all the frames we requested, break
                    if (framesJustRead < framesToReadRightNow)
                    {
                        if (!audioBuffer->looping)
                        {
                            rbuffer_StopBuffer(audioBuffer);
                            break;
                        }
                        else
                        {
                            // Should never get here, but just for safety,
                            // move the cursor position back to the start and continue the loop
                            audioBuffer->frameCursorPos = 0;
                            continue;
                        }
                    }
                }

                // If for some reason we weren't able to read every frame we'll need to break from the loop
                // Not doing this could theoretically put us into an infinite loop
                if (framesToRead > 0) break;
            }
        }
    }

    ReplayProcessor *processor = REPLAY.mixedProcessor;
    while (processor)
    {
        processor->process(pFramesOut, frameCount);
        processor = processor->next;
    }

    ma_mutex_unlock(&REPLAY.System.lock);
}

// Main mixing function, pretty simple in this project, just an accumulation
// NOTE: framesOut is both an input and an output, it is initially filled with zeros outside of this function
static void _replay_MixAudioFrames(float *framesOut, const float *framesIn, ma_uint32 frameCount, ReplayBuffer *buffer)
{
    const float localVolume = buffer->volume;
    const ma_uint32 channels = REPLAY.System.device.playback.channels;

    if (channels == 2)  // We consider panning
    {
        const float left = buffer->pan;
        const float right = 1.0f - left;

        // Fast sine approximation in [0..1] for pan law: y = 0.5f*x*(3 - x*x);
        const float levels[2] = { localVolume*0.5f*left*(3.0f - left*left), localVolume*0.5f*right*(3.0f - right*right) };

        float *frameOut = framesOut;
        const float *frameIn = framesIn;

        for (ma_uint32 frame = 0; frame < frameCount; frame++)
        {
            frameOut[0] += (frameIn[0]*levels[0]);
            frameOut[1] += (frameIn[1]*levels[1]);

            frameOut += 2;
            frameIn += 2;
        }
    }
    else  // We do not consider panning
    {
        for (ma_uint32 frame = 0; frame < frameCount; frame++)
        {
            for (ma_uint32 c = 0; c < channels; c++)
            {
                float *frameOut = framesOut + (frame*channels);
                const float *frameIn = framesIn + (frame*channels);

                // Output accumulates input multiplied by volume to provided output (usually 0)
                frameOut[c] += (frameIn[c]*localVolume);
            }
        }
    }
}

// Check file extension
static bool _replay_IsFileExtension(const char *fileName, const char *ext)
{
    bool result = false;
    const char *fileExt;

    if ((fileExt = strrchr(fileName, '.')) != NULL)
    {
        if (strcmp(fileExt, ext) == 0) result = true;
    }

    return result;
}

// Get pointer to extension for a filename string (includes the dot: .png)
static const char *_replay_GetFileExtension(const char *fileName)
{
    const char *dot = strrchr(fileName, '.');

    if (!dot || dot == fileName) return NULL;

    return dot;
}

// Load data from file into a buffer
static unsigned char *_replay_LoadFileData(const char *fileName, int *dataSize)
{
    unsigned char *data = NULL;
    *dataSize = 0;

    if (fileName != NULL)
    {
        FILE *file = fopen(fileName, "rb");

        if (file != NULL)
        {
            // WARNING: On binary streams SEEK_END could not be found,
            // using fseek() and ftell() could not work in some (rare) cases
            fseek(file, 0, SEEK_END);
            int size = ftell(file);
            fseek(file, 0, SEEK_SET);

            if (size > 0)
            {
                data = (unsigned char *)REPLAY_MALLOC(size*sizeof(unsigned char));

                // NOTE: fread() returns number of read elements instead of bytes, so we read [1 byte, size elements]
                unsigned int count = (unsigned int)fread(data, sizeof(unsigned char), size, file);
                *dataSize = count;

                if (count != size) REPLAY_TRACE(LOG_WARNING, "FILEIO: [%s] File partially loaded", fileName);
                else REPLAY_TRACE(LOG_INFO, "FILEIO: [%s] File loaded successfully", fileName);
            }
            else REPLAY_TRACE(LOG_WARNING, "FILEIO: [%s] Failed to read file", fileName);

            fclose(file);
        }
        else REPLAY_TRACE(LOG_WARNING, "FILEIO: [%s] Failed to open file", fileName);
    }
    else REPLAY_TRACE(LOG_WARNING, "FILEIO: File name provided is not valid");

    return data;
}

#endif // REPLAY_SOURCE_H
