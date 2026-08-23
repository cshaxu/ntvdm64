# M0 T259 S2 — OpenNT RTL/NTDLL ABI Facade Plan

## Objective

Create one source-shaped declaration and ownership surface for the directly
recoverable OpenNT `Rtl*` / `NTSTATUS` calls currently spread through the
COMMAND and DEM mirrors.  This is a consolidation of the existing admitted
`HOST-07` seam, not a replacement runtime and not a change to imported OpenNT
algorithm bodies.

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
4. Unsupported private interfaces are declared neither as fake successes nor
   as new globally visible substitutes; their `HOST-*` deferred disposition
   remains explicit.
5. Focused fixture coverage proves ANSI/OEM/Unicode conversion, allocated and
   caller-owned output, MULTI_SZ environment construction and deterministic
   status-to-DOS-error behavior on the formal target.

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
