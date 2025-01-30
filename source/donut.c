#include "donut.h"

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <unistd.h>

const float theta_spacing = 0.07;
const float phi_spacing   = 0.02;

const float R1 = 1;
const float R2 = 2;
const float K2 = 5;
// Calculate K1 based on screen size: the maximum x-distance occurs
// roughly at the edge of the torus, which is at x=R1+R2, z=0.  we
// want that to be displaced 3/8ths of the width of the screen, which
// is 3/4th of the way from the center to the side of the screen.
// DONUT_WIDTH*3/8 = K1*(R1+R2)/(K2+0)
// DONUT_WIDTH*K2*3/(8*(R1+R2)) = K1
const float K1 = DONUT_WIDTH*K2*3/(8*(R1+R2));

char (*render_frame(float A, float B))[DONUT_WIDTH] {
  // precompute sines and cosines of A and B
  float cosA = cos(A), sinA = sin(A);
  float cosB = cos(B), sinB = sin(B);

  static char output[DONUT_WIDTH][DONUT_HEIGHT];
  float zbuffer[DONUT_WIDTH][DONUT_HEIGHT];

  memset(output, 32, sizeof(output));
  memset(zbuffer, 0, sizeof(zbuffer));

  // theta goes around the cross-sectional circle of a torus
  for (float theta=0; theta < 2*M_PI; theta += theta_spacing) {
	// precompute sines and cosines of theta
	float costheta = cos(theta), sintheta = sin(theta);

	// phi goes around the center of revolution of a torus
	for(float phi=0; phi < 2*M_PI; phi += phi_spacing) {
	  // precompute sines and cosines of phi
	  float cosphi = cos(phi), sinphi = sin(phi);

	  // the x,y coordinate of the circle, before revolving (factored
	  // out of the above equations)
	  float circlex = R2 + R1*costheta;
	  float circley = R1*sintheta;

	  // final 3D (x,y,z) coordinate after rotations, directly from
	  // our math above
	  float x = circlex*(cosB*cosphi + sinA*sinB*sinphi)
		- circley*cosA*sinB;
#if __3DS__
	  float y = circlex*(sinB*cosphi - sinA*cosB*sinphi)
		+ circley*cosA*cosB;
#else
		  float y = (circlex*(sinB*cosphi - sinA*cosB*sinphi)
				+ circley*cosA*cosB)/2;
#endif
	  float z = K2 + cosA*circlex*sinphi + circley*sinA;
	  float ooz = 1/z;  // "one over z"

	  // x and y projection.  note that y is negated here, because y
	  // goes up in 3D space but down on 2D displays.
	  int xp = (int) (DONUT_WIDTH/2 + K1*ooz*x);
	  int yp = (int) (DONUT_HEIGHT/2 - K1*ooz*y);

	  // calculate luminance.  ugly, but correct.
	  float L = cosphi*costheta*sinB - cosA*costheta*sinphi -
		sinA*sintheta + cosB*(cosA*sintheta - costheta*sinA*sinphi);
	  // L ranges from -sqrt(2) to +sqrt(2).  If it's < 0, the surface
	  // is pointing away from us, so we won't bother trying to plot it.
	  if (L > 0) {
		// test against the z-buffer.  larger 1/z means the pixel is
		// closer to the viewer than what's already plotted.
		if(ooz > zbuffer[xp][yp]) {
		  zbuffer[xp][yp] = ooz;
		  int luminance_index = L*8;
		  // luminance_index is now in the range 0..11 (8*sqrt(2) = 11.3)
		  // now we lookup the character corresponding to the
		  // luminance and plot it in our output:
		  output[xp][yp] = ".,-~:;=!*#$@"[luminance_index];
		}
	  }
	}
  }

  // now, dump output[] as the result.
  // bring cursor to "home" location, in just about any currently-used
  // terminal emulation mode
  return output;
}
