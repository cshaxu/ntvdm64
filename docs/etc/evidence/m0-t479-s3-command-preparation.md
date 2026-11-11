# T479 S3 host command preparation delivery

## Delivered boundary

The approved [design](../operations/m0-t479-s3-command-preparation.md) is implemented
against source baseline 267b7a388. Independent guest-input snapshots reuse the
ordinary bounded capture routines without occupying its singleton. Independent
host launch objects retain converted data, absolute CWD, resolved streams and
the S2 authenticated capability pair. They share the existing Run16/Hook launch
path, use an explicit inheritance list, and return their own process handles
and errors. No process-global command mutation, new transport, polling, timeout
or lifecycle owner is added. Ordinary COMMAND still uses its original route.

This is host preparation, not guest multitasking support. Snapshot units use
synthetic memory; launch units use real child processes with test path/installer
boundaries. Guest VMM mapping/identity, actual caller-side encoding/handle
resolution, callback/reentry and CF/AL restoration remain S4/S5 gates. Per-VM
I/O remains S6. Original guest and all mirror files are unchanged.

## Verification

[Machine-readable evidence](m0-t479-s3-command-verification.json) retains source
hashes, build ownership, focused outcomes, every full gate and publication hashes.
Task evidence root is `build/t479-s3-command`; stable Ninja/ccache trees are reused.

- Cached complete x86/x64 builds passed. Shared-library CTest: 14/14 each.
- Production capture object: 31 assertions; independent buffers/drive/tokens,
  ordinary-slot preservation, bounded reads, failed acquire/release and cleanup.
- Production launch adapter: 59 assertions; two concurrent real children with
  different directories/environments/pipes/results, discarded source inputs,
  exact object inheritance, interactive branch, suspended start, endpoint mask,
  failed preparation/installation and single-use rejection. Exact resource
  balance after measured OS initialization: 134 to 132 with two original
  capability handles deliberately closed. No request leak allowance is used.
- VM registry/lifecycle, 233 assertions and eight cases; frontend-base channel
  and service fixtures passed. Actual worker/frontend control RPC: 126/90
  checks, zero failures and zero handle delta. Service fixtures: 42 each arch.
- Native Console/video/lifetime/failure/handoff, actual independent frontend
  RPC and original WOW generic-thunk normal/mismatch checks passed.
- Full: **16/16**, **382108 ms**. DOS/Window command matrices, nested native/DOS,
  WOW launch witnesses, real modern EDIT, immediate relaunch and frontend-loss
  management passed. The pre-existing authorized DOS4GW/NESticle exclusion is
  retained as skipped, never counted passed.
- Documentation governance self-test/verification and diff whitespace checks
  pass. Final closure/admission documentation is checked again before commit.

Earlier failed attempts are retained: initial link omitted the shared attribute
library; the first count included Windows' lazy process and pipe-root handles;
the first inheritance assertion incorrectly treated numeric child handle reuse
as inherited object identity. Each was corrected and rerun. Temporary diagnostic
enumeration is removed. No product failure was waived or converted to a pass.

## Publication, review and handoff

Complete package **74269114-368c-45ab-b52d-bf103d08b0b9** is published to
`assets/release` and `O:/winnt/system32`, with all ten hashes and Hook license
matching the tested candidate. The publisher preserved the preceding package,
checked for concurrent destination changes and completed with exit zero.

Implementer and reviewer are the same authorized main session, with a separate
post-implementation diff/ownership review. Reviewed failure paths preserve
errors, close partial duplicates, abort only an uncommitted suspended child,
and leave successful lifecycle management to the caller/NTSRV. No shared-library
source was forked. Runtime/header changes: +227/-16, net +211 lines; test/build
changes: +292/-1, net +291. Documentation/evidence and release binaries are
separate from those counts. Mirror delta is +0/-0; no guest rebuild occurred.

S3 reaches its bounded host-preparation exit criteria. The owner-approved
continuous plan automatically admits S4/guest-driver after this delivery:
recover the project VxD build/load path and prove actual VM inventory, retirement,
callback and counted wait/wake through a claimed emulated notification resource
on Win3.1 enhanced and Win95. This handoff does not assert those proofs exist.
