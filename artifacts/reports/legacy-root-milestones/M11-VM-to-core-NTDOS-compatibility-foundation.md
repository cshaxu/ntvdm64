# M11 VM-to-Core Compatibility Foundation Research

Status: read-only research.  No source move, API, build change, runtime loader,
or Microsoft binary is introduced by this report.

Scope: establish the target ownership of current `src/vm` capabilities if the
repository is to grow both (1) `nxvm.exe`, a bootable configurable whole-PC
product, and (2) `ntvdm64.exe`, a BYOB adapter for a Microsoft NTDOS guest
runtime.  Native/owned DOS is deliberately out of scope.  It may later consume
the resulting core directly, but does not decide this design.

## Executive Decision

The core should grow outward.  It should become the shared **x86 guest
compatibility foundation**, not merely a collection of CPU and bus primitives.
That foundation can contain optional PC-compatible constructs when they are
reusable by the bootable VM and an NTDOS guest profile.

This is not a wholesale `vm -> core` move.  The decisive test is:

> A component belongs in core when it is an independently testable guest or
> host capability with no NXVM user-policy or Microsoft-runtime-private
> meaning, and when the VM and an NTDOS profile can select it through a public
> contract.

This permits optional core capabilities.  A non-booting future runtime need
not instantiate a floppy controller or a PC ROM to justify those capabilities
being core-owned.

The target shape is:

```text
core/machine                         core/platform
  x86 execution, memory, buses         generic host capabilities
  PC-compatible devices                file/stream/clock/process contracts
  ROM/firmware and bootstrap contracts        |
  generic guest transition mechanism           |
            \                              /
             \                            /
              vm composition       vdm composition
              NXVM policy          NTDOS BYOB profile/policy
              boot/media/UI        ABI adapter/containment/CLI
```

No new `mantle` ownership layer is necessary.  A mantle whose sole purpose is
to hold reusable machine capability would duplicate core.  Product roots still
own session creation, choice of profile, provider binding, threading, UI, and
teardown.  The NTDOS ABI adapter remains a VDM runtime-profile component: core
can dispatch a generic transition but must not know a BOP ID, an NTIO layout,
or a Microsoft component name.

## Current-State Map

`core/machine` already owns an explicit `core_machine` instance and the
lowest guest mechanics: 8086 through 80386 CPU profiles, FPU profile, RAM and
physical mappings, ports, PIC, PIT, DMA, KBC, VADP/CGA-EGA state, trace/debug,
bounded execution, reset/stop/fault state, and profile-time borrowing.  It
also has provider-slot patterns for block and display, although those slots
are currently session-owned by VM rather than part of the machine.

`vm` currently adds the following layers:

| Current area | Present contents | Architectural observation |
| --- | --- | --- |
| `vm/machine` | MC146818 CMOS/RTC, Intel 8272A FDC, floppy medium, ATA-like PIO HDC, hard-disk medium, legacy debug | Mostly guest chipset/controller behavior, but media code directly opens host files. |
| `vm/profile/default_profile` | PC/AT port/IRQ/DMA/ROM/default-memory descriptor; keyboard and mouse mappings | The descriptor is a useful PC compatibility model, but mixes structural facts with NXVM defaults. |
| `vm/profile/default_profile/firmware` | RAM-written F000 ROM image, BDA/IVT data, POST, INT 08/09/10/11/12/13/15/16/1A/40 snippets, boot sector handoff | Its generic assembler/ROM-loading mechanism is reusable; its exact firmware image is not yet proven NTDOS-compatible. |
| `vm/composition/session` | Construct device instances, connect ports/IRQ/DMA, mount images, install firmware, run/reset/refresh sequence | Correct product composition role, but it is the only place that knows how to assemble the current PC. |
| `vm/platform` | console/window rendering, host input translation, kernel/display threads, image command paths | Whole-machine UX and policy; not NTDOS Host ABI. |

The supplied NTDOS research sample establishes that `NTIO.SYS` is coupled to a
prepared virtual machine, BIOS/ROM/device behavior, memory layout, and private
guest-host transitions.  It does **not** establish which exact current PC/AT
implementation, port, ROM byte, or controller it requires.  Every NTDOS
claim below is therefore marked by confidence:

