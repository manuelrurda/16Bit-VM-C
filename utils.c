#include "utils.h"

void printhex(void *src, int size)
{
  unsigned char *p = src;
  printf("\n");
  for (int n = size, break_line = 0; n > 0; n--, p++, break_line++)
  {
    printf("%.02x ", *p);
    if (break_line >= 8)
    {
      break_line = -1;
      printf("\n");
    }
    fflush(stdout);
  }
  printf("\n");
}