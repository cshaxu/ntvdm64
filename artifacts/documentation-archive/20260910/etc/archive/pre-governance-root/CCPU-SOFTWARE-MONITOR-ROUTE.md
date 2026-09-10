# CCPU Software-Monitor Route

## Decision

The fixed NT4 x86 build profile in
`base/mvdm/softpc.new/obj.vdm/cdefine.inc` selects `MONITOR`, `C_VID`, and
`X86GFX`. It does not select `CCPU`. The historical x86 product therefore
depends on the hardware V86 monitor and cannot be restored as an x64
software-emulation runtime merely by recovering more host imports.

The NT4 CCPU source is nevertheless a retained software x86 execution core.
The proposed research route is to retain the historical CCPU, BIOS, BOP,
`nt_bop`, DEM, and NTDOS responsibilities, while replacing only the
monitor-facing execution boundary with a newly designed software monitor.
This is not a claim that the original x86 monitor has been recovered.

## Evidence

`ccpu386/c_main.c` already owns the instruction-side BOP behavior:

- `C4 C?` is decoded as a BOP form; CCPU consumes the selector/argument bytes,
  advances EIP, then calls `bop()` or `EDL_fast_bop()`.
- `D6 ib` consumes the immediate selector, advances EIP, then calls `bop()`.
- BOP `FE` calls `c_cpu_unsimulate()` to leave the current CCPU invocation.
- `c_cpu_simulate()` enters the interpreter, `c_cpu_continue()` resumes at the
  internal continuation point, and `c_cpu_reset()` establishes CCPU reset
  state.

The replacement monitor must therefore not install a second decoder for C4,
D6, or a Microsoft-specific instruction pattern. Historical BOP selection
remains inside CCPU and the original `BIOS[] -> nt_bop -> DemDispatch` chain.

## Boundary

The new monitor owns only these neutral execution services:

1. Create/destroy/reset a CCPU instance and bound a `c_cpu_simulate()` run.
2. Bind CCPU's SAS memory operations to checked guest physical memory.
3. Provide CPU state access through the original register bridge where
   possible, with copied/validated state at the public boundary.
4. Queue and deliver maskable/non-maskable/reset interrupt conditions through
   CCPU's existing interrupt/reset mechanisms.
5. Translate only monitor lifecycle outcomes: resumed, yielded/unsimulated,
   stopped, or faulted. It cannot interpret or complete a DOS SVC/BOP.
6. Supply clock/input/device events through their existing historical owners
   before a subsequent execution slice; it cannot mutate guest state behind
   those owners' backs.

The following remain outside the monitor:

- BIOS selector tables and PC/AT service semantics.
- `MS_bop_0`, `DemDispatch`, DEM state, COMMAND, DPMI, XMS, and NTDOS.
- DOS namespace, PIF policy, OEM path policy, and host process handoff.
- UI, console policy, and release packaging.

## Required Contracts

### Memory/SAS

The CCPU-facing SAS adapter needs byte/word/dword read/write, checked ranges,
A20 state, and a fault return that CCPU can turn into its normal exception
path. It must use a single physical-memory authority. No direct pointer into
unvalidated host memory may be exposed to CCPU or historical providers.

### Register/Execution

The monitor must establish the reset register state through CCPU's own reset
and register APIs. A caller receives a copied register snapshot only at slice
boundaries. It cannot set arbitrary protected-mode caches, descriptor state,
or EIP while CCPU is running.

### Events

IRQ, reset, and stop requests are queued between slices or passed by the
documented CCPU interrupt mechanisms. A host callback cannot synchronously
re-enter `c_cpu_simulate()` from a BOP, device, or file callback.

### BOP

No public API is keyed to C4/C4, D6, BOP selector values, Microsoft names, or
DOS service numbers. CCPU's original `bop()` call is bound to the recovered
historical dispatcher. The monitor only records selector/IP/register evidence
at the boundary for bounded tracing.

## Staged Verification

1. **SM-0:** default-disabled CCPU monitor fixture reaches `c_cpu_init()`,
   `c_cpu_reset()`, and a deliberate BOP-FE unsimulate without BIOS/DEM.
