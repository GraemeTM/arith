/*
 *     color_math.h
 *     by Henry Gray, Graeme McDonough, Oct. 2021
 *     hgray03, gmcdon03
 *     arith
 *
 *     Function declarations for simple color representation manipulations
 */

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

/* stores values from a discrete cosine transformation */
typedef struct DCTSpace_T {
    float a;
    float b;
    float c;
    float d;
} DCTSpace_T;

/* stores four pixel luminosity values */
typedef struct PixSpace_T {
    float y1;
    float y2;
    float y3;
    float y4;
} PixSpace_T;

/* 
 * Purpose: convert from rgb color format to component video format
 * Parameters: an rgb struct that represents a color in rgb form
 * Returns: a ypp struct that represents a color in component video form
 */ 
extern ypp_T rgb_to_ypp(rgb_T rgb);

/* 
 * Purpose: convert from component video format to rgb color format
 * Parameters: a ypp struct that represents a color in component video form
 * Returns: an rgb struct that represents a color in rgb form
 */ 
extern rgb_T ypp_to_rgb(ypp_T ypp);

/* 
 * Purpose: comput a discrete cosine transform on four pixels
 * Parameters: a pixspace struct that stores the luminosity of four pixels
 * Returns: a struct containing the four values returned by a DCT
 */ 
extern DCTSpace_T get_DCT_space(PixSpace_T pixspace);

/* 
 * Purpose: compute pixel luminosity for four pixels given DCT values
 * Parameters: a struct containing the four DCT values
 * Returns: a struct containing the luminosity of four pixels
 */ 
extern PixSpace_T get_pix_space(DCTSpace_T dctspace);

/* 
 * Purpose: gets the average of four values
 * Parameters: four floats
 * Returns: the average of the four values
 */ 
extern float get_avg_four(float a, float b, float c, float d);

/* 
 * Purpose: converts from Pnm_ppm rgb struct to color_math ypp struct
 * Parameters: a pixel stored as Pnm_rgb and the maximum value of the pixel
 * Returns: ypp color that represents the same color as the Pnm_rgb struct
 */ 
extern ypp_T ypp_from_pnm(Pnm_rgb pix, float denom);

/* 
 * Purpose: initializes a new DCT struct with the four values provided
 * Parameters: values given by a discrete cosine transformation
 * Returns: DCT struct
 */ 
extern DCTSpace_T new_dct_t(float a, float b, float c, float d);

/* 
 * Purpose: initializes a new PixSpace struct with the four values provided
 * Parameters: luminosity values of four pixels
 * Returns: PixSpace struct
 */ 
extern PixSpace_T new_pix_t(float y1, float y2, float y3, float y4);


#endif
