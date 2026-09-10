# T225 S25 P1 — Startup-Machine Source, ABI and Failure Map

## Decision

S25 recovers one startup-machine composition package, not a generic BIOS
dispatcher. The package has eight members: `02`, `06`, `12`, `15:AH=88`,
`17`, `5F`, `66`, and `C8`. `5E` remains a top-level OpenNT package identity
and is deliberately excluded from the package ABI.

## Original Contract Map

| Identity | Original owner | Required ordering and result | Direct source reuse | First usable recovery rung |
| --- | --- | --- | --- | --- |
| `02` | `softpc.new/base/system/unexp_nt.c:unexpected_int` | OCW3/read master ISR; software branch writes BDA `046B=FF`; hardware branch reads/masks master and optional slave, sends EOI, then writes the original ISR to BDA. | Not composable: CCPU/SAS register, port and memory ABI. | Fixed-width source-derived `opennt-host` operation transcript over typed mantle RAM/PIC actions. |
| `06` | `base/system/illegalp.c:illegal_op_int`, then `unexpected_int` | Read saved real-mode IP/CS; if IP wraps, write incremented CS first; write incremented IP; run the `02` common protocol. | Not composable for the same CCPU/SAS ABI reason. | Retain S23 transcript, refactored only to call the shared common transcript. |
| `12` | `base/bios/bios.c:memory_size` | Return conventional-memory value for the declared first machine profile. | Historical handler is tied to SoftPC firmware/product state; current source evidence is a fixed BDA snapshot. | Retain existing typed declared profile result; no direct Bochs/CMOS query. |
| `15:AH=88` | Reached `sysinit1.asm` extended-memory query; historical table form is not a general INT 15 admission. | Return only declared extended KiB result; all other AH forms decline. | No isolated historical handler is composable or selected. | Retain existing typed profile result and explicit decline. |
| `17` | `base/comms/printer_.c:printer_io` | With `PRINTER` absent, return without port/device/host action. | Direct build would require optional printer product shape. | Retain exact no-device `RIP+3` continuation. |
| `5F` | `host/src/nt_bop.c:MS_bop_F`, `bios/spckbd.asm:InstSpcKbd` | Guest publishes KIO/IRET data; original host installs vectors/restarts ICA. First profile needs only the source-proven table-state publication and bounded continuation. | Full routine requires historical monitor/ICA/event-thread composition. | Retain checked source-derived stream-state transaction or bounded CF/RIP continuation; do not rehost ICA/keyboard. |
| `66` | `dos/v86/doskrnl/bios/spcemm.asm:InitSpcEmm` | Clear BX, issue BOP; `BX=0` selects original no-EMS failure continuation. | EMS/LIM implementation is not admitted. | Retain typed `BX=0`, `RIP+3` unavailable result. |
| `C8` | `bios.c:v_host_mouse_install1 -> host/src/nt_mouse.c:host_mouse_install1 -> mouse_install1` | Consume declared four-byte vector table; establish INT 33 IVT entry; resume. | Full source depends on host mouse/UI composition. | Retain checked source-derived headless vector transaction; do not enable host mouse/device. |

## Shared `02`/`06` ABI

The current `opennt_int06_provider_v1` exactly serializes the `06` prelude and
the `unexpected_int` common protocol as copied one-byte operations. S25 may
extract the common protocol into a versioned `opennt-host` provider only if:

1. `02` begins directly at original `unexpected_int` OCW3/master-ISR action;
2. `06` retains four saved-frame reads and original CS-before-IP carry order
   before entering that exact common protocol;
3. all RAM and port work remains copied, fixed-width requests executed by
   selector-blind mantle APIs;
4. invalid acknowledgement, invalid frame range, inactive mantle, or any
   unexpected operation fails closed to controlled stop; and
5. neither provider knows a BOP selector—the choice remains in bx-vdm.

No raw callback, CCPU/SAS object, TLS state, Bochs object, host pointer, or
CRT-owned allocation crosses this ABI.

## Existing-Route Migration Rules

- `boot_namespace_composition` is currently the first bridge candidate and
  contains the retained `12`, `15:88`, `17`, `5F`, `66`, and `C8` paths.
- `native_bop_composition` is the second candidate and contains S23's `06`
  route. It currently leaves direct `02` unhandled.
- S25 must create one package-owned composition surface selected by the bridge
  before DEM/COMMAND; it may delegate existing typed member services but must
  eliminate split ownership as the live route.
- The old `machine_composition_v2` callback/TLS island is historical evidence
  only. It is not selected by the formal bridge and must not become a fallback.
- `5E` keeps its existing top-level facade route; S25 must prove this identity
  is not captured by the new machine surface.

## Stop Conditions

The package stops for re-admission if sharing the `02`/`06` transcript needs a
new PIC/device/firmware feature, a Bochs core change, a general BIOS table, a
host mouse/printer/EMS/keyboard service, or any replacement of source-defined
failure with success. No such condition is currently evidenced.
