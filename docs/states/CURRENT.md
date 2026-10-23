# Project Status

## Current Work

### M0 T450 S4 — closure audit and owner acceptance

| Field | Record |
| --- | --- |
| Identifier / mode | M0 T450 S4, Ordinary Mode. |
| Admission and approval | The owner approved the queued Win16 Style removal package and directed execution through T closure, stopping before final owner acceptance on 2026-10-23. |
| Objective | Remove every project-owned command, wire field, RPC/reservation state and runtime branch whose sole purpose is selecting or propagating Win16 Style, while retaining ordinary launch, native classification, BaseSrv lifecycle, worker-base, NTHook and WOW behavior. |
| Non-goals | Do not import, link, test or promise compatibility with the separate `win16style` / `run16s` project; do not modify, build or patch guest media; do not remove ordinary NTHook propagation or queued WOW capability work. |
| Reference baseline | `fd1204f8c`, with the owner-accepted T449 thirteen-image package recorded at `44a0596e8` / `32f3ca2a1`. The removal proposal is [Win16 Style removal](../proposals/proposal-win16-style-removal-001.md). |
| Files and ABI surface | Expected: Run16 options/scope, `common/protocol/window_style.h`, native launch/hook/frontend records, BaseSrv client/service/command records, NTVM/NTVWM style callers, NTHook style bridge, WOW title policy and their feature-only tests. Ordinary program type, console, reservation and lifecycle fields remain. |
| Applicable rules | [Execution](../rules/EXECUTION.md), [architecture](../rules/ARCHITECTURE.md), [coding](../rules/CODING.md), [source policy](../etc/operations/policy/source-policy.md), and the proposal. |
| Verification | The full ten-image x86/x64 build and second no-op increment passed in `build/t450-s1/component-build-r6`; staged publication and dual-destination manifest hashes passed. Removed-option rejection and ordinary installed launch passed; the active source/build/release audit is clean. The closure record is [M0 T450 closure](../history/m0-t450-closure.md). |
| Expected markers | Both public style options are rejected; no `WINDOW_STYLE`, `Ntw16s`, or style-policy branch remains in retained production transport; normal launch data and NTHook propagation still have a single owner. |
| Asset needs | No new sources or guest assets. Existing immutable guest hashes remain unchanged. |
| Reporting | Record removed ABI fields, retained non-style fields, test evidence, release-set reduction and external-project boundary. |
| Stop conditions | Pause for a discovered consumer where style state is inseparable from an independently approved feature, or a proposed new external integration/semantic change. |
| Exit criteria | S1–S3 are complete. S4 records the closure audit, commits and pushes the ten-image package, then waits for owner acceptance. |
| Original owner request | “一样啊，我们：run16 -> CreateProcess -> NTSRV… [then] 这样吧你还是按照方案1，彻底清理掉win16 style部分，就在这个T任务中。帮我规划一下S任务划分。我后续还是会用run16s启动run16来尝试应用窗口风格的。…开始执行，到T收口前停下等我验收。” |
| Similar-issue sweep | Treat style-only release assets, CMake targets, test fixtures, evidence, package scripts and documentation as consumers; do not leave an Off-valued compatibility state. |

### Planned sequential S boundaries

1. **S1 (complete):** removed style selection and propagation control plane.
2. **S2 (complete):** deleted the isolated renderer/demo/test/build component and all style-only release/deployment entries; the package is reduced to ten host images.
3. **S3 (complete):** normal no-style launch, rejection and stale-reference audits passed; publication and hash verification passed.
4. **S4 (active):** closure record, final commit/push verification and owner acceptance.

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
