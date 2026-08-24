# M0 T264 S1 — Project-code filename inventory

## Question

Which live project-authored or mechanical-adaptation code filenames in the
admitted component roots require identity normalization, and which similar
paths must remain untouched because they are historical source inputs rather
than project filenames?

## Inputs

- `src/app`, `src/session`, `src/adapter-bop`, `src/adapter-softpc`, and
  `src/adapter-win32` at admission revision `9afe4b92`.
- The T264 filename-normalization proposal and the source-mirror ownership
  rules.

## Procedure

The inventory recursively selected live `*.c`, `*.cc`, `*.h`, and `*.ps1`
files whose basename contains `ntdos64`, `ntvdm64`, `bx`, or `bochs`.
For each project-authored match, the approved rename rule removes the leading
`bx_ntvdm_` or `ntdos64_` identity prefix only; suffixes such as `_v1` remain
part of the local file contract. The paired include/build/fixture references
are consumers, not a reason to retain an old filename.

## Observations

- `app`: 38 rename rows, all project-authored startup, engine, state and CLI
  files; `bx_ntvdm_` and `ntdos64_` are redundant with the component owner.
- `session`: 4 rename rows, all project-authored session lifecycle files.
- `adapter-bop`: 4 rename rows, all project-authored selector-blind ingress
  files.
- `adapter-softpc`: 73 rename rows, all top-level project-authored mechanical
  bridge files. Their common `bx_ntvdm_` prefix is redundant with the adapter
  owner and is not an OpenNT/SoftPC original filename.
- `adapter-win32`: 0 matching live code filenames.
- Total approved project-code rename rows: 119.

The following are explicit exclusions, not omissions:

- `adapter-softpc/include/mvdm/**` and
  `adapter-softpc/include/softpc.new/**`: retained historical interface input
  paths. They are not project-authored top-level adapter code filenames and
  retain upstream identity until their source-owner migration is separately
  admitted.
- The six mirror roots and every `*-overlay` root: excluded by the owner
  request and mirror-source identity rules.
- Existing historical/legacy fixture directories not paired with an admitted
  project-code rename: retained for their evidence identity; S3 scans their
  direct consumers after production renames.

## Interpretation

The 119 rows can be renamed mechanically with `git mv` and a corresponding
exact include/build-path replacement. No public ABI or source symbol changes
are required. The inventory therefore admits S2 and S3; it does not authorize
renaming an imported historical header or any mirror file.

## Follow-up

S2 renames the 42 `app`/`session` rows and their direct runner references.
S3 renames the 77 adapter rows and their direct fixture/build consumers.

## S2 result

All 42 approved `app`/`session` paths were moved with `git mv`; 55 direct
source, fixture and formal-manifest consumers were updated by complete
filename-with-extension replacement only. A fresh formal graph at
`build/M0-T264-S2/r001` compiled to completion: a final dry run emitted no
work. The scoped filename scan found zero remaining prohibited basenames below
`src/app` and `src/session`. Retained old-path occurrences are confined to
historical probes, historical manifests, or disposable build output and are
not live S2 consumers.

## S3 result

All 77 approved `adapter-bop` and top-level project-owned `adapter-softpc`
paths were moved with `git mv`. Complete filename-with-extension replacement
updated 217 direct consumers in live source, tests and build input. This
included required include-path substitutions in a small number of Bochs and
OpenNT mirror files; it did not rename those mirror files or alter their
algorithms, symbols, ABI, or source-owned semantics. A fresh formal graph at
`build/M0-T264-S3/r001` compiled to completion and a final dry run emitted no
work. The scoped production filename scan is clean; retained matching paths
are excluded historical interface inputs or non-component historical evidence.

## S4 result

The final recursive scan of `src/app`, `src/session`, `src/adapter-bop`,
`src/adapter-softpc`, and `src/adapter-win32` found **zero** code basenames
containing `ntdos64`, `ntvdm64`, `bx`, or `bochs`. The 77 old adapter basenames
from the staged Git rename set produced **zero** direct references under live
`src`, `tests`, and `tools/build` inputs. The same scan intentionally excludes
the retained `adapter-softpc/include/mvdm/**` and
`adapter-softpc/include/softpc.new/**` historical interface inputs and all
mirror/overlay roots.

The S2 and S3 fresh formal-Ninja roots had already compiled to completion; a
subsequent dry run for each reported no work. Documentation governance and
`git diff --check` also pass. This closes the S4 filename-only migration:
no source symbol, ABI, behavioral, mirror-filename, or overlay change was
made in S2-S4. T264 was later reopened as S5 by owner direction.

## S5 P1 result — versioned filenames and private identifiers

Owner direction added standalone `v1`/`v2` filename segments to the prohibited
markers and required non-semantic project/backend/OpenNT identifier prefixes to
be reviewed rather than blindly retained.

- All 70 scoped `_v1`/`_v2` production filenames were moved with `git mv`.
  Their exact direct source/test/build filename consumers have zero remaining
  old-path references.
- The private `bx_ntvdm_*`/`BX_NTVDM_*` and `ntdos64_*`/`NTDOS64_*` code
  families were normalized to owner-neutral private names, including the
  former project-branded session types, constants, and cancellation test macro.
- OpenNT provenance prefixes were removed where they identified only private
  routing/helpers. Names remain where they identify an OpenNT-compatible
  facade, RTL/legacy API surface, or preserved provider contract; those are
  semantic compatibility names. DOS/VDM names remain only for guest semantics
  or a preserved historical interface, and `BX_*` names only for actual Bochs
  machine mechanics. Strings/comments and original-mirror include paths were
  not changed by the identifier sweep.

The fresh formal graph at `build/M0-T264-S5/r001` reaches MSVC, but its
synchronously waited Ninja run fails before a T264-owned target completes:
original Bochs FPU sources define `FLOAT128`, and current Windows SDK
`winnt.h` subsequently macro-expands its own `typedef ... FLOAT128`, producing
MSVC C4430 in `src/bx-core/fpu/{f2xm1,fpatan}.cc`. The disposable captured
external log is under that build root. This is genuine formal-build blocking
evidence, not a dry-run result; S5 and T264 therefore remain open pending an
owner-approved Bochs/toolchain repair packet.
