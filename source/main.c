#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <3ds.h>

#include "donut.h"


int main(int argc, char** argv)
{
	// Donut variables
	float A = 0, B = 0;
	static char output[SCREEN_WIDTH][SCREEN_HEIGHT];

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
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		memcpy(output, render_frame(A, B), sizeof(output));
		for (int j = 0; j < SCREEN_HEIGHT; j++) {
			for (int i = 0; i < SCREEN_WIDTH; i++) {
				putchar(output[i][j]);
				if (kDown & KEY_R) {
					A += SPEED_A;
					B += SPEED_B;
				}
				if (kDown & KEY_L) {
					A -= SPEED_A * 2;
					B -= SPEED_B * 2;
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
