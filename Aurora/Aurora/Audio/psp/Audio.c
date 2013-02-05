/*
 * Audio.c
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
#include "Audio.h"

static int AvcodecLoaded = 0;
static int SascoreLoaded = 0;
static int Atrac3PlusLoaded = 0;
static int MpegbaseLoaded = 0;
static int Mp3Loaded = 0;
static int VAudioLoaded = 0;
static int AacLoaded = 0;
static int G729Loaded = 0;

static int sysFrequency;
inline int getsysFrequency(){
       return sysFrequency;
}
static int SRC = 0;
inline int isSRC(){
       return SRC;
}
static int SRCReserved = 0;
inline int isSRCReserved(){
       return SRCReserved;
}
static int init = 0;

static SceUID PowerCallbackID = 0;
static SceUID PowerCallbackthid = 0;

#define EndAudioMax 10
EndAudio *EndAudioProcs[EndAudioMax];
inline void dummy_printf(const char *format, ...){
}

int audiosetLastError(int error){
    static SceUID err = 0;
    if (error & AUDIO_GET_LAST_ERROR)
       return err | AUDIO_ERROR_ERROR;
    int ret = err;
    err = error;
    return ret;
}
//#define audiogetLastError() audiosetLastError(AUDIO_GET_LAST_ERROR)

#ifdef AUDIO_REGRESSION
#ifndef NO_RESAMPLE
float resample(int cfrequency, int nfrequency, int writeSamples, int readSamples, float offset, s16 *dest, s16 *src, int channels){
      float scale = (float)cfrequency/(float)nfrequency, inc;
      int done;
      for (done = 0; done < writeSamples; done++){
          inc = done*scale + offset;
          int szInc = (int)inc;
          if (szInc >= readSamples) break;
          if (channels == 2)          
             szInc *= 2;
          *dest = src[szInc];
          dest++;
          if (channels == 2){
             *dest = src[szInc+1];
             dest++;
          }
      }
      
      return done*scale + offset;
}
#ifndef NO_UPSCALE
float resample8To16(int cfrequency, int nfrequency, int writeSamples, int readSamples, float offset, s16 *dest, u8 *src, int channels){
      float scale = (float)cfrequency/(float)nfrequency, inc;
      int done;
      for (done = 0; done < writeSamples; done++){
          inc = done*scale + offset;
          int szInc = (int)inc;
          if (szInc >= readSamples) break;
          if (channels == 2)          
             szInc *= 2;
          *dest = src[szInc]*256 - 32768;
          dest++;
          if (channels == 2){
             *dest = src[szInc+1]*256 - 32768;
             dest++;
          }
      }      
      return done*scale + offset;
}
#endif
#endif
#endif

void changeEdianData(short *data, int size){//aligned on 16 bits
     int read = 0;
     short sample;
     while (read < size){
           sample = *data;
           read += 2;
           *data = changeEdian16(sample);
           data++;
     }
}

#ifndef WAV_SAMPLES
#define WAV_SAMPLES 512
#endif

typedef struct{
       PAudio audio;
       int offset;
}NodeData;
#include "linkedlists.h"
typedef Node2Ptr NodePtr;
typedef Node2 Node;
NodePtr newNode(PAudio audio, int offset){
        NodePtr new = malloc(sizeof(Node));
        new->data.audio = audio;
        new->data.offset = offset;
        new->previous = NULL;
        new->next = NULL;
        return new;
}
NodePtr copyNode(NodePtr node){
        NodePtr new = newNode(node->data.audio, node->data.offset);
        return new;
}
NodePtr freeNode(NodePtr node){
        free(node);
        return NULL;
}
NodePtr removeNode(NodePtr node){
        if (node->next)
           node->next->previous = node->previous;
        if (node->previous)
           if (node->previous->next)
              node->previous->next = node->next;
        freeNode(node);
        return NULL;
}
NodePtr copyList(NodePtr list){
        NodePtr new, current = NULL, newList = NULL;
        while (list){
              new = copyNode(list);              
              if (newList)
                 current->next = new;
              else
                 newList = new;
              current = new;
              list = list->next;
        }
        return newList;
}

NodePtr audioList;
void register_audio(PAudio audio){
     if (!audio) return;
     if (audioList == NULL){
        audioList = malloc(sizeof(Node));
        audioList->data.audio = audio;
        audioList->next = NULL;
        audioList->previous = audioList;
     }
     else{
        NodePtr a = malloc(sizeof(Node));
        a->data.audio = audio;
        a->next = NULL;
        a->previous = audioList->previous;
        audioList->previous->next = a;
        a->previous = audioList->previous;
        audioList->previous = a;
     }
}
void unregister_audio(PAudio audio){
     if (!audio) return;
     if (audioList == NULL)
        return;
     NodePtr tmp = audioList;
     while (tmp){
           if (tmp->data.audio == audio){
              if (tmp == audioList){
                 audioList = audioList->next;
              }
              removeNode(tmp);
              return;         
           }
           tmp = tmp->next;
     }
}

int power_callback(int count, int flags, void *common){
    NodePtr tmp = audioList;
    if (flags & PSP_POWER_CB_SUSPENDING){
        while (tmp){       
           if (tmp->data.audio->ufunc)
              tmp->data.audio->ufunc(tmp->data.audio, EVENT_suspending);
           if (tmp->data.audio->stream){
              int play = tmp->data.audio->play;
              tmp->data.audio->play = STOP;
              while (tmp->data.audio->extra->working) sceKernelDelayThread(5);
              sceKernelSuspendThread(tmp->data.audio->extra->thid);
              tmp->data.audio->play = play;
              if (tmp->data.audio->extra->stream){
                 eventData e = {EVENT_suspending, 0, 0};
                 if (tmp->data.audio->extra->stream(tmp->data.audio, &e)){
                    goto nextS;
                 }
              }
              if (tmp->data.audio->stream == STREAM_FROM_FILE){
                 if (tmp->data.audio->flags&AUDIO_FLAG_SCEUID){                    
                    tmp->data.offset = sceIoLseek32((SceUID)tmp->data.audio->file, 0, SEEK_CUR);
                    sceIoClose((SceUID)tmp->data.audio->file);
                 }
                 else if (tmp->data.audio->flags&AUDIO_FLAG_FILE){
                    tmp->data.offset = ftell((FILE*)tmp->data.audio->file);
                    fclose((FILE*)tmp->data.audio->file);
                 }
              }
           }
           nextS:
           tmp = tmp->next;
        }
    }
    else if (flags & PSP_POWER_CB_RESUME_COMPLETE){
        while (tmp){
           if (tmp->data.audio->ufunc)
              tmp->data.audio->ufunc(tmp->data.audio, EVENT_resumeComplete);
           if (tmp->data.audio->stream){
              if (tmp->data.audio->extra->stream){
                 eventData e = {EVENT_resumeComplete, 0, 0};
                 if (tmp->data.audio->extra->stream(tmp->data.audio, &e)){
                    goto nextR;
                 }
              }
              if (tmp->data.audio->stream == STREAM_FROM_FILE){
                 if (tmp->data.audio->flags&AUDIO_FLAG_SCEUID){
                    tmp->data.audio->file = sceIoOpen(tmp->data.audio->filename, PSP_O_RDONLY, 0777);
                    tmp->data.offset = sceIoLseek32((SceUID)tmp->data.audio->file, tmp->data.offset, PSP_SEEK_SET);
                 }
                 else if (tmp->data.audio->flags&AUDIO_FLAG_FILE){
                    tmp->data.audio->file = (u32)fopen(tmp->data.audio->filename, "rb");
                    tmp->data.offset = fseek((FILE*)tmp->data.audio->file, tmp->data.offset, SEEK_SET);
                 }
              }
           }
           nextR:
           sceKernelResumeThread(tmp->data.audio->extra->thid);
           tmp = tmp->next;
        }
    }
    return 0;
}

void reset_wav(PAudio wav){
     sceIoLseek32(wav->file, wav->extra->start, PSP_SEEK_SET);
     wav->offset = 0;
     wav->extra->fill = 0;
     wav->extra->eof = 0;
}
int signal_wav(PAudio wav, eventData *event){
    int ret;
    switch (event->event){
       case EVENT_stopAudio :
          wav->play = STOP;
          while (wav->extra->working) sceKernelDelayThread(10);
		  reset_wav(wav);
		  return 0;
       break;
       case EVENT_freeAudio :
          wav->play = TERMINATED;
          sceKernelWaitThreadEnd(wav->extra->thid, 0);
          sceKernelTerminateDeleteThread(wav->extra->thid);
          ret = sceIoClose(wav->file);
          if (ret < 0){
             audiosetLastError(ret);
          }
          free(wav->extra);
          return 0;
       break;
       case EVENT_getFrequencyAudio :
          return wav->extra->frequency;
       break;
       case EVENT_seekAudio :{//GCC :P
          int seek = event->data;
          if (event->dataType == SZ_SECONDS){
             seek = (seek * wav->frequency)/1000;
          }
          int seekBytes = seek*(wav->channels*(wav->bitRate/8));
          if (seekBytes < 0) return 0;
          while (wav->extra->working) sceKernelDelayThread(10);
          int ret = sceIoLseek(wav->file, seekBytes+wav->extra->start, SEEK_SET);
          if (ret < 0) audiosetLastError(ret);
          wav->offset = seek;
          wav->frac = 0;
       }
       break;
       default :
          return 0;
    }
    return 1;
}

typedef s16 (*Scale)(const void *);
s16 scale8(const u8 *sample){
    return (s16)((*sample)<<8) - 32768;
}
s16 scale16(const s16 *sample){
    return sample[0];
}
#ifdef ALLOW_24_32_BITS
s16 scale24(const u8 *sample){//meh
    return sample[1]|(sample[2] << 8);
}
s16 scale32(const u8 *sample){
    return sample[2]|(sample[3] << 8);
}
#endif
typedef void (* AudioCallback_t)(void *buf, unsigned int reqn, PAudio pdata);
int sndThreadIds[NUM_AUDIO_CHANNELS];
PAudio sndThreadAudios[NUM_AUDIO_CHANNELS];
int sndThreadChannels[NUM_AUDIO_CHANNELS];
AudioCallback_t sndThreadCallbacks[NUM_AUDIO_CHANNELS];
static int channels[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static int threads;
static int getChannel(){
    int i;
    for (i = 0; i < threads; i++){
        if (channels[i] == 0){
           channels[i] = 1;
           return i;
        }
    }
    return -1;
}
static void releaseChannel(int ch){
    if (ch < threads)
        channels[ch] = 0;
}
int setAudioChannelCallback(int channel, AudioCallback_t callback, PAudio pdata){
    sndThreadCallbacks[channel] = callback;
    sndThreadAudios[channel] = pdata;
    sndThreadChannels[channel] = sceAudioChReserve(PSP_AUDIO_NEXT_CHANNEL, PSP_NUM_AUDIO_SAMPLES, PSP_AUDIO_FORMAT_STEREO);
    if (sndThreadChannels[channel] < 0) return 0;
    pdata->play = PLAYING;
    sceKernelWakeupThread(sndThreadIds[channel]);
    return 1;
}
int sound_thread(SceSize args, void *argp){
    int thread = *(int*)argp;
    int x = 0;
    s16 data[4][PSP_NUM_AUDIO_SAMPLES*2];
    sceKernelSleepThread();
    while (1){//till something better comes along
          if (sndThreadAudios[thread]->play != PLAYING){
             sceKernelSleepThread();
          }
          sndThreadCallbacks[thread](data[x], PSP_NUM_AUDIO_SAMPLES, sndThreadAudios[thread]);
          while (sceAudioGetChannelRestLength(sndThreadChannels[thread]))
                sceKernelDelayThread(10);
          sceAudioOutputPanned(sndThreadChannels[thread], sndThreadAudios[thread]->lVolume, sndThreadAudios[thread]->rVolume, data[x]);
           
          x = (x+1)&0x3;//(x+1)%4;
    }
    return 0;
}

int wav_thread(SceSize args, void *argp){
    PAudio wav = *(PAudio*)argp;
    audioExtra *extra = wav->extra;
    Scale scale = NULL;
    int size = 0;
    u32 bitRate = wav->bitRate;
    u32 channels = wav->channels;
    switch (bitRate){
        case 8:
           size = 1;
           scale = (Scale)scale8;
        break;
        case 16:
           size = 2;
           scale = (Scale)scale16;
        break;
#ifdef ALLOW_24_32_BITS
        case 24:
           size = 3;
           scale = (Scale)scale24;
        break;
        case 32:
           size = 4;
           scale = (Scale)scale32;
        break;
#endif
        default:
        break;
    }
    size *= channels;
    int length = WAV_SAMPLES*size;
    char read[length];//quick fix
    int x = 0;
    int samples;
    int ret = 0;
    s16 *data[4] = {wav->data, wav->data+(WAV_SAMPLES*channels*2), wav->data+(WAV_SAMPLES*channels*2)*2, wav->data+(WAV_SAMPLES*channels*2)*3};
    extra->fill = 0;
    start:
    while (1){//till something better comes along
          if (wav->play != PLAYING){
             if (wav->play == TERMINATED) return 0;
             sceKernelDelayThread(50);
             continue;
          }
           
          extra->working = 1;
          int fread = sceIoRead(wav->file, 
          ((wav->frequency != getsysFrequency() || bitRate != 16) && !SRC)?read:(void*)data[x], 
          length);
          extra->working = 0;
          if (fread < length){
             if (fread <= 0)
                if (!extra->fill) break;
             samples = fread/size;
             extra->eof = 1;
          }
          else
             samples = WAV_SAMPLES;
          if (wav->offset+samples > wav->size){
             samples = wav->size - wav->offset;
             extra->eof = 1;
          }
          wav->offset += samples;
          if (SRC || (wav->frequency == getsysFrequency() && bitRate == 16)){
             if (extra->eof)//loop?
                memset(data[x]+samples*channels, 0, (WAV_SAMPLES-samples)*channels*2);
             wav->mixBuffer = data[x];
             if (SRC)
                ret = sceAudioSRCOutputBlocking(wav->lVolume, data[x]);
             else
                ret = sceAudioOutputPannedBlocking(wav->channel, wav->lVolume, wav->rVolume, data[x]);
          }
          else{
             float i = 0;
             while (i < samples){
                   data[x][extra->fill*channels] = scale(&(read[((int)i)*size]));
                   if (channels == 2){
                      data[x][extra->fill*channels+1] = scale(&(read[((int)i)*size+(size>>1)]));
                   }
                      
                   extra->fill++;
                   i += wav->scale;
                   
                   if (extra->fill == WAV_SAMPLES){
                      wav->mixBuffer = data[x];
                      ret = sceAudioOutputPannedBlocking(wav->channel, wav->lVolume, wav->rVolume, data[x]);
                      if (ret < 0){
                         printf("wav sceAudioOutputPannedBlocking 0x%x\n", ret);
                      }
                      extra->fill = 0;
                      x = (x+1)&0x3;
                   }
             }
             if (extra->eof){
                if (extra->fill){
                   memset(data[x]+extra->fill*channels, 0, (WAV_SAMPLES-extra->fill)*channels*2);
                   wav->mixBuffer = data[x];
                   ret = sceAudioOutputPannedBlocking(wav->channel, wav->lVolume, wav->rVolume, data[x]);
                   extra->fill = 0;
                   x = (x+1)&0x3;
                }
             }                   
             x = (x-1)&0x3;
          }
          if (ret < 0){
             printf("wav sceAudioOutputPannedBlocking 0x%x\n", ret);
          }
          x = (x+1)&0x3;//(x+1)%4;
          if (extra->eof) break;
    }
    if (wav->loop)
       reset_wav(wav);
    else
       stopAudio(wav);
    goto start;
    return 1;
}
int bigE_thread(SceSize args, void *argp){
    PAudio au = *(PAudio*)argp;
    audioExtra *extra = au->extra;
    Scale scale = NULL;
    int size = 0;
    u32 bitRate = au->bitRate;
    u32 channels = au->channels;
    switch (bitRate){
        case 8:
           size = 1;
           scale = (Scale)scale8;
        break;
        case 16:
           size = 2;
           scale = (Scale)scale16;
        break;
        default:
        break;
    }
    size *= channels;
    int length = WAV_SAMPLES*size;
    char read[length];
    int x = 0;
    int samples;
    int ret = 0;
    s16 *data[4] = {au->data, au->data+(WAV_SAMPLES*channels*2), au->data+(WAV_SAMPLES*channels*2)*2, au->data+(WAV_SAMPLES*channels*2)*3};
    extra->fill = 0;
    memset(data[x], 0, WAV_SAMPLES*channels*2);
    start:
    while (1){//till something better comes along
          if (au->play != PLAYING){
             if (au->play == TERMINATED) return 0;
             sceKernelDelayThread(50);
             continue;
          }
           
          extra->working = 1;
          int fread = sceIoRead(au->file, 
          ((au->frequency != getsysFrequency() || bitRate != 16) && !SRC)?read:(void*)data[x], length);
          extra->working = 0;
          if (fread < length){
             if (fread <= 0){
                if (!extra->fill) break;}
             samples = fread/size;
             extra->eof = 1;
          }
          else
             samples = WAV_SAMPLES;
          if (au->offset+samples > au->size){
             samples = au->size - au->offset;
             extra->eof = 1;
          }
          au->offset += samples;
          if (SRC ||(au->frequency == getsysFrequency() && bitRate == 16)){
             changeEdianData(data[x], fread);
             if (extra->eof)
                memset(data[x]+samples*channels, 0, (WAV_SAMPLES-samples)*channels*2);
             au->mixBuffer = data[x];
             ret = sceAudioSRCOutputBlocking(au->lVolume, data[x]);
             ret = sceAudioOutputPannedBlocking(au->channel, au->lVolume, au->rVolume, data[x]);
          }
          else{
#ifndef TEST_RESAMPLING
             float sc = au->scale;
             float i = 0;
             short sample;
             while (i < samples){
                   sample = scale(&(read[((int)i)*size]));
                   if (bitRate == 16)
                      sample = changeEdian16(sample);
                   data[x][extra->fill*channels] = sample;
                   if (channels == 2){
                      sample = scale(&(read[((int)i)*size+(size>>1)]));
                      if (bitRate == 16)
                         sample = changeEdian16(sample);
                      data[x][extra->fill*channels+1] = sample;
                   }
                      
                   extra->fill++;
                   i += sc;
                   
                   if (extra->fill == WAV_SAMPLES){
                      au->mixBuffer = data[x];
                      ret = sceAudioOutputPannedBlocking(au->channel, au->lVolume, au->rVolume, data[x]);
                      if (ret < 0){
                         printf("bigE sceAudioOutputPannedBlocking 0x%x\n", ret);
                      }
                      extra->fill = 0;
                      x = (x+1)&0x3;
                   }
             }
             if (extra->eof){
                if (extra->fill){
                   memset(data[x]+extra->fill*channels, 0, (WAV_SAMPLES-extra->fill)*channels*2);
                   au->mixBuffer = data[x];
                   ret = sceAudioOutputPannedBlocking(au->channel, au->lVolume, au->rVolume, data[x]);
                   extra->fill = 0;
                   x = (x+1)&0x3;
                }
             }                   
             x = (x-1)&0x3;
#else
             float sc = (float)getsysFrequency()/(float)au->frequency;
             int i = 0; 
             int j = 0;
             short sample;
             while (samples){
                   sample = scale(&(read[((int)i)*size]));
                   if (bitRate == 16)
                      sample = changeEdian16(sample);
                   data[x][extra->fill*channels] = sample;
                   if (channels == 2){
                      sample = scale(&(read[((int)i)*size+(size>>1)]));
                      if (bitRate == 16)
                         sample = changeEdian16(sample);
                      data[x][extra->fill*channels+1] = sample;
                   }
                   
                   samples--;
                   j++;   
                   extra->fill = j*sc;
                   i++;
                   
                   if (extra->fill >= WAV_SAMPLES){
                      au->mixBuffer = data[x];
                      ret = sceAudioOutputPannedBlocking(au->channel, au->lVolume, au->rVolume, data[x]);
                      if (ret < 0){
                         printf("bigE sceAudioOutputPannedBlocking 0x%x\n", ret);
                      }
                      extra->fill = 0;
                      j = 0;
                      x = (x+1)&0x3;
                      memset(data[x], 0, WAV_SAMPLES*channels*2);
                   }
             }
             if (extra->eof){
                if (extra->fill){
                   memset(data[x]+extra->fill*channels, 0, (WAV_SAMPLES-extra->fill)*channels*2);
                   au->mixBuffer = data[x];
                   ret = sceAudioOutputPannedBlocking(au->channel, au->lVolume, au->rVolume, data[x]);
                   extra->fill = 0;
                   x = (x+1)&0x3;
                   memset(data[x], 0, WAV_SAMPLES*channels*2);
                }
             }                   
             x = (x-1)&0x3;
#endif
          }
          if (ret < 0){
             printf("bigE sceAudioOutputPannedBlocking 0x%x\n", ret);
          }
          x = (x+1)&0x3;//(x+1)%4;
          if (extra->eof) break;
    }
    if (au->loop)
       reset_wav(au);
    else
       stopAudio(au);
    goto start;
    return 1;
}

#ifndef NO_RESAMPLING
void audioCallback(void* buf, unsigned int length, void *userdata){//
     Audio a = *(PAudio)userdata; 

     int size = a.channels*(a.bitRate/8); 
     short *tbuf = (short*)buf; 
     void *start = a.data+(a.offset*size); 
     float inc = 0;
     short l, r;
     int done;
     for (done = 0; done < length; done++){
           inc = ((done*a.scale)+a.frac);
           int szInc = a.channels*(int)inc;
           if (a.bitRate == 8){
              u8 *data = start;
              if (a.channels == 1)
                 l = r = data[szInc]*256 - 32768;
              else{
                 l = data[szInc]*256 - 32768;
                 r = data[szInc+1]*256 - 32768;
              }
           }
           else{
              short* data = start;
              if (a.channels == 1)
                 l = r = data[szInc];
              else{
                 l = data[szInc];
                 r = data[szInc+1];
              }
           }
           if ((a.offset + inc) >= a.size){
              if (!a.loop){
                 memset(tbuf, 0, (length-done)*4);
                 stopAudio((PAudio)userdata);
                 return;
              }
              a.offset = 0;
              a.frac = 0;
              start = a.data+(a.offset*size); 
           }
           *tbuf = l;
           tbuf++;
           *tbuf = r;
           tbuf++;
     }
     a.offset += (int)inc;      
     a.frac = inc - (int)inc;
     a.mixBuffer = buf;
     *(PAudio)userdata = a;             
}
#endif
void audioCallback2(void* buf, unsigned int length, void *userdata){
     PAudio a = (PAudio)userdata; 
     int written = 0;
     while (written < length){
           int avail = a->size - a->offset;
           int copy;
           if (avail >= length)
              copy = length;
           else
              copy = avail;
           memcpy(buf+(written*4), a->data+(a->offset*4), copy*4);
           written += copy;
           a->offset += copy;
           if (a->offset >= a->size){
              if (!a->loop){
                 memset(buf+(written*4), 0, (length-written)*4);
                 a->mixBuffer = buf;
                 stopAudio(a);
                 return;
              }
              a->offset = 0;
           }
     }
     a->mixBuffer = buf;
}

int getID3v2TagSize(const char *id3){
    if (strncmp(id3, "ID3", 3) != 0)
       if (strncmp(id3, "ea3", 3) != 0)
          return 0;
    if (id3[3] == -1 || id3[4] == -1){;//id3[3] < 0xff || id3[4] < 0xff for unsigned
       return 0;}
    if (id3[6] < 0 || id3[7] < 0 || id3[8] < 0 || id3[9] < 0){//id3[6] < 0x80 ... for unsigned
       return 0;}
    int size = id3[6];
    size = (size<<7)|id3[7];
    size = (size<<7)|id3[8];
    size = (size<<7)|id3[9];
    return size + 10;
}
int skipID3v2Tag(SceUID fd){
    char id3[10];
    if (sceIoRead(fd, id3, sizeof(id3)) < sizeof(id3))
       return 0;
    int size = getID3v2TagSize(id3);
    sceIoLseek32(fd, size - sizeof(id3), PSP_SEEK_CUR);
    return size;
}
#ifndef USE_ONLY_SCE_IO
int skipID3v2TagFILE(FILE *file){
    char id3[10];
    if (fread(id3, 1, sizeof(id3), file) < sizeof(id3))
       return 0;
    int size = getID3v2TagSize(id3);
    fseek(file, size - sizeof(id3), SEEK_CUR);
    return size;
}
#endif
void *skipID3v2TagMEMORY(void *data, int size){//not a good idea
     if (size < 10) return 0;
     return data+getID3v2TagSize(data);
}
int getFormat(fmt *f, SceUID in, int dsize){
    sceIoRead(in, f, sizeof(fmt)); 
    if (dsize != sizeof(fmt)){
       printf("fmt %d\n", sizeof(fmt));
       sceIoLseek32(in, dsize - sizeof(fmt), PSP_SEEK_CUR);
    }
    if (f->id != 1){
       printf("Error fmt id %d\n", f->id);
       return 0;
    }
    if ((f->channels != 1)&&(f->channels != 2)){
       printf("Error channels %d\n", f->channels);
       return 0;
    }
    if ((f->bitRate != 8)&&(f->bitRate != 16)
#ifdef ALLOW_24_32_BITS
    &&(f->bitRate != 24)&&(f->bitRate != 32)
#endif
    ){
       printf("Error bits per smple %d\n", f->bitRate);
       return 0;
    }
    if ((f->frequency < 8000)||(f->frequency > 96000)){
       printf("Error frequency %d\n", f->frequency);
       return 0;
    }
/*    if ((f->unk1 > 4)||(f->unk1 == 3)){//no NEED to check this
       printf("Error unk1\n");
       return 0;
    }*/
    return 1;
}
PAudio loadWav(const char *filename, int stream){
       if (!filename) return NULL;

       SceUID in = sceIoOpen(filename, PSP_O_RDONLY, 0777);
       if (in < 0){
          printf("Error file %d\n", in);
          audiosetLastError(AUDIO_ERROR_OPEN_FILE);
          return NULL;
       }

       RIFFHeader head; 
       sceIoRead(in, &head, 12);
       if (head.fsig != RIFF){
          printf("Error RIFF\n");
          sceIoClose(in);
          audiosetLastError(AUDIO_ERROR_INVALID_FILE);
          return NULL;
       } 
       if (head.fmt != WAVE){
          printf("Error WAVE\n");
          sceIoClose(in);
          audiosetLastError(AUDIO_ERROR_INVALID_FILE);
          return NULL;
       }

       chunk cnk;
       fmt f;

       while (sceIoRead(in, &cnk, 8) == 8){
             if (cnk.csig == fmt__){
                if (!getFormat(&f, in, cnk.dsize)){
                   sceIoClose(in);
                   audiosetLastError(AUDIO_ERROR_INVALID_FILE);
                   return NULL;
                }
                cnk.dsize = 0;//no seeking;
             }
             else 
                if (cnk.csig == data_)
                   break;                
             sceIoLseek32(in, cnk.dsize, PSP_SEEK_CUR);
       }
       int start = sceIoLseek32(in, 0, PSP_SEEK_CUR);
       if (!stream){
          cnk.data = malloc(cnk.dsize);
          if (!cnk.data){
             printf("Error memory\n");
             sceIoClose(in);
             audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
             return NULL;
          }
          if (sceIoRead(in, cnk.data, cnk.dsize) != cnk.dsize){
             printf("Error read\n");
             sceIoClose(in);
             free(cnk.data);
             audiosetLastError(AUDIO_ERROR_FILE_READ);
             return NULL;
          }
          sceIoClose(in); 
       }
       PAudio wav = malloc(sizeof(Audio));
       if (!wav){
          printf("Error memory audio\n"); 
          if (!stream)
             free(cnk.data);
          audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
          return NULL;
       }
       wav->type = 1;
       wav->channels = f.channels;
       wav->frequency = f.frequency;
       wav->bitRate = f.bitRate;
       wav->data = cnk.data;
       wav->size = cnk.dsize/f.unk1;
       wav->scale = (float)f.frequency/(float)getsysFrequency();
       wav->offset = 0;
       wav->frac = 0;
       wav->stream = stream;
       wav->channel = 0x80026008;
       wav->sampleCount = stream?WAV_SAMPLES:PSP_NUM_AUDIO_SAMPLES; 
       wav->mixBuffer = NULL; 
       wav->lVolume = PSP_VOLUME_MAX;
       wav->rVolume = PSP_VOLUME_MAX;
       wav->play = 0;
       wav->loop = 0;
       wav->ufunc = NULL;
       wav->flags = 0;
       wav->extra = NULL;
       if (stream){
          wav->stream = stream;
          wav->file = in;
          wav->fstate = STATE_F_OPEN;
          wav->flags = AUDIO_FLAG_SCEUID;
          wav->extra = malloc(sizeof(audioExtra));
          wav->extra->frequency = wav->frequency;
          wav->extra->working = 0;
          wav->extra->eof = 0;
          wav->extra->size = sizeof(audioExtra);
          wav->extra->start = start;
          wav->extra->stream = (StreamEventHandler) signal_wav;
          wav->data = malloc(WAV_SAMPLES*wav->channels*2*4);
          if (!wav->data){
             printf("Error malloc\n");
             free(wav->extra);
             free(wav);
             sceIoClose(in);
             audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
             return NULL;
          }
          wav->extra->thid = sceKernelCreateThread("wav_stream", wav_thread, 0x10, 0x400*8, PSP_THREAD_ATTR_USER, NULL);
          if (wav->extra->thid < 0){
             printf("Error thread creation\n");
             free(wav->extra);
             free(wav);
             sceIoClose(in);
             audiosetLastError(AUDIO_ERROR_THREAD_FAIL);
             return NULL;
          }
          if (!strrchr(filename, ':')){
             char path[256];
             getcwd(path, 256);
             wav->filename = malloc(strlen(path)+strlen(filename)+2);
             if (wav->filename){
                strcat(strcat(strcpy(wav->filename, path), "/"), filename);
             }
             else{
                printf("Error memory\n");
                audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
             }
          }
          else{
             wav->filename = malloc(strlen(filename)+1);
             if (wav->filename){
                strcpy(wav->filename, filename);
             }
             else{
                printf("Error memory\n");
                audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
             }
          }
          sceKernelStartThread(wav->extra->thid, 4, &wav);
       }
       register_audio(wav);
       return wav;       
}
PAudio loadAu(const char *filename, int stream){
       if (!filename) return NULL;

       SceUID in = sceIoOpen(filename, PSP_O_RDONLY, 0777);
       if (in < 0){
          printf("Error file %d\n", in);
          audiosetLastError(AUDIO_ERROR_OPEN_FILE);
          return NULL;
       }

       au_header head; 
       sceIoRead(in, &head, sizeof(head));
       if (head.magic != 0x646e732e){
          printf("Error .snd\n");
          sceIoClose(in);
          audiosetLastError(AUDIO_ERROR_INVALID_FILE);
          return NULL;
       } 
       if ((changeEdian(head.encoding) != 2) && (changeEdian(head.encoding) != 3)){
          printf("unsupported %d\n", changeEdian(head.encoding));
          sceIoClose(in);
          audiosetLastError(AUDIO_ERROR_INVALID_FILE);
          return NULL;
       } 
       int start = sceIoLseek32(in, changeEdian(head.offset), PSP_SEEK_SET);
       void *data = NULL;
       head.size = changeEdian(head.size);
       head.channels = changeEdian(head.channels);
       head.sample_rate = changeEdian(head.sample_rate);
       head.encoding = (changeEdian(head.encoding)==2)?8:16;
       if (head.size  < 0){
          head.size = sceIoLseek32(in, 0, PSP_SEEK_END) - start;
          sceIoLseek32(in, start, PSP_SEEK_SET);
       }
          
       if (!stream){
          data = malloc(head.size);
          if (!data){
             printf("Error memory %d \n", head.size);
             sceIoClose(in);
             audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
             return NULL;
          }
          if (sceIoRead(in, data, head.size) != head.size){
             printf("Error read\n");
             sceIoClose(in);
             free(data);
             audiosetLastError(AUDIO_ERROR_FILE_READ);
             return NULL;
          }
          if (changeEdian(head.encoding)==3){
             changeEdianData(data, head.size);
          }
          sceIoClose(in); 
       }
       PAudio au = malloc(sizeof(Audio));
       if (!au){
          printf("Error memory audio\n"); 
          if (data) 
             free(data);
          audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
          return NULL;
       }
       au->type = 0x00007561;//'\0\0ua';
       au->channels = head.channels;
       au->frequency = head.sample_rate;
       au->bitRate = head.encoding;
       au->data = data;
       au->size = head.size/(head.channels*(head.encoding/8));
       au->scale = (float)head.sample_rate/(float)getsysFrequency();
       au->offset = 0;
       au->frac = 0;
       au->stream = stream;
       au->channel = 0x80026008;
       au->sampleCount = stream?WAV_SAMPLES:PSP_NUM_AUDIO_SAMPLES; 
       au->mixBuffer = NULL; 
       au->lVolume = PSP_VOLUME_MAX;
       au->rVolume = PSP_VOLUME_MAX;
       au->play = 0;
       au->loop = 0;
       au->ufunc = NULL;
       au->flags = 0;
       au->extra = NULL;
       if (stream){
          au->stream = stream;
          au->file = in;
          au->fstate = STATE_F_OPEN;
          au->flags = AUDIO_FLAG_SCEUID;
          au->extra = malloc(sizeof(audioExtra));
          au->extra->frequency = au->frequency;
          au->extra->working = 0;
          au->extra->eof = 0;
          au->extra->size = sizeof(audioExtra);
          au->extra->start = start;
          au->extra->stream = (StreamEventHandler) signal_wav;
          au->data = malloc(WAV_SAMPLES*au->channels*2*4);
          if (!au->data){
             printf("Error memory\n");
             sceIoClose(in);
             free(au->extra);
             free(au);
             audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
             return NULL;
          }
          au->extra->thid = sceKernelCreateThread("au_stream", bigE_thread, 0x10, 0x400*8, PSP_THREAD_ATTR_USER, NULL);
          if (au->extra->thid < 0){
             printf("Error thread creation\n");
             sceIoClose(in); 
             free(au->extra);
             free(au);
             audiosetLastError(AUDIO_ERROR_THREAD_FAIL);
             return NULL;
          }
          if (!strrchr(filename, ':')){
             char path[256];
             getcwd(path, 256);
             au->filename = malloc(strlen(path)+strlen(filename)+2);
             if (au->filename){
                strcat(strcat(strcpy(au->filename, path), "/"), filename);
             }
             else{
                printf("Error memory\n");
                audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
             }
          }
          else{
             au->filename = malloc(strlen(filename)+1);
             if (au->filename){
                strcpy(au->filename, filename);
             }
             else{
                printf("Error memory\n");
                audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
             }
          }
          sceKernelStartThread(au->extra->thid, 4, &au);
       }
      register_audio(au);
      return au;       
}
PAudio loadAiff(const char *filename, int stream){ 
       if (!filename) return NULL;

       SceUID in = sceIoOpen(filename, PSP_O_RDONLY, 0777);
       if (in < 0){
          printf("Error file %x\n", in);
          audiosetLastError(AUDIO_ERROR_OPEN_FILE);
          return NULL;
       }

       RIFFHeader head; 
       sceIoRead(in, &head, 12);
       if (head.fsig != FORM){
          printf("Error FORM\n");
          sceIoClose(in);
          audiosetLastError(AUDIO_ERROR_INVALID_FILE);
          return NULL;
       } 
       if ((head.fmt != AIFF)&&(head.fmt != AIFC)){
          printf("Error AIFF %x\n", head.fmt);
          sceIoClose(in);
          audiosetLastError(AUDIO_ERROR_INVALID_FILE);
          return NULL;
       }

       chunk cnk;
       CommonChunk c;

       while (sceIoRead(in, &cnk, 8) == 8){
             if (cnk.csig == COMM){
                sceIoRead(in, &c, sizeof(CommonChunk));
                sceIoLseek32(in, changeEdian(cnk.dsize)-sizeof(CommonChunk), PSP_SEEK_CUR);
                cnk.dsize = 0;
             }
             else 
                if (cnk.csig == SSND)
                   break;                
             sceIoLseek32(in, changeEdian(cnk.dsize), PSP_SEEK_CUR);
       }
       c.sampleSize = changeEdian16(c.sampleSize);
       c.numSampleFrames = changeEdian(c.numSampleFrames);
       c.numChannels = changeEdian16(c.numChannels);
       if (cnk.csig != SSND){
          printf("No ssnd %x\n", cnk.csig);
          sceIoClose(in);
          audiosetLastError(AUDIO_ERROR_INVALID_FILE);
          return NULL;
       }
       if ((c.sampleSize != 8) && (c.sampleSize != 16)){
          sceIoClose(in);
          audiosetLastError(AUDIO_ERROR_INVALID_FILE);
          return NULL;
       }
       int i;
       for (i = 5; i < 10; i++){
           if (c.sampleRate[i] != 0){
              printf("Error frequency\n");
              sceIoClose(in);
              audiosetLastError(AUDIO_ERROR_INVALID_FILE);
              return NULL;
           }
       }
       
       int frequency;
       int exponent = (((c.sampleRate[0]&0x7f)<<8)|(c.sampleRate[1]));
       exponent = exponent - (1 << 14) + 1;
       unsigned int significand = ((c.sampleRate[2]<<16)|(c.sampleRate[3]<<8)|(c.sampleRate[4]));
       while (!(significand&1)){
             significand >>= 1;
             if (significand == 0) break;//freq = 0
       }
       int bits = 23;
       int bit = 1 << 23;
       while (!(significand&bit)){
             bit >>= 1;
             bits--;
             if (bit == 0) break;//freq = 0
       }
       exponent -= bits;
       if (exponent < 0)
          frequency = significand >> -exponent;
       else
          frequency = significand << exponent;

       int start = sceIoLseek32(in, 8, PSP_SEEK_CUR);

       if (!stream){
          int size = c.numSampleFrames*(c.sampleSize/8)*c.numChannels;
          cnk.data = malloc(size);
          if (!cnk.data){
             printf("Error memory\n");
             sceIoClose(in);
             audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
             return NULL;
          }
          if (sceIoRead(in, cnk.data, size) != size){
             printf("Error read\n");
             sceIoClose(in);
             free(cnk.data);
             audiosetLastError(AUDIO_ERROR_FILE_READ);
             return NULL;
          }
          sceIoClose(in);
          if (c.sampleSize==16){
             changeEdianData(cnk.data, size);
          }          
       }
       PAudio aiff = malloc(sizeof(Audio));
       if (!aiff){
          printf("Error memory audio\n"); 
          if (!stream)
             free(cnk.data);
          sceIoClose(in);
          audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
          return NULL;
       }
       aiff->type = 'FFIA';//0x66666961;
       aiff->channels = c.numChannels;
       aiff->frequency = frequency;
       aiff->bitRate = c.sampleSize;
       aiff->data = cnk.data;
       aiff->size = c.numSampleFrames;
       aiff->scale = (float)frequency/(float)getsysFrequency();
       aiff->offset = 0;
       aiff->frac = 0;
       aiff->stream = stream;
       aiff->channel = 0x80026008;
       aiff->sampleCount = stream?WAV_SAMPLES:PSP_NUM_AUDIO_SAMPLES; 
       aiff->mixBuffer = NULL; 
       aiff->lVolume = PSP_VOLUME_MAX;
       aiff->rVolume = PSP_VOLUME_MAX;
       aiff->play = 0;
       aiff->loop = 0;
       aiff->ufunc = NULL;
       aiff->flags = 0;
       aiff->extra = NULL;
       if (stream){
          aiff->stream = stream;
          aiff->file = in;
          aiff->fstate = STATE_F_OPEN;
          aiff->flags = AUDIO_FLAG_SCEUID;
          aiff->extra = malloc(sizeof(audioExtra));
          aiff->extra->working = 0;
          aiff->extra->eof = 0;
          aiff->extra->frequency = aiff->frequency;
          aiff->extra->size = sizeof(audioExtra);
          aiff->extra->start = start;
          aiff->extra->stream = (StreamEventHandler) signal_wav;
          aiff->data = malloc(WAV_SAMPLES*aiff->channels*2*4);
          if (!aiff->data){
             printf("Error memory\n");
             sceIoClose(in);
             free(aiff->extra);
             free(aiff);
             audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
             return NULL;
          }
          aiff->extra->thid = sceKernelCreateThread("aiff_stream", bigE_thread, 0x10, 0x400*8, PSP_THREAD_ATTR_USER, NULL);
          if (aiff->extra->thid < 0){
             printf("Error thread creation\n");
             sceIoClose(in);
             free(aiff->extra);
             free(aiff);
             audiosetLastError(AUDIO_ERROR_THREAD_FAIL);
             return NULL;
          }
          if (!strrchr(filename, ':')){
             char path[256];
             getcwd(path, 256);
             aiff->filename = malloc(strlen(path)+strlen(filename)+2);
             if (aiff->filename){
                strcat(strcat(strcpy(aiff->filename, path), "/"), filename);
             }
             else{
                printf("Error memory\n");
                audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
             }
          }
          else{
             aiff->filename = malloc(strlen(filename)+1);
             if (aiff->filename){
                strcpy(aiff->filename, filename);
             }
             else{
                printf("Error memory\n");
                audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
             }
          }
          sceKernelStartThread(aiff->extra->thid, 4, &aiff);
       }
       register_audio(aiff);
       return aiff;       
}

