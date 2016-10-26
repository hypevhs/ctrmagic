#include "music.h"

Result musicinit() {
	//init csnd
	Result res = csndInit();
	printf("csndInit = %li\n", res);

	//allocate buffer
	u32* soundBuf = linearAlloc(sizeof(u32) * SOUND_BUF_SIZE);

	//init xmp for playing .mod file
	musicCtx = xmp_create_context();

	//read module file
	Handle fsHandle;
	u32 fsSize;
	fsopen(&fsHandle, &fsSize, "/3ds/ctrmagic/fury.lagon.mod");
	moduleBuffer = linearAlloc(sizeof(char) * fsSize);
	fsread(fsHandle, fsSize, moduleBuffer);

	int xmpres = xmp_load_module_from_memory(musicCtx, moduleBuffer, fsSize);
	printf("xmp load module = %i\n", xmpres);
	xmpres = xmp_start_player(musicCtx, SOUND_SAMPLE_RATE, XMP_FORMAT_MONO);
	printf("xmp start player = %i\n", xmpres);
	xmp_play_buffer(musicCtx, soundBuf, SOUND_BUF_SIZE, 0);
	printf("xmp play buffer = %i\n", xmpres);

	//play buffer
	res = csndPlaySound(0x8, SOUND_ONE_SHOT | SOUND_FORMAT_16BIT, SOUND_SAMPLE_RATE, 1.0, 0.0, (u32*)soundBuf, NULL, SOUND_BUF_SIZE);
	printf("csndPlaySound = %li\n", res);

	return res;
}

void musicFree() {
	linearFree(soundBuf);
	linearFree(moduleBuffer);
    csndExit();
}