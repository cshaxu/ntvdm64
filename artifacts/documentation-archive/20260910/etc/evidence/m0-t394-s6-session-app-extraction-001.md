# M0 T394 S6 — session/app direct-boundary extraction

Date: 2026-09-06

## Result

S6 is closed.  `session_termination.{h,c}` now owns the existing per-thread
origin and the conversion of a selected VDM terminal path to
`session_terminate_current()`.  Its report text, optional environment sink,
completion code and `vdm_for_wow` call shape are unchanged.

`app/report_configuration.{h,c}` now captures and removes
`MVDM_COMMAND_CONTINUATION_REPORT_PATH` and `MVDM_STREAM_IO_REPORT_PATH`
before original startup.  It retains bounded app-private copies and exposes
read-only queries to the still-unmoved observers.  The old SoftPC termination
unit has neither termination state/body nor environment capture state/body.

## Exclusions

No CPU, keyboard, PIC, SAS, COMMAND, DEM, BOP, exception, or main-return
observer moved.  No guest environment, guest state, BOP, media, or execution
semantic was changed.

## Verification

- `rg` found no remaining `mvdm_softpc_terminate_current_session`,
  `mvdm_softpc_set_termination_origin`, or capture API consumer.
- Direct MSVC compilation of `session_termination.c`,
  `report_configuration.c`, and the remaining observation unit passed for
  x86 and x64.
- New `t394_s6_report_configuration_fixture` passed as x86 and x64: both
  variables were removed from the inherited process environment and both
  retained values matched their read-only queries.
- The x64 T310 formal graph generated with both new sources.  Its full Ninja
  invocation could not start `cl.exe` outside a developer environment, then
  stalled after being re-run in one; this is an environment runner condition,
  not claimed as a formal build pass.
