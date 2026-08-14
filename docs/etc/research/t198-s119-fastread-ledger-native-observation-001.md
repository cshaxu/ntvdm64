# M0 T198 S119: Exported Fast-Read Ledger Native Observation

## Result

The one admitted replacement native observation exported all four bounded
`50:42` attempts.  Its provenance declares `runs: 1`; the fixture's guest
run exit code is `2`, its already-defined controlled-observation result, not
a DOS or CLI program result.

| Ordinal | Token (`AX:BP`) | Request (`CX`) | ZF | Disposition | Returned `AX` |
| --- | --- | ---: | --- | --- | ---: |
| 0 | `4E53:4001` | `000D` | clear | accepted/resume `6355` | `000D` |
| 1 | `4E53:4003` | `003E` | set | accepted/resume `6355` | `003E` |
| 2 | `4E53:4003` | `FF00` | clear | accepted/resume `6355` | `C4D0` |
| 3 | `4E53:4005` | `0020` | set | declined | none |

The same log records the pre-existing bounded terminal marker
`next=50:42`, and the next observed BOP is `5F`.  The ledger does not read
guest bytes, retain a namespace pointer, or change runtime dispatch.

## Source-Grounded Classification

Attempt 0 is the immutable thirteen-byte `CONFIG.SYS` materialization:
`"REM NTVDM64\\r\\n"`.  This confirms S115's identification with a full
ordered record.

Attempts 1 and 2 share `4E534003`, so they belong to one open namespace
generation.  The clear-ZF second attempt is explicitly positioned by
`BX:SI`; its `C4D0` result is the full 50,384-byte `COMMAND.COM` image
already independently identified by T151/T155.  The preceding `003E` result
is a bounded read in that same stream, not a separate token/file conclusion.

Attempt 3 is a real declined call after the generation has advanced to
`4E534005`.  Generation advances only after a successful source-derived
close.  Consequently its token alone does not identify a file; it proves a
later open/close lifecycle must be reconstructed before any provider change.
S120 owns that OpenNT/adapter source map.  It must not infer an extra Bochs
device, DOS service or host capability from this record.

## Reproducibility

```text
powershell.exe -ExecutionPolicy Bypass -File tools/Invoke-T198S97PostBootstrapNativeProbe.ps1 \
  -RepositoryRoot O:\repos.hobby\ntdos64 \
  -BuildRoot O:\repos.hobby\ntdos64\artifacts\build\t198-s119-fastread-ledger-export-native-r1
```

- Probe record SHA-256: `9e63e4d3d2a37067ac5dcdc68dd034a54b172806411ffed335ca198d5cab038c`.
- `run.log` SHA-256: `743019e6b7c62780f9d1820878403ed4bbfc42e5e38497a89564ddc66adab787`.
- S117 remains build/path-continuity evidence only: it did not export the
  ledger and is not used for attempt attribution.

## Evidence

- `artifacts/build/t198-s119-fastread-ledger-export-native-r1/t198-s97-post-bootstrap-native-probe.json`
- `artifacts/build/t198-s119-fastread-ledger-export-native-r1/run.log`
- `src/bx-vdm/bx_ntvdm_readonly_namespace.c:5,100-155`
- `src/bx-vdm/bx_ntvdm_dem_readonly_file_service.c:58-61,130-153`
- `docs/etc/research/t198-s115-fastread-fallback-source-map-001.md`
