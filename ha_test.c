/*****************************************************************************
MIT License

Copyright (c) 2020 xenodasein

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*****************************************************************************/

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define HA_ABORT_ON_FAIL
#include "header_array.c"
#undef HA_ABORT_ON_FAIL

#ifdef _WIN32

#  define restrict __restrict
#  define inline __inline

#else

#  define restrict
#  define inline

#endif

struct third_kind
{
  int x, x_square, x_cube;
};

int
main(void)
{
  struct third_kind* restrict x_files = ha_new(sizeof *x_files, 42U);

  assert(ha_size(x_files) == 42U);

  {
    size_t i;
    for (i = 0U; i < ha_size(x_files); ++i)
    {
      x_files[i].x        = rand() % 42;
      x_files[i].x_square = x_files[i].x * x_files[i].x;
      x_files[i].x_cube   = x_files[i].x_square * x_files[i].x;
      printf("%zu: %d, %d, %d\n",
             i,
             x_files[i].x,
             x_files[i].x_square,
             x_files[i].x_cube);
    }
    printf("\n");
  }

  {
    struct third_kind new_addition                = {3, 9, 27};
    *((struct third_kind*)HA_PUSH_BACK(&x_files)) = new_addition;
  }

  HA_RESIZE(&x_files, 256U);
  assert(ha_size(x_files) == 256U);
  x_files[255].x        = 1;
  x_files[255].x_square = 1;
  x_files[255].x_cube   = 1;

  {
    struct third_kind *it, *end;
    size_t             i = 0U;
    for (it = x_files, end = HA_END(x_files); it != end; ++it)
    {
      printf("%zu: %d, %d, %d\n", i++, it->x, it->x_square, it->x_cube);
    }
  }

  HA_DELETE(&x_files);

  return EXIT_SUCCESS;
}
