#include <stdlib.h>
#include <stdio.h>
#include "color_math.h"

int main()
{
    rgb_T rgb = {.red = 1.0, .green = 0.0, .blue = 0.0};

    ypp_T ypp = rgb_to_ypp(rgb);
    printf("[%1.2f, %1.2f, %1.2f]\n", ypp.Y, ypp.Pb, ypp.Pr);

    rgb = ypp_to_rgb(ypp);
    printf("[%1.2f, %1.2f, %1.2f]\n", rgb.red, rgb.green, rgb.blue);

    PixSpace_T pix = {.y1 = 1.0, .y2 = 0.0, .y3 = 1.0, .y4 = 0.0};

    DCTSpace_T dct = get_DCT_space(pix);
    printf("[%1.2f, %1.2f, %1.2f, %1.2f]\n", 
            dct.a, dct.b, dct.c, dct.d);

    pix = get_pix_space(dct);
    printf("[%1.2f, %1.2f, %1.2f, %1.2f]\n", 
            pix.y1, pix.y2, pix.y3, pix.y4);
}
