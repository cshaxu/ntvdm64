# M0 T299 S4 — first-degree translation-unit closure plan

## Purpose

Correct the graph rule used by T299 S3. A physical original implementation
file entered directly from a zero-degree call is part of the first-degree
translation-unit set. Its actually reached same-file helper definitions, and
an externally-linkable definition reached in any other file already in that
set, belong to the same degree. They are not second-degree merely because one
first-degree source function calls them.

## Required result

- Preserve one identity for every concrete `source-root/path/hash/line/signature`
  definition. Neither matching spelling nor matching bytes permits merging.
- Keep every zero-degree direct call with no original non-public body as its
  own `missing-original-definition` record.
- Starting from each direct first-degree definition, take the transitive
  closure over direct named calls whose definitions are in the selected
  physical-file set. Cross-file selection requires an externally-linkable
  definition; `static` definitions remain local to their own file.
- Emit second-degree candidates only for calls that leave the selected
  physical-file set and are not already known zero- or first-degree identities.
- Reproduce the definition, direct-call mapping, spelling index and
  second-degree ledgers from original OpenNT/OpenNT-4.5 sources only.

## Exclusions

This is graph normalization only: no current mirror, adapter, `src.old`, build
input, provider selection, or runtime behavior may be added.
