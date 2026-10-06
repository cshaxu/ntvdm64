# T433 native components x64 migration plan

## Scope

Owner admits the former Queue-head candidate after T432 closure without
manual testing. Status owns the only active packet; owner-approved S1 design
handoff admitted S2 NTVWM, now delivered; owner admits S3 NTCON. Remaining consumer-native probes
continue at their own stages, not as already passed S1 runtime behavior.
[T432 closure](../../history/m0-t432-single-worker-dual-hook-closure.md)
supplies the baseline: one x86 NTVWM, dual Hooks, ten images,
APP0.0.432/RPC41/I/O25. Old dual-worker/eleven-image planning is superseded.

Owner further adds the single NTVWM's x64-native audit/design to S1.
Owner now plans NTSRV native migration immediately after S5 RUN16, without
changing the active S5 packet or starting service implementation.
Final intended widths after the planned service stage: run16, NTSRV, NTCON,
NTMON, NTVWM and Hook64 AMD64; NTVDM, WOW32, VDMREDIR and Hook32 I386.
NTSRV remains I386 throughout S5. No dual workers or new EXE/component; filenames and
system32-relative layout remain. Design-only S1 does not advance production
version, modify source/build inputs or replace the published package.

## Proposed sequential stages

| Stage | Outcome | Required proof |
| --- | --- | --- |
| S1 audit/design | Actual linked-original/project/common ledger, native layouts, source-first alternatives and detailed migration design. | Pulled map/dependency symbols versus unused archives; reuse Hook64 conclusions; explicit unresolved ABI/failure contracts. Design-only delivery. |
| S2 NTVWM | x64-only single NTVWM with architecture-local worker-base/client closure, retaining both target widths. | Hidden Console, input/capture, real target handles/results, matching Hooks, reentry, shutdown and NTSRV final-I/O checkpoint; verify existing x86 frontend/launcher interoperability; no second worker. |
| S3 NTCON | x64-only frontend with architecture-local neutral libraries and RPC clients. | x86 NTSRV/NTVDM and migrated NTVWM, typed resource transfer, Console/Window/text/DIB/input/title, final I/O, cancellation and cleanup. |
| S4 NTMON | x64-only monitor with architecture-local management/RPC closure. | Service-only tree projection, exact labels/hotkeys/control permissions, x86 NTSRV authentication, snapshots and resource cleanup; no local process enumeration. |
| S5 RUN16 | x64-only launcher, unchanged shared discovery/recognition/CLI and both incoming Hook widths. | Original/private versus OS/native ABI, DOS/Win16/native32/native64, real child flags/handles/waits/results, context-only launcher; no launcher fallback helper. |
| S6 NTSRV | Audit/design, then migrate the single service to AMD64 using the original BaseSrv algorithms and existing RPC/resource binding. | Actual selected original/project dependency closure, fixed32 IDs versus native local resources, mixed-width RPC/receipts, DOS/WOW/reentry/cleanup and a reviewed incremental mirror-diff budget no larger than delivered S5. Admission required; no service implementation in S5. |
| S7 mixed-width batch verification | After S6 delivery, verify real BAT execution through run16-started x86 and AMD64 CMD with mixed DOS/Win16/native32/native64 launches. | Actual image widths, both Hook origins, shared search, sequential/nested execution, Windows child waits, direct results, batch branching, I/O handoff and cleanup; distinguish admitted legacy routing from existing guest/WOW limitations. No claim of universal BAT compatibility. |
| S8 integrated delivery | Final coherent recoverable ten-image mixed-width package and T closure audit. | One worker, both Hooks, cross-width nesting and DOS/native handoff, Console17/Window17, independent WOW frontiers, version negatives, mirror/diff audit and published hashes/smoke. |

S3 has completed its implementation/verification and coherent publication;
the current packet records its delivery review. S1's design handoff and S2
delivery are retained. Owner now admits S4 NTMON only; S5 and later await
their own admission. S4 starts with current-source/link/ABI review, preserving
the existing monitor UI/control behavior rather than adding monitoring features.
S4 has completed its native build, affected UI/RPC/lifecycle tests and coherent
publication; [S4 evidence](../evidence/m0-t433-s4-native-monitor-migration.md)
retains delivery review and limitations. Owner now admits S5 RUN16; S6 is
not admitted. S5 implementation now builds the single AMD64 launcher with
minimal registered local-ABI changes and unchanged original RTL algorithms.
[S5 evidence](../evidence/m0-t433-s5-native-launcher-migration.md) records
both-width ABI tests, corrected owned-Window input observation, final Full14,
both real Hook chains, coherent publication and actual deployed checks. The
unclassified rapid-interactive observation remains explicit debt, not a claimed
repair. S5 production P1 39fe1f8a93c855a59298877ec10a93d8dd8bb92d is committed
and pushed; documentation-only P2 records its bounded automated closure.
No S is active; T433 stays open and later stages require admission.
Later rows require bounded stage conclusion and ordinary
execution-rule admission/verification; they are not simultaneous active tasks.
Owner explicitly selects S2 NTVWM, S3 NTCON, S4 NTMON and S5 RUN16.
Owner's later planning inserts S6 NTSRV after RUN16, then S7 mixed-width BAT
verification after NTSRV native migration. Final integrated delivery is S8,
retaining its original audit/publication contract. Component
stages still perform their own affected build/runtime/coherent-package
publication gates; S8 is not permission to defer those until the end.

