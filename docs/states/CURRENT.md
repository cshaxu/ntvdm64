# Project Status

## Current Work

**Active: M0 T413 S5** — NTVDMx64 exception-hook conditional-scope repair.

M0 T413 S4 completes the owner-requested autonomous-binding cleanup and
subsequently approved IP-wrap repair. The [closure audit](../history/m0-t413-accessor-restoration-closure.md#s4-cleanup-and-owner-expanded-ip-repair-closure)
and [S4 evidence](../etc/evidence/m0-t413-s4-binding-cleanup.md) qualify the
earlier S1-S3 closure. The owner now reopens T413 for the five related
NTVDMx64 exception-branch corrections. No next T is admitted; Queue is unchanged.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T413 S5, Ordinary Mode; P1 admission, implementation pending. |
| Candidate Proposal | [Original CCPU recovery](../proposals/proposal-ccpu40-cvid-accessor-layout-restoration-001.md); owner-approved exception-profile extension recorded here. |
| Admission And Approval | Owner: “准入ntvdmx64的多处补丁修复 。新的s任务”. Admit the five previously identified conditional-scope fixes, not the complete ccpu.patch. Standing commit/push approval applies. |
| Objective | Restore original exception delivery when the protected-mode host hook does not handle the fault; continue directly only when it did. Make the existing real-mode divide fixture complete through its guest handler. |
| Non-goals | No general ccpu.patch import, x64/CPU30 build, x87/WOW/broker change, new exception engine, overlay provider or speculative opcode repair. Preserve original exception algorithms beyond the five conditional blocks. |
| Reference Baseline | eb0c4fa33 and [S4 evidence](../etc/evidence/m0-t413-s4-binding-cleanup.md). Both old and current workers time out on real-mode divide. Int0 is identical in OpenNT and OpenNT-4.5; NTVDMx64 supplies the five brace corrections. |
| Files And ABI Surface | c_xcptn.c: contributory_exception, contributory_idt_exception, DF, Int0, PF. Preserve host_exint_hook, do_intrupt, c_cpu_continue, saved EIP, error codes and fault flags. Patch provenance under src/mvdm-softpc-patch; minimal registered mirror edits only; focused tests/build and evidence registers. |
| Applicable Rules | Execution source-recovery and CPU-profile gates; source-policy NTVDMx64 exception; architecture/coding/document rules. No copied replacement function bodies or new runtime component. |
| Verification | Freeze patch source hash and exact selected hunks; mechanically compare all five original/patch/current blocks. Exercise real-mode, protected-mode hook handled/unhandled outcomes, fault flags, vector/error code and saved-IP/stack contracts. Old-code negative controls must reproduce incorrect continuation. Then fresh x86 /MT build of all three EXEs, real divide and existing instruction/accessor/COMMAND/EDIT/MEM/Console regression profile. Documentation gate and git diff --check for delivery. |
| Expected Markers | Real divide reaches installed INT 0 handler and exits successfully; no timeout relabelled pass. Five blocks obey the same hook disposition; no unconditional pre-delivery continue on unhandled faults. Existing repaired accessor/IP tests retain their results. |
| Asset Needs | O:\repos.external\ntvdmx64 commit 84a13d2e7bb1a55d11148971e5b9c8ec99f670bf, ntvdmpatch/patches/common/ccpu.patch SHA-256 D3EA35D3F7C3BD4617F5F6EDC90725D3A70F503E353677C251720D9E5C9929FC. Selected original OpenNT mirrors, existing MSVC x86 toolchain and fixtures. Build root build/M0-T413/S5; only formal EXEs published to `O:\winnt`; runtime logs under `O:\winnt\logs`. |
| Reporting Requirements | Name each adopted hunk, origin, exact mirror delta and zero/new overlay accounting. Separate focused exceptional-path proof from real guest integration; retain failures and report unsupported protected-mode/fault combinations rather than claiming full compatibility. |
| Stop Conditions | Stop and report if the five fixes require an unrelated handler/CPU algorithm, host-kernel emulation or broader patch family. A passing divide alone cannot close the other four branches. |
| Exit Criteria | All five corrections have source provenance and complete handled/unhandled profile proof; actual divide fixture passes; formal package regression and deployment hashes verified; debt disposition updated only with evidence; reviewed commit/push and clean tree. Admission P1 does not close S5. |
| Original Owner Request | “准入ntvdmx64的多处补丁修复 。新的s任务”. |
| Similar-Issue Sweep | Audit every NTVDM host_exint_hook conditional in c_xcptn.c, including already-correct benign_exception. Check normal scalar/RMW/string paths remain unchanged, stack/error-code delivery and fetch/restart boundaries are preserved; tests cover affected exception control rather than inventing new instruction semantics. |

## S1 Closure Record

23a4fa83c: [source audit](../etc/evidence/m0-t413-s1-accessor-layout-audit.md).

## S2 Closure Record

3cc7851e5: [accessor restoration](../etc/evidence/m0-t413-s2-accessor-restoration.md).

## S3 Closure Record

d9d814b00 and fbb121d72: [integrated verification](../etc/evidence/m0-t413-s3-integrated-verification.md).

## S4 Closure Record

eb0c4fa33: [binding cleanup and IP repair](../etc/evidence/m0-t413-s4-binding-cleanup.md).
All preceding deliveries are committed and pushed; none is deferred.

## Current Technical Baseline

- O:\winnt contains the three formally built x86 /MT CCPU40 programs,
  APP_VERSION 0.0.413 / protocol 3. Published hashes match S4 formal products.
- All 76 field-access bodies are unchanged original cpu/src/evid/vglob.c,
  called through original egacpu.h declarations. Autonomous accessor table
  rebinding and all 20 typed conversion wrappers are removed. A registered
  seven-line header seam preserves the full original latch/table layout.
- The original IP-update and page-advance expressions are restored, including
  masking the complete USE16 sum. The retired x64 helper is removed.
- Full accessor/layout/concurrency fixture, 168 IP-expression cases and 11
  functional native instruction cases pass. All 17 command cases, four ConPTY
  sizes, mouse, live resize, native resize contracts and five prefilled 80x5
  EDIT runs pass, including 20 post-EDIT MEM completions.
- Real-mode divide-by-zero still times out on both the prior and current
  worker through unchanged original Int0/NTVDM continuation. It is explicit
  active S5 work, not a functional test pass or a repaired fault-delivery claim.
- Mirror diff distance: 417 versus pre-task 506. Four-overlay cohort: 125,
  back to its pre-task size. Generated runtime binding include loses 176 lines.
  No algorithm is moved into an adapter to disguise its footprint.
- Formal products: build/M0-T413/S4/formal. Fixtures and recoverable previous
  products: build/M0-T413/S4/integration. Runtime logs: O:\winnt\logs only.
  User Terminal/RDP visual acceptance is separate and not asserted as received.

## Recent M0 Closures

T413 S4 supersedes the earlier accessor-binding and IP-exclusion dispositions;
see its requirement/evidence closure above. T412 Broker remains closed within
its recorded architecture. T411/T410/T409/T407/T406 retain their own scopes.

## Recent Governance

S4 P2 is eb0c4fa33. The owner-approved next S reopens this same T without
changing Queue order. This admission changes documentation only, not source
or published products. The next T candidate remains x87 host-layout restoration.
