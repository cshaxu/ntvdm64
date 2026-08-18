# M0 T225 S49 — Post-entry real-mode mechanical continuity map 001

## Scope and question

S47 established that the accepted `50:36` route is a source-shaped void
notification and that the guest-owned portion immediately afterwards cannot
be repaired as an adapter BOP endpoint.  S48 then recorded one bounded,
default-off arrival record without reading guest memory or changing execution.

This record reconciles the original NTDOS `$Exec` post-notification transfer
with the adopted generic bx-core real-mode mechanisms and with the current
mantle/adapter boundaries.  It asks one limited question: does source and
current code identify a missing owner in the transition from `50:36` to the
child far return?

## Original transfer contract

`src/opennt/base/mvdm/dos/v86/doskrnl/dos/msproc.asm:1332-1389` makes the
ownership and order explicit:

| Order | Original NTDOS operation | Required state |
| --- | --- | --- |
| 1 | `lds SI, Exec_Init_IP`; `les DI, Exec_Init_SP`; `mov AX, ES` | `DS:SI` is the child entry; `AX:DI` is the child initial stack. |
| 2 | `SVC SVC_DEMENTRYDOSAPP` | The historical DEM notification occurs before guest register/stack handoff. |
| 3 | `DOCLI`; clear `InDos` | Guest interrupt and DOS-state transition. |
| 4 | `mov SS, AX`; `mov SP, DI`; `STI` | Establish the child real-mode stack. |
| 5 | `push DS`; `push SI` | Manufacture the far-return frame for the child entry. |
| 6 | `mov ES, DX`; `mov DS, DX`; `mov AX, BX` | Establish PDB data segments and initial AX. |
| 7 | `retf` | Pop `IP`, then `CS`, and enter the child. |

For the selected source-built MZ SHARE image, the child image mechanics are
also original guest code: it has a 32-paragraph header, initial `CS=2`,
initial `IP=16`, and payload at artifact offset `0x200`.  It is not an
adapter-loaded COM entry.  The observed `DS:SI=0c41:0100` prior to this
transfer remains compatible with the initial COMMAND execution shape and is
not attribution evidence for SHARE.

## Current owner reconciliation

| Contract | Bound owner | Current implementation/evidence | Result |
| --- | --- | --- | --- |
| `50:36` notification and return | `bx-vdm` only at the narrow host seam | `bx_ntvdm_dem_session_lifecycle_provider_v1.c:41-44` supplies a checked `fault_rip + 4` resume and no register, flag, segment, guest-memory or service-payload write. | Matches original void notification boundary. |
| NTDOS image arrival | Original DEM `demLoadDos` capability plus typed mechanical write | `bx_ntvdm_dem_load_dos_service.c:15-42` writes the immutable NTDOS image to `((uint16_t)EDI)<<4`, validates the aperture and resumes only after the checked action. | No host-selected child placement or entry-state fabrication. |
| Sequential 16-bit instruction progress | `bx-core` CPU | `cpu.h:4427-4441` wraps IP for a 16-bit code segment.  This is the already-registered T198 real-mode IP-width repair, not a new S49 intrusion. | Available generic mechanism. |
| Real-mode segment load | `bx-core` CPU | `segment_ctrl_pro.cc:163-193` maps the selector base to `segment << 4`; it preserves the real-mode cache contract (including supported big-real-mode behavior). | `mov SS`, `mov DS`, `mov ES` stay CPU-owned. |
| 16-bit stack frame | `bx-core` CPU | `stack.h:28-47` uses 16-bit wrapped SP and `stack_write_word` when SS is 16-bit; `stack.h:81-103` does the matching pop. | Both `push` operations stay CPU/memory-owned. |
| Far transfer | `bx-core` CPU | `ctrl_xfer16.cc:107-143` performs `pop_16` IP then CS, checks the applicable limit, loads CS and installs 16-bit EIP. | `retf` is a generic CPU transition, not an adapter or mantle service. |
| Machine lifecycle and backing RAM | `bx-mantle`/`bx-core` | The finite stage arms the real-mode start and ordinary RAM mechanics only; it does not identify selectors or construct DOS PSP, arena, stack or return frames. | Correctly separate from BOP semantics. |

## Evidence boundary and disposition

The S48 record establishes only that a `50:36` event was accepted at
`CS:EIP=0041:5a70` with `DS:ESI=0c41:0100`, and that the typed result resumed
at `5a74`.  It does **not** record `AX`, `DI`, `DX`, `BX`, the resulting
stack, the return frame, or a child instruction.  Under S49 admission, adding
an observer to obtain those values would be a new implementation decision;
inspecting guest memory or selecting a trace-driven BOP repair is explicitly
out of scope.

Accordingly, source/current reconciliation identifies no adapter defect, no
BOP/provider defect, no missing mantle device, and no new Bochs intrusion.
It establishes the owner-qualified limitation instead:

> The post-`50:36` execution handoff has a complete source and generic
> mechanical owner map, but this packet has no combined source-shaped native
> proof of `mov SS`/`mov SP`/two pushes/`retf` for the immutable guest state.
> No repair follows from that absence.

If a later packet needs to reduce this limitation, it must first admit a
whole generic real-mode continuity fixture or a selector-blind mechanical
observation with fixed ABI and source-derived acceptance criteria.  It may
not add a BOP-specific workaround, synthesize a child launch, inspect guest
state ad hoc, or move the NTDOS transfer into bx-vdm.

## Verification

This is a source/current-code evidence packet.  Its checks are the formal
Ninja graph membership review, documentation-governance verification and
`git diff --check`; it runs no native trace and changes no runtime code.