void enableSRCFunctions(){
     SRC = 1;
}
void disableSRCFunctions(){
     if (!SRCReserved)
        SRC = 0;
}
int registerEndAudio(EndAudio p){
    int i;
    for (i = 0; i < EndAudioMax; i++){
        if (EndAudioProcs[i] == p) return 1;
        if (EndAudioProcs[i] == NULL){
           EndAudioProcs[i] = p;
           return 1;
        }
    }
    return 0;
}
int power_callback_thread(){
    PowerCallbackID = sceKernelCreateCallback("Power Callback", power_callback, NULL);
    scePowerRegisterCallback(15, PowerCallbackID);
    sceKernelSleepThreadCB();
    sceKernelExitDeleteThread(0);
    return 0;
}

void unloadAudioCodec(int codec){
     switch (codec){
        case PSP_AV_MODULE_AVCODEC:
           if (AvcodecLoaded){
              AvcodecLoaded--;
              if (!AvcodecLoaded)
                 sceUtilityUnloadAvModule(PSP_AV_MODULE_AVCODEC);
           }
        break;
        case PSP_AV_MODULE_G729:
           if (G729Loaded){
              G729Loaded--;
              if (!G729Loaded)
                 sceUtilityUnloadAvModule(PSP_AV_MODULE_G729);
           }
        break;
        case PSP_AV_MODULE_AAC:
           if (AacLoaded){
              AacLoaded--;
              if (!AacLoaded)
                 sceUtilityUnloadAvModule(PSP_AV_MODULE_AAC);
           }
        break;
        case PSP_AV_MODULE_VAUDIO:
           if (VAudioLoaded){
              VAudioLoaded--;
              if (!VAudioLoaded)
                 sceUtilityUnloadAvModule(PSP_AV_MODULE_VAUDIO);
           }
        break;
        case PSP_AV_MODULE_MP3:
           if (Mp3Loaded){
              Mp3Loaded--;
              if (!Mp3Loaded)
                 sceUtilityUnloadAvModule(PSP_AV_MODULE_MP3);
           }
        break;
       case PSP_AV_MODULE_MPEGBASE:
           if (MpegbaseLoaded){
              MpegbaseLoaded--;
              unloadAudioCodec(PSP_AV_MODULE_AVCODEC);
              if (!MpegbaseLoaded)
                 sceUtilityUnloadAvModule(PSP_AV_MODULE_MPEGBASE);
           }
       break;
       case PSP_AV_MODULE_ATRAC3PLUS:
           if (Atrac3PlusLoaded){
              Atrac3PlusLoaded--;
              unloadAudioCodec(PSP_AV_MODULE_AVCODEC);
              if (!Atrac3PlusLoaded)
                 sceUtilityUnloadAvModule(PSP_AV_MODULE_ATRAC3PLUS);
           }
       break;
        case PSP_AV_MODULE_SASCORE:
           if (SascoreLoaded){
              SascoreLoaded--;
              if (!SascoreLoaded)
                 sceUtilityUnloadAvModule(PSP_AV_MODULE_SASCORE);
           }
        break;
       default:;
    }
}
int loadAudioCodec(int codec){
    int ret;
    switch (codec){
       case PSP_AV_MODULE_AVCODEC:
          if (!AvcodecLoaded){
             ret = sceUtilityLoadAvModule(PSP_AV_MODULE_AVCODEC);
             if (ret == 0x80110f02)// someone else loaded this
                return 1;
             else if (ret < 0){
                return 0;
             }
          }
          AvcodecLoaded++;
       break;
       case PSP_AV_MODULE_G729:
          if (!G729Loaded){
             ret = sceUtilityLoadAvModule(PSP_AV_MODULE_G729);
             if (ret == 0x80110f02)// someone else loaded this
                return 1;
             else if (ret < 0){
                return 0;
             }
          }
          G729Loaded++;
       break;
       case PSP_AV_MODULE_AAC:
          if (!AacLoaded){
             ret = sceUtilityLoadAvModule(PSP_AV_MODULE_AAC);
             if (ret == 0x80110f02)// someone else loaded this
                return 1;
             else if (ret < 0){
                return 0;
             }
          }
          AacLoaded++;
       break;
       case PSP_AV_MODULE_VAUDIO:
          if (!VAudioLoaded){
             ret = sceUtilityLoadAvModule(PSP_AV_MODULE_VAUDIO);
             if (ret == 0x80110f02)// someone else loaded this
                return 1;
             else if (ret < 0){
                return 0;
             }
          }
          VAudioLoaded++;
       break;
       case PSP_AV_MODULE_MP3:
          if (!Mp3Loaded){
             ret = sceUtilityLoadAvModule(PSP_AV_MODULE_MP3);
             if (ret == 0x80110f02)// someone else loaded this
                return 1;
             else if (ret < 0){
                return 0;
             }
          }
          Mp3Loaded++;
       break;
       case PSP_AV_MODULE_MPEGBASE:
          if (!MpegbaseLoaded){
             if (loadAudioCodec(PSP_AV_MODULE_AVCODEC)){
                ret = sceUtilityLoadAvModule(PSP_AV_MODULE_MPEGBASE);
                if (ret == 0x80110f02)// someone else loaded this
                   return 1;
                else if (ret < 0){
                   unloadAudioCodec(PSP_AV_MODULE_AVCODEC);
                   return 0;
                }
             }
          }
          MpegbaseLoaded++;
       break;
       case PSP_AV_MODULE_ATRAC3PLUS:
          if (!Atrac3PlusLoaded){
             if (loadAudioCodec(PSP_AV_MODULE_AVCODEC)){
                ret = sceUtilityLoadAvModule(PSP_AV_MODULE_ATRAC3PLUS);
                if (ret == 0x80110f02)// someone else loaded this
                   return 1;
                else if (ret < 0){
                   unloadAudioCodec(PSP_AV_MODULE_AVCODEC);
                   return 0;
                }
             }
          }
          Atrac3PlusLoaded++;
       break;
       case PSP_AV_MODULE_SASCORE:
          if (!SascoreLoaded){
             ret = sceUtilityLoadAvModule(PSP_AV_MODULE_SASCORE);
             if (ret == 0x80110f02)// someone else loaded this
                return 1;
             else if (ret < 0){
                return 0;
             }
          }
          SascoreLoaded++;
       break;
       default: return 0;
    }
    return 1;
}

