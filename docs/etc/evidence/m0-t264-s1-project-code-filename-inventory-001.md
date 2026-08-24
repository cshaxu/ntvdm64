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
