# T445 S1 retained closed packet

Moved verbatim from Current Work during the S2 admission to respect its 32 KiB
limit. Historical brief only; the active stage is controlled by Current Work.
Links below are adjusted for this evidence location. Closure commit: 2151cf750.
Closure evidence: [installation design](m0-t445-s1-installation-design.md).

## Closed S1 Packet

### Application HWND ownership and appearance-boundary audit

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T445 S1`, Ordinary Mode; one implementer with a separate final review pass. |
| Candidate Proposal | [Windows 3.1 window appearance](../../proposals/proposal-win31-window-appearance-001.md). |
| Admission And Approval | Owner directs: “好的，准入队列第一条”. Admit ownership/API audit and design first, not live window restyling. |
| Objective | Establish the actual host HWND/window-procedure owners for representative Win16, Win32 and Win64 GUI programs and design one shared non-client appearance implementation with two installation entries. First establish authentic Windows 3.1 visuals/interaction from original manuals and owner reference images, explicitly not Windows 95. |
| Non-goals | No NTCON, Console/text rendering, guest Windows desktop framing, client-pixel changes, lifecycle changes, global hooks, helper processes or guest/media modifications. No appearance implementation is delivered by admission. |
| Reference Baseline | T444 closure at `bc363f3d2`; accepted product `fb7650ce0`, release 0.0.444/protocol 45; proposal identifies WOW window/dialog bindings and dual native Hooks as candidate entrypoints. Those static paths require current verification. |
| Files And ABI Surface | Read WOW window/dialog/procedure bindings, nthook creation/installation and NTVWM launch; write indexed audit/design evidence and governance only. Owner subsequently approves src/win16-style placement, not source creation or production ABI implementation in S1. |
| Applicable Rules | Project execution, architecture, coding, document and source policies; background-owned probes if required; preserve unrelated changes; no computer use or subst. |
| Verification | Trace creation, dialogs, later GUI threads, title/activation, sizing, system menu/commands, DPI, procedure replacement and destruction. Record exact source locations and distinguish static findings from runtime witnesses. Admission-only delivery runs governance, relative-link and diff checks. |
| Expected Markers | Per-family owner/installation matrix, preserved callback and teardown ordering, explicit supported/unsupported HWND boundaries and a minimal shared implementation design. |
| Asset Needs | Existing accepted release and representative Win16 WINMINE plus source-owned x86/x64 native GUI fixtures; no new guest media. Runtime witnesses require owned test inputs and recorded identities. |
| Reporting Requirements | Explain common mechanism versus two installation adapters, source placement, message/geometry/DPI/accessibility contract, risks and decisions needing owner approval. Do not equate CreateProcess interception with HWND coverage. |
| Stop Conditions | Missing authenticated in-process installation boundary, need for mirror/guest change, global hook/helper, lifecycle authority or uncontrolled subclass/message interference requires owner review. |
| Exit Criteria | Met for audit/design: source-backed owner/field map, same-thread creation/subclass and teardown design, task-level inheritance, explicit real-caption marker semantics, shared source-owner admission and stage-specific positive/negative/lifecycle checks. Actual implementation proofs belong to S2/S3/S4 and are not claimed passed. Closure evidence is linked above. |
| Original Owner Request | “好的，准入队列第一条”. |
| Similar-Issue Sweep | A/W creation, dialogs, secondary GUI threads, child/owned/popup windows, application subclass replacement, failure and WM_NCDESTROY, custom-drawn frames and non-product launches. |