* **A - current evidence:** needed to make shared core ownership correct now.
* **B - likely PC compatibility:** a reasonable reusable capability, but exact
  NTDOS consumption must be observed.
* **C - deferred:** current VM-only code or insufficient evidence; do not move
  it merely in anticipation.

## Required Core Boundaries

### 1. `core/machine` becomes a compatibility foundation

It retains sole ownership of mutable guest state.  It should add public
contracts for PC-compatible construction, but not a product session:

```text
core_machine
  CPU/FPU, RAM, A20, port bus, IRQ/DMA/timers, KBC, video
  optional device instances selected while configuring
  firmware/ROM registry and executable image mapping
  boot-entry / prepared-entry state
  generic guest-transition dispatcher
```

The configuration lifecycle must remain `create -> configure/bind -> freeze ->
reset/run -> destroy`.  A device or transition provider may retain only a
child reference authorized during configuration; it cannot acquire a hidden
selected-machine global.

### 2. `core/platform` is capability, not DOS policy

Current core platform only provides host-surface leases, display-frame data,
and sleep.  It needs policy-free capability contracts before NTDOS can have a
non-invasive Host ABI:

```text
core/platform/file       opaque file/directory/metadata/read/write/flush
core/platform/stream     console-like byte streams and terminal control
core/platform/clock      wall-clock snapshot and monotonic scheduling source
core/platform/process    optional opaque child-process request/result
core/platform/media      selected host-backed block-image open/flush contract
core/platform/locale     immutable locale/code-page data acquisition
```

These APIs receive opaque handles and neutral byte buffers, never DOS paths,
drive letters, PSP addresses, BOP registers, guest pointers, or permission
decisions.  A VDM composition chooses a namespace root, read/write policy,
process allow-list, cancellation policy, and whether any capability is bound.
The VM composition chooses image-mount policy and need not expose host files as
DOS drives.

### 3. A core mechanism may connect guest and host, but not translate NTDOS

Add a generic `core_machine_guest_transition_provider` only after tracing a
real transition.  Its shape should be roughly:

```c
typedef enum core_machine_guest_transition_kind {
    CORE_MACHINE_GUEST_TRANSITION_ROM,
    CORE_MACHINE_GUEST_TRANSITION_INTERRUPT,
    CORE_MACHINE_GUEST_TRANSITION_PORT,
    CORE_MACHINE_GUEST_TRANSITION_INSTRUCTION,
    CORE_MACHINE_GUEST_TRANSITION_SHARED_MEMORY
} core_machine_guest_transition_kind;

typedef struct core_machine_guest_transition {
    core_machine_guest_transition_kind kind;
    uint32_t selector;          /* generic configured selector, not BOP name */
    core_machine_cpu_state cpu; /* copied in/out state */
} core_machine_guest_transition;

typedef enum core_machine_guest_transition_result {
    CORE_MACHINE_GUEST_TRANSITION_UNHANDLED,
    CORE_MACHINE_GUEST_TRANSITION_RESUME,
    CORE_MACHINE_GUEST_TRANSITION_FAULT,
    CORE_MACHINE_GUEST_TRANSITION_STOP
} core_machine_guest_transition_result;
```

The interface must offer checked guest-memory copy helpers rather than a raw
host pointer.  The VDM NTDOS profile interprets the selector, validates the
runtime version, marshals its private structures, calls core-platform
capabilities through composition-owned providers, and writes guest results.
NXVM may bind no transition provider.  This preserves `core/machine`'s
prohibition on depending on `core/platform`.

## Migration Ledger: Machine and Chipset

### Already Core: retain, complete, do not duplicate

