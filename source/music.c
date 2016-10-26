#include "music.h"

Result musicinit() {
	Result res;
	int xmpres;

	//init dsp
	res = ndspInit();
	printf("ndspInit = %li\n", res);

	//allocate buffer
	audioBuffer = (u32*)linearAlloc(MUSIC_BUF_LENGTH_BYTES * 2);

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

	//start dsp
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	ndspChnSetInterp(MUSIC_CHANNEL, NDSP_INTERP_LINEAR);
	ndspChnSetRate(MUSIC_CHANNEL, MUSIC_SAMPLE_RATE);
	ndspChnSetFormat(MUSIC_CHANNEL, NDSP_FORMAT_STEREO_PCM16);
	float mix[12];
	memset(mix, 0, sizeof(mix));
	mix[0] = 1.0;
	mix[1] = 1.0;
	ndspChnSetMix(MUSIC_CHANNEL, mix);

	printf("Started dsp\n");

	memset(waveBuf,0,sizeof(waveBuf));
	waveBuf[0].data_vaddr = &audioBuffer[0];
	waveBuf[0].nsamples = MUSIC_BUF_LENGTH_SAMPLES;
	waveBuf[1].data_vaddr = &audioBuffer[MUSIC_BUF_LENGTH_SAMPLES];
	waveBuf[1].nsamples = MUSIC_BUF_LENGTH_SAMPLES;

	printf("made wavebufs\n");

	return res;
}

void loadNewSamplesIntoSoundBuf(void *audioBufferL, void *audioBufferR) {
	int xmpres;

	s16 tempBuf[MUSIC_BUF_LENGTH_SAMPLES]; //holds the 16-bit samples, mono

	//xmp into soundBuf
	xmpres = xmp_play_buffer(musicCtx, tempBuf, MUSIC_BUF_LENGTH_BYTES, 0);
	printf("xmp play buffer = %i\n", xmpres);

	memcpy(audioBufferL, tempBuf, MUSIC_BUF_LENGTH_BYTES);
	memcpy(audioBufferR, tempBuf, MUSIC_BUF_LENGTH_BYTES);

	DSP_FlushDataCache(audioBufferL, MUSIC_BUF_LENGTH_BYTES);
	DSP_FlushDataCache(audioBufferR, MUSIC_BUF_LENGTH_BYTES);
}

bool started = false;

void musicTick() {
	if (started == false || (waveBuf[0].status == NDSP_WBUF_DONE && waveBuf[0].status == NDSP_WBUF_DONE)) {
		started = true;
		loadNewSamplesIntoSoundBuf(waveBuf[0].data_pcm16, waveBuf[1].data_pcm16);

		ndspChnWaveBufAdd(MUSIC_CHANNEL, &waveBuf[0]);
		ndspChnWaveBufAdd(MUSIC_CHANNEL, &waveBuf[0]);
	}
}

void musicFree() {
	linearFree(audioBuffer);
	linearFree(moduleBuffer);
    ndspExit();
    dspExit();
}