#ifndef COLORMATH_INCLUDED
#define COLORMATH_INCLUDED

typedef struct color_T {
  float val1; /* red or Y value */
  float val2; /* blue or Pb value */
  float val3; /* green or Pr value */
} *color_T;

/* r, g, b must be normalized to range [0.0, 1.0] */
extern color_T rgb_to_ypp(float r, float g, float b);

extern color_T ypp_to_rgb(float y, float pb, float pr);

extern
#endif
