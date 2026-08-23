# Adapter-External Intrusion Exception Register

## Question

Which source changes outside `src/bx-vdm/` are authorized as
exceptions to the hard Bochs/OpenNT boundary, and what prevents each one from
becoming a semantic migration?

## Scope And Rule

This is the complete register for direct project-authored changes below
`refs/bochs/`, `src/bx-core/`, `src/bx-mantle/` and `src/opennt/`. An entry is
required before a patch is made.
An imported source file, an archived tree, and an un-applied patch are not an
intrusion. Each entry must name its approval, exact files, behavior, negative
case, and removal/review condition.

The owner confirmed on 2026-08-10 that a direct Bochs change is permitted when
necessary, provided it is entered here before the patch. This is exception
authority, not a general permission: the preferred implementation location
remains `src/bx-vdm/`, and no entry may introduce BOP, DOS/WOW/DEM,
OpenNT, or host-service semantics into Bochs.

**T197 location migration.** Historical entries retain the source paths that
were true when they were registered. The adopted native paths are now
`src/bx-core/cpu/`, `src/bx-core/fpu/` and `src/bx-core/memory/`; the complete
crosswalk and negative scope are recorded in
[`t197-s4-core-path-migration-001.md`](t197-s4-core-path-migration-001.md).
Every new exception must name its current path, not the historical prefix.

## Adapter ABI Divergences (Not External Intrusions)

These rows record deliberate, source-derived ABI substitutions inside
`src/bx-vdm`. They are not patches to adopted Bochs or OpenNT source and do
not authorize such patches. They are retained here because they change an
original OpenNT host-composition assumption and must remain as auditable as an
external intrusion.

### BX-VDM-001: Native-Width Host-Handle Representation

**Owner approval.** On 2026-08-20 the owner confirmed that the unified
host-handle manager is definitely unlike the historical OpenNT implementation
and must be recorded as an explicit exception.

**Need.** Original NT4 DEM and COMMAND composition assumes a 32-bit host
`HANDLE` can cross guest-facing register pairs (`AX:BP` or `BX:CX`). That
assumption is not safe for a modern x64 host, and preserving raw host values
would also make the product's x86/x64 ABI behavior depend on host pointer
width. Direct original-source reuse is therefore impossible at this ABI seam;
the original provider bodies remain reusable.

**Procedure.** `src/bx-vdm/bop/shim/bx_ntvdm_host_handle_manager.{h,c}` owns
one session-scoped bidirectional map between a private native `HANDLE` and a
32-bit opaque guest ID. DEM publishes owned handles and COMMAND publishes
borrowed standard/redirection handles. Original register layouts remain in
place and both 16-bit halves carry the same-width opaque ID; zero is invalid
and `UINT32_MAX` remains the historical COMMAND default-handle sentinel. The
manager owns only representation and lifetime; it has no BOP,
OpenNT-service, DOS, path, drive, device, CPU, or Bochs policy.

**Retained behavior and failure rule.** The original DEM/COMMAND dispatcher,
provider order, parameter layout, result registers, carry/error paths, and
host capability calls remain their original owners. A zero, reserved, or
unknown guest token follows the existing invalid-handle failure route; no raw
host pointer is reconstructed. Owned entries close on release or session
reset; borrowed entries are forgotten but never closed. A future
combined DEM/COMMAND runtime session must inject one manager instance rather
than add a new private table.

**Files and verification.** The active routes are
`dem_direct_host_session.c`, `demhndl_shim.c`, `command_misc_shim.c`, and
`command_native_session_shim.c`; focused DEM/COMMAND fixtures and the manager
fixture are recorded by [T232 S3](../evidence/t232-s3-command-unified-handle-migration-result-001.md)
and [T232 S4](../evidence/t232-s4-non-v1-handle-cleanup-result-001.md).

**Review condition.** Reject or revise this exception if it acquires a
selector/service branch, DOS JFN/SFT policy, host-path policy, Bochs/mantle
dependency, raw pointer export, pointer-width-dependent guest ABI, or any
second active non-v1 host-handle/token table.

## Active Exceptions

