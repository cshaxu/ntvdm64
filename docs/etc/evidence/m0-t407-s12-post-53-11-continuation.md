# M0 T407 S12 — Post-53:11 DOSX BOP continuation

## Decision

The selected `53:11` return is **source-correct**.  It is not the
`dxboot.asm` call immediately followed by `NSetSegmentDscr(SEL_VDMTIB, ...)`.
It is the earlier `AllocateExceptionStack` call in the selected 486 DOSX
source.  Therefore S11's inference from “no following `53:00`” was invalid:
the observed BOP has no source requirement to publish `SEL_VDMTIB` next.

No functional repair is warranted at this edge.  The one-run continuation
witness was removed after capture, leaving no product code diff from S12.

## Source and runtime alignment

`mvdm-host/dpmi/486/dxstrt.asm` has the exact selected sequence:

```asm
AllocateExceptionStack:
        ...
        push    es
        mov     ax, selEHStack
        mov     es, ax
        mov     bx, npEHStackLimit
        DPMIBOP InitializePmStackInfo
        pop     es
aes_ok: clc
        ret
```

The bounded WRITE run, PID `34756`, recorded:

```text
MVDM-DPMI-PUBLICATION enter=53:11 cs=00C7 ip=0486 ...
MVDM-DPMI-PMSTACK es=017F cx=0014 dx=6800 success=1
MVDM-DPMI-PMSTACK-NEXT cs=00C7 ip=0486 bytes=07 F8 C3 00 00 00 00 00
MVDM-DPMI-PUBLICATION leave=53:11 cs=00C7 ip=0486 ax=017F cf=0 if=0
```

At DPMI ingress the original dispatcher has already consumed the `11` service
byte, so the observed window begins immediately afterward: `07` is `POP ES`,
`F8` is `CLC`, and `C3` is `RET`.  This exactly matches the source-owned
continuation above.  The `53:11` provider result is also successful.

The distinct `dxboot.asm` sequence which publishes `SEL_VDMTIB` remains a
real original owner, but this run did not identify its particular call site.
It cannot be used to blame this successful `AllocateExceptionStack` return,
and no Fast carrier, BOP convention, CPU instruction change, or synthetic
descriptor publication is authorized by this result.

## Verification and disposition

| Check | Result |
| --- | --- |
| One-run witness | Default-off, host-only `MVDM_DPMI_PUBLICATION_REPORT_PATH`; it read eight bytes only after the unchanged `53:11` provider returned. |
| WRITE | `O:\ntvdm64\ntvdm32.exe O:\ntvdm64\system32\WRITE.EXE`; PID `34756`; completed within 30 seconds; exit `72`. The result is a frontier observation, not WRITE acceptance. |
| Log | `O:\ntvdm64\logs\m0-t407-s12-r001\write-dpmi-continuation.log`. |
| Product disposition | The temporary observer was removed after the source match. No CCPU, BOP, DPMI provider, descriptor, Fast, or WOW behavior remains changed by S12. |
| Next receiver | S13 must attribute the specific `dxboot.asm` `53:11 → NSetSegmentDscr(SEL_VDMTIB, ...)` site before evaluating its descriptor publication. |
