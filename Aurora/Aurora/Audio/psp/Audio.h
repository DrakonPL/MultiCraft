/*
 * Audio.h
 * This library is used to load audio from RIFF WAVE uncompressed audio files(*.wav)
 *
 * MUSAUDIO Version 1 by mowglisanu
 *
 * Copyright (C) 2009 Musa Mitchell. mowglisanu@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef __MUSAUDIO_H__
#define __MUSAUDIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <pspsdk.h>
#include <pspaudio.h>
#include <pspaudiolib.h>
#include <pspiofilemgr.h>
#include <psppower.h>
#include <psputility_avmodules.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <sys/unistd.h>

#define RIFF 0x46464952
#define WAVE 0x45564157
#define fmt__ 0x20746d66
#define fact_ 0x74636166
#define smpl_ 0x6c706d73
#define data_ 0x61746164

#define FORM 0x4d524f46 
#define AIFF 0x46464941
#define AIFC 0x43464941
#define COMM 0x4d4d4f43
#define SSND 0x444e5353

#define PAUSED     -1
#define STOP       0
#define PLAYING    1
#define BUSY       2
#define TERMINATED 3
typedef struct{
        u32 fsig;
        u32 fsize;
        u32 fmt;
}RIFFHeader;
typedef struct{
        u32 csig;
        u32 dsize;
        void *data;
}chunk;
typedef struct{
        u16 id;
        u16 channels;
        u32 frequency;
        u32 bytesPerSec;
        u16 unk1;
        u16 bitRate;
}__attribute__((packed)) fmt;
typedef struct{
        int magic;
        int offset;
        int size;
        int encoding;
        int sample_rate;
        int channels;
}au_header;
typedef struct {
	  short          numChannels;
	  unsigned long  numSampleFrames;
	  short          sampleSize;
	  unsigned char  sampleRate[10];
}__attribute__((packed)) CommonChunk;

typedef struct{
        int event;
        int data;
        int dataType;
}eventData;

typedef int (*StreamEventHandler)(void* audio, eventData *event);
typedef void (*UserEventHandler)(void* audio, int event);
typedef void (EndAudio)(void);

typedef struct{
        int size;
        StreamEventHandler stream;
        SceUID thid;
        int eof;
        int working;
        int start;
//offset to start of audio data in file
        int fill;
        int frequency;
}audioExtra;

typedef struct{
        u32 type;
//type specifies the format of the audio:- 1 = PCM
        u32 channels;
//the number of audio channels
        u32 frequency;
//frequency of audio data
        u32 bitRate;
//the bits per channel for each sample
        void *data;
//pointer to audio data(not encoded)(this depends on audio type &| streaming)
        u32 size;
//number of samples in audio(pointed to by data for type == 1)        
        float scale;
        u32 offset;
        float frac;
        u32 stream;
        int channel;
//hardware audio channel
        u32 sampleCount;
        s16 *mixBuffer;
        u32 lVolume;
        u32 rVolume;
        int play;
//specifies the playstate of the audio
        u32 loop;
        u32 fstate;
//specifies if file is valid open or not        
        u32 file;
//specifies the file handle/object/pointer/etc
        UserEventHandler ufunc;
//specifies the user function, which will be for specific events like playAudio, pauseAudio, stopAudio etc 
        u32 flags;
//specifies user data
        char *filename;
//specifies the filename
        audioExtra *extra;
//specifies extra information for each audio format, should atleast contain the first 3 elements of audioExtra
}Audio, *PAudio;

#ifdef NO_DEBUG
 #define printf(x, ...) 
#endif

#define SZ_SAMPLES 1
#define SZ_BYTES 2
#define SZ_SECONDS 3

#define STATE_F_CLOSE 0
#define STATE_F_OPEN 1
#define STATE_F_OTHER 2

#define STREAM_NO_STREAM 0
#define STREAM_FROM_FILE 1
#define STREAM_FROM_MEMORY 2

#define AUDIO_FLAG_MEMORY    0x00000001
#define AUDIO_FLAG_SCEUID    0x00000002
#define AUDIO_FLAG_FILE      0x00000004
#define AUDIO_FLAG_FREE      0x00000008
#define AUDIO_FLAG_DONT_FREE 0x00000016

#define EVENT_stopAudio           0
#define EVENT_playAudio           1
#define EVENT_pauseAudio          2
#define EVENT_loopAudio           3
#define EVENT_unloopAudio         4
#define EVENT_setFrequencyAudio   5
#define EVENT_getFrequencyAudio   15
#define EVENT_freeAudio           6
#define EVENT_reverseAudio        7
#define EVENT_setVolumeMaxAudio   8
#define EVENT_setVolumeMuteAudio  9
#define EVENT_setVolumeAudio      10
#define EVENT_muteAudio           11
#define EVENT_muteRChannelAudio   12
#define EVENT_muteLChannelAudio   13
#define EVENT_swapChannelAudio    14
#define EVENT_playAsyncAudio      16
#define EVENT_seekAudio           17
#define EVENT_getChannelsAudio    18
#define EVENT_getBitsPerSample    19
#define EVENT_getBitrateAudio     20
#define EVENT_suspending          21
#define EVENT_resumeComplete      22

#define EVENT_stopAudio_BEGIN          EVENT_stopAudio
#define EVENT_playAudio_BEGIN          EVENT_playAudio
#define EVENT_pauseAudio_BEGIN         EVENT_pauseAudio
#define EVENT_loopAudio_BEGIN          EVENT_loopAudio
#define EVENT_unloopAudio_BEGIN        EVENT_unloopAudio
#define EVENT_setFrequencyAudio_BEGIN  EVENT_setFrequencyAudio
#define EVENT_getFrequencyAudio_BEGIN  EVENT_getFrequencyAudio
#define EVENT_freeAudio_BEGIN          EVENT_freeAudio
#define EVENT_reverseAudio_BEGIN       EVENT_reverseAudio
#define EVENT_setVolumeMaxAudio_BEGIN  EVENT_setVolumeMaxAudio
#define EVENT_setVolumeMuteAudio_BEGIN EVENT_setVolumeMuteAudio
#define EVENT_setVolumeAudio_BEGIN     EVENT_setVolumeAudio
#define EVENT_muteAudio_BEGIN          EVENT_muteAudio
#define EVENT_muteRChannelAudio_BEGIN  EVENT_muteRChannelAudio
#define EVENT_muteLChannelAudio_BEGIN  EVENT_muteLChannelAudio
#define EVENT_swapChannelAudio_BEGIN   EVENT_swapChannelAudio
#define EVENT_playAsyncAudio_BEGIN     EVENT_playAsyncAudio
#define EVENT_seekAudio_BEGIN          EVENT_seekAudio
#define EVENT_getChannelsAudio_BEGIN   EVENT_getChannelsAudio
#define EVENT_getBitsPerSample_BEGIN   EVENT_getBitsPerSample
#define EVENT_getBitrateAudio_BEGIN    EVENT_getBitrateAudio

#define EVENT_stopAudio_END          (1000+EVENT_stopAudio)
#define EVENT_playAudio_END          (1000+EVENT_playAudio)
#define EVENT_pauseAudio_END         (1000+EVENT_pauseAudio)
#define EVENT_loopAudio_END          (1000+EVENT_loopAudio)
#define EVENT_unloopAudio_END        (1000+EVENT_unloopAudio)
#define EVENT_setFrequencyAudio_END  (1000+EVENT_setFrequencyAudio)
#define EVENT_getFrequencyAudio_END  (1000+EVENT_getFrequencyAudio)
#define EVENT_freeAudio_END          (1000+EVENT_freeAudio)
#define EVENT_reverseAudio_END       (1000+EVENT_reverseAudio)
#define EVENT_setVolumeMaxAudio_END  (1000+EVENT_setVolumeMaxAudio)
#define EVENT_setVolumeMuteAudio_END (1000+EVENT_setVolumeMuteAudio)
#define EVENT_setVolumeAudio_END     (1000+EVENT_setVolumeAudio)
#define EVENT_muteAudio_END          (1000+EVENT_muteAudio)
#define EVENT_muteRChannelAudio_END  (1000+EVENT_muteRChannelAudio)
#define EVENT_muteLChannelAudio_END  (1000+EVENT_muteLChannelAudio)
#define EVENT_swapChannelAudio_END   (1000+EVENT_swapChannelAudio)
#define EVENT_playAsyncAudio_END     (1000+EVENT_playAsyncAudio)
#define EVENT_seekAudio_END          (1000+EVENT_seekAudio)
#define EVENT_getChannelsAudio_END   (1000+EVENT_getChannelsAudio)
#define EVENT_getBitsPerSample_END   (1000+EVENT_getBitsPerSample)
#define EVENT_getBitrateAudio_END    (1000+EVENT_getBitrateAudio)

#define SceAt3PCodec    0x1000
#define SceAt3Codec     0x1001
#define SceMp3Codec     0x1002
#define SceAacCodec     0x1003
#define SceUnknownCodec 0x1004
#define SceWmaCodec     0x1005

#define AUDIO_ERROR_ERROR                   0x80000000
#define AUDIO_ERROR_OPEN_FILE               0x00000010
#define AUDIO_ERROR_INVALID_FILE            0x00000020
#define AUDIO_ERROR_FILE_READ               0x00000040
#define AUDIO_ERROR_THREAD_FAIL             0x00000100 
#define AUDIO_ERROR_MEM_ALLOC               0x00000200 
#define AUDIO_ERROR_ALREADY_PLAYING         0x00000400 
#define AUDIO_ERROR_OPERATION_FAILED        0x00001000 
#define AUDIO_ERROR_OPERATION_NOT_SUPPORTED 0x00002000 
#define AUDIO_ERROR_NO_FREE_CHANNEL         0x00002000 
#define AUDIO_ERROR_LOAD_MODULES            0x00010000

#define AUDIO_GET_LAST_ERROR 1
#define NUM_AUDIO_CHANNELS 8

/*extern int AvcodecLoaded;
extern int SascoreLoaded;
extern int Atrac3PlusLoaded;
extern int MpegbaseLoaded;

extern int sysFrequency;

extern int SRC;
extern int SRCReserved;
extern int init;*/
extern int loadAudioCodec(int codec);
extern void unloadAudioCodec(int codec);