| ID | Owner approval | Imported code changed | Scope | Status |
| --- | --- | --- | --- | --- |
| BX-MANTLE-091 | 2026-08-22: owner has standing queue authority for T244 S2, while requiring registered, minimum-necessary Bochs changes | Adopted Bochs 2.6 `src/bx-mantle/keyboard.{cc,h}` plus a mantle-private factory/lifecycle; no bx-core source | Sever exactly the upstream 8042 controller's plugin entry, GUI status items, paste path, mouse/CMOS registration and runtime parameter handlers so one native controller registers only ports `60h/64h`, uses the existing native timer primitive and raises ordinary IRQ1 through the existing PIC. | Implemented and verified: no finite SIM fragment was needed. The formal r2 fixture proves inactive rejection, live status/data ports, source `F2` response ordering, IRQ1→IAC `09h`→EOI, cleanup and two clean lifecycles. No BOP/selector/DOS/OpenNT/SoftPC term, host key input, guest table/IVT inspection, synthetic scan-code queue, mouse, GUI, CMOS, plugin manager, PIT, video, firmware, adapter callback, host capability or bx-core change entered the patch. |
| BX-ABI-090 | 2026-08-19: standing owner authority for registered, necessary Bochs mechanical changes; T230/S2 directly imports OpenNT `demhndl.c`, whose `50:47` source contract writes ZF | Adopted `src/bx-core/cpu/exception.cc`; existing fixed-width `bx_ntvdm_generic_ud_outcome_v1` only | Extend the existing selector-blind accepted typed-result application from CF to the already copied ZF bit. Bochs reads only `eflags_write_mask`/`eflags_values`; it gains no selector, BOP, OpenNT, DEM, DOS, pipe, adapter or host-capability knowledge. | Registered before patch. The permitted change is one `set_ZF` application adjacent to the existing `set_CF` block. A focused fixture must prove ZF copied through an accepted generic result, while absent mask preserves original FLAGS. Reject if any result-dependent branch, opcode/address inspection, guest memory, device, firmware, lifecycle, callback or semantic term is needed. |
| BX-CORE-089 | 2026-08-18: P22 leaves the original COMMAND/NTDOS interrupt-return/control-state question after rejecting a post-EndInit adapter file-family repair; P23 proves OpenNT and bx-vdm cannot observe ordinary IRET completion and current upstream instrumentation is a no-op stub | Adopted `src/bx-core/cpu/ctrl_xfer16.cc` and `ctrl_xfer32.cc`; prospective project-owned `src/bx-mantle/bx_ntvdm_interrupt_return_observation_v1.*`; diagnostic fixture/build definition only | Candidate default-off, selector-blind fixed-width ring of successfully completed real/V86 IRET16/IRET32 post-return facts: instruction count, CS:RIP, FLAGS, AX/BX/CX/DX, DS/ES/SS:SP, execution mode and operand width. Core does not interpret retained fields; mantle alone owns configuration, reset, bounded storage and copied query. | Registered before implementation. No BOP/selector/service/vector/function/OpenNT/DOS/SoftPC term, pairing rule, guest-memory or stack-frame access, address/opcode predicate, CPU/register/flag/stack/interrupt-delivery mutation, exception/result path, device/firmware/timer, adapter callback, host capability, CLI argument, default product behavior, or instrumentation activation is permitted. Calls, if implemented, occur only after existing successful `RSP_COMMIT` and only in real/V86 handling; protected-mode IRET records nothing. Require a complete CPU5 x64 `/MT` profile fixture: disabled/default, IRET16 and IRET32, real/V86 admission or recorded justified inapplicability, protected-mode decline, reset/wrap and retained controlled delivery; preserve the default formal graph. Implemented and verified: default-off diagnostic macro calls only follow real/V86 `IRET16`/`IRET32` `RSP_COMMIT`; a fresh CPU5 x64 `/MT` fixture proves disabled behavior, real IRET16/IRET32 copied post-return records, protected-mode decline and ring/reset, while a fresh default native target links with the macro unarmed. V86 is source-path covered but has no dynamic fixture witness. No integration or repair is selected. Remove/reject if any semantic filter, stack/guest capture or execution change is needed. |
| BX-CORE-088 | 2026-08-18: P19 proves the remaining complete COMMAND/NTDOS transient lifecycle cannot be observed through adapter/BOP code; standing owner authorization permits only a registered mechanical exception | Adopted `src/bx-core/cpu/soft_int.cc`; project-owned `src/bx-mantle/bx_ntvdm_software_interrupt_observation_v1.*`; diagnostic fixture/build definition only | Default-off, selector-blind fixed-width ring of every executed real/V86 software-interrupt pre-delivery fact: vector, instruction count, CS:RIP, FLAGS, AX/BX/CX/DX, DS/ES/SS:SP. Core does not interpret any field; mantle owns configuration, reset, bounded storage and copied query. | Registered before implementation. No BOP/selector/service/function/OpenNT/DOS/SoftPC term in core, no vector/function filter, guest-memory access, instruction decode, address/opcode predicate, CPU/register/interrupt-delivery mutation, exception/result path, device/firmware/timer, adapter callback, host capability, CLI argument or default product behavior. Disabled and protected-mode paths record nothing. Implemented and verified: the default graph remains unarmed; the separate CPU5 x64 `/MT` fixture proves disabled, real-mode ordered `20h`/`21h`, protected-mode decline, bounded wrap/reset and retained controlled delivery. The core hook is guarded only by real/V86 mode and has no semantic selector/service branch or guest-memory access. No integration or repair is selected. Remove/reject if a semantic filter or altered delivery is needed. |
| BX-CORE-087 | 2026-08-17: owner standing authorization permits a registered, source-proven Bochs mechanical exception; T225 S55/S56 identify the SoftPC real-mode segment-limit difference | Adopted src/bx-core/cpu/access32.cc and only any independently classified sibling access path; no adapter/mantle semantic source | Candidate default-off, selector-blind compatibility gate for the OpenNT CCPU rule that suppresses real-mode/V86 segment-limit faults. It must retain Bochs behavior when disabled and preserve protected-mode checking. | Registered before implementation. No selector, BOP, OpenNT/DOS/SoftPC name in code, guest address special case, host/profile/CLI input, adapter callback, memory mapping change, device/firmware change, descriptor mutation, or new exception result is permitted. S56 classification finds the source rule spans all real/V86 operand, stack and control-transfer limit behavior. Under standing owner authority, the whole default-off bx-core/bx-mantle compatibility profile is admitted; a narrow access32.cc-only patch remains rejected. Implementation and grouped regression now cover scalar, RMW/string, stack, and fetch/control paths; all retain protected-mode gating. The paired Direct/Readonly native invocation installs composition, enters machine stage and reaches identical finite-budget BOUND diagnostics; it admits no trace-led repair. |
| BX-TRACE-085 | 2026-08-17: T225 S30 is admitted after S29 proves that the current first-fault record lacks the direct `read_virtual_word_32` access tuple; standing owner approval permits only registered, evidence-backed Bochs mechanics | Adopted `src/bx-core/cpu/access32.cc`; new project-owned `src/bx-mantle/bx_ntvdm_segment_access_observation_v1.*`; one formal fixture and existing first-fault lifecycle only | Default-off, selector-blind copied record immediately before the original direct-limit or failed-read-check `exception()` call in `read_virtual_word_32`. It carries only access/branch kind, segment index/value, effective offset, width, cached valid bits and scaled limit. It requests no stop; the existing first-fault observer remains the only finite-stop owner. | Registered before implementation. No BOP/selector/service/OpenNT/DOS term, instruction decoding, guest-memory/linear/physical address copy, CPU/descriptor mutation, exception result change, device/firmware/timer feature, host pointer/capability, adapter callback or public ABI is permitted. An unarmed, invalid or duplicate record declines and the original access and exception path is unchanged. Require formal disabled/direct-limit/duplicate/first-fault-stop tests, source scan, and separate immutable Direct/Readonly evidence before any owner attribution. | Implemented and verified. The formal CPU5 x64 `/MT` fixture proves the real-mode `A1 FFFF` direct-limit tuple, disabled/duplicate decline and retained first-fault stop. Immutable Direct/Readonly each record the identical `DS=0C41`, offset/limit `FFFF`, two-byte direct-limit fact. No guest, BOP/provider or normal unarmed behavior changed. |
| BX-TRACE-084 | 2026-08-17: T225 S28 is admitted after S27 P5 records a repeat native segment-limit failure and the owner grants standing approval for evidence-backed, bounded Bochs mechanical diagnostic exceptions | Adopted `src/bx-core/cpu/exception.cc`; new project-owned `src/bx-mantle/bx_ntvdm_first_fault_observation_v1.*`; formal fixture/lifecycle only | Add a default-off, selector-blind, fixed-width first non-UD exception observer. It copies only vector, error code, pre-delivery CPU register/selector state and fault RIP to mantle. If the private diagnostic is armed and accepts the first event, it requests the existing finite controlled-stop transfer; otherwise the original exception path is unchanged. | Registered before implementation. No BOP/selector/service/OpenNT/DOS term, instruction decode/window, guest-memory access, descriptor modification, CPU-result/resume, device/firmware/timer feature, host pointer, host capability, product shutdown or adapter callback is permitted. Implemented and verified. P2 formal CPU5 x64 `/MT` fixture proves a test-owned vector-0 first-fault copy and existing controlled stop while a local generic-UD bridge declines. P3 routes CLI arm/copy only through the mantle-private diagnostic and existing machine-stage classification; immutable Direct/Readonly each yield the identical finite `#GP(13)` record at `0C41:2357`. No engine request ABI, adapter/provider route, guest access, special address branch or unarmed exception behavior changed. |
| BX-MANTLE-082 | 2026-08-17: T225 S23 P2 source-builds the original 8259 PIC and proves the minimal machine cannot clean up a non-default port map without the original unregistration bodies | Adopted `src/bx-mantle/pic.cc`/`pic.h`; project-owned `src/bx-mantle/minimal_port_space.cc` and minimal-machine lifecycle | Replace the original PIC plugin entry only with mantle-private create/destroy helpers that bind the original `thePic` singleton required by CPU5 `BX_USE_PIC_SMF`; bind one native `bx_pic_c` after the established empty port-space, register only `20h`, `21h`, `A0h`, `A1h` byte handlers through original `init()`, then use the exact original unregistration semantics to restore the stub/default state before normal cleanup. | Implemented and verified: formal MSVC x64 `/MT` Ninja fixture proves native mask/IRQ/IAC/EOI behavior and two consecutive clean lifecycles; the source scan and focused boundary test reject plugin/registry/service dependencies. No bx-core change, plugin loader/registry, `register_state` invocation, debugger, timer, firmware, second device, BOP/selector/OpenNT/DOS term, guest-memory operation, host capability, or adapter callback is allowed. Direct/Readonly are structurally identical because PIC has no host mutation; their bounded native observation remains an S23 exit requirement. Reject if the native PIC needs an unclassified product-shell dependency. |
| BX-MANTLE-083 | 2026-08-17: T225 S23 P5 proves that the source-derived original INT 06 operation transcript needs synchronous native byte-port execution, while bx-vdm must not obtain a Bochs object/callback | Project-authored `src/bx-mantle/bx_ntvdm_port_action_v1.h/.cc` and only the existing minimal-machine lifecycle gate | Expose versioned copied byte port read/write requests; accept only while the established minimal machine owns initialized native port space; delegate exactly to existing `bx_devices.inp/outp`; return only copied success/value. | Admitted before implementation. No selector/BOP/OpenNT/DOS term, device registration, firmware, callback, object pointer, generic device API, host capability or new Bochs source patch is permitted. The focused native lifecycle fixture must prove inactive rejection, native PIC dispatch, copied read result, and post-cleanup rejection. |
| BX-MANTLE-081 | 2026-08-15: T212 S4 maps cooperative CLI cancellation and owner authorizes only its selector-blind mechanical boundary | Project-owned `src/bx-mantle/bx_ntvdm_machine_stage_v1.*` and `bx_ntvdm_engine_*`; no `src/bx-core` source | Add mantle-private atomic cancellation state, a fixed `uint32_t` reason API, and one bounded existing CPU-thread timer poll that may set the existing `kill_bochs_request` latch. After normal stage/composition reset, map that distinct stage status to the pre-existing copied engine `HOST_CANCELLATION` terminal. | Registered before implementation. No Bochs-core edit, CLI/Win32 include or handle, callback/pointer, selector/BOP/OpenNT/DOS term, guest-memory/state inspection, device/firmware feature, direct asynchronous latch write, normal-completion producer, wall-time/resource policy or adapter route is allowed. A focused x64 `/MT` fixture must prove pre-entry/in-flight cancel, invalid reason, budget/typed-stop distinction and two-run cleanup. Reject/remove if a raw cross-thread Bochs field write, core edit, product shell or non-mechanical host semantics is needed. |
| BX-ABI-080 | 2026-08-14: owner authorizes registered Bochs intrusions and requires the adapter to define the real BOP boundary; T199 S37 rejects linking its whole-package lifecycle through the legacy runtime monolith | `refs/bochs/main.cc` | In the existing default-off execution-plan block, call one adapter-owned composition-lifecycle installer after the opaque execution-plan preparation. Bochs receives only absent/ready/rejected; it neither sees a selector, BOP, OpenNT/DOS term, image, host capability nor composition state. | Registered before implementation. The adapter lifecycle unit must own all profile/image/drive/volume capture and unbind its process-local state; the call must reject a declared malformed composition before the CPU loop. A focused MSVC x64 source/link closure and default-off source scan are required. |
| BX-MANTLE-079 | 2026-08-13: T199 S23 admits C2 as the second shared XMS/DPMI prerequisite; owner permits individually registered minimal Bochs intrusions | New project-authored `src/bx-mantle/bx_ntvdm_extended_memory_v1.*`; minimal lifecycle gate in `bx_ntvdm_minimal_machine.*`; declared source membership in the finite-machine link recipe | Expose a fixed-size, mantle-private allocation table over the already initialized native RAM aperture from 1 MiB through `get_memory_len`, with fixed-width query/allocate/free/resize/move records and existing checked ordinary-RAM copies. The pre-existing fixed records may use C linkage so a separately compiled adapter can call the same typed wrapper; no field, state or capability is added. | S24 governance remediation: the uncommitted prototype existed before this row; this row was entered before final implementation acceptance and commit. No selector/BOP/XMS/DPMI/OpenNT/DOS term, adapter state, host pointer, host allocator, device, firmware, interrupt, CPU decoding, or bx-core change is permitted. |
| BX-MANTLE-078 | 2026-08-13: T199 S21 establishes C1 as the first shared XMS/DPMI prerequisite; owner permits individually registered minimal Bochs intrusions | New project-authored `src/bx-mantle/bx_ntvdm_a20_capability_v1.*`; minimal lifecycle gate in `bx_ntvdm_minimal_machine.*` | Expose the existing native `bx_pc_system` A20 set/query primitives through one mantle-private fixed-width request/result wrapper after minimal-machine initialization. The pre-existing fixed records may use C linkage so a separately compiled adapter can call the same typed wrapper; no field, state or capability is added. | Registered before implementation. No selector/BOP/XMS/DPMI/OpenNT term, adapter state, allocator, device, firmware, interrupt, CPU decoding, or core source change is permitted. |
| BX-MANTLE-077 | 2026-08-13: S47's now-observed generic `#UD STOP` returns before the finite watchdog fires, exposing that the original timer API rejects direct unregistration of an active timer | `src/bx-mantle/bx_ntvdm_finite_run.cc` | Before unregistering the finite private stop timer after `cpu_loop` returns, deactivate it through the existing native PC-time API. | Registered before implementation. No CPU, BOP, OpenNT, adapter, guest-memory, device, firmware, product or ABI behavior is changed. Require early generic-STOP cleanup without a timer panic and retained watchdog HLT cleanup. Reject if cleanup needs a new timer primitive or changes production timer policy. |
| BX-CORE-076 | 2026-08-13: S46 neutral fixture independently reproduces a five-byte 16-bit code fetch at `CS:FFFF` advancing to `EIP=00010000`; owner permits necessary individually registered Bochs intrusions | `src/bx-core/cpu/cpu.h`, `cpu.cc`, `instr.h`, and `icache.cc` | Introduce one selector-blind instruction-pointer advance helper: long-64 code retains full RIP, 32-bit code retains existing EIP behavior, and 16-bit code truncates the incremented offset to 16 bits. Replace only existing sequential execution and boundary-fetch increments with that helper. | Registered before implementation. No instruction/opcode/address recognition, BOP/DOS/OpenNT/adapter term, guest-memory access, callback, device/firmware/product behavior, ABI or guest-state change beyond the native code-address-width rule. Full CPU5 x64 `/MT` closure, the positive neutral split fixture, HLT/UD2 regressions, and a changed-path source scan are required. Reject/remove if it needs a special-case input or changes control-transfer semantics. |
| BX-MANTLE-068 | 2026-08-13: T198 S8 requires the whole finite boot-namespace family to use one selector-blind mechanical action boundary; owner architecture requires `bx-vdm -> bx-mantle`, never the reverse | New project-owned `src/bx-mantle/bx_ntvdm_mechanical_action_v1.h` and `.cc` | Define and execute one synchronous, fixed-width copied ordinary-RAM action record: preflight every range, then perform either a bounded copied read, an all-or-nothing copied write, or a no-copy write-access preflight. The preflight kind may report only success/failure and must not read or modify a guest byte. It has no selector, provider, guest-image, host-capability, OpenNT or DOS field and no retained pointer/lifetime. | Registered before implementation. It may use only the existing Bochs ordinary-RAM primitives. A focused MSVC/x86 bare-machine fixture and source scan must prove valid read/write, malformed/out-of-aperture rejection, no partial write, no adapter dependency, and no product/device input. Review/remove if an action needs a callback, host pointer, CPU state mutation, device/firmware behavior, or VDM vocabulary. |
| BX-MANTLE-066 | 2026-08-13: T197 S6 r9 whole-core link proves that `pc_system.cc` combines CPU-reset/time mechanics with GUI/full-device teardown, while finite SIM fixes triple-fault reset false | `src/bx-mantle/pc_system.cc`; new `src/bx-mantle/minimal_product_shell.cc` | Move `bx_pc_system_c::Reset` and `exit` out of the common PC-time object; retain CPU-only reset mechanics and reject hardware reset requiring unadmitted devices; provide finite no-GUI parameter text/exit symbols only | Implemented; r11/r12 source-build and lifecycle witness prove no GUI/device archive is linked. |
| BX-MANTLE-067 | 2026-08-13: T198 S4 proves the retained `#UD` seam incorrectly mixes copied CPU mechanics with raw callbacks, adapter dispatch and product shutdown; S15 requires a fixture to distinguish bridge STOP from watchdog stop | `src/bx-core/cpu/exception.cc`; `src/bx-core/cpu/bx_ntvdm_exception_intercept.h`; current-path `src/bx-mantle` bridge/finite-run sources | Replace the default-off callback seam with one fixed, synchronous core-to-mantle copied-event/outcome bridge; core remains selector-blind and mantle maps controlled stop into its finite lifecycle. The private fixture may record only whether its own generic STOP response was accepted. For an external adapter bridge, the mantle retains those private fixture symbols while compiling out only its default-decline bridge definition. | Implemented and physically cleaned by T198 S5 P3; S23 adds only a selector-blind accepted-STOP observation bit so finite-run distinguishes it from its watchdog. S47 additionally raises the native async-event latch before its existing STOP longjmp, allowing the already-set finite stop latch to be observed by the original event loop. MSVC x64 CPU5 UD2 regression is required. No adapter runtime include, environment selection, transaction, BOP/OpenNT/DOS term, host pointer, product `SIM->quit_sim`, firmware/device/GUI behavior or CPU decode change remains in the core exception source. |
| BX-MANTLE-069 | 2026-08-13: T198 S24 P2 proves terminal IF=0 HLT has two source-distinct guest fatal paths but the finite fixture returns only an enum | `src/bx-mantle/bx_ntvdm_finite_run.*` | Default-off, mantle-private query returns a copied fixed-width terminal CS:IP after `cpu_loop` returns | Registered before implementation. It reads existing CPU state only after loop return; it performs no guest-memory read, CPU mutation, BOP decode, device/firmware action, callback, host pointer transfer, or core-source modification. A neutral HLT fixture and exact NTIO observation must prove zero/default absence and captured state respectively. |
| BX-CORE-070 | 2026-08-13: T198 S35 needs the ordinary instruction immediately preceding the already captured generic `#UD`; the S34 audit rejects existing instrumentation/debugger facilities as broad or pointer-bearing | attempted `src/bx-core/cpu/cpu.cc`; removed project-owned `src/bx-mantle/bx_ntvdm_instruction_history.*`; finite fixture/build recipe | Attempted default-off, selector-blind pre-execution copied record containing only version, CPU id, CS, RIP and sequence. | Rejected and physically removed: CPU5 enables handler chaining, so the original before-execution hook records a trace entry rather than every instruction. The successful x64 fixture's final entry was `0000:0AC1`, while the generic `#UD` remained `0000:0A84`; it cannot prove the required direct predecessor. See `t198-s35-chained-history-rejection-001.md`. |
| BX-CORE-071 | 2026-08-13: owner approved S36 after S35 proved the CPU-loop hook trace-granular; original chained `BX_EXECUTE_INSTRUCTION` is the narrow per-step seam | `src/bx-core/cpu/instr.h`; `src/bx-core/cpu/cpu.cc`; new project-owned `src/bx-mantle/bx_ntvdm_instruction_history.*`; finite fixture/build recipe | Default-off, selector-blind copied position record containing only version, CPU id, CS, RIP and native instruction count. `cpu.cc` records the first trace instruction; the original chained macro records subsequent executed instructions before their existing execution. Mantle owns a bounded copied ring. | Registered before implementation. No instruction-object inspection or retention, guest-memory read, BOP/service/OpenNT/DOS term, CPU-state change, debugger/instrumentation activation, device, firmware or product lifecycle is permitted. Entire CPU5 source closure must rebuild under MSVC x64 `/MT`; disabled, invalid-capacity, wrap/order and exact predecessor tests are required. Remove/reject if the result needs semantic decoding, pointers, broader feature activation or fails to prove the direct predecessor. |
| BX-MANTLE-072 | 2026-08-13: S37 leaves the first code-segment transition into the low-RAM frontier unproven; owner authorizes individually registered intrusive bx changes under strict review | `src/bx-mantle/bx_ntvdm_instruction_history.*`; T198 finite fixture bridge and fixture | When the already admitted default-off copied-position history is enabled, mantle may retain one fixed-width copied pair for the latest consecutive retained records whose `CS` values differ, and expose only a copied query. The fixture may snapshot that pair when its existing generic-stop observation fires. | Registered before implementation. No core source change, selector/service/BOP/OpenNT/DOS term, instruction bytes, guest-memory access, CPU mutation, callback, device/firmware/product behavior, host pointer or adapter interface is allowed. Require disabled/invalid/reset/order tests, exact CPU5 x64 `/MT` relink/run and changed-source boundary scan. Remove/reject if it requires semantic filtering, multiple event streams, wider state, or fails to retain the ordered copied pair. |
| BX-MANTLE-073 | 2026-08-13: S39 proves the finite NTIO fixture enters with undefined profile inputs; owner authorizes strict registered bx repair | `src/bx-mantle/bx_ntvdm_finite_run.*`; finite fixtures/build recipe | Add an optional copied existing mechanical-action record to the private finite request and execute it once after bare machine initialization, before existing preservation/image placement/CPU entry. Mantle validates and delegates only the opaque action; it has no profile, firmware, BOP, OpenNT, DOS or image branch. | Registered before implementation. No core change, new primitive, selector/service/profile/device term, raw pointer/callback, partial write, CPU mutation or lifecycle/device enablement is allowed. Missing/invalid action must reject before entry; source/order tests and exact CPU5 x64 `/MT` run are required. Remove/reject if it requires new action semantics or any special address/byte branch. |
| BX-CORE-074 | 2026-08-13: S40 leaves a selector-blind unexpected CS transfer; owner permits strict registered Bochs intrusion | `src/bx-core/cpu/instr.h`; existing mantle history and finite fixture | Add only copied SS selector and low 16-bit SP to the existing default-off position record at its original capture point. | Registered before implementation. No stack/guest-memory access, instruction decode, BOP/DOS/OpenNT term, CPU mutation, callback, device, firmware or product behavior. Rebuild full affected CPU5 closure and reject if scalar state is insufficient. |
| BX-CORE-087 | 2026-08-18: T228 P5 proves the generic indirect far-call target was already zero; the original operand is `SS:[BP+974Ah]`, but the accepted scalar record lacks BP | `src/bx-core/cpu/instr.h`; existing mantle history record and fixtures | Add only copied low 16-bit BP to the existing default-off instruction-position record at its original capture point. | Registered before implementation. No guest-memory access, address/opcode/selector/service filter, instruction interpretation, CPU mutation, callback, BOP/OpenNT/DOS term, device, firmware or product behavior. Full CPU5 x64 `/MT` default and explicit-provenance closures plus focused scalar and paired immutable evidence are required. Reject/remove if BP is insufficient or any non-scalar state is needed. |
| BX-MANTLE-088 | 2026-08-18: T228 S1 P9/P10 prove a whole guest-control-flow continuity boundary after the source-built COMMAND transient image and before zero-filled dynamic code; standing owner authorization permits a registered, evidence-backed mantle diagnostic | Project-owned src/bx-mantle/bx_ntvdm_instruction_history.* and bx_ntvdm_machine_stage_v1.* only | Default-off fixed ring of at most sixteen already copied scalar CS-transition pairs, retained solely when consecutive existing history records differ in CS and exposed only through a copied terminal diagnostic query. | Registered before implementation. No bx-core change, guest-RAM or instruction-byte read, instruction decode, address/selector/service/BOP/OpenNT/DOS/SoftPC term or filter, CPU/guest mutation, callback/pointer, adapter route, host capability, device/firmware/product behavior, outcome change, or non-fixed capacity is permitted. Disabled/default modes retain no output. Require disabled/order/wrap/reset fixture, source scan, fresh CPU5 x64 /MT explicit diagnostic and default formal graphs, paired immutable Direct/Readonly evidence, governance and diff checks. Reject/remove if scalar transition order is insufficient. |
| BX-MANTLE-075 | 2026-08-13: S41 proves the unknown CS transition cannot be classified from scalar positions; owner permits strict registered Bochs intrusion | `src/bx-mantle/bx_ntvdm_instruction_history.*`; finite fixture bridge | On an already enabled copied-record CS transition only, read fixed 15-byte windows at both the previous and current real-mode physical positions and a fixed 10-byte window at current real-mode SS:SP through existing ordinary-RAM copy. Retain one copied provenance record and expose a copied query. | Registered before implementation. **This is an independently opt-in provenance extension:** `BX-CORE-071`, `BX-MANTLE-072`, and `BX-CORE-074` remain scalar-only and must not include the memory header, address calculation, or ordinary-RAM copies unless the provenance build option is explicitly enabled. No core source change, address/opcode/selector/service filter, decoding, write, CPU mutation, pointer, callback, BOP/OpenNT/DOS term, adapter interface, device/firmware/product behavior or range control is permitted. The successor window is a fixed peer of the predecessor window, never a targeted image probe. Reads must fail closed outside ordinary RAM. Full CPU5 x64 `/MT` rebuild/run and source scan are required. |
| BX-MANTLE-085 | 2026-08-18: T228 P2 scalar terminal history proves a stable runtime span but cannot identify whether it is relocated code or data; owner permits a strictly bounded terminal provenance record | `src/bx-mantle/bx_ntvdm_machine_stage_v1.*`; explicit-provenance Ninja derivative and T228 fixture | Only after the existing watchdog has returned, and only when the explicit provenance build plus diagnostic option are enabled, copy fixed 15-byte ordinary-RAM window at current real-mode CS:IP and fixed 10-byte window at SS:SP into one copied terminal record. | Registered before test/run. Default and scalar-history graphs compile no terminal RAM capture. No CPU-loop alteration, decode, address/opcode/selector/service filter, write, CPU mutation, BOP/OpenNT/DOS term, adapter callback, pointer, device/firmware behavior, or outcome change is allowed. Reads fail closed outside ordinary RAM; the record may select only a complete owner domain, never a leaf repair. CPU5 x64 `/MT` derivative build, negative default/history checks, focused capture fixture, paired immutable Direct/Readonly evidence, source scan, governance checks and `git diff --check` are required. |
| BX-MANTLE-065 | 2026-08-13: T197 S6 whole-CPU5 link probe r8 reduces native core closure to the empty-port object lifetime and its original no-device dispatch methods | `refs/bochs/iodev/devices.cc`; `src/bx-mantle/minimal_port_space.cc` | Move exact `bx_devices_c` constructor, destructor, `init_stubs`, `inp` and `outp` definitions into the mantle empty-port source | Implemented; r9 symbol reduction and r12 lifecycle witness retain no device init/reset/exit or plugin loading. |
| BX-CORE-064 | 2026-08-13: T197 owner-directed physical core/mantle extraction; S6 object probe proves the relocated memory object requires the relocated CPU declaration | `src/bx-core/memory/misc_mem.cc` | Change only its direct CPU-header spelling from the former sibling path to `bx-core/cpu/cpu.h` after the complete CPU family moved under `bx-core` | Implemented and CPU5/MSVC/x86 object-verified; no memory, CPU, device, adapter or OpenNT behavior changes. |
| BX-MANTLE-063 | 2026-08-13: T197 S6 symbol audit proves the minimal machine needs `bx_devices` only for its already extracted empty-port members | `refs/bochs/iodev/devices.cc`; `src/bx-mantle/minimal_port_space.cc` | Move the single `bx_devices_c bx_devices` global definition to the mantle port-space object | Implemented; r12 initializes and cleans the mantle-owned empty port-space without device initialization. |
| BX-MANTLE-062 | 2026-08-12: owner directs `bx-core`/`bx-mantle` physical extraction, with mixed links decided individually; confirms CPU5/Pentium MMX bare-machine contract | `refs/bochs/gui/siminterface.h`; original `paramtree.*` and `logio.cc` moved to `src/bx-mantle/`; `bx_ntvdm_minimal_sim.*` | Mantle owns a finite original typed parameter tree for CPU5/Pentium MMX; SIM product behavior remains excluded | Implemented; r12 proves the fixed CPU5 typed parameter tree initializes without product SIM, parser, GUI/plugin or device path. |
| BX-UD-001 | 2026-08-10: permit the CPU `#UD` handling scheme as a Bochs-intrusion exception | `refs/bochs/cpu/exception.cc`; `refs/bochs/cpu/bx_ntvdm_exception_intercept.h` | A default-off, generic `#UD` interception and explicit resume disposition | Implemented and runtime-verified by r5 generic-#UD fixture. |
| BX-BUILD-002 | 2026-08-10: permit necessary, registered Bochs intrusive changes while avoiding them by default | `refs/bochs/config.cc` | One standards-conforming null-pointer comparison required by GCC 16 | Implemented and build/runtime-verified by r4. |
| BX-UD-003 | 2026-08-10: same exception authority | `refs/bochs/cpu/exception.cc`; reset fixture tool | Default-off test callback installation and generated generic #UD ROM | Implemented and runtime-verified by r5. |
| BX-ABI-004 | 2026-08-10: same exception authority | `refs/bochs/cpu/exception.cc`; `refs/bochs/Makefile.in`; native-container build script | Copy a generic exception record into adapter ABI v1 and link its pure-C pass-through consumer in the same UCRT island | Implemented and runtime-verified by r8. |
| BX-ABI-005 | 2026-08-10: same exception authority | `refs/bochs/cpu/exception.cc`; `refs/bochs/cpu/bx_ntvdm_exception_intercept.h`; `refs/bochs/Makefile.in`; native-container build script | Copy a fixed x86 register/selector snapshot into the existing generic #UD adapter event before pass-through | Implemented and runtime-verified by r9. |
| BX-ABI-006 | 2026-08-10: same exception authority; subsequent owner direction requires exact adapter-side boundary identification | `refs/bochs/cpu/exception.cc`; `refs/bochs/cpu/bx_ntvdm_exception_intercept.h`; `refs/bochs/Makefile.in`; native-container build script | Copy a bounded, same-page raw instruction-byte window into the existing generic #UD adapter event before pass-through | Approved for implementation; no BOP recognition or execution in Bochs. |
| BX-MEM-007 | 2026-08-10: same exception authority; 2026-08-10 holistic adapter host-service-plane approval; 2026-08-13 S15 x64 fetch audit | `refs/bochs/memory/memory.h`; `src/bx-core/memory/misc_mem.cc` | One generic, all-or-nothing copy into preflighted ordinary writable RAM. Each successful direct host-memory chunk performs the original page write-stamp invalidation before copying, matching native physical/DMA write cache coherency. | Implemented; no selector, service, payload interpretation, device fallback or adapter callback is admitted. |
| BX-MEM-010 | 2026-08-10: same exception authority; first guest-startup descriptor requires bounded firmware-state observation | `refs/bochs/memory/memory.h`; `refs/bochs/memory/misc_mem.cc` | One generic, all-or-nothing copy from preflighted ordinary readable RAM | Approved for implementation; no guest address, BOP, OpenNT, DOS, or host-service policy in Bochs. |
| BX-ABI-008 | 2026-08-10: same exception authority; 2026-08-10 holistic adapter host-service-plane approval | `refs/bochs/cpu/exception.cc`; `refs/bochs/Makefile.in`; native-container build script | Invoke the adapter's installed-session dispatcher and pass a same-boundary prepared generic RAM transaction to `BX-MEM-007` | Implemented; r12 explicit-link/runtime fixture revalidated after pending-operation tightening. |
| BX-MACH-009 | 2026-08-10: same exception authority; adapter-owned composition direction | None active | Investigated default-off bare-reset device-manager mode | Rejected by first reset trace; no source patch remains. |
| BX-EXEC-014 | 2026-08-10: same exception authority; r18 first-prefix input admission | `refs/bochs/main.cc`; `refs/bochs/cpu/*`; `refs/bochs/Makefile.in`; native-container build script | One default-off generic execution-plan consumer between original hardware initialization and the first CPU loop | Registered before implementation. |
| BX-EXEC-016 | 2026-08-10: same exception authority; 2026-08-13 S15 x64 UD2 observation proves the setter must invalidate native fetch state after changing CS:RIP | `refs/bochs/main.cc`; `src/bx-core/cpu/cpu.h`; `src/bx-core/cpu/init.cc` | A default-off real-mode entry setter changes only CS and RIP/IP, then invokes the original prefetch invalidation required by every native control transfer; it preserves the native post-reset CPU baseline | Registered before implementation. It must not change decoder, exception, memory, device, BOP, adapter, OpenNT or guest semantic policy. |
| BX-TRACE-017 | 2026-08-10: same exception authority; r20 control-flow audit requires a bounded accepted-resume record | `refs/bochs/cpu/exception.cc` | A default-off generic log of the already copied CPU snapshot and checked resume result | Registered before implementation. |
| BX-ABI-018 | 2026-08-10: same exception authority; `DemGetDrives` requires an exact carry-flag result | `refs/bochs/cpu/exception.cc`; `refs/bochs/cpu/bx_ntvdm_exception_intercept.h`; `refs/bochs/Makefile.in`; native-container build script | Apply a validated v2 result record: existing low-16-bit GPR delta plus an explicitly CF-only EFLAGS delta | Registered before implementation. |
| BX-ABI-019 | 2026-08-10: same exception authority; 2026-08-11 applies it to source-closed `SVC_DEMSETDTALOCATION` | `refs/bochs/cpu/exception.cc`; `refs/bochs/cpu/bx_ntvdm_exception_intercept.h`; `refs/bochs/Makefile.in`; native-container build script | One generic two-phase ordinary-RAM read: adapter requests a checked range, Bochs copies only ordinary RAM, adapter completes the same boundary with a checked result | Registered; narrow DTA use is admitted before implementation. |
| BX-MEM-020 | 2026-08-10: same exception authority; atomic adapter multi-write needs all writable ranges checked before the first write | `refs/bochs/memory/memory.h`; `refs/bochs/memory/misc_mem.cc` | Side-effect-free ordinary writable-RAM span preflight extracted from the existing single-write check | Implemented and runtime-verified by the r23 multi-write fixture. |
| BX-ABI-021 | 2026-08-10: same exception authority; source-derived multi-write transactions require one generic mechanical consumer | `refs/bochs/cpu/exception.cc`; `refs/bochs/cpu/bx_ntvdm_exception_intercept.h`; `refs/bochs/Makefile.in`; native-container build script | Take one copied-boundary opaque multi-write transaction, preflight every ordinary-RAM range, copy only after complete preflight, then apply the checked v2 CPU result | Implemented and runtime-verified by the r23 multi-write fixture; no service-specific branch in Bochs. |
| BX-ABI-022 | 2026-08-10: same exception authority; S5 source closure requires a current-PDB value in addition to a service buffer | `refs/bochs/cpu/exception.cc`; `refs/bochs/Makefile.in`; native-container build script | Before the legacy v3 consumer, offer one generic bounded gather action; a pass-through then reaches v3. A requested gather preflights every opaque ordinary-RAM range, copies them in declared order, then submits one same-boundary completion. | Registered before implementation; no selector, service, PDB, DOS, or host branch in Bochs. |
| BX-MACH-023 | 2026-08-10: owner authorizes individually registered Bochs intrusions and delegates adoption choice | `refs/bochs/bx_ntvdm_minimal_sim.h`; `refs/bochs/bx_ntvdm_minimal_sim.cc` | One private initializer that uses original SIM/parameter-tree types to create only `cpu.reset_on_triple_fault=false` | Implemented and source-boundary-verified; it preserves only the original shutdown branch, not a hard-reset path. |
| BX-MEM-024 | 2026-08-10: same exception authority; S6 source closure separates required reset-window memory allocation from product SIM/state registration | `refs/bochs/memory/memory.h`; `refs/bochs/memory/misc_mem.cc` | A private no-SIM memory initializer retaining only original vector/ROM/block/handler allocation and disabled PCI/SMRAM state | Implemented and source-boundary-verified; no state registration or product memory configuration. |
| BX-IO-025 | 2026-08-10: same exception authority; S6 generic CPU closure requires original unmapped I/O fallback without product device startup | `refs/bochs/iodev/iodev.h`; `refs/bochs/iodev/devices.cc` | Private initialize/cleanup helpers for original default I/O handlers and the two all-default port arrays | Implemented and source-boundary-verified; no timer, plugin, CMOS or device initialization. |
| BX-MACH-026 | 2026-08-10: same exception authority; S6 MSVC closure requires a non-product native lifecycle owner for registered private mechanics helpers | `refs/bochs/bx_ntvdm_minimal_machine.h`; `refs/bochs/bx_ntvdm_minimal_machine.cc`; `refs/bochs/memory/memory.h`; `refs/bochs/iodev/iodev.h` | One Bochs-local minimal-machine composition class, its required original global storage, and two narrowly scoped friendship declarations | Implemented and source-boundary-verified; no `main.cc`, product startup, build recipe, adapter, or OpenNT term. |
| BX-ABI-015 | 2026-08-10: same exception authority; r20 BOP `12h` contract requires an adapter-owned response path | `refs/bochs/cpu/exception.cc`; `refs/bochs/cpu/bx_ntvdm_exception_intercept.h`; `refs/bochs/Makefile.in`; native-container build script | Apply a validated fixed-width low-16-bit GPR delta from an already accepted generic adapter exception result before existing resume | Registered; implementation pending focused ABI and negative tests. |
| BX-ABI-027 | 2026-08-11: owner rule permits this necessary S6 boundary repair after matrix-002 identifies an unconditional adapter header in the minimal CPU candidate | `refs/bochs/cpu/exception.cc` | Default-off compile-time isolation of the existing #UD interception/adapter seam, with one named explicit opt-in macro | Implemented and source-boundary-verified; no build/runtime claim. |
| BX-ABI-028 | 2026-08-11: owner rule permits this necessary S6 modern-MSVC lexical compatibility repair after projected syntax observation 002 | Seven `refs/bochs` source files with direct string-literal/`FMT_LL` adjacency | Add whitespace only to preserve original macro expansion at the observed C++ literal-token boundary | Registered before implementation; no format, feature, or runtime behavior change. |
| BX-EXEC-032 | 2026-08-11: S6 intact-native detached build proves the existing registered execution-plan path is not default-off at compile/link boundary | `refs/bochs/main.cc`; `refs/bochs/Makefile.in` | Put the existing adapter-backed startup-plan include/call/object list behind one named compile-time opt-in, so the default native container has no adapter header/object requirement | Implemented and `/Zs`-verified; no BOP/DOS/DEM/OpenNT behavior change. |
| BX-BUILD-033 | 2026-08-11: S6 r3 intact native build proves a prior source-object split lacks its original generated archive membership | `refs/bochs/iodev/Makefile.in`; generated `refs/bochs/iodev/Makefile` | Add the existing `minimal_port_space.o` to the original non-plugin I/O archive membership | Implemented and r4 native-link verified; no runtime profile, device, adapter, or OpenNT behavior change. |
| BX-IO-029 | 2026-08-11: coordinator selects the lowest-risk S6 retained-owner candidate | `refs/bochs/iodev/devices.cc`; one new Bochs-internal `refs/bochs/iodev/minimal_port_space.cc` | Source-object separation of the complete already registered empty-port lifecycle: initializer, default handlers, registration helpers and cleanup | Registered before implementation; no ABI, behavior, feature, lifecycle, or runtime change. |
| BX-IO-030 | 2026-08-11: S6 single-object observation proves the isolated empty-port initializer still imports only its original default-handler registration members from the rejected full device object | `refs/bochs/iodev/devices.cc`; existing Bochs-internal `refs/bochs/iodev/minimal_port_space.cc` | Source-object separation of the two original default I/O-handler registration members used exclusively by the isolated empty-port initializer | Registered before implementation; no ABI, behavior, feature, lifecycle, or runtime change. |
| BX-EXEC-034 | 2026-08-11: S6 static object inventory proves a narrow same-island startup-session has no broad runtime or Bochs edge | `refs/bochs/main.cc` | In the existing default-off execution-plan block, replace the broad adapter-runtime preparation call with the adapter-owned narrow startup-session endpoint | Registered before implementation; default target remains adapter-free and no Bochs semantic branch is added. |
| BX-TRACE-035 | 2026-08-11: owner permits registered minimum Bochs intrusions; S6 x87 correlation requires a native-state discriminator without guest mutation | `refs/bochs/fpu/fpu.cc` | Default-off fixed diagnostic snapshot immediately before the existing x87 compatibility IRQ13 delivery | Removed by T196 S3 after its historical correlation use; no source, test, generator or build option remains. |
| BX-ABI-036 | 2026-08-11: owner permits individually registered Bochs intrusions; S6 closure rejects the broad runtime for the first startup companion | `refs/bochs/cpu/exception.cc`; `refs/bochs/main.cc` | One default-off generic #UD/write/resume path connected only to the adapter-owned startup transaction bridge | Registered before implementation; no marker/service decode or host semantics in Bochs. |
| BX-ABI-037 | 2026-08-11: owner-approved registered Bochs intrusions; reached `BOP_UNSIMULATE` requires terminal lifecycle expression | `refs/bochs/cpu/exception.cc`; `refs/bochs/cpu/bx_ntvdm_exception_intercept.h` | Default-off generic exception stop disposition that ends the native CPU loop without decoding BOP/OpenNT terms or resuming guest execution | Registered; implementation requires separate focused admission. |
| BX-TRACE-037 | 2026-08-11: r6 fault-state evidence leaves only the original IRQ13 transfer as the next bounded provenance boundary | `refs/bochs/cpu/event.cc` | Default-off, IRQ13-only before/after local CS:EIP snapshot around the existing external-interrupt transfer | Removed by T196 S3 after its historical correlation use; no source, test, generator or build option remains. |
| BX-TRACE-038 | 2026-08-11: r8 proves IRQ13 is raised but not acknowledged; delivery eligibility remains unknown | `refs/bochs/fpu/fpu.cc` | Extend the existing default-off x87 snapshot with current EFLAGS and IF before the unchanged IRQ13 raise | Removed by T196 S3 with BX-TRACE-035; no source, test, generator or build option remains. |
| BX-TRACE-039 | 2026-08-11: source correlation identifies `BOP 12h` as the first required post-`50:11` operation, but r10 has no unmatched-#UD position record | `refs/bochs/cpu/exception.cc` | Default-off log of existing CPU id, CS selector and `prev_rip` only after the narrow startup transaction declines a `#UD` | Removed by T196 S3 after its historical correlation use; no source, test, generator or build option remains. |
| BX-ABI-040 | 2026-08-11: owner requires complete OpenNT BOP inventory, adapter-side connection, and observation before per-BOP implementation | superseded Bochs listener; `src/bx-ntvdm-adapter/bx_ntvdm_exception_observer_v1.c` | Adapter-owned copied-fact BOP catalogue observation; Bochs supplies only generic copied exception delivery and opaque diagnostic output | T195 S3 migrated and source/build-verified. No selector/service identity remains in Bochs; all provider entries retain their prior disposition. |
| BX-ABI-041 | 2026-08-11: S7 `DEMREAD` requires the already-declared one-range bulk result to reach ordinary guest RAM without a service-specific Bochs path | `refs/bochs/cpu/exception.cc`; `refs/bochs/cpu/bx_ntvdm_exception_intercept.h` | Take one opaque copied-boundary bulk result, validate the existing result/resume relation, preflight its full ordinary-RAM range, copy once only when nonempty, then apply the already prepared generic result | Registered before implementation. It must not inspect selector, service, path, token, payload contents, DOS/OpenNT terms, or add a device/memory model. |
| BX-ABI-042 | 2026-08-11: S7 requires the already admitted v2 namespace and BOP runtime to share the same verified NTIO startup session | `refs/bochs/main.cc` | In the existing default-off generic execution-plan consumer only, exchange the obsolete narrow startup-transaction preparation endpoint for adapter runtime's pointer-free `prepare_execution_plan_from_environment`; preserve the plan ABI, RAM preflight/copies and real-mode entry sequence unchanged | Registered before implementation. Bochs receives only ready/absent/rejected and opaque plan/payload records; no BOP, DOS, COMMAND, path, host handle or OpenNT branch is added. |
| BX-ABI-053 | 2026-08-12: owner authorizes individually registered Bochs intrusions; T115/T116 establish a separate machine-composition boundary and a finite original-handler ABI | `refs/bochs/cpu/exception.cc`; `src/bx-ntvdm-adapter/bx_ntvdm_machine_composition_v2.*`; named native derivative build script | Default-off generic engine-to-adapter mechanics callback with copied facts and a transient checked port/RAM/stack/report capability | T195 S4 migrated and source/build-verified. Bochs has no selector/handler selection; adapter owns historical machine composition. |
| BX-ABI-054 | 2026-08-12: T118 source-proves BOP `02h` consumes three bytes then enters guest IRET continuation | `src/bx-ntvdm-adapter/bx_ntvdm_machine_composition_v2.c`; named native derivative build script | Adapter-owned conversion of a selected original handler completion to generic typed resume at the copied fault RIP plus three | T195 S4 migrated and source-built. Bochs receives and mechanically applies only `cpu_result_v2`; runtime remains unclaimed. |
| BX-TRACE-055 | 2026-08-12: T130 reaches the admitted machine-composition path but its boolean result cannot distinguish decline from failed native mechanics | `src/bx-ntvdm-adapter/bx_ntvdm_machine_composition_v2.c`; component tests | Adapter returns generic pass-through, resume, or controlled stop; no Bochs-side outcome trace or handler-specific enum remains | T195 S4 migrated and source/build-verified. Failed checked mechanics select controlled stop rather than a fabricated resume. |
| BX-TRACE-056 | 2026-08-12: T134 proves a repeated real-mode prefetch-limit fault but not the instruction that advanced into it | `refs/bochs/cpu/cpu.cc`; a separately named derivative shim/object | Default-off, fault-only record at the original `EIP > CS.limit` branch of real mode: current CS, `PREV_RIP`, current EIP and the already-read limit | Removed by T196 S3 after its historical correlation use; no source, test, generator or build option remains. |
| BX-TRACE-043 | 2026-08-11: trace 007 reaches `DEMIOCTL`, whose historical dispatcher selects solely from AL while the existing identity listener omits all call inputs | `src/bx-ntvdm-adapter/bx_ntvdm_exception_observer_v1.c` | Adapter-owned copied EAX/EBX observation fields on the BOP identity record | T195 S3 migrated. The observer is copied-fact-only and cannot change a result. |
| BX-TRACE-044 | 2026-08-11: r12 reaches `5F`; its historical entry selects an NTIO table through copied register state, but the generic listener currently logs only AX/BX | `src/bx-ntvdm-adapter/bx_ntvdm_exception_observer_v1.c` | Adapter-owned copied CS/DS/SI/DI/CX/EFLAGS observation fields | T195 S3 migrated. No selector/service formatting remains in Bochs. |
| BX-TRACE-045 | 2026-08-11: r14 reaches guest INT 6 BOP `06`; OpenNT's original illegal-op handler derives the pre-vector CS:IP from the real-mode SS:SP frame | superseded Bochs stack/frame listener diagnostic | Removed in T195 S3 rather than recreate an unapproved guest-RAM observation path | Retained historical evidence only; future stack inspection requires an independently admitted bounded generic read contract. |
| BX-TRACE-046 | 2026-08-11: r22 records a post-close stack fault, but the generic BOP log omits the existing fault position needed to correlate the close call with source | `src/bx-ntvdm-adapter/bx_ntvdm_exception_observer_v1.c` | Adapter-owned copied `fault_rip` field on the BOP identity record | T195 S3 migrated; observer remains result-free and copied-fact-only. |
| BX-TRACE-047 | 2026-08-11: r24 proves the `50:02` resume preserves its source-defined registers, yet later reaches a real-mode `#SS` with `SP=0003`; the first ordinary instruction that changes the stack pointer is unrecorded | attempted `refs/bochs/cpu/cpu.cc`; focused static boundary test | Rejected. The attempted default-off post-instruction stack-pointer logger required a non-chained CPU-loop build while the retained CPU archive was chained. r28 no longer reached a BOP (`0` observations/commits), and Makefile rebuilt the CPU archive. | Removed after r28. No source or shim patch remains. Retain `t95-s7-stack-pointer-diagnostic-rejection-001.md`; future tracing must use an original same-configuration facility or a separately proven full CPU object closure. |
| BX-TRACE-048 | 2026-08-11: r30 proves the read ABI correction reaches `61A7`, but the next ordinary `LeaveDos` stack transfer remains unobserved; native debugger/GDB/instrumentation are disabled and debugger is incompatible with chained handlers | `refs/bochs/cpu/data_xfer16.cc`; focused static boundary test | Default-off, real-mode-only logs within the existing `MOV_SwEw` and `MOV_GwEwM` handlers when their destination is SS or 16-bit SP. Each record contains only current CS:RIP and old/new register/selector values. | Removed by T196 S3 after its historical correlation use; no source, test, generator or build option remains. |
| BX-TRACE-049 | 2026-08-11: r34 source correlation establishes that the terminal state enters `Redisp` through original NTDOS stack switching, but the low-memory vector that reaches `0000:019D` is unproven | `refs/bochs/cpu/exception.cc`; focused static boundary test | Default-off, real-mode-only log in the original `real_mode_int` path after its existing IVT word reads. It records vector, pre-delivery CS:IP/SS:SP, and the already-read IVT target CS:IP. | Removed by T196 S3 after its historical correlation use; no source, test, generator or build option remains. |
| BX-TRACE-050 | 2026-08-11: r35 proves `INT 1` reaches NTIO `intret`, which merely IRETs a TF-bearing frame; the origin frame remains unknown | `refs/bochs/cpu/ctrl_xfer16.cc` | Default-off, real-mode-only log after original `IRET16` pops IP, CS and FLAGS, only if that already-popped FLAGS word contains TF. It records pre-IRET CS:IP/SS:SP and the three popped words. | Removed by T196 S3 after its historical correlation use; no source, test, generator or build option remains. |
| BX-TRACE-051 | 2026-08-11: r36 source correlation proves the first TF frame follows original guest pseudo-ROM INT10, but does not expose the saved machine INT10 far-jump target | `refs/bochs/cpu/ctrl_xfer32.cc` | Default-off, real-mode-only log in original immediate far-jump (`JMP_Ap`) after its existing operand decode and before its original CS:EIP assignment. It records current CS:IP and decoded target CS:IP. | Removed by T196 S3 after its historical correlation use; no source, test, generator or build option remains. |
| BX-ABI-052 | 2026-08-11: r37 native firmware-to-NTIO design requires the historical direct entry to occur only after native Bochs POST | `refs/bochs/main.cc`; `refs/bochs/cpu/exception.cc`; existing adapter startup-plan headers/source; native build shim | One default-off, opaque, single-use startup-plan handoff: initialize an adapter session before the CPU loop without applying its plan; at the existing generic `#UD` seam, take one already validated plan, use existing ordinary-RAM primitives to capture/restore its declared bytes and write its payload, then use the existing real-mode CS:RIP setter before leaving the CPU loop. | Registered before implementation. Bochs may not contain marker/ROM/address/firmware/BOP/OpenNT/DOS/WOW/DEM/COMMAND/service/host terms or a selector/device branch. It must not implement an interrupt, synthesize IVT/BDA, alter native POST/device semantics, accept a second take, or expose a partial result. The adapter alone validates the selected composition marker; unmatched events use the original exception path. |
| BX-ABI-057 | 2026-08-12: owner permits recorded Bochs intrusions; T167 proves textconfig converts every CPU-loop return to historical exit 1 | `refs/bochs/cpu/exception.cc` | In the existing generic accepted-STOP branch only, invoke original `SIM->quit_sim(0)` instead of returning to textconfig; all non-STOP and declined results retain their original flow. | Registered before implementation. Bochs must not inspect selector, service, BOP bytes, guest state, OpenNT/DOS terms, or adapter-private data; the bridge remains the sole STOP decision owner. |
| BX-ABI-058 | 2026-08-12: T176 S21 proves original SoftPC machine BOP `06h` (`illegal_op_int`) needs only copied real-mode SS:ESP, checked ordinary-RAM byte reads/writes, and its original report-then-continue host outcome before it calls the already admitted `unexpected_int` island | `src/bx-ntvdm-adapter/bx_ntvdm_machine_composition_v2.*`; named native derivative build script | Adapter-owned `06h` selection invokes unchanged `illegal_op_int` through checked execution-mode/SS/ESP/RAM/report mechanics and returns only generic result | T195 S4 migrated and source-built. Bochs has no `06h`/SoftPC/CCPU semantics; native failed mechanics produce controlled stop and all other selectors pass through. |
| BX-TRACE-059 | 2026-08-12: T186 records a repeated boot-file BOP but omits DX, which is required to evaluate its already-source-defined `DS:DX` destination | `src/bx-ntvdm-adapter/bx_ntvdm_exception_observer_v1.c` | Adapter-owned copied low EDX field on the BOP identity record | T195 S3 migrated; no Bochs listener macro remains. |
| BX-TRACE-060 | 2026-08-12: T188 S1--S4 proves the generic adapter ingress already receives `54:0C`, static v5 inputs succeed, and only private accumulated session state remains unclassified | `refs/bochs/cpu/exception.cc`; focused state diagnostic | Default-off generic adapter lifecycle diagnostic after pass-through | Removed by T196 S4: lifecycle diagnostics belong to adapter, and no Bochs source/test/generator flag remains. |
| BX-TRACE-061 | 2026-08-12: T189/S1 reduces the real `54:0C` decline to a finite adapter compound-chain uncertainty | `refs/bochs/cpu/exception.cc`; existing copied diagnostic state | Default-off generic first-failure category after pass-through | Removed by T196 S4 with BX-TRACE-060; no Bochs source/test/generator flag remains. |

