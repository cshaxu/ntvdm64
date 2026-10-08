# Project Status

## Current Work

**Active: M0 T437 S1**

## Active Packet

### Installed Windows 3.1 portable launch repair

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T437 S1`, Ordinary Mode |
| Candidate Proposal | [Installed Windows 3.1 portable launch repair](../proposals/proposal-win31-installed-launch-portability-001.md) |
| Admission And Approval | Owner directly admitted the installed-Windows-3.1 portable-launch task on 2026-10-08. |
| Objective | Deliver two bounded tools for an already installed Windows 3.1 tree: `tools/win31-launch` creates the self-contained compatibility `PATCH` and only the explicit `WINSTD`/`WIN386` launch profiles; `tools/win31-path` discovers and repairs relocatable Win3.1 system-path references. |
| Non-goals | `win31-launch` does not rewrite ordinary system paths, and `win31-path` does not install compatibility binaries or alter launch profiles. Neither tool runs/replaces original Setup, claims the deferred Setup transition is fixed, alters retail media, introduces a general guest patcher, promises enhanced-mode reliability, or changes host component lifecycle. |
| Reference Baseline | T436 closed with exact candidate identities, `MOUSE31.DRV`, and self-contained installed profiles, but normal Setup remains deferred. See the [limited closure](../history/m0-t436-win31-setup-limited-closure.md) and [deferred transition proposal](../proposals/proposal-win31-ordinary-setup-protected-mode-transition-001.md). |
| Files And ABI Surface | New project tooling under `tools/win31-launch/` and `tools/win31-path/`; if native finalizer code is needed, `src/addon/win31-launch/`; tests under `tests/component-integration/`. No host ABI/protocol change. |
| Applicable Rules | `docs/rules/EXECUTION.md`, architecture/coding/document rules, `CONTRIBUTING.md`, and the source policy's approved recoverable installed-copy exception. |
| Verification | Validate only fresh disposable installed-tree fixtures: `win31-launch` checks candidate/recovery hashes and generated PIF/CMD/NT profiles; `win31-path` checks discovery, precise relocation repair, idempotence and negative no-write cases. Establish selected standard/386-mode `run16` launches where the guest boundary is available. Run governance and diff checks. |
| Expected Markers | `win31-launch`: `PATCH\\WINSTD.CMD/.PIF`, `PATCH\\WIN386.CMD/.PIF`, shared `CONFIG.NT`/`AUTOEXEC.NT`, released `MOUSE31.DRV`, and recovery copies. `win31-path`: an explicit per-file relocation report with only proven stale paths rewritten. |
| Asset Needs | Existing `assets/release/MOUSE31.DRV` and add-on manifest; already approved candidate identities; owner-selected installed Win3.1 tree or a disposable fixture. No new guest media acquisition. |
| Reporting Requirements | Record actual discovered paths, `win31-path` rewritten configuration items, input/output hashes, success and negative results, and any unchanged-guest limitation. Clearly distinguish a portable installed-tree launch from Setup completion. |
| Stop Conditions | Pause for an unapproved retail-media/guest-binary change, a required candidate outside the two approved images, ambiguous installation topology, or any evidence that the tool would need to invent a Windows behavior rather than repair its own paths/profiles. |
| Exit Criteria | Reproducible tools and tests; self-contained installed PATCH; `WINSTD` and `WIN386` profiles start through plain `run16` on a valid tree; exact recoverable candidate/driver installation; independent configuration-relocation proof; no package/repository/default-profile dependency; documented limits. |
| Original Owner Request | “帮助已经安装好的干净win31增加patch目录…把win31变成绿色软件…不需要走完安装流程…最终目标由队尾T任务承接。” |
| Similar-Issue Sweep | Reuse the T436 installed-PATCH finalizer only where its hash/recovery/PIF logic matches; compare Win1.01 setup’s media-vs-installed separation; reject generated `WORK`, package-path, and fixed-drive dependencies. |

## Current Technical Baseline

The current ten-image release remains unchanged. T436 supplied approved,
identity-bound `KRNL386` and `WIN386` candidates plus released `MOUSE31.DRV`.
This task consumes those assets only for an existing, recoverable installed
Windows 3.1 tree; it does not publish a host release merely for tooling work.

| Architecture | Images |
| --- | --- |
| AMD64 | `run16.exe`, `ntsrv.exe`, `ntcon.exe`, `ntvwm.exe`, `ntmon.exe`, `nthook64.dll` |
| I386 | `ntvdm.exe`, `WOW32.DLL`, `VDMREDIR.DLL`, `nthook32.dll` |

## Recent M0 Closures

| Task | Outcome |
| --- | --- |
| T436 | Limited Windows 3.1 driver/setup delivery; ordinary Setup transition deferred. [Closure](../history/m0-t436-win31-setup-limited-closure.md) |

## Recent Governance

T436 is closed at `7bd752b53`. Its ordinary-Setup transition recovery remains
an unadmitted queue-tail candidate. T437 owns only installed-tree portability
and launch preparation; it must not absorb that outstanding Setup work.
