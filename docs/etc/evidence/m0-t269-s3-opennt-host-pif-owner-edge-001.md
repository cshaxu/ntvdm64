# M0 T269 S3 — OpenNT PIF parser owner-edge disposition

## Question

How can the current COMMAND PIF composition stop including a source body from
`opennt-host` without rewriting the original parser or exporting an invented
cross-component PIF ABI?

## Inputs

- Exact selected OpenNT parser:
  `O:\repos.external\OpenNT\base\mvdm\softpc.new\host\src\nt_pif.c`,
  SHA-256 `4AE2C1D2C4A66DC63C9B2DDD673FCAE067F2FBDAB953DC8D96AECFCF03C262BC`.
- Current COMMAND `cmdpif.h`, `opennt_pif_composition.*` and the current
  component manifest/fixtures.
- T269/S1 baseline and the mirror/owner rules.

## Source and ABI findings

- The original parser's own `PIF_DATA` has no `fgprio` member. The separately
  original COMMAND `cmdpif.h` layout inserts `fgprio` after `idledetect`.
  They are therefore not safely linkable as one declared C type.
- The prior composition contained the source directly, renamed its private
  parser names at translation-unit scope and copied the caller state before
  parsing. That containment is the existing minimum data-layout isolation;
  exporting a new host parser ABI would require a new marshaling contract and
  would be a larger self-authored replacement.
- The parser has no independent current host consumer or host-library build
  object. Its only production consumer is the COMMAND/BOP PIF composition.

## Disposition

The exact parser is moved, with `git mv`, to
`src/opennt-bop/softpc.new/host/src/nt_pif.c`. Its original relative OpenNT
path and full content are retained. `opennt_pif_composition.c` now includes
that same-component body by `../softpc.new/host/src/nt_pif.c`; it no longer
reaches across to `opennt-host`.

This is a component-owner transfer, not a source modification or an interface
replacement. The component-local translation-unit include is retained only for
the pre-existing incompatible historical data-layout isolation and is indexed
as `BOP-DIV-032`. It is not a precedent for cross-component source inclusion.

## Rejected alternatives

1. **Keep the former cross-component include:** violates unique production
   ownership and leaves host conformance incomplete.
2. **Compile `nt_pif.c` into `opennt-host.lib` and directly link it:** exposes
   the incompatible `PIF_DATA` ABI to COMMAND and changes field offsets after
   `idledetect`.
3. **Create a new host-to-COMMAND PIF transport ABI:** would add a self-authored
   interface and duplicate structure marshaling instead of preserving the
   original contained parser route.

## Verification plan

The completed change must prove byte identity at the destination, no remaining
cross-component `opennt-host/*.c` include, a successful formal Ninja host/BOP
link and the existing direct PIF fixture result. The latter verifies only the
already reached parser route; it does not claim new PIF capability.

## Verification result

- The destination hash exactly matches the selected original hash.
- A complete source/include scan found no cross-component inclusion of an
  `opennt-host` C body. The one remaining `#include "../softpc.new/host/src/nt_pif.c"`
  is inside `opennt-bop`, its declared sole owner.
- A fresh formal Ninja graph completed 486/486 edges. The existing direct PIF
  fixture passed its title, command, directory, target, close-on-exit and
  CONFIG/AUTOEXEC checks; the focused host mirror fixture also passed. A
  subsequent Ninja dry-run reported no work.
- The broader `t231-s7-command-get-next-direct-fixture` made no CPU progress
  and was stopped after its PIF-specific counterpart had passed. It is a
  separate COMMAND lifecycle limitation; this S3 neither attributes it to nor
  masks it with the owner transfer.
