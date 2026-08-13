# T198 S44 CALLDEVAD terminal witness

## Scope

S44 changes only the existing exact fixture's already-admitted one-shot
terminal ordinary-RAM snapshot range. It replaces the former stack-window
selection with four bytes at the source-derived final `SS:[CALLDEVAD]` address
`00A7:037A = 0x0DEA`. No production or machine code changes.

## Result

The existing MSVC x64 `/MT` S40 link-and-run recipe rebuilt the fixture and
exited zero. Its relevant output was:

```text
prefetch: EIP [00010000] > CS.limit [0000ffff]
t198-s23 ... state=1:0000:0047 ss-sp=00a7:090e ds-si=bf1f:1482 ...
t198-s23 ... snapshot=1:00dea:4 call-dev-ad=ffff1fbf history=8
t198-s23 cs-transition valid=1 previous=9346:4d53 ss-sp=00a7:0918 ... current=0000:0000 ...
t198-s23 cs-provenance valid=1 ... code=36ff1e7a038b440836a37a0336ff1e ...
```

The little-endian four-byte slot is the far pointer `BF1F:FFFF`, not
`0000:0000`. The fixture retains its existing disabled/invalid snapshot range
checks and all existing BOP, generic-stop and provenance checks.

## Interpretation limit and correction

This is a terminal snapshot after the controlled generic stop, not a new
pre-call watchpoint. It cannot alone prove that the slot had the same value
at the instruction boundary, nor identify every intervening write. It does,
however, refute the earlier S42 inference that the next retained history
position `0000:0000` demonstrated a null far-call operand.

`DEVIOCALL2` stores the live `DS` as the pointer segment immediately before
the call; the run's retained `DS=BF1F` and terminal pointer segment `BF1F`
are mutually consistent. Its `FFFF` offset coincides with the prefetch log's
first out-of-limit `EIP=0x10000` diagnostic. This establishes a concrete
candidate: the original device strategy begins at the end of a real-mode
segment and requires a source/CPU audit of the ensuing instruction fetch.
It is not proof of a Bochs defect yet, and it authorizes no repair.
