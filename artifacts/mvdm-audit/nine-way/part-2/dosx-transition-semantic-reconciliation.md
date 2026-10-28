# Part 2: DOSX transition and disk entry semantic reconciliation

Scope: owned `dpmi/486/dxutil.asm`, `dpmi/dxutil.asm`, `dpmi/dxdisk.asm`, `dpmi/dxrom.inc`, and related descriptor declarations already individually identified in `dpmi-guest-family-review.md`. This supplements rather than replaces that per-file inventory; no guest is rebuilt or modified.

## Call chain and provider

* `src/mvdm/dpmi/486/dxutil.asm:806-820` is the WOW_x86 real-mode continuation: it pushes CS, IP, SS, SP, DS, then emits BOP FD; the receiving stack memory order is DS, SP, SS, IP, CS. The resumed guest discards the three remaining arguments. `dpmi/dxdisk.asm:195,624,839` uses the source macro to return to real mode for disk service; therefore this is not an independently replaceable host disk implementation.
* Original `src/mvdm/softpc.new/base/bios/bios.c:645` maps FD to `switch_to_real_mode` only for `NTVDM && MONITOR`. Current CPU40 counterpart `src/mvdm/dpmi32/modesw.c:127-190` captures the exact continuation before clearing PE, then refreshes CCPU segment caches, and records explicit exception `MVDM-HOST-DIV-229`. Current `dpmi32/dpmi32.c:49-78` retains the original indexed DPMI service dispatcher, including mode switch, app initialization, tables and memory cleanup; a mode switch is not proof all dispatch entries work.
* Original i386 receiver and original guest frame were audited against the CPU40 implementation in `docs/etc/evidence/m0-t404-s2-dosx-bop-fd-contract-audit-001.md`. That record describes a prior r036 failure, r038 nonzero continuation and later progression; those are historical observations, not a new end-to-end passing result or evidence current released binary is identical.

## File decisions and lifecycle limits

| Owned entry | Evidence and bounded disposition |
|---|---|
| `dpmi/486/dxutil.asm` | Preserve as original guest transition provider; original 486 build selection and immutable DOSX identity separately recorded in `dpmi-guest-family-review.md`. Preserve host FD adapter, no guest patch. Frame/segment continuation specifically supported by historical source and r038 observation only. |
| `dpmi/dxutil.asm` | Alternate original source, do not classify as the current 486 provider merely by matching basename. Retain for alternate profiles; no alternate-profile behavior proved. |
| `dpmi/dxdisk.asm` | Original guest disk service caller, retained as functionality. Reentry and disk error returns still require an actual protected-mode DOSX workload, not just source references. |
| `dpmi/dxrom.inc` | Shared guest definitions consumed by multiple DOSX modules; retain as interface, not a runtime provider. Exact conditional expansion remains open. |
| `dpmi/hostdata.inc`, `dpmi/segdefs.inc`, `dpmi/gendefs.inc`, `dpmi/stackchk.inc` | Original guest layout/stack declarations; preserve. Their identities and literal references are in the companion JSON. This audit does not establish individual expanded layouts or ownership by the modern host. |

`switch_to_real_mode` dereferences the translated stack pointer without a separate checked failure branch. The original contract presumes a valid source-owned transition frame: this audit establishes neither that a malformed guest frame is safe nor that adding a new recovery is correct. It would require separate original-behavior and invalid-pointer evidence. On return, the guest remains responsible for its original continuation and service-level cleanup; the host adapter owns mode and cache transition only. Other nested transitions, exceptions, app teardown, failure recovery and all selector lifetime semantics are not proved by the frame audit.

## Evidence and follow-up

Existing `tests/observation/dosx_himem_capability.asm` checks public DOSX/HIMEM availability; `dpmi_descriptor_contract.asm` checks INT31 descriptor validation; `dpmi_debug_lifetime.asm` covers a debug-handle cleanup path; the Win31 mouse report checks six actual DPMI calls. Their presence does not mean they were rerun or that they cover FD frame integrity under nested disk failures. Existing T404 r038 observation is explicitly bounded above.

S11 should exercise unchanged guest DOSX transitions with repeatable frame/segment observations, nested mode switches and protected-mode disk failure/return plus normal task teardown; identify the actual package and compare trace to original contract. S12 owns DOS disk service semantics; S13 owns device/interrupt interactions. Record failures before proposing new exception behavior, and S19 recheck against the matching release. No current failure is claimed, and no file is recommended for removal.

Reproduce source facts with `rg -n "SwitchToRealMode|BOP_SWITCHTOREALMODE" src/mvdm/dpmi/486/dxutil.asm src/mvdm/dpmi/dxdisk.asm`, `rg -n "switch_to_real_mode|DpmiDispatchTable" src/mvdm/dpmi32 src/mvdm/softpc.new/base/bios/bios.c`, and rerun the part-2 inventory observer. No runtime tests executed during this read-only review.
