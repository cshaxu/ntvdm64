#include <windows.h>
#include <stdlib.h>
#include "compat/include/invalid_virtual_instance.h"

__declspec(noreturn) void mvdm_softpc_invalid_virtual_instance(unsigned long id)
{
    EXCEPTION_RECORD record = {0};
    record.ExceptionCode = 0xc0000420; /* Existing broken-owner convention. */
    record.ExceptionFlags = EXCEPTION_NONCONTINUABLE;
    record.NumberParameters = 1;
    record.ExceptionInformation[0] = id;
    RaiseFailFastException(&record, NULL, 0);
    abort();
}
