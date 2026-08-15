# M11 NTVDMx64 Direct-Source Evidence Report

Status: source-reading research only.  No NTVDMx64 code, Microsoft binary,
binary content, machine-local path, loader, BOP dispatcher, or host patch is
introduced into `ntvdm64`.

Purpose: raise the previous NTDOS architecture report from a secondary summary
to the level supported by directly inspected NTVDMx64 repository material.
This report distinguishes what the local checkout proves from what remains
unavailable because the checkout is a patch/build project rather than a full
redistribution of the original NTVDM source tree.

## Result In One Sentence

Direct source evidence confirms that the historical NTVDM model is a coupled
guest machine plus a privileged/private host integration system: it has a
machine-level BOP trap and 256-entry dispatch surface, I/O/interrupt/page-fault
transition paths, DOS/DPMI mode-switch handling, PC BIOS service families,
console registration, loader/process interception, and a multi-file guest
runtime release set.  It **does not** justify copying the NTVDMx64 host model:
its own code relies on injection, internal Windows symbols, CSRSS/ConHost
hooks, and sometimes a custom HAXM driver.

This strengthens the core-expansion conclusion: `core/machine` needs generic
guest transition, PC-compatible firmware/device/profile and prepared-entry
mechanisms; `core/platform` needs explicit safe capabilities.  It also
strengthens the boundary: Microsoft BOP meanings and all Windows-private host
machinery remain outside core and outside a releasable default product.

## Material Actually Inspected

The inspected checkout is the `leecher1337/ntvdmx64` work tree at revision
time available locally.  Its `readme.txt` identifies the project as patches to
the original NT4 NTVDM for 64-bit Windows, not a clean-room implementation.
`ntvdmpatch/doc/minnt.txt` explicitly requires separately supplied MinNT and
old NT4 source archives, then copies the patch tree into that source tree.

The checkout consequently contains these directly readable classes:

| Class | Directly inspected examples | Evidentiary value |
| --- | --- | --- |
| NTVDMx64-owned source | `ntvdmpatch/src/ldntvdm/ldntvdm/*.c` | Strong evidence for its host loader/injection strategy. |
| HAXM monitor source | `ntvdmpatch/experimental/haxm/v86/haxm/i386/{monitor.c,monitorp.h,int.c}` | Strong evidence for a concrete NTVDM guest transition and execution model. |
| MinNT fixes | `minnt/minntfix/.../softpc.new/host/src/nt_w10.c` | Strong evidence for historical console-registration ABI adaptation. |
| Build/release scripts | `mkrelease-minnt.bat`, `doc/minnt.txt`, `doc/patches.txt` | Strong evidence for component graph and required external source/build environment. |
| Patches against omitted original files | `patches/common/dpmi.patch`, `patches/minnt/*.patch` | Evidence that named source locations/behaviors exist; not a substitute for the omitted complete file. |

The local `minntfix/.../base/mvdm/dos/v86` contains only four files and the
local `softpc.new` fix subtree only five files.  Searches found no local
`sysinit1.asm`, `srvcall.asm`, or `biosbop.inc`.  Consequently, those facts
are not attributed to NTVDMx64 itself.  They are now independently verified
against the public OpenNT source reference in the companion
`artifacts/reports/legacy-root-milestones/M11-OpenNT-NTDOS-source-evidence.md`
addendum. The version-specific runtime
claims still require a lawful BYOB dynamic trace.

## Direct Findings

### 1. BOP is a real machine transition, not a speculative abstraction

The HAXM monitor directly implements a BOP interception path.

* `experimental/haxm/v86/haxm/i386/monitor.c`, `cpu_simulate`, recognizes the
  byte prefix `C4 C4` in both debug and invalid-opcode/NMI exit paths, advances
  RIP by three bytes, then calls `EventVdmBop`.
* `EventVdmBop` takes the third byte as the BOP selector and invokes
  `BIOS[iobuf[2]]()` after range checking.
* `monitorp.h` sets `MAX_BOP` to 256 and declares the handler families:
  reset, timer, keyboard, diskette, video, equipment/memory, disk, serial,
  printer, bootstrap, command installation/loading, redirector, EGA, EMS,
  RTC, mouse, protected-mode switching and control BOPs.

This directly supports a generic core guest-transition mechanism.  The
correct core abstraction is an intercepted guest event with copied CPU state,
checked guest memory access, ordered registration and explicit resume/fault/
stop outcome.  It must not be an API named after BOP: the same mechanism can
support a ROM entry, interrupt hook, I/O port, page fault or other future
guest protocol.

The *meaning* of selector values must remain NTDOS-profile code.  The handler
table proves the private protocol is broad and machine-facing; it does not
make its numeric ABI an appropriate core contract.

### 2. The monitored guest is a full execution environment

