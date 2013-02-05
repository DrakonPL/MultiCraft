/*
 * AudioMPEG.h
 * This file is used to load mpeg audio files
 *
 * MUSAUDIOMPEG Version 1 by mowglisanu
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
#ifndef __MUSAUDIOMPEG_H__
#define __MUSAUDIOMPEG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <pspaudiocodec.h>
#include <psputility_avmodules.h>
#include <psptypes.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "Audio.h"

#define MPEG_ERROR_ERROR AUDIO_ERROR_ERROR
#define MPEG_ERROR_LOADMODULES 256
#define MPEG_ERROR_OPEN_FILE AUDIO_ERROR_OPEN_FILE
#define MPEG_ERROR_INVALID_FILE AUDIO_ERROR_INVALID_FILE
#define MPEG_ERROR_FILE_READ AUDIO_ERROR_FILE_READ
#define MPEG_ERROR_MEM_ALLOC AUDIO_ERROR_MEM_ALLOC
#define MPEG_ERROR_THREAD_FAIL AUDIO_ERROR_THREAD_FAIL
#define MPEG_ERROR_LOAD_MODULES AUDIO_ERROR_LOAD_MODULES
#define MPEG_ERROR_INVALID_ARGUMENT 0x00020000
#define MPEG_ERROR_LAYER_NOT_SUPPORTED 0x00040000
#define MPEG_ERROR_AVCODEC_NEEDMEM 0x00080000
#define MPEG_ERROR_AVCODEC_EDRAM 0x00100000
#define MPEG_ERROR_AVCODEC_INIT  0x00200000
#define MPEG_GET_LAST_ERROR AUDIO_GET_LAST_ERROR

#define MPEG_FLAG_NO_ESTIMATE 0x00000100

#define MADMpegCodec 0x40001002

#define SZ_ALL 0x0000
#define SZ_FRAMES 0x0200

#define TYPE_MEMORY 0
#define TYPE_FILE 1
#define TYPE_HANDLE 3

/*#ifdef BUFFER16
 #define buffers 16
#else 
 #ifdef BUFFER8
  #define buffers 8
 #else 
   #define buffers 4
 #endif
#endif
#define bmod (buffers-1)*/

enum MPEG_AUDIO_VERSION_ID{
     MPEG_VERSION_2_5 = 0,
     MPEG_VERSION_RESERVED = 1,
     MPEG_VERSION_2 = 2,
     MPEG_VERSION_1 = 3
};

enum MPEG_AUDIO_LAYER{
     MPEG_LAYER_RESERVED = 0,
     MPEG_LAYER_3 = 1,
     MPEG_LAYER_2 = 2,
     MPEG_LAYER_1 = 3 
};

//extern int sample_rates[13];
extern int mpeg_frequencies[4][4];

extern int mpeg_samples[4][4];

extern int mpeg_bitrates[5][16];

enum MPEG_AUDIO_CHANNEL{
     STERIO = 0,
     JOINT_STERIO = 1,
     DUAL_CHANNEL = 2,
     SINGLE_CHANNEL = 3
};
typedef struct{
        u32 chunk;
        u32 count;
}stsc_info;
typedef struct{
        u32 size;
        u32 name;
        void *data;
}mp4_atom;
typedef struct{
        u32 offset;
        u32 type;
        u32 mdia;
        u32 mdhd;
        u32 time_scale;
        u64 duration;
        u32 minf;
        u32 stbl;
        u32 stsd;
        u32 channels;
        u32 frequency;
        u32 esds;
        u32 stsc;
        u32 stsc_count;
        stsc_info *spc;
        u32 stsz;
        u32 sample_size;
        u32 *sample_sizes;
        u32 stco;
        u32 chunks;
        u32 *offsets;
        u32 co64;
}mp4_track;
typedef struct{
        u32 ftyp;
        u32 mdat;
        u32 moov;
//        u32 mvhd;
        mp4_track *trak;
}atoms_used;
typedef struct{
        int size;
        StreamEventHandler stream;
        SceUID thid;
        int eof;
        int working;
        int start;
        int fill;
        int frequency;
        void *data;
        unsigned long* mp4_codec_buffer; 
        int type;        
        int channels;
        atoms_used *atoms;         
        u32 track;
        u32 tracks;
        u32 index;
        u32 spc;
        u32 chunk;
        u32 samples;
        u32 index_count;
        u32 sample_size;
        u32 *sample_sizes;
        u32 *offsets;
}mp4_extra;

