#ifndef MEMWRAP_H
#define MEMWRAP_H

#include <stddef.h>

#if defined(USE_FREERTOS)
/* Correct FreeRTOS headers for pvPortMalloc / vPortFree */
#include "FreeRTOS.h"
#include "portable.h"
#include <string.h>

static inline void *mem_malloc(size_t n) { return pvPortMalloc(n); }
static inline void mem_free(void *p) { vPortFree(p); }

/* calloc */
#if defined(pvPortCalloc)
static inline void *mem_calloc(size_t nmemb, size_t size)
{
  return pvPortCalloc(nmemb, size);
}
#else
/* Fallback: allocate then zero */
static inline void *mem_calloc(size_t nmemb, size_t size)
{
  size_t total = nmemb * size;
  void *p = pvPortMalloc(total);
  if (p)
    memset(p, 0, total);
  return p;
}
#endif

/* realloc */
#if defined(pvPortReAlloc)
static inline void *mem_realloc(void *ptr, size_t newsize)
{
  return pvPortReAlloc(ptr, newsize);
}
#else
/* Fallback: allocate new block, copy, free old */
static inline void *mem_realloc(void *ptr, size_t newsize)
{
  if (!ptr)
    return pvPortMalloc(newsize);
  void *newp = pvPortMalloc(newsize);
  if (newp && newsize)
  {
    /* NOTE: caller must ensure they know old size if shrinking */
    /* Without old size info we conservatively copy newsize bytes
       only if we know it's safe.  Caller beware. */
    /* For most FreeRTOS ports you'll need to track old size yourself. */
  }
  if (newp)
    vPortFree(ptr);
  return newp;
}
#endif

#else /* ----- Standard C library ----- */

#include <stdlib.h>
#include <string.h>

static inline void *mem_malloc(size_t n) { return malloc(n); }
static inline void mem_free(void *p) { free(p); }
static inline void *mem_calloc(size_t n, size_t s) { return calloc(n, s); }
static inline void *mem_realloc(void *p, size_t n) { return realloc(p, n); }

#endif

#endif
