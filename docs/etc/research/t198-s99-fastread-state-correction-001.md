# T198 S99 DEMFASTREAD state correction 001

## Correction

S98 incorrectly read the decimal literal `66u` as unrelated to service
`0x42`.  They are the same value: the current DEM plane already classifies
`SVC_DEMFASTREAD` as `NAMESPACE`.  The `42u` in the `RAW_MEDIA` group is
decimal 42 (`0x2A`), not `0x42`.

This record supersedes S98's classification-regression conclusion.  S98 is
retained as historical evidence of the failed hypothesis rather than silently
rewritten.

## Verification

The current composition test now explicitly establishes an existing namespace
handle, invokes `C4 C4 50 42` with ZF clear and `BX:SI=0`, and verifies the
existing fast-read provider produces a checked three-byte `REM` bulk write
with `AX=3`.  It then invokes `50:43` and verifies it remains pass-through.

`tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1` source-built, linked and
ran this closure as MSVC x64 `/MT` at
`artifacts/build/t198-s99-fastread-state-r2` with zero exit.

## Consequence

S97's `50:42` stop cannot be attributed to plane classification or provider
reachability.  The provider accepts only an already-open immutable-namespace
handle encoded in `AX:BP`; its ZF-clear branch also seeks to `BX:SI`.  The
next source-only task must map the actual NTDOS handle lifecycle and copied
`AX:BP` state for the S97 call before any provider change or guest rerun.
