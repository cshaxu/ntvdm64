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
Final intended widths: run16, NTCON, NTMON, NTVWM and Hook64 AMD64;
NTSRV, NTVDM, WOW32, VDMREDIR and Hook32 I386. No dual workers or new EXE/component; filenames and
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
| S6 integrated delivery | Final coherent recoverable ten-image mixed-width package and T closure audit. | One worker, both Hooks, cross-width nesting and DOS/native handoff, Console17/Window17, independent WOW frontiers, version negatives, mirror/diff audit and published hashes/smoke. |

S3 has completed its implementation/verification and coherent publication;
the current packet records its delivery review. S1's design handoff and S2
delivery are retained. Owner now admits S4 NTMON only; S5 and later await
their own admission. S4 starts with current-source/link/ABI review, preserving
the existing monitor UI/control behavior rather than adding monitoring features.
S4 has completed its native build, affected UI/RPC/lifecycle tests and coherent
publication; [S4 evidence](../evidence/m0-t433-s4-native-monitor-migration.md)
retains delivery review and limitations. Owner now admits S5 RUN16; S6 is
not admitted. The initial turn records source/link boundaries and difficulty
reporting, without changing production code or the published package.
Later rows require bounded stage conclusion and ordinary
execution-rule admission/verification; they are not simultaneous active tasks.
Owner explicitly selects S2 NTVWM, S3 NTCON, S4 NTMON and S5 RUN16.
S6 retains the prior final integrated audit/publication contract. Component
stages still perform their own affected build/runtime/coherent-package
publication gates; S6 is not permission to defer those until the end.

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

This is source evidence/design, not an x64 launcher runtime pass. Migration
uses build/M0-T433/S5/r001 onward and retains S4 as recoverable production.
