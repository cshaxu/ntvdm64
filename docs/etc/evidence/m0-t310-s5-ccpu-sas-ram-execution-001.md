# M0 T310 S5 — original CCPU to SAS/RAM execution proof

## Objective

Strengthen the bounded original CCPU execution proof so that it establishes a
real original CPU → SAS → RAM effect before the existing original controlled
return.  This remains a mechanical execution proof, not a BIOS, BOP, device,
or runnable-product assertion.

## Exact guest program

The focused fixture seeds original SoftPC SAS at `FFFF:0000` with:

```text
B0 5A          mov al, 5Ah
A2 00 80       mov [DS:8000h], al
D6 FE          original CCPU BOP-FE unsimulate return
```

The fixture explicitly sets the original CCPU DS form to zero before entry.
After each return it requires:

- original `c_getIP() == FFF7h`; and
- original `c_sas_hw_at(00008000h) == 5Ah`.

The executor remains selected through original `nt_cprgs.c`, then entered
through unchanged `nt_cpu.c::host_start_cpu`.  The second pass enters through
unchanged original `nt_cpu.c::host_simulate`.

## Formal verification

Both formal MSVC `/MT` Ninja graphs rebuilt the fixture and exited zero:

```text
build/M0-T310/S5/ccpu/x86/ccpu-bounded-execution.exe
build/M0-T310/S5/ccpu/x64-clean/ccpu-bounded-execution.exe
```

Each printed the original access-function installation, `start`,
`returned-start`, `reenter`, and `returned-recursive` markers.  The fixture
would fail before those markers if either original RAM write or the typed outer
SoftPC return result were absent.

## Boundary result

No guest/host pointer crosses this proof: `c_sas_*` accesses only the original
CCPU/SAS backing memory.  The session mapping manager remains reserved for
component or host-resource identities and the later `nt_mem` physical-page
binding; using it for this intra-SoftPC native backing pointer would create an
unnecessary second address contract.

No BIOS ROM, BOP selector, PIC/PIT/DMA/video/keyboard/mouse device path,
MONITOR/V86 product path, Bochs input, or `src.old` input is selected.
