# Project Status

## Current Work

**No active M/T/S packet. M0 T437 S7 is delivered; any follow-up requires a
new owner admission.**

## S7 Delivery Packet

### Win1.01 Setup completion receipt

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T437 S7`, Ordinary Mode; reopening after S6 closure. |
| Candidate Proposal | [Windows 1.01 EGA/InPort runtime](../proposals/proposal-windows-101-ega-inport-runtime-001.md) — bounded Setup-return repair. |
| Admission And Approval | Owner reports original Setup appears to close but `PATCH\SETUP.CMD` remains blocked until Ctrl+C; owner requests an S repair. |
| Objective | Make an ordinary Win1.01 Setup PIF return through the existing run16/NTSRV/NTVDM completion contract after its guest process exits. |
| Non-goals | Do not alter Setup guest media, treat Ctrl+C as success, change unrelated PIF/DOS completion semantics, or modify Win3.1 packaging. |
| Reference Baseline | S5 media-root driver replacement and S6 reversible recovery; existing `SETUP.CMD` calls `run16` synchronously before installed-profile creation. |
| Files And ABI Surface | Initially diagnostic scripts/evidence; any later product caller/worker/service change requires narrowed source ownership and runtime/publication gates. |
| Applicable Rules | Execution, architecture, coding, documentation and source-policy authorities; normal run16 receipt semantics remain authoritative. |
| Verification | Reproduce with an owned prepared fixture or retained field evidence; prove guest exit, receipt delivery, `run16` return, post-Setup profile prompt and no regression in a normal DOS/PIF wait. |
| Expected Markers | One direct request receives a matching terminal receipt; `SETUP.CMD` prints `Original Setup returned` without Ctrl+C and reaches its prompt. |
| Asset Needs | Existing tools, selected media only with owner authorization, and build-scoped logs/fixtures. |
| Reporting Requirements | Record exact reproduction, process/receipt state, root cause, source ownership, focused positive/negative result and any retained manual boundary. |
| Stop Conditions | Pause for guest-media modification, an unproven cross-component lifecycle redesign, inability to reproduce, or a cause outside project-owned code without an admitted repair route. |
| Exit Criteria | Source-backed diagnosis, minimal repair if project-owned, focused tests, required product gate if production code changes, governance/diff review, commit and push. |
| Original Owner Request | “用apply.cmd跑完了安装流程，安装程序结束之后它好像自己卡住了，我按了ctrl+c才显示出来” and “请你准入一个S修复”. |
| Similar-Issue Sweep | Check ordinary DOS/PIF completion and post-exit receipt handling without expanding into the deferred Win3.1 ordinary-Setup transition. |

## S1 Closure Record

See [S1 evidence](../etc/evidence/m0-t437-s1-installed-win31-tools.md).

## S2 Closure Record

See [S2 evidence](../etc/evidence/m0-t437-s2-single-tool-entrypoint.md).

## S3 Closure Record

See [S3 evidence](../etc/evidence/m0-t437-s3-pif-hash-utilities.md).

## S4 Closure Record

See [S4 evidence](../etc/evidence/m0-t437-s4-win101-cmd-orchestration.md).

## S5 Closure Record

See [S5 evidence](../etc/evidence/m0-t437-s5-win101-reversible-media-mouse.md).

## S6 Closure Record

See [S6 evidence](../etc/evidence/m0-t437-s6-win101-media-unapply.md).

## Current Technical Baseline

The current ten-image release remains unchanged. T436 supplied approved,
identity-bound `KRNL386` and `WIN386` candidates plus released `MOUSE31.DRV`.
This task consumes those assets only for an existing, recoverable installed
Windows 3.1 tree; it does not publish a host release merely for tooling work.

| Architecture | Images |
| --- | --- |
| AMD64 | `run16.exe`, `ntsrv.exe`, `ntcon.exe`, `ntvwm.exe`, `ntmon.exe`, `nthook64.dll` |
| I386 | `ntvdm.exe`, `WOW32.DLL`, `VDMREDIR.DLL`, `nthook32.dll` |

## S7 Closure Record

Source review isolates the hold to the original PIF `CloseOnExit=0` inactive
session path, not guest Setup completion. The project-owned PIF creator now
encodes explicit `CloseOnExit` for every tool-generated PIF whose paired
command file waits synchronously, including Win1.01 `WIN101.PIF` and Win3.1
standard/enhanced launch profiles; generic user PIF creation remains
default-clear. Focused utility/package checks pass, and the owner verified the
repaired installed Win1.01 launch path. See the [S7 evidence](../etc/evidence/m0-t437-s7-win101-setup-close-on-exit.md).

## Recent M0 Closures

| Task | Outcome |
| --- | --- |
| T436 | Limited Windows 3.1 driver/setup delivery; ordinary Setup transition deferred. [Closure](../history/m0-t436-win31-setup-limited-closure.md) |
| T437 | Installed-tree portable launch and path repair. [Closure](../history/m0-t437-installed-win31-portable-launch-closure.md) |

## Recent Governance

T436 is closed at `7bd752b53`; T437 is closed after its portable installed-tree
tool delivery. Its ordinary-Setup transition recovery remains an unadmitted
queue-tail candidate.
