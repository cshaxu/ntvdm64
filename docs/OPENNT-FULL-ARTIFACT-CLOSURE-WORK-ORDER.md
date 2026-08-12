# OpenNT Full Artifact Closure Work Order

Status: build-campaign order, 2026-08-07. Scope is the 75 declared outputs in
`opennt-mvdm-artifact-inventory.json` plus outputs resolved from install rules.
This is not a claim that any staged file is retail-identical, distributable, or
runnable under the current research runner.

## Classification Rule

Every discovered output must have exactly one classification and one current
state:

| Classification | Meaning |
| --- | --- |
| NT4 DOS payload | `.SYS`, `.COM`, `.EXE`, NLS data, or companion required by the direct NTIO/NTDOS DOS profile. |
| VDM host/support | DEM, monitor, BIOS provider, XMS/DPMI/redirector, executable, DLL, or helper required outside guest memory. |
| WOW16 dependency | Win16 driver/runtime output; separate from DOS payload even when it supports mouse, keyboard, display, comms, or sound. |
| sample/development-only | VDD samples, tests, debug aids, and build-only tools. Never silently promoted to runtime input. |

The artifact record must state: managed source closure; EN-US generated input;
exact historical/compatibility toolchain; link/reloc/packaging state; output
hash/size; and runtime result. `source exists` and `object compiles` are not
terminal states.

Use `HISTORICAL-TOOLCHAIN-PROVENANCE-MATRIX.md` for the project-wide
historical-versus-compatible tool classification. Per-file records must still
name their exact assembler, linker, conversion/packing command, and any
stage-local source transformation; they cannot inherit a result merely from a
shared tool-family label.

## P0: Direct Command-Smoke Closure

These are the only guest payloads relevant before the first bounded
`COMMAND.COM /C` objective:

| Artifact | Current evidence | Next proof |
| --- | --- | --- |
| `NTIO.SYS` | managed source-to-image structural rebuild | original initialization-chain trace reaches the validated image origin; no synthetic BOP/BIOS substitute |
| `NTDOS.SYS` | managed source-to-image structural rebuild | original `SVC_DEMLOADDOS -> demLoadDos` load transition using a contained BYOB root |
| `COMMAND.COM` | unchanged-source OpenNT MASM 4.00/LINK16 rebuild verified; prior JWasm/modern-link diagnostics are retired | bounded command initialization only through the governed backend and original service ownership |
| message/locale inputs | EN-US message generation available for selected stages | manifest the exact generated inputs used by P0 artifacts |

P0 is blocked by the original host-chain/core-to-historical bridge gap, not by
another synthetic guest loader. The BYOB validator and transient image loader
are ready only for the pre-entry side of this gate.

## P1: First DOS Device And Compatibility Payloads

Their source/build closure is complete. Admit runtime behavior after P0 has an
original loader path, in this order:

1. `HIMEM.SYS`: managed source-to-image evidence exists; admit only after an
   original XMS trace proves the needed host/XMS service boundary.
2. `ANSI.SYS`, `KEYBOARD.SYS`, `COUNTRY.SYS`, `KB16.COM`, and `NLSFUNC.EXE`:
   group as console/input/NLS but keep each file and generated resource
   identity separate.
3. `SETVER.EXE`, `FASTOPEN.EXE`, `SHARE.EXE`, `APPEND.EXE`, `DEBUG.EXE`, and
   `EDLIN.EXE`: source builds exist but still require loader/runtime
   validation; `SHARE` remains a historical NT no-op, not evidence of
   file-lock support.
4. `REDIR.EXE`: remains opt-in until a contained namespace and its matching
   host redirector service are both source-closed.

No P1 program is an implicit dependency of the P0 command smoke profile.

## P2: Source-Limited Direct Guest Inputs

The prior toolchain blockers for `DOSX.EXE`, `NLSFUNC.EXE`, `MEM.EXE`,
`LOADFIX.COM`, and `GRAPHICS.COM` are superseded by the current pristine
OpenNT tools16 stages. Their runtime behavior remains unvalidated, but they
are not source-build blockers.

The remaining direct guest limitation is source availability, not a guessed
toolchain repair:

| Family | Current disposition | Required treatment |
| --- | --- | --- |
| `BACKUP.EXE`, `QBASIC.EXE`, `MSHERC.COM`, `EDIT.COM` | fixed tree has only explicit binplace ownership; no complete matching source-build rule is present | retain as hash-manifested external research references, never as source-built outputs |

## P3: Full Inventory Reconciliation

The remaining descriptors need an explicit record even where no `dest=` line
exists. The direct command/device owners for `backup`, `graphics`, `qbasic`,
`country`, and `quit` are now resolved in
`OPENNT-MVDM-DECLARED-OUTPUT-TRIAGE.md`. Continue with message/resource and
data-only companions; SIM16/SIM32, V86 utility, and redirector output names;
`bin86` entries that only place external product inputs; and every host/support
target used by the fixed NT4 VDM path.

Each result is recorded as `built`, `blocked`, `external input`, or
`sample/development-only`. No output is omitted just because it is not needed
by P0.

## P4: Host/WOW Pairing

The historical guest set is not complete without recording its host pairings:

- BIOS/SoftPC, CCPU/monitor, DEM, command, XMS, DPMI, redirector, and `ntvdm`
  executable targets are VDM host/support, not DOS payloads.
- `VDMREDIR` and `REDIR.EXE` are a paired optional feature, admitted only with
  namespace containment evidence.
- `MOUSE.DRV` and the WOW16 keyboard/display/comms/sound drivers remain WOW16
  dependencies. They must not be called DOS `.SYS` files or added to the P0
  profile.

## Delivery And Verification Order

1. Regenerate and diff the descriptor inventory from the fixed OpenNT source
   revision before every campaign batch.
2. Preserve the completed P0 source/tool/layout records while separately
   progressing governed backend adoption and the original service oracle.
3. Run P1 only when each item's live trace reaches the corresponding original
   service owner.
4. Attack P2 blockers by recovering historical contracts, never by swapping in
   unrelated tools or guessed binary layouts.
5. Reconcile P3/P4 until every declared output has a classification and state.
6. Emit a local, non-distributed artifact manifest only after the per-file
   records are complete.

The final research artifact set stays outside source/release inputs. Any BYOB
profile consumes owner-selected files through identity validation; it never
copies these files into the repository or a normal product package.
