# M0 T282 S13 COMMAND current-directory bounded binding

## Question

Can the selected original `cmdGetCurrentDir` source compose on both supported
host architectures after its only unsafe guest-pointer lifetime is replaced by
the already-admitted bounded lease form?

## Inputs

- Original mirror: `src/opennt-mvdm-host/dos/command/cmdmisc.c`.
- S9 span ledger: `DS:SI`, maximum
  `MAXIMUM_VDM_CURRENT_DIR + 3` (67) bytes.
- S10 register facade and S12 numeric guest-location/copy operations.

## Procedure

1. Replaced this function's `GetVDMAddr(DS, SI)` alias with a numeric real-mode
   descriptor.
2. Preserved source drive validation, OEM environment lookup, root fallback,
   CF/AX outcomes and environment-update ordering.
3. Staged the source result in a 67-byte local buffer and issued a fresh exact
   synchronous write only after the source result was available.
4. Generated and ran `New-T282OriginalCommandSurfaceNinja.ps1` plus Ninja for
   x86 and x64 under MSVC `/MT`.
5. Generated and ran `New-T282CommandBoundedCopyNinja.ps1` plus its `test`
   target for x86 and x64.

## Observations

- Both original-COMMAND archive graphs built `cmdmisc.obj` and
  `original-command-surface.lib` successfully.
- Both bounded-copy fixture targets exited successfully.
- The original translation unit still emits historical C runtime and pointer
  conversion warnings in unrelated functions and branches. This packet neither
  suppresses them nor treats static compilation as a runtime `54:04` route.

## Interpretation

The only admitted `cmdGetCurrentDir` pointer is now a bounded, synchronous
write through the existing session guest-memory mapping-manager instance. No
native guest pointer survives the function or crosses an adapter boundary.
The local buffer is a transport replacement for pointer lifetime only; the
original algorithm and visible success/failure order remain in the mirror.

## Follow-up

`cmdComSpec` and `cmdGetInitEnvironment` remain unmodified until separately
admitted from their S9 records. A runnable BOP/guest route is not evidence
provided by this source-composition packet.
