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
4. **P4 — whole-tree native-width ABI baseline, display/port and startup
   storage.** Before selecting any individual generated carrier or display
   repair, inventory every function-pointer declaration, table initializer,
   callback cast, pointer/integer conversion and compiler ABI/type diagnostic
   in the complete `mvdm-host` and `mvdm-support` original-source trees.
   Source is authoritative: every candidate requires its declaration,
   initializer/assignment, all selected call forms and original owner context
   to be read before it receives a disposition. Compiler x86/x64 diagnostics
   are a coverage aid and cross-check, never evidence that an unreported
   source contract is safe. The audit must record source identity, original
   owner/package, selected or unselected product profile, actual x86/x64
   diagnostic, pointer/address class, and one
   mirror/overlay/adapter/unavailable disposition. It must group common
   contracts into complete repair clusters, so an individual warning is never
   made the next implementation task merely because a build reached it. Only
   after that baseline is reviewed may P4 recover
   source-shaped basic video/IOS port behavior plus the selected disk/floppy
   read path and its original unavailable/media failure direction. Before any
   `cvidc` video path is enabled, the complete generated C-video ABI includes:
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
   are separately audited but are one P4 ABI closure. Their current generated
   carriers are provisional audit inputs, not a P4 completion claim, until the
   whole-tree baseline establishes their common clusters and dispositions.
   Actual guest addresses continue through the existing mapping-manager
   physical binding; internal SoftPC pointers remain native-width private
   values and are not tokenized. No display entry is enabled merely because
   the static original package archives.
5. **P5 — serial/parallel and full-profile verification.** Restore or record
   source-shaped endpoint disposition, then execute the complete original
   profile matrix on x86 and x64.

## Admitted T310 completion sequence after S8

S8 is the original-machine composition package only.  It is not permission to
close T310 while an alternate machine implementation remains in a production
path, nor while the selected SoftPC mirror has not received a complete
two-architecture purity review.

1. **S9 — Bochs production-closure removal.** After S8 records its original
   SoftPC machine dispositions, remove every current production selection,
   fallback, include, link input, build-manifest input, fixture dependency and
   runtime route for `bochs-core`, `adapter-bochs` and any Bochs overlay.
   Existing Bochs material may remain only as indexed historical/comparison
   evidence outside the current production source/build/runtime closure.  The
   resulting app, session and adapter dependency graph must select SoftPC as
   the sole machine.  This S does not modify SoftPC controller algorithms.
2. **S10 — complete SoftPC capability and mirror-purity acceptance.** Audit
   every selected `mvdm-host/softpc.new` machine package and its reached host
   lifecycle edge: CCPU/CPU, FPU, SAS/RAM, EMS/UMB, BIOS/ROM/CMOS, PIC,
   PIT/timer, DMA, keyboard, mouse, video, disk/floppy and serial/parallel.
   Each has exactly one direct, binding-only, adapter-backed or
   original-unavailable disposition with x86/x64 source and execution
   evidence.  Re-read all mirror diffs and overlays: reuse an available
   original SoftPC package/interface before retaining a project-authored shim;
   retain only registered, minimal `DIVERGENCE:` hooks and private overlays.
   The completed production graph has zero `bochs-core`, `adapter-bochs` and
   Bochs-overlay references.  x86 and x64 compile the same SoftPC source and
   initialization/device path; a target-local exception is allowed only for a
   proven unavoidable ABI binding, is registered, and must not select a
   different controller or guest semantic path.

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

## T310 final closure boundary

T310 closes only after S8, S9 and S10 all close.  Formal MSVC `/MT` x86 and
x64 Ninja graphs must each prove the same selected SoftPC
`create -> reset -> firmware/machine initialization -> bounded execution ->
typed controlled stop -> teardown` path.  The final evidence must prove that
the complete production source/build/runtime closure contains no Bochs,
MONITOR, kernel-VDM or `src.old` route; that all selected SoftPC machine
families have source-shaped positive/negative evidence; and that every
remaining mirror diff/overlay is registered, minimal and cannot be replaced
by an available original SoftPC source/interface.