2. **SM-1:** checked SAS fixture proves RAM, A20, ROM rejection, and fault
   propagation while executing a bounded instruction corpus.
3. **SM-2:** bind the original BIOS BOP table and prove a selector reaches its
   original handler without a replacement handler in the path.
4. **SM-3:** bind original `nt_bop` and DEM, trace a bounded NTDOS startup
   prefix, and enumerate remaining host seams.
5. **SM-4:** only after each seam has a named modern-host replacement, build a
   research-only CLI runner. It remains separate from default builds and never
   bundles protected guest files.

Each stage must demonstrate that the historical handler was called. A guest
advance caused by a new hand-written BOP/SVC handler is a failure, not
progress.

## SM-0 Evidence

The default-disabled
`ntdos64-opennt-ccpu-software-monitor-sm0-fixture` was built with the
standalone x86 clang-cl gate and executed successfully on 2026-08-07. Its
complete bounded sequence is:

1. Call the original `sas_init(1 MiB)`. This is the order used by historical
   `config()` before `c_cpu_init()` and makes CCPU's original SAS vector and
   scratch-buffer setup authoritative.
2. Call original `c_cpu_init()`, which reaches original `c_cpu_reset()`.
3. Place only `C4 C4 FE` at physical address zero, set CS:IP through original
   `nt_aorc.c` register accessors, and enter original `c_cpu_simulate()`.
4. Let CCPU recognize BOP FE and return by original `c_cpu_unsimulate()`.
5. Call original `c_cpu_terminate()` and `sas_term()`.

The fixture admits only two source-defined host results while establishing the
empty machine: `config_inquire()` returns no optional `C_SWITCHNPX` profile,
and `host_read_resource()` returns the documented zero-byte result for absent
ROM resources. The latter leaves the SAS aperture ordinary RAM; no ROM bytes,
BIOS table, BOP handler, device, DEM, or DOS component is supplied. Every
other pulled host callback remains an abort-on-use guard, including EMS,
instruction virtualisation, host simulation, interrupt, timer, and mouse
callbacks.

The successful process exit code was `0`. This proves only the bounded CCPU
execution and unsimulate lifecycle. It is not evidence of a bootable machine
or of any historical NTVDM service chain; those remain SM-1 and later work.

## SM-1 Evidence

The default-disabled `ntdos64-opennt-ccpu-software-monitor-sm1-fixture` was
built and executed successfully on 2026-08-07. It executes bounded real-mode
guest instructions through original CCPU SAS and proves three historical
behaviors: ordinary RAM writes/readback work; a page marked `SAS_ROM` silently
rejects a guest write and retains its old byte; and enabling 20-bit wrapping
makes `FFFF:0010` alias physical address zero.

This is evidence of historical CCPU semantics, not the future neutral memory
contract. In particular, the silent ROM-write rejection and out-of-range
sentinel behavior are incompatible with the required checked/fault-reporting
core boundary. A later SAS adapter must translate those conditions explicitly;
it must not expose the host aperture directly or treat this fixture as a
complete fault-propagation proof.

## SM-2 Closure Evidence

SM-2 uses original `bios.c` rather than a reduced replacement selector table.
Its BOP 21 `Get_build_id` probe therefore causes the linker to retain the
complete historical `BIOS[]` table. The 2026-08-07 closure log demonstrates
that BIOS reachability is a full-machine concern: mouse, keyboard, display,
disk, serial, printer, CMOS, timer, DMA, reset, EMS, XMS, and `MS_bop_0..F`
are co-owned by that table. The initial target intentionally remains
unlinked; no local handler is admitted to make BOP 21 appear to work.

Original OpenNT XMS owns the A20 calls reached from `tape_io.c`; it is now in
the SM-2 closure. Remaining unresolved symbols identify historical owners or
host seams, including suballocation/UMB, NT memory APIs, keyboard INT 15,
and `ClearInstanceDataMarking`. The latter has a call site in `virtual.c` but
no recovered definition in the examined NT4, NT4.5, or NT5 source inputs.
NTVDMx64 contains a same-named empty link stub in
`ntvdmpatch/patches/common/fmstubs.c`, explicitly annotated as unknown. It is
weak evidence that later compatibility work also lacked this definition; it
is not an admissible implementation or a statement of the historical effect.
OpenNT `virtual.c` itself owns NIDDB allocation and deallocation. Its
`VxD_System_Exit` order is `deallocate_all_NIDDB`, `restore_snapshot`, then
the missing marking cleanup. Any future modern-host seam must therefore clear
only host instance-context marking after the original NIDDB lifecycle; it
must not replace NIDDB ownership or alter that ordering.

