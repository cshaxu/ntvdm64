# T194 S2 bootstrap versus normal-return composition divergence 001

Date: 2026-08-12  
Packet: M0 T194 S2  
Disposition: first composition divergence identified; no source, build, or runtime change.

## Compared paths

The normal-return source chain is:

```text
54:01 deliver SHARE -> 50:36 -> SHARE INT 21h/AH=4Ch -> 50:3C
-> guest teardown/parent restore -> 54:11
```

The current finite BYOB launch component has a different, deliberately bounded
contract.  `bx_ntvdm_cmd_get_next_v1_complete` requires `plan->slot_count ==
2`; its first `54:01` supplies `TARGET`, while the second supplies `QUIT.COM`.
`bx_ntvdm_cmd_get_next_state_v1_commit` advances `delivered` through those two
slots.  `QUIT.COM`'s source-defined `C4 C4 FE` then proves controlled stop.

This is a valid finite startup/controlled-stop composition, but it is not the
original normal-return lifecycle.  It has no mode that delivers exactly one
declared target, preserves that target's post-`50:3C` guest continuation, and
waits for the original `54:11` edge before deciding that no next command is
available.

## Why `54:11` is not the first repair

The existing `bx_ntvdm_cmd_return_exit_code_v1_dispatch` already recognizes
only a narrow, source-shaped `54:11`: after exactly one delivery, it copies
DX, clears CF and resumes.  It does not expose a CLI result and it cannot be
used as proof while the guest has not reached the BOP.  Expanding that handler
would not create the missing predecessor.

Similarly, the `06` island correctly lets the historical illegal-op path
advance and continue; T179 shows the next visible edge is `54:01`, not
`54:11`.  It cannot select a command lifecycle policy.

## Decision for S3

The next coherent work is a whole **single-target normal-return launch mode**
within the existing COMMAND host-service plane.  It must be source-mapped to
`cmdGetNextCmd`/`cmdReturnExitCode`, use the same copied CMDINFO and immutable
profile inputs, and define all three lifecycle states together:

1. one target delivery;
2. pending normal guest return; and
3. source-proven `54:11` no-next continuation.

It must not implement a host command queue, invent a second target, turn
`FE` into normal return, expose ambient process creation, or add a CLI DOS
result channel.  S3 is limited to the original source/ABI/failure map for
that mode before any implementation admission.
