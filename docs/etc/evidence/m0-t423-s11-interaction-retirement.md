# T423 S11 Interaction And Retirement

## Admission

Owner admits two repairs after S10 bounded cleanup closure f98825653;
e59e63d74 records the sole active S11 packet. T423 remains open. RDP pointer
escape is assigned to S12, not waived or included in S11 completion.
O:/winnt still contains the S9 publication. No S11 candidate is published.

## Mouse Source Findings

The owner reports delayed block-cursor motion in Window EDIT. Queue residence
time and total latency have not yet been measured; source ceilings alone do
not establish the only cause.

- Original OpenNT base/mvdm/softpc.new/host/src/nt_event.c reads up to five
  records per event-loop iteration and retains Sleep(10) after processing.
- Current mirror reads one raw record, expands a scan-less key into at most
  eight records, and retains that sleep. This limits raw-event consumption.
- Current src/ntvdm-exe/win32/console_compat.c::ReadConsoleInputExW also reads
  only one record, even when its caller offers a larger buffer. Restoring
  only the event-loop request count will therefore not restore batching.
- Preserve scan-less key expansion, ordering, history/returned-input origins,
  host-shortcut filtering and mouse button transitions. Do not change the
  original mouse IRQ delay or move host I/O back into the worker.

## Retirement Source Findings

src/ntkvm-exe/native_console_backend.c::run16_native_backend_members returns
retained ConPTY liveness, not attached-client count. session_service.c tests
this value before OpenNtBaseClientRetireFrontend, so retained idle ConPTY
prevents retirement. Do not remove the guard without replacing its contract.

Required owner predicate: no pending startup, active DOS task, unfinished
direct Win32 request or other attached Console user; then atomically close
admission and retire. Retain one ConPTY during CMD-to-DOS transitions.

Existing S9 evidence already distinguishes Job/process ancestry from Console
attachment and proves early release breaks future native I/O reuse. Reuse
that evidence, not a new speculative Job or observer implementation.

Public system API review:

- [GetConsoleProcessList](https://learn.microsoft.com/en-us/windows/console/getconsoleprocesslist)
  queries the caller's current Console, not an arbitrary HPCON. ntkvm is
  attached to its visible Console, so this is not the requested query.
- [ReleasePseudoConsole](https://learn.microsoft.com/en-us/windows/console/releasepseudoconsole)
  releases the keepalive reference; remaining clients may continue until
  the last disconnect, then the output ends. It is not a read-only query.
- [Microsoft's lifecycle change](https://github.com/microsoft/terminal/pull/14544)
  explains native client lifetime and the loss of subsequent launch support.
  The local S9 admission tests are stronger evidence than assuming a successful
  later CreateProcess also has working Console I/O.

An alternative final-drain protocol could close new frontend admissions once
broker/direct work is empty, release keepalive and preserve existing client
I/O until clean EOF. This changes the owner's required ordering: surviving
clients could no longer submit new work into that same frontend. It therefore
requires an explicit owner choice and is not selected or implemented here.
The non-blocking question was sent; no response has yet been recorded.

## Acceptance Checklist

- [ ] Measure burst queue latency and preserve key/mouse/control event order.
- [ ] Restore bounded batching through both input layers; no fake clicks or
  missing button releases; test scan-less keys and native handoff as well.
- [ ] Establish an authorized actual-use retirement mechanism, not HPCON
  liveness, direct-target lifetime or process-tree inference.
- [ ] COMMAND -> CMD -> exit -> new DOS input -> exit retires the frontend.
- [ ] A still-attached descendant retains its terminal; its final departure
  retires it. Detached surviving processes do not count as attached clients.
- [ ] Pending/new admissions race safely with retirement; no lost request.
- [ ] Resolve S10 Window native-zero/missing text-continuity failures without
  weakening snapshot assertions (15/17 is not a full pass).
- [ ] Full affected x86 build, existing regression and headless WOW frontiers,
  coherent validated publication, documentation checks, commit and push.

Concurrent queue/WOW proposal planning remains separate uncommitted work at
its author's explicit working-tree-only instruction. No blanket clean-tree
claim is made. This ledger contains research/admission, not a repair claim.
