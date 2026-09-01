# M0 T337 — Packaged guest system-file-root / DOS drive-root composition

## Purpose

Recover the source-shaped composition contract that makes immutable DOS and
Win16 media installed beside the product executable available to the original
guest after `NTIO.SYS` has loaded.  This is a deployment-root binding problem,
not a replacement DOS, virtual disk, loader, BOP provider, or guest binary
change.

The installed layout remains:

```
<product-directory>\ntvdm64.exe
<product-directory>\dos\...
<product-directory>\win16\...
<product-directory>\softpc\roms\...
```

`dos/` is the immutable host-side root for selected original guest media.  It
is not assumed to be physical `C:\`, nor does this package create a virtual
`C:` drive.  The task must establish how original DOS paths such as
`\COMMAND.COM` acquire the intended guest-visible boot/root meaning.

## Predecessor evidence

- T336/S4 proves the current fixed x86 process reaches original
  `50:3D -> demExitVDM`; it does not wait on FDC/INT15 and does not lack
  `NTIO.SYS`, `NTDOS.SYS`, `COMMAND.COM`, `config.nt`, `autoexec.nt`, or
  `HIMEM.SYS`.
- `app/package_layout.c` already registers executable-relative `dos/` and
  `win16/` media roots in the session.
- Original `softpc.new/host/src/nt_msscs.c::InitialiseDosEmulation`, original
  `dos/dem/demmisc.c::demLoadDos`, and original `softpc.new/host/src/nt_pif.c`
  already use the selected DOS media root for startup inputs.
- Original `dos/dem/demgset.c::demGetBootDrive` and
  `dos/dem/demfile.c::demOpen` remain the required later guest path/host-file
  contract.  Their relationship to the staged system-file root is not yet
  proven.

## S plan

### S1 — Original root-selection and later-path contract audit

Trace, from original guest `SVC_DEMGETBOOTDRIVE`, `\COMMAND.COM` startup and
`SVC_DEMOPEN`, every original host path conversion and boot-drive result.
Inventory the existing session/app/media-root bindings and identify whether a
direct original composition already exists, which exact boundary is absent,
and which recovery rung is admissible.  Produce a source/ABI/failure ledger.
No production behavior changes occur in S1.

### S2 — Minimal source-shaped root binding

Implement only the boundary selected by S1.  Retain original DEM path order,
canonicalization and failure behavior.  Prefer an original file/provider body
or a same-shaped adapter binding; any project-authored policy needs a
registered exception.  No guest binary, BOP, CPU, BIOS, FDC, or generic DOS
filesystem rewrite is allowed.

### S3 — Local contract verification

Exercise selected-root success, missing-system-file failure and unrelated
host-drive behavior with focused source/adapter tests.  Rebuild and link the
affected formal x86 and x64 graphs.

### S4 — Fixed-container integration attribution

Run one fixed x86 container observation with unchanged media.  Record whether
the original path gets beyond the present `50:3D` terminal; if not, transfer
only the exact newly reached original owner.

## Exit

This task closes only when the original guest system-file root / DOS drive-root
contract is source-defined, one selected composition has local positive and
negative proof, and paired x86/x64 product graphs link normally.  A fixed
runtime observation may transfer a distinct next owner; it is not a claim of
general DOS execution.

## Exclusions

- No virtual boot volume or assumption that guest `C:` is host `C:\`.
- No host installation, mutation, guest-media copy/rewrite, or guest binary
  modification.
- No trace-selected BOP provider, COMMAND/NTDOS rewrite, new mapping manager,
  second filesystem, CPU/machine repair, Bochs route, CSR/kernel-VDM work, or
  multi-session expansion.
