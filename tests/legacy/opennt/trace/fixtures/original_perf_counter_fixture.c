/*
 * Bounded owner probe for the original nt_timer.c performance-counter pair.
 * It neither starts the historical timer thread nor supplies a clock facade.
 */
#include <stdint.h>

extern void InitPerfCounter(void);
extern uint32_t GetPerfCounter(void);

int main(void)
{
    uint32_t first;
    uint32_t second;

    InitPerfCounter();
    first = GetPerfCounter();
    second = GetPerfCounter();
    return second < first;
}
