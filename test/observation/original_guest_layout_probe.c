/* Compile-time x86 layout checks for restored original guest declarations. */
#include <windows.h>
#include <winternl.h>
#include <stddef.h>
#include "dosdef.h"
#include "dossvc.h"
#include "apistruc.h"
#include "vrnmpipe.h"

_Static_assert(sizeof(void *) == 4, "x86 only");
_Static_assert(sizeof(SRCHDTA) == 43, "DTA size");
_Static_assert(offsetof(SRCHDTA, FFindId) == 4, "DTA identity offset");
_Static_assert(sizeof(DIRENT) == 32, "FCB directory size");
_Static_assert(offsetof(DIRENT, pFFindEntry) == 12, "FCB pointer offset");
_Static_assert(sizeof(DEMEXTERR) == 9, "extended error size");
_Static_assert(offsetof(DEMEXTERR, ExtendedErrorPointer) == 5, "error pointer offset");
_Static_assert(sizeof(struct DosWriteMailslotStruct) == 8, "mailslot size");
_Static_assert(offsetof(struct DosWriteMailslotStruct, DWMS_Buffer) == 4, "mailslot pointer");
_Static_assert(sizeof(DOS_CALL_NAMED_PIPE_STRUCT) == 24, "call pipe size");
_Static_assert(offsetof(DOS_CALL_NAMED_PIPE_STRUCT, lpOutBuffer) == 10, "pipe output");
_Static_assert(offsetof(DOS_CALL_NAMED_PIPE_STRUCT, lpInBuffer) == 16, "pipe input");
_Static_assert(sizeof(DOS_ASYNC_NAMED_PIPE_STRUCT) == 24, "async request size");
_Static_assert(offsetof(DOS_ASYNC_NAMED_PIPE_STRUCT, lpSemaphore) == 20, "async semaphore");

int main(void) { return 0; }
