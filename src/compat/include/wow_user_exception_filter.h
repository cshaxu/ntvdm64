#ifndef WOW_USER_EXCEPTION_FILTER_H
#define WOW_USER_EXCEPTION_FILTER_H
#include <windows.h>
/* XP windows/core/w32inc/w32err.h retail FALSE filter catches without
 * setting LastError. Reject a new TRUE consumer until its contract is bound. */
#define W32ExceptionHandler(setError, flags) \
    ((void)sizeof(char[(setError) ? -1 : 1]), EXCEPTION_EXECUTE_HANDLER)

#endif
