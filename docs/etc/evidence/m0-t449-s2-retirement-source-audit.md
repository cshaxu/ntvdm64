# T449 S2 — M5 ordinary task-retirement source audit

This is a source-first working audit, not an individual-task-deletion design or
S2 closure. It traces normal WOW task exit on the selected provider while
keeping the original `HTASK`/`TD` owner separate from NTSRV lifecycle control.

## Normal exit chain

The original worker entry `wkman.c::W32Thread` assigns the real guest
`td.htask16`, links the TD under `gcsWOW`, and hands task initialization to the
registered original-compatible callback. `WK32KillTask` and the fallback
unsimulate path both call original `W32DestroyTask`; they are not replaced by a
service command or a project-owned thread terminator.

`W32DestroyTask` retains the original sequence: hung-app notification,
`W32FreeTask`, per-TD GDI cleanup, `ExitVDM` notification, original
`pfnWOWCleanup`, TD unlink, and thread-handle close. The selected adaptation
interposes only the existing cleanup seam:

```
W32FreeTask
  -> wow_user_task_prepare_exit (destroy task-owned native windows safely)
  -> WowOriginalFreeTask (original task/resource cleanup)
W32DestroyTask -> pfnWOWCleanup
  -> wow_user_task_lifecycle_exit
  -> original bound user/module cleanup
```

`prepare_exit` marks the task cleanup phase, enumerates only table entries
owned by that task, prepares their retained views, asks native USER to destroy
them without holding the project USER domain lock, and verifies retirement.
It fails closed rather than retaining a parallel window-cleanup engine. The
registered cleanup callback distinguishes failed initialization (no published
USER carrier) using the original live TD plus current thread identity, from a
committed task whose original cleanup must run. `retire_task` removes the
adapter carrier only after original cleanup and native hooks/object borrowers
have all been released.

## Authority and non-goals

* The TD and `htask16` are owned by original `wkman.c`; no service request ID
  is cast to either one.
* NTSRV retains whole-worker lifecycle control. This ordinary exit path reports
  the existing `ExitVDM` outcome; it does not implement S3 targeted deletion.
* Native window destruction and native handle lifetimes remain host-owned;
  copied owner tables are checked projections and are withdrawn only after
  native retirement.
* A cleanup/ref-count failure uses the established terminal boundary instead of
  returning success with a partially destroyed task.

## Current-candidate normal-retirement witness

The construction witness at
`build/t449-s2-message-construction-r4` used the current candidate WOW32
(`E1424CAC174C3388D293142A1A7F159634088BA07E65797408997A3B83C5BC39`) and
two actual Win16 tasks. The sender's terminal phase records `destroyed=1` with
zero errors after the original normal task exit, while the receiver separately
reaches its terminal phase with zero errors and continues through the
construction/conversion checks. The current same/cross worker matrix at
`t449-s2-message-cross-r8` also completes each normal retirement path across
Off/Win16/All styles; this is evidence for the selected ordinary exit chain,
not a substitute targeted task-deletion implementation.

## Findings and bounded next evidence

No currently selected normal-exit source path is bypassed by a new autonomous
task terminator. The two-task sibling and cross-worker normal-retirement
witnesses now exist above. Native callback return after terminal cleanup and
all selected cleanup failure receipts remain finite later verification rows;
they are not a reason to add a project-owned terminator. The original early
hung-notification event is **not** a task-completion receipt and will not be
used as one in S2 or the later S3 targeted-operation work.
