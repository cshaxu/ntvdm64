# M0 T270 S1 P2 — Bochs-core mirror disposition ledger

## Baseline and method

The selected original is the pinned local Bochs 2.6 tree at
`O:/repos.external/bochs-2.6-compat/bochs-2.6`.  The audit compared every
non-README production file below `src/bochs-core` after normalizing line
endings only.  A changed file was then checked for a local `DIVERGENCE(...)`
marker, a matching component README index entry, and a central intrusion
record.  This is an ownership audit, not a feature or runtime claim.

## Identity result

- 205 baseline-matched production units were examined: **177 exact** and
  **28 changed**.  No adopted production file lacks a corresponding pinned
  original.
- The changed units total 417 added and 351 removed lines.  The largest
  retained normalized delta is `memory/memory.h` at 18.5 percent of its
  original line count; no retained source exceeds the 50-percent mirror-rule
  limit.
- Every one of the 28 changed units has a local `DIVERGENCE(...)` marker and
  an entry in `src/bochs-core/README.md`.  Their detailed exception evidence
  remains in `docs/etc/research/adapter-external-intrusion-exceptions.md`.

## Changed-file disposition

- **Root/build:** `bochs.h` is an include-root spelling correction;
  `pc_system.cc` is a registered true subset that omits the uncomposed full
  product-shell lifecycle.
- **CPU profile and observation call boundaries:** `cpu/access.cc`,
  `access32.cc`, `cpu.cc`, `cpu.h`, `ctrl_xfer16.cc`, `ctrl_xfer32.cc`,
  `exception.cc`, `icache.cc`, `init.cc`, `instr.h`, `io.cc`, `soft_int.cc`,
  `stack.cc`, and `string.cc`.  Each keeps only an upstream-shaped predicate,
  declaration, or call boundary.  The new copied-state, observation and
  profile bodies live only in `bochs-core-overlay/cpu/`.
- **Toolchain/SIM header surface:** `cpu/i387.h`, `gui/paramtree.cc`, and
  `gui/siminterface.h` are registered include/lexical portability changes;
  they introduce no machine or OpenNT behavior.
- **Native device true subsets and narrow boundaries:** `iodev/devices.cc`,
  `iodev/iodev.h`, `iodev/keyboard.cc`, `iodev/keyboard.h`, `iodev/pic.cc`,
  and `iodev/pic.h`.  Full product-shell/plugin/port-lifetime bodies are
  either omitted as unreachable or private to `bochs-core-overlay/iodev/`.
- **Memory boundary:** `memory/memory.cc`, `memory/memory.h`, and
  `memory/misc_mem.cc` retain declarations/calls or the SIM-free subset;
  checked RAM and reset-window allocation bodies are private to
  `bochs-core-overlay/memory/`.

## Overlay and consumer result

- `bochs-core-overlay` contains the registered CPU, memory and iodev
  implementations only.  The overlay is compiled as a member of the
  `bochs-core` library; it has no independent public ABI.
- A source include scan found **no external overlay consumer**: no source
  outside `src/bochs-core` and `src/bochs-core-overlay` includes a
  `bochs-core-overlay` header.
- A provenance scan found **no imported Bochs body in `adapter-bochs`** after
  P1 moved its former finite port-space source to the matching private
  overlay.  `adapter-bochs` retains machine assembly and calls declared
  mechanical boundaries only.

## Compression decision

P1 removed the only identified misplaced imported body.  The remaining large
deltas are already true subsets or minimal native call/declaration edges; the
corresponding new executable bodies are in the private overlay.  This audit
therefore admits no additional move without changing the established private
Bochs interface or reintroducing omitted product-shell facilities.

## Verification state at P2

- The focused native-PIC boundary fixture passed in P1.
- The fresh formal Ninja graph compiled the moved port-space overlay object.
- P3 is responsible for fresh formal Ninja and executable-fixture closure;
  the interrupted P1 disposable graph is not treated as a pass.
