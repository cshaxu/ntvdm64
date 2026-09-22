# debugger family

The selected debugger owner is the byte-identical original
`src/mvdm/dbg/dbg.c`. It supplies `DBGInit`, `DBGDispatch`, module and
segment notifications, fault/return notifications, and the WOW-facing export
shapes. Its `STATUS_VDM_EVENT` transport is a continuable Win32 debug
exception, not a private CSR RPC. The selected public event layout is the
original `src/opennt-host/public/sdk/inc/vdmdbg.h`.

This component contains only the two worker-local boundaries that the original
body cannot own on a standalone CCPU40 process:

| File | Retained boundary |
| --- | --- |
| `source/dbg_registers.c` | Checked, call-scoped copy of the original six-DWORD DPMI debug-register input before the original DR0--DR3/DR6/DR7 write order. It replaces the unavailable kernel-VDM raw guest alias; no guest pointer persists. |
| `source/dbg_prompt.c` | Thin dynamic binding to the still-exported NTDLL `DbgPrompt` entry. The original entry/result shape is retained; a missing export is an explicit failure, not a synthetic debugger protocol. |

The removed `dbg_init`, `dbg_state`, `dbg_dispatch`, and `dbg_unavailable`
providers were superseded by the original `dbg.c`; they are not fallback
implementations and must not be reintroduced. S43 evidence records the
remaining real-debugger consumer and OEM-path acceptance work.
