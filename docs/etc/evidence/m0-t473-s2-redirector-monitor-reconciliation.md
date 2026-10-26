# M0 T473 S2 — Redirector, monitor and debugger reconciliation

## Redirector boundary

`VDMREDIR.DLL` selects 16 original product sources, its original DEF and
resource.  `src/vdmredir-dll` contains only the three non-mirror DLL edges:
the stdcall entry to original `VrDllInitialize`, numeric guest-copy imports
from the parent worker, and the explicit unavailable private RAP transport.
It does not link a session, TLS, CCPU executor or an alternate redirector
algorithm.

The parent worker has two corresponding bounded operations:

- synchronous `ES:DI` copy resolves a numeric address through the existing
  CPU40/lease provider and copies only for the call;
- named-pipe async state snapshots payload data before the original DLL thread
  outlives the originating guest call, then binds the original completion to
  the same active session/epoch only while copying the result back.

The original `vrnmpipe.c` retains queueing, completion ordering and ICA
notification.  The adapter does not retain a guest alias across the async
operation; it carries locations, a bounded staging buffer and the source
worker epoch.  A dead/changed session rejects completion rather than writing
to a reused worker.  Thus this is a necessary lifetime bridge, not a second
redirector or a session recovery mechanism.

## Monitor, VDD and debugger boundary

| Surface | Selected/original owner | Current boundary result |
| --- | --- | --- |
| DPMI VDM_TIB and printer state | original layouts and selected DPMI callers | `mvdm_vdm_tib` supplies only worker-TLS original-layout storage and fixed source-shaped fields.  It does not expose a kernel pointer or start another CCPU executor. |
| `NtVdmControl` | original call spelling/layout | only the typed synchronous `VdmQueryDir` capability is admitted; all other unavailable kernel VDM services return the explicit source-shaped unsupported status. |
| VDD | original DEM/VDD handle lifecycle remains selected where callable | unavailable VDD registration/hook facilities remain explicit failure boundaries.  No fake callback, port interceptor or success result is supplied. |
| Debugger | selected debugger/CCPU read and register order | bounded guest-memory copy replaces only the invalid raw alias; `DbgPrompt` is dynamically bound or fails explicitly.  No debugger dispatcher/state machine is recreated. |

## Disposition

No original host source is missing from this product group.  Restoring the
private RAP transport, generic kernel VDM control, VDD registration or a
full kernel monitor would require a private subsystem and is prohibited by
the active architecture.  The correct product behavior is the existing
explicit unavailable/error route, with the original caller retaining its own
failure handling.

No adapter removal is justified: deletion would either reintroduce a durable
guest pointer across an asynchronous completion, misinterpret modern handles
as NT4 monitor objects, or turn an intentional unavailable boundary into an
unresolved import.  The current source group therefore ends as selected
original dispatch plus finite ABI/lifetime conversion, with no duplicate
redirector/monitor/debug implementation found.

## Result

This completes the fifth and final S2 source-owner group.  Real VDMREDIR
pipe/RAP, DPMI monitor, VDD and debugger behavior still need the package-level
normal/rejection/teardown evidence assigned to S3; S2 only proves that no
additional source import or autonomous provider is the correct recovery.
