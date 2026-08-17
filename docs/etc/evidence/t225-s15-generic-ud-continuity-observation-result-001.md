# T225 S15 Bounded Generic-UD Continuity Observation Result

## Question

Can the adapter record the first existing generic `#UD` location without
interpreting a BOP or changing the bridge result, and does the formal primary
run produce such a record?

## Implementation

`bx_ntvdm_generic_ud_sequence_observation_v1` is a default-off, fixed
128-record adapter facility.  A record copies only fields already supplied by
the generic bridge:

- CS and EIP;
- execution mode;
- the existing bounded 15-byte instruction window;
- the existing generic outcome disposition.

It contains no selector/service classification, registers, guest-memory read,
provider identity, host handle, callback, retained pointer or CPU mutation.
The generic bridge calls it after selecting its pre-existing accepted result,
or with a local PASS_THROUGH record when both composition handles decline.
The caller-supplied decline outcome remains unchanged.

The CLI exposes it only through `--observe-ud-sequence`; absence of the flag
keeps it inactive.  The normal worker return copies and prints the record,
then clears the facility.  `--validate-only` shows `observe-ud-sequence=0`
without the flag and `=1` with it; a duplicate flag is rejected with exit 2.

## Focused verification

A fresh formal Ninja/MSVC x64 `/MT` graph at
`build/M0-T225-S15/ninja-r1` built 308 edges.  The new focused fixture passes
and proves two ordinary generic `0F 00 00` events:

1. a declined event is recorded with PASS_THROUGH while the caller's
   disposition remains its sentinel value; and
2. an accepted RESUME event is recorded with the existing accepted outcome.

It also verifies CS=`0070`, EIP=`00001234`, real execution mode, exactly the
three copied bytes and disabled-copy refusal.  The full graph subsequently
reports no work to do.

## Primary runs and limiting result

With the immutable S10 primary stage and both explicit sequence flags, the
following normal bounded runs completed:

| Mode | Budget | Exit / terminal | BOP records | Generic-UD records |
| --- | ---: | --- | ---: | ---: |
| Direct | 10,000 | 4 / budget | 0 | 0 |
| Readonly | 10,000 | 4 / budget | 0 | 0 |
| Direct | 100,000 | 4 / budget | 0 | 0 |
| Readonly | 100,000 | 4 / budget | 0 | 0 |

The 100,000-tick raw logs nevertheless contain real-mode `SLDT` diagnostics.
That observation exposed a build-closure omission, not an absence of `#UD`:
`src/bx-core/cpu/exception.cc` compiles its existing core-to-mantle bridge
only when `BX_NTVDM_ENABLE_MANTLE_UD_BRIDGE=1`, while the formal CPU5
projection's additions list is empty.  The bridge is consequently compiled
out in this graph.  Earlier targeted probes supplied the macro directly;
the full Ninja manifest did not carry it.

## Boundary and follow-up

This result does not authorize a BOP, CCPU, CPU, device, BIOS, PIC, IRQ or
memory repair.  S16 owns the smallest build-configuration closure: project
the existing registered bridge gate for the formal target, preserving its
selector-blind copied-event ABI, then rerun this same bounded observation.
No Bochs source patch is required or admitted.