void audioInit(u32 sounds){
     if (init) return;
     if (sounds){
        char threadName[8] = "sound\0\0";
        int i;
        if (sounds > 8)
           sounds = 8;
        for (i = 0; i < sounds; i++){
            threadName[5] = '0' + i;
            sndThreadIds[i] = sceKernelCreateThread(threadName, sound_thread, 0x11, 18*0x400, 0, 0);//16k for audio 8k for rest seem too much
            if (sndThreadIds[i] >= 0){
               sceKernelStartThread(sndThreadIds[i], 4, &i);
            }
        }
     }
     threads = sounds;
     PowerCallbackthid = sceKernelCreateThread("power_callback_thread", power_callback_thread, 0x11, 20*0x400, 0, 0); //0xFA0
     if (PowerCallbackthid >= 0){
        sceKernelStartThread(PowerCallbackthid, 0, 0);
     }

     sysFrequency = 44100;
     init = 1;
}

void audioEnd(){
     int i;
     sceKernelDeleteCallback(PowerCallbackID);
     sceKernelWakeupThread(PowerCallbackthid);
     while (audioList)
           unregister_audio(audioList->data.audio);
     for (i = 0; i < threads; i++)
         sceKernelTerminateDeleteThread(sndThreadIds[i]);
     i = 0;
     while (EndAudioProcs[i] != NULL){
           EndAudioProcs[i]();
           i++;
     }
     if (G729Loaded){
        sceUtilityUnloadAvModule(PSP_AV_MODULE_G729);
        G729Loaded = 0;
     }
     if (AacLoaded){
        sceUtilityUnloadAvModule(PSP_AV_MODULE_AAC);
        AacLoaded = 0;
     }
     if (VAudioLoaded){
        sceUtilityUnloadAvModule(PSP_AV_MODULE_VAUDIO);
        VAudioLoaded = 0;
     }
     if (Mp3Loaded){
        sceUtilityUnloadAvModule(PSP_AV_MODULE_MP3);
        Mp3Loaded = 0;
     }
     if (MpegbaseLoaded){
        sceUtilityUnloadAvModule(PSP_AV_MODULE_MPEGBASE);
        MpegbaseLoaded = 0;
     }
     if (Atrac3PlusLoaded){
        sceUtilityUnloadAvModule(PSP_AV_MODULE_ATRAC3PLUS);
        Atrac3PlusLoaded = 0;
     }
     if (SascoreLoaded){
        sceUtilityUnloadAvModule(PSP_AV_MODULE_SASCORE);
        SascoreLoaded = 0;
     }
     if (AvcodecLoaded){
        sceUtilityUnloadAvModule(PSP_AV_MODULE_AVCODEC);
        AvcodecLoaded = 0;
     }
     if (SRC) sceAudioSRCChRelease();
     for (i = 0; i < 8; i++){
         sceAudioChRelease(i);
     }
     init = 0;
}
void rescale(){
     NodePtr tmp = audioList;
     while (tmp){
           tmp->data.audio->scale = (float)tmp->data.audio->frequency/(float)sysFrequency;
           tmp = tmp->next;
     }
}
void setFrequency(int frequency){
     if (frequency != 44100 && frequency != 48000) return;
     sysFrequency = frequency;
     rescale();
}
#ifdef libaudio
PAudio loadAudio(const char *filename, int stream){//sigh
extern PAudio loadAt3(const char *, int);
extern PAudio loadAa3(const char *);
extern PAudio loadMp4(const char *, int);
extern PAudio loadWma(const char *);
#ifndef NO_FLAC
extern PAudio loadFlac(const char *, int);
#endif
#ifndef NO_OGG
extern PAudio loadOgg(const char *, int);
#endif
extern PAudio loadAac(const char *, int);
extern PAudio loadMp3(const char *, int);
#ifndef NO_MIDI
extern PAudio loadMusic(const char *);
#endif
       if (!filename) return NULL;
       PAudio audio;
       audio = loadWav(filename, stream);
       if (audio) return audio;
       audio = loadAu(filename, stream);
       if (audio) return audio;
       audio = loadAiff(filename, stream);
       if (audio) return audio;
       audio = loadAt3(filename, stream);
       if (audio) return audio;
       audio = loadAa3(filename);
       if (audio) return audio;
       audio = loadMp4(filename, stream);
       if (audio) return audio;
       audio = loadWma(filename);
       if (audio) return audio;
#ifndef NO_OGG
       audio = loadOgg(filename, stream);
       if (audio) return audio;
#endif
       audio = loadAac(filename, stream);
       if (audio) return audio;
#ifndef NO_FLAC
       audio = loadFlac(filename, stream);
       if (audio) return audio;
#endif
       audio = loadMp3(filename, stream);
       if (audio) return audio;
#ifndef NO_MIDI
       audio = loadMusic(filename);
       if (audio) return audio;
#endif
       return NULL;
}
#endif
void playAudio(PAudio audio){
     if (!audio) return;
#ifndef PLAYAUDIO_REPLAY
     if (audio->play >= PLAYING) return;
#endif
     if (audio->ufunc)
        audio->ufunc(audio, EVENT_playAudio_BEGIN);
#ifdef PLAYAUDIO_REPLAY
     if ((audio->play == PLAYING) && !audio->stream){
        audio->offset = 0;
        audio->frac = 0;
        return;
     }
#endif
     if (audio->stream){
#ifdef PLAYAUDIO_REPLAY
        if (audio->play == PLAYING){
           audiosetLastError(AUDIO_ERROR_ALREADY_PLAYING);
           return;
        }
#endif
        if (audio->extra->stream){
           eventData e = {EVENT_playAudio, 0, 0};
           if (audio->extra->stream(audio, &e)){
              return;
           }
        }
        if (SRC){
           if (SRCReserved) return;
           int ret = sceAudioSRCChReserve(audio->sampleCount, audio->frequency, audio->channels);
           if (ret < 0){
              printf("sceAudioSRCChReserve = 0x%08x\n", ret);//0x8026000a unsupported frequency?
              audiosetLastError(ret);
              return;
           }
           SRCReserved = 1;
        }
        else{
           audio->channel = sceAudioChReserve(PSP_AUDIO_NEXT_CHANNEL, PSP_AUDIO_SAMPLE_ALIGN(audio->sampleCount), (audio->channels == 2)?PSP_AUDIO_FORMAT_STEREO:PSP_AUDIO_FORMAT_MONO);
           if (audio->channel < 0){
              printf("audio->channel = 0x%08x\n", audio->channel);
              audiosetLastError(audio->channel);
              return;
           }
        }               
     }
     else{
        audio->channel = getChannel();
        if (audio->channel == -1){
           printf("No channel available\n");
           audiosetLastError(AUDIO_ERROR_NO_FREE_CHANNEL);
           return;
        }
#ifndef NO_RESAMPLING
        if ((audio->frequency == sysFrequency)&&(audio->channels == 2)&&(audio->bitRate == 16))
#endif      
           setAudioChannelCallback(audio->channel, (AudioCallback_t)audioCallback2, audio);
#ifndef NO_RESAMPLING
        else
           setAudioChannelCallback(audio->channel, (AudioCallback_t)audioCallback, audio);
#endif      
     }
     audio->play = PLAYING;
     if (audio->ufunc)
        audio->ufunc(audio, EVENT_playAudio_END);
}

