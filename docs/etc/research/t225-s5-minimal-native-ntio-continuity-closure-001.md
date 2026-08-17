# T225 S5 Minimal Native NTIO Continuity Closure

## P0 Admission

T225 S4 establishes that the original guest DOS `$Exec` COM branch owns PSP,
MCB, entry and return state. Current native staging has reached only
source-built NTIO: NTDOS and COMMAND are not yet executed guest inputs. This
package closes the *machine* prerequisite before a guest DOS fixture is
admitted.

This is a Bochs native-owner package. It does not implement a BOP provider, a
DOS loader, an interrupt service in the adapter, or a host-drive policy.

## Reached-Mechanic Ledger

| Reached or required mechanic | Owner | Present state | S5 disposition |
| --- | --- | --- | --- |
| CPU5/P real-mode decode, RAM and A20 | bx-core / bx-mantle | The minimal machine initializes CPU, ordinary memory and A20 before reset. | Retain; prove entry/continuation state rather than replace it. |
| SIM parameter access during CPU/exception lifetime | bx-mantle | `bx_ntvdm_minimal_sim_initialize` supplies the finite CPU5/P tree and non-reset triple-fault choice. | Retain; audit every S5-reached lookup. |
| Default port-space lifetime | bx-mantle | `init_empty_port_space` installs original default read/write dispatch tables. | Retain; record concrete reached port requirements before adding a device. |
| x87 pending exception, CR0.NE=0 | bx-core plus native machine | The recorded NTIO prefix reaches the original `math_abort` then `DEV_pic_raise_irq(13)` branch. | First diagnostic/closure target; no adapter interception. |
| IRQ13, PIC, IVT and timing | native Bochs machine | No admitted PIC/firmware closure exists in the present minimal machine. | Source/trace-map before any assembly; reject synthetic adapter IRQ. |
| Physical access at `0xA1800` | native memory/machine | A prior post-transaction path reached an unmapped prefetch there. | Establish whether this is a valid reached mapping; do not enable VGA blindly. |
| BOP transaction / checked resume | bx-vdm + bx-core mechanics | Prior narrow transaction copied NTDOS bytes to ordinary RAM and resumed. | Frozen; no BOP selector expansion in S5. |

## Source and Evidence Basis

- `src/bx-mantle/bx_ntvdm_minimal_machine.cc` establishes current startup
  order: SIM, RAM, default ports, CPU initialization and hardware reset.
- `src/bx-mantle/minimal_port_space.cc` provides a native no-device port-space:
  unmapped reads return all ones and writes are ignored.
- `src/bx-core/fpu/fpu.cc:FPU_check_pending_exceptions` preserves original CPU
  semantics: pending x87 summary with `CR0.NE=0` raises native IRQ13.
- `docs/etc/evidence/m0-t95-status-ledger-20260811.md`, source-built NTIO
  prefix observation and correlation records establish the actual reached
  `math_abort` branch. They do not prove a cause or authorize a BIOS/device
  approximation.

## S5 Delivery Sequence

1. Build a default-off, fixed-width native observation of the already-reached
   x87/IRQ13 boundary and machine entry/stop state. It must read neither guest
   semantic data nor BOP fields and must preserve CPU state.
2. Execute one immutable-input observation through the existing source-built
   NTIO prefix fixture; classify the first divergence as entry-state, FPU/IRQ,
   PIC/IVT, or physical-map owned.
3. Admit and assemble only the earliest proven native closure. A device is
   allowed only if the reached path requires it in initialization, reset and
   execution; its lifecycle remains bx-mantle-owned.
4. Re-run the bounded source-built NTIO prefix and record the next terminal
   state. It is not a claim that NTDOS, COMMAND or COM EXEC runs.

## Rejections and Compatibility

Direct and Readonly host-capability profiles remain unchanged; they are not
needed by this machine package. Overlay and Virtual receive no fallback and no
new behavior. Existing adapter BOP routes and ABI stay frozen. Any future
machine change must be selector-blind and may not mention DOS, DEM, COMMAND,
OpenNT or BOP.

## P1 Current-Source Disposition

The historical x87/IRQ13 and `0xA1800` observations are not the current
source-built native frontier. The later current-source record
`t198-s48-clean-ntio-post-wrap-observation-001.md` rebuilds the full CPU5/P
closure, mantle, bx-vdm provider set and its NTIO fixture with MSVC x64 `/MT`.
It reaches `50:0F` (`SVC_DEMGETDRIVES`) and records a valid typed resume. Its
fixture bridge then deliberately issues controlled stop before the ordinary
post-resume continuation.

Consequently, S5 rejects a new x87 diagnostic, PIC/IRQ13 assembly, firmware
addition, or `0xA1800` mapping. Those former records remain historical
provenance only. Reintroducing their retired diagnostics would expand bx-core
without evidence and violate the minimal-machine boundary.

The current native machine closure is sufficient for the next bounded BOP
package observation. The outstanding blocker is the fixture's deliberately
controlled stop, not a missing native machine capability.

## Revised Delivery Sequence

1. Retain S48 as the current-source native-continuity witness.
2. Move the controlled-stop fixture policy into the next *BOP owner-package*
   admission. That package may remove only its test stop after the corresponding
   provider family has an approved disposition and regression.
3. Do not change bx-core or bx-mantle unless a future bounded observation
   proves a new native mechanical requirement.
