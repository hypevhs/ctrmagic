#include "myfs.h"

FILE* fsopen(char* subPath, u32* outSize) {
    FILE* handle;

    printf("loading %s\n", subPath);
    handle = fopen(subPath, "r");
    printf("...opened file: %p\n", handle);
    if (!handle) { printf("error opening file\n"); }
    fseek(handle, 0L, SEEK_END);
    *outSize = ftell(handle);
    printf("...got size: %lu\n", *outSize);
    rewind(handle);
    printf("done    %s\n", subPath);
    return handle;
}

void fsread(FILE* fileHandle, u32 size, char* intoBuf) {
    fread(intoBuf, size, sizeof(char), fileHandle);
}