#ifndef ONLY_STREAM
void playAsyncAudio(PAudio audio){
     if (!audio) return;
     if (audio->stream) return;
     if (audio->ufunc)
        audio->ufunc(audio, EVENT_playAsyncAudio);     
     int check = getChannel();
     if (check < 0)//no channel available
        return;
     releaseChannel(check);
     PAudio aAudio = malloc(sizeof(Audio));
     if (!aAudio) return;//audio error?
     *aAudio = *audio;
     aAudio->type |= 0x80000000;
     aAudio->offset = 0;
     aAudio->frac = 0;
     aAudio->play = 0;
     aAudio->loop = 0;
     playAudio(aAudio);
}
#endif

void pauseAudio(PAudio audio){
     if (!audio) return;
     if (audio->ufunc)
        audio->ufunc(audio, EVENT_pauseAudio);
     if (audio->stream){
        eventData e = {EVENT_pauseAudio, 0, 0};
        if (audio->extra->stream){
           if (audio->extra->stream(audio, &e)){
              if (audio->ufunc)
                 audio->ufunc(audio, EVENT_pauseAudio_END);
              return;
           }
        }
     }
     if (audio->play == PLAYING){
        audio->play = PAUSED;
        if (audio->stream){
           int ret;
           if (SRC){
              while (sceAudioOutput2GetRestSample())
                    sceKernelDelayThread(5);
               ret = sceAudioSRCChRelease();
               SRCReserved = 0;
           }
           else{
              while (sceAudioGetChannelRestLen(audio->channel) > 0)
                    sceKernelDelayThread(5);
              ret = sceAudioChRelease(audio->channel);
           }
           if (ret < 0) printf("pA ret = %x\n", ret);
        }
        else{
           while (sceAudioGetChannelRestLength(sndThreadChannels[audio->channel]) > 0)
                 sceKernelDelayThread(5);
           sceAudioChRelease(sndThreadChannels[audio->channel]);
           releaseChannel(audio->channel);
        }
     }
     if (audio->ufunc)
        audio->ufunc(audio, EVENT_pauseAudio_END);
}

