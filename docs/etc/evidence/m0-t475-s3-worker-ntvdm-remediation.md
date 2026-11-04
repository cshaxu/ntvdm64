# T475 S3 — Worker Base and NTVDM dependency remediation

## P1: VDMREDIR Worker Base input

The owner directed sequential delivery of the S3 design. The x86 production
link for VDMREDIR no longer selects `worker-base.lib`. The real link succeeded;
the ownership verifier now checks VDMREDIR and Hook32, with a negative control
that injects the prohibited archive into VDMREDIR. NTVDM and NTVWM still select
all five Worker Base sources. No product source or ABI changed in P1.

The x86/x64 product build, source ownership and negative controls passed.
Redirector controlled async publication passed 24 checks; OEM capacity and
sequential aliased guest writes passed 15. The coherent ten-image package was
staged, 63 immutable guest files and 274 export ordinals verified, and all 12
selected Control gates passed. Real DOS async-read, async-write and NetAPI
probes passed in a helper-owned short physical stage, with a fresh package ID
and the established test-only bounded PATH. Local package
`fd19d65e-2eec-4bb8-a76a-697b9841c4d3` was published in this worktree with
ten matching hashes. Full was not run; DOS4GW/NESticle was absent.

The checked-in guest boundary runner previously used a global `subst` drive;
it now uses the existing marker-checked physical staging helper and restores
its test-only environment. The first candidate and prior-release baseline
both timed out at guest startup with inherited 2423-character PATH. The
baseline and candidate both passed all three probes with the established
bounded test PATH. This is a controlled result consistent with the known
long-PATH issue, not a claim that its product root cause was repaired.
Logs, commands, start/end times, owned PIDs, hashes and prior release backup
are under `build/t475-s3-p1`. No main-runtime deployment or push occurred.

## P2: shared guest-memory contracts

The guest-memory lease API and redirector copy declarations now reside in
`common/guest-memory`. WOW32 and VDMREDIR consume those declarations without
including the former NTVDM-private headers. The worker retains its single
session-owned memory provider and the existing export ordinals. Compiler-recorded
dependency checks passed, including injected private-header negative controls.

The x86/x64 build, guest-memory lease tests, original WOW callback-frame fixture,
redirector OEM and async fixtures, and 12 selected Control gates passed. Actual
DOS async-read, async-write and NetAPI probes passed in a freshly identified
physical stage with the bounded test PATH. Staging checked 63 immutable guest
files and 274 NTVDM export ordinals. Local package
`8ac27d1f-ff63-4b10-b8f8-2afd5f83d39b` was published with ten matching hashes.
Full was not run; the optional DOS4GW/NESticle fixture was absent. Logs and
the previous release backup are under `build/t475-s3-p2`.

## P3: Redir copy orchestration

VDMREDIR now resolves segment/offset addresses and performs its bounded copy
workflow through the common address and lease contracts. The existing worker
retains its session, memory and compatibility copy exports. The original 4 KiB
chunk size, commit order and partial-failure behavior are preserved. Explicit
imports prevent the DLL link from selecting the worker's private memory object.
The candidate DLL's import table has shared provider ordinals 145, 148 and 149,
but no longer imports redirector-copy ordinals 143 or 144. This does not remove
the remaining NTVDM runtime import, whose scope is still open.

The x86/x64 build, compiler dependency and three negative controls, 45 lease
checks, 17 OEM/alias/chunk checks, 24 async checks, and 12 tests of the actual
original NetAPI entry points passed. All 12 selected Control gates and real DOS
async-read, async-write and NetAPI probes passed. The local ten-image package
`0bcc0986-5646-4a04-8002-0bf6c3d65656` has matching release hashes. Full was
not run; DOS4GW/NESticle was absent. The original-provider fixture required a
stale header path and public library list to be updated and a linker response
file to avoid the Windows command-line length limit. Logs, failed attempts,
the guest test's bounded PATH and prior release backup are under
`build/t475-s3-p3`.

## P4: CRT compatibility library

The unchanged historical `sprintf`, `sscanf` and `_swprintf` UCRT adapters
now belong to `lib-win32/crt-compat`. The NTVDM graph links the independent
`lib-crt-compat` archive; WOW32 compiles the same public source into its own
module. Production link maps select the new provider for NTVDM `sprintf` and
`sscanf` and WOW32 `sprintf`. The targeted x86 fixture verified formatting,
scanning, a failed conversion, and missing and duplicate provider rejection.

The x86/x64 product build, 63 immutable guest files, 274 export ordinals and
12 selected Control gates passed. A real WRITE application on a fresh physical
package displayed its main, document and page-info windows in four samples;
editing and normal exit were not tested. An initial observation used the
wrong window-reader executable and failed; the corrected run passed. The local
ten-image package `187112da-6183-4b8e-b482-ee73d022a604` has matching hashes.
Full was not run; DOS4GW/NESticle was absent. Commands, logs, failed attempt
and previous release backup are under `build/t475-s3-p4`.

## P5: WOW consumer contracts

Shared `common/wow` contracts now cover the WOW32 consumer's USER runtime,
GDI aliases, page-domain operations, package paths, FastBOP offsets, active
worker query and read-only registry facade. The worker retains its session,
USER and page state, registry projection and FastBOP implementation. Of 55
page-domain operations, 37 consumed by WOW32 are shared and 18 worker-only
operations remain private. Existing project-added mirror includes were
retargeted without changing original execution; the mirror ledger records it.

Compiler dependency records for 147 WOW objects show no NTVDM-private header;
ten forbidden-header negative controls passed. The x86/x64 build, real CCPU
page-domain, task lifecycle, process retirement and window-borrow fixtures,
session binding, original callback and GDI alias tests passed. All 12 selected
Control gates and four real WRITE startup window samples passed. The local
ten-image package `fd657e29-7249-493c-a67f-3e74d689d861` has matching hashes.
WRITE editing and normal exit and Full were not tested; DOS4GW/NESticle was
absent. Initial fixture attempts exposed command-line length and stale library
inputs, which were corrected before the final rerun. Logs, both candidates
and prior release backup are under `build/t475-s3-p5`.

## P6: thread mechanism and worker binding

The x86-compatible entry thunk, allocation and thread creation mechanism now
belong to `lib-win32/thread`. `common/worker-thread` supplies the existing
worker session capture, bind and fail-fast terminal cleanup callbacks without
creating a second session or TLS authority. VDMREDIR selects both shared
libraries instead of the former NTVDM-private thread implementation. The
generic library's forbidden-product-dependency negative control passed.

The x86/x64 build, session bind rollback, all four start forms, explicit exit,
exception and terminal cleanup fixture passed. Redirector async publication
(24 checks), actual original NetAPI entry points (12 checks), 12 Control gates
and real DOS async-read, async-write and NetAPI probes passed. The local
ten-image package `d240771a-26b9-4f8c-bccd-d05a90b697f7` has matching hashes.
Full was not run; DOS4GW/NESticle was absent. Logs and prior release backup are
under `build/t475-s3-p6`.

P7's compiler-dependency sweep also found VDMREDIR reaching the async request
contract and the forced SoftPC symbol-compatibility header in NTVDM-private
paths. They are not resolved by P6 and require explicit disposition before
source-boundary closure. NTVDM PE runtime imports remain a separate pending
scope decision.
