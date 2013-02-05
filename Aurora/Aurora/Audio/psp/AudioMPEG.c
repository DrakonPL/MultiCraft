/*
 * AudioMPEG.c
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
#include "AudioMPEG.h"

static int sample_rates[13] = {96000,88200,64000,48000,44100,32000,24000,22050,16000,12000,11025,8000, 7350};//for aac, 12000 and 7350 not supported by ME
int mpeg_frequencies[4][4] = {{11025, 12000, 8000, 0},
                              {0, 0, 0, 0},
                              {22050, 24000, 16000, 0},
                              {44100, 48000, 32000, 0}};//for mp321

int mpeg_samples[4][4] = {{0, 576, 1152, 384},
                          {0, 0, 0, 0},
                          {0, 576, 1152, 384},
                          {0, 1152, 1152, 384}}; 

int mpeg_bitrates[5][16] = {{0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448 , 0},
                            {0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 0},
                            {0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0},
                            {0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256, 0},
                            {0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0}};

extern int audiosetLastError(int error);
extern void register_audio(PAudio audio);
int mpegsetLastError(int error){
    static SceUID err = 0;
    if (error & MPEG_GET_LAST_ERROR)
       return err | MPEG_ERROR_ERROR;
    int ret = err;
    err = error;
    audiosetLastError(error);
    return ret;
}

int mpegInit(){
    if (!loadAudioCodec(PSP_AV_MODULE_AVCODEC)){
       mpegsetLastError(MPEG_ERROR_LOAD_MODULES);
       return 0;
    }
    return 1;
}
#ifdef NO_EXCLUSION
int getFrequencyMpeg(const PAudio mpeg){
    if (!mpeg) return 0;   
    mp3_extra *extra = (mp3_extra*)mpeg->extra;
    if ((mpeg->type == MADMpegCodec) || (mpeg->type == SceMp3Codec) || (mpeg->type == SceAacCodec))
       return extra->frequency;
    mpegsetLastError(MPEG_ERROR_INVALID_ARGUMENT);
    return 0;
}
#endif
int getAacSizefd(SceUID fd, int type);
int getMp3Sizefd(SceUID fd, int type);
int getMp3SizeFILE(FILE *file, int type);
int getMp3SizeMEMORY(void *data, int type, int dataSize);
void calculateLengthMpeg(PAudio mpeg){
     if (!mpeg) return;   
     if (mpeg->play == PLAYING) return;
     mp3_extra *extra = (mp3_extra*)mpeg->extra;
     int cur;
     if (mpeg->type == SceAacCodec){//if file is open
        cur = sceIoLseek32(mpeg->file, 0, PSP_SEEK_CUR);
        sceIoLseek32(mpeg->file, extra->start, PSP_SEEK_SET);
        mpeg->size = getAacSizefd(mpeg->file, SZ_SAMPLES);
        sceIoLseek32(mpeg->file, cur, PSP_SEEK_SET);
     }
    else if ((mpeg->type == MADMpegCodec) || (mpeg->type == SceMp3Codec)){
       if (mpeg->flags&AUDIO_FLAG_SCEUID){//if file is open
          cur = sceIoLseek32(mpeg->file, 0, PSP_SEEK_CUR);
          sceIoLseek32(mpeg->file, extra->start, PSP_SEEK_SET);
          mpeg->size = getMp3Sizefd(mpeg->file, SZ_SAMPLES);
          sceIoLseek32(mpeg->file, cur, PSP_SEEK_SET);                                 
       }                               
       else if (mpeg->flags&AUDIO_FLAG_FILE){//if file is open
          cur = ftell((FILE*)mpeg->file);
          fseek((FILE*)mpeg->file, extra->start, SEEK_SET);
          mpeg->size = getMp3SizeFILE((FILE*)mpeg->file, SZ_SAMPLES);
          fseek((FILE*)mpeg->file, cur, SEEK_SET);                                 
       }                               
       else if (mpeg->flags&AUDIO_FLAG_MEMORY){
          mpeg->size = getMp3SizeMEMORY((void*)mpeg->file+extra->start, SZ_SAMPLES, extra->dataSize);                          
       }                                                                 
    }
    else mpegsetLastError(MPEG_ERROR_INVALID_ARGUMENT);
}
int getBitrateMpeg(const PAudio mpeg){
    if (!mpeg) return 0;    
    mp3_extra *extra = (mp3_extra*)mpeg->extra;
    if ((mpeg->type == MADMpegCodec) || (mpeg->type == SceMp3Codec) || (mpeg->type == SceAacCodec))
       return extra->bitrate;
    mpegsetLastError(MPEG_ERROR_INVALID_ARGUMENT);
    return 0;
}

int getFrameSize(int HMask){
    int type = getType(HMask);
    int layer = getLayer(HMask);
    int frequency = mpeg_frequencies[type][getFrequency(HMask)];
    int index;
    if (type == MPEG_VERSION_1)
       index = 3 - layer;
    else{
       if (layer == MPEG_LAYER_1) index = 3;
       else index = 4;
    }
    int bitrate = getBitrate(HMask);
    if ((frequency == 0)||(bitrate == 0)||(layer == 0)||(type == 1)){
       mpegsetLastError(MPEG_ERROR_INVALID_ARGUMENT);
       return 0;
    }
    int frameScale;
    if (layer == MPEG_LAYER_1)
       frameScale = 12*1000;
    else if (layer == MPEG_LAYER_3 && (type == MPEG_VERSION_2 || type == MPEG_VERSION_2_5))
       frameScale = 72*1000;
    else
       frameScale = 144*1000;
    return frameScale*mpeg_bitrates[index][bitrate]/frequency + getPadding(HMask);
}

int seekNextFrame(SceUID fd, int syncMask){
    int Mask = 0x0000e0ff;
    int buffer[256];
    u8 *ptr = (u8*)&buffer;
    int read;
    int begin, offset;
    begin = sceIoLseek32(fd, 0, PSP_SEEK_CUR);
    offset = begin;
#ifdef MPEG_DEBUG
    printf("seekNextFrame begin %d\n", begin);
#endif
    int i;
    while (1){
          read = sceIoRead(fd, &buffer, sizeof(buffer));
#ifdef MPEG_DEBUG
          printf("seekNextFrame read %d\n", read);
#endif
          if (read < 12){// smallest frame size i think
             sceIoLseek32(fd, begin, PSP_SEEK_SET);
             return -1;
          }
          if (syncMask){
             for (i = 0; i < read; i++){
                 int fh = ptr[i]|ptr[i+1]<<8|ptr[i+2]<<16|ptr[i+3]<<24;
                 if ((fh & 0xc00cfeff)  == syncMask){
/*                    if (getLayer(fh == MPEG_LAYER_2){
                       //CHECK INVALID COMBINATIONS
                    }*/
#ifdef MPEG_DEBUG
                    printf("seekNextFrame fh 0x%08x offset %d mask 0x%08x\n", fh, offset + i,syncMask);
#endif
                    sceIoLseek32(fd, offset + i, PSP_SEEK_SET);
                    return offset+i;
                 }
             }
          
          }
          else for (i = 0; i < read; i++){
             int fh = ptr[i]|ptr[i+1]<<8|ptr[i+2]<<16|ptr[i+3]<<24;
             if ((fh & Mask) == Mask){
                if ((getBitrate(fh) == 0xf)||(getBitrate(fh) == 0x0))
                   continue;
                if (getFrequency(fh) == 0x3)
                   continue;
                if (getType(fh) == MPEG_VERSION_RESERVED)
                   continue;
                if (getLayer(fh) == MPEG_LAYER_RESERVED)
                   continue;
/*                if (getLayer(fh) == MPEG_LAYER_2){
                       //CHECK INVALID COMBINATIONS
                }*/
                sceIoLseek32(fd, offset + i, PSP_SEEK_SET);
#ifdef MPEG_DEBUG
                printf("seekNextFrame fh 0x%08x offset %d\n", fh, offset + i);
                if (offset+i>0x1000) return -1;
#endif
                return offset+i;
             }
          }   
          offset += read;
    }
    return -1;
}
#ifndef USE_ONLY_SCE_IO
int seekNextFrameFILE(FILE *fd, int syncMask){
    int Mask = 0x0000e0ff;
    int buffer[256];
    u8 *ptr = (u8*)&buffer;
    int read;
    int begin, offset;
    begin = ftell(fd);
    offset = begin;
#ifdef MPEG_DEBUG
    printf("seekNextFrame begin %d\n", begin);
#endif
    int i;
    while (1){
          read = fread(&buffer, 1, sizeof(buffer), fd);
#ifdef MPEG_DEBUG
          printf("seekNextFrame read %d\n", read);
#endif
          if (read < 12){// smallest frame size i think
             read = fseek((FILE*)fd, begin, SEEK_SET);
             return -1;
          }
          if (syncMask){
             for (i = 0; i < read; i++){
                 int fh = ptr[i]|ptr[i+1]<<8|ptr[i+2]<<16|ptr[i+3]<<24;
                 if ((fh & 0xc00cfeff)  == syncMask){
/*                    if (getLayer(fh == MPEG_LAYER_2){
                       //CHECK INVALID COMBINATIONS
                    }*/
#ifdef MPEG_DEBUG
                    printf("seekNextFrame fh 0x%08x offset %d mask 0x%08x\n", fh, offset + i,syncMask);  
#endif
                    read = fseek((FILE*)fd, offset + i, SEEK_SET);
                   return offset+i;
                 }
             }
          
          }
          else for (i = 0; i < read; i++){
             int fh = ptr[i]|ptr[i+1]<<8|ptr[i+2]<<16|ptr[i+3]<<24;
             if ((fh & Mask) == Mask){
                if ((getBitrate(fh) == 0xf)||(getBitrate(fh) == 0x0))
                   continue;
                if (getFrequency(fh) == 0x3)
                   continue;
                if (getType(fh) == MPEG_VERSION_RESERVED)
                   continue;
                if (getLayer(fh) == MPEG_LAYER_RESERVED)
                   continue;
//                if (getLayer(fh == MPEG_LAYER_2){
//                       //CHECK INVALID COMBINATIONS
//                }
                read = fseek((FILE*)fd, offset + i, SEEK_SET);
#ifdef MPEG_DEBUG
                printf("seekNextFrame fh 0x%08x offset %d\n", fh, offset + i);
#endif
                return offset+i;
             }
          }   
          offset += read;
    }
    return -1;
}
#endif
int seekNextFrameMEMORY(void *fd, int syncMask, int size){
    int Mask = 0x0000e0ff;
    u8 *ptr = (u8*)fd;
    int read;
    int begin, offset;
    begin = 0;
    offset = begin;
#ifdef MPEG_DEBUG
    printf("seekNextFrame begin %d\n", begin);
#endif
    int i;
    while (1){
          if (offset >= size) 
             return -1;
          else if (offset+1024 > size) 
             read = size-offset;
          else
             read = 1024;
          ptr = (u8*)fd+offset;
#ifdef MPEG_DEBUG
          printf("seekNextFrame read %d\n", read);
#endif
          if (read < 12){// smallest frame size i think
             return -1;
          }
          if (syncMask){
             for (i = 0; i < read; i++){
                 int fh = ptr[i]|ptr[i+1]<<8|ptr[i+2]<<16|ptr[i+3]<<24;
                 if ((fh & 0xc00cfeff)  == syncMask){
//                    if (getLayer(fh == MPEG_LAYER_2){
//                       //CHECK INVALID COMBINATIONS
//                    }
#ifdef MPEG_DEBUG
                    printf("seekNextFrame fh 0x%08x offset %d mask 0x%08x\n", fh, offset + i,syncMask);  
#endif
                    return offset+i;
                 }
             }
          }
          else for (i = 0; i < read; i++){
             int fh = ptr[i]|ptr[i+1]<<8|ptr[i+2]<<16|ptr[i+3]<<24;
             if ((fh & Mask) == Mask){
                if ((getBitrate(fh) == 0xf)||(getBitrate(fh) == 0x0))
                   continue;
                if (getFrequency(fh) == 0x3)
                   continue;
                if (getType(fh) == MPEG_VERSION_RESERVED)
                   continue;
                if (getLayer(fh) == MPEG_LAYER_RESERVED)
                   continue;
//                if (getLayer(fh == MPEG_LAYER_2){
//                       //CHECK INVALID COMBINATIONS
//                }
#ifdef MPEG_DEBUG
                printf("seekNextFrame fh 0x%08x offset %d\n", fh, offset + i);
#endif
                return offset+i;
             }
          }   
          offset += read;
    }
    return -1;
}

