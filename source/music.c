#include "music.h"

Result musicinit() {
	//init csnd
	Result res = csndInit();
	printf("csndInit = %li\n", res);

	//generate buffer
	u32* soundBuf = linearAlloc(sizeof(u32) * SOUND_BUF_SIZE);
	for (int i = 0; i < SOUND_BUF_SIZE; i++) {
		soundBuf[i] = rand();
	}

	//play buffer
	res = csndPlaySound(0x8, SOUND_REPEAT | SOUND_FORMAT_16BIT, 8000, 1.0, 0.0, (u32*)soundBuf, (u32*)soundBuf, SOUND_BUF_SIZE);
	printf("csndPlaySound = %li\n", res);

	return res;
}

void musicFree() {
	linearFree(soundBuf);
    csndExit();
}