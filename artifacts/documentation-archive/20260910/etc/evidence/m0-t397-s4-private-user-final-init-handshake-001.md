# M0 T397 S4 — private USER FinalUserInit handshake recovery

## Reached contract

The S3 fixed-stage observation stopped at the original dialog/message path.
The next direct normal launch of `system32\\WOWEXEC.EXE` exposed an earlier
startup condition hidden by that long-lived path: `WU32NotifyWow` processing
`FUN_FINALUSERINIT` displayed `WOW Error: user.exe and user32.dll are
mismatched.`  Source inspection identifies the condition precisely: modern
`UserRegisterWowHandlers` preserves its export but leaves NT4's private
`PFNWOWHANDLERSOUT.dwBldInfo` and
`pfnWowGetDefWindowProcBits` unpopulated.

## Bounded recovery

`ADAPTER-WOW-013` adds no replacement USER implementation. Its sole callback
zeroes the original caller-provided default-procedure bitmap and returns its
last valid bit, while `WU32NotifyWow` supplies the selected USER16 build value
when the unavailable USER32 output is zero. This retains the original
`FUN_FINALUSERINIT` control flow and leaves all window, dialog, queue, task,
CSRSS, Win32k and guest ABI behavior unclaimed.

## Verification

1. The expanded x86 projection fixture verifies the callback's normal and
   empty-input behavior, as well as the existing opaque-window checks.
2. `wow32.c`, `wuman.c`, and the adapter compiled with the formal x86 provider
   flags; the complete provider linked without `/FORCE` against the fresh
   parent import library.
3. The staged ordinary `WOWEXEC.EXE` observer reports `loaded-wow32=yes`, no
   modal continuation selection, and normal exit `0x00000001` in 2,375 ms.
   The prior `WOW Error` panel is absent.
4. The bounded provider report records `init-success`, multiple original
   `W32Dispatch` calls and returns, and `exit-kernel=0001`. Thus the recovery
   advances through FinalUserInit and does not misrepresent an empty WOWEXEC
   manager invocation as a completed Win16 application lifecycle.

## Next boundary

The selected immutable `write/WRITE.EXE` was then staged and launched through
the same app entry. Its BaseVDM witness proves the original configuration
consumer requested `ASKING_FOR_PIF | ASKING_FOR_WOW_BINARY` (`0x0102`) from
the published WOW record. In a no-diagnostics run it still enters the provider
and exits through `exit-kernel=0001`, without reaching either the new original
`WU32RegisterClass` or `W32CreateWindow` witnesses. Therefore the current
frontier is earlier than `WOWEXEC::InitializeApp`; it is not a class/window
projection failure and it is not evidence that WRITE executed.

The existing full-diagnostics observer is deliberately not substituted for
this result: its fixed reporting environment changes the path into the known
pre-WOW CPU/EOI loop (no WOW32 load), while the ordinary no-diagnostics
product loads WOW32. S4 remains open to recover the true ordinary bootstrap
predecessor of the missing WOWEXEC task handoff.

## Profile-root recovery result

Kernel31's original WOW slow boot calls `GetPrivateProfileString` for bare
`SYSTEM.INI` and asks `[boot]` for `WOWSHELL`; its source default is
`WOWEXEC.EXE`. Public Win32 resolves that bare name under the host Windows
directory, not the selected VDM media root. `MVDM-HOST-DIV-250` therefore
redirects only this exact bootstrap filename through the existing read-only
session media lookup; all other profile paths retain the original public API.
The parent now exports that existing lookup to the late provider and both
images relink normally.

An ordinary staged WRITE run records `profile-root-hit`, proving the original
profile thunk reaches the bridge, but then records `exit-kernel=0001` and no
DEM open of `WOWEXEC.EXE`. Thus the bridge is necessary and active but does
not itself complete the preceding Kernel31 boot-module/shell load failure.
The next S4 audit must classify that exact `LoadNewExe`/`LoadModule` failure
without changing guest media or inventing a shell.

## Exit-thunk coordinate recovery

The ordinary staged `WRITE.EXE` route was repeated with the existing
provider-local transition report.  `WK32ExitKernel` now records only the
already-built `VDMFRAME` scalar fields before its original non-returning
boundary.  The resulting terminal record is:

