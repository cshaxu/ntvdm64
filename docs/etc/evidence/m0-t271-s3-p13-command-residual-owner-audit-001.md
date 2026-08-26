# M0 T271 S3 P13 — COMMAND residual owner audit

This audit revisits the remaining local blocks after P12 against the original
OpenNT `base/mvdm/dos/command/cmdexec.c` lifecycle.

- The detached worker callback remains in `opennt-bop/command`: it executes
  the imported `cmdCreateProcess` body, whose original ordering calls
  `GetNextVDMCommand`, COMMAND globals and source-shaped environment logic.
  Moving that callback into `opennt-host` would create an invalid host-to-BOP
  dependency.  `opennt-host` already owns the generic child start, process,
  wait, completion, cancellation and opaque-stream mechanics.
- `GetVDMAddr` remains source-specific COMMAND glue.  Its returned buffers
  have distinct `cmdExec`, `cmdGetNextCmd`, `cmdCheckBinary`, keyboard and
  configuration writeback/order semantics.  It cannot be represented as one
  selector-blind CCPU/SAS address conversion without losing original source
  ordering.  P9 retains the checked generic span boundary underneath it.
- `runtime_command_misc_redirection_from_guest` and handle publication are
  held for the Redirector owner package: they name `PREDIRCOMPLETE_INFO` and
  source `cmdredir.c` conventions.  A generic adapter must not invent that
  protocol.
- `GetWowKernelCmdLine` remains the explicitly registered WOW terminal; its
  original consumer is the absent WOWEXEC/WOW32/NE-loader composition.
- `TerminateVDM` retains only the source-required non-returning `longjmp`.
  P13 moves its selector-blind typed controlled-stop operation to
  `adapter-softpc/ccpu_frame_context`, leaving no direct CPU-result operation
  in the COMMAND terminal glue.

Formal Ninja relink and the focused CCPU frame-context fixture verify the new
controlled-stop operation alongside the existing register, pending and
bounded-load checks.  This is a disposition milestone, not S3 closure: the
remaining source-shaped blocks must be proven by their corresponding COMMAND,
Redirector and WOW owner-package acceptance work.
