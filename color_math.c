#include <stdlib.h>
#include "assert.h"
#include "color_math.h"


color_T rgb_to_ypp(float r, float g, float b)
{
  color_T ypp = malloc(sizeof(ypp));
  assert(ypp != NULL);
  assert(r >= 0 && r <= 1 && g >= 0 && g <= 1 && b >= 0 && b <= 1);

  ypp->val1 = 0.299 * r + 0.587 * g + 0.114 * b;
  ypp->val2 = -0.168736 * r - 0.331264 * g + 0.5 * b;
  ypp->val3 = 0.5 * r - 0.418688 * g - 0.081312 * b;
  return ypp;
}

color_T ypp_to_rgb(float y, float pb, float pr)
{
  color_T rgb = malloc(sizeof(rgb));
  assert(rgb != NULL);

  rgb->val1 = 1.0 * y + 0.0 * pb + 1.402 * pr;
  rgb->val2 = 1.0 * y - 0.344136 * pb - 0.714136 * pr;
  rgb->val3 = 1.0 * y + 1.772 * pb + 0.0 * pr;
  return rgb;
}
