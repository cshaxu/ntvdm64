# M0 T245 S6 — Printer/LPT owner-package source and ABI map

## Entry And Immediate Guest Contract

The source-built run after the completed C8 handoff reaches `C4 C4 17` at
`0211:02D5` with `SI=1`, `DX=0`.  OpenNT
`base/mvdm/softpc.new/base/bios/bios.c` maps selector 17 to `printer_io` in
`base/comms/printer_.c`.

The concrete guest caller is `dos/v86/doskrnl/bios/mschar.asm:prn_open`:
unless the EXEC bit is set, it sets `SI=PRNIO_OPEN` and `DX` to the selected
zero-based printer, then invokes `%BIOS_PRINTER_IO`.  `printer_io` dispatches
the NTVDM subfunction before any port access:

| SI | Original body | Required state/effect | Current disposition |
| --- | --- | --- | --- |
| 0 | `printer_bop_flush` | Reads monitor 16-bit print buffer, sends it to host, returns `AH=90` or `08`. | Defer: guest buffer plus host output lifecycle. |
| 1 | `printer_bop_openclose(1)` | `DX % 3`; `host_lpt_dos_open(adapter)` sets the per-LPT `dos_opened` flag. | First recoverable session-state subpackage. |
| 2 | `printer_bop_openclose(2)` | If active, close host LPT handle; clear `dos_opened`. | Pair with SI=1, with inactive/no-host-device branch first. |
| other / ordinary BIOS | `AH=0/1/2` write/init/status | Reads BDA port/timeout, uses in/out LPT ports, and returns status in AH. | Defer: actual LPT port device and host printer service. |

## Original Ownership Graph

```
mschar.asm prn_open / prn_close
    -> BOP 17 (SI 1 / 2, DX adapter)
        -> printer_.c printer_io -> printer_bop_openclose
            -> nt_lpt.c host_lpt_dos_open / host_lpt_dos_close
                -> HOST_LPT { active, dos_opened, HANDLE, buffer, status }

ordinary INT 17h BIOS route
    -> printer_.c printer_io (AH 0 / 1 / 2)
        -> BDA LPT addresses + mantle port-space
        -> printer.c / nt_lpt.c host_print_* / CreateFile / WriteFile
```

`printer.c:printer_setup_table` and `nt_lpt.c:host_printer_setup_table` also
depend on the historical MONITOR printer-information table.  They are neither
needed nor safe to infer from the observed SI=1 path.

## Composition Decision

The smallest viable first recovery is a source-shaped mirror of
`printer_io`'s SI=1/2 switch and `printer_bop_openclose`, using a bx-vdm
session-owned three-entry LPT state shim.  It has no host handle, print job,
port operation or Bochs object.  It preserves the original modulo-three
adapter selection and source ordering.  With no active host LPT endpoint,
the original close branch has no external action and only clears state.

This does not manufacture printer success: SI=0 and every ordinary BIOS
subfunction remain declined until the whole corresponding owner subpackage is
available.  A later host-integrated printer package may use public Win32
`CreateFile`/`WriteFile` under explicit device policy; it must not expose that
handle to guest code and must recover the original buffer/status lifecycle.

## Required Boundaries

- `bx-vdm` owns selector recognition, copied SI/DX inputs, state and typed
  resume.
- `bx-mantle` remains selector-blind; a future LPT port device can be added
  only as a pure machine mechanism, with the printer policy still in bx-vdm.
- The existing host-handle manager is relevant only after a host printer
  endpoint is admitted.  It is deliberately unnecessary for the open/close
  state pair.

## Recommendation

Admit the SI=1/2 pair as M0 T245 S7, with positive open/close and adapter
modulo tests plus negative SI=0/AH and protected-mode tests.  Do not attach a
host printer or an LPT device in that packet.  The outstanding flush,
buffered/direct host printer and BIOS port subpackages remain separate tracker
rows under `BOP-DEPENDENCY-109`.
