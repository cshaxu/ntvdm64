# Project Status

## Current Work

**Active: M0 T431 S3** (Ordinary Mode).
Owner accepted T430 and requested closure, admission and execution of the next
queued task on 2026-10-05. T431 admits the controlled native launch compatibility
hook package, beginning with a bounded source/API/bitness and contract audit.
The owner admitted the x86 nthook32-dll source root. The verified32-bit
implementation is published for owner validation. Owner now directs S2 closure
before personal acceptance and admits S3 for64-bit design discussion only.

## Active Packet

| Field | Required record |
| --- | --- |
| Identifier Mode | M0 T431 S3; Ordinary Mode. |
| Admission And Approval | Owner directs S2 closure before personal acceptance and S3 discussion; latest revision limits this T to NTVWM32/64 and nthook32/64, keeps all other components x86, and queues run16/ntcon/ntmon x64 migration as the next candidate T. S3 remains design/source only. |
| Candidate Proposal | [Controlled native launch hooks](../proposals/proposal-native-launch-hook-001.md). |
| Objective | Agree Hook64 and dual-build NTVWM design: each worker owns its hidden Console; DOS/native32/native64 reuse one NTSRV-controlled handoff contract, with explicit cross-width propagation and rollback gates. |
| Non-goals | No production coding/build/injection/publication at design stage; no private WOW64 executable probe, helper implementation, guest/mirror change, x64 MVDM/NTSRV/WOW32/VDMREDIR, global hook, observation or scheduler. Transient installer helper research/discussion is permitted, not shipment approval. |
| Reference Baseline | S2 P2 committed/pushed12160c657; coherent S2/r027-runtime published, r031-publication; APP0.0.427/RPC38/I/O25. Personal acceptance pending. |
| Files And ABI Surface | CURRENT, QUEUE, ARCHITECTURE, native-hook proposal and existing T431 plan/design; new queued component-x64 proposal; read-only Hook/search/bootstrap/worker callers. Proposed nthook64-dll and dual-build NTVWM need implementation admission; shared fixed-width service/I/O contracts remain canonical, required changes must be separately audited/versioned. |
| Applicable Rules | Full AGENTS authorities/source recovery, owner-scoped mixed-width design and optional installer-helper research revision, non-invasive boundary, one active packet, preserved other-session edits and build-only generated artifacts. |
| Verification | Selected-source/primary-reference review; four-width ABI/bootstrap/failure contract; documentation governance, relative links and diff checks. No runtime result claimed. |
| Expected Markers | Shared architecture-neutral logic versus native ABI mechanics; exact32-to64 blocker and64-to32 candidate; no silent helper or reduced width coverage. |
| Asset Needs | Retained S1 source/hash evidence, S2 production tests and exact MIT Detours4.0.1 slice. No new guest media or executable research artifact. |
| Reporting Requirements | Separate source conclusions, feasibility and actual runtime results; record failed earlier recovery rungs and precise remaining engineering gates. |
| Stop Conditions | Security bypass/host mutation, missing provenance, unapproved helper implementation or native creation replacement, changed execution/frontend ownership or unexplained regression; preserve coherent baseline and report rather than publish. |
| Exit Criteria | Reviewed source-backed design and explicit owner decision on implementation boundaries; discussion cannot close Hook64 runtime capability. |
| Original Owner Request | “准入S3，来讨论一下如何实现64位的支持，就是NT Hook 64.dll的实现。” |
| Similar-Issue Sweep | ANSI/Unicode creation, null application/quoting/search, redirected handles/environment/CWD, suspension/debug/token boundaries, GUI/new Console propagation, recursion and early-child exit. |

## S3 Design Freeze