### BX-ABI-037: Generic controlled CPU-loop stop

**Need.** The source-led FE audit establishes a non-returning guest lifecycle
event. A resume result would be false, while `SIM->quit_sim()` is not the
native CPU-loop return path.

**Behavior.** Add one default-off generic exception response disposition
`STOP`. Only after the adapter has already returned a valid zero-effect typed
STOP result, the Bochs seam sets its existing `async_event` and
`kill_bochs_request`, then uses its existing exception `longjmp`. The native
CPU loop's original event check returns to its existing caller.

**Boundary.** The Bochs files name no BOP, selector, service, OpenNT, DOS,
host, or lifecycle reason. They inspect no instruction byte, guest memory or
adapter-private record beyond the typed response, and do not modify RIP,
registers, flags, memory, firmware, device state or configuration. Exact
event recognition is adapter-only.

**Negative cases and verification.** Any invalid result, pass-through result,
resume result, or adapter decline retains the pre-existing path. The adapter
rejects wrong vector, execution mode, bytes, or window length. The generic
seam's lexical boundary test passed. r47's named MSVC/x86 target compiled and
linked the seam, but its bounded trace reached an earlier stack fault before
the terminal event; runtime stop remains unproven.

**Review condition.** Remove or reject this exception if it requires a
selector-specific Bochs branch, a second CPU loop, `quit_sim`, a monitor,
guest-state mutation, or any enabled device. Revisit only after a trace
actually reaches the adapter's typed terminal result.

