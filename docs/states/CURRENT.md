# Project Status

## Current Work

**Active: M0 T413 S3.**

The owner closes M0 T412's Broker architecture scope and transfers former S12
to an independent queue-head proposal, now admitted as M0 T413 S1.
The [T412 closure](../history/m0-t412-broker-architecture-closure.md) maps
requirements to evidence and records the unresolved C-VID defect and receiver.
Former S12 is transferred, not completed. T413 is the sole active T;
S1 audit/design closed at `23a4fa83c`; S2 restoration closes at `3cc7851e5`.
The owner's repair admission continues into the proposal's S3 integration.
S3 automated verification is complete and final products are published for
owner Terminal/RDP confirmation; no next task is admitted.

## Active Packet

### M0 T413 S3 — Integrated C-VID regression and publication

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T413 S3; Ordinary Mode, integration followed by independent final diff review. |
| Admission And Approval | Owner: “准入 开始修复”. S2 P1 is committed/pushed with full-profile focused tests and three formal links; continue the same admitted repair into sequential integration/publication. |
| Objective | Verify restored original accessors against prefilled 80x5 and live-resize Console stress, COMMAND/EDIT/MEM, mouse and command regressions; publish only passing same-source products. |
| Non-goals | No presumption of CCPU instruction defects; no opcode/RMW/string/stack/fetch algorithm changes, broker redesign, host mutation, x87 restoration or full WRITE recovery. No untested candidate publication. |
| Reference Baseline | T412 closure at `21d5464a6`; S11 product source `3af4aa001`, published APP_VERSION 0.0.412 / protocol 3. Known prefilled 80x5 crash and original fault observations transfer intact. |
| Files And ABI Surface | Unchanged shared vglob.c; existing private C-VID binding and generator/build graph; focused original-owner fixtures; seven observation-only CCPU files and unselected ntstubs cleanup; registers/evidence; shared APP_VERSION 0.0.413. Original public VideoVector ABI is retained. |
| Applicable Rules | Execution recovery ladder and complete compatibility-profile gate; architecture/coding/document rules and source policy. |
| Verification | Enumerate all 38 getter/setter pairs and latch/three-selector exceptions; identify short/full vector consumers, GDP offsets, signature conversions, actual map providers, initialization and thread/reentry ownership. Compare selected original bodies with overlays and preserve exact source identities. Governance and link checks, actual diff review and Git synchronization. |
| Expected Markers | Every accessor and consumer has one source/layout/provider disposition; confirmed facts are separated from hypotheses; every restoration candidate has a source owner, required seam and focused plus integration verification. |
| Asset Needs | Intentionally reuse unchanged S2 original-library objects/toolchain; rebuild final complete-table publication binding in build/M0-T413/S2/formal. Exact final hashes and rebuild disposition are in S3 evidence. New harnesses under build/M0-T413/S3/integration; prior runtime backup there. Observations only `O:\winnt\logs`. |
| Reporting Requirements | Root-cause confidence, complete four-rung ledger, measured mirror/overlay/autonomous removal estimates, justified retained bindings and whole-profile implementation/test plan. |
| Stop Conditions | Need for CPU algorithm changes or unrelated architectural expansion; unavailable provenance or historical interface outside the profile. Preserve evidence and revise scope rather than patch a trace leaf. |
| Exit Criteria | Declared short-window and normal geometry runs complete without crash; native cell checks, mouse, command regressions pass; exact product hashes published, actual diff reviewed, evidence committed/pushed and worktree clean. Owner RDP visual confirmation is separate. |
| Original Owner Request | “准入 开始修复” |
| Similar-Issue Sweep | All external accessors, full/five-slot layouts, GDP fields, selected providers, latch/selector exceptions and scratch/ring ownership; preserve adjacent Console/live-resize regressions in the integration plan. |
| Compatibility Profile | x86 /MT CCPU40 plus C-VID host accessor dispatch/layout. CPU instruction scalar/RMW/string/stack/fetch algorithms are outside this profile; no algorithm modification is inferred from the crash. |
| Candidate Proposal | [Original accessor/layout restoration](../proposals/proposal-ccpu40-cvid-accessor-layout-restoration-001.md) |

## Current Technical Baseline

- S1 [audit and design](../etc/evidence/m0-t413-s1-accessor-layout-audit.md)
  accounts for all 29 changed CCPU/C-VID files (506 diff-distance lines) and
  125 overlay lines. All 38 original accessor pairs pass the x86 research
  fixture; nine short-vector consumers have no evaluated vector-field uses.
  The event-thread accessor route and shared-scratch corruption are identified.
  Seven original-file restorations remove 75 diagnostic lines. An independent IP-mask
  regression is recorded outside the admitted instruction-algorithm scope.
  S1 closes with its delivered audit. S2 [restoration evidence](../etc/evidence/m0-t413-s2-accessor-restoration.md)
  records all three links and complete original-library focused tests passing;
  final mirror distance is 419, and the retained overlay cohort is 136 lines.
  S3 [integrated verification](../etc/evidence/m0-t413-s3-integrated-verification.md)
  passes all 17 command cases, four ConPTY sizes, mouse/live resize, native
  resize contracts and five final-hash prefilled 80x5 runs (20 post-EDIT MEM
  completions). Original accessors resolve in the final worker map.
- Published package: run16.exe, basesrv.exe and ntvdm.exe at `O:\winnt`;
  x86 /MT CCPU40, APP_VERSION 0.0.413, protocol 3; exact hashes in S3 evidence.
- S11 P3 `3af4aa001` fixes the blank-gap regression; four-size ConPTY, native
  contract, mouse and 17 command tests pass. Exact final hashes are in the
  [S11 evidence](../etc/evidence/m0-t412-s11-console-handoff.md).
- The prefilled 80x5 shared-scratch failure is repaired by selecting original
  direct C accessors, with original latch/selector owners preserved. No CCPU
  instruction algorithm or renderer rewrite was required. Saved WIP is not
  an acceptance input; the task remains outside [Queue](QUEUE.md).
- Owner Terminal/RDP visual confirmation remains distinct from automated
  evidence. Full WOW/WRITE and x87 restoration retain their separate owners.
- Runtime observations stay in `O:\winnt\logs`; builds remain below build/.
  New x86 products are built at build/M0-T413/S2/formal with shared APP_VERSION
  0.0.413 / protocol 3. The deployed package matches the final build hashes.

## Recent M0 Closures

T412 Broker architecture closes by owner direction with explicit C-VID
transfer, not full worker compatibility. See its linked requirement audit.
T411 C-VID/event, T410 Console, T409 DOS services, T407 DPMI/XMS and T406
mapping retain their bounded conclusions.

## Recent Governance

Former T412 S12 admission `fb6d2d537` is superseded by an independent proposal
and sequential new-T admission. Its unverified patch is retained with a hash
in the closure record. The original Broker plan remains historical evidence.
