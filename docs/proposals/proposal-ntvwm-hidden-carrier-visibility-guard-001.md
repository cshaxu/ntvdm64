# Proposal — NTVWM carrier visibility guard

## Status and objective

Owner admits this package as T446. Current owns its sole active S1 brief.
The proposed three-step sequence below is consolidated into one end-to-end
implementation/integration/verification delivery; scope and acceptance remain.

NTVWM owns a private execution Console for native text targets. NTSRV starts
the worker with `CREATE_NEW_CONSOLE` and `SW_HIDE`; the carrier is not a
user-facing Console. The current `run16 mysmb64.exe` observation establishes
that a target can show that inherited carrier and leave it visible after its
normal Esc exit. The product requirement is that NTVWM keeps its own carrier
hidden throughout worker residency, and hides it once more at direct-target
completion before returning that completion to the outer caller.

The mechanism is a dedicated, event-driven NTVWM-local guard. It must not poll
and must not modify the existing `presentation_pump` capture cadence, route
ownership, input handoff, final capture, or completion protocol.

## Established boundary

- `src/ntsrv-exe/opennt/source/worker_registry.c` starts `ntvwm.exe` with a
  new Console and `SW_HIDE`.
- `src/ntvwm-exe/console_state.c` owns carrier Console setup and explicit
  closure; its fixed 2x4 font accounts for the observed miniature window.
- `src/ntvwm-exe/execution.c` waits for each direct native text target and
  calls its `io.end` callback only after the target process has signalled.
- `src/ntvwm-exe/main.c` implements that callback as `end_io`: it completes
  final capture/input return, releases presentation ownership and only then
  allows the exact native completion receipt to be reported.
- The observed `O:/WINNT/mysmb64.exe`, identical to the owner-maintained
  sibling-project asset, shows its inherited carrier during its Console
  presenter path. Esc is a normal target exit, not NTVWM shutdown.
- `tests/observation/ntvwm_console_winevent_probe.c` establishes that an
  out-of-context Console WinEvent hook requires a message loop and that the
  worker can receive Console-originated events across the worker/conhost
  process boundary.

The retained [native-CUI Console-window virtualization candidate]
(proposal-native-cui-console-window-virtualization-001.md) concerns a larger
target-intent-to-NTCON route. This proposal neither revives that route nor
changes Console identity, target lifetime, frontend/lifecycle authority or
native output ownership.

## Required contract

- NTVWM starts a private guard only after its carrier Console exists. The
  guard records that exact carrier HWND; it never discovers windows by title,
  class, target name, process enumeration, or NTVWM process ID.
- The guard uses `SetWinEventHook` for `EVENT_OBJECT_SHOW` with
  `WINEVENT_OUTOFCONTEXT`. Its dedicated thread runs a message-aware wait and
  has no periodic timeout or polling path.
- A callback acts only when `idObject == OBJID_WINDOW`, `idChild == 0`, and
  the event HWND equals the recorded carrier HWND. It requests
  `ShowWindowAsync(hwnd, SW_HIDE)` and returns. It takes no NTVWM I/O lock,
  sends no RPC, captures no frame, waits for no process, and changes no target
  result.
- The hook cannot filter on NTVWM's PID: the carrier HWND is owned by
  `conhost`. Exact-HWND comparison is the authority boundary. A missing HWND
  on a pseudoconsole/no-window host is a successful no-op.
- All NTVWM-managed carrier-close paths stop and join the guard before
  `FreeConsole` or `ntvwm_console_close`. A cosmetic guard-start or hide
  failure is logged/best-effort only and must not replace a target exit code,
  final-I/O result, input handoff, completion receipt, worker residency
  decision, or normal shutdown result.
- At direct-target completion, after final presentation work and before
  release/completion delivery, NTVWM makes one final best-effort `SW_HIDE`
  request for the same carrier HWND. This is an ordering barrier, not a second
  polling or lifecycle policy.
- Do not alter `presentation_pump`, worker-base, NTSRV, NTCON, run16, Hook,
  guest images, protocol records, target launch identity, target lifetime or
  native output ownership. A target may briefly expose its inherited carrier
  before the asynchronous event is delivered; anti-flicker policy is outside
  this package.

## Proposed S sequence

| S | Deliverable and stop condition |
| --- | --- |
| S1 | Add the NTVWM-local opaque carrier-guard object in `console_state`: dedicated guard thread, `EVENT_OBJECT_SHOW` out-of-context hook, exact-HWND callback, message-aware stop/shutdown wait, and deterministic unhook/join. Keep the implementation separate from `ntvwm_console_close`; no polling, route, lock, IPC, target or result behavior may enter the callback. Stop for any requirement to observe a non-carrier window, redirect target intent, or modify `presentation_pump`. |
| S2 | Integrate guard start after carrier initialization, final completion-boundary hide in `end_io`, and one shared stop-before-close wrapper across management shutdown and local fatal-close paths. Preserve existing worker/error semantics and make no lifecycle/protocol change. Stop for a need to alter NTSRV worker death handling or target survival semantics. |
| S3 | Extend the hidden-Console WinEvent fixture with an actual show-to-hide guard case and shutdown race case. Verify no polling wake occurs, only the recorded carrier is hidden, completion is still delivered after final I/O, and guard teardown precedes Console closure. Build NTVWM and run the required native Console/Window regression set. |

## Acceptance

- `run16 mysmb64.exe`, followed by Esc, returns normally without a remaining
  visible 2x4-font NTVWM carrier Console.
- If a live target calls `ShowWindow` on its inherited carrier, NTVWM receives
  the carrier's show event and asynchronously hides only that carrier; target
  execution and outer Console/NTCON routing remain intact.
- The guard has no periodic timeout or `IsWindowVisible` polling; its callback
  does not acquire the presentation/I/O lock or perform blocking work.
- The outer Console/NTCON restoration occurs after the existing final
  presentation barrier and still precedes the returned completion.
- Native text capture, direct CMD, nested native completion, worker reuse,
  native GUI targets, no-HWND hosts and management shutdown preserve their
  existing behavior.
- Only `ntvwm.exe` changes in the product package; no NTSRV, NTCON, run16,
  Hook, protocol or guest image changes are introduced.
