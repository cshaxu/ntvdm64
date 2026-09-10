# M0 T346 — SoftPC graphics presentation closure

T346 closes the bounded source-selected presentation package.

Completed scope:

- The original SoftPC text and graphics-controller source inventory,
  including the explicit exclusion of `X86GFX`, Console Server, CSRSS and
  hardware/fullscreen replacement routes.
- A bounded session-owned text/graphics surface; original source retains the
  source-facing buffers and update ordering, while app receives snapshots.
- An app-owned public Win32 presentation window, keyboard forwarding and
  session-scoped Alt+Enter transition.
- Local lifecycle and noninterference evidence confirming that the window
  leaves selected SoftPC backend selection and session activation unchanged.

T346 does **not** claim that a DOS graphics program executed. The one fixed
CPU40/x86 container observation reaches only the earlier `54:05` ingress and
then its bounded timeout. Source review shows that the durable guest-address
aliases registered by original `cmdSetInfo` form a COMMAND/DEM/SoftPC lifecycle
cohort; they are neither a presentation leaf nor evidence that `54:05` itself
failed. The successor is M0 T347, which recovers that cohort through the
existing session mapping manager before any graphics-workload continuation.

Evidence:

- `docs/etc/evidence/m0-t346-s4-fixed-presentation-observation-001.md`
- `docs/etc/evidence/m0-t346-s4-p2-presentation-noninterference-fixture-001.md`
- `docs/etc/evidence/m0-t346-s4-p3-command-scalar-alias-owner-transfer-001.md`
- `docs/etc/operations/m0-t346-softpc-graphics-presentation-plan-001.md`
