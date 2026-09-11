# Project Status

## Current Work

**Active: M0 T404 S3**

## Active Packet

### M0 T404 S3 P12 — native Console mouse interaction-contract recovery (delivered)

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T404 S3 P12, Ordinary Mode, delivered. P11's Console-mode timing experiment is withdrawn after owner runtime rejection; P7 is the restored input baseline. |
| Admission And Approval | Owner approved on 2026-09-11: “目标是参照opennt的mvdm实现同样功能，本来唯一区别应该只是如何和现代terminal/conhost去集成。” and “帮我恢复状态并正确修复！”。 After the callback-entry audit, owner expressly approved the single-point CPU40 repair: “ok 那你实施吧”. |
| Candidate Proposal | [Native Console mouse interaction contract](../proposals/proposal-native-console-mouse-coordinate-normalization-001.md); delivered evidence: [P12 audit](../etc/evidence/m0-t404-s3-p12-native-console-mouse-interaction-audit-001.md). |
| Reference Baseline | The selected x86 `ntvdm32.exe`; OpenNT `nt_event.c` consumes visible VDM display cells before `nt_mouse.c` maps them to the INT 33h callback. Public modern Console input reports screen-buffer cells. |
| Applicable Rules | [Execution](../rules/EXECUTION.md), [architecture](../rules/ARCHITECTURE.md), [coding](../rules/CODING.md), [document](../rules/DOCUMENT.md), and the [source policy](../etc/operations/policy/source-policy.md). |
| Objective | Recover CPU40's direct INT33 guest-callback entry from the demonstrated stale `-2` BOP adjustment, while retaining the original event queue and callback ABI; continue the separate Console input/output association audit without an unproven coordinate repair. |
| Non-goals | Changing CCPU decoding/execution, the global `HOST_BOP_IP_FUDGE`, CPU30, any other BOP route, guest media, callback mask/button/absolute-position semantics, coordinate translation, Console-mode timing experiments, a self-painted window, a generic input broker, or cross-process broker work. |
| Files And ABI Surface | `softpc.new/base/keymouse/mouse_io.c` for the selected CPU40 callback-entry seam; `host/src/{nt_hosts.c,nt_fulsc.c}` restores the P7 modern-Console mode boundary after P11 withdrawal. The callback ABI remains `AX` reason, `BX` buttons, `CX/DX` absolute position and `SI/DI` relative motion. |
| Verification | Focused x86 build; automatic after-modal-dismissal zero-button document-area move and press/drag/release sequences; verify callback entry is the registered offset and callback return remains intact; owner test in ordinary `cmd/conhost` EDIT.COM. |
| Expected Markers | CPU40 enters `371A:0223` rather than `371A:0221` for the observed EDIT handler; no-button movement no longer renders Help; press/drag/release preserve their original masks and return. |
| Asset Needs | Existing text-mode `EDIT.COM` and its original `QBASIC.EXE` companion only; no new guest binary, driver, firmware, or Microsoft component. |
| Reporting Requirements | Record P11 withdrawal, the withdrawn P12 coordinate shortcut, source/Console association evidence, formal x86 product identity, and owner acceptance in indexed evidence. |
| Similar-Issue Sweep | Inspect only CPU40 callers that use this direct registered callback form; do not alter guest providers, BOP decoding, or presentation rendering. |
| Stop Conditions | Any need beyond the selected CPU40 direct registered callback entry—especially CCPU, global BOP, guest mouse, callback ABI, coordinate, graphics, or presentation changes—requires renewed admission. |
| Exit Criteria | Met: the selected callback enters its registered first instruction; automatic controls show correct no-button and button-transition behaviour; the input/output association remains documented as separate; x86 build and owner `cmd/conhost` EDIT.COM acceptance passed. Any new BOP architecture work requires a new admission. |
| Original Owner Request | “在该T任务中准入一个月S任务支持鼠标；这个鼠标支持已经在另一个工作树中可直接导入；完成提交推送后，可清除队列中相应的鼠标任务。” |

P8 evidence: [native Console mouse-input recovery](../etc/evidence/m0-t404-s3-p8-native-console-mouse-recovery-001.md). P9 withdrawal: [native Console mouse-vector hypothesis withdrawal](../etc/evidence/m0-t404-s3-p9-native-console-mouse-vector-recovery-001.md). P10 withdrawal: [native Console mouse-delivery policy](../proposals/proposal-native-console-mouse-delivery-policy-001.md). P11 is withdrawn by P12. P12's [button-state, callback-entry, and acceptance record](../etc/evidence/m0-t404-s3-p12-native-console-mouse-interaction-audit-001.md) rejects the synthetic-held-button hypothesis, records the CPU40 direct-entry repair, and retains Console input/output association as a separate question. Owner accepted ordinary `cmd/conhost` EDIT.COM mouse use on 2026-09-11. The next T404 S requires new admission.

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
