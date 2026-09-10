# T225 S8 Primary COMMAND Source-Build Result 001

## Scope

This record restores one of the four required primary source-built guest
inputs for M0 T225 S8.  It is provenance evidence only.  It neither runs a
guest nor admits a BOP, Bochs, or host-capability change.

## Reproducible Stage

Run:

```powershell
& .\tools\build\Build-T225S8PrimaryCommand.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T225-S8\command-primary-source-build-r6
```

The stage manifest is
`build/M0-T225-S8/command-primary-source-build-r6/command-build-manifest.json`.
It records the managed `src/opennt` revision, canonical OpenNT 4.5 historical
tool identities, all 34 assembled COMMAND modules, and the output identity.

## Result

| Field | Value |
| --- | --- |
| Classification | `primary-original-toolchain-source-built` |
| Source | `src/opennt`, revision `5c5b979ec08c17d3ca2eb70e8aad62d26515d01c` |
| Message generation | Original `BUILDMSG.EXE` over `usa-ms` and `command.skl` |
| Compilation/link | Original OpenNT 4.5 `MASM.EXE` and `LINK16.EXE` |
| Packaging | Original OpenNT 4.5 `RELOC.EXE` |
| Output | `base/mvdm/dos/v86/cmd/command/COMMAND.COM` |
| Bytes | 50,384 |
| SHA-256 | `908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43` |

The result matches the locked primary COMMAND identity exactly.  It is a
physically separate primary build stage, not the approved reference sequence.

## DOSBox Completion Contract

The original `BUILDMSG` tool does not reliably return control to a trailing
batch-file marker.  `Invoke-DosBoxBatchWithCompletion.ps1` therefore accepts
an explicit contained output set, waits for every declared output to remain
size-stable for five seconds, then terminates only the DOSBox instance it
started.  Every caller independently validates its final artifact bytes and
SHA-256.  This is a bounded host build-driver mechanism, not a guest runtime
or artifact fallback.

## Remaining Gate

NTIO, SHARE, and COMMAND now have primary manifest-bearing stages.  NTDOS is
still required before the primary Direct/Readonly installation comparison is
admitted.