extern inline int getsysFrequency();
extern inline int isSRC();
extern inline int isSRCReserved();


//I don't know if changeEdian64 works
#define changeEdian64(x) (((x&0xffll)<<56)|((x&0xff00ll)<<40)|((x&0xff0000ll)<<24)|((x&0xff000000ll)<<8)|((x&(u64)0xff00000000ll)>>8)|((x&(u64)0xff0000000000ll)>>24)|((x&(u64)0xff000000000000ll)>>40)|((x&(u64)0xff00000000000000ll)>>56))
#define changeEdian(x) (((x&0xff)<<24)|((x&0xff00)<<8)|((x&0xff0000)>>8)|((x&0xff000000)>>24))
#define changeEdian16(x) (((x&0xff)<<8)|((x&0xff00)>>8))

#undef PSP_VOLUME_MAX
#define PSP_VOLUME_MAX 0xffff
/**
 * initialize the library
 *
 * @param sounds - # of sfx thread to create
 */
void audioInit(u32 sounds);

/**
 * uninitialize the library
 */
void audioEnd();

/**
 * Load a wav PCM file
 *
 * @param filename - Path of the file to load.
 *
 * @param stream -  STREAM_FROM_FILE to read from file and STREAM_NO_STREAM to load to memory.
 *
 * @returns pointer to Audio struct on success, NULL on failure,
 */
