# M58 Governed Backend Readiness Reaudit

Status: completed read-only source reaudit, 2026-08-08. Source examined:
`D:\home\repos.hobby\ntvdm64\src\core\machine`. No source in that
repository was modified.

## Usable Foundations

| Foundation | Current source evidence |
| --- | --- |
| Real-mode prepared entry | `entry_plan_interface.h`: copied register state and up to 16 non-overlapping physical preloads. |
| Frozen checked memory topology | `memory_interface.h`: stopped/paused physical range query reports ordinary RAM vs provider without exposing backing storage. |
| Immutable ROM and A20 | `rom_mapping_interface.h`, `memory_interface.h`. |
| Software CPU and machine lifecycle | `machine_interface.h`: run/reset/stop, real machine lifecycle states, PIC/PIT/DMA/KBC/RTC/FDC/HDC and display foundations. |
| Restricted `#UD` callback discipline | `undefined_instruction_transition_interface.h` plus `machine.c`: frozen registration, copied bytes/GPR/FLAGS and finite result kinds. |

## Remaining Gaps

1. **No configured decode-point observation.** The existing callback is
   entered only after the CPU detects a real `#UD`. Its input has `CS:EIP` and
   GPR/FLAGS but not the copied segment/mode context required for normal
   translation. It cannot observe an otherwise valid configured marker.
2. **No paused multi-span ordinary-RAM transaction.** The current API offers
   checked one-span reads/writes and entry-time preload validation. It has no
   begin/preflight/commit handle, no copied-span all-or-none publication, and
   no stale-boundary invalidation contract.
3. **No external stable ABI.** Current composition has configuration borrows
   for internal product setup; that is not a versioned external adapter ABI.

## Boundary Result

M57 makes a governed x64 software backend, rather than historical x86 V86 or
forced CCPU, the applicable route. The existing foundations are strong enough
to host such a backend, but an external adapter remains blocked until the two
neutral gaps acquire independent consumers, core-owned tests, and then a
copied-value ABI as stated in M55.

This is not a request to place DOS paths, file formats, service identifiers,
host handles, or session policy in core.