`monitor.c` establishes a CPU context with real-mode segment bases and limits,
GDT/IDT limits, CR0 clear, interrupts enabled, and an NTVDM shared state word.
It starts a vCPU, handles hardware interrupt windows, and receives execution
exits for I/O, fast/coalesced MMIO, page fault, debug, NMI/invalid opcode, and
interrupt delivery.  `int.c` records timer and hardware pending state in the
shared VDM state, calls a timer event, acknowledges virtual interrupts, and
contains explicit protected-mode interrupt/IRET-hook handling.

This is direct evidence for all of the following architecture requirements:

* one explicit CPU/memory/interrupt execution context, rather than DOS code
  calling host services directly;
* guest event dispatch that can return to the executor with altered registers;
* an explicit protected-mode/V86 transition contract for DOSX/DPMI-class use;
* a deterministic guest time and interrupt model separated from host UI
  threads;
* profile-owned setup of IVT/IDT, segment and device state before runtime
  bootstrap.

It does **not** prove that the current NXVM PC/AT ROM is compatible with a
particular NTIO version.  It proves a prepared machine context is part of the
historical design.

### 3. PC/BIOS service families are in the host-machine side of NTVDM

The BOP handler declarations in `monitorp.h` include `bootstrap`, `diskette`,
`disk_io`, `keyboard_int`, `keyboard_io`, `video_io`, `ega_video_io`,
`time_int`, `time_of_day`, `rtc_int`, `equipment`, `memory_size`, `rs232_io`,
`printer_io`, `redirector`, `emm_*`, and mouse functions.  This is decisive
evidence against modelling NTDOS as only `NTDOS.SYS` plus INT 21h.

For this repository, the implication is nuanced:

* CPU, RAM/A20, ports, IRQ, DMA, timer, keyboard, video and generic firmware
  transition facilities are shared core-machine candidates.
* A PC-compatible device/topology profile is justified as an optional core
  capability.
* The exact FDC, ATA, serial, printer, EMS, mouse or video feature set is not
  yet an NTDOS minimum requirement.  The declarations establish family
  presence, not selector-to-component usage by a selected guest binary.
* Existing NXVM controllers should therefore be split into reusable controller
  mechanics and VM image/UI policy before promotion, as the companion
  VM-to-core report recommends.

### 4. DPMI is coupled to monitor services and CPU state

`patches/common/dpmi.patch` contains patch context showing DPMI code changes
behavior based on `MONITOR`, includes a BOP path for hung-application
processing, and uses setters for CS/DS/ES/IDT base/limit when monitor support
is not present.  `experimental/haxm/v86/haxm/i386/int.c` describes the
protected-mode interrupt path, descriptor/IDT handling and IRET hooks.

This directly validates the earlier risk assessment: DOSX/DPMI compatibility
cannot be reduced to an ordinary DOS file service.  Core needs correct
protected-mode and descriptor/interrupt mechanics; an NTDOS profile needs
version-specific bridging only where a historical BOP or monitor protocol is
actually observed.

### 5. The release is a runtime graph, not three files

`mkrelease-minnt.bat` copies, per language, `NTVDM.EXE`, `NTIO.SYS`,
`NTDOS.SYS`, versioned NTIO/NTDOS variants, `COMMAND.COM`, `CONFIG.NT`,
`AUTOEXEC.NT`, `HIMEM.SYS`, `DOSX.EXE`, `REDIR.EXE`, `MSCDEXNT.EXE`,
`VDMREDIR.DLL`, `NTVDMD.DLL`, VGA/BIOS ROM files, keyboard/NLS files and
utilities.  It conditionally includes `NetRap.dll` and `xactsrv.dll`.

This confirms the component-graph conclusion using NTVDMx64's own release
logic.  It does not imply every copied file is required for a smallest useful
profile.  A BYOB manifest must identify the selected version and test a
minimal set progressively instead of mixing the listed variants.

### 6. The historical Host ABI is intentionally unsuitable as a project ABI

The direct NTVDMx64 source shows several forms of intrusive host coupling:

* `softpc.new/host/src/nt_w10.c` adapts the private/legacy
  `RegisterConsoleVDM` signature for newer Windows.
* `src/ldntvdm/ldntvdm/symcachefunc.h` enumerates private symbols in
  kernel32, ConHost/ConHostV1, appinfo, ntdll and optionally ntoskrnl.
* `src/ldntvdm/ldntvdm/winevent.c` hooks console events and injects the loader
  into newly created processes.
* `src/ldntvdm/ldntvdm/xpcreateproc.c` intercepts image-format failures and
  private process/CSRSS paths to route DOS/NE execution toward NTVDM.
* HAXM monitor code opens a custom HAX device/service, creates a VM/vCPU and
  expects hypervisor exits for its guest transition model.

