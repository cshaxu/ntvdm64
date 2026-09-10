# T200 S18 — initial-state declaration ABI

## Result

S18 adds `bx_ntvdm_initial_state_v1`, a bx-vdm-owned copied data ABI for
profile initial state.  It deliberately separates a structurally valid
`ABSENT` declaration from an `admitted` `PRESENT` declaration.  Thus a profile
without evidence does not silently acquire historical bytes.

## Contract

`PRESENT` contains at most four ordinary-RAM writes, a bounded copied payload,
and a 32-byte opaque evidence identity.  Admission requires a nonzero evidence
identity, preflight-valid ranges, no overlap, and exact payload coverage.
`ABSENT` has no ranges, payload, or evidence identity and is valid only as an
explicit refusal to publish initial state.

This vocabulary contains no firmware, IVT, BDA, NTIO, BOP, DOS, OpenNT, or
Bochs type.  A future bx-mantle consumer may accept only the already-admitted,
opaque write records and perform generic RAM mechanics.

## Verification

`Invoke-T200S18InitialStateAbiProbe.ps1` passed at
`artifacts/build/t200-s18-initial-state-abi-r2` using x64 `/MT`.  It proves:

- cleared `ABSENT` is valid but rejected for publication;
- a finite present declaration with evidence identity and non-overlapping
  writes is admitted; and
- overlapping ranges and absent evidence identity are rejected.

The probe initializes no machine and executes no guest code.

## Follow-up

S19 must determine how a profile binds a pinned evidence identity and copied
initial-state bytes to a compatible minimal-machine configuration.  It may not
apply ranges to RAM until that source/profile binding is accepted.
