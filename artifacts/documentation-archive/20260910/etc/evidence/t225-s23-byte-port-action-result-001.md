# T225 S23 P6 — Selector-Blind Native Byte-Port Action Result

## Decision

`BX-MANTLE-083` is implemented as
`src/bx-mantle/bx_ntvdm_port_action_v1.{h,cc}`.  It is a fixed-width,
synchronous, one-byte native port request.  It does not recognize a guest
instruction, selector, BOP, OpenNT service, profile, or host capability.

The existing `bx_ntvdm_minimal_machine_c` now enables this action only after
its already-established memory, default port space, native PIC, CPU reset and
capability initialization.  It disables the action before PIC and port-space
cleanup.  While active, the action delegates to the existing native Bochs
`bx_devices.inp/outp`; it exports only copied success and read-byte values.

## Boundary and lifecycle proof

The focused static check is
`tests/bx-mantle/Test-T225S23BytePortActionBoundary.ps1`.  It requires the
two byte operation kinds, lifecycle gate, and native `inp/outp` delegation;
it rejects BOP/OpenNT/DOS/WOW/DEM/selector/callback/plugin/firmware and raw
pointer vocabulary from the new action source.

The existing PIC fixture was changed to use the typed action, rather than
calling the port table directly.  It verifies twice in one process:

1. a request before machine initialization is rejected;
2. after native PIC binding, `21h` reads `FFh`, writes `FEh`, and reads `FEh`;
3. native IRQ0/IAC followed by `20h` OCW3/ISR read and EOI observes `01h` then
   `00h`;
4. cleanup restores the PIC stub and a subsequent typed port request is
   rejected.

No Direct/Readonly branch exists because this is internal machine state, not
a host mutation.

## Formal build result

Fresh root: `build/M0-T225-S23/native-port-action-r2`.

```text
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T225S7FullNinjaGraph.ps1 \
  -RepositoryRoot . -BuildRoot build/M0-T225-S23/native-port-action-r2
ninja -C build/M0-T225-S23/native-port-action-r2 --quiet \
  bin/t225-s23-native-pic-lifecycle-fixture.exe
build/M0-T225-S23/native-port-action-r2/bin/t225-s23-native-pic-lifecycle-fixture.exe
# fixture-exit=0
```

This is the formal MSVC x64 `/MT`, CPU5/P-MMX graph.  During this run the
Ninja generator's link/library response-file rule was corrected from an
absolute drive path to the output-relative graph path.  The previous rule
produced a concrete `LNK1104` for a missing `.rsp`; the fresh graph built and
ran after the correction.  This is a build-graph repair only, not a Bochs or
OpenNT semantic change.

## Limit

P6 supplies the mechanical port half of the P5 provider boundary.  It does
not yet compose the provider with checked ordinary RAM operations, add a
selector `06h` route in bx-vdm, or make a native observation.  Those remain
separate S23 steps.