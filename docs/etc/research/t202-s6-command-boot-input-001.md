# T202 S6 COMMAND boot-input ownership recovery

## Question

Can the atomic COMMAND provider recover the paired `54:0C` and `54:0D`
pathname contract without retaining a mutable DEM/boot-namespace dependency,
creating a host temporary file, or changing a host system boot file?

## Inputs

- Original evidence: `src/opennt/base/mvdm/dos/command/cmdconf.c`, where
  `cmdGetConfigSys` and `cmdGetAutoexecBat` expand an NT-host temporary
  replacement and copy its OEM pathname to the caller's `DS:DX` buffer.
- Current selected profile boot inputs: `\\CONFIG.SYS` and `\\AUTOEXEC.BAT` on
  the selected guest drive.
- Source closure: `tools/Invoke-T198S16BootNamespaceVdmX64Probe.ps1` with
  fixture `boot-namespace` and build root
  `artifacts/build/t202-s6-command-boot-input-r3`.

## Change and procedure

1. Added `bx_ntvdm_command_boot_input_v1`, a fixed-width COMMAND-owned copy
   of the selected drive and two guest pathname values.  It contains neither
   a host path/handle nor guest address.
2. Copied that record while the COMMAND package session is assembled.  The
   `54:0C/0D` path producer now accepts only this record; it no longer reads
   `namespace_plane->provider.readonly_namespace` during BOP dispatch.
3. Kept the old boot-namespace-provider diagnostic entry source-buildable by
   giving it an isolated compatibility copy.  It is not the product COMMAND
   route; the atomic COMMAND session remains the only callable `54:xx` owner.
4. Ran the bound MSVC x64 `/MT` fixture.  The fixture deliberately alters the
   mutable boot-namespace CONFIG/AUTOEXEC paths after binding, invokes both
   COMMAND services through common ingress, and requires their original
   selected path responses.

## Observations

The retained manifest reports `linkExitCode: 0`, `runExitCode: 0`, and
`passed: true`.  The post-bind mutation did not alter `54:0C`'s
`C:\\CONFIG.SYS` result or `54:0D`'s `C:\\AUTOEXEC.BAT` result.

No API that creates, deletes, writes, or changes a host file is used by this
slice.  The result is a copied guest pathname written through the existing
checked multi-write transaction, matching the source ABI's 64-byte destination
constraint without reproducing the historical temporary-file mechanism.

## Interpretation and confidence

This closes the COMMAND-owned default boot-input **path-production boundary**:
the paired services are one session-owned profile-read capability and no
longer borrow mutable DEM state at runtime.  Confidence is high for this
bounded adapter contract because the regression passes through common ingress
and directly proves independence from the old mutable record.

It does not implement host-backed CONFIG/AUTOEXEC expansion, overlay
materialization, standard streams, child lifecycle, VDD/KEYB, or a native
guest trace.  Any host-backed configuration source remains a separately
admitted profile decision.

## Follow-up

Continue T202/S6 by recovering the remaining connected COMMAND lifecycle and
stream capabilities as package slices.  Trace remains post-package integration
evidence only.
