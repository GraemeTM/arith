#include <stdlib.h>
#include <stdio.h>
#include "color_math.h"

int main()
{
  color_T red = rgb_to_ypp(1, 0, 0);
  printf("[%1.2f, %1.2f, %1.2f]\n", red->val1, red->val2, red->val3);
  red = ypp_to_rgb(red->val1, red->val2, red->val3);
  printf("[%1.2f, %1.2f, %1.2f]\n", red->val1, red->val2, red->val3);
}
