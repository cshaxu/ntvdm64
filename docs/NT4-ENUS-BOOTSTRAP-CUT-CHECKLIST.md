# NT4 EN-US Bootstrap-Cut Checklist

Status: R0.2 design checklist, 2026-08-07. This is the admission gate before
any future research runner attempts the fixed `nt4-en-us-command-smoke-v1`
profile. It separates direct source facts from frozen-probe observations and
from unproven machine policy. It does not authorize guest execution.

## Use

The runner must evaluate every `required` row before it writes guest memory or
starts the repaired SoftPC/CCPU software-monitor path. A row marked `trace-derived` must be populated from a redacted,
version-locked trace before the profile can advance beyond its named point. A
row marked `unproven` is an execution stop, not permission for a guessed
default or self-authored BOP/SVC response.

## Fixed Source Facts

| ID | State or action | Evidence | Status | Admission requirement |
| --- | --- | --- | --- | --- |
| BC-01 | A validated `nt4-en-us-command-smoke-v1` BYOB profile supplies NTIO, NTDOS and COMMAND identity metadata. | `NT4-ENUS-BYOB-PROFILE-CONTRACT.md` | required | Complete R0.1 validation succeeds before guest allocation. |
| BC-02 | NTIO load origin is `0070:0000`, physical `00070h`. | `base/mvdm/inc/vint.h`: `NTIO_LOAD_SEGMENT=0x70`, `NTIO_LOAD_OFFSET=0` | required | Preflight `0x700 + ntio.bytes` as one writable ordinary-RAM range; reject any overlap with an immutable/provider range. |
| BC-03 | The normal first CPU entry is the NTIO image origin, not the source map's later `init` label. | `host/src/nt_msscs.c` sets CS:IP to the load constants; staged image evidence documents the initial jump. | required | Set only the source-defined CS:IP after reset and image commit. Do not skip the first guest instruction. |
| BC-04 | The four-byte virtual-interrupt state is at physical `000714h` (`0070:0014`). | `vint.h`: `FIXED_NTVDMSTATE_LINEAR`, `FIXED_NTVDMSTATE_SIZE=4` | required | Reserve, initialize, preserve across NTIO copy, and restore it using an original owner or an explicitly source-equivalent startup adapter. |
| BC-05 | Historical setup performs `io_init()` and original `reset()` before image entry. | `host/src/nt_msscs.c` | required | The runner trace names the original providers and their completed ordering. An outer CLI may not substitute device semantics. |
| BC-06 | The original bootstrap reaches the original `SVC_DEMLOADDOS` transition to request NTDOS loading. | `dossvc.h`, `demdisp.c`, `demmisc.c` | required | Preserve the historical service ownership/delegation `BIOS[] -> MS_bop_0 -> DemDispatch -> demLoadDos` through the repaired monitor; no self-authored selector/service switch is permitted. |
| BC-07 | Original `demLoadDos` uses guest `DI:0000` as the NTDOS load address. | `base/mvdm/dos/dem/demmisc.c` | required | Preflight the complete NTDOS image against checked writable RAM before the original loader reads/copies it; no partial commit on failure. |

## Profile Inputs That Must Be Explicit

| ID | Input | Source fact | Current status | Required decision/evidence |
| --- | --- | --- | --- | --- |
| BC-10 | Boot medium registers `DL`, `CH`, `BX`, `AX`, `DI` at NTIO's `init`. | `msinit.asm` treats them as boot-loader inputs. | unproven | A profile declares their origin and values, or a trace proves that the selected path does not consume a field before setting it. |
| BC-11 | Initial real-mode register state other than source-set CS:IP. | NTIO documents most values as unknown and sets its own stack/data state early. | unproven | A reset-state contract, not arbitrary wrapper register writes. |
| BC-12 | IVT contents and INT 11h/12h/13h/15h/19h behavior. | NTIO saves/replaces vectors and later reaches interrupt paths. | trace-derived | Derive the first live vector from the original chain; then admit the original provider or a named lower host seam. |
| BC-13 | BDA equipment/memory/keyboard state. | NTIO reads BDA locations including `0040:0096`. | trace-derived | Provide a source-backed profile value and ownership; no zero-filled BDA claim. |
| BC-14 | Conventional RAM size, ROM mappings and device-memory topology. | BIOS/NTIO expect machine memory services. | unproven | A checked mapping plan names ordinary RAM, immutable images and provider ranges before reset. |
| BC-15 | A20/XMS, timer/PIC, keyboard/text display and DPB/media configuration. | Their historical owners exist, but first-cut live order is not yet proven. | unproven | Introduce only when an original trace reaches its owner. |

## Forbidden Evidence Shortcuts

The following observations may guide a future trace but cannot satisfy a
checklist row:

- a frozen adapter's individual BIOS BOP or DEM SVC handler returning success;
- a zero-filled IVT/BDA baseline or a scaffold IVT/BDA profile;
- direct execution at an internal NTIO symbol instead of `0070:0000`;
- system-directory discovery of `ntio.sys` or host-root creation of
  placeholder `IO.SYS`/`MSDOS.SYS`;
- a file-name-only runtime directory check; or
- link success of an owner module that has not executed on the live path.

## Transactional Startup Sequence

The future runner's initial sequence is constrained to the following order:

1. Validate the BYOB manifest and component identities (BC-01).
2. Build and validate a complete machine mapping plan (BC-02, BC-04,
   BC-14); do not commit it yet.
3. Create the repaired SoftPC/CCPU monitor configuration and validate its
   reset lifecycle against the source-derived `io_init()`/`reset()` ordering
   (BC-05). The original kernel V86 route remains excluded, while the
   historical software-monitor path is the executable dependency. This stage
   may fail closed on unadmitted callbacks.
4. Commit the topology and reset the machine once.
5. Initialize only the source-backed virtual-interrupt state, apply the
   selected boot-register profile, then atomically copy NTIO to `0070:0000`.
6. Restore the virtual-interrupt word and set the source-defined CS:IP.
7. Run the repaired historical software monitor through frozen profile registrations. On
   an unproven BC-10 through BC-15 dependency, stop with a redacted trace; do
   not manufacture a response.

No later failure may leave a partially copied image, a modified ROM/provider
range, an active host thread, or an unredacted guest-memory record.

## Gate Results

| Result | Meaning |
| --- | --- |
| `bootstrap_cut_not_ready` | One or more required rows are not established. |
| `bootstrap_mapping_rejected` | A required image/range is invalid, overlaps or is not ordinary writable RAM. |
| `bootstrap_input_unproven` | A boot register, IVT/BDA, device or media input has no source-backed profile. |
| `bootstrap_original_owner_missing` | The next live operation lacks its original historical provider/approved lower shim. |
| `bootstrap_trace_stop` | A bounded original-chain trace reached a new dependency; no behavior was synthesized. |

An accepted checklist authorizes only a bounded bootstrap trace. It does not
authorize NTDOS loading, COMMAND execution, or a compatibility claim until
the later R1-R4 gates are independently met.

## Relation To Existing Evidence

`OPENNT-NTIO-ENTRY-STATE-CARD.md` remains the detailed source/probe history.
The self-authored NTIO-to-NTDOS ladder and scaffold-IVT experiments recorded
there are negative evidence: they show that an empty low-memory model and
individual service responses are insufficient. This checklist intentionally
does not inherit their handler semantics into the future runtime.
