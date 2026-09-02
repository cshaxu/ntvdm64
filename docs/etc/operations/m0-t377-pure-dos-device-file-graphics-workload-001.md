# M0 T377 — Pure DOS device, file and graphics workload closure

## Purpose

Extend T375/T376's proved original COMMAND and external-DOS baseline into a
small immutable original DOS workload matrix covering the next complete
keyboard/console, file/seek/IOCTL, timer/IRQ and video path. Recovery remains
package-first: each workload identifies an immediate original owner cohort; it
does not select an isolated BOP, port, interrupt, or compatibility patch.

## Boundary

- CPU40/SoftPC is the only machine backend.  CPU30, Bochs, x64 guest runtime,
  guest rewriting, a host executable loader, and synthetic device/guest input
  are excluded.
- Original DOS/SoftPC code and selected immutable media remain the behavioral
  owners.  New mechanics may exist only in the smallest named adapter after
  the source-recovery ladder and mapping-manager review.
- The package addresses only original keyboard/console, file/seek/IOCTL,
  timer/IRQ, or video paths directly reached by the selected workload matrix.
  Native child, Redirector, WOW/Win16 and arbitrary graphics compatibility are
  excluded.

## Ordered subtasks

1. **S1 — Whole workload matrix and immediate-owner admission.** Inventory
   eligible original staged DOS media and its source, identify the complete
   guest and SoftPC/host call chain for each candidate, select the smallest
   source-complete workload matrix covering every stated path, and freeze the
   source/media/expected marker contracts.  No source change or runtime run.
2. **S2 — Console, keyboard and file/IOCTL owner recovery.** Recover or bind
   the complete original owner cohorts selected for console/keyboard and
   file/seek/IOCTL. Audit pointer/HANDLE boundaries against the session mapping
   manager and preserve original error/cleanup order.
3. **S3 — Timer/IRQ and video owner recovery.** Recover or bind the complete
   original SoftPC owner cohorts selected for timer/IRQ and video. Do not
   replace a controller, interrupt, or guest vector with a project-defined
   behavior.
4. **S4 — Formal matrix closure.** Add all selected original cohorts to the
   formal CPU40/x86 graph, run focused positive and source-shaped negative
   tests, and verify the final product link. No integration run.
5. **S5 — Frozen original workload-matrix observation.** Add only selected
   byte-identical media through the mismatch-refusing stage operation, then
   make the predeclared bounded console-owning observations in the fixed root.
   Record reached device/file/video markers, original returns, or exact
   earliest owner boundaries.

## Completion standard

T377 closes only when the selected workload matrix has original source/media
identity, complete immediate owner maps, formal source/binding closure, and
the predeclared fixed-container observations. A declared unavailable result may close
only when it is the selected original result and every smaller original route
has been exhausted.  It is not a general device, graphics, or DOS
compatibility claim.
