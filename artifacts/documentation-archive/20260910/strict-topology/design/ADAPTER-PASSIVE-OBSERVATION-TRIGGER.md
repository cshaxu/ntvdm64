# Adapter Passive Observation Trigger v0

## Decision

The first observation execution path shall use a profile-declared neutral
`#UD` fixture trigger. It is not a BOP, a DEM service, a BIOS operation, a DOS
filesystem request, or a guest-startup mechanism.

The fixture instruction is `0F 0B` (`UD2`), followed by the native `HLT`
already used by the bounded reset fixture. `UD2` is chosen only because the
existing generic #UD seam has already proved an exact copied instruction
window and its two-byte resume length; the adapter must not infer that length
from an arbitrary exception.

This v0 trigger is limited to a custom reset-fixture ROM. It cannot be used to
claim observation of stock BIOS, NTIO, NTDOS, IVT, BDA, or any OpenNT state.

## Required Profile Record

The next machine-profile ABI revision needs one immutable trigger declaration
paired with one existing observation ID:

| Field | Meaning | Constraint |
| --- | --- | --- |
| `observation_id` | Existing opaque observation declaration | Must exist exactly once. |
| `exception_vector` | Trigger exception class | v0 must be `6` only. |
| `instruction_bytes` | Raw copied-window prefix | v0 is exactly `0F 0B`. |
| `resume_bytes` | Bytes skipped after a successful read | v0 is exactly `2`. |

The record has no segment, linear/physical address, BOP selector, service ID,
host path, Bochs pointer, firmware symbolic name, or guest semantic label.
The range continues to originate only from the existing profile-owned
observation declaration.

## End-to-End Contract

```text
fixture ROM: UD2 ; HLT
        |
generic Bochs #UD seam copies event/state/window
        |
adapter validates exact profile trigger
        |
adapter prepares one profile-owned observation transaction
        |
Bochs takes it at the same copied boundary
        |
generic copy_from_ordinary_ram fills adapter-owned output
        |
adapter returns profile-declared resume RIP (+2)
        |
native HLT / benchmark remains the stop mechanism
```

The adapter must leave the result pass-through if any check fails. The Bochs
side must leave the original exception path untouched unless it receives one
preflighted transaction and subsequently succeeds in the all-or-nothing
ordinary-RAM copy. A failed copy also falls through; it must neither partially
fill output nor resume the guest.

## Ownership

| Layer | Owns | Explicitly does not own |
| --- | --- | --- |
| Bochs | #UD delivery, copied instruction window, ordinary-RAM read, native CPU resume | trigger bytes, profile IDs, output interpretation, BOP/DEM/DOS policy |
| Adapter | profile validation, byte-prefix comparison, pending operation, output lifetime, resume decision | Bochs memory mapping, devices, BIOS/DOS filesystem, guest code execution |
| Fixture profile | opaque range plus trigger declaration | a host service or guest semantic name |
| Fixture ROM | neutral `UD2;HLT` executable input | a permanent firmware or guest runtime dependency |

## Rejected Alternatives

- **BOP `C4 C4 xx yy`:** it would test a host-service protocol rather than a
  passive observation mechanism.
- **Raw CLI/environment address and stop point:** it loses profile ownership
  and turns the adapter into an arbitrary memory reader.
- **Bochs chooses `UD2`, `+2`, or a range:** that makes Bochs an adapter/guest
  policy owner.
- **Adapter rereads Bochs memory:** it leaks Bochs memory semantics across the
  hard boundary.
- **A generic `STOP` result now:** the existing seam has no verified stop
  disposition. Native `HLT` and benchmark already give the fixture a bounded
  original stop path, so v0 needs no new stop control.

## Admission Sequence

1. **Completed:** the trigger record is accepted only from profile-owned BYOB
   metadata, references an admitted observation ID, and rejects any byte
   prefix other than `0F 0B`; no Bochs source changed.
1. **Completed:** adapter matching prepares one transaction and queues it into
   a fixed 4096-byte adapter-owned output slot. Parser, ABI and runtime tests
   prove accepted selection and a non-matching first byte rejection.
1. **Completed:** `BX-MEM-011` takes only a preflighted pending observation,
   calls `copy_from_ordinary_ram`, and resumes only after successful copy.
1. **Completed:** r15 built the dedicated same-version fixture target and ran
   the `UD2;HLT` fixture through copied observation, resume, and native stop.
1. **Completed:** the resulting evidence is recorded as mechanics only; it is
   not generalized into guest startup or a host service.

Any deviation from these constraints requires a new architecture decision and
an entry in the Bochs/OpenNT intrusion register.

## Current Evidence

`byob-profile-test`, `bx-ntvdm-machine-profile-abi-test`, and
`bx-ntvdm-adapter-runtime-test` pass in
`artifacts/build/current/adapter-cli-r1`. r14 also explicitly compiles the
the corresponding profile and composition objects inside its private build copy. None of
these results reads guest memory or executes the fixture through Bochs.

The subsequent same-version execution evidence is
`etc/research/adapter-r15-passive-observation-fixture.md`.
