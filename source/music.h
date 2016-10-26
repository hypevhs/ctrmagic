#ifndef MUSIC_H
#define MUSIC_H

//http://smealum.github.io/ctrulib/audio_2mic_2source_2main_8c-example.html

#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libxmp-lite/xmp.h>
#include "myfs.h"

u32* soundBuf;
char* moduleBuffer;
xmp_context musicCtx;
#define SOUND_SAMPLE_RATE 44100
#define SOUND_BUF_SIZE (SOUND_SAMPLE_RATE*1*2) //1 second * 2 for stereo

Result musicinit();

void musicFree();

#endif /*MUSIC_H*/