# M0 T310 S8 P4 — Configuration guest-profile conditional closure

## Scope

This record closes one architecture-sensitive source contract in the selected
original SoftPC configuration owner.  It does not close P4, the wider warning
ledger, or any controller runtime path.

## Original source finding

`softpc.new/host/src/config.c` used `#ifdef i386` to select the calculation
that sums XMS, EMS and conventional memory.  Its `#else` selected a historical
RISC VDM path which obtains a larger VDM size from the host registry and adds
DPMI memory.  `i386` is a host-compiler marker, not a guest-machine identity.
The selected product has one x86 guest profile on both x86 and x64 hosts, and
the build deliberately does not define `i386` globally.

## Disposition

The mirror retains the original x86 calculation on both host targets and
removes the unselected RISC product branch.  The source carries
`DIVERGENCE(MVDM-HOST-DIV-056)` and the mirror register records the same
exception.  No mapping-manager identity is involved: these values are
fixed-width guest memory capacities, not host pointers or handles.

## Verification

The formal selected original-SoftPC Ninja graph was regenerated and executed
on both targets after the source change:

- `build/M0-T310/S8/p1-machine-source/x64/config-i386-x64-build.log`
- `build/M0-T310/S8/p1-machine-source/x86/config-i386-x86-build.log`

Both builds completed successfully.  The correction establishes the same
source-selected guest memory calculation on x86 and x64; it does not claim
that the complete memory, EMS, XMS, DPMI or runtime-device profile is closed.
