# M0 T245 S12 — Source-Built COMMAND Bootstrap-Path Disposition

## Question

Why does the source-built native run, after accepting the original
configuration-complete selector `5E`, enter `50:3D`; and is that selector a
missing BOP route or the original guest's terminal failure path?

## Original guest contract

OpenNT keeps the initial shell pathname in `sysinit2.asm`:

```asm
commnd  db "\\COMMAND.COM",0
```

After `sysinit1.asm:ConfigDone`, the guest frees its temporary command
allocation, sets `DX=offset commnd`, opens that path to size it, and finally
issues the original `INT 21h/AH=4Bh` `EXEC`.  Any open, seek, or EXEC failure
falls through `comerr`, which calls `badfil` and then emits
`SVC_DEMEXITVDM` at `stall`.

Thus `50:3D` is not a request to continue startup.  Its imported
`demmisc.c:demExitVDM` owner first reports `ED_BADSYSFILE` for `config.nt` and
then calls `TerminateVDM`; the current named shim converts that non-returning
operation into a typed bx-vdm controlled stop.  The S11 native observation's
terminal is consequently source-shaped.  It is not a generic-UD decline.

## Current composition comparison

`dem_v2_startup_composition.c` loads the source-built `NTIO.SYS`, `NTDOS.SYS`
and `COMMAND.COM` by name from `--dos-root`.  Those `byob_image` buffers prove
input identity and provide startup inputs; the active Direct DEM session does
not publish them as a DOS drive.

The currently bound Direct path is intentionally host-integrated:

- `dem_direct_host_session.c` asks public Win32 `GetFileAttributesW` for a
  resolved host pathname;
- `demfile_shim.c:CreateFileOem` converts OEM text and calls `CreateFileA`;
- neither seam maps the guest's `C:\\COMMAND.COM`/`\\COMMAND.COM` lookup to
  `--dos-root`, nor does it copy a boot image into host `C:\\`.

Therefore the current source-built image admission does **not** establish the
distinct guest bootstrap requirement: an admitted Direct path on which the
original `\\COMMAND.COM` lookup succeeds.  The observed `comerr → 50:3D`
terminal is consistent with that missing publication contract.  It must not
be converted to an Ignore/resume route.

## Ownership and disposition

| Concern | Owner | Current disposition |
| --- | --- | --- |
| `\\COMMAND.COM` open, DOS allocation, `EXEC`, and `comerr` | original NTDOS guest source | preserved; source terminal observed |
| `50:3D` report and VDM stop | imported DEM `demmisc.c` plus typed bx-vdm lifecycle seam | code complete; source stop, not decline |
| source-built image identity | CLI/BYOB admission | complete, but identity-only |
| publication of a boot shell to the Direct DOS pathname namespace | host path/boot-composition contract | not implemented or implied by current Direct mode |

The last row is neither a Bochs concern nor a DEM leaf BOP.  It is a product
path-composition decision: Direct mode normally exposes real admitted host
drives, while a source-built boot shell must be visible without copying it to
or mutating the host's real `C:\\`.  Existing policy reserves virtual boot
volume behavior as a separate feature rather than silently mapping
`--dos-root` onto `C:\\`.

## Result

S12 closes the attribution/disposition question.  It does not claim that the
current native run reaches NTDOS `INT 21h/AH=4Bh` or guest parent return.  The
next implementation must first admit a named boot-path publication policy;
it may not patch `50:3D`, alter source-built guest bytes, or make bx-core or
bx-mantle understand DOS paths.
