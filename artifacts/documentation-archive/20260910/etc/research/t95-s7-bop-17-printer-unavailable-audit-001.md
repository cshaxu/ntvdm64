# T95 S7 BOP 17 Printer-Unavailable Audit 001

## Reached identity

r18 records `C4 C4 17` at `CS:EIP=0203:0000`, after the proven C8 transaction.
The OpenNT SoftPC BIOS table in `softpc.new/base/bios/bios.c:164` maps BOP 17
to `printer_io`; `inc/bios.h:45` identifies it as BIOS printer I/O (INT 17h).
This is neither a DOS/DEM file request nor a mouse operation.

## Original no-device semantics

`softpc.new/base/comms/printer_.c:101` defines `printer_io`. All of its BIOS
port selection, polling, output and AH-status mutation are inside `#ifdef
PRINTER`. With that feature not compiled, the function has no state action and
returns to the original BOP dispatcher. The contained Bochs profile does not
admit a printer device, port model, host printer, spooler, UI or filesystem.

## Admitted adapter result

For exactly real-mode `C4 C4 17`, return `RIP+3` with an empty fixed-width CPU
delta and no FLAGS, guest-memory, port, interrupt, host or device action. This
is the source-equivalent compiled-out `PRINTER` branch, not an emulation of a
printer success/status result. Every other selector, mode, malformed window or
overflow remains pass-through.

The next trace may establish only whether this lets the guest reach another
identity; it cannot claim printer support.