PAudio loadWav(const char *file, int stream);

/**
 * Load an au uncompressed file
 *
 * @param filename - Path of the file to load.
 *
 * @param stream -  STREAM_FROM_FILE to read from file and STREAM_NO_STREAM to load to memory.
 *
 * @returns pointer to Audio struct on success, NULL on failure,
 */
PAudio loadAu(const char *file, int stream);

/**
 * Load an aiff uncompressed file
 *
 * @param filename - Path of the file to load.
 *
 * @param stream -  STREAM_FROM_FILE to read from file and STREAM_NO_STREAM to load to memory.
 *
 * @returns pointer to Audio struct on success, NULL on failure,
 */
PAudio loadAiff(const char *file, int stream);
PAudio loadAudio(const char *filename, int stream);

/**
 * Play audio(until samples run out unless set to loop)  
 *
 * @param audio - Pointer to audio info.
 *
 * @note fails to play if no audio channel is available,
 */
void playAudio(PAudio audio);

/**
 * Play audio, if already playing start a new one rfom the begining  
 *
 * @param audio - Pointer to audio info.
 *
 * @note fails to play if no audio channel is available,
 *       (only 4 channels avilable(built on pspaudiolib))
 *       so only four PAudio can be playing at one time.
 */
void playAsyncAudio(PAudio audio);

/**
 * pause audio
 *
 * @param audio - Pointer to audio info.
 */
void pauseAudio(PAudio audio);

/**
 * pause audio if playing or plays audio otherwise
 *
 * @param audio - Pointer to audio info.
 */
void pauseAudio2(PAudio audio);

/**
 * Stop audio and reset playback info and free audio channel
 *
 * @param audio - Pointer to audio info.
 */
void stopAudio(PAudio audio);

/**
 * Free the audio structure and all its resources(free audio channel if necessary)
 *
 * @param audio - Pointer to audio info.
 */
void freeAudio(PAudio audio);

/**
 * Set the audio to loop
 *
 * @param audio - Pointer to audio info.
 */
inline void loopAudio(PAudio audio);

