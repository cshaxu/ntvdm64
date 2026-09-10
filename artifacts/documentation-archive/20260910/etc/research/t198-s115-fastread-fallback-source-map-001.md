# M0 T198 S115: Fast-Read Fallback Source Map

## Established Identity

The S114 accepted fast-read result is AX=`000Dh`. The current profile's
immutable namespace defines exactly one 13-byte file:

```text
CONFIG.SYS = "REM NTVDM64\r\n"
```

`bx_ntvdm_readonly_namespace_v1_initialize` installs that byte array as file
index 1. This agrees with the prior current-composition observation, which
recorded fast reads of 13 bytes (CONFIG), 62 bytes (finite boot namespace),
50,384 bytes (COMMAND.COM), then EOF.

## Why the Fallback Is Not Yet Attributable

The fallback record contains `AX:BP = 4E53:4005`, i.e. namespace token
`0x4E534005`, count `0x20`, ZF set and `DS:DX` destination. The namespace
increments its generation at each successful close and its fast-read provider
can decline before a result for several distinct reasons: no open file,
generation mismatch, span rejection, or provider routing not selected.

The current fixture records only the first declined event after the
composition has returned false. It records neither an attempt ordinal nor an
accepted/declined sequence. Therefore the available evidence cannot tell
whether the retained call occurred before a later open, after a close, or on a
distinct NTDOS resource. Assigning it to COMMAND, AUTOEXEC, TARGET or a Bochs
defect would be speculation.

## Disposition

S116 may add a fixed-capacity test-only attempt ledger at the already existing
composition return boundary. It must retain only copied event and typed-result
metadata, no guest contents, host namespace pointers or execution semantics.
No guest run occurs in S116.

## Evidence

- `src/bx-vdm/bx_ntvdm_readonly_namespace.c:5,7-8,39-50,120-155`
- `src/bx-vdm/bx_ntvdm_dem_readonly_file_service.c:135-153`
- `docs/etc/research/t155-s1-fastread-current-composition-observation-001.md`
- `docs/etc/research/t198-s114-fastread-postcommit-native-observation-001.md`
