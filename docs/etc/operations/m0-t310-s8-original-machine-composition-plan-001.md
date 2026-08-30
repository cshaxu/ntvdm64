# M0 T310 S8 — Original SoftPC machine composition plan

## Objective

Recover one selected original SoftPC/CCPU machine profile in original startup
order. This is one integrated machine package, not a sequence of trace-driven
device patches. The profile starts from the closed S5 CCPU interval and S7
physical binding, then ends only after original creation, reset, firmware
initialization, a bounded execution interval, typed stop and teardown work on
both host architectures.

## Ordered S owner packages

Each S is a bounded source-recovery package. Its listed clusters are atomic:
original definition, table/initializer, callers, prior repairs, x86/x64
diagnostics and local behavior are reviewed together. A later S may consume a
completed public contract but may not reopen it with a local workaround.

1. **S8 — CCPU executor, access table, SAS/monitor vector and extended-BOP
   default.** `SPC-CCPU-EXECUTOR-DISPATCH`, `SPC-CCPU-ACCESS-TABLE`,
   `SPC-CCPU-SAS-MONITOR-VECTOR` and `SPC-CCPU-EXTENDED-BOP-DEFAULT`.
2. **S9 — memory, SAS, A20, EMS and XMS.** `SPC-SAS-MEMORY-CONTRACT`,
   `SPC-MEMORY-MAPPING-BINDINGS`, `MVDM-XMS.486-OWNER-PACKAGE`, plus reached
   XMS/EMS control forms. The mapping manager remains only at a true
   host-identity boundary.
3. **S10 — BIOS, firmware, reset and startup media.**
   `SPC-BIOS-FIRMWARE-BINDINGS` and selected `base/bios`, ROM, CMOS, reset and
   immutable-media forms.
4. **S11 — PIC, PIT, DMA, timer and system controller.**
   `SPC-SYSTEM-CONTROLLER-CALLBACKS`,
   `SPC-HOST-SYSTEM-INTERRUPT-BINDINGS`, and reached timer/ICA/DMA chains.
5. **S12 — keyboard, mouse and input binding.**
   `SPC-INPUT-CONTROLLER-CALLBACKS` and `SPC-HOST-INPUT-BINDINGS`.
6. **S13 — storage, floppy, disk, serial and parallel.**
   `SPC-STORAGE-CONTROLLER-CALLBACKS`, `SPC-HOST-STORAGE-BINDINGS` and
   `SPC-COMMS-CONTROLLER-CALLBACKS`.
7. **S14 — C-video, base-video and host-video.**
   `SPC-CVIDC-GENERATED-DISPATCH`, `SPC-VIDEO-CONTROLLER-DISPATCH` and
   `SPC-HOST-VIDEO-BINDINGS` are one display contract.
8. **S15 — service residuals, cross-family reread and profile verification.**
   Reached `MVDM-DOS-OWNER-PACKAGE`, `MVDM-SIM32-OWNER-PACKAGE`,
   `MVDM-SUPPORT-LIBRARY`, `SPC-HOST-PLATFORM-BINDINGS`,
   `SPC-HOST-CONFIGURATION-BINDINGS`, `SPC-HOST-VDD-BINDING`, all
   zero-diagnostic/unselected clusters, then final all-42-cluster reread and
   full x86/x64 machine-profile verification.

## Whole-tree ABI audit rule

Before selecting any individual generated carrier or display repair, inventory every function-pointer declaration, table initializer,
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

   **P4 execution cadence.** The baseline is not a queue of individual
   compiler diagnostics.  The generated source/call index and compiler ledger
   are first reduced into owner-contract clusters.  A cluster is reviewed as
   one unit: original declaration(s), definition(s), table/initializer forms,
   selected caller forms, x86/x64 diagnostic evidence and its runtime owner
   are read together.  The execution order is maintained in
   `m0-t310-s8-p4-owner-contract-execution-order-001.md`: startup-critical
   CCPU, memory/system/firmware, storage and input are considered before
   optional video, communications, debugger and unselected product surfaces.
   A warning may only cause a mirror correction, overlay or adapter change
   after its entire contract cluster has that evidence.  Non-width source-form
   records remain compiler-visible and leave the x86/x64 worklist by a
   documented cluster disposition, never by suppression.
## T310 S8 closure boundary

S8 closes only when its four CCPU clusters have source-shaped dispositions and
focused x86/x64 evidence. S9--S15 then execute in order. T310 closes only
after S15 completes the all-42-cluster reread and the selected original SoftPC
`create -> reset -> firmware/machine initialization -> bounded execution ->
typed controlled stop -> teardown` profile on both host architectures. Neither
S8 nor T310 removes the remaining Bochs production route or claims
whole-program SoftPC capability/mirror purity.

Those formerly numbered S9 and S10 scopes are intentionally separate queued T
packages after the active packet: first production-route removal, then the
`i386` semantic audit, then complete SoftPC capability and mirror-purity
acceptance. Their independent proposals and ordering are authoritative in the
package-recovery program and `QUEUE.md`.

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

T310 closes when S8 closes. Formal MSVC `/MT` x86 and x64 Ninja graphs must
each prove the same selected SoftPC `create -> reset -> firmware/machine
initialization -> bounded execution -> typed controlled stop -> teardown` path
for the S8-selected machine composition. Bochs production-route removal and
full SoftPC capability/mirror-purity acceptance are explicit subsequent T
packages, not implicit T310 exit conditions.
