# M0 T288 — Original VDM monitor and keyboard/IVT substrate recovery

## Objective

Recover the complete selected OpenNT monitor substrate as one owner package:
`sim16`, `sim32`, `v86`, `ieuvddex`, and the reached original
`UpdateKbdInt15` keyboard/IVT fragment. The result must retain original
monitor/firmware owners and their interface shapes while binding only
selector-blind Bochs mechanics through the existing adapter and session
boundaries.

## Fixed boundaries

- `mvdm-host` and `mvdm-softpc-firmware` retain the selected original source;
  `src.old` is prohibited from build, runtime and acceptance inputs.
- `adapter-mvdm-host-out/monitor` owns same-shaped monitor-facing interfaces;
  `adapter-mvdm-host-out/softpc` owns same-shaped CCPU/SAS/SoftPC calls;
  `adapter-bochs` remains the sole Bochs caller.
- `session` remains the unique owner of mapping-manager instances and their
  lease lifetimes. No monitor or keyboard package creates a mapper.
- Bochs components remain selector-blind and do not acquire OpenNT, VDM, DOS,
  XMS, BOP, keyboard-policy or Win32 semantics.
- Kernel VDM, CSRSS/BaseSrv, multi-process brokers, general keyboard input,
  timer/mouse/video shells, and a second SoftPC CPU executor are outside this
  package unless the source audit identifies a bounded, directly required
  original user-mode fragment.

## Sequential subtasks

| S | Scope | Exit criterion |
| --- | --- | --- |
| S1 | Establish a complete file/package/source identity ledger and external ABI/failure map for `sim16`, `sim32`, `v86`, `ieuvddex`, and `UpdateKbdInt15`; classify every direct pointer, callback, event, thread, VDM_TIB and machine request. | Every selected source file and reachable interface has an original owner, recovery-ladder disposition, mapping/ABI classification, and a named later owner for exclusions. |
| S2 | Re-root the exact `softpc.new/base/keymouse` mirror into `mvdm-softpc-firmware`, then compose directly usable original declaration/control carriers into architecture-separated x86/x64 static islands without enabling monitor behavior. | The firmware mirror has its correct owner root; each admitted unit has exact source identity, a reproducible MSVC graph and a complete unresolved-symbol ledger. |
| S3 | Recover the smallest same-shaped monitor/VDM_TIB/event and fixed-frame boundary proven by S1. | Original layout/call/failure ordering remains source-owned; copied or leased data never crosses component ABI as a native pointer. |
| S4 | Recover source-proven SoftPC/SAS monitor mechanics and selector-blind Bochs requests through existing adapters. | No direct Bochs dependency enters an OpenNT mirror; no second CPU or mapping manager is added. |
| S5 | Recover the minimum original `UpdateKbdInt15` IVT state fragment and any mandatory original monitor lifecycle linkage. | The original IVT compare/update/reset contract is locally verified; no generic keyboard/IRQ/controller product shell is claimed. |
| S6 | Run the complete monitor/keyboard package regression on x86/x64 and the XMS success-path integration that its dependency unlocks. | Original source bodies or their source-proven unavailable outcomes execute in one package matrix; trace is observation only. |
| S7 | Audit source identity, exceptions, interface ownership, mapping managers and all transferred prerequisites; write the T288 closure. | The package is code-complete/local-regression closed, or every remaining prerequisite has a named later owner package. |

## Recovery ladder

For each S1 symbol group, use this order and record the first viable rung:

1. direct original OpenNT translation unit;
2. the original unit through a smallest same-shaped adapter or build binding;
3. an explicitly registered external-code intrusion only where Bochs mechanics
   cannot cross any other boundary;
4. a new bounded replacement only after the first three rungs are evidenced
   unavailable.

No trace hit, historical fixture, convenient public Win32 call, or preexisting
`src.old` code authorizes a substitute implementation.
