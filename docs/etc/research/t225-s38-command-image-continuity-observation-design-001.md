# T225 S38 — COMMAND Guest-Image Continuity Observation Design

## Decision

The smallest useful continuity fact is already exposed by original NTDOS at
the generic `C4 C4` boundary immediately before application transfer.  Do not
add a Bochs code breakpoint, CPU instruction trace, guest-memory read, or a
selector-specific `50:36` handler.  Extend the existing default-off CLI BOP
sequence record so **every** observed BOP carries the fixed-width pre-event
CPU snapshot already available at the generic bridge.

Offline source interpretation may then identify a reached `50:36` record as
the original `$Exec` handoff and correlate its copied `DS:SI`, `AX:DI`, `DX`,
and `BX` with later BOP/fault CS.  The recorder itself remains selector-blind:
it neither recognizes service 36 nor changes its outcome.

## Source contract

`src/opennt/base/mvdm/dos/v86/doskrnl/dos/msproc.asm:1366-1375` documents the
register contract immediately before `SVC SVC_DEMENTRYDOSAPP`:

| Copied registers at the generic BOP boundary | Original meaning |
| --- | --- |
| `DS:SI` | child entry CS:IP for the later far return |
| `AX:DI` | child initial SS:SP |
| `DX` | child PDB/PSP segment |
| `BX` | child initial AX |

The succeeding original instructions install stack/register state and transfer
to the child.  `50:36` itself is only the DEM entry notification; it does not
choose an image, copy bytes, allocate a PSP, or launch a host process.

## Existing implementation audit

`bx_ntvdm_bop_sequence_observation_v1` is default-off and already records all
reached `C4 C4` instructions, accepted or declined, in call order.  Its v1
record only has CS:EIP, selector, service and outcome; it intentionally omits
registers.  The generic bridge event, however, already supplies copied GPRs
and segments.  Independently,
`src/bx-vdm/bx_ntvdm_exception_observer_v1.c:7-34` demonstrates an
adapter-owned observational formatting path over that same copied state,
including AX/BX/CX/DX/SI/DI/CS/DS/SS/SP, without guest reads/writes or CPU
result mutation.

Therefore no new bx-core or bx-mantle hook is needed.  The appropriate owner
is `bx-vdm`'s existing BOP-sequence observation ABI and CLI presentation.

## Proposed v2 record ABI

The implementation packet may introduce a versioned v2 record (or a distinct
v2 sequence type) with the existing v1 identity fields plus:

```text
eax, ebx, ecx, edx, esi, edi, eflags  (uint32)
cs, ds, es, ss                         (uint16)
```

Rules:

1. capture before dispatch, from the generic copied event only;
2. record every syntactically valid `C4 C4` observation—no selector/service
   filter, no `$Exec`, COMMAND, DOS, SoftPC or BOP-family interpretation;
3. preserve current 128-record limit, overflow behavior, default-off state,
   and no-result/no-memory/no-provider side effects;
4. retain v1 consumers or explicitly version their textual presentation;
5. print the added fields only when the existing `--observe-bop-sequence`
   option is selected; normal CLI output is unchanged; and
6. reset on enable/disable and never retain guest/host pointers.

## Required proof

The implementation must include:

- a focused fixture with two generic BOP events using distinct register
  snapshots, proving exact copy/order, disabled non-capture, overflow, and
  no outcome mutation;
- a boundary scan proving no source in `bx-core` or `bx-mantle` gains OpenNT,
  DOS, COMMAND, selector/service or observation semantic knowledge;
- a CLI formatting fixture proving the option exposes the v2 fields and
  unselected output retains the prior contract; and
- only after those pass, one paired Direct/Readonly native observation using
  the already-existing option.  The result is evidence, not an authority to
  implement the next observed BOP.

## Rejected alternatives

| Alternative | Rejection reason |
| --- | --- |
| Add a Bochs execution breakpoint at `$Exec` or `0C41`. | Runtime segment/source offset is unproven; it would add source semantics to bx-core. |
| Add a `50:36` special observer. | It is trace-led and falsely makes a DEM notification an image-loader owner. |
| Read PSP/image bytes at the BOP. | Not needed for the original transfer-register question; expands the evidence boundary. |
| Modify `54h` providers or the guest loader. | Observation cannot justify a behavior repair. |

## Admission

S39 may implement only this versioned, default-off, selector-blind snapshot
extension in `bx-vdm`/CLI and its focused tests.  No Bochs intrusion,
provider, guest, profile, or host-capability change is admitted.