int estimateMp3Size(char *file, int type){
    if (!file) return -1;
    SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);
    if (fd < 0){
       printf("eMs open fail %s %x\n", file, fd);
       sceIoClose(fd);
       mpegsetLastError(MPEG_ERROR_OPEN_FILE);
       return -1;
    }
    if (seekNextFrame(fd, 0) < 0){
       printf("eMs invalid %s\n", file);
       sceIoClose(fd);
       mpegsetLastError(MPEG_ERROR_INVALID_FILE);
       return 0;
    }
    int start = sceIoLseek32(fd, 0, PSP_SEEK_CUR);
    int end = sceIoLseek32(fd, 0, PSP_SEEK_END);
    int id3;
    sceIoLseek32(fd, -128, PSP_SEEK_END);
    sceIoRead(fd, &id3, 4);
    if (id3 == '\03DI')//0x00334449
       end -= 128;
    int size = end - start;
    
    sceIoLseek32(fd, start, PSP_SEEK_SET);
    int HMask;
    sceIoRead(fd, &HMask, 4);
    sceIoClose(fd);
    
    int samples_per_frame = mpeg_samples[getType(HMask)][getLayer(HMask)];
    int frequency = mpeg_frequencies[getType(HMask)][getFrequency(HMask)];
    int frames = size/getFrameSize(HMask);
    switch (type){
       case SZ_SAMPLES :
          return frames*samples_per_frame;
       case SZ_SECONDS :
          return frames*samples_per_frame/frequency;
       case SZ_FRAMES :
          return frames;
       case SZ_BYTES :
          return size;
    }
    mpegsetLastError(MPEG_ERROR_INVALID_ARGUMENT);
    return 0;    
}
int getMp3Sizefd(SceUID fd, int type){
    if (fd < 0){
       printf("Load %x fail\n", fd);
       mpegsetLastError(MPEG_ERROR_OPEN_FILE);
       return -1;
    }
    int start = 0;
    if (seekNextFrame(fd, 0) < 0){
       printf("getMp3Size seekNextFrame fail\n");
       sceIoClose(fd);
       mpegsetLastError(MPEG_ERROR_INVALID_FILE);
       return 0;
    }
    int HMask;
    start = sceIoLseek32(fd, 0, PSP_SEEK_CUR);
    sceIoRead(fd, &HMask, 4);
    int samples_per_frame = mpeg_samples[getType(HMask)][getLayer(HMask)];
    int frames = 0;
    int fullsize = 0;
    int frequency;
    while (1){//de usual
          int size = getFrameSize(HMask);
          fullsize += size;
          frames++;          
          sceIoLseek32(fd, size-4, PSP_SEEK_CUR);
          if (sceIoRead(fd, &HMask, 4) <= 0) break;
          while (((HMask & 0x0000e0ff)  != 0x0000e0ff)||
             (getBitrate(HMask) == 0xf)||
             (getFrequency(HMask) == 0x3)){
             if (seekNextFrame(fd, 0) < 0){
                printf("getMp3Size seekNextFrame fail2 %d\n", frames);
                 goto afterwhile;
             }
             if (sceIoRead(fd, &HMask, 4) <= 0) goto afterwhile;
          }
    }
    afterwhile:
    sceIoLseek32(fd, start, PSP_SEEK_SET);
    frequency = mpeg_frequencies[type][getFrequency(HMask)];
    switch (type){
       case SZ_SAMPLES :
          return frames*samples_per_frame;
       case SZ_SECONDS :
          return frames*samples_per_frame/frequency;
       case SZ_FRAMES :
          return frames;
       case SZ_BYTES :
          return fullsize;
    }
    mpegsetLastError(MPEG_ERROR_INVALID_ARGUMENT);
    return 0;    
}
int getMp3Size(const char *file, int type){
    if (!file) return -1;
    SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);
    int res = getMp3Sizefd(fd, type);
    sceIoClose(fd);
    return res;       
}
#ifndef USE_ONLY_SCE_IO
int getMp3SizeFILE(FILE *file, int type){
    if (!file) return -1;
    int start = 0;
    start = ftell(file);
    if (seekNextFrameFILE(file, 0) < 0){
       printf("getMp3Size seekNextFrame fail\n");
       mpegsetLastError(MPEG_ERROR_INVALID_FILE);
       return 0;
    }
    int HMask;
    fread(&HMask, 4, 1, file);
    int samples_per_frame = mpeg_samples[getType(HMask)][getLayer(HMask)];
    int frames = 0;
    int fullsize = 0;
    while (1){//de usual
          int size = getFrameSize(HMask);
          fullsize += size;
          frames++;          
          fseek((FILE*)file, size-4, SEEK_CUR);
          if (fread(&HMask, 4, 1, file) <= 0) break;
          while (((HMask & 0x0000e0ff)  != 0x0000e0ff)||
             (getBitrate(HMask) == 0xf)||
             (getFrequency(HMask) == 0x3)){
             if (seekNextFrameFILE(file, 0) < 0){
                printf("getMp3Size seekNextFrame fail2 %d\n", frames);
                 goto afterwhile;
             }
             if (fread(&HMask, 4, 1, file) <= 0) goto afterwhile;
          }
    }
    afterwhile:
    fseek((FILE*)file, start, SEEK_SET);
    int frequency = mpeg_frequencies[type][getFrequency(HMask)];
    switch (type){
       case SZ_SAMPLES :
          return frames*samples_per_frame;
       case SZ_SECONDS :
          return frames*samples_per_frame/frequency;
       case SZ_FRAMES :
          return frames;
       case SZ_BYTES :
          return fullsize;
    }
    mpegsetLastError(MPEG_ERROR_INVALID_ARGUMENT);
    return 0;    
}
#endif
int getMp3SizeMEMORY(void *data, int type, int dataSize){
    if (!data) return -1;
    int start;
    if ((start = seekNextFrameMEMORY(data, 0, dataSize)) < 0){
       printf("getMp3Size seekNextFrame fail\n");
       mpegsetLastError(MPEG_ERROR_INVALID_ARGUMENT);
       return 0;
    }
    int HMask;
    memcpy(&HMask, data, 4);
    int samples_per_frame = mpeg_samples[getType(HMask)][getLayer(HMask)];
    int frames = 0;
    int fullSize = 0;
    int frequency;
    while (1){//de usual
          int size = getFrameSize(HMask);
          fullSize += size;
          frames++;          
          if (fullSize >= dataSize)
             break;
          else
             memcpy(&HMask, data+fullSize+start, 4);
          while (((HMask & 0x0000e0ff)  != 0x0000e0ff)||
             (getBitrate(HMask) == 0xf)||
             (getFrequency(HMask) == 0x3)){
             if ((size = seekNextFrameMEMORY(data, 0, dataSize)) < 0){
                printf("getMp3Size seekNextFrame fail2 %d\n", frames);
                goto afterwhile;
             }
             fullSize += size;
             if (fullSize >= dataSize)
                goto afterwhile;
             else
                memcpy(&HMask, data+fullSize+start, 4);
          }
    }
    afterwhile:
    frequency = mpeg_frequencies[type][getFrequency(HMask)];
    switch (type){
       case SZ_SAMPLES :
          return frames*samples_per_frame;
       case SZ_SECONDS :
          return frames*samples_per_frame/frequency;
       case SZ_FRAMES :
          return frames;
       case SZ_BYTES :
          return fullSize;
    }
    mpegsetLastError(MPEG_ERROR_INVALID_ARGUMENT);
    return 0;    
}
    
int getAacFrameSize(const unsigned char *aac_header_buf){
    int aac_header = aac_header_buf[3]; 
    aac_header = (aac_header<<8) | aac_header_buf[4]; 
    aac_header = (aac_header<<8) | aac_header_buf[5]; 
    aac_header = (aac_header<<8) | aac_header_buf[6];
#ifdef REC_AAC_INFO    
    int no_raw_data_blocks_in_frame = aac_header&0x3;
    int adts_buffer_fullness = (aac_header&0x1ffc)>>2;
#endif
    int frame_size = aac_header & 0x3FFE000;//67100672; 
    frame_size = frame_size >> 13; 
#ifdef REC_AAC_INFO    
    {
       FILE *tst = fopen("aac info.txt", "a");
       fprintf(tst, "frame size %d\r\n#data blocks %d\r\nfullness %d\r\n frequency %d\r\n\r\n\r\n", frame_size, no_raw_data_blocks_in_frame, adts_buffer_fullness, (aac_header_buf[2]&0x3c)>>2);
       fclose(tst);
    }
#endif
    return frame_size;
}

int findAacSync(SceUID fd){
    unsigned char adts[256];
    int reads = 0;
    int read;
    while ((read = sceIoRead(fd, adts, sizeof(adts))) > 0){
          int count = 0;
          int nibbles = 0;
          reads += read;
          while (count < read){
                if (nibbles){
                   if ((adts[count]&0xf6) == 0xf0){
                      sceIoLseek32(fd, (count-1)-read, SEEK_CUR);
                      //int l = sceIoLseek32(fd, (count-1)-read, SEEK_CUR);
                      //printf("sync = %d\n0x%02x%02x\n", l, adts[count-1], adts[count]);
                      return read+((count-1)-read);
                   }
                   else
                      nibbles = 0;
                }
                else if (adts[count] == 0xff)
                   nibbles = 2;
                count++;
          }
    }
    sceIoLseek32(fd, -1*reads, SEEK_CUR);
    //int l = sceIoLseek32(fd, -1*reads, SEEK_CUR);
    //printf("!sync = %d\n", l);
    return -1;
}
#ifndef USE_ONLY_SCE_IO
int findAacSyncFILE(FILE *file){//NOT USED
    unsigned char adts[7];
    int reads = 0;
    while (fread(adts, 1, sizeof(adts), file) == sizeof(adts)){
          int count = 0;
          int nibbles = 0;
          while (count < sizeof(adts)){
                if (nibbles){
                   if ((adts[count]&0xf0) == 0xf0){
                      fseek((FILE*)file, -1*(count-1), SEEK_CUR);
                      return count-1;
                   }
                   else
                      nibbles = 0;
                }
                else if (adts[count] == 0xff)
                   nibbles = 2;
                count++;
          }
          reads++;
    }
    fseek((FILE*)file, -7*(reads), SEEK_CUR);
    return -1;
}
#endif
int  findAacSyncMEMORY(const void *data, int size){
     unsigned char *sync = (unsigned char*)data;
     int count = 0;
     int nibbles = 0;
     size -= 7;
     while (count < size){
           if (nibbles){
              if ((sync[count]&0xf0) == 0xf0)
                 return count-1;
              else
                 nibbles = 0;
           }
           else if (sync[count] == 0xff)
              nibbles = 2;
           count++;
     }
     return -1;
}

int estimateAacSize(const char *file, int type){//check for valid aac file, gives a really bad estimate
    if (!file) return -1;
    SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);
    if (fd < 0){
       mpegsetLastError(MPEG_ERROR_OPEN_FILE);
       return -1;
    }
    if (findAacSync(fd) < 0){
       mpegsetLastError(MPEG_ERROR_INVALID_FILE);
       return -1;
    }
    unsigned char aac_header_buf[7]; 
    if (sceIoRead(fd, aac_header_buf, 7) != 7) { 
       return 0;
    }
    int frame_size = getAacFrameSize(aac_header_buf);
    
    int size = sceIoLseek32(fd, 0, PSP_SEEK_END);
    sceIoClose(fd);

    int frames = size/frame_size;
    switch (type){
       case SZ_SAMPLES :
          return frames*1024;//fix when samples per frame an frequency is found, other frame sizes might not be supported anyway
       case SZ_SECONDS :
          return frames*1024/sample_rates[(aac_header_buf[2]&0x3c)>>2];
       case SZ_FRAMES :
          return frames;
       case SZ_BYTES :
          return size;
    }
//    return rand();    
    mpegsetLastError(MPEG_ERROR_INVALID_ARGUMENT);
    return 0;    
}
int getAacSizefd(SceUID fd, int type){//check for valid aac file
    int samples_per_frame = 1024;//see above
    int frames = 0;
    int fullsize = 0;
    if (findAacSync(fd) < 0){
       mpegsetLastError(MPEG_ERROR_INVALID_FILE);
       sceIoClose(fd);
       return 0;
    }
    int start = sceIoLseek32(fd, 0, PSP_SEEK_CUR);
    unsigned char aac_header_buf[7]; 
    sceIoRead(fd, aac_header_buf, 7);
    int frame_size = getAacFrameSize(aac_header_buf);
    sceIoLseek32(fd, frame_size-7, PSP_SEEK_CUR);
    int index = (aac_header_buf[2]&0x3c)>>2;
    if (index >= 13){
       mpegsetLastError(MPEG_ERROR_LAYER_NOT_SUPPORTED);
       sceIoLseek32(fd, start, PSP_SEEK_SET);
       return 0;//although 15 is valid ... whatever
    }

    while (1){
          frames++;          
          fullsize += frame_size;
          read:
          if (sceIoRead(fd, aac_header_buf, 7) != 7){ 
             break;
          }
          if ((aac_header_buf[0] != 0xff) || ((aac_header_buf[1] & 0xf6) != 0xf0)){
             if (findAacSync(fd) < 0) break;
             goto read;          
          }
          frame_size = getAacFrameSize(aac_header_buf);
          sceIoLseek32(fd, frame_size-7, PSP_SEEK_CUR);
    }
          
    sceIoLseek32(fd, start, PSP_SEEK_SET);
    int frequency = sample_rates[index];
    switch (type){
       case SZ_SAMPLES :
          return frames*samples_per_frame;
       case SZ_SECONDS :
          return frames*samples_per_frame/frequency;
       case SZ_FRAMES :
          return frames;
       case SZ_BYTES :
          return fullsize;
    }
    mpegsetLastError(MPEG_ERROR_INVALID_ARGUMENT);
    return 0; 
}
int getAacSize(const char *file, int type){
    if (!file) return 0;
    SceUID fd = sceIoOpen(file, PSP_O_RDONLY, 0777);
    if (fd < 0){
       printf("Load %s fail\n", file);
       sceIoClose(fd);
       mpegsetLastError(MPEG_ERROR_OPEN_FILE);
       return 0;
    }
    int ret = getAacSizefd(fd, type);
    sceIoClose(fd);
    return ret;
}

