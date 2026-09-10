# M0 T198 S74: DEM PDB-Termination Composition Witness

## Change and verification

The existing DEM session-lifecycle provider is now a declared source member of
the current `bx-vdm` composition manifest. The common composition invokes it
only after common ingress, provider-registry selection and DEM-plane
classification. The composition test adds exact real-mode `50:3C` coverage:
its PSP comes from low BX, it resumes at `fault_rip + 4`, and it writes neither
GPR nor flags delta.

A fresh MSVC x64 `/MT` source rebuild at
`artifacts/build/t198-s74-dem-pdb-termination-r1` compiled and linked the
provider with the complete CPU5/mantle/adapter closure. The runtime no longer
stops at `50:3C`; it continues to a later generic-UD observation.

## Runtime observation

The first later boundary is:

```text
status=4 observed-first-generic-ud=1
fault=c000:014a mode=1 vector=6 window=15:ffffffff
state=1:c000:014a eax=00000e43 ...
terminal=1:c000:014c
```

Status `4` is the fixture's `COMPLETED_UD_STOP`. Its earlier run had stopped
at `50:3C`; this later `C000:014A` event proves the new provider returned to
ordinary guest execution. It is not a CLI result or guest-success claim.

## Next boundary

The copied window identifies top-level BOP selector `15h`, with AH=`0Eh`.
OpenNT SoftPC BIOS maps BOP `15h` to `cassette_io` (`tape_io.c`), whose
unrecognized functions follow the original invalid-INT15 result path. That is
a historical machine-handler island, not DEM, DOS, or a Bochs CPU defect.
Its exact guest call site and error-result contract require a separate
source/ownership audit before any disposition is composed.