```text
exit-kernel=0001 task=023F call=00000002 app=021F8347 thunk=021FB8AE
```

The selected retail Kernel31 map resolves those coordinates without guessing:
the loaded selector `021F` is Kernel segment 1, `8347` is
`EXITKERNEL+0006`, and `B8AE` is `EXITKERNELTHUNK+000D`.  The original
provider therefore reaches the Kernel `EXITKERNEL` API thunk and deliberately
requests nonzero termination; this is neither a loader failure nor a missing
WOW32/UI callback.  The report does not claim the preceding Kernel caller,
which remains the next bounded bootstrap provenance question.

During the rebuild, the generated provider graph was found to retain an old
T396 parent import library although `New-T396Wow32ProviderNinja.ps1` accepts
the selected parent explicitly.  Regenerating it with T397's
`original-softpc-process-import.lib` linked the complete 77-body provider
normally and made the current parent exports (`session_*` and
`mvdm_softpc_system_find_file`) available.  This is a build provenance repair,
not a guest or WOW behavior change.

## Task hand-off and cleanup recovery

### Question

After the normal projected class/window path creates a second original WOW
task, does the provider fail in the historical private USER task hand-off or
in its corresponding task/module cleanup?  The question is deliberately
narrow: it does not claim that the selected Win16 application's UI is usable.

### Inputs and procedure

The source-shaped x86 parent at
`build/M0-T397/S3/formal-x86/original-softpc-process.exe` was linked with the
complete x86 WOW32 provider at `build/M0-T397/S3/wow32-x86/wow32.dll`.  The
provider was explicitly copied to the existing `O:\t396` stage (the stage
update script intentionally retains a pre-existing provider), then the
ordinary product route was launched as:

```text
ntvdm32.exe system32\WRITE.EXE
```

with only the existing provider-local report selector set.  The run was
bounded: it was observed for 20 seconds and its specifically launched process
was then stopped.  A second bounded observation measured CPU time over five
seconds before stopping that same launched process.

Source inspection identifies both original consumers of the missing output
slot.  `WU32SignalProc` calls `pfnWOWCleanup` when a task exits and performs
module cleanup; `W32DestroyTask` calls it if the task did not already do so.
The original comment describes the NT4 private USER-server responsibility:
cleaning USER objects created by a module, particularly classes and
subclassed windows.  Modern `UserRegisterWowHandlers` leaves that slot null.

`ADAPTER-WOW-018` therefore installs only
`wow_private_user_cleanup`.  It returns success after accepting and discarding
the historical module/task arguments.  This is source-shaped because the
standalone's public projected windows retain their own session-scoped lifetime
and there is no private USER server object graph for this callback to destroy.
It does not emulate an NT4 USER object server, enumerate native windows, or
change a guest ABI.

The already-admitted task callback additionally serializes hand-off between
the original WOW task threads with per-task events.  It never signals a task
that is still executing CCPU guest state; a waiting task is resumed only when
another task yields.  This preserves the required single-guest-execution
property without creating a USER task list or using native handles as guest
identities.

### Observations

The bounded report recorded, in order, the original `InitTask` entry, a second
`task-thread-enter`, and the original `WK32Yield` dispatch.  The process then
remained alive for the 20-second observation with no provider `exception` or
`syserror` marker.  The independent five-second measurement reported:

```text
alive=True cpu_seconds_5s=0
```

Thus the retained process is not an observed busy loop.  It is consistent with
a wait state after the original dispatch path, but does not prove a native
WRITE window is visible or accepts input.  This execution environment did not
expose the launched native window to its UI observer, so no UI assertion is
made from that absence.

### Interpretation and follow-up

The previous terminal zero-call during `SIGNALPROC(SG_EXIT)` is removed by a
bounded adapter at the exact unavailable private-USER output boundary.  The
ordinary route now advances through second-task creation, cooperative yield,
and task/module cleanup into a non-spinning live wait.  This is progress in
the S4 dialog/message boundary, not a closure: the next audit must resolve
which original message wait/dispatch owns the stable state and establish a
separate visible-window/input witness before any Win16 UI or lifecycle claim.

