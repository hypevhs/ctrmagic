#ifndef MUSIC_H
#define MUSIC_H

//http://smealum.github.io/ctrulib/audio_2mic_2source_2main_8c-example.html

#include <3ds.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

u32* soundBuf;
#define SOUND_BUF_SIZE 1000

Result musicinit();

void musicFree();

#endif /*MUSIC_H*/