Owner's latest revision limits this T to NTVWM32/64 and nthook32/64;
run16/ntcon/ntmon and ntsrv/ntvdm/WOW32/VDMREDIR stay x86. The revised
[frozen design](../etc/operations/t431-native-launch-hook-design.md#s3-design-freeze-component-widths-handoff-and-native-propagation)
and ARCHITECTURE preserve real native child semantics and width-neutral handoff.
S4 organizes only the dual-worker/Hook build and selection boundary; S5 proves
four-direction propagation, including Hook64 context delivery to x86 run16;
S6 retains final audit. None is yet admitted. General component x64 migration
is the next unnumbered Queue candidate, not a prerequisite or active task.
An installer-only transient helper is acceptable if necessary after mechanism
review; no resident helper or changed child identity. No source/runtime/process
or published package is changed;64-bit capability remains unproved.

## S2 Closure Record

Owner explicitly directs S2 closure on2026-10-05 before personal acceptance.
Production P2 is committed/pushed12160c657; automated gates and publication
are retained in [S2 evidence](../etc/evidence/m0-t431-s2-nthook32-implementation.md).
This is bounded32-bit engineering closure, not personal acceptance or T closure.
Default-geometry error87 and unproved API boundaries remain limitations.

Owner verification reports `run16 cmd -> mem` failing with a Windows dialog.
P2 repairs the DLL's extra argv[0]/basename eligibility rule by sharing
run16's unchanged search object. Personal verification remains pending.
Previously passing COMMAND-only Hook chains did not prove MEM interception.
Owner correction requires reusing run16's actual CWD/PATH and COM/EXE/BAT/PIF
discovery, followed by the selected original classifier. The unchanged search
mechanism is now in common for both production callers; the DLL's
separate argv[0]/quoted-absolute eligibility rules are removed.
Preserve launcher syntax, child creation flags/resources and existing execution.
P2/r027-runtime is published coherently; r030 passes Console17/Window17 and
the three retained WOW frontiers. r032 passes ten CMD chains plus WINMINE
startup, including actual interactive CMD -> MEM -> parent output -> exit.
Published bare MEM/parent-return smoke and all nine hashes pass in r031.
Default private-desktop geometry's separate prepare-text error87 is retained
in evidence; no frontend/mirror repair or test-assertion waiver is bundled.

[S1 conclusion](../etc/evidence/m0-t431-s1-hook-contract-conclusion.md) records the bounded
audit, exact unresolved width gate and owner32-only implementation revision.
Installer, copied-context reader, DLL entry/interception and both production
callers are built. The installer fixture passes93 assertions; retained NTVWM
lifetime passes1077 checks; four real CMD/DOS routes and Win16 startup/window
pass. Sealed r004-runtime passes Console17/Window17 and retained WOW frontiers.
Final graph regeneration produced r010-runtime; r011 passes its Product groups,
r014 passes six RPC and six GUI gates, and r017 passes the five final Hook
chains. The extra Full runner has a flat-cache layout failure and a separate
monitor assertion mismatch reproduced on T430; neither is counted as PASS.
[S2 evidence](../etc/evidence/m0-t431-s2-nthook32-implementation.md) records
those failures, failed earlier probes and unsupported boundaries. The coherent
nine-file set is published; published DOS/Hook smoke passes. P1 records this
bounded implementation delivery, awaiting owner verification. GUI propagation
carries launch compatibility independently of
text frontend authority. Other-session edits remain untouched.

### Retained S1 evidence

[Audit checkpoint](../etc/evidence/m0-t431-s1-native-launch-hook-audit.md) pins
actual32/64 CMD imports and existing classifier/NTVWM creation boundaries.
The [plan](../etc/operations/t431-native-launch-hook-plan.md) records exact
future cases. No production changes or deployment occurred. Unmodified Detours
cross-width installation is rejected because it requires a helper; this does
not prove every no-helper design impossible. Its remaining installer,
actual CMD route and authenticated initialization/inheritance proofs transfer to S2.
The [detailed design](../etc/operations/t431-native-launch-hook-design.md) now
specifies module boundaries, one suspended-child transaction, copied bootstrap
and context-only run16 delivery, supported flags, resource rollback and exact
verification gates. Its historical admission wording is superseded by S2 above;
the record remains contract design, not runtime proof.
The follow-up audit pins official PR161 and installed Windows API carriers:
64->32 has an unmerged source candidate;32->64 remains unsupported by both
selected Detours revisions. Private exports do not prove a complete installer
or rollback. Further private WOW64 executable research requires finite
re-admission; cross-width remains deferred at this explicit boundary. No S1 production build,
target injection, process termination or package publication occurred.

## S1 Closure Record

[Bounded S1 conclusion](../etc/evidence/m0-t431-s1-hook-contract-conclusion.md)
closes source/design only under the owner's32-bit implementation revision.
S2 owns runtime proofs and package delivery. T431 remains open; no T closure
is recorded in history for this in-place S continuation.

## Current Technical Baseline

The T431 S2 P2 coherent nine-file set build/M0-T431/S2/r027-runtime is published
at O:/winnt/system32: run16.exe, ntsrv.exe, ntcon.exe, ntvdm.exe, ntvwm.exe,
ntmon.exe, WOW32.DLL, VDMREDIR.DLL and nthook32.dll (MIT notice alongside).
MSVC14.43/SDK22621, x86 /MT CCPU40; APP0.0.427/RPC38/I/O25 unchanged.
S2/r031-publication preserves the P1 recovery set and exact new published
hashes; final Product groups/focused Hook probes and published MEM smoke pass.
S2/r016-publication retains the prior T430 recovery set and P1 hashes.
The retained T430 S6/r010-publication records matching source/
published hashes and recovery of S4; S6/r013-published-smoke
passes actual published Console/Window MEM and native-zero. S6/r008-product
passes Console17/Window17 and independent WOW frontiers. The bounded lease
test passes45 assertions after its pre-fix cross-context failure.

The formal cache is build/M0-T427/S2/r001, with regenerated source manifest.
The immediate recovery package is T431 S2/r010-runtime; T430 S6/r007-runtime
remains the accepted pre-Hook recovery set. Older
fb5843dc6/T430 S4 remains historical recovery evidence.
Both workers retain shared publication/input machinery; original guest media,
VGA presentation, frontend format routing and NTSRV authority are unchanged.
No helper, CPU30, new wire or unrelated sibling repair is included.

## Recent M0 Closures

| Task | Outcome and evidence |
| --- | --- |
| T430 | Owner-accepted non-WOW guest contract repairs/proofs; [closure](../history/m0-t430-non-wow-contract-closure.md). |
| T429 | Owner-accepted performance and shared worker I/O; [closure](../history/m0-t429-performance-worker-io-closure.md). |
| T428 | Owner-accepted worker interface unification; [closure](../history/m0-t428-worker-interface-unification-closure.md). |
| T427 | Owner-accepted system-root/search isolation; [closure](../history/m0-t427-system-root-search-isolation-closure.md). |
| T426 | Owner-accepted monitor tree; [closure](../history/m0-t426-console-root-monitor-tree-closure.md). |


## Recent Governance

T430 is owner-closed; original guest/original-host limits and WOW handoffs remain
explicit. T431 S3 is the sole active packet. Other-session proposal/TODO changes
remain preserved and excluded from this delivery.
