# M0 T225 S58 post-BOP continuity admission correction

## Correction

S57's initial follow-on wording selected a DEM process-resource package.  That
selection was invalidated by the required source/current review before any
runtime change: T225 S19 already closes the complete callable DEM package.

Its evidence covers all `50:00..48` slots in Direct and Readonly, the one
package-session route, and the source-defined lifecycle order:

```text
50:3C demTerminatePDB -> per-PSP FindFirst/FindNext cleanup
DOS_ABORT / normal guest close -> 50:02 demClose
```

The correction is particularly important because `demTerminatePDB` does not
own ordinary JFT/SFT token closure.  Original `demsrch.c` calls host/device
termination hooks and frees its per-PSP search list; original guest DOS later
selects JFT/SFT entries and invokes the normal close route.  The current
session lifecycle provider retains that order, and T225 S19's formal fixture
proves `50:3C` leaves an opaque Direct token live until `50:02` closes it.

## Result

No DEM service, profile seam, guest image, or Bochs behavior is changed by
this correction.  Reimplementing `50:3C`, adding a new PDB/JFT/SFT model, or
using the current BOUND trace to reopen DEM would contradict the retained
source and prior package closure.

Because S57 now establishes all BOP owner/disposition coverage and DEM and
COMMAND are already package-closed at their declared boundaries, S58 is
properly a **post-package guest execution continuity diagnosis**.  Its first
question is the owner of the observed BOUND instruction, not the next BOP
service to implement.  Any repair requires a later bounded admission for a
whole CPU/machine/guest-owner group.