### Original `nt_unix.c` Provider Pass

The 2026-08-07 SM-2 link diagnostic additionally includes the original
default-disabled `host/src/nt_unix.c` object. This is the historical owner of
`host_get_version`, `host_get_years`, and `host_get_unpublished_version`, all
three of which are called by the original BIOS reset/copyright path. The new
log no longer reports any of those imports. No local string-returning shim was
introduced.

The same original module exposes its actual remaining host requirements:
`ActivityCheckAfterTimeSlice` and `WNTPifFgPr` occur through its historical
idle/timeslice path, while `_sys_errlist` and `_sys_nerr` occur only through
its legacy system-error text helper. These are evidence that the full module
cannot silently become a version-string provider. The first two belong to the
already-recorded session/idle host seam; the legacy CRT table must be replaced
only through an explicitly bounded diagnostic/error-text contract, if that
path is required. The pass also leaves unrelated original closure gaps visible,
including historical host disk, serial/parallel, display, timer, and lifecycle
providers. It produces no executable and does not admit any of them.

### Original CCPU SAS/A20 Forwarding Pass

The CCPU software-FPU archive deliberately omits original
`ccpu386/ntstubs.c`, because that translation unit provides fallback no-op NPX
initialisers which collide with `fpu.c`. It also owns the historical generic
SAS forwarders: `sas_enable_20_bit_wrapping`,
`sas_disable_20_bit_wrapping`, and
`sas_twenty_bit_wrapping_enabled` each forward to their corresponding
`c_sas_*` owner. The full no-FPU alternative was tested and rejected as a
shortcut: recompiling its original `sascdef.c` reaches the unrecovered
generated `TYPE_sas_touch` and `TYPE_VirtualiseInstruction` declarations.

SM-2 therefore adds only the original `ntstubs.c` object beside the existing
software-FPU archive. Its three unused fallback exports (`copyROM`,
`initialise_npx`, and `npx_reset`) are preprocessor-renamed only in that
closure object, so actual callers retain the archive's selected FPU owners and
the linker is never forced through a duplicate definition. The 2026-08-07
SM-2 log no longer reports any of the three generic A20/SAS imports. This is
an original ABI forwarding pass, not a new A20, SAS, ROM, NPX, BIOS, BOP, or
DOS implementation. The unrecovered `c_sas_touch` and
`c_VirtualiseInstruction` vector slots remain fail-closed and are not called
by the bounded fixtures.

### Original EOI/ICA Owner Pass

SM-2 also includes the original `host/src/nt_eoi.c` owner. Its selected
historical exports resolve `RegisterEOIHook`, `host_DelayHwInterrupt`,
`host_EOI_hook`, `host_ica_lock`, `host_ica_unlock`, and the original
`DelayIrqLine` state. The trace foundation withdraws only its colliding EOI
guards and sentinel in this profile; SM-0 and SM-1 retain their abort-on-use
guards.

`nt_eoi.c` calls, but does not define, `host_clear_hw_int`. The SM-2
foundation deliberately retains that callback as a fail-closed downstream
dependency rather than inferring a CPU/interrupt-clear implementation. The
remaining EOI object imports (`CurrHeartBeat`, `ActivityCheckAfterTimeSlice`,
`VDMForWOW`, display termination, native critical-section functions, timer
queue operations, and ICA primitives) remain visible as the next historical
host/event closure. This pass produces no runnable IRQ path and does not
admit a timer, V86 monitor, device response, or DOS service.

### Original Timer And Sound Owner Pass

