# Part 8: Win16 SYSTEM standard and platform variants

This is a functional reconciliation for the fourteen owned `wow16/system` paths in `guest-family-inventory-review.md`; the independent shard-2 and shard-3 system-variant reports already provide file-level original identities and matching SYSTEM.DRV image hash. Historical recipes differ; image identity does not prove which built the binary or which operations a live guest reached.

The standard `system/makefile:93-110` assembles `system.asm` and `timer.asm`, links them, and attaches `system.res`. `system.def` exports system-timer creation, enable, disable, millisecond counter and 80x87 state functions. `timer.asm:284-368` selects the INT 8 handler, stores the previous vector, and restores it on disable; under WOW, the source checks timer count before installing. Timer table/event lifetime is guest driver state; host PIC/CPU provide interrupt services. System timer is distinct from the multimedia TIMER.DRV reviewed separately. Exact live event, nested interrupt and error behavior remain unproved.

| Owned path | Role and constrained classification |
| --- | --- |
| `wow16/system/envect.asm` | HP-specific vector source included by alternate hpsystem recipe; not in standard two-object response. |
| `wow16/system/equate.inc` | Shared original symbolic declarations used by envect; exact alternate expansion pending. |
| `wow16/system/hpsystem` | Original HP-specific makefile, not a runtime binary. |
| `wow16/system/hpsystem.def` | Original HP variant module/export interface; do not equate to standard build. |
| `wow16/system/romstuff.inc` | Original ROM declarations; system.inc says its local definitions were picked from this file, which does not itself prove an active include. |
| `wow16/system/romsys` | ROM alternate build recipe. It lists romsys.def as a dependency but its link command spells system.def; do not silently resolve the original inconsistency. |
| `wow16/system/romsys.def` | Original ROM variant definition; retained for alternate recipe, actual link selection unresolved. |
| `wow16/system/system` | Historical standard/alternate named build recipe; not the runtime image. |
| `wow16/system/system.drv` | Immutable original guest binary; prior observers prove exact ZIP/installed/source-tree byte identity only, not source recipe or function acceptance. |
| `wow16/system/system.lrf` | Current-only generated-style response; matching standard recipe exists, but its provenance and actual consumption remain unresolved. |
| `wow16/system/system.rcv` | Original version resource included by system.rc; preserve build-input role. |
| `wow16/system/SYSTEM.RES` | Original resource build artifact; identity in source tree does not establish exact resource bytes inside a released driver. |
| `wow16/system/vecsys.inc` | Original include for envect; HP alternate build input, not proof of standard selection. |
| `wow16/system/windefs.inc` | Original declarations; `system.inc:16` occurrence is commented out, so active consumer/profile is pending. |

Lifecycle and evidence: the source restores the previous INT8 vector on disable, but that is not proof of safe behavior under nested callback, prior-vector replacement, or task teardown. Existing guest identity and profile startup evidence does not prove actual timer callback, cancellation or 80x87 state. S14 should exercise original SYSTEM driver create/disable/re-enable/kill, prior vector restoration and callback reentry on a hash-identified guest; S13 owns PIC/interrupt behavior and S11 protected-mode transition; S19 associates successful observations with the actual release. Preserve HP/ROM profile differences without extending default capability claims. No guest rebuild or actual runtime test performed.

Replay `rg -n "EnableSystemTimers|DisableSystemTimers|CreateSystemTimer|KillSystemTimer" src/mvdm/wow16/system`, inspect standard/HP/ROM recipes, and rerun `artifacts/mvdm-audit/shard-2/win16-system-guest-observer.py` and shard-3 variant observer with bytecode disabled. No product or shared ledger modifications.
