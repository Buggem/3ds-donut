#include <stdint.h>

#define DONUT_WIDTH 30
#define DONUT_HEIGHT 30


#if __3DS__

#define DELAY 0
#define SPEED_A 0.00004
#define SPEED_B 0.00002


#else

#define DELAY 60000
#define SPEED_A 0.00004
#define SPEED_B 0.00002

#endif

#define FF_MULTI 4

char (*render_frame(float,  float))[DONUT_WIDTH];
