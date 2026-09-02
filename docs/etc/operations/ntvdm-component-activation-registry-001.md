# NTVDM component activation registry

## Rule

`compiled` means that a component's selected original/adapter sources occur in
the formal CPU40/x86 link graph. It does **not** mean the component works. A
compiled production component is complete only when a frozen runtime witness
reaches its original owner boundary and verifies its source-shaped success and
failure result. A utility, debugger or debugbreak-evidence library is not a
runtime component and must instead retain an explicit tool/evidence
disposition.

## Current selected production components

| Component | Formal library/source group | Current state | Required runtime witness |
| --- | --- | --- | --- |
| Product composition | `app-machine-shell`, `session`, `broker` | linked; early package rejection observed | staged executable accepts adjacent immutable media and binds one session. |
| Base-VDM/monitor bridge | `basesrv-bindings`, `monitor-bindings` | local fixture only | original `GetNextVDMCommand` consumer receives declared record in a running session. |
| SoftPC bootstrap | `original-ccpu386`, `original-softpc-bios`, `original-softpc-host-roots`, `softpc-bindings` | linked only | original CPU40 entry, firmware, NTIO and NTDOS startup reach a source marker. |
| Conventional machine services | `keymouse`, `system`, `disks`, `comms`, `dos`, `video`, `cvidc` | linked only | selected DOS workload uses each reached timer/input/disk/video service. |
| DOS host services | `original-mvdm-dem`, `original-mvdm-command` | local source/fixture evidence | COMMAND built-in then external DOS EXEC return. |
| XMS | `original-mvdm-xms` | source/build only | one selected XMS/A20/INT15 workload. |
| DPMI32 | `original-mvdm-dpmi32` | source/build only | one selected protected-mode workload. |
| Redirector | `original-mvdm-redir`, `redirector-bindings` | source/build only | local file/directory/pipe workload and explicit unavailable matrix. |
| Host capability bindings | `softpc-win32-bindings`, adapter sources | focused host fixtures only | witnesses above exercise their bound public Win32 behavior. |
| Presentation | app presentation and selected SoftPC video | local/UI source only | DOS graphics workload shown through app presentation. |

`original-mvdm-host-{suballoc,oemuni}` are support libraries: they are
activated by their first original owner and do not receive invented standalone
product behavior. `base-trace`, debugger bindings and NTVDMx64 debugbreak
evidence are non-runtime evidence/tool inputs.

## Not-yet-selected original owner components

- DPMI16 assembly/DOS extender host package.
- WOW32/WOWEXEC/Win16 task, callback, monitor and provider packages.
- VDD and IEU/VDD extension packages.
- FAX/UI extension package.
- BDE, debugger, VDM debugger and extension tools.
- Remaining SoftPC alternatives, generated variants, architecture-specific
  bodies and standalone products: each requires a source-owner decision before
  it can become a production component.

## Admission rule for every not-yet-selected component

The same T must: (1) adopt the complete original owner package or exact
source-shaped subset; (2) compose it through existing/new bounded adapters;
(3) add it to the formal graph; and (4) run one immutable workload that proves
its owner result. A link-only admission is prohibited.

## Activation-frontier priority

Work starts at the top of the selected runtime path. If its one frozen
workload cannot proceed because a specific original owner component is absent
from the formal graph, that missing component becomes the **immediate next
candidate**, ahead of every later selected component. Its task must import or
select the component, bind its complete immediate boundary, formally compile
it, and prove it in the same blocked workload; then the original activation
task resumes. This is dependency-first recovery, not a pass/fail sweep of
libraries and not trace-selected leaf repair.
