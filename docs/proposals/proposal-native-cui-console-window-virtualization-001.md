# Proposal — Native CUI Console-window virtualization

## Status and objective

Owner reports that `mysmb64.exe` is an AMD64 `IMAGE_SUBSYSTEM_WINDOWS_CUI`
program which switches between an application window and its Console with Tab.
When run directly from CMD, both states belong to the original CMD Console. When
started through `run16`, its application window appears, but returning to its
Console exposes the hidden NTVWM carrier Console instead of the NTCON-managed
root frontend.

This candidate restores one logical Console presentation for a native CUI
target that changes the visibility or foreground state of its existing Console
window. It is an unadmitted candidate: it does not allocate a numeric task,
change the active T440 S5 packet, or claim a production repair.

## Established facts and boundary

The current classifier correctly identifies `mysmb64.exe` as an x64 CUI image;
the failure is not a PE-subsystem misclassification. `run16` deliberately
opens a text frontend for `IMAGE_SUBSYSTEM_WINDOWS_CUI` in
`src/run16-exe/main.c`. NTSRV starts NTVWM with `CREATE_NEW_CONSOLE` and
`SW_HIDE` in `src/ntsrv-exe/opennt/source/worker_registry.c`; NTVWM then
creates the CUI target with that carrier Console's standard handles in
`src/ntvwm-exe/execution.c` and `src/run16-exe/native_launch.c`. That carrier
is intentionally private while NTCON owns the visible Console/Window
projection.

The injected native hook currently governs CreateProcess propagation, not
`GetConsoleWindow` or Console-window visibility. A CUI target that displays
the HWND returned by `GetConsoleWindow` can therefore reveal NTVWM's hidden
carrier. The exact API sequence used by the Tab transition remains to be
captured at admission: likely `ShowWindow`/`ShowWindowAsync`,
`SetForegroundWindow` or `SetWindowPos`, but `FreeConsole`/`AllocConsole` and
`AttachConsole` must be positively excluded before choosing the repair.

## Required contract

- Keep CUI classification and the text-worker route. A CUI image that creates
  an ordinary application HWND must not be reclassified as GUI merely because
  it later shows a window.
- Preserve the worker's hidden Console as a carrier; it must never become a
  user-visible fallback because a target requests visibility of its Console.
- Map a proven request to show/foreground the target's *existing* carrier
  Console to the existing NTCON Console projection. Map a proven hide request
  only to the corresponding frontend presentation state where that preserves
  normal Console semantics.
- Do not infer control from arbitrary HWND operations. The interception must
  apply only when the HWND is the calling target's current Console window and
  the target has the authenticated existing frontend context.
- Do not alter a target's standard streams, Console ownership, process
  completion, NTSRV lifecycle authority, NTVWM hidden-Console model, or the
  ordinary direct-CMD behavior.
- A true `FreeConsole`, `AllocConsole`, or `AttachConsole` transition changes
  Console identity. It is not silently emulated by a visibility hook; audit
  and explicitly retain/refuse/design that distinct case.

## Proposed ownership

| Owner | Candidate responsibility |
| --- | --- |
| `nthook32-dll` / `nthook64-dll` | Finite same-process interception and exact current-Console-HWND recognition; no frontend rendering, worker ownership, polling, or lifecycle authority. |
| NTVWM / worker-base | Receive only a bounded worker-side presentation request using the already authenticated text channel; retain target execution, input and final-I/O behavior. |
| NTCON | Reuse its existing frontend display-selection mechanism to select the Console projection. It remains worker-neutral and does not identify `mysmb64` or any application. |
| NTSRV | Retain existing authenticated binding, worker selection, completion and lifecycle control; no per-window process tracking or new Console registry. |
| run16 | Retain image classification and request submission. No filename, subsystem exception or application-specific route. |

No new persistent process, global hook, Job object, polling loop, observed-task
record, application allow-list, or guest/media change is permitted.

## Proposed S sequence

| S | Deliverable and stop condition |
| --- | --- |
| S1 | Reproduce direct CMD and `run16` paths; capture the target's actual Console/window API sequence and identities. Stop if it allocates or attaches a distinct Console rather than changing visibility of its inherited carrier. |
| S2 | Add the smallest authenticated visibility-request route for the proven API family, with exact HWND/current-Console validation and no target-name branch. Cover both hook widths, failure passthrough, recursion, and target completion. |
| S3 | Integrate the request with existing NTCON Console/Window selection, then verify CUI text I/O, ordinary GUI targets, CMD nesting, direct CMD behavior, Console/Window switching and hidden-carrier non-exposure. |

## Acceptance

- Direct CMD → `mysmb64` retains its current Console/window toggle behavior.
- CMD → `run16 mysmb64` keeps the CUI target on NTVWM/NTCON's text route;
  switching back to Console returns to the root NTCON projection, never an
  NTVWM carrier Console window.
- A native CUI program that never requests Console-window visibility is
  unchanged; native GUI, DOS, Win16 and existing text-worker handoffs do not
  acquire a new route.
- The proven visibility API is invoked only for the target's current Console
  HWND. Unrelated HWND calls, missing frontend context, explicit new-Console
  child creation and unsupported identity-changing Console calls retain their
  documented native behavior or explicit failure.
- Build the affected width artifacts and run focused positive/negative tests,
  then the appropriate serial product Console/Window regressions before any
  package publication.

This candidate must begin with source-backed API tracing; it must not guess
that Tab means `ShowWindow`, nor use a program-specific special case to hide
the carrier window.
