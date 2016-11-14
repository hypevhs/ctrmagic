#include "myfs.h"

Result fsinit() {
    FS_Path archivePath = fsMakePath(PATH_EMPTY, "");
    Result res = FSUSER_OpenArchive(&sdmcArchive, ARCHIVE_SDMC, archivePath);
    if (res) { printf("error opening SDMC archive\n"); }
    return res;
}

void fsopen(Handle* outFileHandle, u32* outSize, char* subPath) {
    Result loadRes;
    u64 fileSize;

    printf("loading %s\n", subPath);
    FS_Path fsPath = fsMakePath(PATH_ASCII, subPath);
    loadRes = FSUSER_OpenFile(outFileHandle, sdmcArchive, fsPath, FS_OPEN_READ, 0);
    if (loadRes) { printf("error opening file\n"); }
    loadRes = FSFILE_GetSize(*outFileHandle, &fileSize);
    if (loadRes) { printf("error getting file size\n"); }
    *outSize = (u32)fileSize;
    printf("done    %s\n", subPath);
}

Result fsread(Handle fileHandle, u32 size, char* intoBuf) {
    u32 bytesRead;
    Result res = FSFILE_Read(fileHandle, &bytesRead, 0, intoBuf, size);
    if (res) { printf("error reading from file\n"); }
    return res;
}
