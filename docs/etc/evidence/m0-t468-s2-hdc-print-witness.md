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

## Device-capability endpoint disposition

The selected original entry is GDI ordinal 454.  Its existing
`WG32DeviceCapabilities` body in `wgprnset.c` owns the 16:16 argument
conversion and then lazily loads `WINSPOOL.DRV`; it is not correct to replace
that route with a native-only provider probe or a new device manager.  A
disposable Win16 witness was constructed for `DC_VERSION` against the same
named PDF provider.  It corrected its NE data-segment offsets and uses the
same `USER.InitApp` import as the passing HDC witness.  It also stages the
immutable `WINSPOOL.EXE` facade under the observer-selected test system root,
which is required for the original loader's normal redirect path.

The resulting owned run `build/t468-s2/devicecaps-r10` did not return from
the original provider-load/call boundary in the observer's 30-second
observation budget and wrote no guest record.  It is therefore an explicit
**unavailable / not-accepted endpoint**, not evidence that g454 conversion
works or fails.  The untracked witness sources were removed rather than
committed as a misleading regression test.  No guest, profile, printer,
provider, production source, or runtime policy was changed.  This leaves the
device-capability normal/failure endpoint for the later S3 failure-and-
retirement package, where a reliably observable selected provider can be
admitted without widening the original owner boundary.

## PrintDlg default-data witness

`tests/observation/win16_printdlg_default_probe.asm` calls original
`COMMDLG.PrintDlg` ordinal 20 with the documented `PD_RETURNDEFAULT` contract:
the incoming `hDevMode` and `hDevNames` handles are null, so the original
`WCD32PrintDlg` path obtains and converts default data without presenting a
dialog.  The probe records the guest boolean and original extended error after
the call.  This is intentionally a real 16-bit `PRINTDLGDATA16` invocation,
not a native `comdlg32` substitute.

The owned private-desktop result
`build/t468-s2/printdlg-default-r1` reports normal observer exit and:

```text
PASS original Win16 PrintDlg PD_RETURNDEFAULT witness
result=1; extendedError=0
```

This proves the selected default-data conversion and return edge only.  It
does not claim interactive print-dialog cancellation, hook reentrancy, or
returned-DC retirement; the selected returned-object ownership is recorded
separately in the [S3 retirement disposition](m0-t468-s3-printing-retirement.md).

## PrintDlg hook-cancellation witness

`tests/observation/win16_printdlg_cancel_probe.asm` covers the distinct
interactive cancellation contract without UI automation: it supplies a real
16-bit `PD_ENABLEPRINTHOOK` callback through original
`KERNEL.MakeProcInstance`.  On the original `WM_INITDIALOG` callback, the
hook invokes original `USER.EndDialog(hwnd, FALSE)`.  This is the common
dialog's documented cancellation route, not a synthetic host cancellation.
After `COMMDLG@20` returns, the probe reads original `COMMDLG@26` extended
error and retires the callback with original `FreeProcInstance`.

The owned private-desktop run `build/t468-s2/printdlg-cancel-r1` exited
normally and recorded:

```text
PASS original Win16 PrintDlg hook cancellation and ProcInstance retirement witness
result=0; extendedError=0; hooks=1; freed=1
```

Thus the selected cancellation boundary proves callback entry, the normal
cancel return (not a transport failure), zero extended error, and balanced
guest callback carrier retirement.  It does not claim a returned printer DC,
which cancellation does not produce.