/**
 * Set the audio to not loop
 *
 * @param audio - Pointer to audio info.
 */
inline void unloopAudio(PAudio audio);

inline audioExtra *getExtraAudio(PAudio audio);

/**
 * Get the audio playback frequency
 *
 * @param audio - Pointer to audio info.
 *
 * @returns number of samples being played per second
 *
 * @note return audio->frequency;
 */
inline u32 getPlaybackFrequencyAudio(PAudio audio);

/**
 * Get the audio frequency
 *
 * @param audio - Pointer to audio info.
 *
 * @returns number of samples to be played per second
 *
 * @note may not return audio->frequency;
 */
inline u32 getFrequencyAudio(PAudio audio);

/**
 * Set the audio frequency(8000 <= frequency <= 96000)
 *
 * @param audio - Pointer to audio info.
 *
 * @note changes the rate at which the audio is played back
 *       increasing would cause fast-forward and decrease
 *       sllooowwwww
 */
void setFrequencyAudio(PAudio audio, u32 frequency);

/**
 * Get the user event handler function
 *
 * @param audio - Pointer to audio info.
 *
 * @returns audio->ufunc
 *
 */
inline UserEventHandler getUserFunctionAudio(PAudio audio);

/**
 * Set the user event handler function
 *
 * @param audio - Pointer to audio info.
 *
 */
void setUserFunctionAudio(PAudio audio, UserEventHandler ufunc);

/**
 * Create mute audio.
 *
 * @param audio - Pointer to audio info.
 *
 * @returns pointer to Audio struct on success, NULL on failure,
 *
 * @note this function may not fail if fed bad data,
 *       it may just change them to default values. 
 */
PAudio createAudio(u32 channels, u32 frequency, u32 bitrate, u32 size, u32 sztype);

/**
 * Create a copy of audio.
 *
 * @param audio - Pointer to audio info.
 *
 * @returns pointer to Audio struct on success, NULL on failure,
 */
PAudio copyAudio(PAudio audio);

/**
 * Set the audio data.
 *
 * @param wav - Pointer to audio info.
 *
 * @param data - The PCM audio data(same bps as PAudio wav)
 *
 * @param size - The size of the data pointed to by data
 *
 * @param sztype - One of SZ_SAMPLES, SZ_BYTES, SZ_SECONDS
 *                 SZ_SAMPLES: Number of samples per channel
 *                 SZ_BYTES: Number of bytes 
 *                 SZ_SECONDS: Number of miliseconds 
 *
 * @returns -1 on failure
 */
int setDataAudio(PAudio wav, void *data, u32 size, u32 sztype);

/**
 * Reverses the audio( try it on some metal:) ).
 *
 * @param audio - Pointer to audio info.
 *
 * @returns 0 on failure 1 on sucess
 *
 * @note this function fails if audio is playing. 
 */
int reverseAudio(PAudio audio);

/**
 * Swap the audio channels L->R, R->L
 *
 * @param audio - Pointer to audio info.
 *
 * @returns 0 on failure 1 on sucess
 *
 * @note this function fails if audio is playing. 
 */
int swapChannelAudio(PAudio audio);

/**
 * Fills the L channel with Zeros
 *
 * @param audio - Pointer to audio info.
 *
 * @returns 0 on failure 1 on sucess
 *
 * @note this function fails if audio is playing. 
 */
int muteLChannelAudio(PAudio audio);

/**
 * Fills the R channel with Zeros
 *
 * @param audio - Pointer to audio info.
 *
 * @returns 0 on failure 1 on sucess
 *
 * @note this function fails if audio is playing. 
 */
int muteRChannelAudio(PAudio audio);
 
/**
 * Fills all audio data with Zeros
 *
 * @param audio - Pointer to audio info.
 *
 * @returns 0 on failure 1 on sucess
 *
 * @note this function fails if audio is playing. 
 */
int muteAudio(PAudio audio); 

/**
 * Get the type of the audio
 *
 * @param audio - Pointer to audio info.
 *
 * @returns id of the audio data
 *
 */
int getTypeAudio(PAudio audio);

/**
 * Get the number of channels
 *
 * @param audio - Pointer to audio info.
 *
 * @returns 1 for mono or 2 for sterio
 *
 */
int getChannelsAudio(PAudio audio);

/**
 * Get the number of bits ina sample(on a single channel)
 *
 * @param audio - Pointer to audio info.
 *
 * @returns 1 for mono or 2 for sterio
 *
 */
u32 getBitsPerSampleAudio(PAudio audio);

/**
 * Get the number of bits used to store 1 second of audio(i think)
 *
 * @param audio - Pointer to audio info.
 *
 * @returns bitrate
 *
 */