| Capability | Current core state | Required work for NTDOS/PC compatibility |
| --- | --- | --- |
| x86 CPU/FPU | CPU profiles 8086/80186/80286/80386; bounded executor; CPU execution context | Verify V86, protected-mode transitions, exception and descriptor behavior against the selected NTDOS version. Do not create another CPU path. |
| RAM/physical map | RAM, real/linear access helpers, mappings, reset | Make A20 semantics and a profile-selected conventional/HMA/UMB map explicitly observable and testable. |
| Port bus | ordered installation/conflict checking and provider callbacks | Keep as the only I/O-port authority. Device migration must bind through it. |
| PIC/PIT/DMA/KBC/VADP | reusable state already in `core_machine`; profile binding exposes child objects | Preserve existing reset and IRQ/DMA ordering while adding PC-compatible profile data. |
| execution/time | run budget, ticks, stop/fault/reset, execution provider | Add only a generic bootstrap/transition hook; do not put a product run loop in core. |

### Promote now: MC146818-compatible CMOS/RTC (**A**)

Current implementation: `vm/machine/cmos.{c,h}` models 0x70/0x71 index/data
ports; RTC registers 00h-0Dh and selected configuration registers; BCD/binary
and 12/24-hour modes; UIP, periodic/alarm/update flags; register-C
read-to-clear and IRQ deassertion; IRQ 8 through the PIC; and elapsed-tick
advancement.  Its connection is only CPU, PIC, port bus, configuration ports,
IRQ and tick rate.  These are core-machine concepts.

Target owner and split:

```text
core/machine/rtc_mc146818.{c,h}
  device register state; port provider; IRQ binding; reset; elapsed advance
  calendar source is an injected core-platform-neutral snapshot value

PC profile or product composition
  equipment byte, base/extended memory, drive types, boot order, initial date
  and policy deciding wall-clock snapshot versus deterministic test clock
```

Required public interfaces:

```c
typedef struct core_machine_rtc_config {
    uint16_t index_port, data_port;
    uint8_t irq;
    uint32_t ticks_per_second;
} core_machine_rtc_config;

typedef struct core_machine_rtc_nvram_defaults { /* selected profile data */ };
type_status core_machine_rtc_mc146818_bind(...);
type_status core_machine_rtc_mc146818_apply_nvram_defaults(...);
```

Do not preserve the VM name, direct CPU pointer if unused, or a host-time read
inside the device.  A configuration profile supplies initial NVRAM; a
composition refreshes calendar state through a clock capability if desired.
NXVM uses its existing defaults.  An NTDOS profile can select compatible CMOS
contents without copying VM UI policy.

### Promote after backend split: Intel 8272A FDC (**B**)

Current implementation: `vm/machine/fdc.{c,h}` models the 8272A command,
execution and result phases; DOR/MSR/DR/DIR/CCR; Specify, seek, read-ID,
read/write/format command state; status ST0-ST3; IRQ 6; and DMA channel 2.
Its configured ports are 3F2h, 3F4h/3F5h, and 3F7h.  It binds a core DMA
request and core PIC IRQ source, which is strong evidence that the controller
belongs beside those core devices.

It cannot move unchanged because it directly names `t_fdd`, and `t_fdd`
contains a RAM image plus VM-owned file insertion/removal routines.  Split it:

```text
core/machine/floppy_controller_8272a
  port/IRQ/DMA state machine and a core block-medium provider

core/machine/floppy_medium
  guest-visible CHS geometry, read/write/format, media-change generation
  no host pathname and no stdio

core/platform/media (or composition provider)
  opens, reads, flushes and closes a user-approved image source

vm composition
  creates/mounts/ejects images and exposes Console commands
```

The core media interface must represent drive identity, media-change events,
write protection, CHS and sector size.  The current single block slot is not
sufficient: it supports one geometry and CHS transfers but cannot distinguish
floppy versus hard disks, multiple drives, or operation completion/error
classes.  Do not migrate FDC before replacing that interface.

NTDOS relevance: normal direct NTDOS startup may not require FDC.  Keep it
optional.  It becomes shared once an observed NTDOS configuration invokes its
BIOS/device path, or when the project declares PC-compatible boot media a
shared capability.

### Promote after block-contract redesign: ATA-like PIO HDC (**B**)