## Ordinary projected-HWND visibility recovery

The previous ordinary run had three top-level windows but none visible.  That
observation was revisited after inspecting the original `walias.h` conversion
boundary.  The public-window projection intentionally returns an opaque
16-bit guest window ID from `W32CreateWindow`; however, original `HWND32(h16)`
still sign-extended that ID and supplied the invented value directly to public
USER32 APIs.  `CreateWindow` itself therefore succeeded, while later original
operations such as `ShowWindow(HWND32(...))` could not recover the native
window.

`MVDM-HOST-DIV-254` keeps the opaque guest ID in guest/WOW bookkeeping, but
changes only the public-USER conversion macro to resolve an existing projected
ID back to its native HWND.  An unprojected value retains the original signed
16-bit conversion.  The focused x86 projection fixture verifies both cases,
including the `0xffff -> -1` fallback; the full provider then rebuilt and
linked normally without `/FORCE`.

The refreshed Console-owning, no-diagnostic fixed-stage observation was:

```text
ntvdm32.exe -f -o --command system32\WRITE.EXE
loaded-wow32=yes
top-level-window-peak=3
visible-top-level-window-peak=1
visible-top-level-window-first-class=WOWExecClass
visible-top-level-window-first-title=WOWExec
result=timeout
exit=0x53504354
```

The controlled timeout is intentional: the observer stopped the live product
after 30 seconds.  This proves that the ordinary path now creates and shows
the native WOWEXEC shell window; it does **not** prove that `WRITE.EXE` has a
visible application window, receives input, or can complete its lifecycle.
The provider-report environment remains unsuitable as ordinary acceptance: in
this container that diagnostic selection reproduces the known pre-WOW path
(`loaded-wow32=no`), so it is not used to infer the normal task-handoff state.

A second ordinary run used an expanded passive top-level-window census.  It
recorded exactly `WOWExecClass`/`WOWExec` (visible), `WOWFaxClass` (hidden),
and the system `IME` helper (hidden).  No WRITE-class or other application
window appeared even transiently during the 30-second sample.  The remaining
frontier is consequently after the shell's successful visible initialization
and before WRITE's first native top-level window creation.  It is not a
console-presentation failure, a hidden WRITE window, or a projected-HWND
`ShowWindow` failure.

## First-turn ordering and default-window-procedure refinement

The two live task threads from the ordinary run were resolved against the
current parent and provider maps.  Both have the original `W32Thread` start
address and are stopped under `Win32_host_timer`; neither is an uncreated
application record or a synthetic worker.  Kernel31 `StartWOWTask` explicitly
requires its new task to wait until the creator returns from `WK32Yield` and
performs the source-owned yield.  `ADAPTER-WOW-019` therefore adds only that
missing first-turn admission on the existing per-task event, after the
original creator-event signal.  It creates no guest task list or second CPU
scheduler.

The same source pass found that USER16's `DWPBits` table gates whether its
`DefWindowProc` thunk reaches the retained WOW32 marshaller.  The prior
adapter exposed `WM_NCCREATE` only.  `WM_NCCALCSIZE` has its retained
`WM32NCCalcSize` marshaller and is the next default non-client calculation in
the native creation sequence, so the bounded table now exposes both bits.  A
focused x86 projection fixture verifies the two-bit contract, projected-HWND
resolution, and the original signed-16 fallback; the complete 77-body
provider relinks without `/FORCE`.

Neither change yet produces a WRITE top-level window in a fresh 30-second
no-diagnostic staged run: the passive census is still exactly visible
`WOWExecClass`/`WOWExec`, hidden `WOWFaxClass`, and hidden `IME`, with two
live original `W32Thread` instances.  This is negative runtime evidence, not
a closure claim.

The existing provider transition-report selector was also rechecked solely
as a diagnostic control.  In this Console-owning container its presence
reproduces the known altered pre-WOW startup (`loaded-wow32=no`, 15 modules,
no top-level windows, and no report file), whereas the normal run has
`loaded-wow32=yes`.  It is consequently disqualified from this frontier's
runtime inference; all UI and lifecycle assertions remain based on the
no-diagnostic ordinary route.
