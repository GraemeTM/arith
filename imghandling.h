#include <stdlib.h>
#include <stdio.h>
#include "pnm.h"

extern Pnm_ppm get_ppm(FILE *fp, A2Methods_T methods);

extern void trim_edges(Pnm_ppm ppm);

extern void read_header(FILE *fp, unsigned *width, unsigned *height);

extern Pnm_ppm Pnm_ppm_new(unsigned width, unsigned height,
                           unsigned denominator, A2Methods_T methods);