### BX-IO-029: Empty-Port Source-Object Separation

**Need.** The minimal composition calls only
`bx_devices_c::init_empty_port_space`, but its current definition shares
`iodev/devices.cc` with full PC/device/plugin members. The CPU3 retention
trace consequently retains rejected product-owner edges from the common
`devices.obj`. The lowest-risk candidate isolates the already registered
empty-port behavior into one original Bochs-internal translation unit.

**Behavior.** Move exactly the existing definitions of
`bx_devices_c::init_empty_port_space`,
`bx_devices_c::default_read_handler`, and
`bx_devices_c::default_write_handler`,
`bx_devices_c::register_default_io_read_handler`,
`bx_devices_c::register_default_io_write_handler`, and
`bx_devices_c::cleanup_empty_port_space` from `iodev/devices.cc` to
`iodev/minimal_port_space.cc`. Their signatures, bodies, defaults, and class
ownership remain unchanged; no declaration, caller, or lifecycle ordering
changes.

**Boundary.** This is a source-object boundary only. It adds no adapter,
OpenNT, DOS/WOW/DEM, BOP, host, configuration, feature-flag, plugin, device,
timer, firmware, or execution behavior. It does not alter `iodev.h`, ABI,
port defaults, handler registration, cleanup semantics, or full-device
initialization.

