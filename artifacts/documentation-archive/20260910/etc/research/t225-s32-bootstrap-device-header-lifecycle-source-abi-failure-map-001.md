# T225 S32 original bootstrap/device-header lifecycle source/ABI/failure map

## Scope and method

This is a source-first map of the whole original guest bootstrap package from
the NTIO entry through the first `DEVIOCALL2` device invocation. It records
what the current composition proves, what the original guest owns, and what
remains unproved. It is not a repair proposal: no guest state, BOP result,
device header, request packet, CPU wrap rule, or machine lifecycle is
synthesized by this record.

The only native observation retained by this map is S30's profile-neutral
`DS=0C41`, offset `FFFF`, two-byte direct-limit read. An observed segment
selector is not treated as proof that any named original routine dynamically
executed.

## Fixed current composition contract

The current initial plan writes the immutable NTIO image at physical `0700h`,
enters real mode with `CS=0070h`, and begins at offset `0714h`:

| Current boundary | Evidence | Contract | Classification |
| --- | --- | --- | --- |
| Initial NTIO staging | `src/bx-vdm/bx_ntvdm_composition_runtime_v1.c:347-366`; `src/bx-vdm/bx_ntvdm_startup_session.c:70-91` | Checked ordinary guest-RAM write at `0700h`; entry `0070:0714` | Proven bx-vdm startup boundary |
| DEM load-DOS ingress | `src/bx-vdm/bx_ntvdm_boot_namespace_plane_v1.c:111-114` | Only classified DEM `50:11` may use the NTDOS image seam | Proven host-service routing |
| 50:11 mechanical action | `src/bx-vdm/bx_ntvdm_dem_load_dos_service.c:17-42` | Requires real mode and exact `C4 C4 50 11`; writes exact authenticated NTDOS bytes to physical `(uint16)EDI << 4`; resumes at `fault_rip + 4` | Proven narrow write/resume seam |

Thus the adapter neither selects a substitute DOS load segment nor manufactures
a device chain. The original caller supplies `DI`; the adapter preserves its
16-bit segment interpretation and returns execution to the instruction after
the BOP. Direct/Readonly policy has no branch in this operation because the
only input is the already-copied immutable NTDOS image.

## Original lifecycle ledger

