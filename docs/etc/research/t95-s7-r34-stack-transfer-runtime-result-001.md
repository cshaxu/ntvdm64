# T95 S7 r34 Stack-Transfer Runtime Result 001

## Question

Can a default-off, generic Bochs observation of original real-mode `MOV SS`
and `MOV SP,r/m16` instructions distinguish the post-close stack transition
without changing CPU or OpenNT semantics?

## Procedure

Fresh r34 links the copied `data_xfer16.cc` as a uniquely named object before
the original CPU archive, with only
`BX_NTVDM_ENABLE_STACK_TRANSFER_DIAGNOSTIC=1`.  The source closure, contained
profile, guest inputs and 15-second watchdog are otherwise r30-identical.
The binary SHA-256 is
`81271FCD60D27272B5E5E77424B3E108BB19926B1C5C66BAE90BBB32EA1E173F`.

## Observation

The trace contains 39 BOP observations and 10 commits, matching r30.  It now
contains real-mode transfer records, proving the uniquely named object links.
Near the retained terminal state, the last records are repeated internal
stack changes at `CS=0032:RIP=26B5/26B9`, then:

```
mov-ss cs=0032 rip=260a old=95ab new=00a7
```

There is no later `MOV SP,r/m16` record before the unchanged
`SS=00A7`, `SP=0003` `stackPrefetch(0xFFFF)` exception escalation.  The
diagnostic does not observe other stack-consuming/control-transfer
instructions, so it cannot identify the instruction that establishes or
consumes the final `SP=0003` state.

## Interpretation

The diagnostic is mechanically valid and narrows the remaining state boundary:
the final transition into SS `00A7` happens after the correctly resumed file
close, but an ordinary non-`MOV SP,r/m16` path follows.  This excludes neither
the original `DOIRET` nor another source-owned stack/control instruction.  It
does exclude attributing the terminal SP directly to the corrected read/close
BOP results.  No new BOP, adapter behavior, device, host capability, or CPU
semantic follows from this evidence.

## Follow-up

First map runtime `0032:260A` and the immediately following original NTDOS
instructions precisely.  Any further diagnostic must be justified by that
source path; do not expand this generic MOV observer into a CPU-loop trace.