## S1 detailed ledger

Per consumer record origin, source/hash, actual pulled symbol, flags/ABI,
x64 composability, public OS versus private historical layout, resource owner
and failure contract. An unused x86 assembly archive member is not a linked
dependency. Audit original run16 Base classifier/environment/CSR capture/RTL;
NTCON/NTMON/NTVWM do not acquire that closure just by sharing an archive name.
NTVWM also audits hidden Console ownership, target creation, both matching
Hooks, shared publisher/input waits and typed recipient-local attachments.
Old dual-worker code remains evidence, not automatically reusable source.

Reuse common/native_image, application_search and Hook64 classifier/context
findings. Audit HANDLE versus task IDs, native SEC_IMAGE structures, local
PEB/TEB/TLS layouts, checked lengths, recipient-local attachments and Window
callback userdata. No second resolver/classifier. Try original composition
with the smallest architecture-correct facade before a proposed mirror diff;
record exact unavailable boundary, source alternatives, owner and tests.

Plan architecture-local static libraries and MIDL outputs under build roots,
never mixed object widths/CRTs. Wire types stay fixed-width; genuine wire
changes advance APP protocol/IDL together, not bitness alone. The first later
production-source stage advances application identity to0.0.433.

NTSRV stays task/lifecycle/I/O authority; preserve worker execution,
final paint/input return/double ACK. No global WOW64-redirection disable,
new scheduler/registry, resident helper, process-tree observation, ownership
shift into common/worker-base or revival of rejected dual workers.

S1 output: build/M0-T433/S1/r001. Retained maps/manifests are immutable;
new indexed audit/design evidence distinguishes source proof, compile
experiments and runtime results. Do not run a product matrix merely for
admission. Escalate a material required mechanism rather than silently rewrite.

## S5 admission audit and migration priorities

Current run16 cache matches the sealed S4 image hash52F0C847B47C327C9F3B17BB856BBD9915A5F5CE4490E4EB65E09A3D687AA9F4.
Its map still selects original classifier/client/capture and RTL error/environ,
not just project RPC. These bodies require their own native ABI audit; prior
native frontend/monitor success does not certify them.

1. frontend_scope.c:24/94 still parses/formats native resource locators with
   strtoul/%lx. Use checked full-width local values; keep broker authentication
   and syntax. native_launch.c already has the delivered full-width formatter.
2. Original csrutil.c has ULONG pointer-offset arrays, a pointer-to-ULONG cast
   and four-byte alignment. The project provider ignores capture metadata and
   does not implement original pointer rebasing. Audit which metadata is live
   and native message/string alignment before selecting a bounded ABI binding;
   do not widen numeric fields or reimplement CSR. Zero original diff is not
   promised. A required material/unapproved mirror adaptation stops for review.
3. Retain original DOS/NE/PIF classification. Original native classification
   has a same-machine restriction, but run16 already falls back to verified
   SEC_IMAGE metadata, accepting both native widths. Reuse the Hook64's
   classifier-local _X86_ guest-rule composition after native declarations,
   not a global x86 compile define or a second classifier.
4. Preserve original RTL environment algorithms through their existing finite
   private TLS/public-API PEB/TEB facade. Audit native local structures and
   allocation/free/lock/encoding/failure contracts; never cast real modern
   PEB/TEB layouts to NT4 or replace original environment behavior casually.
5. Rebuild launcher clients/MIDL/common/Hook context at AMD64. NTSRV remains
   x86 and existing typed resource/copy contracts remain unchanged. Both
   Hook origins must seed the actual x64 launcher without interception of
   run16 itself; current installer already derives the actual child's machine.
6. System32 is the native view for an x64 launcher, and SysWOW64 is explicit
   x86. Sysnative is a WOW64-only alias: existing tests must choose paths by
   the actual launcher ABI and verify the resulting PE machine. Preserve
   CWD/PATH order, explicit selected file identity, original arguments and
   shell fallback; do not disable redirection globally or force all CMD32.
7. Retain DOS/native receipts, GUI startup-only/explicit wait, Win16 startup
   behavior and the final Console restored acknowledgement before outer CMD
   resumes. Native compile alone cannot prove nested or failure/cleanup paths.

At admission this was source evidence/design, not an x64 launcher runtime
pass. Delivered implementation evidence above uses build/M0-T433/S5/r001
onward and retains S4 as independently recoverable production.

## Planned S6 NTSRV native migration

