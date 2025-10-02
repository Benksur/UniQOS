#ifndef BLOOP_OPTIMIZED_H
#define BLOOP_OPTIMIZED_H

#include <stdint.h>

// External declarations for the bloop arrays
extern int16_t bloop_base[];

#define BLOOP_BASE_SIZE 25
#define BLOOP_REPEAT_COUNT 38 // 25 * 38 = 950 (close to original 951)

#endif // BLOOP_OPTIMIZED_H
