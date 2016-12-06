#ifndef MYFS_H
#define MYFS_H

#include <3ds.h>
#include <stdio.h>
#include <string.h>

Result fsinit();

FILE* fsopen(char* subPath, u32* outSize);

void fsread(FILE* fileHandle, u32 size, char* intoBuf);

#endif /*MYFS_H*/
