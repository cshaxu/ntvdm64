# S11 CPU monitor: printer fast-path boundary

The NT4 `instemul.asm` `OpcodeINB` and `OpcodeOUTB` first call
`VdmPrinterStatus` and `VdmPrinterWriteData` for the standard printer
status/data ports on a PC-compatible machine; unsuccessful calls fall back
to `Ki386VdmDispatchIo`. The kernel implementations in `vdmprint.c` consult
the bound VDM TIB printer mode. In direct-I/O mode the original write path
flushes its 16-byte TIB buffer at `PRT_DATA_BUFFER_SIZE`; the original direct
status path flushes that buffer before issuing its status IOCTL. These are
observable mode-dependent semantics, not simply an alternative port number.

The selected CCPU `nt_inthk.c` instead calls `IN8`/`OUT8`, which reach the
SoftPC IOS tables. `base/comms/printer.c` registers `printer_inb` and
`printer_outb`; its dongle path can request direct access. The actual
`obj/host/nt_lpt.obj` production command includes both `CCPU` and `MONITOR`.
`host_lpt_open` requires a successfully opened `FILE_TYPE_CHAR` object before
enabling direct access. `host_print_byte` then calls the project-selected
`MonitorPrinterWriteData`; that routine appends to the same 16-byte TIB array
without a capacity test, while `NtVdmControl` currently implements only
`VdmQueryDir`. The host `flushBuffer` operates on `HOST_LPT.kBuffer`, not
that TIB array. This source chain warrants a direct-access boundary witness;
it is not evidence that an installed printer endpoint is available, that the
dongle state was reached, or that a product failure has occurred.

S11 therefore must not treat generic scalar-port fixture success as proof of
NT4 printer fast-path parity. The printer/device owner is already assigned
to S14 in the approved stage plan: test an actual selected provider with a
controlled character-device boundary, both simulated and direct modes, the
16-byte threshold, status-triggered flush, failure, close and any VDD port
hook. Compare original operation ordering and state before choosing a small
implementation. Do not blindly enable the kernel branch, fake a successful
flush, assume the host buffer is the TIB buffer, or repair unrelated device
code solely from this source finding.
