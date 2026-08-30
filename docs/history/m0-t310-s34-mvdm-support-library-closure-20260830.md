# M0 T310 S34 — MVDM support-library closure

S34 closes the selected original MVDM support libraries for the Win32/x86
CCPU40 composition. The formal graph directly archives original
`suballoc.c` and original `oemuni/{file,process}.c`; their historical test
programs remain excluded from the product graph.

The source-level caller review retains the original XMS/DPMI allocator callback
contract and the DEM/PIF OEM file/environment contract. Existing named
machine-memory and Win32 bindings are reused; no generic support shim or
second mapping manager is introduced. The fresh 381-edge formal graph and
forced-link audit expose no S34-owner unresolved symbol.

Evidence: [S34 P1](../etc/evidence/m0-t310-s34-mvdm-support-library-closure-001.md).
