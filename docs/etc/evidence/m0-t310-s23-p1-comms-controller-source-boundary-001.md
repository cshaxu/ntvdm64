# M0 T310 S23 P1 — communications-controller source boundary

## Question

Which original communications-controller contracts belong to the selected
Win32/x86 CCPU40 SoftPC profile, and which host endpoint contracts must remain
with the subsequent host-communications packet rather than becoming a new
controller implementation?

## Inputs and procedure

Read the selected mirror forms of `softpc.new/base/comms/com.c`,
`rs232_io.c`, `printer.c`, `printer_.c` and `prt_scrn.c`; then read the
corresponding host provider forms `softpc.new/host/src/nt_com.c`, `nt_wcom.c`
and `nt_lpt.c`, their reached BIOS reset callers, and the generated formal
Ninja configuration.  The inspection covers declarations, definitions,
controller/table use, selected callers, host-boundary calls and conditional
compilation.  It does not open a serial or parallel host device.

## Observations

### Original controller and BIOS contracts

- `base/comms/com.c` is the original serial-controller owner.  It defines the
  controller state and port callbacks, including `com_init`, `com_post` and
  `com_int_data`; its calls to `host_com_*` are a host-endpoint boundary, not
  a reason to replace the UART algorithm.
- `base/comms/rs232_io.c` is the original INT 14-style BIOS front end.  The
  selected BIOS reset code installs the RS232 vector at `INT 14h`, and
  `base/bios/reset.c` calls `com_init` followed by `com_post` for every
  configured serial adapter.  This preserves the source-defined reset order.
- `host/src/nt_com.c` contains the corresponding original host endpoint
  operations: initialization, open/close, read/write, ioctl, locking,
  heartbeat and EOI/poll handling.  Its `host_com_ioctl(int, int, intptr_t)`
  host-local argument is native-width inside the one process and does not
  cross a fixed-width guest ABI.  Actual endpoint policy and public Win32
  binding belong to S24.
- `base/comms/printer.c`, `printer_.c` and `prt_scrn.c` retain the original
  parallel/printer/screen-print source.  The same selected CCPU40 Ninja flags
  include `CCPU`, `CPU_40_STYLE`, `CPU_30_STYLE` as a source compatibility
  carrier, and `PROD`, but include neither `PRINTER` nor `SLAVEPC`.
  Consequently printer initialization and host printer bodies are original
  condition-unselected paths in this profile.  `base/bios/reset.c` retains
  the original zero-printer equipment count and omits `printer_init` /
  `printer_post` when `PRINTER` is absent.
- `host/src/nt_lpt.c` is the original host printer endpoint provider.  It
  includes real handle, direct-access and MONITOR-specific forms behind its
  original conditions.  Because `PRINTER` is not selected, S23 neither opens
  a printer nor substitutes synthetic status.  Any admitted public host
  printer/parallel endpoint remains S24 work.

### Boundary and identity disposition

No S23 controller table carries an opaque host identity across a guest or
component ABI.  Controller registers, serial buffers, IRQ state and BIOS
scalars remain original local controller state.  A future active endpoint may
own native handles only inside its host-binding provider; if it publishes an
identity across a historical fixed-width boundary, S24 must use the existing
session host-resource mapping-manager instance.  S23 creates no alternate
mapping table and no Bochs, MONITOR, kernel-VDM or `src.old` route.

## P1 disposition

| ID | Original contract | S23 disposition | Later owner |
| --- | --- | --- | --- |
| S23-001 | `com.c` UART controller/table/callback body | direct original controller composition | S23 P2 formal assembly |
| S23-002 | `rs232_io.c` INT 14 front end and BIOS reset ordering | direct original controller composition | S23 P2 formal assembly |
| S23-003 | `host_com_*` host endpoint operations | binding-only boundary; no endpoint is activated | S24 |
| S23-004 | printer, parallel and screen-print controller sources | exact original `PRINTER`-unselected unavailable disposition | S23 P2 / S24 if profile changes |
| S23-005 | `host_lpt_*` / `host_print_*` host endpoint operations | binding-only boundary; no synthetic printer result | S24 |

## Interpretation, confidence and follow-up

Confidence is high: source definitions, reset callers and the selected formal
configuration agree on the controller-versus-endpoint split.  S23 P2 must
create a fresh formal x86 CCPU40 build root, verify the original communications
archive and forced-link ownership for this cluster, and retain the exact
`PRINTER`-unselected result as focused negative evidence.  It must not turn a
serial or printer host device into an S23 implementation.
