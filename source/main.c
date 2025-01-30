#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <3ds.h>

#include "donut.h"

static int asciiWidth  = (int) GSP_SCREEN_HEIGHT_TOP/8; // why the fuck are these reversed :P
static int asciiHeight = (int) GSP_SCREEN_WIDTH/8;


int main(int argc, char** argv)
{
	// Donut variables
	float A = 0, B = 0;
	static char output[DONUT_WIDTH][DONUT_HEIGHT];
	int asciiDonutCenterX = (asciiWidth / 2) - (DONUT_HEIGHT / 2);

	// Initialize services
	gfxInitDefault();
	cfguInit();




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
	gfxExit();
	return 0;
}
