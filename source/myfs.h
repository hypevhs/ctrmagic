#ifndef MYFS_H
#define MYFS_H

#include <3ds.h>
#include <stdio.h>
#include <string.h>

FS_Archive sdmcArchive;

Result fsinit();

//returns size
void fsopen(Handle* outFileHandle, u32* outSize, char* subPath);

Result fsread(Handle fileHandle, u32 size, char* intoBuf);

#endif /*MYFS_H*/