void pauseAudio2(PAudio audio){
     if (!audio) return;
     if (audio->play == PLAYING)
        pauseAudio(audio);
     else 
        playAudio(audio);
}

void stopAudio(PAudio audio){
     if (!audio) return;
     if (audio->play == STOP) return;
     if (audio->ufunc)
        audio->ufunc(audio, EVENT_stopAudio_BEGIN);
     if (audio->stream){
        if (audio->extra->stream){
           eventData e = {EVENT_stopAudio, 0, 0};                       
           if (audio->extra->stream(audio, &e)){
              if (audio->ufunc)
                 audio->ufunc(audio, EVENT_stopAudio_END);
              return;
           }
        }
        audio->play = STOP;
        if (SRC){
           while (sceAudioOutput2GetRestSample() > 0)
                 sceKernelDelayThread(10);
           sceAudioSRCChRelease();
           SRCReserved = 0;
        }
		else{
           while (sceAudioGetChannelRestLength(audio->channel) > 0)
                 sceKernelDelayThread(10);
           int ret = sceAudioChRelease(audio->channel);
           if (ret < 0)printf("sA 0x%08x - %d\n", ret, audio->channel);
        }
     }
     else{
        audio->play = BUSY;
        while (sceAudioGetChannelRestLength(sndThreadChannels[audio->channel]) > 0)
              sceKernelDelayThread(5);
        int ret = sceAudioChRelease(sndThreadChannels[audio->channel]);
        releaseChannel(audio->channel);
        if (ret < 0)printf("sA 0x%08x- %d - %d\n", ret, audio->channel, sndThreadChannels[audio->channel]);
     }
     audio->play = STOP;
     audio->offset = 0;
     audio->frac = 0;
     if (audio->type & 0x80000000) free(audio);
     if (audio->ufunc)
        audio->ufunc(audio, EVENT_stopAudio_END);
}

