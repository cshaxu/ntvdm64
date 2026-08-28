# M0 T309 S2 P3 — original error-source boundary audit

## Question

Does the whole original `softpc.new/host/src/nt_error.c` translation unit
compose through the current modern-host source boundary; if not, what exact
original boundary blocks it?

## Inputs and procedure

The unchanged original file was selected by the residual-owner ledger because
it defines `host_direct_access_error`, `RcErrorDialogBox`, and `RcMessageBox`.
The formal T309 Ninja graph compiled it independently on x86 and x64.  The
tracked byte-exact OpenNT `conapi.h` declaration carrier was imported into
`opennt-abi` after both approved OpenNT editions produced the same SHA-256.

## Observations

- Before the declaration import, both architectures stopped at the missing
  private `conapi.h` textual include.
- `conapi.h` is declarations only, but its API set proves the original unit is
  not merely an error-string helper: it also owns historic Console registration,
  display, full-screen, VDM input, and private control shapes.
- With the byte-exact header in the current modern SDK include environment,
  its old `CONSOLE_FONT_INFO` and `CONSOLE_READCONSOLE_CONTROL` layouts collide
  with modern SDK definitions.  The historical Console declarations therefore
  require a same-shaped `adapter-mvdm-host-out/win32` facade; the original
  header remains source evidence and must not be edited.
- The same x86/x64 compiler pass also reaches `CpuH -> CpuInt_c.h` through
  `cpu4.h`.  This is the original SoftPC CPU executor interface, not a missing
  public Win32 declaration.
- No function body was changed and no source-derived dialog or error handler
  was substituted.

## Disposition

The original error unit remains selected as a whole source candidate, but it
cannot enter the present Bochs-only T309 provider graph.  Its normal error
and direct-access functions must remain source-shaped and be revisited with
two named prerequisites: `adapter-mvdm-host-out/win32` owns a same-shaped
modern Console declaration/binding facade, while the selectable original-
SoftPC backend owns the `CpuH` form.  `session` owns the typed controlled-stop
binding.  Its WOW hard-error and historical Console product branches remain
explicit later owner decisions.

## Confidence and follow-up

High confidence.  The formal x86/x64 logs in
`build/M0-T309/S2/cohorts/<arch>/error-source-audit-machine.log` each record
the same declaration collision and `CpuInt_c.h` boundary.  The next P3 group
is the original event/host/LPT cohort, not a local rewrite of this error
provider.
