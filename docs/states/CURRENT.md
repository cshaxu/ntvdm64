# Project Status

## Current Work

**Active: M0 T413 S4** — original C-VID composition and autonomous-binding cleanup.

The owner reopens T413 after challenging the retained autonomous binding.
The [earlier closure audit](../history/m0-t413-accessor-restoration-closure.md)
preserves S1/S2/S3 results, not proof that the binding is irreducible.
No next candidate is admitted; [Queue](QUEUE.md) keeps its existing order.

## Active Packet

| Field | Record |
| --- | --- |
| Identifier Mode | M0 T413 S4, Ordinary Mode; admission P1, implementation pending. |
| Candidate Proposal | [Original accessor restoration](../proposals/proposal-ccpu40-cvid-accessor-layout-restoration-001.md). |
| Admission And Approval | Owner request: append an S task to clean the autonomous binding; reopen the same T, without allocating a new T. Standing commit/push permission applies. |
| Objective | Recover the original C-VID source/header/table/build composition and remove superseded autonomous accessor rebinding, typed wrappers and generation logic while preserving the proven repair. |
| Non-goals | No IP-wrap/instruction, x87, WOW/WRITE, broker, guest-media or unrelated CPU-vector redesign. No relocation into adapters or generated files counted as deletion. |
| Reference Baseline | fbb121d72; [S2](../etc/evidence/m0-t413-s2-accessor-restoration.md), [S3](../etc/evidence/m0-t413-s3-integrated-verification.md). Mirror distance 419; four-overlay cohort 136 lines; added generator 29 lines and 20 generated typed bridges require explicit disposition. |
| Files And ABI Surface | Original cpu/src/evid/vglob.c, C-VID tables and headers, setup hook, paired vector-binding overlay, GenerateCvidcCpuBinding.mjs, source/build manifests, focused tests and divergence registers. All 38 accessor pairs, latch/selector slots, GDP layout and publication ordering remain in scope. |
| Applicable Rules | docs/rules/EXECUTION.md source-recovery and profile gates; architecture, coding, document rules; source policy before original-source research. |
| Verification | Admission: documentation/link gate and git diff --check. Implementation: original composition comparison and practical compile/link attempts; complete accessor/layout/scratch/concurrency fixture; fresh x86 /MT three-program build; S3 command, short-window, mouse and resize regression profile. |
| Expected Markers | Final map identifies original providers; complete field and scratch-preservation tests pass; obsolete binding/wrappers/generator output absent where replaced; no displaced autonomous implementation; measured retained exceptions. |
| Asset Needs | Selected pinned original OpenNT sources; existing MSVC x86 toolchain and T413 fixtures; existing O:\winnt guest media. Build products under build/M0-T413/S4; runtime observations only O:\winnt\logs. |
| Reporting Requirements | Explain original provider selection and signature differences; report mirror diff, overlay, adapter, generator and generated-runtime footprint separately against fbb121d72 and pre-T413 baseline. Distinguish deletion from movement, original reuse and new glue. |
| Stop Conditions | Pause for evidence that safe removal needs an excluded algorithm or architecture change. If a binding cannot be removed, name the exact unavailable original dependency and failed earlier recovery routes; do not declare minimality or cleanup complete merely because tests pass. |
| Exit Criteria | Original composition recovered where usable; superseded autonomous code deleted; every retained binding has source-grounded necessity and disposition, with owner resolution of any unmet cleanup requirement; full regression/build/publication evidence, reviewed registers and measured deltas; committed/pushed clean tree. Admission P1 alone does not close S4. |
| Original Owner Request | “请你追加一个s任务清理”, following the demand to remove autonomous overlay added while reconnecting original code. |
| Similar-Issue Sweep | All 38 pairs, 20 typed bridges, both vglob carriers, header variants, full/short vector consumers and setup/republication routes; inspect adjacent latch/selector ownership without expanding into unrelated CPU mechanics. |

Owner Terminal/RDP visual confirmation remains separate and has not been
reported as received. No production source or published binary changes are
part of this admission delivery.

## S1 Closure Record

23a4fa83c delivered the [source audit](../etc/evidence/m0-t413-s1-accessor-layout-audit.md).

## S2 Closure Record

3cc7851e5 delivered the [accessor restoration](../etc/evidence/m0-t413-s2-accessor-restoration.md).

## S3 Closure Record

d9d814b00 delivered [integrated verification](../etc/evidence/m0-t413-s3-integrated-verification.md);
fbb121d72 recorded the earlier closure. No preceding P delivery is deferred.

## Current Technical Baseline

- Three programs at `O:\winnt`: run16.exe, basesrv.exe and ntvdm.exe; original
  x86 /MT CCPU40, APP_VERSION 0.0.413 / protocol 3. The exact published hashes
  match formal outputs and the [S3 evidence](../etc/evidence/m0-t413-s3-integrated-verification.md).
- Original cpu/src/evid/vglob.c supplies all 76 external field-access bodies.
  The finite private binding preserves original latch/selector ownership,
  correct GDP offsets and signatures without generated scratch clobber.
- All 17 command regressions, four ConPTY sizes, mouse, live resize, native
  resize contracts and five final-hash prefilled 80x5 runs pass. The latter
  contain 20 independently confirmed post-EDIT MEM opens/completions.
- Mirror diff distance is 419 versus 506 before this task; private overlay
  cohort is 136 versus 125. Eight original files are restored, with retained
  typed glue measured rather than hidden.
- The S11 blank-gap repair remains intact. This task does not alter CCPU
  instruction algorithms, x87, WOW/WRITE, broker semantics or guest media.
  The independent IP-mask finding is explicitly retained in [Debt](TODO.md);
  it is not declared fixed by this closure.
- Formal products remain under build/M0-T413/S2/formal; harnesses, source audit
  and recoverable prior products are under build/M0-T413/S3/integration.
  Runtime captures remain only in O:\winnt\logs.

## Recent M0 Closures

T413 S1-S3 restore original accessor bodies and pass the declared integration
profile; S4 reopens the remaining composition-cleanup requirement.
T412 Broker closure remains bounded to its architecture;
its transferred C-VID defect is now handled by T413, not retroactively counted
as a T412 test. T411/T410/T409/T407/T406 retain their recorded scopes.

## Recent Governance

The earlier T413 closure is qualified by the owner's S4 reopening; it is not
erased. The active packet remains until cleanup and its evidence are reviewed.
The next queue candidate remains x87 host-layout restoration; no task number
or admission has been allocated to it here.
