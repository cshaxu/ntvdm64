# T477 S4 VM frontend groups

## Approved behavior and implementation

The owner approved independent frontend groups, ordered CONSOLE, VM,
WIN16/WOW, native WIN32/WIN64 observations, then unassociated workers.
Each group keeps its own worker chain; nested VM hosts produce separate
VM groups rather than recursive UI indentation. VM PID displays hostPID-VMID;
TASK remains empty. Payload and incarnation are not shown.

NTSRV copies the existing authenticated instance association into management
rows. VM selection uses service epoch, owner generation and broker instance
identity; displayed PID/VM is not a selector. Immutable display identity is
retained by the existing reference-counted association for MISSING rows after
endpoint removal. This adds no registry, scheduling or lifecycle authority.
The existing stable block sorter preserves chain grouping. Native NTCON keys,
close behavior and WOW task handling remain unchanged.

The copied snapshot gains one 64-bit VM display field; protocol 67 updates
both IDL and interface references. All peers are rebuilt together. NTMON's
semantic snapshot comparison includes this field. Both service and UI reject
VM group close, including a forged action bit. Workers retain their existing
close/trace operations. The old UI filter for parentless native workers is
removed: NTSRV already omits transient GUI carriers, and actual unassociated
CUI workers must remain visible.

## Verification and corrections

The first x86 build caught a newly introduced test include missing its explicit
generated RPC-header dependency: the compiler could read the old header before
MIDL ran. The generator now orders that test object after service.h. The
corrected cached x86 build and x64 full component build exited zero.

The real service frontend-chains fixture passes with copied snapshot checks
for sibling groups, nested separate groups, full-width identity, read-only
rows, retained child association and missing endpoints. The initial NTMON
layout fixture failed its refresh-loop assertion because its mocked snapshot
path still depended on a product package identity during binding setup.
Its scope provider is now explicitly isolated in the unit fixture; it no
longer requires a released image resource to reach the mocked snapshot RPC.
The original failed run is retained, not counted as a pass. The corrected
fixture passes renderer, 64-bit composite PID, blank TASK, readonly dispatch,
stale identity and unassociated native worker visibility assertions.

The existing actual RPC frontend-instances fixture now also queries the real
management snapshot, checks VM rows and full-width VM IDs, rejects management
close over RPC, and checks removal/reuse without losing its sibling.
The final runner PID 33760, started 2026-11-05T22:46:20.3883512Z,
exited zero. Full passed 16/16 selected gates in 400331 ms: guest media,
export ordinals, WOW startup frontiers, Console-17, Window-17, actual RPC,
native GUI, version negatives, strict directory, modern EDIT return, cooked
return, rapid/interactive relaunch, session isolation, retirement wiring and
nested-window handoff. Both 42-case service matrices, frontend-base suites,
six frontend integration fixtures, 171 registry assertions and seven lifecycle
cases passed. Worker/frontend control units passed 126/90 assertions.
The actual RPC frontend-instances fixture passed the new management checks.
Frontend link ownership audit passed. Initial failed attempts above remain
recorded; their fixes were exercised by this successful final run.

## S4 P1 publication and review

All ten images and the hook license were published to assets/release and
O:/winnt/system32 with matching hashes at both destinations. Package identity
is 7b89c5d6-c8e0-472b-8aa8-b7f174c71ff8; release-manifest.json retains the
image hashes. Product version is 0.0.477, RPC protocol 67. Publication exited
zero. Owned commands, process records, logs and backups are under
build/t477-s4. The p2/p3 filenames are verification-attempt labels, not P
numbers; this is S4 P1.

Review relative to 1c11929f2: production +103/-31 (net +72), tests +113/-3
(net +110), build generator +1/-1. Documentation and release binaries are
separate. Mirror/guest diff is zero and no mirror exception is required.
The source review checks immutable association ownership, copied row ABI,
retained missing groups, full-width rendering, stale selection and existing
worker operations. No new lifecycle authority, polling or transport was added.
Documentation governance/self-test, LF validation and diff whitespace checks
are recorded with the delivery. Host contract proof is bounded by the tests
below; it is not exhaustive OS failure or arbitrary callback-reentry proof.

## Boundaries

There are no mirror or guest changes. Tests establish host foundation behavior,
not an operational guest VxD or actual guest multitasking. The queued guest
bridge supplies real guest VM identity, asynchronous launch and copied I/O.
WOW startup frontiers do not prove gameplay. The existing Full-suite
DOS4GW/NESticle exclusion remains explicit, not a newly waived test.