u32 getBitrateAudio(PAudio audio);

/**
 * Get the lenght of the audio
 *
 * @param audio - Pointer to audio info.
 *
 * @param type - Specifies what to return 
 *
 * @returns number of samples or miliseconds for SZ_SAMPLES or SZ_SECONDS respectively
 *
 */
u32 getLengthAudio(PAudio audio, int type);

/**
 * Get the current offset
 *
 * @param audio - Pointer to audio info.
 *
 * @param type - Specifies what to return 
 *
 * @returns number of samples or miliseconds for SZ_SAMPLES OR SZ_SECONDS respectively
 *
 */
u32 getTimeAudio(PAudio audio, int type);

/**
 * Seek to a specified location
 *
 * @param audio - Pointer to audio info.
 *
 * @param seekTime - Specifies the how much to seek, interpritation depends on seektype
 *
 * @param seekType - number of samples or miliseconds to seek for SZ_SAMPLES OR SZ_SECONDS respectively
 *
 * @param seekType - SEEK_SET or SEEK_CUR      SEEK_END not supported
 *
 */
void seekAudio(PAudio audio, int seekTime, int seekType, int whence);

/**
 * Increase the volume 
 *
 * @param audio - Pointer to audio info.
 *
 * @param lVolume - Left channel volume.
 *
 * @param rVolume - Right channel volume.
 *
 * @returns 0 on failure 1 on sucess
 *
 */
int increaseVolumeAudio(PAudio audio, int lVolume, int rVolume);

/**
 * Decrease the volume 
 *
 * @param audio - Pointer to audio info.
 *
 * @param lVolume - Left channel volume.
 *
 * @param rVolume - Right channel volume.
 *
 * @returns 0 on failure 1 on sucess
 *
 */
int decreaseVolumeAudio(PAudio audio, int lVolume, int rVolume);

/**
 * Sets the volume 
 *
 * @param audio - Pointer to audio info.
 *
 * @param lVolume - Left channel volume.
 *
 * @param rVolume - Right channel volume.
 *
 * @returns 0 on failure 1 on sucess
 *
 */
int setVolumeAudio(PAudio audio, int lVolume, int rVolume);

/**
 * Sets the volume to Zero
 *
 * @param audio - Pointer to audio info.
 *
 * @returns 0 on failure 1 on sucess
 *
 */
int setVolumeMuteAudio(PAudio audio);

/**
 * Sets the volume to its Maximum(I think)
 *
 * @param audio - Pointer to audio info.
 *
 * @returns 0 on failure 1 on sucess
 *
 */
int setVolumeMaxAudio(PAudio audio); 

/**
 *
 * use the sceAudioSRC* funtions instead of the sceAudio* functions
 *
 * @note: only one audio can play at a time
 * @note: only supports sterio
 * @note: may not be supported by all formats
 * @note: should not call while audio is playing
 *
 */

void enableSRCFunctions();

/**
 *
 * stop using the sceAudioSRC* functions
 * @note: should not call while audio is playing
 *
 */

void disableSRCFunctions();

/**
 *
 * @param audio - Pointer to audio info.
 *
 * @returns STOP if stopped, PLAYING if playing, PAUSED if paused
 *
 */
int getPlayAudio(PAudio audio); 

/**
 *
 * @param audio - Pointer to audio info.
 *
 * @param size - Pointer to variable to recieve mix buffer size in audio samples.
 *
 * @returns pointer to the current buffer
 *
 */
s16 *getMixBufferAudio(PAudio audio, int *size); 

/**
 *
 * @param audio - Pointer to audio info.
 *
 * @returns 1 if playing
 *
 */
int isPlayingAudio(PAudio audio); 

/**
 *
 * @param audio - Pointer to audio info.
 *
 * @returns 1 if stopped
 *
 */
#define isStopedAudio isStoppedAudio
//spelling error
int isStoppedAudio(PAudio audio); 

/**
 *
 * @param audio - Pointer to audio info.
 *
 * @returns 1 if paused
 *
 */
int isPausedAudio(PAudio audio); 

/**
 * Get the last error to occur
 *
 * returns one of AUDIO_ERROR_* (or custom) error codes
 *
 */
#define audiogetLastError()  audiosetLastError(AUDIO_GET_LAST_ERROR)
int audiosetLastError(int error);
#ifndef ONLY_STREAM
#define getAudioChannel() Channel(0, 0)
#define setAudioChannel(ch) Channel(1, ch)
#endif

/**********************************************************************************************\
\**********************************************************************************************/

#ifdef __cplusplus
};
#endif

#endif
