# NT4/NT5 Local VDM Source Crosscheck

Status: research evidence, 2026-08-06. This report is read-only analysis of
local source trees. It does not import a source file, link a binary, enable a
runtime target, or change the NT4 research profile.

## Scope

The NT4 VDM model remains the required reference for the research runner:

```text
software CPU -> original BIOS[] -> MS_bop_0 -> DemDispatch -> DEM -> NTDOS
```

The local trees were used with distinct roles:

| Tree | Role in this review | Admission status |
| --- | --- | --- |
| `D:\home\repos.hobby\opennt` | OpenNT/NT4 source reference. | Read-only evidence. |
| `D:\home\repos.hobby\opennt-4.5\nt\private\mvdm` | NT4.5 source and historical i386 build/artifact reference. | Read-only evidence. |
| `D:\home\repos.hobby\win2000src\private\mvdm` | NT5 WOW/header subset comparison only. | Read-only evidence; not a complete NT5 VDM source tree. |

No NT5 source is an input to the NT4 runner. No binary from the NT4.5 tree is
copied into this research tree.

## CCPU Input Audit

The current default-disabled CCPU link diagnostic reports three names that
cannot be supplied by the already selected historical CCPU objects:

```text
c_sas_touch
c_VirtualiseInstruction
c_effective_addr
```

The local crosscheck rules out a simple source-copy omission:

| Name | OpenNT tree | NT4.5 tree | Conclusion |
| --- | --- | --- | --- |
| `c_sas_touch` | Declared and placed in the `cSasPtrs` vector by `base/mvdm/softpc.new/base/ccpu386/sascdef.c`; no definition under `base/mvdm`. | Same declaration/vector role; no definition under `nt/private/mvdm`. | Missing generated/proprietary CCPU input, not a normal source file awaiting migration. |
| `c_VirtualiseInstruction` | Same `sascdef.c` vector-only use; no definition under `base/mvdm`. | Same. | Missing generated/proprietary CCPU input. It must not be re-created as an ad hoc BOP or SVC handler. |
| `c_effective_addr` | Declared by the generated CCPU header `base/ccpu386/cpu4gen.h`; no CCPU definition found. | Same. | Missing CCPU-side generated/assembly input. The x86 monitor's ordinary `effective_addr` is a different symbol and calling model. |

This fact holds across both available NT4-era source snapshots. The correct
current disposition is therefore **proven unavailable from the admitted CCPU
source set**, not merely "not yet searched." A future input may be admitted
only with an exact source/provenance record and an ABI comparison against the
NT4 CCPU generated headers.

As a configuration crosscheck, the local OpenNT and NT4.5 copies of both
`softpc.new/obj.vdm/cdefine.inc` and `softpc.new/obj.vdm/sources` have matching
SHA-256 values. The NT4.5 tree therefore independently confirms the same CPU
selection and final-link shape; it is not a nearby variant that happens to
contain the missing CCPU definitions. A recursive scan also finds zero
definitions of the three names in its generated `base/cvidc` directory and in
the supplied NT5 `private/mvdm` subset.

## Historical Build Selection

`opennt-4.5\nt\private\mvdm\softpc.new\obj.vdm\cdefine.inc` selects the
following architecture profiles:

| Historical target host | Relevant defines | CPU path |
| --- | --- | --- |
| i386 | `MONITOR`, `C_VID`, `X86GFX` | Native V86 monitor path. `CCPU` is not selected. |
| non-i386 with `CCPU` selected | `CPU_40_STYLE`, `CCPU`, `C_VID`, `NEW_CPU`, `SPC386`, `SIM32`, `V7VGA` | Software CCPU fallback. |

`obj.vdm/sources` confirms the final link selection: on i386,
`CPULIBS` is the historical assembler CPU library; when `CCPU` is selected it
is `ccpu486.lib`. This explains why trying to make the source-released CCPU
library into an i386 VDM produces dependencies the original i386 product did
not carry.

The `base/cvidc` directory is a generated C/video-side input set, not evidence
that the three CCPU symbols are defined there. Its `sources` file selects
generated `SINIT*`/`SEVID*` units and related glue; a symbol scan found no
definition of the three missing CCPU names.

## x86 Monitor Evidence And Its Limit

