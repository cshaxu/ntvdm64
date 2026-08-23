# M0 T259 S2 — OpenNT RTL/NTDLL ABI Facade Plan

## Objective

Create one source-shaped declaration and ownership surface for the directly
recoverable OpenNT `Rtl*` / `NTSTATUS` calls currently spread through the
COMMAND and DEM mirrors. This is a consolidation of the existing admitted
`HOST-07` seam: recover the full reached interface contract while original
interface shape and source call order stay intact, so the imported bodies and
Bochs need no corresponding invasive edit. A modern public-API implementation
supplies unavailable historical internals. It is not a replacement runtime and
does not rewrite imported OpenNT algorithm bodies.

## Admission

- Source evidence: imported COMMAND `cmdconf.c`, `cmdenv.c`, `cmdexec.c` and
  `cmdmisc.c`; imported DEM `demsrch.c` and its existing shim declarations.
- Catalog owner: `HOST-07`; related pointer/handle crossings remain `HOST-10`
  and `HOST-11` under T258.
- This S owns declaration consistency, allocation pairing, conversion,
  environment and status-error contracts only.  It does not admit
  `NtVdmControl`, BaseSrv/CSR, VDD, WOW/Redirector dynamic loading, or any
  CCPU/SAS product shell.

## Required outcomes

1. One canonical internal header states the exact supported `Rtl*` layouts,
   calling convention, allocation/free pairing and `NTSTATUS` conversion
   rules for x86 and x64.
2. Existing COMMAND/DEM source-shaped shims include that header rather than
   redeclaring incompatible copies of `ANSI_STRING`, `UNICODE_STRING`, or
   their ownership rules.
3. Public Win32 conversions/heap functions remain implementation details of
   the facade.  The imported OpenNT body retains its original function calls,
   order and failure branches.
4. A historical implementation with no direct export receives a source-shaped
   public-Win32 facade when its observable contract can be proven. Only an
   unproven remainder stays explicitly deferred; it is never a fake success.
5. Focused fixture coverage proves ANSI/OEM/Unicode conversion, allocated and
   caller-owned output, MULTI_SZ environment construction and deterministic
   status-to-DOS-error behavior on the formal target.

## P1 progress

P1 has centralized the active COMMAND declarations in
`opennt_rtl_compat.h`, added layout assertions, and recovered
`RtlNtStatusToDosError` through the same-named current `ntdll` export plus a
bounded source-relevant fallback. Its formal build and fixture evidence is
[`m0-t259-s2-p1-rtl-contract-status-recovery-001.md`](../evidence/m0-t259-s2-p1-rtl-contract-status-recovery-001.md).
P2 will reconcile remaining allocation/conversion ownership across COMMAND
and DEM without modifying imported bodies.

## Stop conditions

- A proposed helper exposes host pointers or makes pointer width part of a
  guest ABI.
- A convenience API requires changing an imported OpenNT control-flow body.
- A private NT4 function is implemented merely because a similarly named
  `Rtl*` helper exists on a current Windows installation.

## Exit criteria

One named `HOST-07` facade contract is used by every active direct mirror
consumer; duplicate declarations are removed or become thin aliases with no
independent semantics; focused tests and formal Ninja build/link pass.
