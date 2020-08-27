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

#include <stddef.h>
#include <stdlib.h>

#include "header_array.h"

#ifdef _WIN32

#  define restrict __restrict
#  define inline __inline

#else

#  define restrict
#  define inline

#endif

#ifdef _WIN64
#  define HA_X64
#endif

inline static size_t
ha_round_up(size_t arg)
{
  --arg;
  arg |= arg >> 1;
  arg |= arg >> 2;
  arg |= arg >> 4;
  arg |= arg >> 8;
  arg |= arg >> 16;
#ifdef HA_X64
  arg |= arg >> 32;
#endif
  ++arg;
  return arg;
}

struct ha_header
{
  size_t capacity;
  size_t size;
};

/* Obtain header pointer from begin pointer. */
#define HA_BTH(A) \
  ((struct ha_header*)(void*)((unsigned char*)(A) - sizeof(struct ha_header)))
/* Obtain begin pointer from header pointer. */
#define HA_HTB(A) ((void*)((unsigned char*)(A) + sizeof(struct ha_header)))

#define HA_CAPACITY(A) ((HA_BTH(A))->capacity)

#define HA_SIZE(A) ((HA_BTH(A))->size)

#define HA_CALCULATE_ALLOCATION_SIZE(element_size, element_count) \
  (ha_round_up(sizeof(struct ha_header) + (element_size) * (element_count)))

#define HA_CALCULATE_CAPACITY(element_size, current_allocation_size) \
  (((current_allocation_size) - sizeof(struct ha_header)) / (element_size))

static enum ha_bool
ha_reallocate(void* restrict* const restrict ha_begin_ptr,
              const size_t                   element_size,
              const size_t                   allocation_size);

void*
ha_new(const size_t element_size, const size_t element_count)
{
  register const size_t allocation_size =
    HA_CALCULATE_ALLOCATION_SIZE(element_size, element_count);
  register void* result = malloc(allocation_size);

  if (result != NULL)
  {
    ((struct ha_header*)result)->capacity =
      HA_CALCULATE_CAPACITY(element_size, allocation_size);
    ((struct ha_header*)result)->size = element_count;
    result = (unsigned char*)result + sizeof(struct ha_header);
  }
#ifdef HA_ABORT_ON_FAIL
  else
  {
    abort();
  }
#endif

  return result;
}

void
ha_delete(void* restrict* const restrict ha_begin)
{
  free(HA_BTH(*ha_begin));
  *ha_begin = NULL;
}

inline size_t
ha_size(void* const restrict ha_begin)
{
  return HA_SIZE(ha_begin);
}

inline void*
ha_end(void* const restrict ha_begin, const size_t element_size)
{
  return (unsigned char*)ha_begin + HA_SIZE(ha_begin) * element_size;
}

void*
ha_push_back(void* restrict* const restrict ha_begin_ptr,
             const size_t                   element_size)
{
  register void* back;

  if (HA_SIZE(*ha_begin_ptr) == HA_CAPACITY(*ha_begin_ptr))
  {
    ha_reallocate(
      ha_begin_ptr,
      element_size,
      HA_CALCULATE_ALLOCATION_SIZE(element_size, HA_SIZE(*ha_begin_ptr))
        << 1U);
  }

  back = ha_end(*ha_begin_ptr, element_size);
  HA_SIZE(*ha_begin_ptr) += 1U;

  return back;
}

inline void
ha_pop_back(void* const restrict ha_begin)
{
  HA_SIZE(ha_begin) -= 1U;
}

enum ha_bool
ha_resize(void* restrict* const restrict ha_begin_ptr,
          const size_t                   element_size,
          const size_t                   new_size)
{
  register enum ha_bool success =
    ha_reserve(ha_begin_ptr, element_size, new_size);

  if (success)
  {
    HA_SIZE(*ha_begin_ptr) = new_size;
  }

  return success;
}

inline size_t
ha_capacity(void* const restrict ha_begin)
{
  return HA_CAPACITY(ha_begin);
}

enum ha_bool
ha_reserve(void* restrict* const restrict ha_begin_ptr,
           const size_t                   element_size,
           const size_t                   desired_capacity)
{
  register enum ha_bool success;

  if (HA_CAPACITY(*ha_begin_ptr) < desired_capacity)
  {
    success = ha_reallocate(
      ha_begin_ptr,
      element_size,
      HA_CALCULATE_ALLOCATION_SIZE(element_size, desired_capacity));
  }
  else
  {
    success = Ha_True;
  }

  return success;
}

static enum ha_bool
ha_reallocate(void* restrict* const restrict ha_begin_ptr,
              const size_t                   element_size,
              const size_t                   allocation_size)
{
  register enum ha_bool success;

  register void* const restrict reallocation =
    realloc(HA_BTH(*ha_begin_ptr), allocation_size);

  if (reallocation != NULL)
  {
    *ha_begin_ptr = HA_HTB(reallocation);
    HA_CAPACITY(*ha_begin_ptr) =
      HA_CALCULATE_CAPACITY(element_size, allocation_size);
    success = Ha_True;
  }
  else
  {
#ifdef HA_ABORT_ON_FAIL
    abort();
#else
    success = Ha_False;
#endif
  }

  return success;
}

#undef HA_BTH
#undef HA_HTB
#undef HA_CAPACITY
#undef HA_SIZE
#undef HA_CALCULATE_ALLOCATION_SIZE
#undef HA_CALCULATE_CAPACITY

#undef restrict
#undef inline

#undef HA_X64
