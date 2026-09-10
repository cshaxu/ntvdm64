# NetAPI/RAP Redirector network-boundary recovery proposal

## Purpose

Recover the original OpenNT Redirector's reached network-management contracts
without turning the product into a replacement SMB, RAP, NetBIOS, DLC, RPC or
CSRSS implementation. This is a distinct owner package after the local
Redirector cohort: local files, pipes and mailslots must not be used as a
pretext to report remote-network success.

## Original owner surface

- `src/mvdm-host/vdmredir/vrnetapi.c` — NetAPI and downlevel network
  management dispatch.
- `src/mvdm-host/vdmredir/vrremote.c` — remote/RAP request path.
- Reached `src/mvdm-host/dos/command/cmdredir.c` caller forms and
  `src/mvdm-host/inc/vdmredir.h` declarations.
- `src/opennt-host/netapi/netlib/ntstatus.c` — selected, byte-identical
  `NetpNtStatusToApiStatus` / `NetpApiStatusToNtStatus` conversion algorithms.
- The P02/T306 NetAPI cohort ledger, in particular its public
  `NetWkstaGetInfo` / `NetWkstaUserGetInfo` rows and its RAP hard-boundary row.

## Scope and recovery order

1. Re-read each original caller, declaration, input/output structure, status
   conversion and native-buffer/handle lifetime.
2. Bind an individual public modern Windows API only where it preserves the
   original selected contract. The binding remains source-shaped and
   session-owned; it copies data across the boundary and uses the existing
   host-resource mapping manager for opaque identity.
3. Preserve the original `ntstatus.c` conversion algorithm rather than
   reimplementing error maps in an adapter.
4. For every RAP/downlevel/remote path, decide from original code whether a
   proven public API mapping exists. Otherwise retain the original failure
   path with its exact owner and reason.
5. Compile/link the selected source closure on the formal Win32/x86 and x64
   CCPU40 graphs, then run focused host-side status and admitted-binding tests.

## Explicit boundaries

- `XsNet*`, `RxNet*`, `RxRemoteApi` and their SMB/RAP transactions are not
  replaced by a project-authored network stack.
- NetBIOS and DLC sources (`vrnetb.c`, `vrdlc*.c`) remain named VDD/device
  transfers; they require their own owner-package admission.
- Private NetAPI RPC stubs, server-side service packages, CSRSS and kernel
  facilities remain outside this source closure.
- No route may expose a native pointer, `HANDLE`, RPC buffer or network buffer
  in an MVDM field. No generic adapter or new mapping manager is allowed.

## Evidence and exit criteria

The recovery record must list, for every reached interface, its original
caller and physical declaration, requested semantics, data lifetime, selected
public API or adapter, original failure result, and any later owner transfer.
The package closes only when all selected-source compile/link rows pass on
both target architectures and focused checks prove status conversion plus each
admitted public binding's positive and negative behavior. Remote guest-network
execution is a separate claim and is not implied by this package.
