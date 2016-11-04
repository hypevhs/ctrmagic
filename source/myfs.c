#include "myfs.h"

Result fsinit() {
    FS_Path archivePath = fsMakePath(PATH_EMPTY, "");
    Result res = FSUSER_OpenArchive(&sdmcArchive, ARCHIVE_SDMC, archivePath);
    printf("archive open result is = %li\n", res);
    return res;
}

void fsopen(Handle* outFileHandle, u32* outSize, char* subPath) {
    Result loadRes;
    u64 fileSize;

    FS_Path fsPath = fsMakePath(PATH_ASCII, subPath);
    printf("made fspath\n");
    loadRes = FSUSER_OpenFile(outFileHandle, sdmcArchive, fsPath, FS_OPEN_READ, 0);
    printf("file open result is = %li\n", loadRes);
    loadRes = FSFILE_GetSize(*outFileHandle, &fileSize);
    printf("file size result is = %li\n", loadRes);
    *outSize = (u32)fileSize;
}

Result fsread(Handle fileHandle, u32 size, char* intoBuf) {
    u32 bytesRead;
    Result res = FSFILE_Read(fileHandle, &bytesRead, 0, intoBuf, size);
    printf("read result is = %li\n", res);
    return res;
}