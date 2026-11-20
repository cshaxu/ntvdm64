#ifndef COMPAT_INPUT_READINESS_H
#define COMPAT_INPUT_READINESS_H
#include <windows.h>

/* State shared by the wait producer and the Console reader. The owner
 * publishes it after initialization and joins its waiter before disposal. */
typedef struct compat_input_readiness {
    CRITICAL_SECTION lock;
    HANDLE rearm, wake;
    DWORD error;
} compat_input_readiness;

DWORD compat_input_readiness_ack(compat_input_readiness *);
#endif
