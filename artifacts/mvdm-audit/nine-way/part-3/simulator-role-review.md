# Part 3: historical SIM16/SIM32 serial simulator

Scope: the ten owned keys in `simulator-observations.json`; all ten match both original source trees byte-for-byte. Reproduce with `python artifacts/mvdm-audit/nine-way/part-3/simulator-observer.py`. This is a source-role audit, not a runtime test.

`sim16/makefile:46` builds `sim16.obj` from `sim16.asm` and its include files and `siminit.obj` from `siminit.asm`; the link rule produces `sim16.dll` using `sim16.def`. The four exported functions include `Sim32SendSim16` and `Sim16SendSim32`; `sim16.asm` implements the packet-side operations and `siminit.asm:101` accesses its serial port. `sim32/sim32.c:26,71,212,321` implements packet-based VDM access and opens `\\Device\\Serial1`. Its neighboring `sources` declares a library with an empty `SOURCES` and `UMTEST=sim32`; `b.cmd` invokes the original build and copies the resulting test executable. These paths describe historical simulator tooling and not the same-named active `softpc.new/host/src/sim32.c` provider. Existing `docs/etc/evidence/m0-t473-s1-unselected-host-source-disposition.md:23` records the current CPU40 selection.

| Owned key | Specific role / recommendation |
|---|---|
| sim16/incs.inc | Includes simulator definitions/macros; retain historical tool source, not a product capability claim. |
| sim16/segdefs.inc | Segment declarations; historical source, consumer/build dependency remains unproved. |
| sim16/sim16.asm | Serial protocol endpoint for the simulator DLL; historical tool source. |
| sim16/sim16.def | Simulator DLL exported interface; historical tool source. |
| sim16/sim16.inc | Simulator declarations, explicit make dependency; historical tool source. |
| sim16/sim16.mac | Simulator macros, explicit make dependency; historical tool source. |
| sim16/siminit.asm | Simulator DLL initialization and serial setup; historical tool source. |
| sim32/b.cmd | Historical simulator test build/copy helper; historical tool source. |
| sim32/sim32.c | Simulator host serial packet implementation; historical tool source, do not conflate with active host SIM32 provider. |
| sim32/sim32.h | Simulator API and pointer macros; historical tool source. |

No executable or deployed guest was compared or run here; no conclusion follows about serial debug tooling availability in another configuration. File identity establishes no behavioral equivalence and no percentage of runtime coverage. `segdefs.inc` is not listed in the examined explicit object prerequisites, so its inclusion and effective configuration remain pending rather than inferred.
