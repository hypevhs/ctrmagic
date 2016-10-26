#include "music.h"

Result musicinit() {
	Result res;
	int xmpres;

	//init csnd
	res = csndInit();
	printf("csndInit = %li\n", res);

	//allocate buffer
	soundBuf = linearAlloc(sizeof(u32) * MUSIC_BUF_SIZE);

	//init xmp for playing .mod file
	musicCtx = xmp_create_context();

	//read module file
	Handle fsHandle;
	u32 fsSize;
	fsopen(&fsHandle, &fsSize, "/3ds/ctrmagic/fury.lagon.mod");
	moduleBuffer = linearAlloc(sizeof(char) * fsSize);
	fsread(fsHandle, fsSize, moduleBuffer);

	xmpres = xmp_load_module_from_memory(musicCtx, moduleBuffer, fsSize);
	printf("xmp load module = %i\n", xmpres);
	xmpres = xmp_start_player(musicCtx, MUSIC_SAMPLE_RATE, XMP_FORMAT_MONO);
	printf("xmp start player = %i\n", xmpres);

	return res;
}

Result musicTick() {
	Result res;
	int xmpres;

	CSND_ChnInfo chanInfo;
	res = csndGetState(MUSIC_CHANNEL, &chanInfo);
	if (chanInfo.active) {
		return 0; //wait until it's not active
	}
	printf("csnd state = %hhu ... %hi\n", chanInfo.active, chanInfo.adpcmSample);

	//play mod into buffer
	xmpres = xmp_play_buffer(musicCtx, soundBuf, MUSIC_BUF_SIZE, 0);
	printf("xmp play buffer = %i\n", xmpres);

	//play buffer
	res = csndPlaySound(MUSIC_CHANNEL, SOUND_ONE_SHOT | SOUND_FORMAT_16BIT, MUSIC_SAMPLE_RATE, 1.0, 0.0, (u32*)soundBuf, NULL, MUSIC_BUF_SIZE);
	printf("csndPlaySound = %li\n", res);

	return res;
}

void musicFree() {
	linearFree(soundBuf);
	linearFree(moduleBuffer);
    csndExit();
}