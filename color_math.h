#ifndef COLORMATH_INCLUDED
#define COLORMATH_INCLUDED

#include "pnm.h"

/* We use two different structs here for clarity and modularity */

/* Used for storing normalized [0, 1] RGB color representation */
typedef struct rgb_T {
    float red;
    float green;
    float blue;
} rgb_T;

/* Stores YPbPr representation */
typedef struct ypp_T {
    float y;
    float pb;
    float pr;
} ypp_T;

typedef struct DCTSpace_T {
    float a;
    float b;
    float c;
    float d;
} DCTSpace_T;

typedef struct PixSpace_T {
    float y1;
    float y2;
    float y3;
    float y4;
} PixSpace_T;

/* r, g, b must be normalized to range [0.0, 1.0] */
extern ypp_T rgb_to_ypp(rgb_T rgb);

extern rgb_T ypp_to_rgb(ypp_T ypp);

extern DCTSpace_T get_DCT_space(PixSpace_T pixspace);

extern PixSpace_T get_pix_space(DCTSpace_T dctspace);

extern float get_avg_four(float a, float b, float c, float d);

extern ypp_T ypp_from_pnm(Pnm_rgb pix, float denom);

extern DCTSpace_T new_dct_t(float a, float b, float c, float d);

extern PixSpace_T new_pix_t(float y1, float y2, float y3, float y4);


#endif
