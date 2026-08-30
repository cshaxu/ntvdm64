# M0 T313 S2 P3 — original CCPU40 archive evidence

Date: 2026-08-30  
Packet: M0 T313 S2  
Profile: Win32/x86, `CPU_40_STYLE`, `NEW_CPU`, `CCPU`.

## Source-shaped cohort

The formal graph selected the original `softpc.new/base/ccpu386` instruction,
SAS and execution sources, plus only the already-registered reached overlay
translation units.  The CPU package remains the original CCPU40 executor;
no second executor or retired Bochs route participates.

## Verification

The external disposable root `build/M0-T313/S2/r1` completed the exact target:

```text
[133/133] ... lib.exe /nologo /out:original-ccpu386.lib ...
```

`original-ccpu386.lib` was produced at 679,002 bytes.  The build logs contain
no compiler error, linker error, `LNK4006`, or stopped-build marker.

Historical source warnings (for example fixed-width scalar narrowing and
legacy macro collisions) remain visible.  This packet does not suppress or
rewrite them: under the selected x86-only CCPU40 plan, they require later
owner-contract disposition rather than a speculative source-body change.

## Boundary

This is formal compile/archive evidence for the CPU/SAS execution cohort.  It
does not prove the complete create-to-reset lifecycle or guest execution;
those remain the bounded behavior portion of T313 S2.
