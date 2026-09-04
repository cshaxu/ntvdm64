# M0 T390 S1 — pure-DOS configuration-carrier audit

## Result

The original configuration-selection carrier is recoverable and bounded.  A
PIF payload is the source-shaped way to select a separate `CONFIG.NT` /
`AUTOEXEC.NT` pair for the initial DOS VDM.  The current local Base VDM seam
does not yet copy that payload, so the profile is not enabled by this audit.
That is an adapter binding omission, not a reason to replace original
`cmdconf.c` or to modify guest media.

## Source chain

1. `mvdm-host/softpc.new/host/src/config.c:303-453` creates `VDMINFO`, asks
   `GetNextVDMCommand` with `ASKING_FOR_PIF | ASKING_FOR_DOS_BINARY`, then
   calls original `process_pif_exe(PifName)`.
2. `process_pif_exe` calls original `GetPIFData(&pfdata, PifName)`.
3. `mvdm-host/softpc.new/host/src/nt_pif.c:275-292` copies the NT PIF
   extension's `achConfigFile` and `achAutoexecFile` into
   `pchConfigFile`/`pchAutoexecFile` whenever `IgnoreConfigAutoexec` is not
   selected.
4. `nt_pif.c:61-90` makes `GetPIFConfigFiles` select that override; otherwise
   it uses the product default.  The current default's root lookup is the
   registered `MVDM-HOST-DIV-157` binding.
5. `mvdm-host/dos/command/cmdconf.c:198-249` passes the selected result to
   the unchanged original `ExpandConfigFiles` body.  Its missing/open/short
   root failures retain the existing original fatal direction.

This is the precise boundary required by the package: app/adapter may carry a
PIF pathname and immutable PIF/configuration assets; OpenNT code remains the
PIF parser and configuration preprocessor.

## Current seam gap

`adapter-mvdm-host-out/basesrv/include/base_vdm_local.h:28-48` currently has
no PIF capture member in `base_vdm_command`.  Consequently
`base_vdm_local.c:153-221` explicitly sets `VDMINFO.PifLen` to zero while
fulfilling the initial `ASKING_FOR_PIF` request.  This prevents the recovered
original PIF configuration branch from being selected, even though its owner
source is already compiled.

S2 must add a bounded PIF pathname capture to this adapter's copied BaseSrv
record and copy it into the caller-provided original `VDMINFO.PifFile` buffer.
The public shape is already defined by `opennt-abi/.../vdmapi.h`:
`PifFile` plus `PifLen`.  It is not a new configuration parser or a guest
contract.

## Stage identity and selected pure-DOS asset form

The current external stage is `O:\\ntvdm64`:

* `config.nt` SHA-256
  `6ae1e74e78de00c0fa03124e6f947d977c71f253855ad2af979554074aea189a`;
  it is an original-style NT configuration and already selects `HIMEM.SYS`.
* `autoexec.nt` SHA-256
  `c2a23a5fb96682c33d99c13a57e6f3c1e10dfd3d263d972775a8c42bfb6a9a9b`;
  it explicitly invokes `MSCDExNT`, `REDIR`, then `DOSX`, and is therefore
  unsuitable for the pure-DOS predecessor.
* OpenNT 4.5's original
  `nt/public/oak/bin/_default.pif` SHA-256
  `75caf5aca5b155afbd4ce7effe825fe1bf4767a0afee64a8d726c5e9694fa33a`
  contains the original `WINDOWS NT  3.1` PIF extension and demonstrates the
  two configuration pathname fields.

S2 will stage a **new named profile PIF** and two immutable profile inputs
under `O:\\ntvdm64` without overwriting the current default pair.  The
profile autoexec is a minimal original-syntax configuration whose sole
semantic difference is omission of the three optional startup programs.  It
must carry no synthetic DOS commands.  The profile PIF will use literal,
bounded product-root paths rather than `%SystemRoot%`, because the product
root is intentionally not the host Windows installation root.

## Failure and scope disposition

* A malformed/missing PIF is handled by original `GetPIFData` fallback and
  the existing original config preprocessing result.
* An absent selected config/autoexec input follows the existing
  `ED_BADSYSFILE` / `TerminateVDM` source direction.
* A profile pathname which cannot fit the original `PIFDEFPATHSIZE` field is
  rejected at the app/adapter capture boundary; it must not be truncated.
* The audit does not change default launches, direct-target launches, PIF
  fullscreen semantics, DPMI/WOW, guest `COMMAND.COM`, BOP, keyboard or CPU.

## S1 exit assessment

S1 is complete: the original carrier, its current adapter omission, exact
asset provenance and the bounded S2 binding work are identified.  The next
step is source-shaped PIF capture plus profile-stage creation, followed by a
formal product and fixed-container observation.  No interactive COMMAND
completion is claimed here.
