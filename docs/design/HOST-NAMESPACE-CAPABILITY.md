# Host Namespace Capability

## Purpose

This capability supplies a bounded host directory namespace to the adapter's
future DOS search family.  It is not a general host filesystem API, a Bochs
device, or an extension of BYOB guest-artifact admission.

## Ownership

| Layer | Owns | Must not own |
| --- | --- | --- |
| CLI | Parses and validates an explicit namespace descriptor/base; passes an immutable child-only launch selection | Guest-memory access, DOS wildcard interpretation, Bochs options |
| Host compatibility seam | Opens an approved non-reparse root and enumerates beneath it according to the descriptor | Guest selectors, BOP/service selection, Bochs internals |
| Adapter host-service plane | Maps an admitted drive to an opaque namespace handle; implements source-derived search state/token/result rules | Raw unrestricted host paths, Bochs pointers, device policy |
| Bochs | Generic typed read/write/result mechanics only | Names, files, roots, attributes, search state, drive policy |
| OpenNT guest | Existing FCB/DTA search client | Host path authority or emulator mechanics |

## v0 proposed descriptor

The host-drive visibility policy is selected by CLI arguments, distinct from
`--byob-profile` and `--byob-root`:

```text
--include-drives=c,d,e
--exclude-drives=e
```

Each value is a comma-separated set of DOS drive letters.  Parsing is
case-insensitive; an empty element, non-letter, duplicate option, malformed
separator or unsupported option is rejected.  `exclude` has priority over
`include`.  If no include argument occurs, all host logical drives are
initially eligible; if no exclude argument occurs, no eligible drive is
removed; if neither occurs, all host logical drives are eligible.  The host
compatibility seam snapshots the actually present logical drives once at
session admission and applies these masks before adapter installation.  It
does not rescan ambient drives while processing a guest request.

The CLI hands the adapter only the fixed-width inclusion/exclusion policy in
its child-only environment.  During adapter-session installation, the host
compatibility seam captures the real logical-drive inventory exactly once and
stores the filtered result as adapter-local immutable state.  The current
BYOB artifact root is never an implicit filesystem namespace root.

## v0 behavior

- Each admitted logical host drive is exposed under the same DOS drive letter;
  excluded or unavailable drives are not searchable.
- Read-only lookup/enumeration only; create, delete, rename, write and file
  handles are out of scope.
- A guest path is normalized and matched only within its selected admitted
  drive; it cannot select another root, escape through `..`, traverse reparse
  points or cause a volume change.
- Enumeration ordering is deterministic before guest-visible result creation.
- A generated 8.3 name collision is rejected at namespace admission rather
  than chosen nondeterministically at runtime.
- The adapter retains bounded opaque search tokens.  Tokens are validated on
  next, cleared on exhaustion/failure, and released on session termination.

## Required source-derived adapter transaction

For the FCB family, the adapter must first obtain the bounded guest path and
`SRCHBUF` state through generic ordinary-RAM reads.  After namespace lookup it
must prepare one atomic opaque multi-write containing the source-written
`SRCHBUF` fields and its opaque continuation cookie/ID, plus a checked CF/AX
result.  No partial guest result is permitted.  The paired path-style family
uses the same search session and must be admitted in the same design review.

## Non-goals and gates

No new Bochs intrusion is proposed.  No DOS filesystem mutation, file I/O, or
WOW behavior is admitted.  Host-drive enumeration occurs once at CLI/adapter
session admission under the explicit user policy, never in Bochs and never as
a guest-directed rescan.  Implementation requires a source-derived
field/error table for both FCB and DTA search families.
