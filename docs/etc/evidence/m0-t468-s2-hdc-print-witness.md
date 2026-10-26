# T468 S2 — real Win16 HDC document witness

This is the first bounded S2 acceptance result.  It does not close raw-spool,
device-setup or PrintDlg work, and it changes no production image or guest
input.

## Witness

`tests/observation/verify-win16-print-probe.ps1` is the restored narrow
successor to the former style-coupled generic hook runner.  It has no style
parameter or UI automation.  It compiles the independently authored
`win16_print_probe.asm`, stages a complete physical copy of the existing
published package, verifies the immutable `WINSPOOL.EXE` identity, and uses
the existing private-desktop observer.  The probe explicitly creates an HDC
for **Microsoft Print to PDF**; it never selects a default or physical printer.

It uses original GDI `SetAbortProc` (381), `StartDoc` (377), `StartPage`
(379), `EndPage` (380), `EndDoc` (378), `DeleteDC` (68), and KERNEL
`MakeProcInstance`/`FreeProcInstance`.  The runner requires the original
`RET_SETABORTPROC` callback trace to return on the task that installed it,
two DC deletions, one ProcInstance free, a normal PDF, and no cancelled PDF.
Those checks prove the HDC document route uses the existing W07/W08 DC owner
and original callback/return machinery; they do not manufacture a spool job
manager.

## Executed result

The owned private-desktop run `build/t468-s2/print-hdc-r1` completed on
2026-10-26.  Its observer reports `result=exited`, `exit=0x00000000`; the
guest report has four `PRN1` records and the runner recorded:

```text
PASS real Win16 original HDC print/abort normal-cancel cleanup witness
```

The normal output is a 1,322-byte `%PDF-` document.  Cancellation produced no
`cancel.pdf`.  This establishes only the selected HDC normal/cancel and
callback/DC cleanup path on the named provider.

## Raw spool boundary

The selected raw `OPENJOB` body begins by querying the legacy `[devices]`
profile mapping through `GetDriverName`.  The current host has no such mapping
in its `win.ini`; the inspected source therefore cannot reach `OpenPrinterA`
without adding host configuration or inventing a mapping.  S2 will record that
as a real endpoint non-pass and may add a bounded negative witness.  It will
not alter global profile state, select an unrelated printer by name, or
substitute `OpenJob` with the HDC route.

The local service inventory does contain four normal providers, including
Microsoft Print to PDF, but that modern provider listing is not the legacy
profile mapping required by the original raw-spool body.  The distinction is
intentional: provider availability does not authorize a new compatibility
policy.

`tests/observation/win16_raw_spool_probe.asm` and
`verify-win16-raw-spool-profile.ps1` now verify that boundary through the
actual immutable guest thunk rather than profile-file inspection alone.  The
probe invokes original GDI `OpenJob` (240) with `FILE:` and writes its exact
16-bit result.  The owned isolated run
`build/t468-s2/raw-spool-profile-r3` completed normally and recorded:

```text
PASS raw OpenJob reaches original explicit no-profile failure without host configuration
openJob=0; closeJob=0
```

The probe deliberately calls `CloseJob` only on an unexpected nonzero return,
so this evidence neither leaks nor synthesizes a native job.  Its first two
attempts are retained only as test-input construction failures: r1/r2 had an
incomplete NE module-reference table and therefore did not write a guest
record; r3 uses the same three-module table shape as the retained working
Win16 probe and is the only result used here.