Current implementation: `vm/machine/hdc.{c,h}` accepts a narrow primary ATA
configuration: data 1F0h, features/error 1F1h, sector registers 1F2h-1F6h,
status/command 1F7h, alternate status/device control 3F6h, IRQ 14, 16-bit
data port, 8-bit task-file registers, LBA28, one master and no secondary
channel.  It implements BSY/DRDY/DSC/DRQ/ERR state, command data phases and
512-byte sector buffers, and exposes a core port provider plus PIC IRQ source.

These controller mechanics are core candidates.  `t_hdd` is not: it is a
VM-specific RAM image that calls `STD_FOPEN`, `STD_FREAD`, and `STD_FWRITE` on
paths chosen by Console/session code.  The correct split is identical in
principle to FDC, but with an LBA-capable block backend:

```text
core/machine/ata_pio
  task-file registers, data PIO, IRQ, reset and identify/read/write behavior
core/machine/block
  drive registry, geometry + LBA sectors, synchronous result/error contract
core/platform/media
  host image open/flush only after product authorization
vm composition
  mount/eject/create command policy and default drive order
```

The existing `core_machine_block_provider_slot` is a seed, not yet the needed
contract.  It needs stable `drive_id`, `sector_count`, `bytes_per_sector`,
LBA read/write, CHS translation only where requested, write-protect/media
generation, reset/freeze/lifetime, and an error enum.  The current VM binding
sets geometry but supplies null read/write callbacks, which confirms that the
firmware/controller paths are not yet a clean shared block boundary.

NTDOS relevance is unproven for direct bootstrap.  It is important to a
bootable PC-compatible profile and potentially to INT 13h probing; retain as
an optional core device, not an NTDOS prerequisite.

### Move the generic firmware image framework, not the current ROM personality (**A/B**)

Current `vm/profile/default_profile/firmware/bios.c` is a useful concrete
example.  It allocates and writes code into F000:0000, initializes BDA at
0040:0000, builds all IVT entries, appends POST snippets and boot code, writes
hard-disk parameter tables, and leaves a reset jump at F000:FFF0.  It uses
only core RAM and a core block-provider slot, so its *firmware mapping and
assembly framework* is reusable.

But its content is a compact NXVM PC/AT ROM personality: fixed BDA values;
one keyboard table; text/limited EGA INT 10h; INT 11/12/15; IRQ0/INT 1A;
IRQ1/INT 16; FDC and HDC INT 13h; INT 40h; boot choice and boot-failure loop.
The values include a 16 MiB profile, 80x25 text assumptions, one hard disk,
and fixed ROM pointers.  That image must not be promoted as “the NTDOS BIOS”
without binary trace evidence.

Target split:

```text
core/machine/firmware
  service registry with callable providers, ordered POST, ROM segments,
  IVT entry installation, reset-vector mapping, checked image writes,
  declarative BDA/CMOS/block/display dependencies

core/machine/pc_compat
  reusable PC-compatible firmware service contracts and optional reference
  services only where independently tested

vm/profile/default_profile
  current ROM byte sequences, default boot policy, Console-visible messages

vdm/runtime/ntdos
  exact discovered NTIO bootstrap image/entry data and private trap bindings
```

This does **not** mean every BIOS must be generated from snippets, assembled,
or rewritten.  The core framework needs three equal image sources:

| Firmware source | Core action | Example |
| --- | --- | --- |
| immutable binary ROM | validate size/address/checksum policy and map/copy its supplied bytes into guest ROM space | an owner-authorized PC BIOS ROM supplied by a profile |
| synthesized firmware | compose declared POST/interrupt/data fragments and write the resulting bytes | the current small NXVM RAM-written default ROM |
| prepared environment | install only the discovered memory/IVT/device state and entry CPU state; no BIOS image is required | a future NTDOS direct bootstrap, if evidence supports it |

The public core contract should accept an already-formed byte image or an
image provider; it should not expose the assembler as a BIOS requirement.
The current `aasm32` use is only one VM-profile implementation technique and
may remain there.  A binary-ROM profile must be able to bypass it completely.

The existing `core_machine_firmware` is only a descriptor array.  It must gain
provider callbacks and an explicit freeze order before it can own this role.
Its interface must distinguish POST action, ROM mapping, interrupt service,
and bootstrap entry.  It should also allow an NTDOS profile to request a
prepared environment without pretending it has performed a disk boot.

