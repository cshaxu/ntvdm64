# Project Status

## Current Work

**Active: M0 T450 S5**

## Active Packet

### M0 T450 S5 — normalize Run16's Console entry contract

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T450 S5, Ordinary Mode. |
| Admission And Approval | The owner approved converting the existing CUI `run16` entry from `wWinMain` to `wmain`, retaining an untouched raw command tail and adding coverage for redirection, pipelines and `cmd /c` arguments on 2026-10-23. |
| Objective | Normalize `run16` to its actual Console-subsystem contract without changing command-tail semantics or Console inheritance. |
| Non-goals | No GUI-subsystem launcher, no NTSRV/NTCON/worker lifecycle change, no guest change, and no command-shell parsing or reserialization by `run16`. |
| Reference Baseline | `244450652`: T450 S1–S4 style removal and ten-image publication are complete; the current launcher remains `/SUBSYSTEM:CONSOLE` but uses a `wWinMain` entry override. |
| Files And ABI Surface | `run16` entry, raw-tail helper, x86/x64 Ninja entry metadata and the existing launch-options test. No RPC or guest ABI changes. |
| Applicable Rules | [Execution](../rules/EXECUTION.md), [architecture](../rules/ARCHITECTURE.md), [coding](../rules/CODING.md), [source policy](../etc/operations/policy/source-policy.md). |
| Verification | Passed: `build/t450-s5/focused` reports `RUN16-LAUNCH-OPTIONS-PASS cases=22`; generated x86/x64 launcher graphs retain `subsystem = console` / `wmainCRTStartup`; `component-build-r2` passed all ten images and a 1,476-intermediate no-op increment; stage and dual-destination publication hashes passed. An installed `cmd /c ver` observation produced the expected version text and Run16 exited, but its ad-hoc redirected-pipe harness could not naturally close because the child chain retained the pipe; it is recorded as observation, not a runtime pass. |
| Expected markers | `run16` stays `/SUBSYSTEM:CONSOLE`; its only entry override is `wmainCRTStartup`; `>`, `>>`, `|`, quoted paths and `cmd /c ver` remain exact target-tail text. |
| Asset Needs | No new assets or guest changes. |
| Reporting Requirements | Record link contract, raw-tail cases, build/publication hashes and any intentionally unchanged Console behavior. |
| Stop Conditions | Pause only if the C runtime entry change requires a new public launch semantic or exposes an unrelated legacy command-tail contract. |
| Exit Criteria | Source review, focused raw-tail test, coherent ten-image package publication and hash verification, then commit/push and owner acceptance. Delivery evidence is [T450 S5 Run16 entry evidence](../etc/evidence/m0-t450-s5-run16-entry.md). |
| Original Owner Request | “批准，请你做这个修改，并确保已有测试能够覆盖尾部情况 ( \">\", \">>\", \"|\" 管道和程序参数如 \"cmd /c ver\"之类)” |
| Similar-Issue Sweep | Check both x86 and x64 generated graph owners and preserve the existing parser's no-allocation target-slice rule. |
| Candidate Proposal | [Win16 Style removal](../proposals/proposal-win16-style-removal-001.md), whose completed T450 boundary is retained while S5 makes this launcher-only correction. |

### Planned sequential S boundaries

1. **S1 (complete):** removed style selection and propagation control plane.
2. **S2 (complete):** deleted the isolated renderer/demo/test/build component and all style-only release/deployment entries; the package is reduced to ten host images.
3. **S3 (complete):** normal no-style launch, rejection and stale-reference audits passed; publication and hash verification passed.
4. **S4 (complete):** closure record and final commit/push completed at
   `db91004e5`; the ten-image package is deployed.
5. **S5 (active; delivery ready for owner acceptance):** normalized the
   retained CUI Run16 entry, proved raw-tail preservation, and published the
   ten-image package without adding a GUI-launcher mode.

## S1 Closure Record

[T450 S1--S4 closure ledger](../etc/evidence/m0-t450-s1-s4-closure-ledger.md)
records the published removal/control-plane package and its retained evidence.

## S2 Closure Record

[T450 S1--S4 closure ledger](../etc/evidence/m0-t450-s1-s4-closure-ledger.md)
records the renderer/demo removal and reduced ten-image package evidence.

## S3 Closure Record

[T450 S1--S4 closure ledger](../etc/evidence/m0-t450-s1-s4-closure-ledger.md)
records normal-launch/rejection and publication evidence.

## S4 Closure Record

[T450 S1--S4 closure ledger](../etc/evidence/m0-t450-s1-s4-closure-ledger.md)
records closure audit and owner-acceptance boundary evidence.

## Current Technical Baseline

The pending T450 release is a ten-image package. All release images match at
`assets/release/` and `O:\winnt\system32`; the three former style binaries are
absent. Immutable guest media and guest configuration remain unchanged. The
T450 closure record carries the build, runtime and publication evidence.

The ordered WOW32 capability packages remain in [Queue](QUEUE.md). Existing
unplanned debt remains in [TODO](TODO.md); it is not implicitly admitted by
T450.

## Recent M0 Closures

| Task | Disposition |
| --- | --- |
| T449 | [Owner-accepted WOW32 execution continuation closure](../history/m0-t449-closure.md). |
| T448 | [Owner-accepted lifecycle closure](../history/m0-t448-closure.md). |
| T447 | [Owner-directed scope-transfer closure](../history/m0-t447-transfer-closure.md), not full WOW functional acceptance. |

## Recent Governance

T449 closure records the accepted S4 physical application witness and the
next candidate's non-admitted status. No product binary changed in this
documentation-only delivery.
