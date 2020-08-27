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

#ifndef HEADER_ARRAY_H
#define HEADER_ARRAY_H

#include <stddef.h>

#ifdef _WIN32

#  define restrict __restrict
#  define inline __inline

#else

#  define restrict
#  define inline

#endif

enum ha_bool
{
  Ha_False = 0,
  Ha_True  = !0,
};

struct ha_header;

void*
ha_new(size_t element_size, size_t element_count);

void
ha_delete(void* restrict* const restrict ha_begin);

#define HA_DELETE(ha_begin_ptr) \
  (ha_delete((void* restrict* const restrict)(ha_begin_ptr)))

inline size_t
ha_size(void* const restrict ha_begin);

/* Obtain past-the-end pointer from begin pointer. */
inline void*
ha_end(void* const restrict ha_begin, const size_t element_size);

/* Obtain past-the-end pointer from begin pointer. */
#define HA_END(ha_begin) (ha_end(ha_begin, sizeof *ha_begin))

/* Obtain past-the-end pointer from begin pointer. (In-place) */
#define HA_END_2(ha_begin) ((ha_begin) + ha_size(ha_begin))

void*
ha_push_back(void* restrict* const restrict ha_begin_ptr,
             const size_t                   element_size);

#define HA_PUSH_BACK(ha_begin_ptr)                              \
  (ha_push_back((void* restrict* const restrict)(ha_begin_ptr), \
                sizeof **(ha_begin_ptr)))

inline void
ha_pop_back(void* const restrict ha_begin);

enum ha_bool
ha_resize(void* restrict* const restrict ha_begin_ptr,
          const size_t                   element_size,
          const size_t                   new_size);

#define HA_RESIZE(ha_begin_ptr, new_size)                    \
  (ha_resize((void* restrict* const restrict)(ha_begin_ptr), \
             sizeof **(ha_begin_ptr),                        \
             new_size))

inline size_t
ha_capacity(void* const restrict ha_begin);

enum ha_bool
ha_reserve(void* restrict* const restrict ha_begin,
           const size_t                   element_size,
           const size_t                   desired_capacity);

#define HA_RESERVE(ha_begin_ptr, desired_capacity)            \
  (ha_reserve((void* restrict* const restrict)(ha_begin_ptr), \
              sizeof **(ha_begin_ptr),                        \
              desired_capacity))

#undef restrict
#undef inline

#endif
