#include <stdint.h>

#define SCREEN_WIDTH 30
#define SCREEN_HEIGHT 30


#if __3DS__

#define DELAY 6000

#else

#define DELAY 60000

#endif

char (*render_frame(float,  float))[SCREEN_WIDTH];
