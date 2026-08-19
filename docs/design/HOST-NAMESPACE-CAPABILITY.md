# Host Namespace Capability

## Purpose

This capability projects actual host logical drives and their directories into
the adapter's DOS-facing service families under an explicit CLI policy. It is
not a Bochs device or an extension of BYOB guest-artifact admission.

## Ownership

| Layer | Owns | Must not own |
| --- | --- | --- |
| CLI | Parses and validates the host-drive policy and operation profile | Guest-memory access, DOS wildcard interpretation, Bochs options |
| Host capability seam | Uses ordinary user-mode Win32 APIs for the selected real drives, paths and current-directory state | Guest selectors, BOP/service selection, Bochs internals, host installation or system-file modification |
| Adapter host-service plane | Maps an admitted real drive to opaque capability state; implements source-derived search, path and result rules | Bochs pointers or unapproved host capability |
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

During session admission, the host capability seam captures the real logical
drive inventory and applies the policy before adapter installation. The
current BYOB artifact root is not an implicit filesystem namespace root:
NTIO, NTDOS and COMMAND are loaded as guest-memory startup inputs and are not
copied or materialized as `IO.SYS`, `MSDOS.SYS`, `COMMAND.COM`, `CONFIG.SYS`
or `AUTOEXEC.BAT` on a real host drive.

## Host-storage capability

The sole product capability maps each admitted logical host drive under the
same DOS drive letter and recovers the applicable original OpenNT host API
behavior where that owner package can be composed. It may perform ordinary
user-authorized host mutations; this is still non-invasive because NTVDM
itself neither installs nor modifies Windows. No project-authored `readonly`,
`overlay`, or `virtual` mode is part of the product contract. Such retained v1
material is historical comparison code, not selectable or extensible
capability.

VDM startup does not require any projected host drive: NTIO, NTDOS, COMMAND,
CONFIG/AUTOEXEC and the declared target are immutable guest-memory inputs.
A profile's `command_placement.drive_index` is a guest DOS placement, not a
requirement to project the same host letter. If a later guest operation selects
an excluded host C: (or any excluded drive), the selected OpenNT service
returns its source-derived unavailable/path failure; admission does not reject
startup merely for that exclusion. Read, directory enumeration, path
normalization and current-directory behavior are selected host capabilities,
not ambient guest access.
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

No new Bochs intrusion is proposed. Host-drive enumeration occurs at
CLI/adapter session admission under the explicit user policy, never in Bochs.
Implementation requires source-derived field/error tables for FCB, DTA,
path/current-directory and every reached OpenNT mutation/error path. A shim
may adapt only unavailable historical host-composition mechanics; it must not
introduce a separate mutation policy.
