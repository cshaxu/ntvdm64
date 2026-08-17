# T225 S8 Archived NOSRVBLD Extractor Rejection 001

## Question

Can the retained evidence-only `nosrvbld_class_extract.c` become the
source-derived fallback for the unavailable OpenNT-compatible NOSRVBLD tool?

## Procedure

The file was compiled unchanged in a disposable directory with MSVC x64.  A
single `_CRT_SECURE_NO_WARNINGS` compiler compatibility definition was needed
to suppress the archived source's legacy CRT-deprecation diagnostics; no source
file was changed.  It was invoked separately for classes 3 through 6 using
the manifest-managed `msbio.skl` and `usa-ms.msg` inputs.  Each output was
hash-compared to the corresponding managed OpenNT `MSBIO.CL*` file.

## Observation

None of the four outputs matched.

| Class | Extractor bytes | Required bytes |
| --- | ---: | ---: |
| CL3 | 1,233 | 1,339 |
| CL4 | 96 | 104 |
| CL5 | 140 | 152 |
| CL6 | 196 | 211 |

The helper is therefore neither an exact byte producer nor a harmless
format-only wrapper for the required NTIO class contract.

## Disposition

The helper remains `trace-or-tool evidence` under the local-code governance
ledger.  It is rejected as an S8 build input and is not copied, moved,
modified, linked, or promoted.  Together with the tracked DOS NOSRVBLD audit,
this exhausts the currently retained local candidates without authorizing a
new generator.

## Follow-up

The NTIO primary stage requires recovery of an OpenNT-compatible NOSRVBLD
implementation/tool or a newly proposed source-derived generator with a full
four-class equivalence contract.  The latter is a new governed build-support
component, not an adapter, BOP, or Bochs change.
