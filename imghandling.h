/*
 *     imghandling.h
 *     by Henry Gray, Graeme McDonough, Oct. 2021
 *     hgray03, gmcdon03
 *     arith
 *
 *     Function declarations for full-image manipulations or interpretation
 */

#include <stdlib.h>
#include <stdio.h>
#include "pnm.h"

/* 
 * Purpose: read and store a ppm from a file
 * Parameters: File pointer to a ppm, list of functions used to operate on ppm
 * Returns: Pnm_ppm containing the data from the provided ppm
 */ 
extern Pnm_ppm get_ppm(FILE *fp, A2Methods_T methods);

/* 
 * Purpose: trims the last row and/or column of a ppm in order to have even 
 *          dimensions
 * Parameters: ppm to trim
 * Returns: nothing
 */ 
extern void trim_edges(Pnm_ppm ppm);

/* 
 * Purpose: reads the first two lines of a compressed file and gets the width 
 *          and height from it
 * Parameters: File pointer to the compressed file, pointers to the width and 
 *             height
 * Returns: nothing
 */ 
extern void read_header(FILE *fp, unsigned *width, unsigned *height);

/* 
 * Purpose: creates and initializes a new Pnm_ppm with the provided values and a
 *          blank pixmap
 * Parameters: widhth, height, denominator, and methods value of the new ppm
 * Returns: Pnm_ppm structW
 */ 
extern Pnm_ppm Pnm_ppm_new(unsigned width, unsigned height,
                           unsigned denominator, A2Methods_T methods);