The actual NT4.5 source includes
`v86/monitor/i386/sources`, which selects `i386/sas.c`. That source defines
the *non-CCPU* `effective_addr(Segment, Offset)` by forwarding to
`Sim32GetVDMPointer` with a protected-mode flag. The associated historical
i386 `ntvdm.exe` exports `Sim32pGetVDMPointer`, `host_simulate`, the standard
register accessors, and VDD interfaces.

This is useful evidence of original ownership, but it is not an admissible
implementation source for the current software-emulation route:

* It depends on the V86 monitor address model and `Sim32GetVDMPointer`.
* It defines `effective_addr`, not `c_effective_addr`.
* It does not define `c_sas_touch` or `c_VirtualiseInstruction`.
* Reusing it would reintroduce the user-rejected V86/monitor execution model.

Accordingly, the only permitted use is semantic comparison when specifying a
future neutral software-memory bridge. It cannot be copied into
`src/opennt/local/shim`, linked into the CCPU fixture, or used as a substitute
for a missing generated input.

## NT5 Comparison Boundary

`D:\home\repos.hobby\win2000src\private\mvdm` contains `inc`, `wow16`, and
`wow32`; it does not contain `softpc.new` or `v86/monitor` in the supplied
snapshot. It is consequently useful for later WOW/VDM public-private interface
comparison, but cannot prove NT5 CPU, SAS, BIOS, or DEM build closure.

The first NT4-versus-NT5 difference study should therefore begin only after a
complete NT5 VDM source input is identified. Its comparison table must retain
three columns: NT4 behavior, NT5 behavior, and whether either is relevant to
the neutral software-emulation contract. It must not treat a missing NT5 source
subtree as evidence that NT5 removed a feature.

### Windows XP SP1 Crosscheck

`D:\home\repos.hobby\winxpscodes\Source\XPSP1\NT\base\mvdm` is a complete
later MVDM source tree, unlike the supplied Windows 2000 subset. It contains
`softpc.new`, `v86/monitor`, the guest `dos/v86` tree, and historical ROM/profile
inputs. It is therefore a useful evolutionary reference, but it is not an NT4
input and must not be mixed into the NT4 build profile.

The XP SP1 result reinforces, rather than closes, the CCPU gap:

* Its `softpc.new/base/ccpu386/cpu4gen.h` and `sascdef.c` contain the same
  declarations/vector entries for `c_effective_addr`, `c_sas_touch`, and
  `c_VirtualiseInstruction`; no definition occurs under its `softpc.new` or
  `v86` trees.
* Its `obj.vdm/cdefine.inc` still selects `MONITOR`, `C_VID`, and `X86GFX` on
  i386 (with the later `ARCX86` addition); `CCPU` remains a non-i386 option.
* Its final link configuration likewise selects `monitor.lib` for i386 and
  selects `DASM386.LIB` plus `ccpu486.lib` only on non-i386 hosts.

Thus a full later source tree does not reveal a missing NT4 CCPU `.c` file.
The absent implementations were outside the released source inputs or were
generated/private build products across this lineage. XP does provide later
semantic reference for ROM/profile discovery, device organization, and host
seams, but its source and ROM artifacts remain research-only and inadmissible
as substitutions for the NT4 runtime.

## NT4.5 Final-Link Delta

The NT4.5 `softpc.new/obj.vdm/sources` file gives a more useful completeness
baseline than a collection of individual unresolved symbols. Its i386
`LINKLIBS` selects:

```text
MAIN.obj, FPRT.obj,
SYSTEM.LIB, BIOS.LIB, VIDEO.LIB, DOS.LIB, COMMS.LIB, DEBUG.LIB,
SUPPORT.LIB, DISKS.LIB, KEYMOUSE.LIB, SRC.LIB,
monitor.lib, dem.lib, dbg.lib, command.lib, dpmi32.lib, xms486.lib,
suballoc.lib, oemuni.lib
```

The current research CMake gates source-close parts of CCPU, the BIOS table,
`nt_bop`, DEM, and several device/provider libraries, but do **not** create an
equivalent final-link target. The following are still explicit closure gaps:

