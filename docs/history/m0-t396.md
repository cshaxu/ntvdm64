# m0-t396 historical record

Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.

## m0-t396-wow32-provider-dispatch-frontier-closure-20260907.md

# M0 T396 closure — WOW32 provider and dispatch frontier

## Outcome

T396 closes its approved complete-WOW32-provider scope. It does not claim a
completed Win16 lifecycle, visible window, message loop, or graceful
`WOWEXEC.EXE` exit.

The fresh x86 product now proves one continuous source-shaped route through:

```text
DPMI32/DOSX bootstrap
  → MS_bop_1
  → SafeLoadLibrary("WOW32")
  → W32Init return
  → W32Dispatch
  → WU32RegisterClass
```

The former D1CD real-mode code-segment and C9D9 stack-segment CPU40 failures
were eliminated by preserving the original transition ordering at the CCPU
immediate-validation seam. The resulting fixed non-debug run remains live at
the first private USER class-registration call rather than faulting.

## Boundary and disposition

`WU32RegisterClass` calls the NT4-private `pfnRegisterClassWOWA` field and
requires server-owned WOW class/window storage (`PWC`/`PWW`, `GCL_WOW*`,
`GWL_WOW*`) for later window-procedure and message-thunk behavior. The
original USER/Win32k/CSRSS server is an explicit source-policy stopping
boundary. A public `RegisterClassA` wrapper alone cannot retain that contract;
a new private UI service is outside T396.

The evidence is [S34 first dispatch frontier](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t396-s34-first-dispatch-frontier-002.md)
and [S35 private USER class-registration audit](../../artifacts/documentation-archive/20260910/etc/evidence/m0-t396-s35-private-user-class-registration-audit-001.md).

## Successor

The existing unnumbered **WOW16 service, UI and host integration activation**
candidate in [QUEUE.md](../states/QUEUE.md) owns any future bounded public-UI design
and implementation. It requires a new owner-approved numeric T packet; T396
does not authorize further USER/CSRSS/Win32k, guest, CPU or BOP work.

