# M0 T310 S8 — Original SoftPC machine composition plan

## Objective

Recover one selected original SoftPC/CCPU machine profile in original startup
order. This is one integrated machine package, not a sequence of trace-driven
device patches. The profile starts from the closed S5 CCPU interval and S7
physical binding, then ends only after original creation, reset, firmware
initialization, a bounded execution interval, typed stop and teardown work on
both host architectures.

## Ordered work packages

1. **P1 — reset, BIOS/ROM/CMOS and startup-media intake.** Audit and directly
   compose the complete original reset roots, selected immutable firmware
   inputs, ROM/CMOS initialization and their host control edges. Record exact
   media-present and media-missing results. Do not add a controller substitute.
2. **P2 — memory/SAS/A20 and PIC/PIT/heartbeat.** Consume S7 physical binding;
   recover original `nt_timer.c`, `timer.c`, `ica.c` and their session/public
   Win32 boundaries while retaining original tick, IRQ and cancellation order.
3. **P3 — input and DMA.** Recover original keyboard/mouse controller/input
   initialization and DMA port/range behavior with fixed-width copied input or
   checked numeric ranges only.
4. **P4 — display/port and startup storage.** Recover source-shaped basic
   video/IOS port behavior plus the selected disk/floppy read path and its
   original unavailable/media failure direction. Before any `cvidc` video
   path is enabled, this package closes the complete generated C-video ABI:
   `cvidc/evidgen.h` vector slots, every `vglfunc.c` initializer, every
   generated-table initializer in `evidfunc.c`, and every reached
   `base/video` handler-table or callback assignment must agree with the actual
   `S_*` callable signature on both x86 and x64. The original generated
   `S_*` bodies take four generic host-word arguments, while the vector tables
   declare typed zero- and one-argument interfaces. x86 happened to tolerate
   that source-era mismatch; x64 cannot treat it as a runtime contract.
   Recovery must be generated, private `mvdm-host-overlay` carriers derived
   from the original generator output—not hand-written per-slot wrappers—and
   they must preserve the original selected vector/table order.  The CVIDC
   table carrier and the independently generated/base-video callback families
   are separately audited but are one P4 ABI closure.
   Actual guest addresses continue through the existing mapping-manager
   physical binding; internal SoftPC pointers remain native-width private
   values and are not tokenized. No display entry is enabled merely because
   the static original package archives.
5. **P5 — serial/parallel and full-profile verification.** Restore or record
   source-shaped endpoint disposition, then execute the complete original
   profile matrix on x86 and x64.

## Recovery constraints

- Directly compose the original translation unit first. A modern public API
  may bind a historical dependency but cannot replace an available controller
  algorithm.
- A source-bearing native pointer, handle or 32-bit identity crossing uses the
  existing session mapping manager on both host architectures. Short-lived
  guest leases cannot be physical backing or asynchronous device state.
- No package selects a Bochs, MONITOR, kernel-VDM or `src.old` route.
- Any newly authored behavior belongs in its named adapter and must preserve
  the original function shape, ordering and failure direction. Mirror changes
  carry `DIVERGENCE:` and their README register entry.

## P1 acceptance boundary

P1 is complete only once the complete source-selected reset/firmware/media
group has a direct composition attempt and an owner/disposition for every
outbound form. It may establish an explicit original unavailable result for
missing media, but it may not claim a runnable device profile until P2--P5.