### Split the PC/AT descriptor (**A**)

`vm_profile_default_pc_at_descriptor` already records: CPU/FPU, memory,
timing, ROM mapping/reset vector, CMOS defaults, PIC/PIT/DMA/KBC/video/CMOS/
FDC port ranges, IRQ/DMA routes, primary ATA PIO details, and firmware hook
list.  This is nearly the declaration required for a core profile contract.

It mixes three layers and must be separated:

| New core descriptor data | Keep product/profile data |
| --- | --- |
| port ranges; IRQ/DMA routes; ROM mapping; memory-map and A20 policy; device presence; CPU/FPU compatibility; firmware service requirements | default 16 MiB selection; boot floppy versus HDD choice; image paths; Console commands; whether graphics opens a window; exact default ROM content |

Proposed contracts:

```c
typedef struct core_machine_pc_compat_topology { ... };
typedef struct core_machine_rom_mapping { ... };
typedef struct core_machine_boot_entry {
    enum { CORE_MACHINE_BOOT_RESET, CORE_MACHINE_BOOT_PREPARED } kind;
    core_machine_cpu_state initial_cpu;
} core_machine_boot_entry;
typedef struct core_machine_profile_provider {
    type_status (*configure)(core_machine *, void *context);
    type_status (*reset_prepare)(core_machine *, void *context);
} core_machine_profile_provider;
```

The crucial addition is two equal entry types.  `RESET` means architectural
reset then firmware/boot execution; `PREPARED` means verified CPU, IVT, BDA,
ROM/device and guest memory state supplied by a profile.  NXVM chooses the
first.  An NTDOS adapter may choose the second after evidence determines the
actual NTIO contract.  Neither is encoded as a special mode of the other.

### Keep input guest mechanics core; carefully split host mapping (**A/B**)

KBC and guest scan-code submission already belong to core.  Current VM host
input travels through `vm/platform` request queues and then uses default
profile keyboard/mouse maps to create PS/2/set-1 guest events.  The following
split avoids making Win32 key policy a machine concern:

```text
core/machine: KBC queues, IRQ 1, PS/2 guest protocol, scan-code ingress
core/platform: optional normalized host-key event contract
core/machine/pc_compat: reusable normalized-key -> set-1 mapping data, if
                        consumed by VM and NTDOS profiles
vm/platform: Win32/Linux capture, focus and Console/window behavior
vdm composition: runtime-specific input/cancellation and focus policy
```

No evidence yet says NTDOS needs the exact NXVM keyboard mapper.  Promote the
data table only after it is declared as a common PC input profile; do not move
host message handling or window focus logic.

### Video: keep the device core, promote only BIOS-facing contracts (**A/B**)

VADP state, ports 3C0h/3C4h/3CEh/3D4h-3DAh and copied text/graphics snapshots
are already core.  VM supplies a default ROM INT 10h subset that mutates BDA,
video memory and CRTC registers, plus display presentation threads.  The core
need is an explicit firmware-to-video service contract and a stable snapshot
provider binding; it is not another renderer.

The current supported surface is 80x25 text, CGA 320x200x4 and EGA 320x200x16.
An NTDOS runtime may require only text/video BIOS behavior, or may depend on
additional VDD-like semantics; no claim can be made until observed.  Keep the
current INT 10h bytes VM-profile-private while promoting the generic firmware
hook and video capability descriptors.

### Debug: do not move `vm/machine/debug` into core/machine (**A**)

Core already has machine debug contracts and `core/product/debug` owns shared
debug UX.  The VM legacy debug object includes retained Console behaviors and
file recording.  Its future direction is adaptation to core debug/product
contracts, not elevation into the machine foundation.  NTDOS diagnostics
should bind a core debug target through VDM composition.

## Firmware and Device Interaction Table

