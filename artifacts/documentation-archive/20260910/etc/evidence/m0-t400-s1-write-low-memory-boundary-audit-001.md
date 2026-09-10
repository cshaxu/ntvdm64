# M0 T400 S1 — WRITE low-memory dialog boundary audit

## Question

What original source boundary produces the current diagnostic-free x86 WRITE
dialog, and does its text identify `FInitMemory()` as the unique failed
operation?

## Source path

The selected immutable WRITE source is explicit:

```text
mmw.c:WinMain
  -> initwin.c:FInitWinInfo
  -> CreateWindow(parent)
  -> WM_CREATE
  -> initwin.c:MmwCreate
  -> [one or more original initialization branches]
  -> initwin.c:InzFailed
  -> diaalert.c:Error(IDPMTCantRunM)
```

`diaalert.c` resolves `IDPMTCantRunM` from the preloaded `hszCantRunM` message
carrier. This is the observed “Not enough memory for Write to complete this
operation...” dialog. It is a WRITE guest dialog, not T398's host
`ERRORPANEL` and not an app presentation result.

## Failure-set ledger

`FInitMemory()` is only one `MmwCreate` predecessor. Its direct false returns
are the selected `FCreateRgbp`, reserved `LocalAlloc`, FCB `HAllocate`,
`FSetScreenConstants`, `FInitDocs`, `FInitProps`, `FInitFiles`, and emergency
save-space `HAllocate` paths. Its `FALSE` causes `MmwCreate` to set
`fMessageInzFailed`, which routes `FInitWinInfo` to `InzFailed` after the
parent `CreateWindow` returns.

However, the same `InzFailed → IDPMTCantRunM` result is also reached by:

1. resource/cursor/accelerator/class setup before parent creation;
2. every later `MmwCreate` child-control, page-info DC, brush or menu failure;
3. post-parent `InitCommDlg`, `ObjInit`, `FInitArgs`, and `ValidateMemoryDC`
   failure; and
4. `fMessageInzFailed` raised by either the parent or child window procedure.

The early first `LoadCursor(IDC_WAIT)` return is a separate no-dialog false
return. Disk flags at `InzFailed` select `IDPMTCantRunF`, so the observed
`IDPMTCantRunM` excludes only that terminal disk branch; it does not identify
any one member of the remaining failure set.

## Comparison with current runtime evidence

The valid short-root, diagnostic-free x86 observation shows WOW32 loaded,
`MSWRITE_MENU`, and WRITE's visible dialog. It proves the source result family
above. It does not carry a guest return value, original branch identity or a
module-qualified live selector. Earlier CCPU decoder-side file/event/
instruction transports and selector/IP inference are already rejected in T397
because they alter the ordinary route or lack a stable module identity.

Consequently, the prior shorthand “WRITE reached FInitMemory” is not an
admissible causal conclusion. The correct current boundary is the finite
`FInitWinInfo` initialization failure set listed above.

## Disposition

S1 selects no repair. A successor may only use a non-invasive,
module-qualified observation that discriminates an original failure result
without guest mutation, decoder-side transport, guessed selector mapping,
private USER/CSRSS replacement, synthetic memory, or x64 work. Until such a
mechanism is admitted and proves one exact original branch, no local-heap,
GDI, common-dialog, DPMI or CPU repair is justified.
