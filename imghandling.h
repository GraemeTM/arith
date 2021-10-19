#include <stdlib.h>
#include <stdio.h>
#include "pnm.h"

extern Pnm_ppm get_ppm(FILE *fp, A2Methods_T methods);

extern void trim_edges(Pnm_ppm ppm);