The original `host/src/nt_timer.c` object is now present in SM-2. It resolves
the historical `host_timer_init`, `host_GetSysTime`, `host_timer_event`,
`host_TimeStamp`, heartbeat state, and resume-thread exports. Only the two
foundation guards that it actually owns are withdrawn in SM-2; the timer code
remains the historical owner of tick/heartbeat order. Its newly exposed
requirements are preserved as evidence: host thread and critical-section
operations, PIF priority state, event scaling, serial/LPT/floppy and DEM
heartbeat calls, sound, console state, and native timing imports.

The associated original `host/src/nt_sound.c` object is separately gated and
then included in SM-2. It resolves `host_timer2_waveform`, `host_alarm`,
`InitSound`, `PlayContinuousTone`, and the bell path. Its native beep-device
imports remain visible; no replacement beep, timer waveform, or speaker port
policy is introduced. Together these two passes still produce only a failed
closure link, not a running heartbeat or guest timer.

### Original Event-Thread Owner Pass

SM-2 now includes original `host/src/nt_event.c`. This resolves the timer
owner's `GetScaleEvent` call and the original keyboard hardware queue helpers
`KbdHdwFull`, `WaitKbdHdw`, and `HostReleaseKbd`. It does not define the
foundation's `host_set_hw_int`, `host_swint_hook`, or mouse-install guards;
those remain downstream device/CPU callbacks rather than being misclassified
as event-thread behavior.

The resulting closure exposes the historical event/session surface exactly as
implemented: console input wait and injection, console display operations,
mouse focus/display operations, VDD block/resume hooks, screen-refresh and
scale handling, PIF key enablement, stream I/O, and event-thread lifecycle.
These are not admitted as a replacement console or input product. They are
the evidence needed to define an eventual contained modern host event/console
seam while keeping historical queue, keyboard, mouse, and timer ownership in
their original modules.

### `nt_cpu.c` Generated-ABI Classification Pass

`host/src/nt_cpu.c` is not the CCPU interpreter and does not own BIOS, video
ROM, EMS, or DOS dispatch.  Its CCPU-relevant normal-path contribution is the
small `host_set_hw_int() -> cpu_interrupt(CPU_HW_INT, 0)` notification.  The
same translation unit also carries the historical `CPU_40_STYLE` register
facade (`InitNtCpuInfo` and `nt_cpu_info`), which directly addresses generated
`GDP_PTR`/`GLOBAL_*` state and consumes the generated `cpu_c.h` constraint
layout.

On 2026-08-07, its isolated clang-cl/x86 compile gate first failed because
the generic profile selected an unrelated host `cpu_c.h`; selecting the staged
CCPU/CVIDC `cpu_c.h` removed that enum error but exposed the required paired
generated state layout: `GDP_PTR`, `GLOBAL_CurrentUniverse`,
`GLOBAL_nanoEax`, and the related `GLOBAL_*` macros.  The CCPU and CVIDC trees
contain similarly named but layout-coupled generated headers, so choosing one
header directory cannot safely complete the source.  The provisional include
selection was removed; the checked-in build graph is unchanged.

This proves a source-layout boundary, not a missing modern shim.  Recreating
or mixing the register-layout headers would amount to implementing a distinct
CPU monitor ABI and risks silently corrupting CPU state.  Therefore SM-2 keeps
the existing fail-closed `host_set_hw_int`/`host_clear_hw_int` guards, and
`nt_cpu.c` remains a default-disabled compile-only reference until a complete,
matched generated CPU-state input set is recovered and independently
validated.  It is not a prerequisite for classifying video, EMS, ROM, BIOS,
or DEM ownership.

The source also establishes that this blocker is not on the CCPU execution
entry itself: under the selected CCPU profile `host_start_cpu()` is only a
one-line `cpu_simulate()` wrapper, and `cpu4gen.h` aliases that name to
`c_cpu_simulate()`.  The original `ntstubs.c` provides the corresponding CCPU
interrupt forwarders (`c_cpu_interrupt(CPU_TIMER_TICK, ...)` and
`c_cpu_interrupt(CPU_HW_INT, ...)`).  A future software-monitor path may call
the verified CCPU entry and these original forwarders directly, but must keep
the current guards until it has a separately validated interrupt-delivery
contract.  It must not revive the incompatible `CPU_40_STYLE` register facade
as a shortcut.
