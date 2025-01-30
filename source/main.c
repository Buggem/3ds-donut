#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <malloc.h>
#include <string.h>

// libcitra stuff
#include <3ds.h>

// modplug
#include <libmodplug/modplug.h>


// constants and renderer
#include "donut.h"

static int asciiWidth  = (int) GSP_SCREEN_HEIGHT_TOP/8; // why the fuck are these reversed :P
static int asciiHeight = (int) GSP_SCREEN_WIDTH/8;

// BEGIN AUDIO BULLSHIT

typedef struct
{
	ModPlugFile *plug;
	ModPlug_Settings settings;
}  ModplugDecoder;

// roughly a video frame's worth of audio
static const size_t decoderBufSize = 800 * 2 * 2;
static ModplugDecoder decoder;
static ndspWaveBuf wavebufs[2];
static int nextBuf = 0;

void audioCallback(void *const data) {
	if(wavebufs[nextBuf].status == NDSP_WBUF_DONE) {
		size_t decoded = ModPlug_Read(decoder.plug, wavebufs[nextBuf].data_pcm16, decoderBufSize);
		if (decoded!=0) {
			wavebufs[nextBuf].nsamples = ((decoded / 2) / sizeof(int16_t));
			DSP_FlushDataCache(wavebufs[nextBuf].data_pcm16, decoded);

			ndspChnWaveBufAdd(0, &wavebufs[nextBuf]);

			nextBuf ^= 1;
		}
	}
}

// END AUDIO BULLSHIT

int main(int argc, char** argv)
{
	// Donut variables
	float A = 0, B = 0;
	static char output[DONUT_WIDTH][DONUT_HEIGHT];
	int asciiDonutCenterX = (asciiWidth / 2) - (DONUT_HEIGHT / 2);


	// Initialize services
	gfxInitDefault();
	cfguInit();

// BEGIN AUDIO BULLSHIT

	ndspInit();
	romfsInit();

	decoder.settings.mFlags = MODPLUG_ENABLE_OVERSAMPLING | MODPLUG_ENABLE_NOISE_REDUCTION;
	decoder.settings.mChannels = 2;
	decoder.settings.mBits = 16;
	decoder.settings.mFrequency = 44100;
	decoder.settings.mResamplingMode = MODPLUG_RESAMPLE_LINEAR;

	/* Fill with modplug defaults */
	decoder.settings.mStereoSeparation = 128;
	decoder.settings.mMaxMixChannels = 32;
	decoder.settings.mReverbDepth = 0;
	decoder.settings.mReverbDelay = 0;
	decoder.settings.mBassAmount = 0;
	decoder.settings.mBassRange = 0;
	decoder.settings.mSurroundDepth = 0;
	decoder.settings.mSurroundDelay = 0;
	decoder.settings.mLoopCount = -1;

	ModPlug_SetSettings(&decoder.settings);

	struct stat fileStat;
	stat(MUSIC_PATH, &fileStat);
	size_t bufferSize = fileStat.st_size;

	FILE* file = fopen(MUSIC_PATH, "rb");

	void* buffer = (void*)malloc(bufferSize);
	fread(buffer, bufferSize, 1, file);

	decoder.plug = ModPlug_Load(buffer, bufferSize);

	free(buffer);
	fclose(file);

	if (decoder.plug == 0) {
		printf("Couldn't load mod file!\n");
	} else {
		ModPlug_SetMasterVolume(decoder.plug, 128);

		ndspChnReset(0);
		ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);
		ndspChnSetRate(0, 44100);
		ndspChnSetInterp(0, NDSP_INTERP_POLYPHASE);

		// Set up audiobuffers using linearAlloc
		// This ensures audio data is in contiguous physical ram
		wavebufs[0].data_pcm16 = (int16_t*)linearAlloc(decoderBufSize);
		wavebufs[0].looping = false;
		wavebufs[0].status = NDSP_WBUF_DONE;

		wavebufs[1].data_pcm16 = (int16_t*)linearAlloc(decoderBufSize);
		wavebufs[1].looping = false;
		wavebufs[1].status = NDSP_WBUF_DONE;

		printf("Playing modplug file!\n");

		// Fill the two audio buffers
		audioCallback(NULL);
		audioCallback(NULL);

		// and chain the rest of the audio using the callback
		ndspSetCallback(audioCallback, NULL);
	}
// END AUDIO BULLSHIT



	// Init console for text output
	consoleInit(GFX_TOP, NULL);

	printf("\033[2J");
	printf("\x1b[H");

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();

		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		memcpy(output, render_frame(A, B), sizeof(output));

		consoleClear();
		for (int j = 0; j < DONUT_HEIGHT; j++) {
			printf("\033[%dC", asciiDonutCenterX);
			for (int i = 0; i < DONUT_WIDTH; i++) {
				putchar(output[i][j]);
				if (kHeld & KEY_R) {
					A += SPEED_A * FF_MULTI;
					B += SPEED_B * FF_MULTI;
				}
				if (kHeld & KEY_L) {
					A -= SPEED_A * (1 + FF_MULTI);
					B -= SPEED_B * (1 + FF_MULTI);
				}
				A += SPEED_A;
				B += SPEED_B;
			}
			putchar('\n');
		}
		usleep(DELAY);
		printf("\x1b[23A");

		// Flush and swap framebuffers
		gfxFlushBuffers();
		gfxSwapBuffers();
		gspWaitForVBlank();

		usleep(DELAY);
	}

	// Exit services
	cfguExit();

	if (decoder.plug != 0) {
		ModPlug_Unload(decoder.plug);
		linearFree(wavebufs[0].data_pcm16);
		linearFree(wavebufs[1].data_pcm16);
	}

	romfsInit();
	ndspExit();

	gfxExit();
	return 0;
}
