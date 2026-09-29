# Proposal — Product experience repair

## Objective and admission

Repair user-visible launch, Console ownership and completion behavior while
preserving original DOS/WOW execution semantics and verified capabilities.
By latest owner direction, this former candidate is removed from the T queue
and its scope is transferred to the final planned S14 of the
[Console/Window frontend package](proposal-kvm-window-graphics-presentation-001.md).
It shares that package's component-lifecycle ownership and is not a separate T.
The previous queue-position-2 instruction is superseded. Owner inserted text
geometry repair as S13 on 2026-09-29; this former S13 is now S14, admitted
by the owner on 2026-09-29 and closed after owner-reported verification on
2026-09-29. T423 stays open and no next S is automatically admitted. CURRENT
is the sole packet-status authority.
The former RDP S13 plan was cancelled because the owner confirmed it resolved;
this product-experience scope does not reopen RDP work.

## Initial defect and scope

Owner-reported symptom: dragging a program onto run16 can leave a Console
window that never disappears. Its cause is not yet established. Reproduce
actual Explorer drag-and-drop and distinguish an active interactive guest,
a blocked launcher, a worker awaiting a legitimate command, and a completed
run whose Console remains owned by another product process.

Audit and repair the surrounding launch/use/exit experience: quoted paths,
arguments, working directory, missing/unsupported-image errors, inherited
versus product-owned Console lifetime, normal exit, startup failure and user
cancellation/window closure. Record each adjacent confirmed defect separately.

The owner also reported that Window-mode DOS COMMAND → native CMD → modern
Microsoft Edit shows NTCON's square pointer but does not activate the Edit
menu on click. Test the actual input representation at the NTCON hidden
Console boundary: a native `MOUSE_EVENT` consumer and a VT-input consumer may
require different input records while receiving the same pointer position.
Keep NTVDM's original mouse contract and the shared text frame unchanged.
In Window, Ctrl+Alt+M releases the captured pointer to the host without
changing display mode, closing the Window or delivering the hotkey to the
target. The next deliberate capture gesture may recapture it.

run16 owns external launch and parent completion; NTVDM and NTCON own their
respective workers; NTKVM owns visible presentation; NTSRV owns registered
coordination. Reuse the completed S12 lifecycle implementation. Preserve original
owners and use minimal bindings. Do not conceal a lifetime defect with an
arbitrary timeout or unconditional termination.

The later [error-response proposal](proposal-error-dialog-termination-semantics-restoration-001.md)
owns original Abort/Retry/Ignore and WOW hard-error behavior. This scope
reuses any completed error-response results in product launch flows. Any proven
blocking dependency must be explicitly promoted rather than duplicated or
assumed complete.

## Work items within the final S

| Work item | Scope | Exit condition |
| --- | --- | --- |
| 1 | Reproduce drag-and-drop retention; trace arguments, Console ownership and launcher/worker/broker states for Explorer and existing-shell entry. | Identify the first incorrect transition for each defect; separate legitimate interactive residency from leaks. |
| 2 | Repair launch/completion and owned-Console lifecycle, including one-shot success and startup failure. | Completed runs release unintended product-owned windows/tasks; inherited shells remain usable; interactive COMMAND/EDIT remain active until legitimately finished. |
| 3 | Verify interactive exit, cancellation/window closure, nested COMMAND and concurrent workers; repair adjacent evidenced experience defects. | No stale task/window remains; one run cannot damage another; original guest exit semantics are preserved. |
| 4 | Formal x86 build, deployment, full regression and minimal-diff review. | Existing direct/nested COMMAND, MEM and EDIT tests and real Explorer scenarios pass; governance passes; reviewed changes are committed/pushed for owner testing. |

S14's focused exit cases are direct `run16 winmine` (Win16 GUI) and
`run16 notepad.exe` (Win32 GUI) with no lingering product-owned Console,
plus interactive `run16 command` and `run16 cmd` followed by `exit`, with no
launcher/frontend wait left stuck. The actual host Edit location is discovered
from Windows system directories; no fixed drive letter is a product input.
The modern Edit test must show the negative control (raw `MOUSE_EVENT` leaves
the menu closed) and positive control (the production NTCON translation opens
the menu). A mouse-square movement alone is not a passing click test.

## Acceptance

Record package hashes, invocation method and arguments, Console ownership,
related process IDs, broker task state, actual guest text and exit results.
Cover paths containing spaces, missing/unsupported images, one-shot commands,
interactive COMMAND, EDIT returning to COMMAND and nested commands. Actual
Explorer drag-and-drop is required; shell-only invocation is insufficient.
Verify conhost and Windows Terminal where available and state untested modes.

A product-owned Console should be released after its associated run finishes;
an inherited Console remains usable. Active interactive work must not be
closed prematurely. A new pause-on-exit policy requires an explicit behavior
decision, not an undocumented lifecycle workaround. Preserve the established
broker lifetime and independent workers.

Report defects, causes, owner placement, test results, limitations and mirror/
non-mirror line deltas. Use established build/test/log locations. The later
minimization task receives completed boundaries, not unfinished UX repairs.

## Non-goals

No guest-media changes, new idle-worker timer, replacement COMMAND parser,
arbitrary process killing, host installation changes, generic UX framework or
unrelated refactoring. Preserve the approved executable ownership boundaries.
