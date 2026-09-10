# M0 T289 — Original DPMI owner package recovery

## Objective

Recover the coupled original `dpmi32` host package and DOSX guest package in
dependency order, preserving original source/provider shape and keeping
protected CPU mechanics selector-blind below `adapter-bochs`.

## Sequential subtasks

| S | Scope | Exit criterion |
| --- | --- | --- |
| S1 | Source/ABI/failure admission and all `53:00..18` owner inventory. | Every source group and BOP has one disposition. |
| S2 | Protected-machine mechanical audit and minimum typed frame/LDT interface design. | Every original protected CPU/memory import is existing, named-new or refused; no source body enabled. |
| S3 | Implement and prove the selector-blind typed mechanical foundation through `adapter-bochs`; use no DPMI selector. | Atomic copied frame, bounded protected spans and minimum descriptor/mode operations have x86/x64 mechanical proof. |
| S4 | Compose original initialization, descriptor and xmem source groups using the admitted interfaces and session-owned resource identities. | Original source bodies compile and local provider cases preserve source failure behavior. |
| S5 | Compose original stack/interrupt/fault source groups with the same atomic frame contract. | No partial register/frame commits; unimplemented historical monitor branches remain explicit failure. |
| S6 | Compose eligible DOS translation and VDD/debug dispositions, then execute one x86/x64 package matrix. | Every `53:xx` has source behavior or an original/proven unavailable outcome. |
| S7 | Audit source identity, exceptions, mappings and transfers, then close T289. | Package is local-regression closed or every remaining prerequisite has a named later owner. |

## Non-negotiable boundaries

- DOSX remains a guest artifact/source package, never a host-linked translation
  unit.
- `adapter-bochs` is the only Bochs caller. `bochs-core` remains selector-blind.
- `session` remains the sole mapping-manager owner.
- `Sim32GetVDMPointer` becomes an explicit short bounded lease/copy; it can
  never reappear as a durable raw pointer.
- Historical `NtVdmControl`, `VdmInstall*Handler`, CCPU executor and CSRSS
  paths must retain source-proven unavailable behavior unless a named owner
  package admits a public modern replacement.
