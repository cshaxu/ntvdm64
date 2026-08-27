# M0 T291 S3 — WOW frame/callback transaction recovery

## Purpose

Implement the complete source-shaped interface group selected by S2 before
any original WOW32 provider body is linked or routed: bounded WOW guest-pointer
scopes, numeric task/frame projection, and one-session `CallBack16`
pause/resume transaction.

## Ordered work

1. Add paired, bounded WOW pointer-scope calls to the existing SoftPC adapter
   family.  Their only backing storage is a current-session
   `guest_memory` lease; they must preserve `GETVDMPTR`/`FLUSHVDMPTR`/
   `FREEVDMPTR` ordering and reject a missing, overflowed or retained span.
2. Add the source-shaped numeric WOW task/frame projection to the existing
   monitor family.  It owns only copied task fields and bounded frame leases;
   it does not expose a native TEB, CCPU context or Bochs object.
3. Add the one-session callback transaction in the existing WOW adapter
   family.  It must retain original `CBVDMFRAME` construction, stack-change,
   callback pause/resume, AX:DX return and stack restoration order.  It calls
   typed SoftPC/Bochs mechanics only through their declared boundary.
4. Prove positive, invalid-span, nested/re-entry rejection, stale lease and
   cleanup cases on formal x86 and x64 Ninja fixtures.  Verify no provider
   body, selector or guest load enters the graph.

## Exclusions

No fast monitor, `CurrentMonitorTeb`, whole-memory `IntelMemoryBase`, PEB
lock/AeDebug, CSRSS/BaseSrv, GUI API, WOWEXEC, BOP ingress or private USER/GDI
provider is part of S3.  Those paths retain the exact S2 disposition.

## Exit criteria

The three interface layers must have one source-shaped owner each, use only
the existing three session mapping-manager instances, and pass their x86/x64
fixtures.  The selected original WOW32 bodies remain disabled until the
complete package implementation is separately admitted.