These are source-level proof that the original-host compatibility path is not
the desired architecture for a releasable NXVM/NTDOS project.  Reusing it
would violate the stated non-invasive BYOB objective and conflate product
launch policy with guest compatibility.

The project replacement is not to emulate every Windows private interface.
It is to provide a bounded VDM composition that selects safe core-platform
capabilities: explicit files/directories, byte streams/terminal, clock,
optional process handle, media, locale and user-approved containment.  The
NTDOS adapter translates only the discovered guest request surface to these
capabilities.  No process injection, system DLL patch, registry mutation,
CSRSS hook, ConHost version override or custom kernel driver is permitted.

## Revised Architectural Decisions

| Previous proposal | Source-evidence status | Revised decision |
| --- | --- | --- |
| Generic guest transition/trap in `core/machine` | Confirmed by HAXM BOP, I/O, fault and interrupt exits | Keep. Design as protocol-neutral dispatch and checked guest-copy surface. |
| Microsoft BOP dispatcher in core | Refuted by broad private handler table | Keep all selector meanings and structures in `vdm/runtime/ntdos`. |
| PC-compatible environment above basic CPU | Confirmed by BIOS handler families and monitor initialization | Make optional PC-compatible topology/firmware contracts core-owned. |
| NXVM's current default BIOS is NTDOS-ready | Not evidenced | Keep its byte image/profile private until binary trace tests it. |
| RTC/FDC/ATA controller migration | RTC/FDC/disk families are present; exact requirements unknown | RTC mechanics are a high-confidence core extraction; controllers remain optional and require backend split plus trace evidence. |
| `core/platform` equals current NTVDM host ABI | Refuted | Core platform must be a new safe capability layer, not a clone of Windows private ABI. |
| Directly launch guest binary is sufficient | Refuted | A bounded bootstrap/machine-state contract is required; exact NTIO state remains unverified. |

## What This Audit Cannot Yet Prove

The following claims remain explicitly unverified because the required original
source files are absent from this checkout and no BYOB binary was executed:

1. exact NTIO entry point, loader sequence, CPU registers, IVT/BDA contents,
   memory layout and bootstrap handoff for any specific NTIO/NTDOS version;
2. selected-version BOP parameters, guest buffer formats, reentrancy or
   asynchronous completion rules.  OpenNT verifies the historical selector
   names and dispatch shape, not a supplied binary's exact behavior;
3. whether a particular NTDOS profile invokes FDC, ATA, BIOS ROM or only a
   smaller virtual machine subset;
4. minimum working component set and whether versioned NTIO/NTDOS files can
   ever be mixed;
5. whether current `ntvdm64` CPU, A20, protected-mode, video or timing
   behavior meets a chosen NTDOS runtime's observable requirements.

The proper next evidence source is an owner-provided, version-locked BYOB
runtime plus a passive trace harness.  It should log bootstrap state and each
generic guest transition without loading Microsoft files into the repository
or turning the product into NTVDMx64's invasive launcher.

## Implications for the VM-to-Core Migration Report

The companion report remains correct but its confidence labels change:

* Promote generic ROM mapping, reset/prepared entry, callable firmware
  providers, generic transitions, PC topology contracts and RTC mechanics.
* Promote FDC/ATA controller mechanics only after the new multi-drive block
  contract separates them from host file paths; maintain them as optional.
* Do not migrate NXVM Console, image mount commands, renderer/thread policy,
  default boot selection, host path policy or any NTVDMx64-style loader hook.
* Treat host file/process/console capabilities as capability contracts in
  `core/platform`, then bind access policy in VDM composition.

## Source Location Index

All paths below are relative to the inspected `ntvdmx64` work tree.  They are
listed for reproducibility only; this report does not copy their code.

* `readme.txt`, sections “What is it?”, “How does it work?” and build flavours.
* `ntvdmpatch/doc/minnt.txt`, Requirements and setup steps.
* `ntvdmpatch/mkrelease-minnt.bat`, DOS release-copy list.
* `ntvdmpatch/experimental/haxm/v86/haxm/i386/monitor.c`, `cpu_init`,
  `cpu_simulate`, `EventVdmBop`.
* `ntvdmpatch/experimental/haxm/v86/haxm/i386/monitorp.h`, `MAX_BOP` and
  BOP handler declarations.
* `ntvdmpatch/experimental/haxm/v86/haxm/i386/int.c`, interrupt and DPMI
  support.
* `ntvdmpatch/patches/common/dpmi.patch` and
  `ntvdmpatch/patches/minnt/ccpu.patch`.
* `ntvdmpatch/minnt/minntfix/minnt/base/mvdm/softpc.new/host/src/nt_w10.c`.
* `ntvdmpatch/src/ldntvdm/ldntvdm/{symcachefunc.h,winevent.c,xpcreateproc.c}`.

No NTDOS executable, system file or binary contents were copied into this
research directory.
