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

P1 additionally compiles the existing source-shaped binding inputs
from Base VDM, SoftPC, monitor, Win32, and Redirector adapters into a separate
static archive. No original provider, dispatcher entry, or adapter behavior is
changed by this delivery.

## Results

- Both original cohorts compile and archive on x86 and x64.
- The selected named binding cohort, including the three existing `session`
  mapping/lease/lifecycle inputs, compiles and archives on x86 and x64.
- The raw original-cohort forced link reports 212 unique unresolved external
  symbols. Adding the selected bindings removes the direct register, SAS,
  Base VDM, guest-location, host-identity, redirector-token, VDD-shadow and
  session mapping/lease forms. The current x64 bound audit reports 208 unique
  unresolved forms.

This is not a provider failure: static archive selection has moved the audit
one layer downward. The remaining mechanical symbols are the already-named
`adapter-bochs` machine facade/lifecycle forms. They must be composed as their
own component rather than copied into DEM/COMMAND.

The first x86 graph reuse exposed a truncated `.ninja_log`/`.ninja_deps` state
file, not a source or ABI failure. Removing only those disposable Ninja state
files and regenerating the graph rebuilt all three session inputs. The same
clean-state procedure verified x64. No tracked build artifact or source file
was removed.

## Disposition

The next P will add only the existing `adapter-bochs` C++ machine forms to the
audit graph, then remeasure the
remaining external boundary. It must not add a hand-written BOP service,
rewrite a provider, or import the excluded historical PIC/second executor.

The persistent non-admitted classes are unchanged: historical floppy/FDC and
INT13 machine bodies remain machine/firmware-owned; Redirector, VDD, WOW, and
BaseSrv/CSRSS product shells remain their designated owner packages.
