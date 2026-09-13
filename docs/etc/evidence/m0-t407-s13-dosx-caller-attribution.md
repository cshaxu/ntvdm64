# M0 T407 S13 — DOSX descriptor-publication caller attribution

## Decision

The currently reached `53:00` and `53:11` calls return through their original
DOSX continuations.  The selected WRITE path does **not** reach the distinct
`dxboot.asm` `53:11 → NSetSegmentDscr(SEL_VDMTIB, ...)` sequence.  It is
therefore incorrect to synthesize or move that publication earlier.

## Evidence

The default-off, one-run observer recorded for WRITE PID `30476`:

```text
MVDM-DPMI-CONTINUATION service=00 cs=00C7 ip=3662 bytes=59 5B 58 07 5F 5B 58 8B
MVDM-DPMI-CONTINUATION service=11 cs=00C7 ip=0486 bytes=07 F8 C3 00 00 00 00 00
```

`59 5B 58 07 5F 5B 58` is the original descriptor helper's saved-register
unwind (`POP CX/BX/AX/ES/DI/BX/AX`); each of the three observed `53:00`
calls returned successfully.  `07 F8 C3` is the already-proved
`AllocateExceptionStack` continuation in `dxstrt.asm`.

The special `dxboot.asm` block has a different immediate continuation:
`DPMIBOP InitializePmStackInfo`, followed by `MOV AX, SIZE VdmPmStackInfo`
and `cCall NSetSegmentDscr,<SEL_VDMTIB,...>`.  No observed window matches
that `MOV AX` continuation.  The first unresolved source owner is instead
the caller of `AllocateExceptionStack` in `dxstrt.asm`: it returns to the
startup path which executes `FSTI` and then `SwitchToRealMode`.

## Verification and disposition

| Check | Result |
| --- | --- |
| Formal x86 build | `build/M0-T407/S13/r001-caller-attribution`; linked successfully. |
| Clean baseline regressions | Deployed baseline MEM PID `39276` and COMMAND PID `15320` both completed within 30 seconds with exit `0`. |
| WRITE observation | PID `30476`, completed within 30 seconds, exit `72`; log: `O:\ntvdm64\logs\m0-t407-s13-r001\write-caller-attribution.log`. |
| Product disposition | The temporary observer was removed after the source match; no Fast, BOP, descriptor, CPU, DPMI or WOW semantic change remains. |
| Next receiver | S14 follows the original `AllocateExceptionStack` caller through `FSTI`/`SwitchToRealMode`, rather than inventing an unreached `SEL_VDMTIB` publication. |
