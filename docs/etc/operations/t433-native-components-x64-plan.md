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
