# Proposal — Product experience repair

## Objective and admission

Repair user-visible launch, Console ownership and completion behavior while
preserving original DOS/WOW execution semantics and verified capabilities.
The owner places this candidate immediately before the project-wide
original-source implementation minimization review. It is unnumbered and
queued; it does not change the active T420/S40 packet.

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

run16 owns external launch and parent completion; ntvdm owns execution and
worker-local cleanup; basesrv owns registered coordination. Preserve original
owners and use minimal bindings. Do not conceal a lifetime defect with an
arbitrary timeout or unconditional termination.

The preceding [error-response proposal](proposal-error-dialog-termination-semantics-restoration-001.md)
owns original Abort/Retry/Ignore and WOW hard-error behavior. This candidate
verifies its results in product launch flows rather than duplicating that work.

## Proposed S tasks

| S | Scope | Exit condition |
| --- | --- | --- |
| S1 | Reproduce drag-and-drop retention; trace arguments, Console ownership and launcher/worker/broker states for Explorer and existing-shell entry. | Identify the first incorrect transition for each defect; separate legitimate interactive residency from leaks. |
| S2 | Repair launch/completion and owned-Console lifecycle, including one-shot success and startup failure. | Completed runs release unintended product-owned windows/tasks; inherited shells remain usable; interactive COMMAND/EDIT remain active until legitimately finished. |
| S3 | Verify interactive exit, cancellation/window closure, nested COMMAND and concurrent workers; repair adjacent evidenced experience defects. | No stale task/window remains; one run cannot damage another; original guest exit semantics are preserved. |
| S4 | Formal x86 build, deployment, full regression and minimal-diff review. | Existing direct/nested COMMAND, MEM and EDIT tests and real Explorer scenarios pass; governance passes; reviewed changes are committed/pushed for owner testing. |

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
unrelated refactoring. Retain the three-program architecture.