typedef struct{
        int size;
        StreamEventHandler stream;
        SceUID thid;
        int eof;
        int working;
        int start;
        int fill;
        int frequency;
        void *data;
        int dataSize;
        unsigned long* mp3_codec_buffer; 
        int position;
        int index;
        int type;
        int layer;
        int channels;
        int frameScale;
        int mask;
        int bitrate;
}mp3_extra;

#define mpeggetLastError() mpegsetLastError(MPEG_GET_LAST_ERROR)

#define mpegbase PSP_AV_MODULE_MPEGBASE

#define VBRI 0x49524256

#define MP4A 0xAAC12

#define getLayer(fh) ((fh & 0x0600) >> 9)
#define getType(fh) ((fh & 0x1800) >> 11)
#define getFrequency(fh) ((fh & 0x0c0000) >> 18)
#define getBitrate(fh) ((fh & 0xf00000) >> 20)
#define getPadding(fh) ((fh & 0x020000) >> 17)
#define getChannel(fh) ((fh & 0xc0000000) >> 30)
#define getChannelCount(fh) ((getChannel(fh)==SINGLE_CHANNEL)?1:2)

/**
 * Calculate the length of an mpeg audio stream
 *
 * @param mpeg - Audio stream to calculate length.
 *
 * @NOTE - loadMp3, loadMp3Advance, loadMADMpeg and loadAac only estimates audio length.
 *       - Add the MPEG_FLAG_NO_ESTIMATE flag when calling loadMp3Advance or loadMADMpeg.
 */
void calculateLengthMpeg(PAudio mpeg);

/**
 * Load a mp3 audio stream
 *
 * @param file - Path of the file to load.
 *
 * @param stream - Determines how the audio stream is loaded.
 *
 * @returns pointer to Audio struct on success, NULL on failure,
 */
PAudio loadMp3(char *file, int stream);

/**
 * Load a mp3 audio stream
 *
 * @param file - Path of the file to load.
 *
 * @param stream - Determines how the audio stream is loaded.
 *
 * @param size - Size of audio to load.
 *
 * @param sztype - Determines how size is interpreted.
 *
 * @param flags - Determines file is interpreted(and other functionality).
 *
 * @returns pointer to Audio struct on success, NULL on failure,
 */
PAudio loadMp3Advance(void *file, int stream, int size, u32 sztype, u32 flags);
#ifndef SANE

/**
 * Load a mp3, mp2, mp1 audio stream
 *
 * @param file - Path of the file to load.
 *
 * @param stream - Determines how the audio stream is loaded.
 *
 * @param size - Size of audio to load.
 *
 * @param sztype - Determines how size is interpreted.
 *
 * @param flags - Determines file is interpreted(and other functionality).
 *
 * @returns pointer to Audio struct on success, NULL on failure,
 */
PAudio loadMADMpeg(void *file, int stream, int size, u32 sztype, u32 flags);
#endif

/**
 * Load an aac/adts audio stream
 *
 * @param file - Path of the file to load.
 *
 * @param stream - Determines how the audio stream is loaded.
 *
 * @returns pointer to Audio struct on success, NULL on failure,
 */
PAudio loadAac(const char *filename, int stream);

/**
 * Load aac tracks from an mp4 file
 *
 * @param file - Path of the file to load.
 *
 * @returns pointer to Audio struct on success, NULL on failure,
 */
PAudio loadMp4(const char *filename);

/**
 * Get the numder of loaded tracks
 *
 * @param mp4 - mp4 Audio.
 *
 * @returns loaded tracks.
 */
int getTrackCountMp4(PAudio mp4);

/**
 * Get the current tracks
 *
 * @param mp4 - mp4 Audio.
 *
 * @NOTE - 0 based index.
 */
int getTrackMp4(PAudio mp4);

/**
 * Set the current tracks
 *
 * @param mp4 - mp4 Audio.
 *
 * @param track - track to set.
 *
 * @NOTE - 0 based index.
 *         If mp4 is playing, may cause playback failure.
 */
void setTrackMp4(PAudio mp4, int track);
#ifdef NO_EXCLUSION
int getFrequencyMpeg(const PAudio mpeg);//use getFrequencyAudio
#endif
int getBitrateMpeg(const PAudio mpeg);


#ifdef __cplusplus
}
#endif

#endif
          
