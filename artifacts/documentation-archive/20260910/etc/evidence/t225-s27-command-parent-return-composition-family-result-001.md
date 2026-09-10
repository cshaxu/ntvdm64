# T225 S27 P3 — COMMAND Parent-Return Composition Family Result

## Question

Does the bound COMMAND composition preserve the original parent-return split in
both enabled mutation modes without collapsing `54:0B` into `54:00`?

## Inputs and procedure

A fresh MSVC x64 `/MT`, CPU5/P-MMX graph was generated at
`build/M0-T225-S27/command-parent-return-r4`. The formal
`t225-s27-command-parent-return-composition-fixture` links the complete owned
static closure: `bx-core`, `bx-mantle`, `bx-vdm`, `opennt-host`, and `cli`.
It then ran the bound composition fixture in Direct and Readonly modes.

## Observations

The r4 fixture exited `0` and printed:

```text
S27 COMMAND parent return composition direct/readonly family verified
```

In each mode it proves:

| Case | Observed bound-composition result |
| --- | --- |
| `54:0B`, declared plan exhausted | resumes at `RIP+4`, CF clear, AX low byte equals DX low byte, and exactly one opaque ordinary-completion fact is published |
| `54:0B`, another declared command remains | resumes with CF set and publishes no ordinary-completion fact |
| repeated return | declined after the first accepted return |
| `54:00` | produces the separate `TOP_LEVEL_EXIT` record with no DOS exit code; it is not a parent-return record |

The fixture uses the same completed BYOB profile-admission contract as the
COMMAND family fixture. It performs no host filesystem access.

## Interpretation

The ordinary-completion consumer is now exercised through the actual bound
composition route, while original COMMAND origin remains distinguishable:
`54:0B` has a source-derived exhausted-plan fact and `54:00` has the existing
top-level terminal fact. Direct/Readonly agree because this return lifecycle
is not a host mutation operation. Overlay remains ABI-deferred.

## Limitations and follow-up

This is composition-family evidence, not a source-built guest COM
parent/child/native execution proof. The remaining S27 gate is one bounded
native observation that validates post-stop engine cleanup consumes the opaque
fact correctly. No trace-selected BOP work is admitted.