| Stage | Original owner and source contract | Required live state / ABI | Current provision and proof | Failure disposition |
| --- | --- | --- | --- | --- |
| 1. NTIO resident startup state | Original NTIO/sysinit guest code. `src/opennt/base/mvdm/dos/v86/doskrnl/bios/msinit.asm:197-208` stores `device_list` and `current_dos_location=dos_load_seg`. | Resident NTIO image; original data labels and initialized values. | NTIO image geometry is proven only as the bounded `0070:0714` startup plan. Its executed initialization is not proven by S32. | Do not prewrite `device_list` or `current_dos_location`; preserve as guest-continuity question. |
| 2. NTDOS load | Original `msinit.asm:244-252` computes `dosdatasg`, puts `dos_load_seg` in `DI`, issues `SVC_DEMLOADDOS`, then jumps to `sysinit`. | Real-mode `DI` is the original guest-selected destination; complete NTDOS byte image. | The exact 50:11 bytes, real-mode gate, `DI<<4` address conversion, bounded low-memory aperture, checked write, and `+4` continuation are proven by current bx-vdm code. | Reject a host-selected NTDOS segment or adapter-side continuation. Invalid ABI stops rather than claiming DOS loaded. |
| 3. Temporary NTDOS relocation | Guest `sysinit`, `src/opennt/base/mvdm/dos/v86/doskrnl/bios/sysinit1.asm:903-928`. It chooses a temporary segment above `sysinitseg`, copies `dossize`, obtains the DOS origin from loaded-image offset 3, records `dosinit`, and adjusts `current_dos_location`. | Executing original `sysinit`; intact loaded image, original BIOS/sysinit data, working real-mode copy instructions and stack. | The isolated S29/S30 first-fault record does not prove this stage. The retained S19 BOP sequence, correlated by S33, does prove later reach of original `sysinit`; it does not establish the exact temporary/final segment value. | Guest owner. Do not replace relocation with a fixed host buffer or infer state from selector values. |
| 4. DOSDATA and DOSINIT transition | Guest `DosInit`, `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msinit.asm:232-260`; `sysinit1.asm:956-1002`. `sysinit` passes `DS:SI=device_list`, `ES:DI=BiosComBlock`, and `DX=memory_size`, establishes its stack, then calls the recorded DOS init pointer. `DosInit` relocates DOSDATA into final low memory. | Correct temporary code/data relation; resident device-chain far pointer; BIOS communication block; memory size; original stack/interrupt prerequisite. | The retained S19 `50:0F` record, correlated by S33, proves original `DosInit` reached its drive-count request. It does not establish the actual input tuple or post-call DS/SS/DOSDATA placement. | Guest owner. Do not seed DS:SI, SS, a BIOS communication block, or DOSDATA as an adapter workaround. |
| 5. Device-chain initialization | Original `DosInit` invokes `charinit` for console and subsequent device headers, `msinit.asm:440-524`. It preserves the console header, follows `SDEVNEXT`, records CON/CLOCK pointers, and later requests the DPB list. | A resident original linked `SYSDEV` chain and `DS:SI` at each header. | S33 correlates the retained `50:46` record after `charinit`/device-chain looping, proving passage through `continit`; it does not prove any individual `DS:SI` header value. | Guest/NTIO owner. Reject synthetic CON, static replacement header, or manually advanced chain. |
| 6. Request packet and device call | `charinit`, `msinit.asm:787-804`, writes the `devcall` initialization request in DOSDATA, makes `ES:BX` point to it, and invokes `DEVIOCALL2`. `dev.asm:518-542` requires `DS:SI` device header, reads `SDEVSTRAT` at `+6`, calls strategy, then reads `SDEVINT` at `+8`. `inc/devsym.inc:12-18` defines that layout. | DOSDATA-backed request packet; `DS:SI` real resident header; valid strategy/interrupt far-call fields. | S31 identifies the `SDEVSTRAT` read as an exact static shape candidate for the observed end-of-segment word access. It does **not** prove this stage ran, its `SI`, its instruction boundary, or the strategy/interrupt target. | Keep the candidate unimplemented. No `CALLDEVAD`, request packet, strategy pointer, interrupt pointer, or device return is supplied by bx-vdm. |

## Address and ownership reconciliation

The formal load address is an algebraic consequence of the current service:
`physical = (DI & 0xffff) << 4`. It is not an image-layout assertion made by
the adapter. The retained primary trace/S29 evidence is compatible with
`DI=0C41h` and hence a physical load address of `0C410h`, but source code
requires several guest-owned relocations after that point. A later `DS=0C41h`
does not demonstrate that relocation was skipped, completed, or failed.

| Concern | Owner | S32 disposition |
| --- | --- | --- |
| NTDOS byte identity, 50:11 checked RAM write, BOP continuation | bx-vdm mechanical ingress/service plane | Retain; proven narrow seam. |
| Executing NTIO, `sysinit`, `DosInit`, `charinit`, resident headers and DOSDATA | Original guest NTIO/NTDOS component | Source contract mapped; continuity unproven. |
| Real-mode execution, RAM and ordinary far-call mechanics | bx-core/bx-mantle | No fault is assigned here without a source-backed runtime witness. |
| Host filesystem, profile mutation, CLI capability | opennt host capability / CLI | Not reached by this package; no disposition change. |

## Conclusion and successor boundary

The current code correctly preserves the narrow original `SVC_DEMLOADDOS`
shape: guest-selected destination, ordinary RAM copy, and normal instruction
continuation. It does not bypass original initialization with a host-owned
DOSDATA or device-chain model.

The first native failure is therefore **post-bootstrap but pre-instruction-attribution**. The
only justified conclusion is that a narrow guest-state/instruction provenance question remains. It is not evidence for a BOP-provider repair, a
Direct/Readonly capability change, a real-mode wrap alteration, or a synthetic
NTIO/CON implementation.

S32's successor observation package, S33, is now closed: it reuses the existing
selector-blind BOP sequence and proves reach of `sysinit`, `DosInit`, and
post-`charinit` `continit`. The remaining scope is the narrower S34
source/build attribution of the first non-UD instruction/state; no guest-byte,
lifecycle-state, BOP/provider, CPU, or machine repair is admitted by this map.