| Function | Current guest-visible path | Core target | Product-specific residue |
| --- | --- | --- | --- |
| Reset | CPU reset -> F000:FFF0 -> RAM-written ROM POST | reset-vector and boot-entry provider | NXVM boot selection; NTDOS prepared bootstrap values |
| Time | PIT/IRQ0 + CMOS/IRQ8 + INT 1A | PIT, RTC device, BIOS-service contract | calendar source choice and defaults |
| Keyboard | host event -> KBC -> IRQ1 -> INT 09 -> BDA -> INT 16 | KBC, normalized ingress, firmware hooks | host capture/focus, key policy |
| Display | INT 10/BDA -> VADP ports/memory -> snapshot -> renderer | VADP and firmware/display contracts | exact INT 10 content and renderer/window policy |
| Floppy | INT 13/40 -> 8272A ports -> DMA2/IRQ6 -> medium | optional FDC + medium/provider contract | image mount/eject and VM commands |
| Hard disk | INT 13 -> ATA ports -> IRQ14 -> medium | optional ATA + drive registry | image mount/eject and default boot drive |
| NTDOS host request | NTIO/NTDOS private transition -> host service | generic transition dispatch + checked copies | NTDOS ABI/version table, DOS mapping and permissions |

## NTDOS-Specific Adapter: Strict Non-Core Boundary

The BYOB NTDOS profile owns four things and only those things:

1. a manifest of user-supplied components (logical identity, version,
   architecture, hash, role, dependency class, ABI profile, verdict; never a
   local path or a binary copy);
2. the discovered bootstrap procedure: which guest files load where, initial
   CPU/IVT/BDA/ROM state, CONFIG.NT/AUTOEXEC.NT processing, and program handoff;
3. private guest-host ABI decoding: BOP/ROM/vector/port/shared-memory event,
   register contract, guest structure marshalling, callbacks and errors;
4. a binding to VDM policy providers for filesystem namespace, console,
   process, memory services, network/CD-ROM/printer if individually enabled.

The adapter may request a core PC-compatible profile, but it may not modify
core based on a Microsoft version.  If a required behavior is generic and
independently tested (for example A20 or an interrupt-chain facility), promote
the mechanism.  If it is a particular NTIO selector or structure offset, keep
it in the adapter.

## Host Capability Boundary and Required Interfaces

The current VM media classes directly use C file APIs from `vm/machine`; this
is the most immediate architectural violation for a broader core.  Core
machine must operate on guest media or provider callbacks, never a host path.

The minimum platform capability set should use opaque context and explicit
results.  Illustrative contracts:

```c
typedef struct core_platform_file_provider {
    type_status (*open)(void *ctx, const core_platform_path *path,
                        core_platform_open_mode mode, core_platform_file **out);
    type_status (*read)(void *ctx, core_platform_file *, uint64_t offset,
                        void *buffer, size_t bytes, size_t *done);
    type_status (*write)(void *ctx, core_platform_file *, uint64_t offset,
                         const void *buffer, size_t bytes, size_t *done);
    type_status (*metadata)(void *ctx, core_platform_file *,
                            core_platform_file_metadata *out);
    void (*close)(void *ctx, core_platform_file *);
} core_platform_file_provider;

typedef struct core_machine_block_device_provider {
    type_status (*describe)(void *ctx, core_machine_block_device_info *out);
    type_status (*read_lba)(void *ctx, uint64_t lba, void *buffer,
                            uint32_t sectors, core_machine_block_error *out);
    type_status (*write_lba)(void *ctx, uint64_t lba, const void *buffer,
                             uint32_t sectors, core_machine_block_error *out);
    type_status (*flush)(void *ctx);
} core_machine_block_device_provider;
```

Those examples are contracts to design, not declarations to add now.  The
first accepts product-authorized paths or opaque file tokens; the second never
sees a path.  VDM containment sits before the file provider and can deny every
request.  VM mounting is an explicit user operation.  Neither exposes a
default host directory to any guest.

Process launch deserves separate caution.  NTDOS may expose historical Win32
launch semantics, but generic `core/platform/process` can only define opaque
spawn/result primitives.  Program classification, command-line quoting, DOS
environment construction, parent-console ownership, and permission remain
VDM policy.  No process launch capability is needed to promote PC devices.

## Recommended Migration Order

This is an ownership and interface order, not implementation authorization.

