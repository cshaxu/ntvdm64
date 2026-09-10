# M0 T265 S3 adapter-Bochs provenance ledger

## Pinned comparison input

The baseline is the local pinned Bochs 2.6 tree at
`O:/repos.external/bochs-2.6-compat/bochs-2.6`.  This ledger records the
pre-move ownership decision for every production source initially below
`src/adapter-bochs`; it is a source-layout record, not a runtime claim.

## P1 moved now

- `scancodes.cc`, `scancodes.h` are byte-identical to
  `iodev/scancodes.{cc,h}` and now reside at
  `src/bochs-core/iodev/`.
- `paramtree.h` is byte-identical to `gui/paramtree.h` and now resides at
  `src/bochs-core/gui/`.
- `paramtree.cc` is the original same-shaped source with only the
  `BX-BUILD-002` MSVC adjacent-string-token spelling divergence.  Its two
  local `DIVERGENCE(BX-BUILD-002)` markers and the `bochs-core` README index
  register the exception.
- `logio.cc` now resides at `src/bochs-core/logio.cc` and uses the original
  upstream-relative `cpu/cpu.h` include spelling; it is byte-identical to the
  pinned source.

The manifest compiles these four sources into `bochs-core`, not
`adapter-bochs`.  The refreshed formal graph initially exposed stale corrupt
objects left by the earlier concurrent build attempt; after verifying that no
Ninja process existed, only `obj/bochs-core` and `lib/bochs-core.lib` under
the disposable S2 formal root were regenerated.  The resulting single-process
203-step formal build completed with exit `0`, and its subsequent dry run
reported no work.

## P3 moved now

- `pc_system.{cc,h}` now resides at `src/bochs-core/`.  The header remains
  byte-identical.  The source is a true subset: its full product-shell
  `Reset`/`exit` methods are excluded and the finite lifecycle remains in
  `adapter-bochs/minimal_product_shell.cc` (`BX-MACH-023`).  The build-header
  root redirect is `BX-BUILD-003`.
- `pic.{cc,h}` now resides at `src/bochs-core/iodev/`.  Its native 8259 state,
  register protocol, port handlers, IRQ and IAC mechanics are retained from
  Bochs.  The unreachable plugin product-shell entry points are a true subset
  (`BX-MACH-026`), the finite port-space teardown body is private
  `bochs-core-overlay/iodev/pic_lifecycle.cc` (`BX-MACH-024`), and the fixed
  object creation/destruction is self-authored Bochs-only assembly in
  `adapter-bochs/minimal_pic.{cc,h}`.  The focused source boundary check and
  native PIC fixture both pass.
- `keyboard.h` now resides at `src/bochs-core/iodev/` as the same-shaped
  Bochs class declaration with one registered `fini()` boundary.  The complete
  byte-identical upstream `keyboard.cc` is reference material, not a live
  production input, and is therefore retained under
  `docs/etc/legacy_code/bochs-2.6/iodev/`.  The actual headless derivative is
  `bochs-core-overlay/iodev/keyboard_headless.cc`, reached only through the
  two-call `keyboard_bridge.cc` boundary.  `adapter-bochs` calls that core
  boundary through `headless_8042.cc` and sees neither the keyboard singleton
  nor the overlay.  The headless 8042 fixture compiles, links, and exits with
  no remaining fixture process.

The current source scan confirms that `adapter-softpc` consumes no
`bochs-core` header, type, object, or global.  It may consume only the
declared selector-blind `adapter-bochs` facade.

## Remaining S3 disposition

- `headless_8042.{cc,h}`, `minimal_machine.{cc,h}`,
  `minimal_port_space.cc`, `minimal_product_shell.cc`, and
  `minimal_sim.{cc,h}`: no upstream file identity.  They remain the only
  candidate `adapter-bochs` self-authored, Bochs-only assembly files, pending
  the complete S3 vocabulary/provenance sweep.

No BOP, OpenNT, DOS, VDM, WOW, SoftPC/CCPU, or Win32 semantics were added.

## Required dependency repair

The completed source scan finds no `adapter-softpc` import of a `bochs-core`
header and no direct `bx_cpu`, `bx_mem`, or `bx_pc_system` access. The eight
formerly direct paths now call only the declared selector-blind, fixed-width
`adapter-bochs/machine_facade.{h,cc}`. The facade owns minimal-machine
lifecycle, ordinary RAM, A20, protected-range checks, real-mode state copy,
CPU run/stop, and timer registration; it exposes neither a Bochs type nor a
Bochs object. This satisfies the T265/S1 edge rule while preserving the
historical SoftPC-facing interfaces in `adapter-softpc`.

### A20 first seam

`a20_capability.cc` is now removed from the direct-core-edge list. Its retained
SoftPC-facing status and result ABI calls the fixed-width
`adapter-bochs/machine_facade.{cc,h}` directly. The facade is the sole source
that accesses the native PC A20 line; no app callback binding or Bochs object
is exposed to `adapter-softpc`.

The formal graph rebuilt the 91 affected actions successfully.  Both
`t226-s4-a20-mantle-fixture` and
`t237-s3-xms-a20-source-mirror-fixture` exited `0`; its final dry run was
`no work`.  The wider `t255-s2-generic-context-resume-fixture` observed exit
`2` in this run; it is not used as evidence for this A20 seam and remains in
the later CPU/protected-range migration set.

### Ordinary-RAM seam

The same facade owns checked ordinary-RAM range/read/write operations.
`mechanical_action.cc`, `ordinary_ram_reservation.cc`, `ivt_watch.cc`, and
the opt-in provenance branch of `instruction_history.cc` no longer import a
Bochs header or use `bx_mem`.  `machine_facade` is the sole owner of the
native memory calls. `adapter-softpc` sees only the fixed-width facade ABI.

The `t256-s8-ordinary-ram-reservation-fixture` and the existing XMS A20
source-mirror fixture both exited `0`; the formal Ninja dry run again reported
`no work`.

### CPU/timer facade closure

`finite_run.cc` and `machine_stage.cc` now use the same facade for lifecycle,
ordinary RAM, CS:IP state, timing, CPU loop, and typed stop request;
`protected_range_action.cc` uses it for protected-mode range execution. The
timer callback carries only an opaque pointer and a selector-blind completion
signal; it is registered by the facade and does not expose a Bochs timer or
object to `adapter-softpc`.

The refreshed formal Ninja graph compiled and linked the affected 96 actions.
`t226-s4-a20-mantle-fixture`, `t237-s3-xms-a20-source-mirror-fixture`,
`t256-s8-ordinary-ram-reservation-fixture`, and
`t242-s2-machine-stage-reset-fixture` each exited `0`; final `ninja -n`
reported `no work`. The pre-existing
`t255-s2-generic-context-resume-fixture` continues to exit `2`, as it did
before this dependency repair; it is an already-admitted CPU context-resume
diagnostic, not evidence for or a regression in this facade migration.
