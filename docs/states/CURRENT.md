# Project Status

## Current Work

**Active: M0 T404 S3**

## Active Packet

### M0 T404 S3 P11 — native Console OpenNT mouse-contract recovery

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T404 S3 P11, Ordinary Mode. P10's raw-Console movement suppression is withdrawn: it changed original OpenNT guest delivery and reused an observer readiness marker absent from the P8 source. |
| Admission And Approval | Owner approved on 2026-09-11: “目标是参照opennt的mvdm实现同样功能，本来唯一区别应该只是如何和现代terminal/conhost去集成。” and “帮我恢复状态并正确修复！” |
| Candidate Proposal | [Native Console OpenNT mouse-contract recovery](../proposals/proposal-native-console-opennt-mouse-contract-recovery-001.md) |
| Reference Baseline | The selected x86 `ntvdm32.exe`; OpenNT `mouse_io_interrupt()` exits stream I/O before any INT 33h mouse service, then `host_disable_stream_io()` performs the original Console/window transition and enables mouse input. P8 forced input records before that transition. |
| Applicable Rules | [Execution](../rules/EXECUTION.md), [architecture](../rules/ARCHITECTURE.md), [coding](../rules/CODING.md), [document](../rules/DOCUMENT.md), and the [source policy](../etc/operations/policy/source-policy.md). |
| Objective | Restore the original OpenNT mouse event and INT 33h delivery shape. Establish a truthful modern `conhost`/Terminal adapter readiness boundary at the original stream-to-window transition while retaining `CX/DX` as the authoritative absolute guest hit-test coordinate. |
| Non-goals | Filtering guest movement, modifying guest media, BOP routing, callback mask/button/absolute-position semantics, treating relative motion as the coordinate source, the self-painted presentation window, a new generic input broker, or cross-process broker work. |
| Files And ABI Surface | P10 removal in `softpc.new/host/src/nt_event.c`; original Console transition owners `softpc.new/{base/keymouse/mouse_io.c,host/src/{nt_fulsc.c,nt_hosts.c}}`; and the observer only if it needs a truthful default-off host witness. The pre-existing INT 33h callback ABI remains `AX` reason, `BX` buttons, `CX/DX` absolute position and `SI/DI` relative motion. |
| Verification | Source-contract audit; focused x86 compile/link; bounded real-Console observation anchored at original INT 33h stream-to-window transition rather than an invented stage; mode and event-path proof; owner-visible EDIT.COM acceptance after Escape; governance and diff checks. |
| Expected Markers | No P10 gate or stage 8 reuse; original mouse movement/press/release delivery remains intact; a no-button Console move yields `AX=1`, `BX=0`, and mapped absolute `CX/DX`; observed Console mode changes only after original mouse activation; staged x86 executable. |
| Asset Needs | Existing text-mode `EDIT.COM` and its original `QBASIC.EXE` companion only; no new guest binary, driver, firmware, or Microsoft component. |
| Reporting Requirements | Record P10 withdrawal, the P8 observer limitation, source-first recovery classification, focused build result, truthful mode/event evidence, staged executable identity, and owner acceptance limitation in indexed evidence. |
| Similar-Issue Sweep | Inspect the original stream-to-window activation, Console mode ownership, pointer lifecycle and observer readiness only; do not alter guest mouse providers or presentation rendering. |
| Stop Conditions | Any need to change guest mouse logic/media, BOP routing, callback masks/button/absolute-position semantics, graphics semantics, or introduce a non-OpenNT host behavior requires renewed admission. |
| Exit Criteria | P10 is removed; the original event route is intact; a modern Console readiness observation corresponds to a real original transition; any retained adapter change is source-first justified; the reviewed product is staged and owner-accepted in ordinary `cmd/conhost` EDIT.COM. |
| Original Owner Request | “在该T任务中准入一个月S任务支持鼠标；这个鼠标支持已经在另一个工作树中可直接导入；完成提交推送后，可清除队列中相应的鼠标任务。” |

