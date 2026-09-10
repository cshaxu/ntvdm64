# M0 T318 S2 P71 — remove startup-observation perturbation

## Change

P66--P70 had already captured the pre-guest milestones needed to establish the
original entry state.  The three associated `WriteFile` observations were not
part of OpenNT/SoftPC semantics:

- after original `InitialiseDosEmulation()`;
- immediately before original `ccpu(FALSE)`; and
- after original CCPU `SETUP_HOST_IP` at the first fetch.

P68 had independently demonstrated that another hot-path console writer
changes the bounded trace.  Retaining these startup writers would therefore
make the fixed observer a product input.  This P removes the three calls, the
first-fetch latch, and their now-unused adapter entry.  It retains the narrow
observation at original `MS_bop_0`, where the selector and service byte have
already been decoded and no BOP routing is changed.

## Formal verification

The fresh selected x86 CPU40 Ninja target completed successfully after the
removal:

```text
ninja -C build/M0-T318/S2/formal-x86 -j 8 original-softpc-process.exe
```

The historical compiler warnings remain visible and are not suppressed by this
change.

## Fixed-container result

The product was staged in a new disposable directory
`build/M0-T318/S2/runtime-r34-no-startup-milestones` and run once through the
unchanged external console-owning, non-debug eight-second observer.

It timed out normally and its console capture recorded:

```text
MVDM-BOP-DISPATCH 50:11
MVDM-BOP-DISPATCH 50:3B
MVDM-BOP-DISPATCH 50:0F
MVDM-BOP-DISPATCH 50:1B
```

The remaining console bytes are deliberately not interpreted as an additional
service claim.  The raw report, console capture, and identity manifest are
retained under `artifacts/research/m0-t318-s2-p71-no-startup-milestones.txt`
with `.console.txt` and `.json` companions.

## Result

The prior P67 non-observation of the first NTIO BOP is superseded: it was a
property of the diagnostic-instrumented product, not evidence that the
original BOP carrier was unreachable.  The uninstrumented source-shaped
product reaches original DEM load, debug-state, drive-query, and DTA-location
dispatches.  This is continuous bootstrap progress only; it does not prove an
ordinary child `EXEC` / parent-return closure.
