# T479 S2 host foundation delivery

## Scope and ownership

The owner approves continuous host-first execution through S7 in the
[stage plan](../operations/m0-t479-guest-bridge-plan.md). This stage implements
only the host-foundation findings from the
[design audit](../operations/m0-t479-s2-guest-design-boundaries.md).
S3 owns command snapshots; S4 owns the actual guest driver and VMM proof.
No guest driver, original guest bytes, original mirror or runtime wire ABI is
changed here. VM registration remains a host facility awaiting its guest caller.

## Implemented contracts

- `vm_host.c` pins payload lifetime under a short lock. End/close reject new
  admission and drain references with a condition variable; RPC and registry
  destruction run outside the host lock. There is no new thread or polling.
- `vm_frontend.c` reserves a per-VM list entry before registration RPC, exposes
  it only once ready, serializes retirement of that entry, and leaves other VMs
  usable. Failure retains the existing broker-owned rollback record. An
  invalidation event wakes guest-operation waits before teardown joins.
- `frontend-base/channel.h` passes borrowed stop and worker handles to dispatch.
  VM providers additionally receive VM invalidation and broker handles. Providers
  must wait on cancellation as well as their result. This does not impose a
  timeout, synchronize ordinary frames or transfer lifecycle authority.
- `ntvdm_vm_host_launch_context` acquires both frontend and execution capabilities
  through existing authenticated NTSRV APIs, or closes both on failure. It does
  not mutate NTVDM's upstream association. The actual RPC fixture now calls this
  same composition and verifies distinct execution objects for two VM roots.
- NTCON keeps its existing presentation cancellation implementation; only its
  shared dispatch signature changes. New guest-provider cancellation is not a
  claim that NTCON's renderer has been rewritten.

The owner must drain control callers before destroy. Synchronous control APIs
are not CCPU callbacks: future guest notifications must enqueue control work
and return. I/O callback context remains borrowed until successful retirement;
callbacks may not recursively retire their own channel. Failure preserves the
owning object rather than freeing live callbacks. These restrictions are part
of the public headers, not inferred from tests.

## Verification and review record

Evidence root: `build/t479-s2-guest`; stable MSVC Ninja trees and repository-local
ccache reused. Focused and complete builds run hidden with owned supervisors.

- First focused compilation exposed a project-owned `MAXULONG` spelling with
  no declaration in this SDK-only unit; replaced by `MAXDWORD` for ULONG's
  existing 32-bit count. The failed attempt is retained, not counted passed.
- A restricted-host channel test raised 0xc0000008 at the old assertion querying
  its deliberately closed worker handle. Instrumented assertion output pins
  that location. The test now checks exact before/after resource counts for
  failed bind/attach, preserving leak verification without an invalid-handle
  API call. Temporary diagnostic printing was removed.
- Modern pwsh is required for runner commands. The legacy Windows PowerShell
  attempt failed before verification because Get-FileHash was unavailable.
- The first real-package observer run used the restricted token and recorded
  `result=not-started`. It is a runner setup failure, not a product result.
  Reused the previous successful modern-pwsh desktop-capable execution context;
  no Computer Use, token helper or alternative test was introduced.
- Focused channel/service tests passed. The latest VM fixture passes 233
  assertions: independent same-payload retirement under a deliberately held
  close RPC, another VM's I/O and a third VM registration, context acquisition
  rollback, cancellation without caller release, stale identity and broker
  loss with both pipe and provider waits. Eight registry/lifecycle cases pass.
- Cached x86/x64 complete product builds passed, each with 14 shared CTest
  cases. Real RPC, complete product gates and publication remain pending until
  their final result is appended below.

Source review covers publication of busy entries, lifetime pins, waiting with
locks released, relookup after wake, failure retention and close/origin races.
The paired capability may race later closure; NTSRV remains authoritative and
may reject subsequent launch. No local stale capability permits admission.
These tests prove host mechanisms, not guest scheduling or guest I/O behavior.

## Difference accounting

No MVDM/non-MVDM/guest mirror changes; mirror diff +0/-0. All changed runtime
code belongs to existing project-owned common/NTVDM/NTCON owners. Existing
shared upstream component source remains untouched. Final source/test line
counts and complete publication evidence are recorded at closure.


S3 preparation found an important source correction: cmdexec's SetStdHandle
is already redirected by DIV-149 to thread-local child STARTUPINFO through
ADAPTER-WIN32-037. The design audit now states the actual selected chain.
No redundant standard-handle rewrite is justified; the remaining command
snapshot/reentry/global-result isolation is still S3 scope.


## S2 closure and S3 continuation

Cached complete x86/x64 builds pass; each shared CTest suite passes 14/14.
Channel/service tests pass on both widths, the VM unit has 233 assertions,
and all eight registry/lifecycle cases pass. Service fixtures pass 42/42 per
architecture. Actual frontend-instance RPC, generic-thunk normal/mismatch,
Console/video/publication/failure/return cases and Full 16/16 pass. Full took
367710 ms. DOS4GW/NESticle remains the prior owner-excluded external fixture,
not a pass. No guest-driver capability is claimed.

Package a0f0dc2c-9f29-4bbe-8dad-8859cabea128 is published in full to
assets/release and O:/winnt/system32; all ten images and the hook license match
candidate hashes. Previous binaries and their hashes were backed up before
replacement. [Verification summary](m0-t479-s2-host-verification.json) retains
actual processes, gate outcomes, publication hashes and difference counts.

The source audit checked 1055 catalogued mirror files against their pinned
original paths: 482 exact, no newline-only differences, no missing original.
The other retained mirror differences are unchanged; this is not a fresh
claim about their branch semantics. No mirror source changed in this S.
Project runtime source adds 217/removes 68 lines (net +149); tests add
108/remove 20 (net +88). This counts src/ and test/ separately and excludes
documents and binary assets. Necessary added state is lifetime pins, per-entry
serialization and cancellation handles; no new thread, timer or transport.

The owner authorized automatic stage closure after verified delivery. S2 host
foundation is complete, with S3 host command preparation next. T479 remains
open. Guest ABI/wait/wake and I/O remain with S4--S6, as planned, not waived.
Documentation governance/self-test and git diff --check pass before delivery.