#ifndef USE_ONLY_SCE_IO
int getAacSizeFILE(FILE *file, int type){//check for valid aac file
    if (!file) return -1;
    int samples_per_frame = 1024;//see above
    int frames = 0;
    int fullsize = 0;
    if (findAacSyncFILE(file) < 0){
       mpegsetLastError(MPEG_ERROR_INVALID_FILE);
       return -1;
    }
    int start = ftell(file);
    unsigned char aac_header_buf[7]; 
    fread(aac_header_buf, 1, 7, file);
    int index = (aac_header_buf[2]&0x3c)>>2;
    if (index >= 13){
       mpegsetLastError(MPEG_ERROR_LAYER_NOT_SUPPORTED);
       return -1;//although 15 is valid ... whatever
    }

    while (1){//de usual
          int frame_size = getAacFrameSize(aac_header_buf);
          fullsize += frame_size;
          read:
          if (fread(aac_header_buf, 1, 7, file) <= 0){ 
             break;
          }
          if ((aac_header_buf[0] |= 0xff) || ((aac_header_buf[1] & 0xf6) != 0xf0)){
             if (findAacSyncFILE(file) < 0) break;
             goto read;          
          }
          fseek(file, frame_size-7, SEEK_CUR);
          frames++;          
    }
    
    fseek(file, start, SEEK_SET);      
    int frequency = sample_rates[index];
    switch (type){
       case SZ_SAMPLES :
          return frames*samples_per_frame;
       case SZ_SECONDS :
          return frames*samples_per_frame/frequency;
       case SZ_FRAMES :
          return frames;
       case SZ_BYTES :
          return fullsize;
    }
    mpegsetLastError(MPEG_ERROR_INVALID_ARGUMENT);
    return 0; 
}
#endif

int getAacSizeMEMORY(void *data, int type, int size){//may need fixing, if ever used
    if (!data) return -1;
    int samples_per_frame = 1024;//see above
    int frames = 0;
    int fullSize = 0;
    int start;
    if ((start = findAacSyncMEMORY(data, size)) < 0){
       mpegsetLastError(MPEG_ERROR_INVALID_FILE);
       return -1;
    }
    unsigned char *aac_header_buf; 
    aac_header_buf = data+start;
    int index = (aac_header_buf[2]&0x3c)>>2;
    if (index >= 13){
       mpegsetLastError(MPEG_ERROR_LAYER_NOT_SUPPORTED);
       return -1;//although 15 is valid ... whatever
    }

    while (1){
          int frame_size = getAacFrameSize(aac_header_buf);
          fullSize += frame_size;
          read:
          if (start+fullSize >= size){ 
             break;
          }
          else
             aac_header_buf = data+start+fullSize;
          if ((aac_header_buf[0] |= 0xff) || ((aac_header_buf[1] & 0xf6) != 0xf0)){
             int offset = findAacSyncMEMORY(data, size-(start+fullSize));
             if (offset < 0) break;
             fullSize += offset;
             goto read;          
          }
          frames++;          
    }
          
    int frequency = sample_rates[index];
    switch (type){
       case SZ_SAMPLES :
          return frames*samples_per_frame;
       case SZ_SECONDS :
          return frames*samples_per_frame/frequency;
       case SZ_FRAMES :
          return frames;
       case SZ_BYTES ://:P
          return fullSize;
    }
    mpegsetLastError(MPEG_ERROR_INVALID_ARGUMENT);
    return 0; 
}

void mp3_reset(PAudio mp3){
     if (!mp3) return;
     mp3_extra *extra = (mp3_extra*)mp3->extra;
     if (mp3->flags&AUDIO_FLAG_SCEUID)
        sceIoLseek32(mp3->file, extra->start, PSP_SEEK_SET);
#ifndef USE_ONLY_SCE_IO
     else if (mp3->flags&AUDIO_FLAG_FILE)
        fseek((FILE*)mp3->file, extra->start, SEEK_SET);
#endif
     else if (mp3->flags&AUDIO_FLAG_MEMORY)
        extra->position = 0;
     mp3->offset = 0;   
     extra->fill = 0;    
}
void mp4_reset(PAudio mp4){
     if (!mp4) return;
     mp4_extra *extra = (mp4_extra*)mp4->extra;
     mp4->offset = 0;   
     extra->fill = 0;
     extra->index = 0;
     extra->chunk = 0;
     extra->spc = 0;
     extra->samples = 0;
}

 void mp4_seek(PAudio mp4, int offset){
     if (!mp4) return;
     if (offset < 0) return;
     if (offset >= mp4->size) return;
     mp4_extra *extra = (mp4_extra*)mp4->extra;
     extra->fill = 0;
     int index = offset/1024;
     if (index == 0){
        mp4_reset(mp4);
        return;
     }
     mp4->offset = index*1024;
     extra->index = index;
     if (extra->atoms->trak[extra->track].stsc_count == 1){
        extra->chunk = index/extra->atoms->trak[extra->track].spc[0].count;
        int extras = index%extra->atoms->trak[extra->track].spc[0].count;
        int seek = extra->atoms->trak[extra->track].offsets[extra->chunk];
        int ss_index = index-1;
        while (extras){
              if (extra->sample_size)
                 seek += extra->atoms->trak[extra->track].sample_size;
              else{
                 seek += extra->atoms->trak[extra->track].sample_sizes[ss_index];
                 ss_index--;
              }
              extras--;
        }
        extra->spc = 1;
        sceIoLseek32(mp4->file, seek, PSP_SEEK_SET);
        return;
     }
     int tmp = 0, prev, i = 0, co = 0;
     while (index > tmp){
           prev = tmp;
           int chunks = (extra->atoms->trak[extra->track].spc[i+1].chunk-extra->atoms->trak[extra->track].spc[i].chunk);
           co += chunks;
           tmp += chunks*extra->atoms->trak[extra->track].spc[i].count;
           i++;
     }
     extra->spc = i;
     co -= ((tmp-prev)/extra->atoms->trak[extra->track].spc[i-1].count)-((index-prev)/extra->atoms->trak[extra->track].spc[i-1].count);
     extra->samples = extra->atoms->trak[extra->track].spc[i-1].count-(index-prev)%extra->atoms->trak[extra->track].spc[i-1].count;
     extra->chunk = co;
     if (index != tmp){
        int extras = (index-prev)%extra->atoms->trak[extra->track].spc[i-1].count;
        int seek = extra->atoms->trak[extra->track].offsets[extra->chunk];
        int ss_index = index-1;
        while (extras){
              if (extra->sample_size)
                 seek += extra->atoms->trak[extra->track].sample_size;
              else{
                 seek += extra->atoms->trak[extra->track].sample_sizes[ss_index];
                 ss_index--;
              }
              extras--;
        }
        sceIoLseek32(mp4->file, seek, PSP_SEEK_SET);
     }
}
int getTrackCountMp4(PAudio mp4){
    if (!mp4) return 0;
    if (mp4->type != MP4A) return -1;
    return ((mp4_extra*)mp4->extra)->tracks;
}
int getTrackMp4(PAudio mp4){
    if (!mp4) return 0;
    if (mp4->type != MP4A) return -1;
    return ((mp4_extra*)mp4->extra)->track;
}
void setTrackMp4(PAudio mp4, int track){
     if (!mp4) return;
     if (mp4->type != MP4A) return;
     mp4_extra *extra = (mp4_extra*)mp4->extra;
     if (track == extra->track)
        return;
     if (track < 0 || track >= extra->tracks){
     //invalid arguement
        printf("invalid arguement\n");
        return;
     }
     extra->track = track;
     mp4->frequency = extra->atoms->trak[track].frequency;
     mp4->scale = (float)mp4->frequency/(float)getsysFrequency();
     mp4->size = round(((double)extra->atoms->trak[track].duration/(float)extra->atoms->trak[track].time_scale)*(double)mp4->frequency);
     extra->sample_size = extra->atoms->trak[track].sample_size;
     extra->sample_sizes = extra->atoms->trak[track].sample_sizes;
     extra->index_count = mp4->size/mp4->sampleCount;
     extra->offsets = extra->atoms->trak[track].offsets;
     mp4->offset = 0;   
     extra->fill = 0;
     extra->index = 0;
     extra->chunk = 0;
     extra->spc = 0;
     extra->samples = 0;
     extra->frequency = mp4->frequency;
     extra->channels = extra->atoms->trak[track].channels;
     int ret;
     sceAudiocodecReleaseEDRAM(extra->mp4_codec_buffer);
     memset(extra->mp4_codec_buffer, 0, sizeof(unsigned long)*65);
     memset(mp4->data, 0, mp4->sampleCount*4*5);
     if ((ret = sceAudiocodecCheckNeedMem(extra->mp4_codec_buffer, SceAacCodec)) < 0) {
        printf("sceAudiocodecCheckNeedMem = 0x%08x\n", ret);  
        mpegsetLastError(MPEG_ERROR_AVCODEC_NEEDMEM);
        return;
     }
     if ((ret = sceAudiocodecGetEDRAM(extra->mp4_codec_buffer, SceAacCodec)) < 0) {
        printf("sceAudiocodecGetEDRAM = 0x%08x\n", ret);
        mpegsetLastError(MPEG_ERROR_AVCODEC_EDRAM);
        return;
     }
     extra->mp4_codec_buffer[10] = mp4->frequency;
     if ((ret = sceAudiocodecInit(extra->mp4_codec_buffer, SceAacCodec)) < 0) { // 0x807f00ff bad frequency?
        printf("sceAudiocodecInit = 0x%08x %x\n", ret, mp4->frequency);
        mpegsetLastError(MPEG_ERROR_AVCODEC_INIT);
        return;
     } 
}
void mp3_seek(PAudio mp3, int offset){
     if (!mp3) return;
     if (offset < 0) return;
     //I can't remember what the hell that suppose to mean
     if (mp3->size && (offset > mp3->size)) return;
     mp3_extra *extra = (mp3_extra*)mp3->extra;
     if (offset < mp3->offset)
        mp3_reset(mp3);
     int HMask;
     int start = sceIoLseek32(mp3->file, 0, PSP_SEEK_CUR);
     int time = mp3->offset;
     int samples_per_frame = mp3->sampleCount;
     int size = 0;
     while (offset > mp3->offset){
           if (sceIoRead(mp3->file, &HMask, 4) <= 0){
              goto afterwhile;
           }
           while ((HMask & 0xc00cfeff)  != extra->mask){
              if (seekNextFrame(mp3->file, extra->mask) < 0){
                 printf("seek seekNextFrame fail2\n");
                 goto afterwhile;
              }
              if (sceIoRead(mp3->file, &HMask, 4) <= 0)goto afterwhile;
           }
           size = getFrameSize(HMask);
           mp3->offset += samples_per_frame;
           sceIoLseek32(mp3->file, size-4, PSP_SEEK_CUR);
     }
     extra->fill = 0;
     return;
     afterwhile:
     printf("mp3 seek failed\n");
     sceIoLseek32(mp3->file, start, PSP_SEEK_SET);  
     mp3->offset = time;
}
#ifndef USE_ONLY_SCE_IO
void mp3_seekFILE(PAudio mp3, int offset){
     if (!mp3) return;
     if (offset < 0) return;
     if (mp3->size && (offset > mp3->size)) return;
     mp3_extra *extra = (mp3_extra*)mp3->extra;
     if (offset < mp3->offset)
        mp3_reset(mp3);
     int HMask;
     int start = ftell((FILE*)mp3->file);
     int time = mp3->offset;
     int samples_per_frame = mp3->sampleCount;
     int size = 0;
     while (offset > mp3->offset){
           if (fread(&HMask, 1, 4, (FILE*)mp3->file) <= 0){
              goto afterwhile;
           }
           while ((HMask & 0xc00cfeff)  != extra->mask){
              if (seekNextFrameFILE((FILE*)mp3->file, extra->mask) < 0){
                 printf("seek seekNextFrame fail2\n");
                 goto afterwhile;
              }
              if (fread(&HMask, 1, 4, (FILE*)mp3->file) <= 0)goto afterwhile;
           }
           size = getFrameSize(HMask);
           mp3->offset += samples_per_frame;
           fseek((FILE*)mp3->file, size-4, SEEK_CUR);
     }
     extra->fill = 0;
     return;
     afterwhile://crocodile
     printf("mp3 seek failed\n");
     fseek((FILE*)mp3->file, start, SEEK_SET);  
     mp3->offset = time;
}
#endif
void mp3_seekMEMORY(PAudio mp3, int offset){
     if (!mp3) return;
     if (offset < 0) return;
     if (mp3->size && (offset > mp3->size)) return;
     mp3_extra *extra = (mp3_extra*)mp3->extra;
     if (offset < mp3->offset)
        mp3_reset(mp3);
     int HMask;
     int start = extra->position;
     int time = mp3->offset;
     int samples_per_frame = mp3->sampleCount;
     int size = 0;
     int seek;
     while (offset > mp3->offset){
           if (extra->position+4 >= extra->dataSize){
              goto afterwhile;
           }
           else{
              char *ptr = extra->data+extra->start+extra->position;
              HMask = ptr[0]|ptr[1]<<8|ptr[2]<<16|ptr[3]<<24;
           }
           while ((HMask & 0xc00cfeff)  != extra->mask){
              if ((seek = seekNextFrameMEMORY((void*)mp3->file, extra->mask, extra->dataSize-extra->position)) < 0){
                 printf("seek seekNextFrame fail2\n");
                 goto afterwhile;
              }
              extra->position += seek;
              if (extra->position+4 >= extra->dataSize){
                 goto afterwhile;
              }
              else{
                   char *ptr = extra->data+extra->start+extra->position;
                   HMask = ptr[0]|ptr[1]<<8|ptr[2]<<16|ptr[3]<<24;
              }
           }
           size = getFrameSize(HMask);
           mp3->offset += samples_per_frame;
           extra->position += size;
     }
     extra->fill = 0;
     return;
     afterwhile:
     printf("mp3 seek failed\n");
     extra->position = start;  
     mp3->offset = time;
}