**Negative cases and verification.** A focused static test must require each
of the six moved definitions exactly once in the new source and absent from
`devices.cc`; it must reject adapter terms in the new source. Existing FMT
lexical and default-off/retained #UD tests must still pass. The package must
not compile, link, run, change a build recipe, or claim object/link closure.

**Review condition.** Reject this exception if the move requires any
declaration/API/call-site/semantic change, alters cleanup semantics or a
 default-handler result, reaches an adapter, or requires a further
 source-object split.
Revisit after a separately admitted object/link observation establishes
whether this isolated member removes the predicted rejected-owner retention.

### BX-IO-030: Empty-Port Default-Handler Registration Separation

**Need.** The permitted CPU3 compile-only observation for `BX-IO-029` proves
that the isolated initializer has exactly two remaining Bochs-local imports:
`register_default_io_read_handler` and `register_default_io_write_handler`.
Their only source definitions remain in the rejected full `devices.cc` object.
They write the existing default-handler record used by the isolated empty-port
initializer; neither function initializes a device or plugin.

**Behavior.** Move exactly the existing definitions of
`bx_devices_c::register_default_io_read_handler(...)` and
`bx_devices_c::register_default_io_write_handler(...)` from `iodev/devices.cc`
to the existing `iodev/minimal_port_space.cc`. Their signatures, bodies,
allocation/deallocation, string copy, return value, ownership and invocation
order remain unchanged. No declaration, caller, lifecycle or cleanup changes.

**Boundary.** This is only a Bochs-internal source-object boundary. It adds no
adapter, OpenNT, DOS/WOW/DEM, BOP, host, configuration, feature, plugin,
device, timer, firmware, execution or CRT policy. It does not remove a
default handler or change an unmapped I/O result.

**Negative cases and verification.** Extend the focused static boundary test
to require each registration definition exactly once in
`minimal_port_space.cc` and zero times in `devices.cc`, alongside the three
already moved members. It must reject adapter/OpenNT references and preserve
the original cleanup definition in `devices.cc`. Retained FMT lexical and
default-off/retained #UD tests, documentation governance and safe diff checks
must pass. No compile, link, execution, build recipe, project/configure,
feature or device action is admitted by this exception.

**Review condition.** Reject this exception if the move needs a declaration,
API, caller, allocation, cleanup, default-result or semantic change, or if it
does not eliminate both object-local non-CRT Bochs imports. Revisit only after
a separately admitted compile/object observation; it does not authorize a
link closure claim.

### BX-ABI-027: Default-Off Existing #UD Interception Compilation Boundary

**Need.** The prior #UD seam is intentionally retained for separately built
same-island adapter evidence, but `cpu/exception.cc` currently includes its
adapter ABI headers and compiles the seam unconditionally.  Matrix-002 thereby
forces the Bochs-local minimal CPU candidate to depend on
`src/bx-ntvdm-adapter` before it can establish its own configuration shape.

**Behavior.** In `refs/bochs/cpu/exception.cc` only,
`BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT` defaults to `0`.  The pre-existing
adapter includes, private #UD interception helpers/state, #UD snapshot/window
capture, and accepted-resume application are compiled only when the macro is
nonzero.  A separately declared build may opt in with the exact macro.  With
the default, the original Bochs exception flow continues directly from
`BX_INSTR_EXCEPTION` to its existing debugger/log/exception delivery code.

**Boundary.** This changes compilation reachability only.  It does not remove
or alter the registered #UD seam when explicitly enabled; add an adapter,
guest, BOP, DOS/WOW/DEM, OpenNT, service, decoder, memory, device, firmware,
or configuration semantic.  No adapter header, type, global, function, or
call may remain in the compiled default CPU path.

**Negative cases and verification.** A focused static test must require the
macro's local default and a single opt-in guard covering every adapter include
and the exception-body seam.  It must fail if any `bx_ntvdm_*` include/type/
symbol is outside that guard, if the default changes to enabled, or if the
original post-`BX_INSTR_EXCEPTION` exception flow is replaced.  `LOG_THIS` is
an original logging-context definition and must remain outside the guard. The existing
#UD boundary test must continue to find the retained seam text.

**Review condition.** Reject this exception if it requires a second source
file, a broad/generated configuration default, a new adapter ABI, a runtime
switch, or any execution-semantic change.  Revisit/remove it when an approved
minimal recipe can express the same exclusion without a source guard.

**Evidence.** `tests/bx-ntvdm-adapter/Test-BochsUdDefaultOffBoundary.ps1`
proves the local default, guarded adapter includes and guarded exception-body
seam. `Test-BochsUdInterceptBoundary.ps1` preserves the existing seam record.
Neither test compiles, links, or runs Bochs.

### BX-ABI-028: `FMT_LL` String-Literal Lexical Compatibility

**Need.** Projected syntax observation 002 reaches original `osdep.h:59`, which
correctly supplies the MSVC `FMT_LL` format macro, but modern MSVC lexes a
string literal immediately followed by `FMT_LL` (or `FMT_LL` immediately
followed by a string literal) as a user-literal form before the intended macro
concatenation can occur. The result is C3688, independent of devices, adapter,
or configuration feature selection.

**Behavior.** Only the 14 directly adjacent occurrences in
`gui/paramtree.cc` (2), `gui/siminterface.cc` (2),
`iodev/hdimage/hdimage.cc` (1), `iodev/usb/scsi_device.cc` (3),
`iodev/harddrv.cc` (2), `iodev/pit.cc` (3), and
`iodev/virt_timer.cc` (1) may receive lexical whitespace, e.g.
`"0x" FMT_LL "x"`. The literal contents, macro name/definition, argument
order, format conversion text, numeric behavior, and all non-adjacent
formatting remain unchanged.

**Negative cases and verification.** A focused static test must scan every
imported `*.cc`/`*.h` source for both direct adjacency forms and fail on any
remaining occurrence. It must require the expected spaced forms in all seven
registered files, and reject any `FMT_LL` macro-definition modification or
change outside the listed files.

**Review condition.** Reject this exception if it requires a config/macro
change, a new feature, adapter/guest term, runtime behavior change, or any
source file beyond the seven listed. Revisit if a supported historical MSVC
mode proves the unspaced spelling valid without a source compatibility patch.

### BX-ABI-022: Generic Gather Read

**Need.** S5 must obtain a bounded direct service buffer and the separately
located current-PDB word at one copied #UD boundary. Bochs must not know why
two ranges are present.

**Behavior.** Bochs offers the validated generic gather record before the
legacy v3 one-range consumer.  A gather pass-through is the only path that
reaches v3, so a declared gather trigger cannot be consumed first by an older
generic observation trigger.  It preflights every opaque ordinary-RAM range
before copying any byte, copies ranges in declared order into the adapter-owned
contiguous arena, then calls the matching same-boundary completion. The
returned result is applied only after completion succeeds.

**Negative cases.** Absent, invalid, zero-length, unreadable, mismatched, or
failed-copy ranges leave the original exception path intact and expose no
partial completion. The Bochs block contains no range-address/content branch,
selector, BOP, service, PDB, OpenNT, DOS/WOW/DEM, profile, or host term.

**Review condition.** Reject this exception if it acquires any interpretation
of range bytes, accepts unbounded/scatter I/O, bypasses all-range preflight,
or becomes a path around adapter-owned completion.

### BX-MACH-023: Minimal Original SIM Parameter for Triple-Fault Shutdown

**Need.** Generic CPU exception code unconditionally dereferences
`SIM->get_param_bool(BXPN_RESET_ON_TRIPLE_FAULT)` before selecting its
existing triple-fault branch.  The M0 profile needs that original lookup to be
valid without importing the product configuration, GUI, plugin, or device
startup graph.

**Behavior.** A private Bochs-local initializer may call the original
`bx_init_siminterface()`, obtain its original root parameter list, and create
only the original `cpu` list and the original
`BXPN_RESET_ON_TRIPLE_FAULT` `bx_param_bool_c` with value `false`.  It exports
no adapter ABI and accepts no CLI, guest, host, BOP, DOS/WOW/DEM, OpenNT, or
profile input.  It must reject duplicate/missing/type-mismatched tree state
before CPU entry.

This preserves the original `exception.cc` false branch: panic/shutdown and
its normal decode-loop `longjmp`.  It does **not** read CMOS, call
`bx_pc_system.Reset`, reset devices, or claim a bounded M0 stop result.  A
future controlled-stop boundary is separate execution work.

**Explicit exclusion.** The `true` branch is not admitted.  It reads
`DEV_cmos_get_reg(0x0f)` and invokes hardware reset, which calls
`DEV_reset_devices`.  The constructor-installed CMOS stub is not a substitute:
its operation is a plugin panic.  Any real CMOS/device-reset closure therefore
requires a separately registered exception, source-to-object audit, and
negative tests.

**Negative cases.** The initializer must not call `config.cc` parameter
creation, GUI/text configuration, plugin loading, device initialization,
timer startup, or adapter/OpenNT code.  No parameter may be changed from
false through this helper.  A missing or wrong parameter type must fail before
CPU execution rather than being repaired dynamically.

**Review condition.** Reject or split this exception if it creates a second
parameter, exposes a mutable configuration API, reaches CMOS/devices, or
becomes a hidden product-startup path.  Remove it if a later native Bochs
configuration expresses the same one-parameter invariant without a project
helper.

### BX-MEM-024: No-SIM Reset-Window Memory Initialization

**Need.** The original `BX_MEM_C::init_memory` contains the M0-required
vector, ROM, block-table and memory-handler allocation, but its tail queries
the I440FX parameter via `SIM` and registers product state.  M0 must preserve
the original storage representation without importing either operation.

**Behavior.** A private `BX_MEM_C` helper may retain only the original
allocation/initialization prefix: vector/ROM/bogus setup, ROM fill, empty
block pointers, and empty memory-handler table.  It must set `pci_enabled`
and all SMRAM state false and return before `SIM`,
`BXPN_I440FX_SUPPORT`, or `register_state`.  It is one-shot: pre-existing
allocation is an explicit failure, not a repair/reuse path.  Original
`cleanup_memory` remains the only permitted cleanup owner.

**Negative cases.** The helper must not load a ROM file, allocate a PCI/VGA
mapping, register GUI/configuration state, create an adapter/guest mapping,
or accept a host pointer.  Its focused static/lifecycle test must prove
disabled PCI/SMRAM state and reject a second initialization.

**Review condition.** Reject this exception if it gains a configuration
parameter, state-tree operation, device policy, non-reset memory source,
OpenNT/DOS/BOP meaning, or a special mapping for a consumer.

### BX-IO-025: Empty Original Port-Space Initialization

**Need.** Generic CPU I/O handlers remain part of the Bochs decoder closure.
They require initialized port-handler arrays even when M0 exposes no device.
The original `bx_devices_c::init` supplies the needed default handlers and
arrays, then enters timers, plugins, CMOS and device startup.

**Behavior.** Private `bx_devices_c` initialize/cleanup helpers may retain
only the original default read/write handler registration, self-linked
sentinels, two `PORTS` arrays initialized to those sentinels, and matched
cleanup of these owned arrays/names.  Reads retain the original all-ones
unmapped value and writes remain ignored.  Cleanup must fail rather than
delete/reset a non-default handler.

**Negative cases.** The helpers must not call `SIM`, `bx_virt_timer`,
`bx_slowdown_timer`, plugin loaders, CMOS, device initialization, IRQ setup,
port 92 reset, GUI, adapter, guest or OpenNT code.  The focused test must
prove every port begins at the default handler and no non-default ownership is
silently removed.

**Review condition.** Reject this exception if it adds any concrete device,
timer, plugin, I/O policy, guest marker, host callback, or service term.

### BX-MACH-026: Minimal Native Machine Composition

**Need.** `main.cc` owns Bochs's historical global machine storage and product
startup.  The registered minimal SIM, memory and empty-port helpers are
private and intentionally have no caller.  A fixture cannot link `main.cc`
without re-admitting product configuration, firmware loading, devices and
plugins, and it cannot define the same globals alongside it.

