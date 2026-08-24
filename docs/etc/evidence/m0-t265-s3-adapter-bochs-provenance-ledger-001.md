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

## Remaining S3 disposition

- `pc_system.{cc,h}`: upstream root material, with `pc_system.cc` a true
  subset that omits product reset/teardown.  Its present header is also
  consumed directly by `adapter-softpc`; this is an existing forbidden
  production edge under T265/S1 and must be removed through the approved
  app/session machine-wiring boundary before final relocation.
- `keyboard.{cc,h}`: adopted `iodev` controller with headless product-shell
  omissions and local teardown.  It requires a per-hunk mirror/overlay
  disposition before moving.
- `pic.{cc,h}`: adopted `iodev` PIC with a project factory/teardown body.
  The factory is new executable mechanics, so the body requires the S4
  overlay disposition rather than an unmarked mirror move.
- `headless_8042.{cc,h}`, `minimal_machine.{cc,h}`,
  `minimal_port_space.cc`, `minimal_product_shell.cc`, and
  `minimal_sim.{cc,h}`: no upstream file identity.  They remain the only
  candidate `adapter-bochs` self-authored, Bochs-only assembly files, pending
  the complete S3 vocabulary/provenance sweep.

No BOP, OpenNT, DOS, VDM, WOW, SoftPC/CCPU, or Win32 semantics were added.

## Required dependency repair

The active source scan finds the following `adapter-softpc` files importing a
Bochs production header or accessing a Bochs global directly:

- `a20_capability.cc`;
- `finite_run.cc`;
- `instruction_history.cc`;
- `ivt_watch.cc`;
- `machine_stage.cc`;
- `mechanical_action.cc`;
- `ordinary_ram_reservation.cc`; and
- `protected_range_action.cc`.

This is not a license for an exception: it is an existing violation of the
T265/S1 edge rule.  S3 must replace those direct edges with an
app/session-bound copied-data mechanical callback contract, then move the
remaining upstream `pc_system`, keyboard and PIC material according to the
mirror/overlay rule.  Until that is done, this ledger is P1/P2 evidence only
and S3 is not closed.
