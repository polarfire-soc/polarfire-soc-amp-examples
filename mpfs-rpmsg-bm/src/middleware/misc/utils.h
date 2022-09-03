#ifndef UITLS_H
#define UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

# include <stdint.h>
# include <stdio.h>
#include "mpfs_hal/mss_hal.h"

#  define TICKS_PER_SEC         ((unsigned long long)LIBERO_SETTING_MSS_RTC_TOGGLE_CLK)
#  define TICKS_PER_MILLISEC    (TICKS_PER_SEC/1000llu)
#  define ONE_SEC               (1llu * TICKS_PER_SEC)
#  define ONE_MILLISEC          (1llu * TICKS_PER_MILLISEC)

uint64_t GetTime(void);
bool Timer_IsElapsed(uint64_t startTick, uint64_t durationInTicks);
void SpinDelay_MilliSecs(uint32_t milliseconds);

#ifdef __cplusplus
}
#endif

#endif
