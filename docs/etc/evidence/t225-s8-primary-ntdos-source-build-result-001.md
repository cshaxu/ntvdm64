# T225 S8 Primary NTDOS Source-Build Result 001

## Scope

This record restores the final required primary source-built guest input for
M0 T225 S8.  It is provenance evidence only and does not run guest code or
change BOP, Bochs, or host-capability behavior.

## Reproducible Stage

```powershell
& .\tools\build\Build-T225S8PrimaryNtdos.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T225-S8\ntdos-primary-source-build-r1
```

The stage manifest is
`build/M0-T225-S8/ntdos-primary-source-build-r1/ntdos-build-manifest.json`.
It identifies the managed source revision, canonical OpenNT 4.5 tool hashes,
38 assembled modules, and final identity.

## Result

| Field | Value |
| --- | --- |
| Classification | `primary-original-toolchain-source-built` |
| Source | `src/opennt`, revision `5c5b979ec08c17d3ca2eb70e8aad62d26515d01c` |
| Message generation | Original `BUILDIDX.EXE` + `NOSRVBLD.EXE` over `msdos.skl` |
| Compilation/link | Original OpenNT 4.5 `MASM.EXE` and `LINK16.EXE` |
| Packaging | Original OpenNT 4.5 `RELOC.EXE` then `STRIPZ.EXE` |
| Output | `base/mvdm/dos/v86/doskrnl/dos/NTDOS.SYS` |
| Bytes | 27,858 |
| SHA-256 | `957662320654ad5251c3a8b228a5dadec28aa65dddbcba38c3658a6e7f93bc84` |

The output matches the locked primary NTDOS identity exactly.  LINK16 emitted
its historical `$READ` stack-size warning but returned a valid `NTDOS.EXE`;
the final `NTDOS.SYS` identity check is the acceptance gate.

## S8 Input State

The four physically distinct primary source-built manifest stages now exist:
NTIO, NTDOS, COMMAND, and SHARE.  The next S8 operation may stage only these
manifest-bearing outputs and run the already admitted Direct/Readonly
installation comparison.  The approved reference sequence remains separate
comparison evidence and is never a fallback.
