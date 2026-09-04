# M0 T391 S6 — Rejected First-PermCom Child-Disposition Source Map

> Superseded by `m0-t391-s6-image-disposition-and-same-vdm-exec-001.md`.
> The earlier “DOS product child” interpretation below is retained as dated
> investigation history only. Original guest `msproc.asm::$EXEC` proves that
> recognized DOS COM/MZ images execute in the current VDM; this document is
> not acceptance evidence for a COMMAND child-launch adapter.

## Result

The first resident `COMMAND.COM` is not treated as an app-owned launcher.
Its initial record is delivered through the original COMMAND/BaseVDM sequence:

```text
IsFirstCall = TRUE
  -> cmdGetNextCmd: ASKING_FOR_FIRST_COMMAND
  -> GetNextVDMCommand initial record succeeds
  -> cmdGetNextCmd clears IsFirstCall
  -> guest executes the initial image
  -> PE: cmdCheckBinary writes /z command tail
  -> guest BOP reaches original cmdExec
  -> original cmdExec32 worker / result path
```

`cmdCheckBinary` rejects a PE only while `IsFirstCall` remains true, which is
the source-defined Autoexec protection. That condition is already false for
the first PermCom's delivered initial record.

## Source Evidence

- `src/mvdm-host/dos/command/cmdmisc.c:126-140` sets
  `ASKING_FOR_FIRST_COMMAND` while `IsFirstCall` is true.
- `src/mvdm-host/dos/command/cmdmisc.c:292-305` calls
  `GetNextVDMCommand` and then clears `IsFirstCall` before returning to guest
  COMMAND execution.
- `src/mvdm-host/dos/command/cmdexec.c:280-289` preserves the original
  first-call-only PE refusal.
- `src/mvdm-host/dos/command/cmdexec.c:291-359` preserves the original
  `PE -> /z -> guest command tail` construction.
- `src/mvdm-host/dos/command/cmdexec.c:738-786` now performs one bounded
  source-facing child disposition before either original `cmdExec` form:
  direct `AH=0` or `COMSPEC /c` `AH=1`.

## Disposition Boundary

`adapter-mvdm-host-out/command/mvdm_command_child_vdm` examines only a copied,
resolved simple host image:

- DOS COM/MZ: starts the current product executable with its ordinary
  positional target and copied arguments. The child uses the existing initial
  BaseVDM/PermCom route; app receives no new child-launch syntax.
- Win16 NE: returns the explicit bootstrap-gated unavailable disposition.
- PE, host built-ins, compound syntax, unresolved paths: decline and retain
  the original source-selected direct or `COMSPEC /c` route.

Neither adapter nor app retains guest memory, mapping-manager IDs or native
handles across the child process boundary. Original `cmdExec32`,
`cmdCreateProcess`, worker/re-entry and `54:0B` remain the parent owner.

## Local Verification

- Focused x86 fixture:
  `node tools/build/Generate-T391CommandChildVdmNinja.mjs . build/M0-T391/S6/command-child-vdm-x86`, followed by the generated MSVC/Ninja fixture,
  compiled with `/W4 /WX` and exited successfully. It covers simple DOS with
  arguments, NE unavailable, PE decline and compound decline.
- A complete formal CPU40/x86 rebuild succeeded in
  `build/M0-T391/S6/formal-x86-child-vdm-r1`, including original COMMAND,
  the child-disposition adapter and final product link.

The formal x86 candidate is staged only as
`build/output/ntvdm32.exe` and `O:\ntvdm64\ntvdm32.exe`: both are
`3158016` bytes with SHA-256
`13f8eccb529960bf03b096706b36b2cf625a7460e8c1123a65090b1396f53e68`.
The separately retained x64 product slot is `build/output/ntvdm64.exe` and
`O:\ntvdm64\ntvdm64.exe`: both are `3761152` bytes with SHA-256
`5772261a00f9b5d42135267312416db5c02ef48248f016c4e6065d45c2a4f173`, from
the successful `build/M0-T391/S6/formal-x64-product-r1` final link. Generic
`ntvdm.exe` and task-specific `ntvdm-child-route.exe` were removed from both
publication roots. The root contains no JSON/TXT/LOG/MAP/PDB diagnostic file;
such records belong below `O:\ntvdm64\logs`. Runtime confirmation still
requires an interactive guest `COMMAND.COM` input of `MEM.EXE`, followed by
observation of the parent prompt and original child-result return.
