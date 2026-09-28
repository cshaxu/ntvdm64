# run16

`main.c` is the public CreateProcess-shaped launcher. It discovers the image,
calls the selected original classifier, and starts/submits and waits for its
direct target. It does not own Console/Window presentation, input pumps,
hidden Console helpers, a guest executor or BaseSrv DOS/WOW record policy.

## Launch contract

```text
run16 [--wait] [--] <binary> [arguments]
```

`launch_options.c` consumes only leading launcher options; the remaining
command text retains target arguments. DOS and native Console targets remain
synchronous. Native GUI targets return the actual creation result by default;
`--wait` waits for the process and returns its exit code. Accepted native
targets are not terminated merely because their launcher exits.

WOW targets use original BaseCheckVDM/Update/Get/Exit task records. Default
asynchronous launch waits for the authenticated original InitTask startup
notification (or original redundant-WOWEXEC successful no-op), not merely queue
admission or worker existence. `--wait` retains original task completion;
it does not promise an invented Win16 process exit-code contract. Worker
failure and task completion are separate outcomes. Project Status and the S8
evidence record own verification, publication and delivery state.

Non-image command text retains the existing COMSPEC /c fallback for COMMAND
built-ins, batch files and shell syntax. Classification failure routed through
that path returns the shell result, not a fabricated GUI creation result.

## Frontend and service boundary

`frontend_scope.c` is a client of the independent `ntkvm.exe` frontend. It
resolves or requests an authenticated character-session association, submits
native Console execution, and waits for its direct target. It owns no renderer
or frontend notification pump. `ntkvm-exe` owns visible Console, Window,
display state, hidden Console/helper, direct worker I/O and input routing.
Original guest devices, painters and execution remain in ntvdm/MVDM.
GUI segments do not inherit character-frontend authority; character segments
can share their authenticated frontend without sharing completion ownership.

The ntsrv client library owns the service protocol and bindings to original
BaseSrv records. `console_probe.c` supplies its bounded private Console
membership query; it is not an interactive frontend. Native handles remain
local or explicitly authenticated transferred capabilities. No frame/input
stream is relayed through the broker.
