# Project Status

## Current Work

**Active: M0 T484 S6** (Ordinary Mode). S5 closed at P5 d2c25de2b
with shared PIF and INI owners verified. S6 is the final CMD migration
acceptance step. S7 remains queued within T484.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T484 S6, Ordinary Mode. |
| Admission And Approval | Continuous T484 execution was approved. [Sequenced acceptance](../etc/operations/t484-cmd-build-release-plan.md). |
| Original Owner Request | Remove Python, PowerShell and JavaScript build dependencies. Use CMD for actual project build, generator, fixture and publication entry points. |
| Objective | Independently validate clean and cached CMD-only builds, input invalidation, failure handling, coherent publication, and applicable runtime regression on the final consolidated graph. |
| Candidate Proposal | [Unified build/release](../proposals/proposal-unified-build-cmd-release-001.md). |
| Reference Baseline | S5 P5 d2c25de2b. Current published host identity and the fifteen-file addon manifest are comparison inputs. |
| Files And ABI Surface | Build, code generation, fixture production, staging, publication, and their test/evidence paths. Product ABI, mirrored source and original guest files stay fixed. |
| Applicable Rules | CMD-only project scripts, source-first checks, release consistency, existing test ownership, and LF for authored text. |
| Verification | Clean bootstrap and all eleven host images and authored addons; no-op and changed-input rebuilds; sanitized tool environment and child command evidence; outside-root invocation, missing prerequisite and failed publication cases; relevant runtime gates and both publication hashes. |
| Expected Markers | Independent clean receipts and cache measurements, explicit failure exits, matching source/artifact identities, reviewed toolchain trace and actual test witnesses. |
| Asset Needs | Existing published addons, approved guest fixtures, native compiler/assembler/toolchain, and an owned short build root. |
| Non-goals | No original guest rebuild, unrelated runtime change, mirror rewrite, or S7 implementation. |
| Similar-Issue Sweep | All actual producer and publication paths, including nested native graph invocations and test addons. |
| Reporting Requirements | Record exact tool commands, graph roots, timestamps/hashes, failure and runtime gate results, limitations, and pushed commit. |
| Stop Conditions | Preserve unrelated changes; investigate any mismatch or missing gate and keep the acceptance open until resolved or explicitly scoped. |
| Exit Criteria | Clean and cached build, publication, failure checks and applicable runtime regression independently verified against current source, committed and pushed. |


## Current Progress

S6 P1 verifies the clean and cached host/addon builds, selected fixture graphs, staged manifests and negative cases. The same-source-path x86 control reduces both path-sensitive comparisons to PE timestamps. Full passed 20/20 on the current release. Live launcher matrices remain in progress. [Evidence](../etc/evidence/t484-s6-clean-cmd-acceptance.md).

S5 P5 promotes the existing guest INI editor to INI.EXE and removes both launch scripts' duplicate INI writers. The released tool passed Win31 six-profile, Win95 four-profile and apply/unapply package verification. [Evidence](../etc/evidence/t484-s5-shared-ini.md).

S5 P4 replaces the standard and NT-only native PIF writers with authored layouts and the existing PIF editor. Both layouts and the x86/x64 native payload checks passed. [Evidence](../etc/evidence/t484-s5-pif-layouts.md).

S5 P3 consolidates the keyboard profile through the released PIF editor. Byte equivalence and the real DOS keyboard startup and reader checks passed. [Evidence](../etc/evidence/t484-s5-keyboard-pif-consolidation.md).

S5 P2 consolidates pure DOS and XMS through the same CMD/PIF owner and retires two native writers. Five variants are byte-identical and two differ only in unused legacy string tails. Actual pure DOS and XMS cases passed. The cached host build and host/addon hashes match the existing release. [Evidence](../etc/evidence/t484-s5-pure-xms-pif-consolidation.md).

S5 P1 consolidates EMS profile generation through the current PIF editor and CMD. Native field tests, byte equivalence, two real EMS cases and a released-tool EMS case passed. All fourteen addons were verified and only PIF.EXE changed. The two host publication sets retained all eleven hashes. [Evidence](../etc/evidence/t484-s5-ems-pif-consolidation.md).

