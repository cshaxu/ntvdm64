# M0 T351 S3 — original bootstrap local-contract closure

## Contract frozen for observation

The single selected source-shaped path is:

```text
COMMAND guest 54:05
  -> SoftPC nt_bop.c::MS_bop_4
  -> COMMAND cmddisp.c::CmdDispatch(05)
  -> cmdmisc.c::cmdSetInfo
  -> numeric SCS scalar registration / short session leases
  -> original CPU40/SAS executes the resumed guest instruction stream
```

`CmdDispatch(05)` and the original `cmdSetInfo` control flow remain in the
MVDM mirror.  The only selected compatibility boundary is the already
registered scalar-location facade: a valid, bound session may acquire a
short-lived lease; stale, absent, wrong-session, and out-of-range access is
rejected.  There is no second COMMAND dispatcher, guest result writer,
alternate CPU, raw pointer alias, or global WOW alias.

## Focused verification

The current x86 scalar lifecycle fixture was rebuilt from these exact current
sources and passed:

```text
tests/adapter-mvdm-host-out/softpc/command-scalar-state-fixture.c
src/adapter-mvdm-host-out/softpc/mvdm_command_guest_state.c
src/adapter-mvdm-host-out/softpc/mvdm_guest_location.c
src/session/{session,guest_memory_lease,mapping_manager}.c
```

It proves the active-session positive path, the original one-byte and little
endian `WORD` operations, modular FD-access wrap, wrong-session refusal,
lease-end refusal, restarted-epoch refusal, and range refusal.  The compiler
completed with the normal `/W4` visibility and no warning suppression.

S2's independent fresh 425-action CPU40/x86 formal product link remains the
formal product identity for this unchanged source cohort.  No source changed
between that link and this local-contract review.

## Retired fixture disposition

`tests/adapter-mvdm-host-out/softpc/t282_s10_command_registers_fixture.c`
is a stale historical test: it includes `mvdm_command_registers.h` and links
`mvdm_command_registers.c`, neither of which remains in the live component
after the canonical command-state consolidation.  Its attempted x86 build
therefore correctly fails at the missing historical header.  Recreating that
retired register facade would introduce a parallel ABI and would not validate
the selected original `cmdSetInfo` state contract.  It is not used as evidence
and is transferred to the next test-source hygiene review; this S records the
failure rather than masking it.

## S4 input freeze

S4 may stage only the already formally linked CPU40/x86 product with the
existing immutable DOS and firmware media, then run one non-debug,
console-owning eight-second observer using exactly `-f -o --ordinary-child`.
It may classify the observed result only.  A trace result cannot authorize a
new BOP or machine repair inside T351.
