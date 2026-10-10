# M0 T442 native CUI Console-window virtualization plan

## Decision and boundary

T442 preserves the existing architecture: NTVWM owns a hidden carrier Console;
NTCON owns the one user-visible Console/Window projection; NTSRV owns the
authenticated frontend/worker/target relationship.  A native CUI target may
request that its **current inherited carrier Console HWND** be shown or brought
to foreground.  That request must select the already-bound NTCON Console
projection, never the carrier HWND itself.

This is a presentation request, not a Console transition. `FreeConsole`,
`AllocConsole`, and `AttachConsole` remain distinct identity-changing calls.
They are outside T442 unless S1 proves the reproduction uses one; in that case
the packet stops for re-admission.

## Existing path

1. NTSRV starts NTVWM with `CREATE_NEW_CONSOLE` and `SW_HIDE`.
2. NTVWM creates a text target on that private carrier and injects the matching
   `nthook` image before resuming it.
3. The hook inherits only the authenticated frontend/execution attachments.
   It currently intercepts `CreateProcessA/W`; it has no frontend pipe and no
   display ownership.
4. NTSRV owns the logical frontend route and authorizes the worker's one I/O
   lease.
5. NTVWM publishes the carrier screen through that lease. NTCON already has a
   local `frontend_session_display(window)` switch which selects its Console or
   Window projection.

Therefore a direct hook-to-NTCON call would bypass both service ownership and
the worker's serialized channel. It is rejected.

## S1 static audit result

The injected hook currently attaches only `CreateProcessA/W`. Its bootstrap
contains the inherited frontend and execution attachments, but no worker I/O
pipe and no NTCON entry point. The attachment is intentionally enough for a
nested `run16` launcher to bind its execution context; it is not authority to
select a frontend display. NTVWM's existing presentation thread is already the
only native owner which can safely serialize a frontend request, while NTCON's
`frontend_session_display()` is the local owner of actual projection selection.

This confirms the control path below is a necessary boundary rather than an
architectural preference. It also rejects reusing `CONSOLE_IO_SET_DISPLAY_MODE`:
that existing operation controls a Win32 Console display mode, not NTCON's
Console-versus-Window projection.

The O: locations available for this audit did not yield `mysmb64.exe`; no
dynamic target trace has been claimed. The exact intercepted API family and
the semantics of a hide request remain deliberately unresolved until the
owner supplies the binary or its path. The present design is complete for the
owners, validation and ordering, conditional on that finite trace.

## Target control path

After S1 proves the exact API family, the implementation has this finite flow:

```text
native CUI target
  -- intercepted current carrier HWND visibility/foreground request --> nthook
  -- authenticated presentation intent (execution + frontend attachments) --> NTSRV
  -- validates direct target, current execution context and route ------------> NTSRV route
  -- signals registered native-worker presentation event ---------------------> NTVWM
  -- one existing authenticated console-I/O presentation operation -----------> NTCON
  -- frontend_session_display(CONSOLE) ----------------------------------------> visible root Console
```

For a proven hide request, the same path selects `WINDOW` only if the request
is for that same current carrier HWND and the target has an ordinary application
Window presentation. S1 must demonstrate that this preserves the target's
native behavior. Otherwise hide is passed through untouched. The service stores
only the latest requested display mode on the already existing native route;
it does not create a task, observed record, process tree, Console registry or
window inventory.

## Validation and ordering

The hook accepts a request only when all of these are true:

- the intercepted HWND equals `GetConsoleWindow()` in the calling process at
  the time of the call;
- both inherited attachments exist and NTSRV can bind the execution attachment
  to the calling authenticated process and the retained frontend root;
- the process is the direct active native CUI target of the matching NTVWM
  route; and
- no new-Console creation flag or identity-changing Console operation has
  invalidated that route.

The hook submits intent through the normal local RPC connection, preserving the
real API's return value and `GetLastError` on every unsupported, unauthenticated
or service-unavailable path. It does not wait for paint, mutate standard handles
or call NTCON. NTSRV signals the worker only after validation. NTVWM consumes
the event on its existing presentation thread, serializes the resulting request
with normal publication, and its already-authenticated channel reaches NTCON.
NTCON neither knows the worker type nor the target identity.

The small new control surface is deliberately one request/retrieval pair and
one worker-owned event registration, rather than an arbitrary HWND RPC:

| Owner | Later addition |
| --- | --- |
| `nthook32/64` | Detours for only the API family witnessed in S1; current-Console-HWND check; best-effort authenticated intent submission. |
| common RPC declarations | Fixed enum `CONSOLE`/`WINDOW` intent, no HWND or PID on wire. |
| NTSRV | Validate the caller against its existing direct record, execution context, frontend root and native route; retain the latest intent and signal the native worker event. |
| NTVWM | Register/wait one presentation-intent event and turn a retrieved intent into one serialized frontend channel operation. |
| NTCON | Reuse its existing display selector through one typed channel operation; no target or worker-kind branching. |

The request wire carries only the `CONSOLE` or `WINDOW` intent and the existing
execution attachment. It does not carry an HWND, PID, title or application
name. NTSRV authenticates the caller from the RPC binding and compares the
execution attachment with its existing direct native record before setting the
route's latest intent and its registered native-worker event. NTVWM waits on
that event beside its existing presentation waits, retrieves-and-clears the
intent through NTSRV, then issues a new typed Console-I/O projection operation.
That operation is handled by NTCON using `frontend_session_display()`.

No participant polls for an intent, and an intent never changes worker
ownership, I/O lease, target execution, standard handles or completion.

## S sequence

| S | Work and exit gate |
| --- | --- |
| S1 (active) | Capture `mysmb64` direct-CMD and run16 API/Console/window sequence; prove the affected HWND and exclude/identify identity-changing operations. Finalize exact detours, wire names and test fixtures from that evidence. |
| S2 | Implement the validated hook/service/worker/typed-channel route for both hook widths. Unit-test authorization, foreign HWND, stale context, unsupported API, recursion and failure passthrough. |
| S3 | Run serial product regressions: direct CMD, run16 CUI toggle, nested CUI, Console/Window routes, native GUI, DOS, Win16, worker teardown and carrier non-exposure. Publish only a matching ten-image package. |

## Acceptance matrix

| Scenario | Required result |
| --- | --- |
| Direct CMD native CUI | Existing direct behavior unchanged. |
| `run16` native CUI, carrier-show request | Root NTCON Console appears; private NTVWM carrier never appears. |
| Unrelated HWND operation | Original call is untouched. |
| CUI with no carrier visibility request | No new service/channel operation. |
| Native GUI, DOS and Win16 | No hook/display route is acquired. |
| `FreeConsole`/`AllocConsole`/`AttachConsole` found | Stop; do not emulate it through visibility. |
