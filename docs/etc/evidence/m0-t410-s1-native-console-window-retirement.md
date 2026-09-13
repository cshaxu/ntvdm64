# M0 T410 S1 — native Console window-route retirement

## Question

Can the project-owned independent presentation window be removed without
removing the selected native Console text-output path or introducing a
replacement GUI route?

## Source and build ledger

The removed route was entirely project-authored:

| Route element | Disposition |
| --- | --- |
| `src/app/presentation_window.[ch]` | Removed: custom window class, `WM_*` loop, keyboard/mouse injection, text snapshot and DIB display. It is not an OpenNT mirror body. |
| `entry.c` window initialization, activation preparation and shutdown | Removed with that route. |
| window fixtures and the presentation-mouse launcher | Removed. |
| formal app selection | `New-T310OriginalSoftpcNinja.ps1` now selects only `machine_shell.c`, `package_layout.c`, `launch_declaration.c` and `entry.c`. |
| observation presentation-toggle gesture | Removed; it existed solely to exercise the retired Alt+Enter/window branch. The remaining mouse observation path remains for later D29/D24 work. |

The retained native path is source-facing rather than an app GUI substitute:
`RegisterConsoleVDM` allocates the bounded VDM text plane, original SoftPC
writers populate it, and `InvalidateConsoleDIBits` commits the rectangle with
`WriteConsoleOutputA` in `console_compat.c`.  No `src/app` code now calls
`WriteConsoleInput*`.

The former graphics plane/event capacity remains a D34 question.  It has no
selected app consumer after this S; S1 neither claims graphical acceptance nor
deletes source-facing SoftPC renderer inputs merely because a GUI consumer was
retired.

## Procedure and results

1. Ran `tests/app/Verify-T410NativeConsoleRoute.ps1`: pass.  It proves the
   retired sources and build entries are absent, `entry.c` has no presentation
   reference, app has no Console-input injection, and the native text
   registration/invalidation/`WriteConsoleOutputA` path remains.
2. Generated formal x86 graph at
   `build/M0-T410/S1/r001-native-console-retirement`.  The default graph is a
   library-closure target, so it was not treated as an EXE proof.  Explicitly
   linked `original-softpc-process.exe` after building its two graph-selected
   dependencies (`original-opennt-base-vdm.lib` and `vdd-bindings.lib`).
   Link succeeded; product is 3,229,696 bytes, SHA-256
   `c0771b414f93b1f1644765447f89439860e23e6f9fb67a1319222955b267652f`.
3. Generated and compiled the remaining Console boundary fixtures and the
   `console-startup-observer`; the presentation-toggle target is absent.
4. Staged the linked x86 product at `O:\ntvdm64\ntvdm32.exe` under the
   product publication rule.  The same SHA-256 was recorded.
5. Ran two automatic native-Console COMMAND observations from a disposable
   formal-build stage, first without and then with its `VDMREDIR.DLL`
   companion. Both ended before DOS buffered-console input readiness with
   exit `0x00000040`; reports are
   `O:\ntvdm64\logs\m0-t410-s1-native-console-command.txt` and `-r2.txt`.
   A bounded direct `ntvdm32.exe MEM.EXE` check also failed to complete before
   the test window and its exact test process was stopped.

## Interpretation

D23 is source/build complete: the independent route has zero selected
consumers and no replacement window provider was added.  Static tests and the
final x86 link demonstrate that the retained native text Console route still
composes.

The automatic runtime container did **not** reach the existing COMMAND/MEM
baseline, so S1 does not claim a new keyboard, mouse or program-execution
pass.  Its short-path/package identity and startup failure must be reconciled
by the next integrated Console/input stage before using it as a regression
oracle.  This is a runtime-observation blocker, not evidence that removal of
the isolated window code changed guest event semantics.

## Follow-up

S2 owns D24/D28/D29/D30 and D09. It must establish a native Console baseline
that reaches guest input, then test the original queue and modifier contracts
without reinstating any app window or `WriteConsoleInput*` producer.
