# M0 T400 S2 — passive WRITE window-topology discrimination

## Run

The existing Console-owner observer ran the unchanged x86 product for 30
seconds at the valid seven-character `O:\t398` root:

```text
ObserveSoftpcStartup.mjs
  --launcher build/tools/console-startup-observer.exe
  --product build/M0-T398/S2/error-interaction-x86/original-softpc-process.exe
  --stage O:\t398
  --product-command system32\WRITE.EXE
  --without-diagnostics --observation-timeout-ms 30000
```

The report is retained outside the repository at
`O:\t398\t400-s2-write-topology.txt`. It records product SHA-256
`f1c3f81d793198d32ed685429e4509547a581fc0b3f87240b97296022f0a66a0`, the
fixed stage manifest identity, no child diagnostic environment, no scripted
input and no message posting.

## Observation

The ordinary result remains the visible `#32770` dialog titled `Write`, with
the original `IDPMTCantRunM` low-memory text and its three dialog children
(`OK` plus two statics). WOW32 is loaded.

The same passive process-wide census additionally records a hidden top-level
`MSWRITE_MENU` window with a native menu. It records **no** WRITE parent child
controls beyond the dialog's own three children. The observer enumerates
children of every launched-process top-level HWND, so this is not merely a
foreground-window sample.

## Source-order conclusion

`FInitWinInfo` has already created the `MSWRITE_MENU` parent. In `MmwCreate`,
the non-null native menu establishes that its first `GetMenu(hWnd)` guard has
passed. The next source operation is `FInitMemory()`. Only after a true
return does `MmwCreate` create horizontal/vertical scrollbars, the size box,
page-info child, and its DC/brush state. Since none of those parent children
exists while the modal error is live, their later failure branches are
excluded for this ordinary run.

The current source-and-runtime boundary is therefore narrowed from the S1
union to the direct false-return set **inside original `FInitMemory()`**:
`FCreateRgbp`, reserve `LocalAlloc`, FCB `HAllocate`, `FSetScreenConstants`,
`FInitDocs`, `FInitProps`, `FInitFiles`, or emergency-save `HAllocate`.
This is a branch-set reduction, not proof of which internal operation failed.

## Disposition

S2 selects no repair. A later audit must discriminate among these original
`FInitMemory` returns without guest mutation, CCPU decoder transport, selector
guessing, synthetic memory, input injection, private USER/CSRSS recreation or
x64 work.
