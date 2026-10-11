# M0 T443 S5 — COMSPEC fallback retain disposition

## Question

Does run16 need a second project-owned classifier to distinguish CMD built-ins,
batch/script syntax and failed image-like command text before choosing its
`COMSPEC /c` fallback?

## Inputs and procedure

Reviewed the public run16 entry in `src/run16-exe/main.c`, its shared
application search (`src/common/application_search.c`), the original
`OpenNtBaseGetBinaryTypeW` contract in
`src/opennt-host/base/win32/client/vdm.c`, and S3's accepted COMMAND tail
route.

## Observation

The run16 entry identifies a possible first target only to select a known
DOS/WOW/native image route. If no classification succeeds, it preserves the
complete original command text and starts Hooked `COMSPEC /c <text>` through
the existing NTVWM path. It does not parse CMD metacharacters or maintain an
internal-command list.

Original `GetBinaryTypeW` reports a file-not-found/unknown-image failure to
its direct caller. That is not a command-shell parser. Original COMMAND
separately constructs `COMSPEC /c <tail>` for command text; S3 preserved that
route. The project public `run16` command-text fallback intentionally follows
the latter contract for every unclassified input.

## Disposition

Retain the existing single fallback. Splitting explicit paths, extensions,
bare names or CMD built-ins would introduce project-owned shell/file-intent
heuristics and violate the T443 simplification objective. No production source
or release image changed for S5.

## Acceptance

The owner explicitly accepted the single-route semantics on 2026-10-10:
classified images use their existing routes; all other command text is passed
unchanged to Hooked `COMSPEC /c`. S5 is closed as a no-change audit.
