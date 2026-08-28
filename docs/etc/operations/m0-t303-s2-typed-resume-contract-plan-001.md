# M0 T303 S2 — typed finite-resume mechanical-contract plan

## Objective

Close the shared mechanical gap recorded by T303/S1: make the historical
`host_simulate` call shape use a bounded, selector-blind Bochs resume operation
with a copied, typed outcome.  This is a machine contract only.  It must not
decode BOP selectors, name DEM/COMMAND/WOW/DPMI, create a second CPU executor,
or introduce guest-service completion semantics.

## Required implementation

1. Add an `adapter-bochs` copied finite-resume request/outcome contract that
   has an explicit version, finite budget and mechanical stop class.  It must
   distinguish invalid input, inactive machine, budget stop and unexpected
   machine return.
2. Route the existing `adapter-mvdm-host-out/softpc` `host_simulate` spelling
   through that contract without leaking a Bochs object or a host pointer.
   The historical `void` signature stays unchanged; its observable failure
   direction is captured in session-owned mechanical status only.
3. Verify on both MSVC x86 and x64 with a live opaque machine that scalar
   register reads/writes, bounded guest-memory leases and a finite resume all
   share one machine/session and do not instantiate SoftPC/CCPU execution.
4. Add negative tests for no active machine, invalid finite budget and
   unexpected return.  Tests may observe only typed mechanical results.

## Exclusions and handoff

- Event/BOP ingress and copied fault-frame delivery belong to P04.
- `GetVDMAddr` source-site conversions and DEM SFT/JFT integration belong to
  P05; S2 only validates their shared lease/shadow prerequisites.
- A20/SAS/XMS and protected frame/descriptor forms remain P08/P09.
- WOW callback task and command-broker semantics remain P11/P15.

## Exit

One x86/x64 formal Ninja recipe proves the typed mechanical resume contract,
the existing scalar register facade and session lease guard in a single live
machine test.  The next admitted package may use the typed result but must
attach its own owner-specific semantics.
