# M0 T276 S25 conclusive SoftPC final expected-integration dispositions

## Question

Which `softpc.new` paths already have a final product conclusion directly from
their original subpackage role, without prematurely deciding the host control,
monitor or app-composition sources?

## Inputs and procedure

The 998-row T276/S12 SoftPC ledger separates historical machine execution,
firmware input, independent tools, build/profile controls, declarations and
host-control candidates.  This record applies the final taxonomy only to the
five categories whose final treatment follows directly from that original
role.  No body is enabled and all other SoftPC categories remain explicitly
unresolved for their own owner-package review.

## Final conclusions

- 489 historical CPU, device and machine execution paths are
  `not-host-runtime`: importing them would create a second executor alongside
  Bochs.  `adapter-softpc -> adapter-bochs` preserves historical call shape at
  admitted callers; it does not link the historical executor.
- 58 BIOS, ROM and data paths are `firmware-only`.  They remain immutable
  `opennt-mvdm-firmware` inputs selected by an `adapter-bochs` manifest, not C
  host providers.
- Seven historical build utility paths are `tool-only`.
- Two Unix/profile paths and two original build-control paths are
  `not-host-runtime` in the supported Win32/x86 and Win32/x64 product.

This classifies 558 paths.  It intentionally leaves the 59 `host/src`
candidate bodies, 189 host control declarations/inputs, 156 SoftPC-shaped
declarations, eight support units, 19 debugger inputs and nine original
`obj.vdm` composition paths to later final owner decisions.  That restraint is
required: those files may carry real historical control-flow contracts and
cannot be labelled by their current non-enablement state.

## Verification

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Set-T276S25SoftpcObviousFinalDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64

powershell.exe -NoProfile -ExecutionPolicy Bypass `
  -File tools/governance/Verify-T276S25SoftpcObviousFinalDisposition.ps1 `
  -RepositoryRoot O:\repos.hobby\ntvdm64
```

These commands verify only final-disposition audit coverage; they do not
compile, link or execute historical SoftPC source.