Owner requests this stage after run16, with additional original-mirror diff
preferably no larger than run16's delivered migration. S5 is delivered; this
plan is not S6 implementation admission. Preserve the completed S5 package as
the independently recoverable baseline and audit before editing service source.

- Audit the actual NTSRV link map and selected BaseSrv/RTL/ABI dependencies,
  including any selected x86 assembly, not entire archives. Reuse the native
  dependency work already validated by S5 before adding another adaptation.
- Keep RPC records, DOS/WOW task IDs, parent receipts and guest fields at their
  existing fixed widths. Current hParent is a uint32 receipt carried in the
  original HANDLE-shaped interface, not a transported native event address.
  Retain its existing resource-table resolution and original tagged-ID rules.
  Local pointers, allocations, callbacks and actual process/event handles use
  the service's native ABI. Do not widen wire fields merely for bitness.
- Preserve srvvdm.c's original DOS/WOW records, scheduling, completion,
  blocking/reentry and cleanup. No replacement registry/scheduler, CSR shell,
  second service/helper, ownership change or NTVDM/WOW32/VDMREDIR migration.
- Prefer unchanged original source plus existing bounded ABI/build bindings.
  Before implementation, compare the incremental service-only mirror changes
  with final S5: changed original body/header files, changed executable or
  declaration lines and changed expressions; report comment-only registration
  separately. Do not hide changes by moving original logic into an adapter.
  Shared S5 adaptations already in the baseline do not count as new S6 diff.
  If a comparable dimension would exceed S5, or an original algorithm must
  change, stop and present the smallest alternative for owner review rather
  than treating this planning request as permission for a larger rewrite.
- Generate an architecture-local AMD64 service/MIDL/static closure. Maintain
  one ntsrv.exe and the ten-image system32 package; no bitness-only protocol
  bump or automatic production fallback. Verify x86 NTVDM/Hook32 and x64
  launcher/frontend/worker/monitor/Hook64 through unchanged authentication,
  command/resource transfer, direct results and Console return barriers.
- Require retained service/RPC positive and negative tests, shared/exclusive
  DOS and WOW startup/completion, reentry and DOS/native bidirectional handoff,
  worker reuse, broker loss/rundown, version rejection and resource cleanup.
  Retain Console17/Window17 and independent WOW frontiers, coherent publication,
  deployed hash/smoke verification and explicit untested boundaries.

No AMD64 NTSRV compile/runtime success or final mirror-diff size is claimed by
this plan. S6 begins only after S5 closure and its own active packet admission.

## Planned S7 mixed 16/32/64-bit batch verification

Owner side-conversation direction on2026-10-05 inserts a separate test stage
immediately after NTSRV64 migration. This is planning only: S5 was then the
active packet and is now delivered; no test, process, implementation or new stage is admitted
by this edit. S7 requires delivered S6 and its own active packet admission.

- Start actual x86 and AMD64 CMD through run16, then execute a BAT containing
  admitted DOS16 programs and both native target widths. Verify PE machine
  identity and selected file hashes rather than inferring width from filenames.
- Cover representative sequential chains16→32→64→16 and64→16→32→64, nested
  CALL/BAT and CMD launches, and both Hook origins propagating across native
  descendants. Include Win32 CUI and GUI targets; Win16 GUI startup uses the
  existing startup-only contract and independent accepted WOW frontiers, not
  an invented GUI task-exit guarantee or new gameplay gate.
- Exercise bare and explicit COM/EXE/BAT/PIF names, CWD/PATH lookup, quoted
  paths/arguments and the actual System32/SysWOW64 view. Confirm shared run16/
  Hook discovery selects the intended admitted16-bit file and redirects it to
  NTVDM rather than letting native Windows reject it as unsupported16-bit.
  Unsupported guest behavior remains a separate explicit failure, not routing
  success or a promise that every historical executable runs.
- Verify real output and operation order using unique per-run file/stream
  evidence, not unsupported scrollback or old on-screen prompts. Preserve
  continuous typeahead cases separately from condition-paced interaction.
- Check actual exit codes, ERRORLEVEL and conditional branching, including
  nonzero and missing-target negatives. Test START/START /WAIT explicitly,
  preserving their Windows semantics and existing GUI/Win16 startup-only
  versus explicit-wait distinctions; do not infer universal coverage from
  ordinary synchronous launches.
- Cover supported stdin/stdout/stderr redirection, Console and Window paths,
  DOS/native return and final Console restoration. Assert normal/failure
  cleanup and session isolation without new task records, scheduler, helper,
  production protocol or process-tree termination.
- Keep global BaseSrv product cases serial and test-owned. Fixtures/reports
  stay under build/; if a short alias is needed use only Z: and remove it in
  cleanup. Guest binaries remain immutable. Record package/input hashes and
  failed observations; do not relax assertions or retry failures into a pass.

Deliver reproducible tests and a bounded capability ledger. A newly discovered
implementation gap is investigated at its existing owner and requires scoped
approval if it would expand the stage, especially an original mirror change.
Existing Console17/Window17/WOW and coherent-package delivery gates remain.