void seekAudio0(PAudio audio, int seekTime){
     if (!audio) return;
     if (audio->ufunc)
        audio->ufunc(audio, EVENT_seekAudio_BEGIN);
     if (audio->stream){
        if (audio->extra->stream){
           eventData e = {EVENT_seekAudio, seekTime, SZ_SAMPLES};
           if (audio->extra->stream(audio, &e)){
              if (audio->ufunc)
                 audio->ufunc(audio, EVENT_seekAudio_END);
              return;
           }
        }
     }
     if (audio->play == PLAYING){
        audio->play = BUSY;
     }
     if ((seekTime < 0)||(seekTime >= audio->size)){
        printf("audio bad seektime\n");
        return;          
     }
     audio->offset = seekTime;
     audio->frac = 0;
     if (audio->play == BUSY){
        audio->play = PLAYING;
     }
     if (audio->ufunc)
        audio->ufunc(audio, EVENT_seekAudio_END);
}        

void seekAudio(PAudio audio, int seekTime, int seekType, int whence){//add SEEK_END
     if (!audio) return;
     int replay = 0;
     if (audio->play == PLAYING){
        audio->play = BUSY;
        replay = 1;
     }
     if (seekType == SZ_SECONDS){
        seekTime = (seekTime * (int)getFrequencyAudio(audio))/1000;//int * u32 = crap
        seekType = SZ_SAMPLES;
     }
     if (seekType == SZ_SAMPLES){
        if (whence != SEEK_SET)
           seekTime += audio->offset;
        seekAudio0(audio, seekTime);
     }
     if (replay) audio->play = PLAYING;
}        