**Behavior.** A new Bochs-local C++ composition class may define only the
original global storage needed by its selected non-SMP configuration:
`pluginlog`, `bx_pc_system`, `bx_dbg`, `bx_cpu`, `bx_mem`, and `bx_user_quit`
where their referenced original objects require them.  It may receive private
friend access only to call the already registered minimal memory and empty
port-space helpers.  Its one-shot initialization order is: original logging
accessors; minimal false triple-fault parameter; no-SIM memory; empty port
space; original CPU decoder initialization; original hardware CPU reset.  It
does not call `main.cc`, `bx_pc_system_c::initialize`,
`bx_pc_system_c::Reset`, a CPU loop, a firmware loader, product configuration,
or device/plugin startup.  Cleanup reverses only the owned empty-port and
memory state; SIM parameter storage lasts for the fixture process lifetime.

**Negative cases.** No public C ABI, adapter callback, host pointer, CLI
argument, guest payload, BOP, DOS/WOW/DEM/OpenNT term, device reset, timer,
CMOS, GUI, plugin or ROM-file path may enter the class.  Failure at any stage
must stop before CPU entry and clean up only earlier owned stages; it must not
fall back to product cleanup or partially repair external state.

**Review condition.** Reject this exception if it grows into an emulator
front end, defines execution-policy semantics, exposes CPU/memory/device
pointers, changes CPU decode/exception semantics, adds another global feature,
or substitutes for a separately versioned adapter bridge.  The composition
unit alone does not admit a configuration header, object list, linker recipe
or executable build.

### BX-ABI-015: Mechanical CPU Result Delta

**Need.** A source-derived host capability can have a CPU-register result
without a guest-memory transaction. BOP `12h` is the first example: OpenNT's
owner sets AX and returns. Bochs must not acquire the selector or BIOS meaning.

**Behavior.** The adapter result may carry a fixed-width, validated mask and
up to the eight low-16-bit general-register replacements. The Bochs wrapper
applies those values only after the installed adapter has accepted the generic
exception result, and immediately before its existing resume. It contains no
fixed register, selector, guest address, value, BOP, DOS, BIOS, or profile
branch. A zero mask is the normal result.

**Negative cases.** Missing/invalid result delta, pass-through, unsupported
disposition, or a non-adapter callback leaves every CPU register unchanged and
follows the original native exception path. No segment, execution mode,
FLAGS, memory, or device state is mutable through this v1 delta.

**Review condition.** Reject this exception if it adds a selector-specific
branch, exposes a CPU pointer to adapter code, changes decoder behavior, or
adds arbitrary memory/segment/FLAGS mutation instead of a separately admitted
record.

### BX-EXEC-016: Reset-Baseline-Preserving Entry Delta

**Need.** Historical OpenNT `InitialiseDosEmulation` resets the machine, loads
NTIO, restores the four virtual-interrupt bytes, then calls only `setCS` and
`setIP`.  It does not publish a complete register image.  Replacing all
registers, FLAGS and segment state with zero is therefore neither source
derived nor a valid interpretation of the startup plan.

**Behavior.** The generic execution-plan consumer may apply a checked
real-mode `CS:IP` pair after the RAM transaction.  The Bochs helper loads CS
with its ordinary real-mode segment mechanics and assigns RIP/IP.  It preserves
all GPRs, FLAGS, the remaining segment registers, execution mode, descriptor
state, devices and memory exactly as left by native Bochs reset.

**Negative cases.** An absent, malformed or non-real-mode entry record reaches
no helper.  The helper has no selector-specific branch and cannot change any
state other than CS and RIP/IP.  A caller requiring any other CPU mutation
requires a separately versioned, source-derived record and a new exception.

**Review condition.** Reject this exception if it gains OpenNT/DOS/WOW/DEM/BOP
terms, profile/address branches, register defaults, or access to adapter
objects.  Revisit it only if source evidence proves a distinct post-reset CPU
field must be set.

### BX-TRACE-017: Default-Off Accepted-Resume Trace

**Need.** The r20 control-flow audit needs evidence of the fixed-width state
already supplied to the generic exception bridge at an accepted resume.  A
pass-through record alone cannot distinguish a wrong resume target from a
pre-existing stack or register inconsistency.

**Behavior.** When the explicit process environment switch is set, the generic
bridge logs the copied general-register/selector/FLAGS snapshot, checked
resume RIP and fixed-width result-delta mask.  It observes no new memory and
does not change dispatch, decode, CPU, memory, device or guest behavior.  The
switch is off by default and is not profile input.

**Negative cases.** An unset or non-`1` switch logs nothing.  Pass-through,
invalid and non-resume results retain their pre-existing behavior.  The log
contains no selector, BOP, OpenNT, DOS/WOW/DEM, service, profile or guest
address branch.

**Review condition.** Remove or reject this exception if it becomes a trace
control plane, reads guest memory, adds a semantic match, persists state, or
is used as a substitute for a source-derived capability contract.

### BX-ABI-018: CF-Only CPU Result v2

**Need.** The retained v1 result ABI deliberately cannot alter FLAGS. The
source-derived `DemGetDrives` completion requires CF clear in addition to its
AX result. Reusing v1 reserved bits or assuming the incoming CF would violate
its contract.

**Behavior.** The new, separately versioned result record contains the v1
low-16-bit GPR delta and an EFLAGS delta whose validation permits precisely one
bit: CF. Bochs applies that bit only after a v2 adapter result has been
accepted, using its native CF setter immediately before its existing resume.
It has no service, selector, address, profile, guest-memory or device branch.

**Negative cases.** A missing/invalid v2 record, unsupported flag bit,
pass-through, non-resume result or old v1 caller preserves the native exception
path and changes no FLAGS. No other FLAGS, segments, mode, memory or device
state is mutable through this result.

**Review condition.** Reject this exception if it adds another FLAGS bit,
permits raw EFLAGS replacement, embeds service policy in Bochs, or changes
decoder/exception-delivery behavior.

### BX-ABI-019: Generic Two-Phase Ordinary-RAM Read

**Need.** A host capability may need a small, source-derived guest value before
it can form a result. The adapter cannot borrow a Bochs memory pointer, and a
profile-owned passive observation cannot express an address derived from a
copied boundary state. Retrying the guest instruction, exposing a mapping, or
making a Bochs service branch would weaken the existing hard boundary.

**Proposed behavior.** A versioned adapter response may request one bounded
ordinary-RAM input range and defer its result. Bochs validates and copies that
range using the existing all-or-nothing ordinary-RAM primitive, then invokes a
separate adapter completion with only the original copied event/state/window,
the declared range and copied bytes. Only a valid completion result may resume;
any malformed request, failed read, completion decline or mismatch follows the
unchanged native exception delivery path. Bochs retains no mapping or data
after the callback.

**Boundary.** The Bochs code may know only opaque request/completion records,
the generic ordinary-RAM primitive and native resume machinery. It must not
contain a BOP byte, selector, service, OpenNT/DOS/WOW/DEM/COMMAND token,
register interpretation, profile, address constant, device policy or guest
data interpretation. The adapter owns address derivation, request admission,
byte interpretation, session lifetime and result semantics.

**Review condition.** Reject this exception if it permits multiple reads,
device/ROM/VGA fallback, guest-memory pointers, retries, persistence in Bochs,
or a service-specific Bochs branch. Implement only after the generic request,
completion and negative-test ABI is reviewed.

**2026-08-11 narrow application.** `SVC_DEMSETDTALOCATION` is the first
admitted use.  The adapter's already source-tested request names exactly two
ordinary-RAM bytes (the 16-bit `DOSWOWDATA.lpSftAddr` field); its completion
derives and stores only adapter-local physical locators, then supplies the
generic `RIP+4` result.  The narrow Bochs call site receives a generic action,
copies its declared range into a fixed adapter-independent local buffer, and
passes those bytes back for completion.  It contains neither the service name
nor any BOP/DOS/DEM byte/address/register interpretation.  A declined or
invalid action remains on the existing native `#UD` path.  This does not
authorize the broad runtime, `host_session`, a general guest-read facility,
search, filesystem behavior, or a second read.

### BX-MEM-020: Generic Ordinary-Writable-RAM Preflight

**Need.** `copy_to_ordinary_ram` checks one span before writing it, but a
transaction with several independent opaque spans must prove every target
writable before modifying the first. The existing read-only predicate cannot
prove writable mappings.

**Behavior.** Add `ordinary_ram_writable(addr,len)` by extracting the existing
side-effect-free validation from `copy_to_ordinary_ram`: reject zero/overflow,
A20 translation, handler overlap, monitor pages and absent ordinary writable
mappings. It copies nothing, retains no mapping and names no adapter/service.
The existing copier keeps its own validation and behavior unchanged.

**Negative cases.** Any invalid span returns false without memory/device
access. This method has no payload, callback, list, address constant, profile
or CPU/result behavior.

**Review condition.** Reject it if it gains DPB, BOP, DOS, OpenNT, adapter,
transaction, payload, device exception, write fallback or mutation semantics.

### BX-MACH-009: Default-Off Bare-Reset Composition Mode

**Need.** The r12 reset trace reaches CPU/RAM/ROM, generic I/O table setup,
the virtual timer engine and the PC-system benchmark stop; it reaches no guest
I/O device. The stock `bx_devices_c::init` nevertheless unconditionally
loads a PC device suite. A configuration switch cannot express that difference.

**Result.** The candidate was implemented only in the local r12 test image,
then rejected and removed from the project source. Its first reset reached
`speaker` reset, which calls the PIC lower-IRQ path; without PIC this hit the
native stub and panicked before the CPU loop. This is a mechanics dependency,
not evidence that the fixture guest used a speaker or PIC port.

**Boundary finding.** A future composition hook still must contain no guest
marker, selector, service, OpenNT/DOS/WOW/DEM/COMMAND name, file/profile
parser, payload or host capability. It must model reset-mechanics dependencies
explicitly, rather than assuming that no guest I/O means no device dependency.

**Next gate.** Derive the complete reset dependency closure (including plugin
reset callbacks and interrupt paths) before proposing another mode. Do not add
PIC as an isolated workaround.

**Review condition.** Reject this entry if the mode grows a device-specific
exception, a service-specific condition, or a second profile-specific Bochs
branch. Further devices must enter an adapter composition descriptor with
trace evidence, not this method.

### BX-ABI-008: Installed Adapter Session Commit Bridge

**Need.** `BX-UD-001` supplies a mechanical event and `BX-MEM-007` supplies a
mechanical all-or-nothing copy, but neither knows when a CLI-admitted adapter
transaction exists. The adapter must own profile verification, payload
lifetime, service selection and transaction preparation; Bochs needs only one
generic call sequence to consume an already-prepared range.

**Behavior.** The existing Bochs exception wrapper first gives copied records
to the adapter runtime. On a checked resume result, it obtains one pending
transaction and its same-process payload and invokes the generic ordinary-RAM
copy with the transaction's physical range. Only after that call succeeds does
it return the existing resume disposition. Pass-through, an absent session,
an invalid transaction, or a failed copy returns to the original native
exception path with no CPU update.

**Boundary.** The Bochs change contains no marker byte, selector, service
identifier, OpenNT/DOS/WOW/DEM/COMMAND name, profile parsing, file access,
hashing, or host capability. The adapter runtime, linked in the same UCRT
process, owns all of those non-Bochs concerns. Its payload pointer is never a
field in a versioned ABI or retained by Bochs.

**Review condition.** Reject this entry if a second special commit route, a
service-specific Bochs branch, guest-memory decoding, or a Bochs-owned
payload/file lifetime becomes necessary.

### BX-MEM-007: Generic Ordinary-RAM Commit

**Need.** The adapter's transaction records intentionally contain a guest
physical range but no Bochs pointer. `dmaWritePhysicalPage` is unsuitable as a
commit boundary: after a direct-map failure it falls back to byte-wise physical
writes, which can reach a handler/device and can partially mutate state.
`getHostMemAddr(..., BX_WRITE)` alone is also insufficient because a registered
direct-access handler may return a pointer.

**Behavior.** A new Bochs memory method accepts an already-owned byte span and
a physical range. It first rejects an overflow, A20 remapping, non-RAM range,
memory-handler overlap (including direct-access handlers), monitored page,
VGA/ROM/PCI veto, or missing writable mapping for every affected page. Only
after the complete validation pass succeeds does it copy page chunks through
ordinary writable RAM mappings. It has no marker parsing, service selection,
guest state update, or device fallback.

**Boundary.** The method is generic Bochs memory mechanics. It names no
OpenNT, DOS/WOW/DEM/COMMAND concept, selector, service number, adapter ABI, or
host capability. The adapter remains responsible for selecting a transaction;
the later exception bridge remains responsible for deciding whether to resume.

**Negative cases.** Any failed validation returns false before the first byte
is written. No fallback calls `writePhysicalPage` or DMA, and no device/ROM
handler is invoked. A zero-length write is rejected.

**Review condition.** Reject this exception if it requires a guest-memory
parser, endpoint-specific address rule, special device exemption, or a
service-specific mutation. Revisit the two-pass assumption if Bochs acquires a
concurrent memory-map mutator on the execution path.

### BX-MEM-010: Generic Ordinary-RAM Observation

**Need.** The first guest-startup descriptor requires evidence of a selected
Bochs machine's copied IVT/BDA/virtual-interrupt fields at a controlled stop.
The adapter cannot inspect Bochs memory or receive a memory pointer. Existing
physical-read paths may invoke handlers/devices and are therefore not a safe
observation boundary.

**Behavior.** A companion method accepts an adapter-owned destination span and
a physical range. It uses the same complete page-by-page ordinary-RAM
preflight as `BX-MEM-007`: reject overflow, A20 remapping, memory-handler
overlap (including direct-access handlers), monitor pages and any range lacking
an ordinary readable mapping. Only after the entire range passes does it copy
page chunks into the destination. It never invokes a physical-read, DMA, ROM,
VGA, PCI or device-handler fallback.

**Boundary.** The method is symmetrical Bochs memory mechanics only. It knows
no fixed address, profile, selector, marker, BOP, OpenNT, DOS/WOW/DEM/COMMAND
term, adapter ABI, host capability, event or session. The adapter must submit
a typed bounded request and owns the output lifetime; a later bridge decides
when an observation is allowed.

**Negative cases.** Any invalid span returns false before the first byte is
copied. A zero-length read and null destination are rejected. The method does
not weaken `BX-MEM-007` or grant a persistent Bochs-memory mapping.

**Review condition.** Reject this exception if it acquires a fixed guest
address, a service-specific branch, a callback into host policy, a retained
pointer, a handler/device exemption, or changes CPU/firmware/device behavior.

### BX-ABI-006: Bounded Instruction-Window Observation

**Need.** The adapter may recover a historical owner selection only after it
can verify exact guest input. The fault RIP alone does not identify bytes.
Rereading guest memory in adapter code would leak Bochs memory semantics;
using `access_read_linear` during exception processing can raise a new fault.