| Original NT4.5 link group | Source root observed locally | Current research status | Why it cannot be silently filled |
| --- | --- | --- | --- |
| `monitor.lib` | `v86/monitor/i386` | Semantic evidence only. | It is the native V86 monitor and is excluded from the software CPU runtime. |
| `DOS.LIB` beyond selected DEM | `softpc.new/base/dos` (`TARGETNAME=dos`). | Its source root is absent from the current `src/opennt` input. | It contains DOS/EMS and machine-state policy that must remain original and profile-controlled. |
| `SRC.LIB` / `FPRT.obj` | `softpc.new/host/src` | Individual compile gates only. | This is where console, process, threading, profile, timing, host resource, and monitor seams meet. |
| `DEBUG.LIB`, `dbg.lib`, `DASM386.LIB` | `softpc.new/base/debug`, `base/dasm386`, `mvdm/dbg` | Not admitted. | Debugger behavior is not startup evidence and carries host interaction policy. |
| `command.lib` | `mvdm/dos/command` | Not admitted. | It owns command/process handoff, not a generic CLI convenience layer. |
| `dpmi32.lib` | `mvdm/dpmi32` | Not admitted. | It depends on full protected-mode, selector, and interrupt contracts. |
| `xms486.lib` | `mvdm/xms.486` | Not admitted. | It owns XMS/A20/UMB behavior and has processor-specific sources. |
| `suballoc.lib` | `mvdm/suballoc` | Not admitted. | It is historical allocator state, not interchangeable CRT allocation. |
| `oemuni.lib` | `mvdm/oemuni` | Partial compile/facade research only. | It uses removed NT PEB/TEB/OEM assumptions and must be replaced only at its narrow host boundary. |

This table changes the near-term definition of progress: a successful
`BIOS[] -> MS_bop_0 -> DemDispatch` object chain is useful but is not a
candidate `ntvdm.exe`. A future default-disabled runner must declare which
original link groups it has restored, which exact modern-host seam substitutes
an unavailable platform dependency, and which groups are intentionally
profile-disabled. It cannot acquire completeness by linking a historical
NT4.5 executable or library binary.

The NT4.5 source files themselves provide the source-to-output mapping above:
each named root has a `sources` file whose `TARGETNAME` matches its final link
group. Comparing those roots with the current `src/opennt` confirms that the
following original inputs are not yet present there at all: `monitor`, the
SoftPC `dos`, `debug`, and `dasm386` libraries, plus `dbg`, `command`,
`dpmi32`, `xms486`, and `suballoc`. `oemuni` is present but remains only a
partial facade/compile-research input. This is an input-provenance inventory,
not permission to bulk-enable the groups or to combine their outputs.

## Guest Artifact Source Gap

The host-stack delta is separate from the guest artifact requirement. The
fixed `src/opennt` input currently contains `base/mvdm/dos/dem`, but not the
NT4.5 `mvdm/dos/v86` tree that historically builds the modified DOS runtime
and programs. Its root makefile orders these groups:

```text
doskrnl (dos + bios) -> dev -> lib/xmm -> cmd -> redir
```

The `cmd` makefile in turn enumerates `command`, `append`, `backup`, `edlin`,
`exe2bin`, `debug`, `qbasic`, `setver`, `mem`, `loadfix`, `nlsfunc`, `keyb`,
`graphics`, `fastopen`, and `share`. This is the source inventory needed to
study production of the wider NT-modified DOS file set; the current tree does
not yet contain it, so no claim of complete guest-system-file reconstruction
is supported.

Any later admission of this tree must preserve its directory layout and build
scripts under `src/opennt/base/mvdm/dos/v86`, carry a per-file manifest and
source restriction, and use a default-disabled guest-artifact target. It must
not import historical output binaries, make them a runtime dependency, or
conflate guest-artifact compilation with host-stack execution closure.

## Next Evidence Work

1. Search available historical build drops, generated-output archives, and
   processor-specific source packages for the three CCPU definitions, preserving
   the source's exact build/profile provenance.
2. If no source is available, keep the CCPU runtime path unadmitted and use the
   object-level BIOS/BOP/DEM chain only for static dependency analysis.
3. Independently inventory the original i386 monitor's memory/interrupt
   contracts as semantic evidence, without importing its V86 implementation.
4. Obtain a complete NT5 `private/mvdm` source input before asserting detailed
   NT4/NT5 VDM differences.