void freeAudio(PAudio audio){
     if (!audio) return;
     if (audio->ufunc)
        audio->ufunc(audio, EVENT_freeAudio_BEGIN);
     if (audio->play) stopAudio(audio);
     if (audio->stream){
        if (audio->extra->stream){
           eventData e = {EVENT_freeAudio, 0, 0};
           if (audio->extra->stream(audio, &e)){
              if (audio->ufunc)
                 audio->ufunc(NULL, EVENT_freeAudio_END);
              return;
           }
           if (audio->filename != NULL)
              free(audio->filename);
        }
     }
     unregister_audio(audio);
     if ((audio->flags&AUDIO_FLAG_DONT_FREE) == 0)
        free(audio->data);
     free(audio);
     if (audio->ufunc)
        audio->ufunc(NULL, EVENT_freeAudio_END);
}

void loopAudio(PAudio audio){
     if (!audio) return;
     if (audio->ufunc)
        audio->ufunc(audio, EVENT_loopAudio_BEGIN);
     if (audio->stream){
        if (audio->extra->stream){
           eventData e = {EVENT_loopAudio, 0, 0};
           if (audio->extra->stream(audio, &e))
              return;
        }
     }
     audio->loop = 1;
     if (audio->ufunc)
        audio->ufunc(audio, EVENT_loopAudio_END);
}
void unloopAudio(PAudio audio){
     if (!audio) return;
     if (audio->ufunc)
        audio->ufunc(audio, EVENT_unloopAudio_BEGIN);
     if (audio->stream){
        if (audio->extra->stream){
           eventData e = {EVENT_unloopAudio, 0, 0};
           if (audio->extra->stream(audio, &e))
              return;
        }
     }
     audio->loop = 0;
     if (audio->ufunc)
        audio->ufunc(audio, EVENT_unloopAudio_END);
}

audioExtra *getExtraAudio(PAudio audio){
    if (!audio) return 0;
    return audio->extra;
}

u32 getPlaybackFrequencyAudio(PAudio audio){
    if (!audio) return 0;
    return audio->frequency;
}
u32 getFrequencyAudio(PAudio audio){
    if (!audio) return 0;
    if (audio->stream){
       if (audio->extra->stream){
           eventData e = {EVENT_getFrequencyAudio, 0, 0};
           int ret = audio->extra->stream(audio, &e);
           if (ret)
			  return ret;
        }
     }
     return audio->frequency;
}

void setFrequencyAudio(PAudio audio, u32 frequency){
     if (!audio) return;
     if (audio->ufunc)
        audio->ufunc(audio, EVENT_setFrequencyAudio_BEGIN);
     if (audio->stream){
        if (audio->extra->stream){
           eventData e = {EVENT_setFrequencyAudio, frequency, 0};
           if (audio->extra->stream(audio, &e))
			  return;
        }
     }
#ifndef NO_FREQUENCY_RESTRICTION
     if (frequency < 8000) return;
     if (frequency > 96000) return;
#endif
     audio->frequency = frequency;
     audio->scale = (float)frequency/(float)sysFrequency;
     if (audio->ufunc)
        audio->ufunc(audio, EVENT_setFrequencyAudio_END);
}

UserEventHandler getUserFunctionAudio(PAudio audio){//fix
       if (!audio) return 0;
       return audio->ufunc;
}
void setUserFunctionAudio(PAudio audio, UserEventHandler ufunc){
     if (!audio) return;
     audio->ufunc = ufunc;
}
#ifndef NO_STREAM
PAudio createAudio(u32 channels, u32 frequency, u32 bitrate, u32 size, u32 sztype){
       PAudio wav = malloc(sizeof(Audio));
       if (!wav){
          printf("Error memory audio\n");//no setting error codes?
          return NULL;
       }
       if (channels > 2) channels = 2; 
       if (frequency < 8000 || frequency > 96000) frequency = sysFrequency; 
       if (bitrate != 8 || bitrate != 16) bitrate = 16; 
       u32 frame = channels*(bitrate/8);
       if (sztype == SZ_BYTES)
          size = size/frame;
       else
          if (sztype != SZ_SAMPLES)
             printf("invalid sztype\n");
       void *data;
       int type;
       if (size == 0){
          data = NULL;
          type = 0;
       }
       else{
           data = malloc(size*frame);
           if (!data){
              printf("Error memory data\n");
              return NULL;
           }
           type = 1;
       }
       {
          Audio tmp = {type,
                       channels,
                       frequency,
                       bitrate,
                       data,
                       size,
                       (float)frequency/(float)sysFrequency,
                       0, 0, 0, 0, 0, 0, PSP_VOLUME_MAX, PSP_VOLUME_MAX, 0, 0, 0, 0, 0, 0, 0, 0};
          *wav = tmp; 
       }
       return wav;       
}

