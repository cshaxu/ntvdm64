# M0 T395 S1 — DPMI32 runtime admission audit

Date: 2026-09-06

## Cohort correction

The prior `16-body` wording mixed two incompatible source profiles. The
CCPU40 formal provider has 15 C bodies: the thirteen portable entries under
`src/mvdm-host/dpmi32/sources`, plus `dpmimemr.c` and `dpmimscr.c` selected by
the formal product profile. `i386/dpmi386.c`, `i386/dpmimem.c`, and
`i386/dpmimisc.c` are NT4 kernel-VDM bodies, not CPU40 software-emulation
providers, and remain excluded by source policy.

`MS_bop_3` in `softpc.new/host/src/nt_bop.c` calls the original
`DpmiDispatch`; its original `dpmi32.c` dispatch table owns subfunctions
`53:00` through `53:18`.

## Formal build evidence

The formal x86 graph was regenerated with:

```text
New-T310OriginalSoftpcNinja.ps1 -Architecture x86 \
  -BuildRoot build/M0-T395/S1/formal-x86
```

The fresh graph compiled all 414 C object targets, including the 15 DPMI32
objects. `original-mvdm-dpmi32.lib` was rebuilt (79,600 bytes), and the final
`original-softpc-process.exe` linked successfully (3,533,824 bytes).

The generator now creates every `/Fo` parent directory in its emitted graph;
this corrected a fresh-build infrastructure omission and did not alter a
source-provider body. The DPMI dispatch ingress now makes a default-off
observation record immediately after the original decoded service index and
IP advance, before table validation/call. It neither reads new guest memory
nor changes the table, service result, or guest ABI.

## Immutable bootstrap stage

The runtime stage records exact existing media bytes for `system.ini` and
`system32/KRNL386.EXE`, both required by the already documented DOSX/WOW
bootstrap. They are load-only staged media, never host link inputs and were
not modified.

## Runtime observations

Using the freshly compiled console-owning observer and the fresh x86 product:

| Declared workload | Result | DPMI conclusion |
| --- | --- | --- |
| `system32\\DOSX.EXE` | 10-second startup timeout, no console text or BOP report | Did not reach observed `53h`; not proof of DPMI failure. |
| `COMMAND.COM /C system32\\DOSX.EXE` | 30-second startup timeout, no console text or BOP report | Same pre-dispatch startup wait. |
| no declared command with scripted `ver`/`exit` | process exits `69` (`APP_STARTUP_COMMAND_REJECTED`), no guest console-ready marker | This is the app's no-command boundary, not a guest/DPMI result. |

The selected DOSX workload therefore has a complete static CPU40 DPMI32
provider closure, but no runtime `53h` reach or return proof. S1 remains
active: the next admissible work is to identify the immediate original
pre-dispatch startup owner from this fixed workload; no WOW32 body, DPMI
kernel-VDM body, synthetic BOP, or guest-media mutation is admitted by this
audit.
