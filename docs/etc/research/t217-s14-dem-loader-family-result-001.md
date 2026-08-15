# T217 S14 — DEM Loader Handle-Family Result

## Scope

This record closes the bounded source-derived regression for the composed DOS
image-loader handle family.  It covers one declared, immutable `C:\COMMAND.COM`
image through `50:12` open, `50:00` seek, `50:42` FastRead compatibility,
`50:16` ordinary read, and `50:02` close.  It does not claim DOS EXEC,
COMMAND execution, a native trace, or host filesystem mutation.

## Original Sources And Contract

| BOP | Original owner | Retained fact |
| --- | --- | --- |
| `50:12` | DOS `$Open` / `demOpen`, `dos/v86/doskrnl/dos/file.asm`, `dos/dem/demfile.c` | `DS:SI` path opens an opaque host/declared-image handle and returns the split token. |
| `50:00` | DOS seek / `demChgFilePtr`, `dos/v86/doskrnl/dos/handle.asm`, `dos/dem/demhndl.c` | Origin is `BL`; signed displacement is `CX:DX`; the result is `DX:AX`. |
| `50:16` | DOS read / `demRead`, `handle.asm`, `demhndl.c` | `AX:BP` remains the token; `ZF` from `test sf_nt_seek` controls whether `BX:SI` repositions before the read. |
| `50:02` | DOS close / `demClose`, `handle.asm`, `demhndl.c` | A valid token closes with carry clear; stale tokens fail without becoming host handles. |
| `50:42` | DOS `FastOrSlow` caller in `handle.asm`; OpenNT `demdisp.c` table names `demNotYetImplemented` | It is an optimization using the same copied token/position/count/destination ABI as read, never an independent filesystem service. |

The original DEM C translation units are not directly composable with the
modern x64 CLI because they require CCPU/SAS/SoftPC state and the NT4 product
shell.  The retained step-(b) seam is the existing `bx-vdm` package/session
with checked mechanical reads/writes.  No Bochs or external-code intrusion is
needed, and no new filesystem semantics were authored.

## Regression

`tests/bx-vdm/bx_ntvdm_dem_loader_family_v1_test.c` intentionally uses a
two-byte declared image.  It proves, in one session:

1. `50:12` returns one opaque token for `C:\COMMAND.COM`.
2. `50:00` seeks that token to zero with zero `CX:DX` displacement.
3. `50:42` reads the first byte; `50:16`, with ZF retained as the original
   `sf_nt_seek` contract requires, reads the next byte rather than reopening
   or resetting the position.
4. `50:02` closes the same token, and a forged later `50:42` receives
   DOS `ERROR_INVALID_HANDLE` with carry set.

The fixture has no host namespace, so it cannot read, create, modify, or
delete a host file.  It cannot prove a normal guest target execution.

## Reproducible Evidence

The tool source closure was extended with the `dem-loader-family` fixture and
built with MSVC x64 `/MT`.  The current retained result is:

```text
artifacts/build/t217-s14-dem-loader-family-r3/t217-s14-dem-loader-family.json
runExitCode: 0
passed: true
stackReserveBytes: 8388608
```

The first invocation generated the declared closure command file; the same
command file was then completed and linked with its recorded MSVC environment.
The result contains its source closure, object closure, link map, executable,
and JSON outcome.  This is a source-built fixture, not a native Bochs run.

## Out-Of-Scope Finding

The older all-DEM identity fixture currently returns `197` at its independent
overlay `50:1A demSetDefaultDrive` expectation before reaching any loader
family assertion.  That fixture is not used as S14 acceptance because its CWD
precondition belongs to the separately owned DEM session-context package.
The failure is recorded for later owner-package review; S14 neither changes
that provider nor masks the failure.

## Disposition

The composed image-loader handle family is closed at the declared-image
provider boundary.  A single native trace may now be admitted only as
integration observation and only under a newly selected packet; it cannot
select the next BOP implementation or establish COMMAND/EXEC completion.
