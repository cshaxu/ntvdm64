# T95 S6 rejected-owner retention-root trace

## Input and machine ledger

This read-only trace starts only with the 35 rejected identities in
observation-003's frozen link ledger. The machine-readable grouped trace is
`artifacts/analysis/t95-s6-msvc-x86-cpu3-link-retention-003-20260811-001/rejected-owner-root-path-ledger.json`,
SHA-256 `F0D8E732541FE72863081E1DCD8CDA3E579A369A4BEE4D56ECC66AFAB3DCE336`.
It assigns every rejected identity to its immediate linker referencing object
and one of the four roots below. GETSEC and all default retained CPU handlers
are excluded.

## Distinct root paths

| Root | Immediate rejected-owner objects/members | Classification | Source evidence | Disposition |
| --- | --- | --- | --- | --- |
| Minimal SIM | `siminterface.obj`: `bx_real_sim_c` virtual methods for rc/config/plugin/UI plus `bx_atexit` | fixture/composition rooting artifact | `bx_ntvdm_minimal_machine.cc:43` -> `bx_ntvdm_minimal_sim.cc:13` -> `gui/siminterface.cc:82-200,319-350`; direct rejected calls at `105-106,352-354,436-456,834-836,1387-1396` | A Bochs-local narrowed SIM retention surface might be possible, but it requires a separately admitted seam. Adapter ownership is forbidden. |
| Empty port composition | `devices.obj`: plugin control/entry points, CMOS/DMA/PIC/PIT/VGA/floppy/hard-drive/keyboard providers | member that should be dead-stripped but is not | admitted root is `bx_ntvdm_minimal_machine.cc:52` -> `iodev/devices.cc:273`; rejected full-PC members are `devices.cc:103-271,356-390,976-990` | The minimal helper itself does not call these full-PC members. A Bochs-local object-retention separation could be investigated later; no seam is selected. |
| PC-system global | `pc_system.obj` GUI cleanup reference | fixture/composition rooting artifact | composition-owned global is in `bx_ntvdm_minimal_machine.cc:20-22`; constructor `pc_system.cc:48`; rejected `exit` path `pc_system.cc:209-230` | A Bochs-local lifecycle-surface proof would be needed. No adapter transfer or seam decision follows. |
| Logging accessor | `logio.obj` -> `bx_atexit` | member that should be dead-stripped but is not | root `bx_ntvdm_minimal_machine.cc:41`; `logfunctions::fatal` calls `bx_atexit` at `logio.cc:552-564` | Hard incompatibility for the current object model until a source-backed Bochs-local logging retention boundary is admitted; adapter cannot own it. |

No path proves that a minimal-core method genuinely requires a rejected product
owner. The trace instead shows common-object/vtable/lifecycle retention from
the admitted composition roots. This is not proof that a future seam is safe;
it only distinguishes the root shapes for coordinator review.

## Stop and next gate

No seam, exception, object, configuration, build, device, adapter, or runtime
action was chosen. The next gate is coordinator review of the four roots and
their static-retention boundary; any attempt to remove a root needs a new
bounded admission and must remain Bochs-local.