void aac_seek(PAudio aac, int offset){
     if (!aac) return;
     if (offset < 0) return;
     if (aac->size && (offset > aac->size)) return;
     mp3_extra *extra = (mp3_extra*)aac->extra;
     if (offset < aac->offset)
        mp3_reset(aac);
     unsigned char aac_header_buf[7];
     int start = sceIoLseek32(aac->file, 0, PSP_SEEK_CUR);
     int time = aac->offset;
     int samples_per_frame = aac->sampleCount;
     int size = 0;
     while (offset > aac->offset){
           if (sceIoRead(aac->file, aac_header_buf, 7) <= 0){
              goto afterwhile;
           }
           while ((aac_header_buf[0] != 0xff)||((aac_header_buf[1]&0xf6) != 0xf0)){
              if (findAacSync(aac->file) < 0){
                 printf("aac seek seekNextFrame fail2\n");
                 goto afterwhile;
              }
              if (sceIoRead(aac->file, aac_header_buf, 7) <= 0)goto afterwhile;
           }
           size = getAacFrameSize(aac_header_buf);
           aac->offset += samples_per_frame;
           sceIoLseek32(aac->file, size-7, PSP_SEEK_CUR);
     }
     extra->fill = 0;
     return;
     afterwhile:
     printf("aac seek failed\n");
     sceIoLseek32(aac->file, start, PSP_SEEK_SET);  
     aac->offset = time;
}

int mp4_EventHandler(PAudio mp4, eventData *event){
    if (!mp4) return 1;
    mp4_extra* extra = (mp4_extra*)mp4->extra;
    int ret = 0;
    switch (event->event){
       case EVENT_stopAudio :
          mp4->play = STOP;
          while (extra->working) sceKernelDelayThread(5);
		  mp4_reset(mp4);
		  return 0;
       break;
       case EVENT_freeAudio : 
          mp4->play = TERMINATED;
          sceKernelWaitThreadEnd(mp4->extra->thid, 0);
          sceKernelTerminateDeleteThread(mp4->extra->thid);
          ret = sceIoClose(mp4->file);
          if (ret < 0){
             mpegsetLastError(ret);
          }
          if (extra->mp4_codec_buffer){
             sceAudiocodecReleaseEDRAM(extra->mp4_codec_buffer);
             free(extra->mp4_codec_buffer);
          }
          int i;
          for (i = 0; i < extra->tracks; i++){
              free(extra->atoms->trak[i].offsets);
              if (extra->atoms->trak[i].sample_size == 0){
                 free(extra->atoms->trak[i].sample_sizes);
              }
              free(extra->atoms->trak[i].spc);
          }
          free(extra->atoms->trak);
          free(extra->atoms);
          free(extra->data);
          free(extra);
          return 0;
       break;
       case EVENT_getFrequencyAudio :
          return extra->frequency;
       break;
/*       case EVENT_getBitrateAudio :
          
       break;*/
       case EVENT_seekAudio :
          while (extra->working) sceKernelDelayThread(5);
          mp4_seek(mp4, event->data);
       break;
       default :
          return 0;
    }
    return 1;
}
int mp3_EventHandler(PAudio mp3, eventData *event){
    if (!mp3) return 1;
    mp3_extra* extra = (mp3_extra*)mp3->extra;
    //int ret = 0;
    switch (event->event){
       case EVENT_stopAudio :
          mp3->play = STOP;
          while (extra->working) sceKernelDelayThread(5);
		  mp3_reset(mp3);
		  return 0;
       break;
       case EVENT_freeAudio : 
          mp3->play = TERMINATED;
          sceKernelWaitThreadEnd(mp3->extra->thid, 0);
          sceKernelTerminateDeleteThread(mp3->extra->thid);
          if (mp3->flags&AUDIO_FLAG_SCEUID)
             sceIoClose(mp3->file);
#ifndef USE_ONLY_SCE_IO
          else if (mp3->flags&AUDIO_FLAG_FILE)
             fclose((FILE*)mp3->file);
#endif
          else if (mp3->flags&AUDIO_FLAG_MEMORY)
             free((void*)mp3->file);
          if (extra->mp3_codec_buffer){
             sceAudiocodecReleaseEDRAM(extra->mp3_codec_buffer);
             free(extra->mp3_codec_buffer);
          }
          if (extra->data)
             free(extra->data);
          return 0;
       break;
       case EVENT_getFrequencyAudio :
          return extra->frequency;
       break;
       case EVENT_getBitrateAudio :
          return getBitrateMpeg(mp3);
       break;
       case EVENT_seekAudio :
          while (extra->working) sceKernelDelayThread(5);
          if (mp3->type == SceAacCodec)
             aac_seek(mp3, event->data);
          else{
             if (mp3->flags&AUDIO_FLAG_SCEUID){
                mp3_seek(mp3, event->data);
             }
#ifndef USE_ONLY_SCE_IO
             else if (mp3->flags&AUDIO_FLAG_FILE)
                mp3_seekFILE(mp3, event->data);
#endif
             else if (mp3->flags&AUDIO_FLAG_MEMORY)
                mp3_seekMEMORY(mp3, event->data);
          }
       break;
       default :
          return 0;
    }
    return 1;
}
int decodeMp3Frame(PAudio mp3, s16 *data);
int mp3_thread(SceSize args, void *argp){
    PAudio mp3 = *(PAudio*)argp;
    if (!mp3) return -1;
    mp3_extra *extra = (mp3_extra*)mp3->extra;
    int x = 0;
    int ret;
    s16 *data[4] = {mp3->data, mp3->data+(mp3->sampleCount*2*2), mp3->data+(mp3->sampleCount*2*2)*2, mp3->data+(mp3->sampleCount*2*2)*3};
    s16 *read = mp3->data+(mp3->sampleCount*2*2)*4;
    start:
    do{
        if (mp3->play != PLAYING){
           if (mp3->play == TERMINATED) return 0;
           sceKernelDelayThread(50);
           continue;
        }
        extra->working = 1;
        ret = decodeMp3Frame(mp3, ((!isSRC())&&(mp3->frequency != getsysFrequency()))?read:data[x]);
//        ret = decodeMp3Frame(extra->mp3_codec_buffer, mp3->file, mp3->samplesCount, mp3_extra->frameScale, mp3_extra->frequency, extra->index, extra->mask, (!isSRC())&&(mp3->frequency != getsysFrequency())?read:data[x]);
        extra->working = 0;
        if (ret == -1)break;
        if (ret == 0) continue;
        int samples = ret;

        mp3->offset += samples;

        if (isSRC()){
           mp3->mixBuffer = data[x];
           ret = sceAudioSRCOutputBlocking(mp3->lVolume, data[x]);
           x = (x+1)&3;
        }
        else{
           if (mp3->frequency != getsysFrequency()){
              float i = 0.0f;
              float scale = mp3->scale;
              while (i < samples){
                    data[x][extra->fill*2] = read[(int)i*2];
                    data[x][extra->fill*2+1] = read[(int)i*2+1];
                       
                    extra->fill++;
                    i += scale;
                    
                    if (extra->fill == mp3->sampleCount){
                       mp3->mixBuffer = data[x];
                       ret = sceAudioOutputPannedBlocking(mp3->channel, mp3->lVolume, mp3->rVolume, data[x]);
                       if (ret < 0){
                          printf("mp3 sceAudioOutputPannedBlocking 0x%x\n", ret);
                       }
                       extra->fill = 0;
                       x = (x+1)&3;
                    }
              }
              if (mp3->extra->eof){
                 if (extra->fill){
                    memset(data[x]+extra->fill*2, 0, (mp3->sampleCount-extra->fill)*2*2);
                    mp3->mixBuffer = data[x];
                    ret = sceAudioOutputPannedBlocking(mp3->channel, mp3->lVolume, mp3->rVolume, data[x]);
                    extra->fill = 0;
                    x = (x+1)&3;
                 }
              }                   
           }
           else{
              mp3->mixBuffer = data[x];
              ret = sceAudioOutputPannedBlocking(mp3->channel, mp3->lVolume, mp3->rVolume, data[x]);
              x = (x+1)&3;
           }
        }
        if (ret < 0)
           printf("mp3 audio outputg 0x%x\n", ret);
    }
    while (1);//meh
    if (mp3->loop)
       mp3_reset(mp3);
    else{
       stopAudio(mp3);}
    goto start;
    return 1;
}

