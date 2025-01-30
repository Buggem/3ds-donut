#include <stdint.h>

#define SCREEN_WIDTH 30
#define SCREEN_HEIGHT 30


#if __3DS__

#define DELAY 1000

#else

#define DELAY 60000

#endif

#define SPEED_A 0.00004
#define SPEED_B 0.00002

char (*render_frame(float,  float))[SCREEN_WIDTH];
