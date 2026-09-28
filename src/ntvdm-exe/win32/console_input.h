#ifndef NTVDM_CONSOLE_INPUT_H
#define NTVDM_CONSOLE_INPUT_H
#include <windows.h>
/* Original five-record read, with room for eight Scan-1 transitions per
 * scan-less host key. No pending queue or ownership transfer in this layer. */
#define NTVDM_PC_INPUT_RECORDS 40u
BOOL ntvdm_console_read_pc_input(HANDLE,PINPUT_RECORD,DWORD,LPDWORD,USHORT);
#endif