PAudio loadMp3(char *filename, int stream){
       return loadMp3Advance(filename, stream, 0, 0, 0);
}
PAudio loadMp3Advance(void *file, int stream, int size, u32 sztype, u32 flags){
       if (!mpegInit()){
          printf("mpegInit() fail\n");
          return NULL;
       }
       if (!file || (flags&AUDIO_FLAG_MEMORY && size == 0))
          return NULL;
       u32 HMask;
       int dataSize = 0;
       int start = 0;
       int totalSamples = 0;
       PAudio mp3 = NULL;
       mp3_extra *extra = NULL;
       unsigned long *mp3_codec_buffer = NULL;
       SceUID f = -1;
       if (!(flags&(AUDIO_FLAG_SCEUID| 
#ifndef USE_ONLY_SCE_IO 
       AUDIO_FLAG_FILE|
 #endif 
       AUDIO_FLAG_MEMORY))){
//          if (MPEG_FLAG_NO_ESTIMATE&flags)
//             totalSamples = getMp3Size(file, SZ_SAMPLES);
//          else
          totalSamples = estimateMp3Size(file, SZ_SAMPLES);
          if (totalSamples <= 0){
             mpegsetLastError(MPEG_ERROR_INVALID_FILE);
             return NULL;
          }
          f = sceIoOpen(file, PSP_O_RDONLY, 0777);
          if (f < 0){
             mpegsetLastError(MPEG_ERROR_OPEN_FILE);
             return NULL;   
          }
          if (stream == STREAM_FROM_MEMORY){
             if (size == 0){
                size = sceIoLseek32(f, 0, PSP_SEEK_END);
                sceIoLseek32(f, 0, PSP_SEEK_SET);
             }
             void *data = malloc(size);
             if (!data){
                mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
                sceIoClose(f);
                return NULL;
             }
             int read = sceIoRead(f, data, size);
             sceIoClose(f);
             if (read < size){
                mpegsetLastError(MPEG_ERROR_FILE_READ);
                free(data);
                return NULL;
             }
             start = 0;
             dataSize = read;
             file = data;
             flags |= AUDIO_FLAG_MEMORY|AUDIO_FLAG_FREE;
          }
          else{
//             file = (void*)f;
             flags |= AUDIO_FLAG_SCEUID;
          }
       }
       if (flags&AUDIO_FLAG_SCEUID){
          SceUID fd;
          if (!f) 
             fd = (SceUID)file; 
          else
             fd = f;
          start = seekNextFrame((SceUID)fd, 0);
          if (start < 0){
             mpegsetLastError(MPEG_ERROR_INVALID_FILE);
             return NULL;
          }
          start = sceIoLseek32((SceUID)fd, 0, PSP_SEEK_CUR);
          if (MPEG_FLAG_NO_ESTIMATE&flags)
             totalSamples = getMp3Sizefd((SceUID)fd, SZ_SAMPLES);
          if (sceIoRead((SceUID)fd, &HMask, 4) != 4){
             sceIoClose((SceUID)fd); 
             mpegsetLastError(MPEG_ERROR_FILE_READ);
             return NULL;
          }
          if (stream != STREAM_FROM_MEMORY){
             dataSize = sceIoLseek32((SceUID)fd, 0, PSP_SEEK_END) - start;
          }
          sceIoLseek32((SceUID)fd, start, PSP_SEEK_SET);
       }
#ifndef USE_ONLY_SCE_IO       
       else if (flags&AUDIO_FLAG_FILE){
          start = seekNextFrameFILE(file, 0);
          if (start < 0){
             mpegsetLastError(MPEG_ERROR_INVALID_FILE);
             return NULL;
          }
          start = ftell(file);
          if (MPEG_FLAG_NO_ESTIMATE&flags)
             totalSamples = getMp3SizeFILE(file, SZ_SAMPLES);
          if (fread(&HMask, 1, 4, (FILE*)file)<=0){
             fclose(file);
             mpegsetLastError(MPEG_ERROR_FILE_READ);
             return NULL;
          }
          if (stream != STREAM_FROM_MEMORY){
             fseek((FILE*)file, 0, SEEK_END);
             dataSize = ftell(file) - start;
          }
          fseek((FILE*)file, start, SEEK_SET);
       }
#endif
       else if (flags&AUDIO_FLAG_MEMORY){
          if (file == NULL){
             mpegsetLastError(MPEG_ERROR_INVALID_ARGUMENT);
             return NULL;
          }
          if (MPEG_FLAG_NO_ESTIMATE&flags)
             totalSamples = getMp3SizeMEMORY(file, SZ_SAMPLES, dataSize);
          start = seekNextFrameMEMORY(file, 0, dataSize);
          memcpy(&HMask, file+start, 4);
       }
       u32 samples_per_frame; 
       u32 channels; 
       int type = getType(HMask);
       int layer = getLayer(HMask);
       int frequency = mpeg_frequencies[type][getFrequency(HMask)];
       channels = getChannelCount(HMask);
       int EDRam = 0;
       
       int frameScale;// what was framescale used for again?
       if (layer == MPEG_LAYER_1)
          frameScale = 12*1000;
       else if (((layer == MPEG_LAYER_3) && (type == MPEG_VERSION_2)) || (type == MPEG_VERSION_2_5))
          frameScale = 72*1000;
       else
          frameScale = 144*1000;
       int index;
       if (type == MPEG_VERSION_1)
          index = 3 - layer;
       else{
          if (layer == MPEG_LAYER_1) index = 3;//no go
          else index = 4;
       }
       samples_per_frame = mpeg_samples[type][layer];
       if (samples_per_frame == 0 || type == MPEG_VERSION_RESERVED || frequency == 0){
          mpegsetLastError(MPEG_ERROR_LAYER_NOT_SUPPORTED);
          goto error;
       }
       if (layer != MPEG_LAYER_3){
#ifndef SANE
          if (layer == MPEG_LAYER_2 || layer == MPEG_LAYER_1){
             if (f >= 0){
                sceIoClose(f);
                flags = 0;
             }
             return loadMADMpeg(file, stream, size, sztype, flags);
          }
#else
          mpegsetLastError(MPEG_ERROR_LAYER_NOT_SUPPORTED);
          goto error;
#endif
       }

//       HMask = HMask & 0xc00cfeff;//my mask

       if (sztype == SZ_BYTES)
          size = size/(2*channels);
       else if (sztype == SZ_ALL)
          size = totalSamples;
       else if (sztype == SZ_SECONDS)
          size = size*frequency;

       size = (size/samples_per_frame)*samples_per_frame;

       mp3_codec_buffer = memalign(64, 66*sizeof(unsigned long));
       if (!mp3_codec_buffer){
          printf("mp3_codec_buffer allocation fail\n");
          mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
          goto error;
       }
       int ret;
       if ((ret = sceAudiocodecCheckNeedMem(mp3_codec_buffer, SceMp3Codec)) < 0){
          printf("sceAudiocodecCheckNeedMem fail 0x%x\n", ret);
          mpegsetLastError(MPEG_ERROR_AVCODEC_NEEDMEM);
          goto error;
       }
       if ((ret = sceAudiocodecGetEDRAM(mp3_codec_buffer, SceMp3Codec)) < 0){
          printf("sceAudiocodecGetEDRAM fail 0x%x\n", ret);
          mpegsetLastError(MPEG_ERROR_AVCODEC_EDRAM);
          goto error;
       }
       EDRam = 1;
       if ((ret = sceAudiocodecInit(mp3_codec_buffer, SceMp3Codec)) < 0){
          printf("sceAudiocodecInit fail 0x%x\n", ret);
          mpegsetLastError(MPEG_ERROR_AVCODEC_INIT);
          goto error;
       }
       void *data = NULL;
       
       mp3 = malloc(sizeof(Audio));
       if (!mp3){
          mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
          goto error;
       }
       mp3->mixBuffer = NULL;
       mp3->sampleCount = samples_per_frame;
       if (f)
          mp3->file = f;
       else
          mp3->file = (u32)file;
       mp3->fstate = STATE_F_OPEN;
       mp3->size = totalSamples;
       mp3->sampleCount = samples_per_frame;
       mp3->data = NULL;
       mp3->frequency = frequency;
       mp3->play = STOP;
       mp3->offset = 0;
       mp3->frac = 0;
       mp3->scale = (float)frequency/(float)getsysFrequency();
       mp3->lVolume = PSP_VOLUME_MAX;
       mp3->rVolume = PSP_VOLUME_MAX;
       mp3->loop = 0;
       mp3->stream = stream;
       mp3->channels = 2;
       mp3->bitRate = 16;
       mp3->ufunc = NULL;
       mp3->flags = flags;
       extra = malloc(sizeof(mp3_extra));
       if (!extra){
          mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
          goto error;
       }
       mp3->extra = (audioExtra*)extra;
       extra->type = type;
       extra->layer = layer;
       extra->frequency = frequency;
       extra->channels = channels;
       extra->index = index;
       extra->frameScale = frameScale;
       extra->mask = HMask&0xc00cfeff;
       extra->data = memalign(64, frameScale*320/frequency+1);
       if (!extra->data){
          printf("malloc failed extra data\n");
          mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
          goto error;
       }
       extra->mp3_codec_buffer = mp3_codec_buffer;
       extra->start = start;
       extra->position = 0;
       if (!stream){
          int samplesdecoded = 0;
          mp3->type = 1;
          if (size > totalSamples)
             size = totalSamples;
          data = malloc(size*2*2);
          if (!data){
             mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
             goto error;
          }
          u8 *tmp = data;
          while ((ret = decodeMp3Frame(mp3, (s16*)tmp)) >= 0){
                samplesdecoded += ret;
                if (samplesdecoded >= size) break;
                tmp += (ret*2*2);
          }
          if (samplesdecoded < size)
             mp3->size = samplesdecoded;
          else
             mp3->size = size;
          mp3->data = data;
          sceAudiocodecReleaseEDRAM(mp3_codec_buffer);
          free(extra->data);
          free(extra);
          if (flags&AUDIO_FLAG_SCEUID)
             sceIoClose((SceUID)file);
#ifndef USE_ONLY_SCE_IO             
          else if (flags&AUDIO_FLAG_FILE)
             fclose(file); 
#endif
          else if (flags&AUDIO_FLAG_MEMORY)
             if (flags&AUDIO_FLAG_FREE)
                free(file);
       }
       else{
          mp3->type = SceMp3Codec;
          mp3->data = memalign(64, samples_per_frame*2*2*5);
          if (!mp3->data){
             printf("Error alloc mix\n");
             mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
             goto error;
          }
          extra->working = 0;
          extra->fill = 0;
          extra->eof = 0;
          extra->bitrate = mpeg_bitrates[index][getBitrate(HMask)]*1000;
          extra->stream = (StreamEventHandler)mp3_EventHandler;
          extra->thid = sceKernelCreateThread("mp3_stream", mp3_thread, 0x10, 8*0x400, PSP_THREAD_ATTR_USER, NULL);
          if (extra->thid < 0){
             printf("Error thread creation\n");
             mpegsetLastError(MPEG_ERROR_THREAD_FAIL);
             goto error;
          }
          if (flags&AUDIO_FLAG_SCEUID && f){
             if (!strrchr(file, ':')){
                char path[256];
                getcwd(path, 256);
                mp3->filename = malloc(strlen(path)+strlen(file)+2);
                if (mp3->filename){
                   strcat(strcat(strcpy(mp3->filename, path), "/"), file);
                }
                else{
                   printf("Error memory\n");
                   audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
                }
             }
             else{ 
                mp3->filename = malloc(strlen(file)+1);
                if (mp3->filename){
                   strcpy(mp3->filename, file);
                }
                else{
                   printf("Error memory\n");
                   audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
                }
             }
          }
          sceKernelStartThread(mp3->extra->thid, 4, &mp3);
       }
       register_audio(mp3);
       return mp3; 
error:
      printf("Error mp3\n");
      if (flags&AUDIO_FLAG_FREE){
         if (flags&AUDIO_FLAG_SCEUID)
            sceIoClose((SceUID)file);
#ifndef USE_ONLY_SCE_IO
         else if (flags&AUDIO_FLAG_FILE)
           fclose(file);
#endif
         if (flags&AUDIO_FLAG_MEMORY)
           free(file);
      }
      if (mp3_codec_buffer){
         if (EDRam == 1)
            sceAudiocodecReleaseEDRAM(mp3_codec_buffer);
         free(mp3_codec_buffer);
         if (mp3){
            if (extra){
               if (extra->data)
                  free(extra->data);
               free(extra);
            }
            free(mp3);
         }
      }
      return NULL;
}

int decodeMp3Frame(PAudio mp3, s16 *data){
    mp3_extra *extra = (mp3_extra*)mp3->extra;
    int mp3_header;
    int ret;
    if (mp3->flags&AUDIO_FLAG_SCEUID){
       if (sceIoRead(mp3->file, &mp3_header, 4) != 4){
          printf("read fail 1\n");
          return -1;
       }
    }
#ifndef USE_ONLY_SCE_IO
    else if (mp3->flags&AUDIO_FLAG_FILE){
       if (fread(&mp3_header, 1, 4, (FILE*)mp3->file) != 4){
          printf("read fail 1\n");
          return -1;
       }
    }
#endif
    if (mp3->flags&AUDIO_FLAG_MEMORY){
       if (extra->position+4 < extra->dataSize){
          printf("read fail 1\n");
          return -1;
       }
       memcpy(&mp3_header, (void*)mp3->file+extra->start+extra->position, 4);
    }

    int padding = getPadding(mp3_header); 
    int bitrate = getBitrate(mp3_header); 
    int frame_size = extra->frameScale*mpeg_bitrates[extra->index][bitrate]/extra->frequency + padding; 

    int data_start = 0;
    if (mp3->flags&AUDIO_FLAG_SCEUID){
       data_start = sceIoLseek32(mp3->file, -4, PSP_SEEK_CUR); //seek back 
       if (sceIoRead(mp3->file, extra->data, frame_size) != frame_size){ 
          printf("read fail 2 \n");
          return -1;
       }
    }
#ifndef USE_ONLY_SCE_IO
    else if (mp3->flags&AUDIO_FLAG_FILE){
       fseek((FILE*)mp3->file, -4, SEEK_CUR); //seek back 
       data_start = ftell((FILE*)mp3->file);
       if (fread(extra->data, 1, frame_size, (FILE*)mp3->file) != frame_size){ 
          printf("read fail 2 \n");
          return -1;
       }
    }
#endif
    else if (mp3->flags&AUDIO_FLAG_MEMORY){
       if (extra->position+frame_size < extra->dataSize){
          printf("read fail 2\n");
          return -1;
       }
       memcpy(extra->data, (void*)mp3->file+extra->start+extra->position, frame_size);
    }
       
    extra->mp3_codec_buffer[6] = (unsigned long)extra->data; 
    extra->mp3_codec_buffer[8] = (unsigned long)data;//mp3_mix_buffer; 
       
    extra->mp3_codec_buffer[7] = extra->mp3_codec_buffer[10] = frame_size; 
    extra->mp3_codec_buffer[9] = mp3->sampleCount*2*2;
    ret = sceAudiocodecDecode(extra->mp3_codec_buffer, SceMp3Codec);
    if (ret < 0){ 
       printf("sceAudiocodecDecode fail 0x%x\n", ret); 
       if (mp3->flags&AUDIO_FLAG_SCEUID){
          sceIoLseek32(mp3->file, data_start+1, PSP_SEEK_SET);
          data_start = seekNextFrame(mp3->file, extra->mask);
       }
#ifndef USE_ONLY_SCE_IO
       else if (mp3->flags&AUDIO_FLAG_FILE){
          fseek((FILE*)mp3->file, data_start+1, SEEK_SET);
          data_start = seekNextFrameFILE((FILE*)mp3->file, extra->mask);
       }
#endif
       else if (mp3->flags&AUDIO_FLAG_MEMORY){
          data_start = seekNextFrameMEMORY((void*)mp3->file+extra->start+extra->position+1, extra->mask, extra->dataSize-extra->position);
          if (data_start >= 0) extra->position += data_start+1;
       }
       if (data_start & 0x80000000){//data_start<0
          return -1;
       }
       return 0;
    } 
    extra->position += frame_size;
    return mp3->sampleCount;
}