Earlier S4 preparation deliveries are indexed in [supporting material](../etc/README.md). The [S3 P181–P213 delivery record](../etc/evidence/t484-s3-early-deliveries.md#p181-p213-deliveries) retains those batch results. The [S4 closure record](#s4-closure-record) identifies the final batch and runtime boundaries.

## S5 Closure Record

[Existing INI editor consolidation](../etc/evidence/t484-s5-shared-ini.md)
and the [last PIF producer delivery](../etc/evidence/t484-s5-pif-layouts.md)
close S5 at d2c25de2b. Six native PIF writers were retired. The one existing
INI owner was promoted and two duplicate CMD writers were removed. Current
launcher/package tests and cached builds passed, both host sets were checked
and the addon release now has fifteen verified files. Runtime acceptance and
clean dependency tracing continue in S6.


## S4 Closure Record

[P46 and final S4 matrix evidence](../etc/evidence/t484-s4-matrix-preparation.md)
record the last delivery, pushed as 5b88ee331. All 355 frozen candidates
and the supplemental staging chains have a disposition. The published
host sets were hash-verified. Recorded Win95 runtime limitations remain
open within their stated boundaries. Clean acceptance of the final
consolidated build and publication graph remains S6.

## S3 Closure Record

[S3 closure and S4 handoff](../etc/evidence/t484-s3-closure.md) records
the delivered addon/test producers through P217, actual validation and32
unresolved inventory entries plus24 supplemental preparation chains retained
for S4. Runtime failures remain explicit; final clean acceptance remains S6.

## S2 Closure Record

The [candidate reconciliation](../etc/evidence/t484-progress-reconciliation.md)
tracks all355 original paths and separates file changes from completed
dispositions. Remaining disposition work is explicit; batch counts are not a
completion percentage.

[S2 P7 closure](../etc/evidence/t484-s2-worker-cmake.md), commit ce4fc0f9c,
completes the native host composition and CMD entry. All11 images are
published to both locations under package d8983d2b-3fe1-4d18-8ea9-64613c84346c.
The final worker reconciliation covers791 compiler profiles,67 archives,
153 links and274 export lines. Prior S2 batches and their actual comparison
limits are indexed in [supporting material](../etc/README.md).
S3 was admitted under the owner's continuous T484 authorization.

## S1 Closure Record

[S1 inventory/design](../etc/evidence/t484-s1-build-inventory.md) fixes the
ordered worklist:817 scanned inputs,355 migration candidates and1116 literal
references. These are lexical inventory counts, not proved live dependencies.
No production build/publication/runtime pass is attributed to S1.

## Current Technical Baseline

[T483 closure](../history/m0-t483-closure.md) indexes all S1-S21 outcomes,
retained mirror decisions and known boundaries. Final production revision is
9e7eb19c0 (S20 P85), package100b16d9-992d-4e2c-8d99-66a4e891dd26.
The current publication is T484 S2 P7, recorded above; P85 is its behavior
comparison baseline. P85 Full20/20 runtime evidence is reused for this
byte-equivalent/timestamp-only build change, not reported as a new Full run.

S19 completed38/38 B-to-C2 files; S20 completed94/94 B-to-C1 files.
Private compiled headers/search roots and linked implementation dependencies
from B to C1/C2 are zero. Legal synchronous callbacks and original named-image
APIs remain explicit; C1/C2 preserve original OpenNT connections.

P85 non-error mirror delta+4points/+244lines; S20 cumulative+15/+815;
S19 cumulative+12/+574. Retained decisions are not blanket-approved by closure.
The inherited environment failure and other boundaries remain in
[TODO](TODO.md) and the closure record. No product or guest change in S21.

## Recent M0 Closures

- [T483](../history/m0-t483-closure.md).
- [T482](../history/m0-t482-closure.md).
- [T481](../history/m0-t481-closure.md).

## Recent Governance

S5 uses the approved existing-editor consolidation and leaves final clean acceptance to S6.