PAudio copyAudio(PAudio audio){
       PAudio wav = malloc(sizeof(Audio));
       if (!wav){
          audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
          printf("Error memory audio\n");
          return NULL;
       }
       memcpy(wav, audio, sizeof(Audio));
       if (audio->type == 1){
          int size = audio->size*audio->channels*(audio->bitRate/8);
          wav->data = malloc(size);
          memcpy(wav->data, audio->data, size);
          if (!wav->data){
             audiosetLastError(AUDIO_ERROR_MEM_ALLOC);
             printf("Error memory audio\n");
             wav->type = 0;
          }
          wav->flags &= ~AUDIO_FLAG_DONT_FREE;
       }
       return wav;       
}

int setDataAudio(PAudio wav, void *data, u32 size, u32 sztype){
    if (!wav){
       printf("Error memory audio\n");
       return -1;
    }
    if (wav->stream){
       printf("Error stream\n");
       return -1;
    }
    u32 frame = wav->channels*(wav->bitRate/8);
    if (sztype == SZ_BYTES){
       size = size/frame;
       sztype = SZ_SAMPLES;
    }
    else if (sztype == SZ_SECONDS){
       size = size*wav->frequency/1000;
       sztype = SZ_SAMPLES;
    }
    if (sztype == SZ_SAMPLES){
       if (!(wav->flags&AUDIO_FLAG_DONT_FREE))
          if (wav->data)
             free(wav->data);
       wav->type = 1;
       wav->data = data;
       wav->size = size;
       wav->flags |= AUDIO_FLAG_DONT_FREE;
       wav->offset = 0;
       wav->frac = 0;
       return 0;
    }
    return -1;
}

int reverseAudio(PAudio audio){
    if (!audio) return 0;
    if (audio->stream) return 0;//consider adding, if so move user function above it
    if (audio->ufunc)
       audio->ufunc(audio, EVENT_reverseAudio_BEGIN);
    if (audio->play == PLAYING) return 0;
    int i, loops = audio->size/2, size = audio->channels*(audio->bitRate/8);
    for (/*int no c99:(*/i = 0; i < loops; i++){//i wonder if this correct
        switch (size){
           case 1 :{
              u8* data8 = audio->data;
              u8 swap8 = data8[i];
              data8[i] = data8[audio->size-1-i];
              data8[audio->size-1-i] = swap8;
           }
           break;
           case 2 :{
              short* data16 = audio->data;
              short swap16 = data16[i];
              data16[i] = data16[audio->size-1-i];
              data16[audio->size-1-i] = swap16;
           }
           break;
           case 4 :{
              u32* data32 = audio->data;
              u32 swap32 = data32[i];
              data32[i] = data32[audio->size-1-i];
              data32[audio->size-1-i] = swap32;
           }
           break;
           default:
              printf("Error invalid data\n");
              return 0;
        }
    }
    if (audio->offset)
       audio->offset = audio->size - audio->offset;
    if (audio->frac != 0)
       audio->frac = 1.0 - audio->frac;
    if (audio->ufunc)
       audio->ufunc(audio, EVENT_reverseAudio_END);
    return 1;
}  
int swapChannelAudio(PAudio audio){
    if (!audio) return 0;
    if (audio->stream) return 0;
    if (audio->channels == 1) return 0;
    if (audio->ufunc)
       audio->ufunc(audio, EVENT_swapChannelAudio_BEGIN);
    int i;
    for (/*int no c99:(*/i = 0; i < audio->size; i++){
        if (audio->bitRate == 8){
           u8* data8 = audio->data;
           u8 swap8 = data8[i*2];
           data8[i*2] = data8[i*2+1];
           data8[i*2+1] = swap8;
        }
        else{
           short* data16 = audio->data;
           short swap16 = data16[i];
           data16[i*2] = data16[i*2+1];
           data16[i*2+1] = swap16;
        }
    }
    if (audio->ufunc)
       audio->ufunc(audio, EVENT_swapChannelAudio_END);
    return 1;
}   
int muteLChannelAudio(PAudio audio){
    if (!audio) return 0;
    if (audio->stream) return 0;    
    if (audio->channels == 1) return 0;
    if (audio->ufunc)
       audio->ufunc(audio, EVENT_muteLChannelAudio_BEGIN);
    int i;
    for (/*int no c99:(*/i = 0; i < audio->size; i++){
        if (audio->bitRate == 8){
           u8* data8 = audio->data;
           data8[i] = 0;
        }
        else{
           short* data16 = audio->data;
           data16[i] = 0;
        }
    }
    if (audio->ufunc)
       audio->ufunc(audio, EVENT_muteLChannelAudio_END);
    return 1;
}
   
int muteRChannelAudio(PAudio audio){
    if (!audio) return 0;
    if (audio->stream) return 0;
    if (audio->channels == 1) return 0;
    if (audio->ufunc)
       audio->ufunc(audio, EVENT_muteRChannelAudio_BEGIN);
    int i;
    for (/*int no c99:(*/i = 0; i < audio->size; i++){
        if (audio->bitRate == 8){
           u8* data8 = audio->data;
           data8[i+1] = 0;
        }
        else{
           short* data16 = audio->data;
           data16[i+1] = 0;
        }
    }
    if (audio->ufunc)
       audio->ufunc(audio, EVENT_muteRChannelAudio_END);
    return 1;
}  
 
int muteAudio(PAudio audio){
    if (!audio) return 0;
    if (audio->stream) return 0;
    if (audio->ufunc)
       audio->ufunc(audio, EVENT_muteAudio);
/*    int result muteLChannelAudio(audio);
    if (audio->channels == 1)
       return result; 
    if (result)
       result = muteRChannelAudio(audio);
    return result;
*/    
    memset(audio->data, 0, audio->size*audio->channels*(audio->bitRate/8));
    return 1;
}   
#endif

u32 getTimeAudio(PAudio audio, int type){
    if (!audio) return 0;
    if (type == SZ_SAMPLES)
       return audio->offset;
    if (type == SZ_SECONDS)
       return (audio->offset/audio->frequency)*1000;
    return 0;
}

u32 getLengthAudio(PAudio audio, int type){
    if (!audio) return 0;
    if (type == SZ_SAMPLES)
       return audio->size;
    if (type == SZ_SECONDS)
       return (audio->size/audio->frequency)*1000;
    return 0;
}

int increaseVolumeAudio(PAudio audio, int lVolume, int rVolume){
    if (!audio) return 0;
    return setVolumeAudio(audio, audio->lVolume+lVolume, audio->rVolume+rVolume);
}   

int decreaseVolumeAudio(PAudio audio, int lVolume, int rVolume){
    if (!audio) return 0;
    return setVolumeAudio(audio, audio->lVolume-lVolume, audio->rVolume-rVolume);
}

int setVolumeAudio(PAudio audio, int lVolume, int rVolume){
    if (!audio) return 0;
    if (audio->ufunc)
       audio->ufunc(audio, EVENT_setVolumeAudio_BEGIN);
    if (lVolume > PSP_VOLUME_MAX) 
       lVolume = PSP_VOLUME_MAX;
    else if (lVolume < 0) 
       lVolume = 0;
    if (rVolume > PSP_VOLUME_MAX) 
       rVolume = PSP_VOLUME_MAX;
    else if (rVolume < 0) 
       rVolume = 0;
    audio->lVolume = lVolume;
    audio->rVolume = rVolume;
    if (audio->ufunc)
       audio->ufunc(audio, EVENT_setVolumeMuteAudio_END);
    return 1;
}   

int setVolumeMuteAudio(PAudio audio){
    if (!audio) return 0;
    if (audio->ufunc)
       audio->ufunc(audio, EVENT_setVolumeMuteAudio_BEGIN);
    audio->lVolume = 0;
    audio->rVolume = 0;
    if (audio->ufunc)
       audio->ufunc(audio, EVENT_setVolumeMuteAudio_END);
    return 1;
}   

int setVolumeMaxAudio(PAudio audio){
    if (!audio) return 0;
    if (audio->ufunc)
       audio->ufunc(audio, EVENT_setVolumeMaxAudio);
    audio->lVolume = PSP_VOLUME_MAX;
    audio->rVolume = PSP_VOLUME_MAX;
    return 1;
}   

int getTypeAudio(PAudio audio){
    if (!audio) return -1;
    return audio->type;
}

int getChannelsAudio(PAudio audio){
    if (!audio) return -1;
    if (audio->stream){
       if (audio->extra->stream){
          eventData e = {EVENT_getChannelsAudio, 0, 0};
          int ret = audio->extra->stream(audio, &e);
          if (ret)
             return ret;
       }
    }
    return audio->channels;
}

u32 getBitsPerSampleAudio(PAudio audio){
    if (!audio) return 0;
    return audio->bitRate;
}

u32 getBitrateAudio(PAudio audio){
    if (!audio) return 0;
    if (audio->stream){
       if (audio->extra->stream){
          eventData e = {EVENT_getBitrateAudio, 0, 0};
          int ret = audio->extra->stream(audio, &e);
          if (ret)
             return ret;
       }
    }
    return audio->channels*audio->bitRate*audio->frequency;
}
s16 *getMixBufferAudio(PAudio audio, int *size){
    if (!audio) return NULL;
    *size = audio->sampleCount;
    return audio->mixBuffer;
}
int getPlayAudio(PAudio audio){
    if (!audio) return 0;
    return audio->play;
}
int isStoppedAudio(PAudio audio){
    if (!audio) return 0;
    return getPlayAudio(audio)==STOP;
}
int isPlayingAudio(PAudio audio){
    if (!audio) return 0;
    return getPlayAudio(audio)==PLAYING;
}
int isPausedAudio(PAudio audio){
    if (!audio) return 0;
    return getPlayAudio(audio)==PAUSED;
}