int decodeAacFrame(unsigned long *aac_codec_buffer, SceUID fd, u32 samples_per_frame, void *aac_data_buffer, s16 *aac_mix_buffer);
int aac_thread(SceSize args, void *argp){
    PAudio aac = *(PAudio*)argp;
    if (!aac) return -1;
    mp3_extra *extra = (mp3_extra*) aac->extra;
    int x = 0;
    int ret;
    s16 *data[4] = {aac->data, aac->data+(aac->sampleCount*2*2), aac->data+(aac->sampleCount*2*2)*2,  aac->data+(aac->sampleCount*2*2)*3};
    s16 *out =  aac->data+(aac->sampleCount*2*2)*4;
    start:
    do{
        if (aac->play != PLAYING){
           if (aac->play == TERMINATED) return 0;
           sceKernelDelayThread(50);
           continue;
        }
        extra->working = 1;
        ret = decodeAacFrame(extra->mp3_codec_buffer, aac->file, aac->sampleCount, extra->data, (aac->frequency==getsysFrequency())||isSRC()?data[x]:out);
        extra->working = 0;

        if (ret == -1) break;
        if (ret == 0) continue;
        int samples = ret;

        aac->offset += samples;

        if (isSRC()){
           aac->mixBuffer = data[x];
           ret = sceAudioSRCOutputBlocking(aac->lVolume, data[x]);
            x = (x+1)&3;
        }
        else if (aac->frequency != getsysFrequency()){
           float i = 0.0f;
           float scale = aac->scale;
           while (i < samples){
                 data[x][extra->fill*2] = out[((int)i)*2];
                 data[x][extra->fill*2+1] = out[((int)i)*2+1];
                 extra->fill++;
                 i += scale;
                 if (extra->fill == aac->sampleCount){
                    aac->mixBuffer = data[x];
                    ret = sceAudioOutputPannedBlocking(aac->channel, aac->lVolume, aac->rVolume, data[x]);
                    if (ret < 0) printf("aac sceAudioOutputPannedBlocking = %x\n", ret);
                    extra->fill = 0;
                    extra->index = (extra->index+1)&3;//?
                    x = (x+1)&3;
                 }
           }
        }
        else{
           aac->mixBuffer = data[x];
           ret = sceAudioOutputPannedBlocking(aac->channel, aac->lVolume, aac->rVolume, data[x]);
           x = (x+1)&3;
        }
        if (ret < 0){
           printf("audio output 0x%x\n", ret);
        }
    }
    while (1);//meh
    if (aac->loop)
       mp3_reset(aac);
    else
       stopAudio(aac);
    goto start;
    return 1;
}
int mp4_thread(SceSize args, void *argp){
    PAudio aac = *(PAudio*)argp;
    if (!aac) return -1;
    mp4_extra *extra = (mp4_extra*) aac->extra;
    int x = 0;
    int ret;
    int sample_size;
    short *data[4] = {aac->data, aac->data+(aac->sampleCount*2*2), aac->data+(aac->sampleCount*2*2)*2,  aac->data+(aac->sampleCount*2*2)*3};
    short *out =  aac->data+(aac->sampleCount*2*2)*4;
    short *mix_buffer;
    unsigned long *mp4_codec_buffer = extra->mp4_codec_buffer;
    memset(aac->data, 0, aac->sampleCount*2*2*5);
    start:
    do{
        if (aac->play != PLAYING){
           if (aac->play == TERMINATED) return 0;
           sceKernelDelayThread(50);
           continue;
        }
        
        extra->working = 1;
        if (!extra->atoms->trak[extra->track].sample_size){
           sample_size = extra->atoms->trak[extra->track].sample_sizes[extra->index];
        }
        else{
           sample_size = extra->atoms->trak[extra->track].sample_size;
        }
        if (extra->atoms->trak[extra->track].spc[extra->spc].chunk == extra->chunk){
           extra->samples = extra->atoms->trak[extra->track].spc[extra->spc].count;
           sceIoLseek32(aac->file, extra->atoms->trak[extra->track].offsets[extra->chunk], PSP_SEEK_SET);
           extra->spc++;
        }
        
        sceIoRead(aac->file, extra->data, sample_size);
        
        if ((aac->frequency == getsysFrequency()) || isSRC()){
           mix_buffer = data[x];
        }
        else{
           mix_buffer = out;
        }
        
        mp4_codec_buffer[6] = (unsigned long)extra->data; 
        mp4_codec_buffer[8] = (unsigned long)mix_buffer; 
       
        mp4_codec_buffer[7] = (unsigned long)sample_size; 
        mp4_codec_buffer[9] = (unsigned long)aac->sampleCount*2*2;
    
        ret = sceAudiocodecDecode(mp4_codec_buffer, SceAacCodec);//0x807f0001 if edram released?
        if (ret < 0) { 
           printf("sceAudiocodecDecode = 0x%08x %d %d 0x%x %d %d %d 0x%x\n", ret, extra->index, extra->index_count,sample_size, extra->spc, extra->chunk, extra->samples, sceIoLseek32(aac->file, 0, PSP_SEEK_CUR));
           memset(mix_buffer, 0, aac->sampleCount*2*2);
        }
        extra->index++;
        extra->samples--;
        if (extra->samples == 0){
           extra->chunk++;
           if (extra->atoms->trak[extra->track].spc[extra->spc].chunk != extra->chunk)
              sceIoLseek32(aac->file, extra->atoms->trak[extra->track].offsets[extra->chunk], PSP_SEEK_SET);
           extra->samples = extra->atoms->trak[extra->track].spc[extra->spc-1].count;
        }
        extra->working = 0;
        if (ret < 0) { 
           continue;
        }
        
        int samples = aac->sampleCount;

        aac->offset += samples;

        if (isSRC()){
           aac->mixBuffer = data[x];
           ret = sceAudioSRCOutputBlocking(aac->lVolume, data[x]);
           x = (x+1)&3;
        }
        else if (aac->frequency != getsysFrequency()){
           float i = 0.0f;
           while (i < samples){
                 data[x][extra->fill*2] = out[((int)i)*2];
                 data[x][extra->fill*2+1] = out[((int)i)*2+1];
                 extra->fill++;
                 i += aac->scale;
                 if (extra->fill == aac->sampleCount){
                    while (sceAudioGetChannelRestLength(aac->channel))sceKernelDelayThread(10);
                    aac->mixBuffer = data[x];
                    ret = sceAudioOutputPanned/*Blocking*/(aac->channel, aac->lVolume, aac->rVolume, data[x]);
                    if (ret < 0) printf("mp4 sceAudioOutputPanned = %x\n", ret);
                    extra->fill = 0;
                    x = (x+1)&3;
                 }
           }
        }
        else{
           aac->mixBuffer = data[x];
           ret = sceAudioOutputPannedBlocking(aac->channel, aac->lVolume, aac->rVolume, data[x]);
           x = (x+1)&3;
        }
        if (ret < 0){
           printf("%saudio output 0x%x\n#channel %x\n", isSRC()?"SRC:":"", ret, aac->channel);
        }

        if (extra->index == extra->index_count)
           break;
    }
    while (1);//meh
    if (aac->loop)
       mp4_reset(aac);
    else
       stopAudio(aac);
    goto start;
    return 1;
}

