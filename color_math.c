#include <stdlib.h>
#include "assert.h"
#include "color_math.h"
#include "math.h"
#include "pnm.h"

ypp_T rgb_to_ypp(rgb_T rgb)
{
    float r = rgb.red, b = rgb.blue, g = rgb.green;
    assert(r >= 0 && r <= 1 && g >= 0 && g <= 1 && b >= 0 && b <= 1);

    ypp_T ypp;
    ypp.y = 0.299 * r + 0.587 * g + 0.114 * b;
    ypp.pb = -0.168736 * r - 0.331264 * g + 0.5 * b;
    ypp.pr = 0.5 * r - 0.418688 * g - 0.081312 * b;
    return ypp;
}

rgb_T ypp_to_rgb(ypp_T ypp)
{
    float y = ypp.y, pb = ypp.pb, pr = ypp.pr;

    rgb_T rgb;
    /* Use fabs cast to make sure we don't have any negative 0s */
    rgb.red = fabs(1.0 * y + 0.0 * pb + 1.402 * pr);
    rgb.green = fabs(1.0 * y - 0.344136 * pb - 0.714136 * pr);
    rgb.blue = fabs(1.0 * y + 1.772 * pb + 0.0 * pr);
    return rgb;
}

DCTSpace_T get_DCT_space(PixSpace_T pixspace)
{
    float y1 = pixspace.y1, y2 = pixspace.y2;
    float y3 = pixspace.y3, y4 = pixspace.y4;

    DCTSpace_T dct;
    dct.a = (y4 + y3 + y2 + y1) / 4.0;
    dct.b = (y4 + y3 - y2 - y1) / 4.0;
    dct.c = (y4 - y3 + y2 - y1) / 4.0;
    dct.d = (y4 - y3 - y2 + y1) / 4.0;
    return dct;
}

PixSpace_T get_pix_space(DCTSpace_T dctspace)
{
    float a = dctspace.a, b = dctspace.b, c = dctspace.c, d = dctspace.d;

    PixSpace_T pix;
    pix.y1 = a - b - c + d;
    pix.y2 = a - b + c - d;
    pix.y3 = a + b - c - d;
    pix.y4 = a + b + c + d;

    return pix;
}

float get_avg_four(float a, float b, float c, float d)
{
    float avg = (a + b + c + d) / 4.0;
    return avg;
}

ypp_T ypp_from_pnm(Pnm_rgb pix, float denom)
{
    printf("%u %u %u\n", pix->red, pix->green, pix->blue);
    rgb_T rgb = {
        .red = pix->red / denom,
        .green = pix->green / denom,
        .blue = pix->blue / denom
    };
    ypp_T out = rgb_to_ypp(rgb);
    return out;
}