**Proposed behavior.** Before generic #UD hand-off, Bochs copies at most 15
already-fetched bytes from the current instruction page into a fixed-width,
pointer-free adapter record. If fewer bytes remain in that page, the copied
count is smaller; no cross-page read is attempted. This observation is passed
to the existing adapter dispatch, which initially returns pass-through.

**Boundary and negative cases.** The Bochs patch contains no opcode marker,
selector, BOP/DOS/OpenNT term, decoder branch, service call, guest write, or
cross-page/translated memory access. A null, unavailable, short, invalid, or
inactive observation preserves native #UD behavior. The adapter ABI must
validate only record shape and count; no owner is invoked by this entry.

**Review condition.** Reject or remove this entry if it needs a CPU pointer
outside callback lifetime, a second guest-memory read, decoder changes,
instruction execution changes, or an OpenNT dependency.

### BX-ABI-005: Mechanical State Snapshot Capture

**Need.** The first evidence-backed OpenNT owner observes general registers,
IP, flags, and selectors. Exception ABI v1 supplies only fault metadata. A
copied snapshot is the smallest way to prove that the native CPU can report
the needed mechanical state without lending out a CPU object.

**Proposed behavior.** At a #UD instruction boundary, Bochs constructs the
already-defined fixed-width state record and the same-island adapter validates
then receives a copy. The existing result remains pass-through unless a
separately admitted consumer supplies the existing resume result.

**Boundary and negative cases.** The patch must contain no BOP byte, selector,
service name, OpenNT name, memory access, register writeback, decoder change,
or device behavior. A missing/invalid snapshot or inactive bridge preserves
the original #UD path. The initial fixture must prove only a generic
real-mode register sample reaches pass-through; it cannot claim a BOP or
OpenNT call.

**Review condition.** Reject or remove this entry if capturing state requires
changing decoder semantics, retaining a CPU object past the callback, exposing
descriptor/page state, or introducing a raw memory mapping.

**Evidence.** r9 compiled the added C11 state object into the same UCRT
container and `make bochs.exe` completed successfully. The r9 adapter fixture
used a generated ordinary `UD2; HLT` ROM and observed
`ntdos64 adapter CPU state snapshot passed through`, followed by the existing
benchmark-bounded native stop. The consumer validated the copied state record
before returning pass-through; no BOP, guest-memory operation, state writeback,
or OpenNT service was exercised.

### BX-ABI-004: Same-Island Mechanical Adapter Bridge

The native build script copies the adapter ABI header/source into the private
Bochs build directory. A Bochs-internal callback converts its request to the
fixed-width adapter event, calls `bx_ntvdm_exception_dispatch_v1`, then converts
only a valid explicit result back to the existing internal response. The first
consumer always returns pass-through. This proves record copying and same-CRT
linkage, not service dispatch. No pointer, C++ object, BOP byte, or OpenNT
name is exchanged.

The r8 `-AdapterBridgeFixture` completed with the bridge pass-through log
predicate plus the benchmark-controlled native stop predicates. It proves
same-island C linkage and record copying while preserving original Bochs
exception delivery. It does not prove an OpenNT bridge.

### BX-UD-001: Generic `#UD` Hand-off

**Inputs.** The pinned Bochs 2.6 source under `refs/bochs`, its original
`BX_CPU_C::exception` entry point, and the OpenNT BOP encoding record in
`src/opennt/base/mvdm/inc/bop.h`.

**Procedure.** Before native #UD delivery mutates exception state, Bochs may
call an installed callback with a fixed-width record containing only ABI
version, CPU id, vector, error code, and fault RIP. A callback may either
decline or return a checked `resume_rip`. Decline preserves the original Bochs
path byte-for-byte after the call. Acceptance sets the supplied RIP and returns
to the native decode loop.

**Boundary.** The Bochs patch has no BOP byte pattern, service number, OpenNT
symbol, DOS/WOW/DEM/COMMAND term, guest-memory parser, or service dispatch.
The registration callback is an internal Bochs build seam, not the
Bochs/OpenNT ABI: no OpenNT object, pointer, C++ object, CRT allocation, or
function pointer crosses the typed adapter boundary. A later in-island wrapper
must translate this mechanical event to the adapter's versioned records.

**Negative cases.** No callback, a vector other than #UD, an ABI mismatch, an
unknown disposition, or a callback decline all execute the original native
exception path. The focused source-boundary test asserts those constraints and
forbids OpenNT/BOP vocabulary in the Bochs patch.

**Limits.** This exception does not authorize decoder changes, instruction
handlers, memory translation, device/firmware work, BOP recognition, or any
OpenNT source modification.

### BX-BUILD-002: GCC 16 Null-Pointer Comparison

**Inputs.** The one stopped UCRT build recorded under
`artifacts/build/bochs-2.6-native-nogui-ucrt-r3/build.log` and the original
Bochs 2.6 configuration writer at `refs/bochs/config.cc:3261`.

**Procedure.** Replace the invalid ordered pointer comparison
`getptr() > 0` with the semantically equivalent null test `getptr() != 0`.
No configuration option, output value, ownership boundary, CPU behavior, or
device selection changes.

**Why an adapter cannot replace it.** The expression is compiled before an
adapter exists and blocks the native Bochs executable from linking. A wrapper
cannot change C++ language validity inside that translation unit.

**Negative case and removal condition.** The focused source test accepts only
the null comparison at this line and rejects the old ordered comparison. If a
supported historical toolchain proves the original expression portable, this
exception may be removed with the corresponding build recipe; it is not a
license for unrelated compiler cleanups.

### BX-UD-003: Generic #UD Runtime Fixture

**Purpose.** `BX-UD-001` needs runtime proof without introducing a service
bridge. When, and only when, the process environment contains
`NTDOS64_UD_TEST=1`, Bochs installs a test callback. It accepts an ordinary
#UD and returns `fault_rip + 2`; the generated test ROM contains only
`UD2; HLT` at its reset vector. The benchmark timer then terminates the halted
native CPU loop.

**Boundary.** This is a test installation mechanism, not an OpenNT endpoint.
It has no BOP marker, service identifier, guest-memory parser, or OpenNT
include. Without the environment variable it does nothing and `BX-UD-001`
remains uninstalled. The fixture ROM is generated under `artifacts/`, not
checked in as product firmware.

**Removal condition.** Replace this local test path with the first verified
adapter installation path once the adapter ABI can register the same generic
callback. Do not promote this environment variable to product configuration.

### BX-MEM-011: Generic Pending-Observation Consumption

**Status.** Registered before implementation. It is the sole proposed Bochs
change for the profile-owned passive-observation fixture.

**Procedure.** After the existing same-island adapter has returned a checked
resume result, Bochs may take one pending observation bound to that exact
copied exception boundary. It passes only the record's ordinary physical range
and adapter-owned output span to existing `copy_from_ordinary_ram`. It resumes
only if the copy succeeds; otherwise the original exception path remains in
force.

**Boundary.** The Bochs code must not inspect instruction bytes, profile IDs,
addresses selected by the profile, output contents, BOP encodings, service
numbers, or any OpenNT/DOS/WOW/DEM term. Trigger matching and resume choice
remain adapter work. The existing ordinary-RAM primitive retains its all-or-
nothing handler/ROM/device rejection rules.

**Verification.** A focused source-boundary test must require one pending
observation take and `copy_from_ordinary_ram`, reject trigger/profile/BOP/DOS
vocabulary in the Bochs block, and prove failed copy cannot return resume. The
fixture must use only the generated `UD2;HLT` ROM and must not claim guest
startup.

### BX-MEM-012: Opaque Observation Digest Log

**Status.** Registered before implementation for the stock-firmware probe.

**Procedure.** After `BX-MEM-011` has successfully copied one pending
transaction, Bochs may ask the adapter to complete that exact same-island
transaction. The adapter validates the copied record, output pointer and byte
count against its one-time pending state, computes a fixed FNV-1a digest, and
returns only that integer for a Bochs log line.

**Boundary.** Bochs does not inspect, decode, name or branch on output bytes;
it logs an opaque adapter-provided integer only. The adapter does not receive a
Bochs pointer or memory mapping. The digest is evidence transport, not a guest
memory API, firmware parser, BOP/DEM service, or result value supplied to a
guest.

**Failure.** Completion failure returns to the original exception path and
does not resume. Completion is single-use; a repeated or mismatched completion
is rejected.

### BX-MEM-013: Generic Multi-Range Read Preflight

**Status.** Implemented and runtime-proven by the r17 custom fixture.

**Need.** `BX-MEM-010` proves an individual range before copying it, but a
snapshot must not expose any result unless every declared range is ordinary
readable RAM. Calling the existing copier sequentially would make the first
range's destination bytes exist before a later range is rejected.

**Procedure.** Add one generic `ordinary_ram_readable(addr,len)` method which
uses the same side-effect-free full-span validation as `copy_from_ordinary_ram`.
It rejects null-equivalent spans, overflow, A20 remapping, handlers, monitor
pages and non-ordinary direct mappings exactly as the existing reader.
The snapshot seam may first call that method for every opaque range, then call
the unchanged copier for each range into the adapter-owned contiguous output
arena. It reports completion only after every copy succeeds.

**Boundary.** The new Bochs method knows only a physical range and ordinary
readability. It names no profile, snapshot, adapter, BOP, OpenNT, DOS, BIOS,
IVT, BDA, host capability or output meaning. The exception seam sees opaque
ranges/output spans only. It must not branch on range addresses or contents.

**Failure.** Any preflight failure occurs before the first snapshot byte is
copied. A post-preflight copy failure returns to the original exception path;
the adapter never completes or exposes the partially filled arena.

**Verification.** Extend the memory boundary test to require the shared
preflight safety tokens and extend the exception boundary test to require a
snapshot take, all-range preflight loop, generic copies and one opaque digest
completion, while rejecting profile/firmware/BOP/DOS vocabulary.

### BX-EXEC-014: Generic Atomic Execution-Plan Consumer

**Need.** The first source-admitted guest prefix requires one operation that
cannot be represented by an exception callback: capture an ordinary-RAM range,
copy an admitted image that overlaps it, restore exact captured bytes, apply a
complete real-mode CPU state, then enter the original CPU loop. Splitting this
into a boot ROM or individual register hooks would expose partial state.

**Procedure.** At one location after original `bx_init_hardware()` and before
the first `BX_CPU(0)->cpu_loop()`, a default-off seam may request a
pointer-free plan from the same-island adapter. It uses generic ordinary-RAM
preflight/copy mechanics for capture/write/restore and existing native CPU
setters for complete state application. Only complete success may enter the
CPU loop. An absent plan preserves native startup.

**Boundary.** Bochs code contains no image/guest name, physical address,
vector, firmware, BOP, OpenNT, DOS/WOW/DEM/COMMAND term, profile/file/hash,
service dispatch, payload interpretation or device exception. It knows only
opaque plan fields, generic RAM operations and native CPU setters. The adapter
owns all source-derived meaning and payload lifetime.

**Failure and verification.** A malformed plan, failed RAM operation, failed
state application or repeated attempt prevents CPU-loop entry; it may not fall
back to firmware boot or make a partial plan visible. A focused source test
must require the single post-hardware/pre-loop seam and two-copy order, reject
the semantic vocabulary above, and prove default startup remains unchanged.
A custom same-version fixture must prove a harmless plan before real guest
bytes are admitted.

**Review condition.** Reject this exception if it needs another lifecycle
hook, a special address/selector/service branch in Bochs, a persistent mapping,
a device/firmware override, partial state application, or an object/pointer ABI.

### BX-BUILD-033: Isolated Port-Space Archive Membership

**Need.** The one permitted r3 intact-native target build compiled the prior
`BX-IO-029`/`BX-IO-030` source split, but its untouched generated I/O archive
membership still names only `devices.o`.  Consequently the linker cannot find
the five moved member definitions in `minimal_port_space.o`.  This is a
direct, deterministic result of the prior source-object separation, not an
adapter, runtime-profile, or device requirement.

**Behavior.** Add `minimal_port_space.o` immediately after `devices.o` in
`OBJS_THAT_CANNOT_BE_PLUGINS` in both the generated source `iodev/Makefile`
and its input template `iodev/Makefile.in`.  The object stays in the original
`libiodev.a` archive and is compiled with the existing I/O component flags.
No source body, symbol, header, caller, archive ordering beyond that one
member, linker input, configuration value, or runtime control changes.

**Boundary.** This is a Bochs build-graph repair wholly inside the existing
I/O archive.  It does not enable a device, plugin, GUI, firmware, network,
USB, PCI, sound, adapter, CLI, OpenNT, BOP, DOS/WOW/DEM, guest I/O mapping,
or host-service policy.  The normal native profile continues to disable
optional plugins at runtime.

**Negative cases and verification.** A static source check must require one
and only one `minimal_port_space.o` membership in each corresponding archive
list and reject any new adapter or OpenNT token in either file.  A future
fresh-root build, separately admitted in `STATUS.md`, may establish only
whether the four r3 undefined symbols resolve; it must not be a retry of r3,
run the target, or imply guest capability.

**Review condition.** Reject this exception if membership needs another
archive, any device/plugin feature change, a new object-selection scheme, or
a source/API/semantic change.  Revisit after exactly one new fresh-root
observation; if another independent closure failure appears, record it before
any further correction.

### BX-EXEC-032: Default-Off Compile/Link Isolation

**Need.** The detached intact-native build reached root `main.cc` and proved
that the existing BX-EXEC-014 code unconditionally includes the adapter
runtime header.  `Makefile.in` also unconditionally lists adapter objects,
while the retained generated MSVC Makefile does not.  An absent execution plan
therefore does not preserve a buildable native startup container.

**Procedure.** Guard the existing BX-EXEC-014 include, private helper and
post-hardware/pre-loop call with one named compile-time opt-in whose default is
off.  Guard the matching `Makefile.in` include path and adapter object list
with the same opt-in.  With the opt-in absent, the original Bochs startup path
contains neither adapter header nor adapter object dependency.  A future
same-island execution-plan fixture must explicitly enable the opt-in and
supply its own adapter closure.

**Boundary.** This change removes a default dependency; it adds no runtime
behavior.  It must not modify execution-plan bytes/validation, Bochs CPU or
memory mechanics, profile parsing, device selection, BOP, DOS/WOW/DEM, OpenNT,
host service or CLI behavior.

**Negative cases and verification.** A focused static test must prove the
default path has no `bx_ntvdm_adapter_runtime.h`, adapter call or
`ntdos64_adapter` object in the compiled/link object vectors, and that the
only adapter-bearing blocks occur under the named opt-in.  The existing
execution-plan source boundary remains required when enabled.  Re-run native
container work only in a new root after a separate build admission.

**Review condition.** Reject if one macro cannot isolate all three locations,
if any adapter ABI becomes visible by default, if source semantics change, or
if this grows into a second adapter integration mechanism.

### BX-EXEC-034: Narrow Opt-In Startup-Session Owner

**Need.** `BX-EXEC-032` correctly removed adapter linkage from the default
native target, but its opt-in `main.cc` helper still names
`bx_ntvdm_adapter_runtime`. The accepted static inventory proves the smaller
startup-session closure has no runtime/service or Bochs dependency. Retaining
the broad runtime merely to obtain the first NTIO plan would violate the
adapter boundary.

