# T225 S23 P3 — Native PIC Lifecycle Result

## Result

The original Bochs 8259 PIC now forms a finite `bx-mantle` machine facility in
the formal MSVC x64 `/MT` Ninja graph.  The result is intentionally narrower
than an INT 06 implementation: it proves native PIC port/lifecycle mechanics
only.  It does not decode a selector, route a BOP, invoke an OpenNT handler,
write guest bytes, or make a native startup-continuity claim.

## Source and ownership

- Adopted source: `refs/bochs/iodev/pic.cc` and `pic.h`, identities fixed by
  [the S23 P1 map](../research/t225-s23-int06-native-pic-source-map-001.md).
- The original PIC constructor, initialization, command/mask/IRQ/IAC behavior,
  and port handlers remain native Bochs behavior in `src/bx-mantle/pic.*`.
- `BX-MANTLE-082` records the only severance: the original plugin entry is
  replaced by mantle-private create/destroy helpers, since CPU5 uses the
  translation-unit `thePic` singleton and the product plugin registry is
  excluded.
- The four port registration/unregistration members are recovered from
  `refs/bochs/iodev/devices.cc:376-686`; no product device discovery, loader,
  timer, firmware, state persistence, or additional device is admitted.

## Formal build and execution

A fresh graph was generated with:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T225S7FullNinjaGraph.ps1 `
  -RepositoryRoot . -BuildRoot build/M0-T225-S23/native-pic-r5
ninja -C build/M0-T225-S23/native-pic-r5 --quiet `
  bin/t225-s23-native-pic-lifecycle-fixture.exe
```

The graph accepted `src/bx-mantle/pic.cc` as a `bx-mantle` member, emitted
`bx-core.lib`, `bx-mantle.lib`, and the focused fixture under the pinned
CPU5/P-MMX, MSVC x64 `/MT` configuration.  Its test-local #UD bridge always
declines and is never exercised; this keeps the fixture's link closure limited
to `bx-core` plus `bx-mantle`, rather than importing BOP routing.

`tests/bx-mantle/t225_s23_native_pic_lifecycle_fixture.cc` exited `0` over two
consecutive machine lifecycles.  Per lifecycle it verified:

1. PIC binding replaces the stub after the empty port-space exists.
2. native mask port `21h` reads `FFh`, accepts `FEh`, then returns `FEh`.
3. native `raise_irq(0)` and `bx_pc_system.IAC()` yield the original vector
   `08h`.
4. native OCW3/EOI commands at `20h` expose ISR bit 0 then clear it.
5. cleanup unregisters the eight PIC handlers, restores the stub, and permits
   a second lifecycle.

The final fresh graph includes the factory-null failure path and has no residual
`BX_PLUGGABLE` export macro; its fixture again exited `0`.

## Boundary and profile result

`tests/bx-mantle/Test-T225S23NativePicBoundary.ps1` passes.  It checks the
factory/init/fini/stub-restore/destroy order and rejects plugin entry points,
registry invocation, service vocabulary, adapter input, and environment input.

PIC lifecycle has no host namespace or mutation operation, so Direct and
Readonly have no differing branch to test here: both use this same
profile-neutral mechanical facility.  This is not a substitute for the later
Direct/Readonly bounded native observation required by the S23 packet.

## Remaining S23 work

The original x86 `illegal_op_int`/`unexpected_int` logic remains unlinked.  A
later admitted step must separately recover its saved-frame, PIC, BDA, and
failure contract through a typed x64 boundary.  No inference from this PIC
fixture authorizes an INT 06 provider or any BOP implementation.