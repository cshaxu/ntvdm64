# T102 S1 aperture-entry observation design audit 001

## Question

Does the `0xAF6AF` aperture-entry gap require another Bochs diagnostic
intrusion, or is the first transfer already proven by retained same-family
evidence?

## Observations

The existing registered diagnostic sequence `BX-TRACE-048` through
`BX-TRACE-051` already follows the post-close ordinary control path without
BOP semantics. In particular, the retained r37 observation records the
original OpenNT pseudo-ROM far jump `073B:0939 -> 0000:0000`.
`spckbd.asm` copies the prior IVT INT10 pointer into `host_int10` before that
jump, so the saved machine INT10 vector was zero. The preceding firmware/IVT
audit establishes why: mapping the Bochs ROM does not execute POST; the direct
execution plan transfers to NTIO before `F000:FFF0` ROM POST, IVT
initialization and VGA ROM initialization.

The current T100 record independently has the same direct-entry condition:

- its generated makefile compiles `main.cc` with
  `BX_NTVDM_ENABLE_EXECUTION_PLAN=1`;
- its log records `requested execution plan applied`; and
- `main.cc` invokes that helper after `bx_init_hardware()` and before the
  first CPU loop.

The current r5-derived closure does not compile either `main.cc` or
`exception.cc` with `BX_NTVDM_ENABLE_DEFERRED_STARTUP_PLAN=1`, despite the
source already containing the deferred plan arm/take consumer.

## Decision

No new transfer observation and no new Bochs intrusion are needed. The
required observation outcome already exists and identifies an upstream
firmware lifecycle omission, not an unknown ordinary CPU transition.

The only aligned successor is an object/configuration closure for the existing
native POST + option-ROM neutral-marker + opaque deferred-plan handoff. It
must make direct pre-loop execution-plan entry and deferred handoff mutually
exclusive as binary consumer modes.  A deferred profile legitimately carries
both the opaque startup-plan data and its handoff descriptor: the latter only
selects when the former may be consumed.  It cannot synthesize IVT state, decode BOP/DOS
terms, add a device, or make a video aperture executable.

## Follow-up

T103 S1 is a source/build-recipe audit only: inventory the exact existing
`main.cc`, `exception.cc`, adapter runtime and option-ROM composition inputs;
prove the macro/object closure and negative mutual-exclusion checks before any
compile or post-POST guest run.
