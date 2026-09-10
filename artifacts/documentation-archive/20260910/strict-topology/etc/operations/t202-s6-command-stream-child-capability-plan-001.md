# T202 S6 COMMAND stream and child capability plan

## Question

What complete, package-owned capability is required before `54:06`, `54:08`,
and `54:0A` may replace their current deferred results without restoring the
historical NT4 global-standard-handle, event-thread, or VDM re-entry shell?

## Inputs

- `src/opennt/base/mvdm/dos/command/cmdredir.c` (`cmdGetStdHandle`);
- `src/opennt/base/mvdm/dos/command/cmdexec.c` (`cmdExec`,
  `cmdExecComspec32`, `cmdCreateProcess` and `cmdReturnExitCode`);
- `src/opennt/base/mvdm/dos/command/cmd.h` (`STD_HANDLES` and
  `REDIRCOMPLETE_INFO`);
- the T201 shared mutation profile ABI; and
- the T202 S5 whole-provider regression design.

## Source ABI and ownership

The original COMMAND component owns all three identities as one flow.
`54:06` accepts `CX` = stdin/stdout/stderr selector and an historical
`AX:BX` `REDIRCOMPLETE_INFO` pointer, then returns a raw 32-bit `HANDLE` in
`BX:CX`.  `54:08` reads a command tail from `DS:SI`, an ANSI environment from
`ES:0000`, and the packed three-`ULONG` `STD_HANDLES` record from `SS:BP`.
`54:0A` obtains COMSPEC and the environment at `ES:0000`.  The original
implementation changes process-global standard handles, starts a worker
thread, waits for a child, and re-enters the historical VDM broker.

The adapter must preserve the selector, service order, packed-record order
(`stderr`, `stdout`, `stdin`) and source-shaped register failure result.  It
must not preserve the historical raw-handle or global-process implementation:
those are NT4 product-shell internals, not guest ABI requirements.

## Replacement boundary

One `bx_ntvdm_command_stream_child_v1` session-owned provider will own the
complete flow.  It has three private parts:

1. A fixed three-slot standard-stream table captured at CLI composition
   admission.  It owns any real Win32 `HANDLE` privately and emits a
   generation-bound opaque 32-bit token instead of the historical handle.
   A token is valid only in its session and is never accepted as a host value.
2. A checked fixed-width gather transaction.  `54:06` returns the token in
   the historical `BX:CX` shape with `DX:AX = 0`; `54:08` gathers exactly the
   12-byte `STD_HANDLES` record at `SS:BP`, validates all three tokens, and
   reads bounded command/environment inputs.  It stores no guest pointer or
   guest address after that transaction completes.
3. A typed child-lifecycle record.  It may internally duplicate valid stream
   handles for `STARTUPINFO`/`STARTF_USESTDHANDLES` and close every duplicate
   itself.  It must not call `SetStdHandle`, create the old temporary pipe
   files, create VDM pipe workers, expose `HANDLE`/PID values to guest
   registers, or use `GetNextVDMCommand`/VDD/CCPU re-entry.

The child record is the deliberate composition replacement for the original
thread-plus-broker protocol.  Its success, completion, cancellation and
return-to-DOS states must be explicit fixed-width session states before a
host child is launched.  Until those states exist, `54:08/0A` retain the
current source-shaped `CF=0, AL=ERROR_NOT_SUPPORTED` outcome.

## Profile matrix

| Mode | `54:06` stream acquisition | `54:08/0A` child result | Host effect |
| --- | --- | --- | --- |
| direct + COMMAND `HOST_GLOBAL` grant | opaque tokens available | admitted once typed lifecycle is valid | ordinary user-mode `DuplicateHandle`/`CreateProcessW`; no installation or global handle mutation |
| readonly | source-shaped unavailable | source-shaped unavailable | none |
| overlay | source-shaped unavailable until a stream/child overlay backend exists | source-shaped unavailable | none |
| virtual | source-shaped unavailable until a virtual process backend exists | source-shaped unavailable | none |
| no owner grant | source-shaped unavailable | source-shaped unavailable | none |

This uses the existing shared `HOST_GLOBAL` class; it does not create a BOP-
specific policy class.  `CreateProcessW`, `DuplicateHandle`, process wait and
private handle closure are all available normal Win32 APIs.  The remaining
gap is the replacement lifecycle contract, not a missing Win32 API.

## Implementation and regression order

1. Add the session-private stream table and token ABI, then prove invalid,
   stale, cross-session and raw-handle-equality rejection.
2. Add checked command/environment/`STD_HANDLES` gather and prove no guest
   pointer survives the dispatch call.
3. Add the typed child state machine and only then admit a direct-profile
   launch backend.  It must prove failure propagation, private duplicate
   cleanup, no `SetStdHandle`, and no child launch under readonly, overlay,
   virtual or missing-grant cases.
4. Move `54:06`, `54:08`, and `54:0A` together into this provider and extend
   the existing 17-service common-ingress regression.  `54:0B` remains the
   same COMMAND lifecycle owner but consumes only the typed completion state;
   it must not reinterpret a stream token as `REDIRCOMPLETE_INFO`.
5. Only after the full package regression passes may one bounded native trace
   observe the new package.  The trace cannot add a service-sized task.

## Interpretation and follow-up

This plan keeps BOP semantics outside Bochs and avoids a trace-selected
`54:06`/`54:08` patch.  It does not claim a child provider, interactive stream
backend, Redirector pipe integration, or native COMMAND closure.  The next
implementation step is the stream-token table, because it establishes the
private boundary required by every later child result.