int skipID3v2Tag(SceUID fd);
PAudio loadAac(const char *filename, int stream){
       u32 aac_samples_per_frame; 
       u32 aac_data_start; 
       u8 aac_getEDRAM = 0; 
       u32 aac_channels; 
       u32 aac_samplerate;
       int totalSamples;
       unsigned long *aac_codec_buffer = NULL;
       SceUID aac_handle = -1;
       PAudio aac = NULL;
       mp3_extra* extra = NULL;

       if (!mpegInit()){
          printf("mpegInit() fail\n");
          return NULL;
       }
       totalSamples = estimateAacSize(filename, SZ_SAMPLES);
       if (totalSamples < 0){
          mpegsetLastError(MPEG_ERROR_INVALID_FILE);
          printf("Adts sync not found\n");
          goto error;
       }

       aac_handle = sceIoOpen(filename, PSP_O_RDONLY, 0777); 
       if (aac_handle < 0){
          mpegsetLastError(MPEG_ERROR_OPEN_FILE);
          printf("aac_handle = 0x%08x\n", aac_handle);
          goto error;
       }
       skipID3v2Tag(aac_handle);
       if (findAacSync(aac_handle) < 0){
          mpegsetLastError(MPEG_ERROR_INVALID_FILE);
          printf("Adts sync not found\n");
          goto error;
       }
       unsigned char aac_header_buf[7]; 
       if (sceIoRead(aac_handle, aac_header_buf, 7) != 7) { 
          mpegsetLastError(MPEG_ERROR_FILE_READ);
          printf("Read < 7\n");
          goto error;
       } 

       int index = (aac_header_buf[2]&0x3c)>>2;
       if (index >= 13){
          mpegsetLastError(MPEG_ERROR_LAYER_NOT_SUPPORTED);
          goto error;//although 15 is valid ... whatever
       }

       aac_channels = 2; //it will be decoded as stereo, so... 
       aac_samplerate = sample_rates[index]; 
       aac_samples_per_frame = 1024; 
    
       aac_data_start = sceIoLseek32(aac_handle, -7, PSP_SEEK_CUR);//for forward compatability
    
       aac_codec_buffer = memalign(64, sizeof(unsigned long)*65); 
       if (!aac_codec_buffer){
          mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
          goto error;
       }
       memset(aac_codec_buffer, 0, sizeof(unsigned long)*65); 
    
       int ret; 

       if ((ret = sceAudiocodecCheckNeedMem(aac_codec_buffer, SceAacCodec)) < 0){
          printf("sceAudiocodecCheckNeedMem = 0x%08x\n", ret);  
          mpegsetLastError(MPEG_ERROR_AVCODEC_NEEDMEM);
          goto error;
       }
       if ((ret = sceAudiocodecGetEDRAM(aac_codec_buffer, SceAacCodec)) < 0){
          printf("sceAudiocodecGetEDRAM = 0x%08x\n", ret);
          mpegsetLastError(MPEG_ERROR_AVCODEC_EDRAM);
          goto error; 
       }
       aac_getEDRAM = 1; 
    
       aac_codec_buffer[10] = aac_samplerate; 
       if ((ret = sceAudiocodecInit(aac_codec_buffer, SceAacCodec)) < 0){ 
          printf("sceAudiocodecInit = 0x%08x\n", ret);
          mpegsetLastError(MPEG_ERROR_AVCODEC_INIT);
          goto error; 
       } 
      
       aac = malloc(sizeof(Audio));
       if (!aac){
          mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
          goto error;
       }
       aac->type = SceAacCodec;
       aac->channels = aac_channels;
       aac->frequency = aac_samplerate;
       aac->bitRate = 16;
       aac->data = NULL;
       aac->sampleCount = aac_samples_per_frame;
       aac->mixBuffer = NULL;
       aac->size = totalSamples;
       aac->scale = (float)aac_samplerate/(float)getsysFrequency();
       aac->offset = 0;
       aac->frac = 0;
       aac->stream = stream;
       aac->channel = -1;
       aac->lVolume = PSP_VOLUME_MAX;
       aac->rVolume = PSP_VOLUME_MAX;
       aac->play = STOP;
       aac->loop = 0;
       aac->file = aac_handle;
       aac->fstate = STATE_F_OPEN;
       aac->ufunc = NULL;       
       aac->flags = AUDIO_FLAG_SCEUID;
       extra = malloc(sizeof(mp3_extra));
       if (!extra){
          mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
          goto error;
       }
       aac->extra = (audioExtra*)extra;
       extra->size = sizeof(mp3_extra);
       extra->mp3_codec_buffer = aac_codec_buffer;
       extra->frequency = aac_samplerate;
       extra->stream = (StreamEventHandler) mp3_EventHandler;
       extra->fill = 0;       
       extra->data = memalign(64, (6144/8)*aac_channels);
       if (!extra->data){
          mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
          goto error;
       }
       if (!stream){
          int samplesdecoded = 0;
          aac->type = 1;
          int size = getAacSizefd(aac_handle, SZ_SAMPLES);
          void *data = memalign(64, size*2*2);
          if (!data){
             mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
             goto error;
          }
          u8 *tmp = data;
          memset(data, 0, size*2*2);
          while ((ret = decodeAacFrame(aac_codec_buffer, aac_handle, aac_samples_per_frame, extra->data, (short*)tmp)) >= 0){
                samplesdecoded += ret;
                if (samplesdecoded >= size) break;
                tmp += (ret*2*2);
          }
          if (samplesdecoded < size)
             aac->size = samplesdecoded;
          else
             aac->size = size;
          aac->data = data;
          sceAudiocodecReleaseEDRAM(aac_codec_buffer);
          free(aac_codec_buffer);
          free(extra->data);
          free(extra);
          sceIoClose(aac->file);
          register_audio(aac);
          return aac;
       }
       extra->start = aac_data_start;
       extra->bitrate = aac_samplerate*aac_channels*16;//how to find out the aac bitrate?
       aac->data = memalign(64, aac->sampleCount*2*2*5);
       extra->thid = sceKernelCreateThread("aac_stream", aac_thread, 0x10, 8*0x400, PSP_THREAD_ATTR_USER, NULL);
       if (extra->thid < 0){
          printf("Error thread creation \"aac_stream\"\n");
          free(extra);
          mpegsetLastError(MPEG_ERROR_THREAD_FAIL);
          goto error;
       }
       extra->eof = 0;
       if (!strrchr(filename, ':')){
          char path[256];
          getcwd(path, 256);
          aac->filename = malloc(strlen(path)+strlen(filename)+2);
          if (aac->filename){
             strcat(strcat(strcpy(aac->filename, path), "/"), filename);
          }
          else{
             printf("Error memory\n");
             audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
          }
       }
       else{ 
          aac->filename = malloc(strlen(filename)+1);
          if (aac->filename){
             strcpy(aac->filename, filename);
          }
          else{
             printf("Error memory\n");
             audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
          }
       }
       if (stream){
          sceKernelStartThread(extra->thid, 4, &aac);
       }
       register_audio(aac);
       return aac; 
       error:
            printf("aac fail\n");
            if (aac_handle >= 0)
               sceIoClose(aac_handle); 
            if (aac_codec_buffer){
               if (aac_getEDRAM == 1)
                  sceAudiocodecReleaseEDRAM(aac_codec_buffer);
               if (aac){ 
                  if (extra){ 
                     if (extra->data)
                        free(extra->data);
                     free(extra);   
                  }
                  free(aac);
               }
            }
            return NULL;
}
PAudio loadMp4(const char *filename){ 
       int aac_getEDRAM = 0; 
       int tracks_alloc;
       unsigned long *aac_codec_buffer = NULL;
       PAudio mp4 = NULL;
       atoms_used *atoms = NULL;
       SceUID aac_handle = 0;
       int var;//32 bit read var
       int i;//generic loop var
       char var8;
       
       int tracks = 0, total_tracks = 0;

       if (!mpegInit()){
          printf("mpegInit() fail\n");
          return NULL;
       }

       aac_handle = sceIoOpen(filename, PSP_O_RDONLY, 0777); 
       if (aac_handle < 0){
          mpegsetLastError(MPEG_ERROR_OPEN_FILE);
          printf("aac_handle = 0x%08x\n", aac_handle);
          return NULL;
       }
       atoms = malloc(sizeof(atoms_used));
       if (!atoms){
          printf("malloc on atoms fails\n");
          mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
          goto error;
       }
       tracks_alloc = 4;
       atoms->trak = calloc(tracks_alloc, sizeof(mp4_track));
       if (!atoms->trak){
          printf("malloc on tracks fails \n");
          mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
          goto error;
       }
       mp4_atom cnk;
       cnk.data = 0;
       int read = sceIoRead(aac_handle, &cnk, 8);
       if (read < 8){
          printf("read atom fails 0\n");
          mpegsetLastError(MPEG_ERROR_FILE_READ);
          goto error;
       }
       cnk.size = changeEdian(cnk.size);
       if (cnk.name != 'pytf'){
          printf("invalid file\n");
          mpegsetLastError(MPEG_ERROR_INVALID_FILE);
          goto error;
       }
       atoms->ftyp = 0;
       do{
           if (sceIoLseek32(aac_handle, cnk.size-8, PSP_SEEK_CUR) < 0 || cnk.size == 0){
              printf("seek atom fails 1\n");
              mpegsetLastError(MPEG_ERROR_FILE_READ);
              goto error;
           }
           read = sceIoRead(aac_handle, &cnk, 8);
           if (read < 8){
              printf("read atom fails 1\n");
              goto error;
           }
           cnk.size = changeEdian(cnk.size);
       }
       while (cnk.name != 'voom');
       atoms->moov = sceIoLseek32(aac_handle, 0, PSP_SEEK_CUR)-8;

       next_track:
       read = sceIoRead(aac_handle, &cnk, 8);
       if (read < 8){
          if (tracks == 0){
             printf("read atom fails 2 %x\n", read);
             mpegsetLastError(MPEG_ERROR_FILE_READ);
             goto error;
          }
          goto end_tracks;
       }
       cnk.size = changeEdian(cnk.size);
       while (cnk.name != 'kart'){
           //printf("'search %s'\n", (char*)&cnk.name);
           if (sceIoLseek32(aac_handle, cnk.size-8, PSP_SEEK_CUR) < 0 || cnk.size == 0){
              printf("seek atom fails 2\n");
              mpegsetLastError(MPEG_ERROR_FILE_READ);
              goto error;
           }
           read = sceIoRead(aac_handle, &cnk, 8);
           if (read < 8){
              if (tracks == 0){
                 printf("read atom fails 3 %p %d %d %x\n", &cnk, total_tracks, tracks, read);
                 mpegsetLastError(MPEG_ERROR_FILE_READ);
                 goto error;
              }
              printf("no more track 0x%x %d\n", sceIoLseek32(aac_handle, 0, PSP_SEEK_CUR), cnk.size-8);
              goto end_tracks;
           }
           cnk.size = changeEdian(cnk.size);
       }
       if (tracks >= tracks_alloc){
          void *tmp = realloc(atoms->trak, (tracks+1)*sizeof(mp4_track));
          if (!tmp) goto end_tracks;
          atoms->trak = tmp;
          tracks_alloc = tracks+1;
       }
       atoms->trak[tracks].offset = sceIoLseek32(aac_handle, 0, PSP_SEEK_CUR)-8;
       total_tracks++;
       
       read = sceIoRead(aac_handle, &cnk, 8);
       if (read < 8){
          printf("read atom fails 4\n");
          mpegsetLastError(MPEG_ERROR_FILE_READ);
          goto error;
       }
       cnk.size = changeEdian(cnk.size);
       while (cnk.name != 'aidm'){
           if (sceIoLseek32(aac_handle, cnk.size-8, PSP_SEEK_CUR) < 0 || cnk.size == 0){
              printf("seek atom fails 3\n");
              mpegsetLastError(MPEG_ERROR_FILE_READ);
              goto error;
           }
           read = sceIoRead(aac_handle, &cnk, 8);
           if (read < 8){
              printf("read atom fails 5\n");
              mpegsetLastError(MPEG_ERROR_FILE_READ);
              goto error;
           }
           cnk.size = changeEdian(cnk.size);
       }
       atoms->trak[tracks].mdia = sceIoLseek32(aac_handle, 0, PSP_SEEK_CUR)-8;
       
       read = sceIoRead(aac_handle, &cnk, 8);
       if (read < 8){
          printf("read atom fails 6\n");
          mpegsetLastError(MPEG_ERROR_FILE_READ);
          goto error;
       }
       cnk.size = changeEdian(cnk.size);
       while (cnk.name != 'dhdm'){
           if (sceIoLseek32(aac_handle, cnk.size-8, PSP_SEEK_CUR) < 0 || cnk.size == 0){
              printf("seek atom fails 4\n");
              mpegsetLastError(MPEG_ERROR_FILE_READ);
              goto error;
           }
           read = sceIoRead(aac_handle, &cnk, 8);
           if (read < 8){
              printf("read atom fails 7\n");
              mpegsetLastError(MPEG_ERROR_FILE_READ);
              goto error;
           }
           cnk.size = changeEdian(cnk.size);
       }
       atoms->trak[tracks].mdhd = sceIoLseek32(aac_handle, 0, PSP_SEEK_CUR)-8;
       
       while (cnk.name != 'fnim'){
           if (sceIoLseek32(aac_handle, cnk.size-8, PSP_SEEK_CUR) < 0 || cnk.size == 0){
              printf("seek atom fails 5\n");
              mpegsetLastError(MPEG_ERROR_FILE_READ);
              goto error;
           }
           read = sceIoRead(aac_handle, &cnk, 8);
           if (read < 8){
              printf("read atom fails 7\n");
              mpegsetLastError(MPEG_ERROR_FILE_READ);
              goto error;
           }
           cnk.size = changeEdian(cnk.size);
       }
       atoms->trak[tracks].minf = sceIoLseek32(aac_handle, 0, PSP_SEEK_CUR)-8;
       
       read = sceIoRead(aac_handle, &cnk, 8);
       if (read < 8){
          printf("read atom fails 8\n");
          free(atoms);
          goto error;
       }
       cnk.size = changeEdian(cnk.size);
       while (cnk.name != 'lbts'){
           if (sceIoLseek32(aac_handle, cnk.size-8, PSP_SEEK_CUR) < 0 || cnk.size == 0){
              printf("seek atom fails 6\n");
              mpegsetLastError(MPEG_ERROR_FILE_READ);
              goto error;
           }
           read = sceIoRead(aac_handle, &cnk, 8);
           if (read < 8){
              printf("read atom fails 9\n");
              mpegsetLastError(MPEG_ERROR_FILE_READ);
              goto error;
           }
           cnk.size = changeEdian(cnk.size);
       }
       atoms->trak[tracks].stbl = sceIoLseek32(aac_handle, 0, PSP_SEEK_CUR)-8;
       
       read = sceIoRead(aac_handle, &cnk, 8);
       if (read < 8){
          printf("read atom fails 10\n");
          mpegsetLastError(MPEG_ERROR_FILE_READ);
          goto error;
       }
       cnk.size = changeEdian(cnk.size);
       int st_count = 0;
       while (1){
           switch (cnk.name){
              case 'dsts':
                 atoms->trak[tracks].stsd = sceIoLseek32(aac_handle, 0, PSP_SEEK_CUR)-8;
                 st_count++;
              break;
              case 'zsts':
                 atoms->trak[tracks].stsz = sceIoLseek32(aac_handle, 0, PSP_SEEK_CUR)-8;
                 st_count++;
              break;
              case 'csts':
                 atoms->trak[tracks].stsc = sceIoLseek32(aac_handle, 0, PSP_SEEK_CUR)-8;
                 st_count++;
              break;
              case 'octs':
                 atoms->trak[tracks].stco = sceIoLseek32(aac_handle, 0, PSP_SEEK_CUR)-8;
                 st_count++;
              break;
           }
           if (st_count == 4) break;
           if (sceIoLseek32(aac_handle, cnk.size-8, PSP_SEEK_CUR || cnk.size == 0) < 0){
              printf("seek atom fails 7\n");
              mpegsetLastError(MPEG_ERROR_FILE_READ);
              goto error;
           }
           read = sceIoRead(aac_handle, &cnk, 8);
           if (read < 8){
              printf("read atom fails 11\n");
              mpegsetLastError(MPEG_ERROR_FILE_READ);
              goto error;
           }
           cnk.size = changeEdian(cnk.size);
       }

       sceIoLseek32(aac_handle, atoms->trak[tracks].stsd+12, PSP_SEEK_SET);
       sceIoRead(aac_handle, &var, 4);
       if (var != 0x01000000){
          printf("more than 1 descriptors\n");//what is a descriptor? I don't know
       }
       sceIoRead(aac_handle, &var, 4);//seek instead of read?
       //not interested in this value;
       sceIoRead(aac_handle, &var, 4);
       if (var != 'a4pm'){
          cnk.name = var;
          printf("atom %s\n", (char*)&cnk.name);
          sceIoLseek32(aac_handle, atoms->trak[tracks].offset, PSP_SEEK_SET);
          read = sceIoRead(aac_handle, &cnk, 4);
          if (read < 4){
              printf("read atom fails 14\n");
              mpegsetLastError(MPEG_ERROR_FILE_READ);
              goto error;
          }
          sceIoLseek32(aac_handle, changeEdian(cnk.size)-4, PSP_SEEK_CUR);
          goto next_track;
       //this track is no good
       //see if there is a next one?
       }
       sceIoLseek32(aac_handle, 16, PSP_SEEK_CUR);//skip all that crap
       short var16;
       sceIoRead(aac_handle, &var16, 2);
       atoms->trak[tracks].channels = changeEdian16(var16);
       if (atoms->trak[tracks].channels != 1 && atoms->trak[tracks].channels != 2){
          printf("channels = %d\n", atoms->trak[tracks].channels);
          sceIoLseek32(aac_handle, atoms->trak[tracks].offset, PSP_SEEK_SET);
          read = sceIoRead(aac_handle, &cnk, 4);
          if (read < 4){
              printf("read atom fails 18\n");
              mpegsetLastError(MPEG_ERROR_FILE_READ);
              goto error;
          }
          sceIoLseek32(aac_handle, changeEdian(cnk.size)-4, PSP_SEEK_CUR);
          goto next_track;
       }
       sceIoRead(aac_handle, &var16, 2);
     
       sceIoLseek32(aac_handle, 4, PSP_SEEK_CUR);//skip all that crap
       sceIoRead(aac_handle, &var16, 2);//fixed point wtf?
       sceIoLseek32(aac_handle, 2, PSP_SEEK_CUR);//skip the other 2
       atoms->trak[tracks].frequency = changeEdian16(var16);
       switch (atoms->trak[tracks].frequency){//ratch, how to convert long unsigned fixed point to int?
          case 0x7700:
             atoms->trak[tracks].frequency = 96000;
             break;
          case 0x5888:
             atoms->trak[tracks].frequency = 88200;
             break;
          case 0xFA00:
             atoms->trak[tracks].frequency = 64000;
             break;
       }
       
       read = sceIoRead(aac_handle, &cnk, 8);
       if (read < 8){
          printf("read atom fails 16\n");
          mpegsetLastError(MPEG_ERROR_FILE_READ);
          goto error;
       }
       cnk.size = changeEdian(cnk.size);
       while (cnk.name != 'sdse'){
           if (sceIoLseek32(aac_handle, cnk.size-8, PSP_SEEK_CUR) < 0 || cnk.size == 0){
              printf("seek atom fails 8\n");
              mpegsetLastError(MPEG_ERROR_FILE_READ);
              goto error;
           }
           read = sceIoRead(aac_handle, &cnk, 8);
           if (read < 8){
              printf("read atom fails 17\n");
              mpegsetLastError(MPEG_ERROR_FILE_READ);
              goto error;
           }
           cnk.size = changeEdian(cnk.size);
       }
       sceIoLseek32(aac_handle, 5, PSP_SEEK_CUR);
       unsigned char objID;
       sceIoRead(aac_handle, &objID, 1);
       if (objID != 0x80 && objID != 0xEE){//extended start tags left out
          sceIoLseek32(aac_handle, 4, PSP_SEEK_CUR);
       }
       else{
          sceIoLseek32(aac_handle, 7, PSP_SEEK_CUR);
       }
       sceIoRead(aac_handle, &objID, 1);
       if (objID != 0x80 && objID != 0xEE){//extended start tags left out
          //sceIoLseek32(aac_handle, 0, PSP_SEEK_CUR);
       }
       else{
          sceIoLseek32(aac_handle, 3, PSP_SEEK_CUR);
       }
       sceIoRead(aac_handle, &objID, 1);
       if (objID != 0x40 && objID != 0x67){
          printf("objID = 0x%02x\n", objID);
          sceIoLseek32(aac_handle, atoms->trak[tracks].offset, PSP_SEEK_SET);
          read = sceIoRead(aac_handle, &cnk, 4);
          if (read < 4){
              printf("read atom fails 18\n");
              mpegsetLastError(MPEG_ERROR_FILE_READ);
              goto error;
          }
          sceIoLseek32(aac_handle, changeEdian(cnk.size)-4, PSP_SEEK_CUR);
          goto next_track;/*not an aac stream(maybe), look for a next track*/          
       }
       
       sceIoLseek32(aac_handle, atoms->trak[tracks].stsz+12, PSP_SEEK_SET);
       sceIoRead(aac_handle, &var, 4);
       atoms->trak[tracks].sample_size = changeEdian(var);
       if (atoms->trak[tracks].sample_size == 0){
          sceIoRead(aac_handle, &var, 4);
          var = changeEdian(var);
          atoms->trak[tracks].sample_sizes = malloc(var*4);
          if (!atoms->trak[tracks].sample_sizes){
             printf("malloc failed on atoms->trak[tracks].sample_sizes %x\n", var);
             mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
             goto error;
          }
          read = sceIoRead(aac_handle, atoms->trak[tracks].sample_sizes, var*4);
          if (read != var*4){
             printf("read failed on atoms->trak[tracks].sample_sizes %x %x\n", var, read);
             mpegsetLastError(MPEG_ERROR_FILE_READ);
             goto error;
          }
          for (i = 0; i < var; i++){
              atoms->trak[tracks].sample_sizes[i] = changeEdian(atoms->trak[tracks].sample_sizes[i]);
          }
       }

       sceIoLseek32(aac_handle, atoms->trak[tracks].stco+12, PSP_SEEK_SET);
       sceIoRead(aac_handle, &var, 4);
       atoms->trak[tracks].chunks = changeEdian(var);
       atoms->trak[tracks].offsets = malloc(atoms->trak[tracks].chunks*4);
       if (!atoms->trak[tracks].offsets){
          printf("malloc failed on atoms->trak[tracks].offsets\n");
          mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
          goto error;
       }
       read = sceIoRead(aac_handle, atoms->trak[tracks].offsets, atoms->trak[tracks].chunks*4);
       if (read != atoms->trak[tracks].chunks*4){
          printf("read failed on atoms->trak[tracks].offsets\n");
          mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
          goto error;
       }
       for (i = 0; i < atoms->trak[tracks].chunks; i++){
           atoms->trak[tracks].offsets[i] = changeEdian(atoms->trak[tracks].offsets[i]);
       }
       
       sceIoLseek32(aac_handle, atoms->trak[tracks].stsc+12, PSP_SEEK_SET);
       sceIoRead(aac_handle, &var, 4);
       var = changeEdian(var);
       atoms->trak[tracks].stsc_count = var;
       atoms->trak[tracks].spc = malloc(var*8);
       for (i = 0; i < var; i++){
           sceIoRead(aac_handle, &atoms->trak[tracks].spc[i], 8);
           atoms->trak[tracks].spc[i].chunk = changeEdian(atoms->trak[tracks].spc[i].chunk)-1;
           atoms->trak[tracks].spc[i].count = changeEdian(atoms->trak[tracks].spc[i].count);
           sceIoLseek32(aac_handle, 4, PSP_SEEK_CUR);
       }
         
       sceIoLseek32(aac_handle, atoms->trak[tracks].mdhd+8, PSP_SEEK_SET);
       sceIoRead(aac_handle, &var8, 1);
       if (var8){
          sceIoLseek32(aac_handle, 19, PSP_SEEK_CUR);
       }
       else{
          sceIoLseek32(aac_handle, 11, PSP_SEEK_CUR);
       }
       sceIoRead(aac_handle, &var, 4);
       atoms->trak[tracks].time_scale = changeEdian(var);
       if (var8){
          sceIoRead(aac_handle, &atoms->trak[tracks].duration, 8);
          atoms->trak[tracks].duration = changeEdian64(atoms->trak[tracks].duration);
       }
       else{
          sceIoRead(aac_handle, &var, 4);
          atoms->trak[tracks].duration = changeEdian(var);
       }

       sceIoLseek32(aac_handle, atoms->trak[tracks].offset, PSP_SEEK_SET);
       read = sceIoRead(aac_handle, &cnk, 4);
       if (read < 4){
          printf("read atom fails 19\n");
          mpegsetLastError(MPEG_ERROR_FILE_READ);
          tracks++;
          goto error;
       }
       sceIoLseek32(aac_handle, changeEdian(cnk.size)-4, PSP_SEEK_CUR);
       tracks++;
       goto next_track;
       
       end_tracks:
       //printf("tracks %d\n", tracks);
    
       aac_codec_buffer = memalign(64, sizeof(unsigned long)*65); 
       if (!aac_codec_buffer){
          mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
          goto error;
       }
       memset(aac_codec_buffer, 0, sizeof(unsigned long)*65); 
    
       int ret; 

       if ( (ret = sceAudiocodecCheckNeedMem(aac_codec_buffer, SceAacCodec)) < 0 ) {
          printf("sceAudiocodecCheckNeedMem = 0x%08x\n", ret);  
          mpegsetLastError(MPEG_ERROR_AVCODEC_NEEDMEM);
          goto error;
       }
       if ( (ret = sceAudiocodecGetEDRAM(aac_codec_buffer, SceAacCodec)) < 0 ) {
          printf("sceAudiocodecGetEDRAM = 0x%08x\n", ret);
          mpegsetLastError(MPEG_ERROR_AVCODEC_EDRAM);
          goto error; 
       }
       aac_getEDRAM = 1; 
    
       aac_codec_buffer[10] = atoms->trak[0].frequency;
       if ( (ret = sceAudiocodecInit(aac_codec_buffer, SceAacCodec)) < 0 ) { //0x807f00ff invalid frequency 0x2ee0, 0x1cb6?
          printf("sceAudiocodecInit = 0x%08x %x\n", ret, atoms->trak[0].frequency);
          mpegsetLastError(MPEG_ERROR_AVCODEC_INIT);
          goto error; 
       } 
      
       mp4 = calloc(1, sizeof(Audio));
       if (!mp4){
          printf("calloc failed on Audio\n");
          mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
          goto error;
       }
       mp4->type = MP4A;//SceAacCodec;
       mp4->channels = 2;
       mp4->frequency = atoms->trak[0].frequency;
       mp4->bitRate = 16;
       mp4->sampleCount = 1024;
       mp4->mixBuffer = NULL;
       //printf("time scale %u\nduration %llu\n", atoms->trak[0].time_scale, atoms->trak[0].duration);
       mp4->size = round(((double)atoms->trak[0].duration/(float)atoms->trak[0].time_scale)*(double)mp4->frequency);
       mp4->scale = (float)atoms->trak[0].frequency/(float)getsysFrequency();
       mp4->stream = 1;
       mp4->lVolume = PSP_VOLUME_MAX;
       mp4->rVolume = PSP_VOLUME_MAX;
       mp4->file = aac_handle;
       mp4->fstate = STATE_F_OPEN;
       mp4->flags = AUDIO_FLAG_SCEUID;
       mp4_extra* extra = calloc(1, sizeof(mp4_extra));
       if (!extra){
          printf("calloc failed on Audio.extra\n");
          mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
          goto error;
       }
       mp4->extra = (audioExtra*)extra;
       extra->data = memalign(64, (6144/8)*2);
       if (!extra->data){
          printf("memalign failed on extra->data\n");
          mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
          goto error;
       }
       extra->size = sizeof(mp4_extra);
       extra->mp4_codec_buffer = aac_codec_buffer;
       extra->frequency = atoms->trak[0].frequency;
       extra->stream = (StreamEventHandler) mp4_EventHandler;
       extra->index_count = mp4->size/1024;;
       extra->tracks = tracks;       
       extra->atoms = atoms;
       //extra->bitrate = 0;//how to find out the bitrate?
       mp4->data = memalign(64, mp4->sampleCount*2*mp4->channels*5);
       if (!mp4->data){
          printf("memalign failed on Audio.data\n");
          mpegsetLastError(MPEG_ERROR_MEM_ALLOC);
          goto error;
       }
       extra->thid = sceKernelCreateThread("mp4_stream", mp4_thread, 0x10, 8*0x400, PSP_THREAD_ATTR_USER, NULL);
       if (extra->thid < 0){
          printf("Error thread creation \"aac_stream\"\n");
          free(extra);
          mpegsetLastError(MPEG_ERROR_THREAD_FAIL);
          goto error;
       }
       if (!strrchr(filename, ':')){
          char path[256];
          getcwd(path, 256);
          mp4->filename = malloc(strlen(path)+strlen(filename)+2);
          if (mp4->filename){
             strcat(strcat(strcpy(mp4->filename, path), "/"), filename);
          }
          else{
             printf("Error memory\n");
             audiosetLastError(MPEG_ERROR_MEM_ALLOC);
          }
       }
       else{ 
          mp4->filename = malloc(strlen(filename)+1);
          if (mp4->filename){
             strcpy(mp4->filename, filename);
          }
          else{
             printf("Error memory\n");
             audiosetLastError(MPEG_ERROR_MEM_ALLOC);
          }
       }
       sceKernelStartThread(extra->thid, 4, &mp4);
       register_audio(mp4);
       return mp4;
       error:
             sceIoClose(aac_handle);
             if (atoms){
                if (atoms->trak){
                   free(atoms->trak);
                }
                free(atoms);
                if (aac_codec_buffer){
                   if (aac_getEDRAM){
                      sceAudiocodecReleaseEDRAM(aac_codec_buffer);
                      if (mp4){
                         if (mp4->extra){
                            if (((mp4_extra*)mp4->extra)->data){
                               free(((mp4_extra*)mp4->extra)->data);
                            }
                            for (i = 0; i < tracks; i++){
                                free(((mp4_extra*)mp4->extra)->atoms->trak[i].offsets);
                                if (((mp4_extra*)mp4->extra)->atoms->trak[i].sample_size == 0){
                                   free(((mp4_extra*)mp4->extra)->atoms->trak[i].sample_sizes);
                                }
                                free(((mp4_extra*)mp4->extra)->atoms->trak[i].spc);
                            }
                            free(((mp4_extra*)mp4->extra)->atoms->trak);
                            free(((mp4_extra*)mp4->extra)->atoms);
                            free(mp4->extra);
                         }
                         if (mp4->data){
                            free(mp4->data);
                         }
                         free(mp4);
                      }
                   }
                   free(aac_codec_buffer);
                }
             }
       return NULL;
}
        