1. **Architecture decision and tests first.** Amend the module authority so
   core may own optional shared PC-compatible mechanisms, while concrete
   NXVM policy stays VM-owned.  Record the no-NTDOS-private-data invariant.
2. **Firmware contract.** Replace the descriptor-only core firmware registry
   with callable, frozen provider bindings; add ROM mapping, IVT, POST and
   reset/prepared-entry concepts.  Keep existing default ROM bytes in VM.
3. **PC topology descriptor.** Split the current default PC/AT descriptor into
   core structural topology and VM default choices.  Prove that NXVM behavior
   is byte/boot compatible after each move.
4. **RTC/CMOS.** Move MC146818 mechanics to core; make calendar source and
   NVRAM defaults injected.  This is small, low-risk, and exercises the model.
5. **Block registry.** Replace the single CHS slot with multi-drive, LBA-aware
   provider contracts and media lifecycle.  Do not change VM media UX yet.
6. **Controller extraction.** Move 8272A and ATA PIO mechanics only after they
   consume the new media contracts.  Leave image pathname operations in VM
   composition/platform.
7. **Narrow NTDOS experiment.** With owner-provided binaries only, determine
   whether a prepared core PC profile reaches NTIO.  Add no private transition
   feature until a trace supplies its exact contract.
8. **Generic transition mechanism.** Promote only the observed common machine
   hook; implement NTDOS decoding in the VDM profile.  Progress through
   bootstrap, NTDOS init, COMMAND, basic COM, file I/O, then optional XMS/DPMI,
   redirector, CD-ROM and process features.

## Non-Migration List

The following must remain outside core even after the expansion:

* `vm/platform` Console/window renderer selection, UI threads, host focus,
  image commands and retained NXVM Console vocabulary;
* VM session construction, media paths, default boot drive, termination and
  boot-failure UX;
* NTDOS component loading logic, binaries, hashes in source, BOP numbers,
  private structures, version shims and BYOB eligibility policy;
* VDM drive mapping, host filesystem containment, process consent,
  cancellation and CLI behavior;
* any source that directly opens a user-selected host path while modeling a
  guest device.

## Risks and Falsifiers

The main risk is promoting the current minimal PC/AT ROM as though it were a
faithful NTVDM/SoftPC environment.  A failed NTIO bootstrap may be caused by
missing BIOS semantics, different ROM layout, protected-mode behavior, A20,
device timing, BOP dispatch, host ABI, or a mismatched Microsoft component
set.  It is not evidence that the core topology is wrong.

Conversely, if a traced Microsoft runtime uses a completely private SoftPC
machine surface that cannot be represented as normal BIOS/device/transition
contracts, only the generic execution hook belongs in core; the private
emulation remains an NTDOS profile implementation.  The project must not
contaminate the shared PC foundation with one opaque historical ABI.

## Acceptance Evidence for a Future Go Decision

For each migrated capability, require: VM boot regression; device-level port,
IRQ/DMA/reset tests; provider freeze/lifetime tests; no `core/machine ->
core/platform` include; no guest state in `core/platform`; and at least one
second consumer or an explicit NTDOS trace-based justification.  For NTDOS,
add version-locked BYOB manifest validation and progressive runtime gates.
The default build, default release and ordinary NXVM boot must remain
independent of any user-supplied Microsoft file.

## Read-Only Evidence

Repository evidence was read from the current `ntvdm64` tree, principally:

* `src/core/machine/machine_interface.h`, `execution_provider.h`,
  `firmware_interface.h`, `block_{interface,provider}.h`, and
  `display_interface.h`;
* `src/vm/machine/{cmos,fdc,fdd,hdc,hdd}.{c,h}`;
* `src/vm/profile/default_profile/pc_at_profile.{c,h}` and firmware sources;
* `src/vm/composition/session/{session,machine_devices,profile_firmware,
  provider_lifecycle,block}.{c,h}`;
* `src/vm/platform/platform.h` and `src/core/platform/host_surface_interface.h`;
* the repository architecture records and the owner-supplied NTDOS Discovery
  Draft v2.

No Microsoft binary, local binary path, component loader or external source
was read or added.
