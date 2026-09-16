# Project Status

## Current Work

**Active: M0 T413 S1.**

The owner closes M0 T412's Broker architecture scope and transfers former S12
to an independent queue-head proposal, now admitted as M0 T413 S1.
The [T412 closure](../history/m0-t412-broker-architecture-closure.md) maps
requirements to evidence and records the unresolved C-VID defect and receiver.
Former S12 is transferred, not completed. T413 is the sole active T;
S1 audits the complete original accessor profile and designs its restoration.

## Active Packet

### M0 T413 S1 — Original CCPU/C-VID accessor profile audit and design

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T413 S1; Ordinary Mode, source audit followed by independent final diff review. |
| Admission And Approval | Owner explicitly closes T412, promotes former S12 into its own proposal at queue head and authorizes immediate admission. This packet performs its complete audit/design; subsequent restoration and integration follow the proposal's sequential S boundaries. |
| Objective | Establish the actual accessor/layout fault boundary, complete selected-consumer/provider inventory and source-first restoration design, with measured removable and necessary diffs/overlays. |
| Non-goals | No presumption of CCPU instruction defects; no opcode/RMW/string/stack/fetch algorithm changes, broker redesign, host mutation, x87 restoration or full WRITE recovery. No untested candidate publication. |
| Reference Baseline | T412 closure at `21d5464a6`; S11 product source `3af4aa001`, published APP_VERSION 0.0.412 / protocol 3. Known prefilled 80x5 crash and original fault observations transfer intact. |
| Files And ABI Surface | Original vglob.c, CCPU/C-VID GDP and VideoVector headers/tables, selected consumers, existing binding overlay and build graph; focused read-only inspection and audit evidence. No runtime or public ABI change in this audit packet. |
| Applicable Rules | Execution recovery ladder and complete compatibility-profile gate; architecture/coding/document rules and source policy. |
| Verification | Enumerate all 38 getter/setter pairs and latch/three-selector exceptions; identify short/full vector consumers, GDP offsets, signature conversions, actual map providers, initialization and thread/reentry ownership. Compare selected original bodies with overlays and preserve exact source identities. Governance and link checks, actual diff review and Git synchronization. |
| Expected Markers | Every accessor and consumer has one source/layout/provider disposition; confirmed facts are separated from hypotheses; every restoration candidate has a source owner, required seam and focused plus integration verification. |
| Asset Needs | Existing source mirrors/read-only upstream and S11 logs; retained hashed patch under build/M0-T412/S12/admission. New artifacts under build/M0-T413/S1; observations only `O:\winnt\logs`. |
| Reporting Requirements | Root-cause confidence, complete four-rung ledger, measured mirror/overlay/autonomous removal estimates, justified retained bindings and whole-profile implementation/test plan. |
| Stop Conditions | Need for CPU algorithm changes or unrelated architectural expansion; unavailable provenance or historical interface outside the profile. Preserve evidence and revise scope rather than patch a trace leaf. |
| Exit Criteria | Whole-profile audit/design and quantitative disposition complete, all consumers accounted for, implementation and integration gates explicit, evidence committed/pushed and worktree clean. This does not claim the crash fixed. |
| Original Owner Request | “收口当前的T任务，然后把S12呢转成一个新的proposal加入到队列首位并且准入。” |
| Similar-Issue Sweep | All external accessors, full/five-slot layouts, GDP fields, selected providers, latch/selector exceptions and scratch/ring ownership; preserve adjacent Console/live-resize regressions in the integration plan. |
| Compatibility Profile | x86 /MT CCPU40 plus C-VID host accessor dispatch/layout. CPU instruction scalar/RMW/string/stack/fetch algorithms are outside this profile; no algorithm modification is inferred from the crash. |
| Candidate Proposal | [Original accessor/layout restoration](../proposals/proposal-ccpu40-cvid-accessor-layout-restoration-001.md) |

## Current Technical Baseline

- Published package: run16.exe, basesrv.exe and ntvdm.exe at `O:\winnt`;
  x86 /MT CCPU40, APP_VERSION 0.0.412, protocol 3.
- S11 P3 `3af4aa001` fixes the blank-gap regression; four-size ConPTY, native
  contract, mouse and 17 command tests pass. Exact final hashes are in the
  [S11 evidence](../etc/evidence/m0-t412-s11-console-handoff.md).
- Prefilled 80x5 C-VID crash remains unresolved. All accessor/layout work,
  saved WIP and adjacent live-resize verification transfer to the
  [new proposal](../proposals/proposal-ccpu40-cvid-accessor-layout-restoration-001.md),
  now admitted here and removed from [Queue](QUEUE.md).
- Owner Terminal/RDP visual confirmation remains distinct from automated
  evidence. Full WOW/WRITE and x87 restoration retain their separate owners.
- Runtime observations stay in `O:\winnt\logs`; builds remain below build/.
  No new-task binary is yet built or published. Before T413's first product
  build, shared APP_VERSION must advance to 0.0.413; current runtime and source
  version remain the unchanged T412 audit baseline.

## Recent M0 Closures

T412 Broker architecture closes by owner direction with explicit C-VID
transfer, not full worker compatibility. See its linked requirement audit.
T411 C-VID/event, T410 Console, T409 DOS services, T407 DPMI/XMS and T406
mapping retain their bounded conclusions.

## Recent Governance

Former T412 S12 admission `fb6d2d537` is superseded by an independent proposal
and sequential new-T admission. Its unverified patch is retained with a hash
in the closure record. The original Broker plan remains historical evidence.
