# Part 8: Win16 TIMER driver call and lifetime reconciliation

Eleven owned timer paths and their original identities are recorded individually in `guest-family-inventory-review.md`. The original `wow16/timer/makefile:9-16,83-103,128-136` selects libinit first followed by timer/local/startend/api/math objects, links MMSYSTEM, and adds TIMER resources. This is an original recipe, not proof that the shipped TIMER.DRV was rebuilt from this source.

The driver message dispatch in `api.asm:46-80` selects `tddSetTimerEvent`, `tddKillTimerEvent`, time and period endpoints. `timer.asm:55-56,315-574` owns allocation/identity and cancellation of guest timer events. `local.asm:235-310,433-469,556-640` describes and implements interrupt-driven due-event processing, ordering of EOI and guest `DriverCallback`, missed ticks and callback failure. This is distinct from a host polling loop or the host WINMM synchronous `tid32Message` test. `m0-t466-s3-async-multimedia-owner-audit.md` traces mmsystem ownership and explicitly requires a real immutable guest callback/cancellation witness; no claim is made that an original physical hardware ISR is necessarily selected by every modern guest profile.

| Owned path | Role and bounded decision |
| --- | --- |
| `wow16/timer/api.asm` | Original driver-message dispatch. Preserve callable endpoint and error behavior. |
| `wow16/timer/libinit.asm` | Original initialization object linked first; initialization/failure in a specific released guest unverified. |
| `wow16/timer/local.asm` | Original event ISR/callback/EOI path; preserve reentrancy and callback-failure semantics. |
| `wow16/timer/math.asm` | Recipe-selected original mathematical helper. No semantic claim for every arithmetic corner. |
| `wow16/timer/njumps.mac` | Original macro file; no direct recipe reference shown by prior scan, conditional consumers remain pending. |
| `wow16/timer/startend.asm` | Recipe-selected original startup/shutdown helper; exact cleanup on target profile requires actual task witness. |
| `wow16/timer/sysinfo.inc` | Original declarations included by libinit; not independent clock authority. |
| `wow16/timer/timer.asm` | Original timer event identity, set/kill and period state owner. |
| `wow16/timer/timer.inc` | Shared original interface/state declarations used by source objects; preserve guest ABI. |
| `wow16/timer/timer.rcv` | Original version resource input in the historical recipe; localized selection pending. |
| `wow16/timer/TIMER.RES` | Original resource artifact; exact source-to-packaged-binary correspondence unproved. |

Failure/lifecycle obligations: preserve the timer driver's event and callback owner, original callback failure path, cancellation and reentrancy, and task/process rundown; do not create a competing host timer-event list. Source describes reentrant and failure cases but this review executes none. Existing T466 historical focused evidence proves native timer packet lookup/call, not asynchronous guest callback, cancellation, IRQ correctness or teardown. Existing PIC/RTC callback probes cannot alone prove this guest driver's behavior.

S14 should execute immutable guest one-shot and periodic timers, cancellation before callback, callback-initiated cancel/reentry, callback failure and task teardown, then check no stale callbacks/event handles. S13 should verify device/PIC timing and EOI if the reached implementation needs it; S19 must bind the exact TIMER.DRV and runtime hashes and repeat those relevant to release. No guest rebuild, new timer provider or test result is claimed.

Reproduce with `rg -n "tddSetTimerEvent|tddKillTimerEvent|DriverCallback|callback" src/mvdm/wow16/timer`, read its makefile and T466 asynchronous owner evidence, then replay part-8 guest inventory observer. No guest, product or shared ledger changes.
