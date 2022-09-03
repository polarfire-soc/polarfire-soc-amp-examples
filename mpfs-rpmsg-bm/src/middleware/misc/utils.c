
# include "utils.h"

#define mHSS_ReadRegU64(block, reg) mHSS_ReadRegEx(uint64_t, block, reg)
#define mHSS_ReadRegEx(type, block, reg) (*(volatile type*)(block + reg))

#define CLINT_BASE_ADDR		  		0x02000000
#define CLINT_MTIME_OFFSET		        0xBFF8

uint64_t CSR_GetTime(void)
{
    uint64_t time;

    time = mHSS_ReadRegU64(CLINT_BASE_ADDR, CLINT_MTIME_OFFSET);

    return time;
}

uint64_t GetTime(void)
{
    uint64_t tickCount;

    tickCount = CSR_GetTime();
    return tickCount;
}

bool Timer_IsElapsed(uint64_t startTick, uint64_t durationInTicks)
{
    return (GetTime() > (startTick + durationInTicks));
}

void SpinDelay_MilliSecs(uint32_t milliseconds)
{
    uint64_t delayTick = GetTime();
    while (!Timer_IsElapsed(delayTick, ONE_MILLISEC * milliseconds)) { ; }
}
