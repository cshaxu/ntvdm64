# M0 T407 S11 — Non-fast DOSX publication trace

## Decision

The current WRITE frontier is **not** a missing Fast carrier, a missing final
IDTR descriptor, a failed `53:01` transition, or a failed `53:11` DPMI-info
projection.  The next original operation after the successfully returned
`53:11` does not execute.  The failure boundary is therefore the CPU40 BOP
return/direct guest-continuation edge, before WOW's `MS_bop_1` ingress.

This is a trace result, not a repair claim.

## Original order and observed order

The selected `WOW_x86` source in `mvdm-host/dpmi/486/dxboot.asm` has this
order:

1. DOSX publishes descriptor data through `53:00` and installs handlers with
   `53:02`.
2. `53:01` enters protected mode.
3. DOSX calls `53:11` (`InitializePmStackInfo`) with the hardware-interrupt
   stack selector in ES.
4. Immediately after that BOP returns, DOSX calls
   `NSetSegmentDscr(SEL_VDMTIB, cx, dx, 0, SIZE VdmPmStackInfo, STD_DATA)`.
5. Only later does KRNL386 enter the original `MS_bop_1` WOW ingress.

The PID-correlated final WRITE run (`PID 20140`) recorded:

```text
... enter=53:01 cs=D1C9 ip=1101 ...
... leave=53:01 cs=00CF ip=1101 ax=0001 cf=0 if=0
... enter=53:11 cs=00C7 ip=0486 ...
MVDM-DPMI-PMSTACK es=017F cx=0014 dx=6800 success=1
... leave=53:11 cs=00C7 ip=0486 ax=017F cf=0 if=0
```

There is no subsequent `53:00` descriptor-publication event and the existing
single WOW-ingress witness did not write a marker.  This places the frontier
between the completed `53:11` provider return and DOSX's next source
instruction, before `NSetSegmentDscr(SEL_VDMTIB, ...)` and before WOW32 is
entered.  The trace contains guest scalars only; it contains no host pointer.

## Instrumentation disposition

`MVDM_DPMI_PUBLICATION_REPORT_PATH` is a default-off host-only selector.  It
is captured and removed before the original COMMAND environment copy, restored
only for a native child, and writes only:

- all `53:00`, `53:01` and `53:11` ingress/return pairs;
- the first 32 `53:02` pairs, preventing handler registration from becoming a
  high-frequency trace; and
- the completed `53:11` ES and CX:DX result.

No dispatcher, return IP, guest memory, descriptor, CPU, Fast or WOW policy
changes.  In the default configuration the report-path test returns before
any trace counter or file operation.

## Verification

| Check | Result |
| --- | --- |
| Formal x86 build | `build/M0-T407/S11/r001-dosx-publication-trace`; Node `v22.22.1`; `original-softpc-process.exe` linked and subsequent Ninja run said `no work to do`. |
| Staged product | `O:\ntvdm64\ntvdm32.exe`, 3,235,328 bytes, SHA-256 `a582da17a8ce25cc02a55bb0bc154ddb386809a575fbdf47a0fb489f66ce76ed`. |
| MEM | PID 53184, completed within 30 seconds, exit `0`. |
| COMMAND | PID 18080, `COMMAND.COM /c exit` completed within 30 seconds, exit `0`. |
| WRITE | PID 20140, completed within 30 seconds, exit `72`; narrow trace hit `53:00/11/02/01`, successful PM-stack publication, and no later `53:00`/WOW marker. |
| Logs | `O:\ntvdm64\logs\m0-t407-s11-r001\`. |

## Next source-first experiment

The receiver is **T407 S12: post-`53:11` DOSX BOP continuation**.  It must
observe the source-owned CCPU return from `53:11` and the immediate DOSX
instruction window until either `NSetSegmentDscr(SEL_VDMTIB, ...)` reaches
`53:00` or the original CPU terminal is identified.  It must not modify the
provider result, restore Fast support, or assume WOW32 is involved.
