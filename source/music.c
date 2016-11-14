#include "music.h"

Result musicinit() {
    Result res;
    int xmpres;

    //init dsp
    res = ndspInit();
    if (res) { printf("couldn't ndspInit = %li\n", res); }

    //allocate buffer
    audioBuffer = (u32*)linearAlloc(MUSIC_BUF_LENGTH_BYTES * 2); //double buffering

    //init xmp for playing .mod file
    musicCtx = xmp_create_context();

    //read module file
    Handle fsHandle;
    u32 fsSize;
    fsopen(&fsHandle, &fsSize, "/3ds/ctrmagic/fury.lagon.mod");
    moduleBuffer = linearAlloc(sizeof(char) * fsSize);
    fsread(fsHandle, fsSize, moduleBuffer);

    xmpres = xmp_load_module_from_memory(musicCtx, moduleBuffer, fsSize);
    if (xmpres) { printf("couldn't xmp_load_module = %i\n", xmpres); }
    xmpres = xmp_start_player(musicCtx, MUSIC_SAMPLE_RATE, 0);
    if (xmpres) { printf("couldn't xmp_start_player = %i\n", xmpres); }

    //init dsp
    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
    ndspChnSetInterp(MUSIC_CHANNEL, NDSP_INTERP_LINEAR);
    ndspChnSetRate(MUSIC_CHANNEL, MUSIC_SAMPLE_RATE);
    ndspChnSetFormat(MUSIC_CHANNEL, NDSP_FORMAT_STEREO_PCM16);
    float mix[12];
    memset(mix, 0, sizeof(mix));
    mix[0] = 1.0;
    mix[1] = 1.0;
    ndspChnSetMix(MUSIC_CHANNEL, mix);

    memset(waveBuf, 0, sizeof(waveBuf));
    waveBuf[0].data_vaddr = &audioBuffer[0];
    waveBuf[0].nsamples = MUSIC_BUF_LENGTH_SAMPLES;
    waveBuf[1].data_vaddr = &audioBuffer[MUSIC_BUF_LENGTH_SAMPLES];
    waveBuf[1].nsamples = MUSIC_BUF_LENGTH_SAMPLES;

    printf("initted dsp, made wavebuffers\n");

    return res;
}

s16 maxAmp = 0;

void loadNewSamplesIntoSoundBuf(u32 *audioBuffer) {
    //xmp into soundBuf
    xmp_play_buffer(musicCtx, audioBuffer, MUSIC_BUF_LENGTH_BYTES, 0);

    for (int i = 0; i < MUSIC_BUF_LENGTH_SAMPLES; i++) {
        u32* pcmRaw = &audioBuffer[i];
        s16* pcmLR = (s16*)pcmRaw;

        //update max amplitude
        maxAmp = (s16)fmaxf(abs(pcmLR[0]), maxAmp);
        maxAmp = (s16)fmaxf(abs(pcmLR[1]), maxAmp);

        //scale volume by max amplitude
        float volGain = SHRT_MAX / (maxAmp + 1);
        pcmLR[0] *= volGain; //left speaker is LSB
        pcmLR[1] *= volGain; //right speaker is MSB
    }

    DSP_FlushDataCache(audioBuffer, MUSIC_BUF_LENGTH_BYTES);
}

int started = 0;
bool audioSwap = false; //if true, we're doing work on the second buffer

void musicTick() {
    if (started < 2 || (waveBuf[audioSwap].status == NDSP_WBUF_DONE)) {
        started++;
        loadNewSamplesIntoSoundBuf((u32*)waveBuf[audioSwap].data_pcm16);

        ndspChnWaveBufAdd(MUSIC_CHANNEL, &waveBuf[audioSwap]);

        audioSwap = !audioSwap; //now we wait on the other buffer
    }
}

void musicFree() {
    linearFree(audioBuffer);
    linearFree(moduleBuffer);
    ndspExit();
    dspExit();
}