# Project Status

## Current Work

**Active: M0 T481 S8 — Win3.1 full-screen mouse pointer investigation.**
S7 is owner-closed with the reported Win3.1 interaction and black-screen
results and the undelivered candidate and limitations documented in the
[S7 closure](../etc/evidence/m0-t481-s7-closure.md). After S8, S9 audits and
imports applicable recent SoftPC software-emulation fixes; S10 investigates
Win95 and completes its remaining multitask acceptance. The T remains open.
S8 source and test evidence is tracked in [mouse investigation](../etc/evidence/m0-t481-s8-mouse.md).
The full-screen pointer driver and the 11-image host package with the original
EGA cursor coordinate-restoration fix are published as an owner-test candidate.
Full selected gates and the Win3.1 four-case matrix passed; repeated CAF and
Alt+Enter acceptance and Win95 regression remain outstanding. S8 is not closed.
Owner reports the candidate generally works, but a windowed DOS EDIT.COM
session can show two malformed pointer blocks or crash after switching to full
screen, using CAF, returning to a window and entering full screen again. This
actual report is part of S8 and needs reproduction and diagnosis.

## Active Packet

| Field | Record |
| --- | --- |
| Candidate Proposal | [Guest multitask I/O completion](../proposals/proposal-guest-multitask-io-completion-001.md). |
| Identifier Mode | M0 T481 S8, Ordinary Mode. |
| Admission And Approval | Owner closed S7 and admitted S8 to investigate the Win3.1 full-screen mouse pointer, with repair and delivery under existing authorization. |
| Objective | Reproduce and identify why the mouse pointer disappears when switching EDIT.COM and EDIT.EXE sessions to full screen while mouse input continues; repair the reached cause. |
| Non-goals | Do not alter original guest binaries or assume the failures share a cause. |
| Reference Baseline | S7 owner-tested O:/w31 configuration and published S5 host and driver package; S6/S7 candidates are not a qualified release. |
| Files And ABI Surface | Inspect VM input, video and activation paths, INSIGNIA.386 and NTVDM64.386 interfaces, and production probes. |
| Applicable Rules | Execution, source policy, guest immutability and original-semantics-first. |
| Verification | Trace original and actual paths; compare window versus full screen and both editors using isolated facilities, verify pointer pixels as well as input and switching back, run applicable regressions, and verify released and O:/w31 deployed identities. |
| Expected Markers | Evidence identifies input destination, VM activation and display ownership at each failure or states missing observations. |
| Asset Needs | Existing installed Win3.1, released runtime and addon drivers, and existing test facilities. |
| Reporting Requirements | Describe reproducibility, causes, repairs, tests, deployment and limitations; distinguish facts from hypotheses. |
| Stop Conditions | Original guest modification or new observable semantic relaxation. |
| Exit Criteria | Reported failures repaired and verified, applicable coherent package built, published and hash-verified, O:/w31 updated, changes reviewed, committed and pushed. |
| Original Owner Request | In Win3.1, EDIT.EXE and EDIT.COM retain working mouse motion and clicks after switching full screen, but the mouse pointer disappears. |
| Similar-Issue Sweep | Compare single and multiple VM input routing, frame ownership, foreground switching and exit. |

## Current Technical Baseline

The eleven host components and W31 addon pair are published; see
[qualification](../../artifacts/t481-s5/qualification001.json),
[unified driver name](../../artifacts/t481-s5/unified-name.json), and
[S5 closure](../etc/evidence/m0-t481-s5-closure.md). Win95 has not yet received
owner manual acceptance. The reproduced DOS4GW inherited-environment issue and
Win95 exit-code limitation remain recorded and are not made passes by S6.

## Recent M0 Closures

- [T480](../history/m0-t480-closure.md): adjacent audit closure.
- [T479](../history/m0-t479-closure.md): guest multitask transferred to T481.

## Recent Governance

See the [execution rules](../rules/EXECUTION.md) for the current governance.
