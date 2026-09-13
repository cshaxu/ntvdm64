# M0 T407 S14 — FSTI fixed-selector recovery

## Decision

S14 restores the registered CPU40 `0040h` early selector carrier in
`softpc.new/base/ccpu386/c_seg.c`. This is not a new descriptor-table policy:
it is the minimal standalone substitute for the fixed VDM selector inherited
from the NT kernel before DOSX publishes its ordinary descriptor table.

S5 had removed that carrier on the assumption that the GDT image would always
contain the DOSX descriptor. The selected original order disproves that
assumption. `dxstrt.asm` returns from `AllocateExceptionStack`, expands
`FSTI`, and then uses the original `SwitchToRealMode` route. The later
`dxboot.asm` `NSetSegmentDscr(SEL_BIOSDATA, 0400h, FFFFh, STD_DATA)` publication
cannot service that earlier `MOV DS,40h`.

## Evidence

The one-run, default-off scalar observer recorded the pre-repair terminal at
`00C7:0118`, the `MOV DS,AX` in `FSTI`. It neither read nor wrote guest memory
and was removed before delivery. With the recovered carrier, the same bounded
WRITE run recorded this original continuation:

```text
MVDM-DOSX-EXCEPTION-STACK-RETURN source=00C7:0489 target=00C7:0113
MVDM-DOSX-STARTUP-STI cs=00C7 ip=0137 msw=0011 if=1
MVDM-DOSX-RM-SWITCH source=00CF:118F target=D1C9 msw=0011
```

This proves the repaired path passes the original `FSTI`, executes its
original `STI`, and reaches the selected BOP `FDh` real-mode-transition
provider. It does not claim WRITE acceptance or a WOW lifecycle repair.

## Implementation and verification

| Check | Result |
| --- | --- |
| Source/current audit | `e8e41ab7f` removed the previous `0040h` carrier while leaving `MVDM-HOST-DIV-227` registered. The current repair restores only that 20-line CPU40 branch and no diagnostic hook. |
| x86 build/link | `build/M0-T407/S14/r002-from-s13-clean`: rebuilt changed CCPU/DPMI/adapter objects, recreated their owner libraries, and linked `original-softpc-process.exe`. |
| Final staged EXE | `O:\ntvdm64\ntvdm32.exe`, SHA-256 `3E575063FD6D4EBE005BF2346A5661A5798941C38163E36FE2262BF2063866B7`. |
| MEM / COMMAND Console regression | Existing bounded Console observer: both completed with exit `0`; logs `O:\ntvdm64\logs\m0-t407-s14-r002\{mem,command}-clean-regression.log`. |
| WRITE, no diagnostics | `system32\write.exe` completed within 30 seconds with exit `255`; this is the prior WOW frontier, not success. |
| Trace run | `O:\ntvdm64\logs\m0-t407-s14-r002\write-rm-switch.log`; it is evidence-only and the compiled observer code was removed afterward. |

## T407 disposition

T407's DPMI/XMS, descriptor-domain and immediate DOSX transition work is now
closed. The remaining WRITE/WOW lifecycle is explicitly owned by the queued
WOW/debugger and single-process WOW16 packages; no kernel Fast carrier, TSS
reconstruction, broker, or synthetic descriptor publication is retained or
introduced by this closure.
