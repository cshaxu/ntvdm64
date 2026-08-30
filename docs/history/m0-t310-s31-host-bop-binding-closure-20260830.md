# M0 T310 S31 — host-BOP binding closure

S31 closes the original SoftPC host-BOP binding boundary for the selected
Win32/x86 CCPU40 profile.  `nt_bop.c`, `cntlbop.c`, and the VDD `ms_bop`
table carrier are selected original bodies.  Their historical call order,
fixed-width reads and register/IP updates were not replaced.

The source shows that `nt_bop.c` is an ingress aggregator.  Every dispatched
owner is explicitly retained or transferred to its complete owner packet;
MONITOR-only IRET behavior remains unselected.  No new host table, BOP
dispatcher, Bochs route, or service-specific substitute was added.

The formal selected CCPU40 graph compiles the complete carrier set.  Its
remaining `DpmiDispatch` and `DBGDispatch` unresolved references are correctly
owned by S37 and S42, rather than hidden by S31.

Evidence: [S31 P1](../etc/evidence/m0-t310-s31-host-bop-source-boundary-001.md).
