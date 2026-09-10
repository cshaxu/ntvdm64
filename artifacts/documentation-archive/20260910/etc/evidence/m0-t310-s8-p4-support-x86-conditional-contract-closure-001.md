# M0 T310 S8 P4 — support x86 conditional contract closure

## Scope

This closure reviews the `i386` conditions in the original support headers
`mvdm.h` and `suballoc.h`, within the current whole-tree scope of
`src/mvdm-host` and `src/mvdm-support`.

## Source finding

Both conditions distinguish original x86 VDM behavior from historical
MIPS/RISC behavior.  They do not distinguish 32-bit from 64-bit Windows.
The formal product deliberately does not define `i386` on either host
architecture, so leaving the conditions intact silently selected the RISC
fallback on x64.

## Disposition

- `mvdm.h` retains the original direct x86 fetch/store macro bodies.
- `suballoc.h` retains the original 4-KiB x86 commitment granularity.

The resulting x86 guest contract is identical on x86 and x64.  No pointer,
guest location, handle, or external identity crosses either header boundary,
so the mapping manager is not involved.

## Verification

The formal selected original DEM object that includes `mvdm.h` was forcibly
rebuilt on both architectures:

- `build/M0-T310/S8/p1-machine-source/x64/support-headers-forced-x64.log`
- `build/M0-T310/S8/p1-machine-source/x86/support-headers-forced-x86.log`

Both builds completed.  Their unrelated historical diagnostics remain visible
and are deliberately not suppressed. `suballoc.h` has no current selected
Ninja consumer; its source-level disposition is recorded for the later
original support-library inclusion rather than being hidden behind a temporary
fixture.
