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

The existing `frontend-client.lib` now has this launcher source owner:
`bootstrap_client.c`, `native_request_client.c`, `native_request_io.c` and
`native_launch_packet.c`. Public declarations are under `interface`.
NTKVM reuses the transport/codec without linking target creation.
`native_launch.c` is the separately linked restricted resource/CreateProcess
primitive used by run16 GUI creation and NTCON native text creation. It owns
no worker state, Console session, execution policy or frontend renderer.

`frontend_scope.c` is a client of the independent `ntkvm.exe` frontend. It
resolves or requests an authenticated character-session association, submits
native Console execution, and waits for its direct target. It owns no renderer
or frontend notification pump. `ntkvm-exe` owns visible Console, Window,
display state, direct worker I/O and input routing. NTCON owns its ordinary
hidden Console and native targets; native completion alone does not end its
worker residency. There is no product helper or ConPTY backend.
Original guest devices, painters and execution remain in ntvdm/MVDM.
GUI segments do not inherit character-frontend authority; character segments
can share their authenticated frontend without sharing completion ownership.

The ntsrv client library owns the service protocol and bindings to original
BaseSrv records. NTKVM reports its attached root Console identity through its
authenticated connection; BaseSrv owns matching and resident-worker reuse.
The launcher does not register an outer Console member snapshot. Native
handles remain local or explicitly authenticated transferred capabilities. No
frame/input stream is relayed through the broker.