P8 evidence: [native Console mouse-input recovery](../etc/evidence/m0-t404-s3-p8-native-console-mouse-recovery-001.md), superseded for the stream-transition/readiness claim by [P11 native Console OpenNT mouse-contract recovery](../etc/evidence/m0-t404-s3-p11-native-console-opennt-mouse-contract-recovery-001.md). P9 withdrawal: [native Console mouse-vector hypothesis withdrawal](../etc/evidence/m0-t404-s3-p9-native-console-mouse-vector-recovery-001.md). P10 withdrawal: [native Console mouse-delivery policy](../proposals/proposal-native-console-mouse-delivery-policy-001.md). Final owner-visible Console acceptance remains pending.

## S1 Closure Record

[T404 S1 WOW hard-error recovery evidence](../etc/evidence/m0-t404-s1-wow-hard-error-recovery-001.md).

## S2 Closure Record

[T404 S2 DOSX BOP FD contract audit](../etc/evidence/m0-t404-s2-dosx-bop-fd-contract-audit-001.md).

[M0 T403 x64 mirror and mapping retirement](../history/m0-t403-x64-mirror-retirement.md)
removed the mapping manager, identity/token paths, and active x64-only
overlays from the sole Win32/x86 product. Formal x86 link, I386 staging,
focused Redirector validation, governance checks, and owner runtime testing
passed.

## Latest Product Closure

[M0 T401 WRITE diagnostic reconciliation](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t401-s4-write-compatible-dc-non-equivalence-001.md).

## Latest Governance Closure

M0 Td S5 P1 clarified the unadmitted cross-process-broker proposal: `run16`
is the standalone BaseClient/CLI entry, `basesrv` owns broker selection and
cleanup, and `ntvdm` returns to the worker role.  It records a one-minute
idle-worker policy as a future product policy, not as an OpenNT fact.  No
product source, runtime, ABI, Queue order, or candidate admission changed.

M0 Td S2 P1 strengthened the governance gate with fixed-topology, compact
state, supporting-index, encoding, relative-link and anchor checks; it also
reconciled the public README, product UX, closed-evidence lifecycle and a
historical encoding defect. NXVM governance materials were read-only quality
references; no product, ABI, build, runtime or queue behavior changed.

M0 Td S3 P1 added governance self-test and CI execution, explicit one-candidate
one-proposal Queue rows, and mechanical Queue/TODO schema checks. No product,
ABI, build, runtime or queue ordering changed.

## Recent M0 Closures

| Task | Compact result |
| --- | --- |
| T403 | x64 mapping and overlay residue retired; formal x86 build and owner test passed. |

## Recent Governance

- **M0 Td S5 P1:** clarified the candidate-2 standalone broker split and proposed cleanup policy; no product behavior changed.
- **M0 Td S2 P1:** fixed topology, links, index, encoding, and compact status controls.
- **M0 Td S3 P1:** CI, self-test, and one-candidate/one-proposal queue controls.
- **M0 Td S3 P2:** identifier, authority schema, path allowlist, and queue/TODO checks.

## Current Technical Baseline

- The sole product and acceptance executable is the Win32/x86
  `ntvdm32.exe`, using the original SoftPC CCPU40 profile. Native x64 is not a
  product, build, runtime, or acceptance target.
- M0 T395 (CPU40 DPMI32/bootstrap), M0 T396 (WOW32 first-dispatch frontier),
  M0 T397 (first WOW16 UI/service boundary), M0 T400 (WRITE observation
  boundary), M0 T401, M0 T402, and M0 T403 are closed with their linked
  evidence and history.
- Candidate packages are authoritative only in [QUEUE.md](QUEUE.md). No
  candidate is a numeric task or active packet until owner admission here.

## Historical Status Ledger

The complete pre-compaction status ledger is retained unchanged in
[history/status-ledger-snapshot-20260910.md](../../artifacts/documentation-archive/20260910/etc/evidence/status-ledgers/status-ledger-snapshot-20260910.md).
Use it only for historical lookup; it does not define current work.

## Status Hygiene

This page contains only current packet state, compact baseline, and links to
closed records. Detailed run chronology belongs in `docs/etc/evidence/`;
closed-task facts belong in `docs/history/`.
