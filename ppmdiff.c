/*
ppmdiff.c
Responsible for printing out the root mean square difference between 2 ppms
Authors: Graeme McDonough and Henry Gray
Comp40 Fall 2021
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "pnm.h"
#include "math.h"


/*
* Purpose: Holds information to pass to our closure argument in map functions
* Parameters: int sum for comparison, the other pixel map, and methods
*/
typedef struct closure {
    double *sum;
    A2Methods_UArray2 pixmap2;
    A2Methods_T methods;
    unsigned d1;
    unsigned d2;
} *closure;

/* Function prototype for opening file, defined later */
FILE *openFile(const char *filename, const char *mode);

void map_pixels_comp_sum(int i, int j, A2Methods_UArray2 array2,
                       A2Methods_Object *elem, void *cl);

/*
* Purpose: Prints out the correct usage when user input is invalid
* Parameters: program name as a c-string
* Returns: void, exits program with EXIT_FAILURE after running
*/
static void
usage(const char *progname)
{
    fprintf(stderr, "Usage: %s ppm1 ppm2\n",
            progname);
    exit(EXIT_FAILURE);
}



int main(int argc, char *argv[])
{
    assert(argc == 3);

    char *ppm1_name = argv[1];
    char *ppm2_name = argv[2];

    /* default to UArray2 methods */
    A2Methods_T methods = uarray2_methods_plain;
    assert(methods);

    /* default to best map */
    A2Methods_mapfun *map = methods->map_default;
    assert(map);

    /* If given image file, open it */
    FILE *fp1;
    FILE *fp2;
    if (ppm1_name != NULL && ppm2_name != NULL)
    {
        fp1 = openFile(ppm1_name, "r");
        fp2 = openFile(ppm2_name, "r");
    }
    else
    {
        usage(argv[0]);
    }

    Pnm_ppm ppm1 = Pnm_ppmread(fp1, methods);
    Pnm_ppm ppm2 = Pnm_ppmread(fp2, methods);
    printf("%d/%d v %d/%d", ppm1->width, ppm1->height, ppm2->width, ppm2->height);

    if (abs((int)ppm1->width - (int)ppm2->width) > 1 ||
        abs((int)ppm1->height - (int)ppm2->height) > 1)
    {
        fprintf(stderr, "Dimensions should differ by at most 1\n");
        fprintf(stdout, "%f\n", 1.0);
    }
    printf("HMMM\n");
    double final_sum = 0;
    closure cl = malloc(sizeof(*cl));
    cl->sum = &final_sum;
    cl->pixmap2 = ppm2->pixels;
    cl->methods = methods;
    cl->d1 = ppm1->denominator;
    cl->d2 = ppm2->denominator;
    printf("HMMM\n");

    int sm_width = ppm1->width < ppm2->width ? ppm1->width : ppm2->width;
    int sm_height = ppm1->height < ppm2->height ? ppm1->height : ppm2->height;
    printf("HMMM\n");


    methods->map_default(ppm1->pixels, map_pixels_comp_sum, cl);



    double e = sqrt(final_sum / ((double)sm_width * (double)sm_height * 3.0));
    //e /= ppm1->denominator;
    printf("denom: %u, %u\n", ppm1->denominator, ppm2->denominator);

    printf("%1.4f\n", e);

    free(cl);
    Pnm_ppmfree(&ppm1);
    Pnm_ppmfree(&ppm2);
}


void map_pixels_comp_sum(int i, int j, A2Methods_UArray2 array2,
                       A2Methods_Object *elem, void *cl)
{
    /* Get closure */

    closure c = (closure)cl;
    (void)array2;
    if(i < c->methods->width(array2) && i < c->methods->width(c->pixmap2) &&
       j < c->methods->height(array2) && j < c->methods->height(c->pixmap2))
    {
        unsigned d1 = c->d1;
        unsigned d2 = c->d2;

        Pnm_rgb pix1 = ((Pnm_rgb)elem);
        Pnm_rgb pix2 = ((Pnm_rgb)c->methods->at(c->pixmap2, i, j));

        double rdiff = pow(pix1->red/(double)d1 - pix2->red/(double)d2, 2);
        double bdiff = pow(pix1->blue/(double)d1 - pix2->blue/(double)d2, 2);
        double gdiff = pow(pix1->green/(double)d1 - pix2->green/(double)d2, 2);

        double to_add = (rdiff + bdiff + gdiff);
        *(c->sum) += to_add;
    }
}

/*
* Purpose: Opens a file, checks for errors, and then returns the file pointer
* Parameters: c-string filename and c-string mode for reading (i.e. rb, r etc.)
* Returns: File pointer, if open and valid.
*/
FILE *openFile(const char *filename, const char *mode)
{
    FILE *fp = fopen(filename, mode);
    if (fp == NULL)
    {
        fprintf(stderr, "Could not open file %s with mode %s\n", filename, mode);
        exit(EXIT_FAILURE);
    }
    return fp;
}