**Procedure.** Add one adapter-owned environment endpoint that reads only the
two immutable child-environment strings set by the CLI, owns one narrow
startup-session and returns absent, ready or rejected. In the existing
`BX_NTVDM_ENABLE_EXECUTION_PLAN` block, replace only the included adapter
header and plan-preparation call. The generic RAM-copy and real-mode-entry
consumer stays byte-for-byte and order-for-order intact. Do not alter a Bochs
Makefile: the default target must continue to have no adapter object, and a
future companion target supplies its separately admitted adapter closure.

**Boundary.** The endpoint may use existing BYOB profile/image verification to
stage NTIO and retain NTDOS/COMMAND identities. It must not include the broad
runtime, host session/service, drive policy, DEM/BOP/CMD/search/namespace, a
Bochs header, or a guest-memory primitive. Bochs must not read profile/root
environment values itself.

**Negative cases and verification.** Focused source tests must reject every
broad-runtime term from `main.cc` and the endpoint, prove the opt-in macro
still encloses the only adapter-bearing Bochs blocks, and preserve the generic
copy/entry ordering. No build, link, guest write or run is evidence for this
exception.

### BX-TRACE-035: Default-Off x87 Compatibility Snapshot

**Need.** The accepted source-built NTIO first-prefix observation reaches the
original x87 compatibility IRQ13 path, but its existing diagnostic omits the
x87 fields needed to distinguish reset state from a prior x87 operation. The
design rejects adapter `#UD`, debugger, and instrumentation paths because they
either mutate the guest path or add a general tracing feature.

**Procedure.** In `fpu/fpu.cc`, introduce one local compile-time macro whose
default is `0`. When explicitly enabled, and only within the already selected
`CR0.NE==0` compatibility branch of `FPU_check_pending_exceptions`, log fixed
native CPU/x87 state immediately before the existing `math_abort` info log and
unchanged `DEV_pic_raise_irq(13)`. The values are CPU ID, CS:RIP, CR0, x87
control/status/tag/TOP, and existing last-x87 fields. No state is allocated,
retained, changed, or exported through an ABI.

**Boundary.** This is Bochs-local observability only. It has no adapter,
OpenNT, DOS/WOW/DEM, BOP, host-service, profile, firmware, device, guest
memory, pointer, callback, instruction-decoding, tracing, stop, resume, or
interrupt-handling semantics. The original FPU predicate and IRQ13 delivery
remain the sole behavior owners.

**Negative cases and verification.** A static test must require the local
default-off guard; reject adapter/OpenNT/guest semantic tokens; and prove the
new log, old `math_abort` log, and original `DEV_pic_raise_irq(13)` remain in
that order inside the unmodified compatibility branch. Default builds must
compile the branch without the added log. Any object/link/run uses a separate
admission and a fresh root.

**Review condition.** Reject this exception if it needs a header/object,
instrumentation/debugger enablement, arbitrary instruction stream, guest
address/range, event coalescing, or a CPU/FPU/PIC result change. Review after
one separately admitted immutable-input observation; remove it if the same
facts become available through an original bounded diagnostic.

### BX-ABI-036: Narrow Startup-Transaction #UD/Write Consumer

**Need.** The existing r6 companion uses the narrow startup-session endpoint
for the initial plan, while the older #UD consumer reaches the broad adapter
runtime and its unrelated read, observation, multi-write and secondary-service
paths. The first source-built NTIO transition requires only one existing
pending write and a checked resume. Linker dead stripping is insufficient as a
hard-boundary definition.

**Procedure.** Add a local `BX_NTVDM_ENABLE_STARTUP_TRANSACTION` macro that
defaults to `0`. When explicitly enabled, `cpu/exception.cc` may capture the
same existing scalar #UD event, CPU snapshot and bounded instruction window;
call an adapter-owned startup-transaction bridge; take one same-boundary
opaque pending write; use the existing ordinary-RAM write helper; and apply
the returned checked resume through the existing exception longjmp. In the
already opt-in execution-plan block, `main.cc` may request the plan only from
that same bridge. After the existing write and resume checks have both
succeeded, the opt-in path may log only the generic committed write address,
byte count, resume RIP, and the copied fault CS:EIP from the existing generic
exception request. The default path retains no bridge header, type, object or
call.

**Boundary.** Bochs contains no marker byte, selector, service identifier,
guest image name, DOS/WOW/DEM/COMMAND/OpenNT term, host path, service table,
profile detail, address policy, device/firmware/IVT logic or pointer/object
ABI. The bridge returns only versioned copied records plus same-island payload
data for an already-preflighted generic write. Existing broad interception is
neither changed nor selected by this macro. The confirmation log is a
post-commit mechanical outcome only; its CS:EIP fields are copied from the
generic exception request and it has no selector/service decode, instruction
bytes, guest-image or host-path field. It does not affect control flow.

**Negative cases and verification.** A focused source test must require the
local default-off macro and the one generic dispatch/take-write/resume flow;
it must reject broad-runtime/read/gather/observation/multi-write/secondary
service tokens from the new block and prove the original exception path remains
when disabled. The startup bridge test must reject nonmatching input, repeated
or mismatched take, non-real-mode state, and every non-`50:11` service. The
existing default-off and generic #UD boundary tests remain required.

**Review condition.** Reject if another Bochs lifecycle hook, decode change,
special guest address/byte branch, callback, mapping, device/firmware action,
or general service facility becomes necessary. Revisit after one separately
admitted same-version companion observation.

### BX-ABI-053: Default-Off Machine-Composition Mechanical Call

**Need.** T110--T116 prove that reached selector `02h` is a machine operation,
not an adapter host service, and that the unchanged original handler has only
three finite mechanical imports. The existing generic adapter seam cannot
carry synchronous PIC/RAM mechanics without violating the adapter boundary.

**Procedure.** In `cpu/exception.cc`, behind one local macro that defaults to
`0`, add one selector-blind call to the external machine-composition component.
It receives only the already copied generic exception/window facts and a
dynamically scoped context whose callbacks invoke existing registered Bochs
8-bit port read, 8-bit port write, and checked ordinary-RAM byte write owners.
It may return only a generic decline or a future separately admitted handled
disposition. The T117 implementation supplies no original handler and must
therefore decline every invocation. Add only the component object to the named
opt-in derivative's `Makefile.in` membership.

**Boundary.** Bochs contains no selector constant, marker-byte decode,
historical handler name, BIOS table, OpenNT/SoftPC/CCPU/DOS/WOW/DEM term,
adapter callback, guest address policy, host capability, device initialization
or changed CPU exception semantics. The mechanics context never enters the
adapter, cannot escape the call, and delegates rather than reimplements PIC or
RAM behavior. The component, not Bochs, owns any later catalogue selection.

**Negative cases and verification.** A source boundary test must require the
default-off guard and one generic external call, reject selector and historical
semantic terms from the Bochs block, and prove default builds retain neither
the new object nor call. Component tests must decline host-service,
unavailable, non-`02h`, missing-handler and failed-mechanics requests. A
derivative object/build audit must name only `exception.cc` plus the component
object; no guest execution or handler call is admitted in T117.

**Review condition.** Reject this exception if it needs a second lifecycle
hook, a generic port/device API, a selector/service branch in Bochs, adapter
mechanics access, any device enablement, a guest-memory policy, or runtime
handler invocation. Review before T118; remove if the machine-composition
boundary cannot remain selector-blind and default-off.

### BX-MANTLE-078: Selector-Blind Native A20 Capability

**Need.** The full XMS/DPMI package plan identifies A20 state as capability
C1. The minimum machine already uses Bochs' native A20 setter during reset,
but adapter code must not own a shadow state or call native internals directly.

**Procedure.** Add project-authored mantle files
`bx_ntvdm_a20_capability_v1.h/.cc`. A Boolean lifecycle gate in the existing
minimal-machine owner becomes true only after successful initialization and
false at cleanup. The wrapper exposes fixed-width `set` and `query`
operations, call existing `bx_pc_system.set_enable_a20`/`get_enable_a20`, and
allow access only while a successfully initialized minimal machine owns the
native state. The wrapper neither imports nor names a BOP, XMS/DPMI, OpenNT,
DOS, guest address, host feature, device, interrupt, firmware, allocator, or
CPU decoder.

**Negative cases and verification.** A focused MSVC x64 `/MT` fixture must
prove native enabled-to-disabled-to-enabled transition and query, reject
unknown/version-invalid/pre-initialize/post-cleanup input, and prove each
rejection leaves native state unchanged. The existing minimal-machine boundary
test must still prove the initial A20-before-reset ordering. Source scans must
reject adapter/BOP/OpenNT terms from the wrapper. No BOP result, guest run, or
device enablement is admitted.

**Review condition.** Reject/remove this exception if it requires a bx-core
patch, product-shell API, callback, pointer/handle, selector-specific branch,
second state copy, allocator, interrupt/firmware action, or direct adapter
dependency.

### BX-MANTLE-079: Mantle-Private Native Extended-Aperture Capability

**Need.** The shared XMS/DPMI plan identifies a bounded extended-RAM allocator
as C2.  The native memory owner already exposes RAM length and checked copied
ordinary-RAM operations, but those methods do not provide a bounded allocation
lifetime and cannot expose host mappings across a component boundary.

**Registration correction.** An uncommitted C2 prototype preceded the entry,
contrary to the register's prospective rule.  S24 does not treat that fact as
authorization: it records the deviation, reviews the final surface against
this row, and admits only the verified implementation in the resulting commit.

**Procedure.** Add only project-authored mantle files
`bx_ntvdm_extended_memory_v1.h/.cc`.  A fixed-size internal table is active
only while the existing minimal machine owns initialized memory.  Its
fixed-width requests support capacity query, allocation, release, resize and
checked copy between allocated ranges.  Successful results return opaque
numeric handles and physical offsets only.  The aperture begins at 1 MiB and
ends at the native memory length; all copying uses checked physical-backing
RAM primitives.  The wrapper contains no historical service selector
or guest-service provider behavior.

**Amendment (S31).** OpenNT's historical XMS allocator is host/SAS-owned and
continues to allocate extended backing storage when the guest has temporarily
enabled 20-bit wrapping. Bochs' `ordinary_ram_*` primitives correctly reject
that case because they model a guest A20-applied access. Add selector-blind
`BX_MEM_C::{backing_ram_readable,backing_ram_writable}` and matching checked
copy primitives in `bx-core/memory/{memory.h,misc_mem.cc}`. They validate an
unmasked physical backing range, including bounds, handler, and monitor
rejection; expose no mapping or pointer; do not change CPU or A20 state; and
are used only by the mantle-private aperture. This is the sole S31 bx-core
change. Fixed C linkage on the two mantle records permits a separately
compiled adapter to consume opaque records without changing provider semantics.

**Negative cases and verification.** A focused MSVC x64 `/MT` fixture must
prove inactive rejection, no allocation below 1 MiB, non-overlap, invalidated
handles, resize atomicity, bounds rejection, overlap-safe copied moves and
post-cleanup rejection.  Rejected operations leave allocation state unchanged.
The whole-core finite-machine recipe must explicitly name the new translation
unit.  Source scans must reject adapter/BOP/OpenNT and host-pointer vocabulary
from the wrapper.  No BOP result, guest run, device enablement, or XMS/DPMI
provider is admitted.

**Review condition.** Reject/remove this exception if it requires a further
bx-core patch outside the bounded physical-backing primitive above,
product-shell API, callback, raw host pointer, host allocator,
selector-specific branch, device/firmware/interrupt behavior, or direct
adapter dependency.

### BX-MANTLE-083: Selector-Blind Native Byte-Port Action

**Need.** The P5 source-derived `illegal_op_int`/`unexpected_int` transcript
contains original byte PIC operations.  The existing mantle owns the native
PIC and port space, while `bx-vdm` may not receive a Bochs object, callback, or
port table.  A fixed copied-operation seam is therefore the smallest
composition capability.

**Procedure.** Add only `bx_ntvdm_port_action_v1.h/.cc` in `src/bx-mantle`.
The record permits exactly one byte read or byte write at a 16-bit port.  A
private lifecycle bit becomes true only after the existing minimal machine has
finished memory, port-space, PIC, CPU reset and its existing capability setup;
it becomes false before PIC/port cleanup.  The executor rejects invalid or
inactive records and otherwise calls the already-owned `bx_devices.inp/outp`.
It returns a copied read value or success/failure only.

**Boundary.** The files contain no selector, BOP, OpenNT, DOS/WOW/DEM,
provider, host-capability, device-registration, firmware, callback, raw
Bochs-object, or pointer ABI.  They create no device and alter no core source:
the fixed native PIC remains the sole current port owner.

**Negative cases and verification.** A formal MSVC x64 `/MT` native fixture
must prove invalid/pre-initialize/post-cleanup rejection, one native PIC mask
write/read through this seam, and no effect on rejected requests.  A static
source scan must reject semantic and product-shell terms.  Direct and Readonly
are structurally identical because no host mutation occurs.

**Review condition.** Reject/remove this entry if it needs a selector branch,
a port-range policy, an arbitrary-width or batch API, a second device,
firmware, callback/object export, or any bx-core change.
### BX-CORE-084: Selector-Blind Typed Segment Resume Delta

**Need.** Original OpenNT `cmdexec.c` `cmdCheckBinary` returns real-mode
DS:DX and ES:BX locations inside its SCSINFO handoff.  The existing generic
UD result carried GPR and flags but not segment updates, so omitting them
would change the original return contract.

**Procedure and boundary.** Extend only the fixed CPU delta and generic-UD
outcome with a six-entry segment write mask/value array.  `exception.cc`
applies an accepted entry through Bochs' existing `load_seg_reg`; it remains
selector-, BOP-, DOS-, OpenNT- and provider-blind.  No device, firmware,
memory, callback or host pointer is added.

**Verification.** T231 S6's focused command fixture proves the typed DS/ES
delta values produced by the original body; the formal native closure must
compile the changed core source before any native execution claim.

**Review condition.** Remove/reject if this transport acquires selector or
service recognition, guest-memory inspection, host capabilities, or any
semantic branch beyond applying a typed native segment update.

## OpenNT Intrusions

None authorized or implemented. OpenNT remains unmodified by the adapter
route. Any future OpenNT overlay or source edit requires its own row here,
including the reached original owner and an ABI/failure-behavior record.

## Registered But Not Active Candidates

| Candidate | Reason it is not an active exception |
| --- | --- |
| `src/bx-ntvdm-adapter/patches/bochs-2.6-msvc-fmt-ll.patch` | Retained un-applied historical diagnostic material; it is not part of the pinned source or active build. |

## Verification And Follow-up

Run `tests/bx-ntvdm-adapter/Test-BochsUdInterceptBoundary.ps1` after changing
the exception seam. Run the documentation-governance verifier and
`git diff --check` for the full record. Runtime proof remains pending until a
link-built native Bochs container is available under separately approved build
policy.
