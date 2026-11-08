# T481 S3 closure — VM handoff candidate

S3 closes the owner-frozen Win3.1 font-bank, CCPU access and dynamic directory
work. The entire T remains open. S4 owns the remaining Win95 lifecycle and
entry-path work; S5 owns combined completion and final acceptance.

| Requirement | Evidence and limit |
| --- | --- |
| CCPU access | `access391.json` compares the same 90-check protected-page fixture: old inverted CPL handling fails 30, selected handling passes 90. The CCPU40 mirror change is registered as MVDM-HOST-DIV-891. This is not proof of every CCPU/V86 case. |
| Per-VM directory | `directory498.json` observes dynamic DOSWOWDATA/NetCDS registration and two distinct directories in real Win3.1 VMs; scalar478 covers registration, identity and expired memory epochs. The guest probe is tied to one identified NTDOS image, not all DOS layouts. |
| Font banks | `font539.json` verifies distinct bank contents in two VMs through glyph offset 8175 and 320 callback checks. The independently authored VxD selects banks before its VMM-mediated virtual writer; original guest bytes are unchanged. |
| Win3.1 behavior | `editors535.json` records EDIT.EXE and EDIT.COM, both completion orders, keyboard, guest mouse menus and saved content; DOSONLY=1 rejects the host program and runs the guest editor. The owner also manually accepted both editors on candidate502. Actual 50-row visible geometry remains unverified on this 31-row-max desktop; 80x50 backing with 25 visible rows passes resize548. |
| Win95 assessment | `win95-editors593.json` records two distinct prompt VMs, keyboard, guest mouse menu, saved content and terminal completions using the diagnostic real-command host. Ordered wake/PIC/callbacks pass separately. Exact production-host prompt coverage, VMM blocked-bit interpretation, and Win95 exit-code semantics remain S4 boundaries. |
| Build, Full, publication | `qualification549.json` records the four Win3.1 standard/enhanced x DOSONLY matrix and Full549 16/16; `publication579.json` records eleven coherent host images, addon manifests, and W31 driver hashes. Both release destinations and W31 drivers were rechecked after publication. The package is a user-test candidate; owner has not yet reported manual results for candidate579. |

Win95 batch-launched native CMD produced empty output both with candidate and
historical host/VxD. It did not reach the selected interactive command BOP in
the retained trace. S4 must determine its original execution path and whether
it is a required part of the admitted goal; it is neither a successful test
nor evidence of an S3 regression. The detailed experiments, exact artifacts,
coverage-only boundaries and subsequent S4 work are in the
[working plan](../operations/m0-t481-multitask-completion-plan.md).
