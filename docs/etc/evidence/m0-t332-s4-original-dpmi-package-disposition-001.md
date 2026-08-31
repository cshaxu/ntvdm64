# M0 T332 S4 — Original DPMI/DPMI32 package disposition

## Closure result

T332 closes the DPMI/DPMI32 **source and product-route** package.  All
twenty-five original `DpmiDispatchTable` entries have a unique disposition in
the S1 ledger, and every selected original provider body is in the x86 and
x64 final product link.  No source-derived DPMI dispatcher, CPU executor,
guest DOSX host library or private mapping manager was added.

## Runtime boundaries retained exactly

- `53:02` protected interrupt registration reaches the original
  `NtVdmControl(VdmSetInt21Handler, ...)` contract.  Modern user mode has no
  corresponding kernel VDM service; the current monitor adapter returns
  `STATUS_NOT_IMPLEMENTED`, rather than reporting success.
- `53:03` fast BOP and `53:0E` debug-register handling belong to the original
  kernel-VDM/V86 `i386_SOURCES` path.  They remain mirror/evidence-only until
  a monitor/debugger owner package can provide an equivalent source-shaped
  contract.  They are not substituted by a CPU40 shortcut.
- `53:12` retains the original VDD/VCD boundary and waits for the VDD package.
- Stack, interrupt/fault IRET and DOSX transfer code is selected original
  source, but there is no observation that the fixed startup container reaches
  a complete protected-mode/DOSX route.  The existing startup observation
  stops earlier in the CPU40/worker path, so no new trace-selected repair is
  admitted here.

## Formal verification

The x86 and x64 `original-softpc-process.exe` targets were rebuilt in an
explicit MSVC developer environment.  Both passed final non-`/FORCE` linking
with `original-mvdm-dpmi32.lib` as a direct input.  This verifies build and
link closure, not DPMI runtime completion.

## Successor ownership

The next ordered queue package is Redirector.  DPMI's named deferred edges
remain with their actual owners: monitor/debugger for kernel-V86 control,
VDD for VCD service, and the later cross-family integration package for any
guest route that reaches the already-selected original code.  None requires a
new DPMI-only shim.
