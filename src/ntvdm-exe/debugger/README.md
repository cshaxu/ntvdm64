# debugger family

The selected debugger owner is the byte-identical original
`src/mvdm-host/dbg/dbg.c`. It supplies `DBGInit`, `DBGDispatch`, module and
segment notifications, fault/return notifications, and the WOW-facing export
shapes. Its `STATUS_VDM_EVENT` transport is a continuable Win32 debug
exception, not a private CSR RPC. The selected public event layout is the
original `src/non-mvdm/public/sdk/inc/vdmdbg.h`.

This component contains the remaining worker-local boundary required by the
selected standalone CCPU40 process:

| File | Retained boundary |
| --- | --- |
| `source/dbg_registers.c` | Checked, call-scoped copy of the original six-DWORD DPMI debug-register input before the original DR0--DR3/DR6/DR7 write order. It replaces the unavailable kernel-VDM raw guest alias; no guest pointer persists. |

The removed `dbg_init`, `dbg_state`, `dbg_dispatch`, and `dbg_unavailable`
providers were superseded by the original `dbg.c`; they are not fallback
implementations and must not be reintroduced. S43 evidence records the
remaining real-debugger consumer and OEM-path acceptance work.

The DEM `DbgPrompt` caller uses the original `ntrtl.h` NTAPI declaration and
the existing NTDLL import library. The redundant dynamic forwarder was removed
by T483 S5 P213 after original-declaration and native-link verification.
