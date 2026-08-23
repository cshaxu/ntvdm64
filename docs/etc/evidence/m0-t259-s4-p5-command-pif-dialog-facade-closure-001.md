# M0 T259 S4 P5 — COMMAND/PIF Shared Dialog Facade Closure

Status: completed S4 final reconciliation part

## Finding

The reverse completion audit found that directly imported OpenNT
`softpc.new/host/src/nt_pif.c` reached `RcMessageBox` through
`bx_ntvdm_command_pif_parser_message_box`, but that wrapper discarded all
arguments and returned a fixed Ignore result.  The same current composition
already had the source-shaped shared `RcMessageBox` facade used by COMMAND
and DEM callers.  A parser-local policy was therefore both unnecessary and
incorrect.

The audit also found an interface-value drift in
`command_misc_shim.h`: historical OpenNT `error.h` defines `RMB_ABORT` as
`1`, while the local header said `2`.  That changed the option bit visible to
the imported `cmdenv.c` and `cmdpif.c` callers.

## Correction

- `command_pif_shim.h` now publishes the complete same-named historical
  `RMB_ABORT/RETRY/IGNORE` and icon constants for the directly imported PIF
  parser.
- `bx_ntvdm_command_pif_parser_message_box` delegates to the single shared
  `bx_ntvdm_opennt_rc_message_box` facade.  Thus original allocation retry
  ordering and result categories are preserved; `RMB_EDIT` remains the
  documented explicit Abort because editable PIF UI is a separate owner.
- `command_misc_shim.h:RMB_ABORT` is restored to its original value `1`.
  No per-caller translation or second dialog implementation was introduced.

This is source-first recovery at the smallest possible boundary: the imported
`nt_pif.c`, COMMAND mirrors and Bochs layers remain unmodified.

## Verification

Fresh external graph: `build/M0-T259-S4/formal-r17/`.

- Full Ninja graph completed 323 build/link actions and produced all 79
  manifest-declared executables (78 fixtures plus `ntdos64-native`).
- `t234-s2-command-pif-direct-import-fixture.exe` passes, including a
  fixture-suppressed call through the PIF parser's same-named dialog wrapper,
  original PIF title/command/directory/target parsing, and CONFIG/AUTOEXEC
  selection.
- `t231-s2-command-misc-direct-import-fixture.exe` passes the existing
  COMMAND dialog/result ABI check with the corrected shared value.
- `t230-s8-demerror-lock-direct-import-fixture.exe` passes, guarding the
  shared DEM hard-error dialog/NTDLL facade.

## Residual disposition

This closes the directly recoverable PIF dialog seam.  `RMB_EDIT` remains an
explicit source-shaped terminal behavior until the PIF/console owner can
safely provide editable OEM input.  `demmisc.c` long-lived/debug
`Sim32GetVDMPointer` uses remain recorded mapping-manager migration debt; they
are not silently treated as a successful durable or protected pointer facade.
