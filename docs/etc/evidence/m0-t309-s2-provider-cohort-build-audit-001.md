# M0 T309 S2 P1 — original provider-cohort build audit

## Inputs and procedure

`New-T309OriginalProviderCohortsNinja.ps1` reads the literal original
`sources` cohorts:

- DEM: 16 source files, including its original `demdisp.c` table;
- COMMAND: 11 source files, including its original `cmddisp.c` table.

It builds each complete cohort into a static library under MSVC `/MT` and
`/W4`, for both x86 and x64. Static libraries preserve unresolved external
references, so the graph also performs a deliberately non-runnable
`/force:unresolved` audit link. That audit is evidence only; it never creates
a product executable or treats the result as runnable.

P1 additionally compiles the 16 already-existing source-shaped binding inputs
from Base VDM, SoftPC, monitor, Win32, and Redirector adapters into a separate
static archive. No original provider, dispatcher entry, or adapter behavior is
changed by this delivery.

## Results

- Both original cohorts compile and archive on x86 and x64.
- The selected named binding cohort compiles and archives on x86 and x64.
- The raw original-cohort forced link reports 212 unique unresolved external
  symbols. Adding the selected bindings removes the direct register, SAS,
  Base VDM, guest-location, host-identity, redirector-token, and VDD-shadow
  forms, but exposes the selected adapters' lower mechanical dependencies;
  the bound audit consequently reports 221 unique unresolved forms.

This increase is not a regression or a provider failure: static archive
selection has moved the audit one layer downward. In particular, the new
symbols are the already-named `adapter-bochs` machine facade/lifecycle and
`session` mapping/guest-memory forms. They must be composed as their own
components rather than copied into DEM/COMMAND.

## Disposition

The next P will add only the existing `session` C sources and the existing
`adapter-bochs` C++ machine forms to the audit graph, then remeasure the
remaining external boundary. It must not add a hand-written BOP service,
rewrite a provider, or import the excluded historical PIC/second executor.

The persistent non-admitted classes are unchanged: historical floppy/FDC and
INT13 machine bodies remain machine/firmware-owned; Redirector, VDD, WOW, and
BaseSrv/CSRSS product shells remain their designated owner packages.
