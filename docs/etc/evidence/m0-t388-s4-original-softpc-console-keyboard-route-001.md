# M0 T388 S4 original SoftPC Console keyboard route

## Question

Can one ordinary single-process session use the original SoftPC Console
event/keyboard/IRQ1 route, and can its worker end before the session is
released, without introducing an application-owned DOS input path?

## Inputs

- `mvdm-host/softpc.new/host/src/nt_event.c`
- `mvdm-host/softpc.new/host/src/nt_keycd.c`
- `mvdm-host/softpc.new/base/keymouse/keyba.c`
- `mvdm-host/dos/command/cmdmisc.c`
- `mvdm-host/softpc.new/host/src/nt_fulsc.c`
- `adapter-mvdm-host-out/win32/source/{console_compat.c,nt_thread_alert_compat.c}`
- `adapter-mvdm-host-out/softpc/mvdm_softpc_event_thread.c`

## Source route

1. Original `cmdInitConsole` sets `fConOutput` and calls original
   `nt_init_event_thread`.
2. Original `nt_init_event_thread` resumes the original event worker.
3. Original `nt_event_loop` waits on the public Console input handle and
   calls same-shaped `ReadConsoleInputExW`.
4. Original `nt_process_keys` calls `nt_key_down_action`/
   `nt_key_up_action`; these retain `KeyMsgToKeyCode` and the original
   `host_key_*_fn_ptr` dispatch.
5. Original `keyba.c` deposits the scan code through its keyboard-controller
   path and raises `ica_hw_interrupt(0, 1, 1)`: original IRQ1.

The route contains no app text parser, BOP record producer or guest-memory
write. `GetConsoleInputWaitHandle` and `ReadConsoleInputExW` are the bounded
adapter-win32 same-shaped bindings to public Console handles and input APIs.

## Teardown binding

Original `nt_remove_event_thread` had only `NtAlertThread`, which was enough
when NT4 ended the process.  In the retained in-process session, the new
`mvdm_softpc_event_thread_alert_and_join` adapter duplicates the host-local
worker handle before the unchanged alert direction, then waits on the
duplicate.  This prevents the source worker's own `CloseHandle` from racing
the join.  No guest address, guest handle, mapping token or input payload
crosses the helper.

`MVDM-HOST-DIV-206` and `ADAPTER-SOFTPC-045` register this sole source/adapter
divergence.

## Verification

- `New-T310HeartbeatNinja.ps1 -Architecture x86`, followed by
  `all-verify`, compiled and ran:
  - existing alertable `NtAlertThread` fixture;
  - existing source-worker session-binding fixture;
  - new `softpc-event-thread-shutdown-fixture`, which starts an alertable
    worker, alerts it through the adapter and proves joined exit code zero.
- `New-T310HostInputNinja.ps1 -Architecture x86`, followed by `ninja test`,
  passed `console_input_contract_fixture.exe` with
  `PASS: console compatibility input and video contracts`.
- The formal CPU40/x86 graph rebuilt `nt_event.c`, the new adapter object,
  `softpc-bindings.lib`, `original-softpc-host-roots.lib` and
  `original-softpc-process.exe`.  The link succeeded.  Historical source
  warnings remain visible and were not suppressed.

## Interpretation

S4 is source and locally closed: the public Console is routed through the
original SoftPC event/keyboard controller/IRQ1 implementation, and normal
teardown no longer leaves that original worker bound to the session.  This is
not evidence that an interactive prompt, editing, Ctrl+C or `exit` has run in
the product; those are fixed-container runtime rows reserved for S7.  It also
does not choose a graphics/fullscreen surface; that is S5/S6.
