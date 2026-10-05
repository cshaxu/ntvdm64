# T433 native components x64 migration plan

## Scope

Owner admits the former Queue-head candidate after T432 closure without
manual testing. Status owns the only active packet: S1 audit/design.
[T432 closure](../../history/m0-t432-single-worker-dual-hook-closure.md)
supplies the baseline: one x86 NTVWM, dual Hooks, ten images,
APP0.0.432/RPC41/I/O25. Old dual-worker/eleven-image planning is superseded.

Final widths: run16, NTCON, NTMON and Hook64 AMD64; NTSRV, NTVDM, NTVWM,
WOW32, VDMREDIR and Hook32 I386. No new EXE/component; filenames and
system32-relative layout remain. Design-only S1 does not advance production
version, modify source/build inputs or replace the published package.

## Proposed sequential stages

| Stage | Outcome | Required proof |
| --- | --- | --- |
| S1 audit/design | Actual linked-original/project/common ledger, native layouts, source-first alternatives and detailed migration design. | Pulled map/dependency symbols versus unused archives; reuse Hook64 conclusions; explicit unresolved ABI/failure contracts. Design-only delivery. |
| S2 frontend/monitor | x64 NTCON/NTMON with separately built neutral and RPC clients. | x64 clients/x86 NTSRV, authenticated resource transfer, Console/Window/text/DIB/input/title, management-only projection, shutdown/cancellation and cleanup. |
| S3 launcher/Hook integration | x64 run16, unchanged shared discovery/recognition/CLI and both incoming Hook widths. | Original/private versus OS/native ABI, DOS/Win16/native32/native64, real child flags/handles/waits/results, context-only launcher; no launcher fallback helper. |
| S4 integrated delivery | Coherent recoverable ten-image mixed-width publication. | One worker, both Hooks, cross-width nesting and DOS/native handoff, Console17/Window17, independent WOW frontiers, version negatives, mirror/diff audit and published hashes/smoke. |

Only S1 is admitted. Later rows require bounded stage conclusion and ordinary
execution-rule admission/verification; they are not simultaneous active tasks.

## S1 detailed ledger

Per consumer record origin, source/hash, actual pulled symbol, flags/ABI,
x64 composability, public OS versus private historical layout, resource owner
and failure contract. An unused x86 assembly archive member is not a linked
dependency. Audit original run16 Base classifier/environment/CSR capture/RTL;
NTCON/NTMON do not acquire that closure just by sharing an archive name.

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