int decodeAacFrame(unsigned long *aac_codec_buffer, SceUID fd, u32 samples_per_frame, void *aac_data_buffer, short *aac_mix_buffer){
    unsigned char aac_header_buf[7]; 
    if (sceIoRead(fd, aac_header_buf, 7) != 7) { 
       printf("Read < 7\n");
       return -1;
    } 
    int frame_size = getAacFrameSize(aac_header_buf) - 7;
  
    if (sceIoRead(fd, aac_data_buffer, frame_size) != frame_size) { 
       printf("Read != frame_size\n");
       return -1;
    }
       
    aac_codec_buffer[6] = (unsigned long)aac_data_buffer; 
    aac_codec_buffer[8] = (unsigned long)aac_mix_buffer; 
       
    aac_codec_buffer[7] = frame_size; 
    aac_codec_buffer[9] = samples_per_frame * 4;  
    
    int res = sceAudiocodecDecode(aac_codec_buffer, SceAacCodec); 
    if (res < 0) { 
       printf("aac sceAudiocodecDecode = 0x%x\n", res);
       if (findAacSync(fd) < 0) return -1;
       return 0; 
    }
    return samples_per_frame; 
}

typedef struct{
        char *Title,
             *Artist,
             *Album,
             *Year,
             *Comment,
             *Track,
              Genre;
} ID3v1;

ID3v1 *getMp3ID3Info(char *filename){
      if (!filename) return NULL;
      SceUID fd = sceIoOpen(filename, PSP_O_RDONLY, 0777);
      if (fd < 0){
         return NULL;       
      }
      sceIoLseek32(fd, -128, PSP_SEEK_END);
      char Tag[128];
      sceIoRead(fd, Tag, 128);
      sceIoClose(fd);
      if (strncmp(Tag, "TAG", 3) != 0){
         return NULL;
      }
      ID3v1 *ID3 = malloc(sizeof(ID3));
      int length = strlen(Tag+3);
      if (length > 30)
         length = 30;
     
      ID3->Title = malloc(length+1);
      strncpy(ID3->Title, Tag+3, length);
     
      length = strlen(Tag+33);
      if (length > 30)
         length = 30;
      
      ID3->Artist = malloc(length+1);
      strncpy(ID3->Artist, Tag+33, length);
     
      length = strlen(Tag+33);
      if (length > 30)
         length = 30;
     
      ID3->Album = malloc(length+1);
      strncpy(ID3->Album, Tag+63, length);

      ID3->Year = malloc(5);
      strncpy(ID3->Year, Tag+93, 4);
     
      length = strlen(Tag+97);
      if (length > 30)
         length = 30;
     
      ID3->Comment = malloc(length+1);
      strncpy(ID3->Comment, Tag+97, length);
     
      ID3->Track = malloc(4);
      sprintf(ID3->Track, "%d", Tag[126]);
     
      ID3->Genre = Tag[127];
     
      return ID3;
}
