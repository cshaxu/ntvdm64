# M0 T244 S1 — PC Keyboard/IVT BOP-5F Machine Audit

## Decision

The complete `BOP 5F` lifecycle is a PC-machine package, but it is **not** a
reason to import the historical NT4 CCPU/ICA/event-thread product shell or to
place keyboard/BOP terms in bx-core/bx-mantle.  T243 already closes the
headless `STREAM_IO=2` table publication.  The remaining package is native
PC hardware composition: 8042 port lifecycle, a pre-existing native 8259,
timer basis and guest-owned IVT handlers.

The only admissible S2 implementation slice is a headless, native Bochs 8042
controller composed in bx-mantle beside the existing native PIC.  It must
remain selector-blind: it exposes ordinary ports `60h`/`64h` and normal IRQ1
to the CPU, accepts no host input or BOP request, and uses original Bochs
device behavior after its minimum product-shell dependencies are explicitly
severed.  Physical host-input ingestion, mouse, paste, GUI status bar,
keyboard layouts, `BOP 5C`, video/printer, and the NT4 event thread are
separate later capabilities.

## Original OpenNT path and ABI

```text
NTIO spckbd.asm
  installs guest IVT 08h,09h,13h,16h,19h,1Ah,40h
  writes 33-word CS:SI kio_table; DS is resident keyboard code
  BOP 5F, AX=BEEF, DI=iret_bop_table, CX=entry size
      -> nt_bop.c:MS_bop_F
          -> keybd_io.c:kb_setup_vectors
          -> monitor iret-table address, unmask/restart ICA, CF=1
      -> guest JC isk_int9
```

The `kio_table` is a historical cross-layer layout, not a Bochs device ABI.
The table effects separate as follows.

| Table words | Original effect | Owner/disposition |
| --- | --- | --- |
| `0..11` | key maps, dummy/INT05/INT15/NOP/RTC callback locations | Guest table plus OpenNT keyboard monitor. No mantle interpretation. |
| `12..16` | idle pointers, INT1B, INT10 caller/vector | Guest/timer/video ownership; out of S2. |
| `17` | `useHostInt10` state byte | Closed in T243 headless route only. |
| `18..31` | video, printer, wait, floppy, VGA, INT15, INT08/13 and stream buffers | Separate machine or guest owner packages; reject from S2. |
| `32` | `BEEFh` table guard | Closed in T243. |

`nt_bop.c:MS_bop_F` additionally stores the monitor IRET BOP table from
`DS:DI`, verifies `CX` only in a debug build, clears `DelayIrqLine`, restarts
the historical ICA and sets CF under `MONITOR`.  The IRET table and historical
`nt_eoi.c` lock/event state depend on CCPU/monitor-private globals and must
not be rehosted as a bx-vdm policy.  Existing T241 has already recovered the
separate `UpdateKbdInt15` IVT-15 compare/clear/update fragment for XMS 52:09;
it is not a keyboard-controller substitute.

## Current native machine facts

| Concern | Current code | Audit outcome |
| --- | --- | --- |
| Port-space lifecycle | `src/bx-mantle/minimal_port_space.cc` | Reusable. It provides original handler registration and exact cleanup, currently defaulting all ports. |
| 8259 PIC | `src/bx-mantle/pic.cc`, exception `BX-MANTLE-082` | Reusable and live. It registers `20h/21h/A0h/A1h`, starts masked, and supports normal IRQ/IAC/EOI. No new PIC implementation is warranted. |
| PC timer core | `src/bx-mantle/pc_system.cc` | Reusable. It owns Bochs timer registration/ticks without a product scheduler. It is a mechanism only. |
| 8042 device | `refs/bochs/iodev/keyboard.{cc,h}` | Original Bochs source exists, but is not presently in the formal manifest and is not directly composable. |
| PIT | `refs/bochs/iodev/pit.{cc,h,82c54.cc}` | Not needed to initialize an 8042 or deliver IRQ1; defer until an original caller requires timer port/IRQ0 behavior. |
| Existing BOP transport | bx-vdm generic-UD route | T243 remains the sole interpreter of BOP-5F. The S2 machine does not receive selector, table, DOS, OpenNT or SoftPC data. |

## Why raw Bochs keyboard cannot be linked unchanged

`keyboard.cc:init()` correctly supplies ports `60h/64h`, IRQ1/12 and a native
controller timer, but it also assumes the full Bochs product shell:

1. plugin construction/registration and the `pluginKeyboard` global;
2. `keyboard_mouse.*` SIM parameters (serial delay, paste delay, mouse type,
   mouse enabled and keyboard type);
3. CMOS mutation, PS/2 mouse registration and mouse lifecycle;
4. GUI status-bar registration and runtime GUI parameter handlers;
5. paste-buffer and host GUI ingestion paths.

The minimal SIM contains only CPU parameters, no GUI object, no CMOS or mouse
device, and deliberately does not invoke the plugin manager.  Directly
calling the upstream `init()` would dereference absent parameters/GUI and
silently expand the product shell.  Recreating those subsystems merely to
make the file link is rejected.

## Four-rung ledger and S2 boundary

| Rung | T244 disposition |
| --- | --- |
| Original source reuse | Reuse the adopted Bochs 8042 controller body and existing native PIC/port/timer mechanisms wherever independent of the product shell. |
| Smallest composition/shim | Add a mantle-private factory/lifecycle plus the smallest fixed SIM parameter fragment needed by the retained 8042 body. It names only mechanical keyboard-controller configuration, never BOP/DOS/OpenNT semantics. |
| Registered external-code intrusion | Expected but not yet authorized: a narrow, documented headless split in the adopted 8042 source to omit GUI, mouse, CMOS, paste and plugin registration while retaining controller/port/IRQ semantics. Register it before any patch; no bx-core change is indicated. |
| Newly authored behavior | Rejected: do not write a synthetic keyboard, fake IRQ1, adapter-owned scan-code queue, or NT4 ICA/event replacement. |

S2 must begin with a source-level severance map and decide whether the
headless 8042 can retain upstream methods with a finite parameter fragment.
It must prove: inactive port rejection; `60h/64h` registration and cleanup;
no GUI/CMOS/mouse/plugin dependency; native PIC IRQ1 assertion/acknowledgment;
and two clean machine lifecycles.  It must *not* accept host key events yet.
If this exact slice needs PIT, video, CMOS, mouse, firmware or a CCPU event
thread, S2 stops and transfers that dependency rather than enabling it.

## Receiving work

- T244 S2: headless selector-blind 8042 lifecycle only, contingent on a
  registered external-code exception.
- Later PC device package: PIT/IRQ0 only after a source caller proves it.
- Top-level/console package: host input, `BOP 5C`, COMMAND keyboard layout
  and console notification.
- Existing XMS `52:09`: already uses its own IVT-watch contract; no rewrite.
