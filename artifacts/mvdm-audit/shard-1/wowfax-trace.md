# Shard 1: WOWFAX original-driver reachability review

## Scope and frozen identity

Shard-1 owns `fax/wowfax/wowfax.c` in the frozen MVDM inventory. The pinned
inventory hash is `F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196`.
Related WOW32, UI and guest source are read-only cross-boundary evidence and
are not reclassified by this report.

## Original component roles

- `src/mvdm/fax/wowfax/sources` selects `wowfax.c` for a distinct
  `TARGETTYPE=GDI_DRIVER` named WOWFAX; `fax/wowfaxui/sources` defines its
  separate UI DLL. `fax/printer.inf` names both install files.
- The frozen product selection reports these two source packages as
  `not-product-selected`, while `wow32/wow32fax.c` is `wow32-product`.
- The selected WOW32 `wkman.c:1869` calls `WOW32FaxHandler`; the handler in
  `wow32fax.c` subclasses the guest WOWEXEC fax window. The immutable guest
  source `wow16/test/shell/wowexfax.c` implements FaxWndProc and its named
  driver dispatch. Guest presence alone is not proof that this path ran.
- `wkernel.c:172,223` can call `IsFaxPrinterWriteProfileString`; if the
  device is recognized, `wow32fax.c::InstallWowFaxPrinter` reads/writes the
  original WOW Fax Registry subtree, loads WINSPOOL and copies `WOWFAX.DLL`
  and `WOWFAXUI.DLL` from `pszSystemDirectory` to the printer driver directory
  before invoking spooler driver/printer installation APIs.
- The bounded allowlist comment in `mvdm_shadow_registry.c:273` is therefore
  explained by the **selected WOW32 fax Registry client**. It is not evidence
  that either separate GDI driver DLL has been built, installed, or executed.

## Current evidence and finite gap

A direct name-filtered listing of `O:/winnt/system32` and `assets/release`
found no fax-named files at the time of review; this cannot rule out files in
other system directories, configurations, external installations, or a later
package. `docs/etc/evidence/m0-t468-s1-printing-spool-owner-matrix.md:28-29`
explicitly leaves fax installation outside that printing/spool admission.
No fresh fax execution, guest deployment, spooler mutation or device test was
performed. Do not infer that an existing external fax provider is absent,
that this capability works, or that a machine-wide driver install is safe or
authorized from the source evidence alone.

`InstallWowFaxPrinter` additionally has a source-level suspected fault:
`bRetVal` is assigned on `AddPrinterDriver` failure but read on its success
path without an evident preceding initialization. This is not a reproduction.
Trace the pinned original and selected compile, established Registry and
provider availability, and safe existing mock/fixture evidence first. Under
source policy, a proved host defect requires the smallest registered fix and
an explicit normal/failure/lifecycle test, without changing guest media.

Disposition: **pending cross-package disposition**, not full or excluded.
The finite missing work is to establish whether the original fax driver/UI
are required and legally/architecturally admissible as a separate product or
are supplied externally, then prove the selected caller, registry, guest
communication, spooler provider and teardown chain or record a justified
boundary with its affected capability. Avoid silently classing a standalone
GDI driver as an NTVDM worker object.
