# T198 S38 code-segment transition witness 001

## Question

Which retained control-flow boundary most recently changes the code-segment
selector before the already observed first non-BOP `#UD` at `0000:0A84`?

## Mechanism and boundary

`BX-MANTLE-072` extends the already registered, default-off S36 position
history only.  When history is enabled, mantle compares the `CS` fields of
two consecutive retained fixed-width records.  On a difference it replaces a
single mantle-private copied pair.  The query returns that pair by value; it
does not expose storage or a CPU object.

The existing finite-fixture bridge copies this value only when it already
observes the first generic `#UD`.  No `bx-core`, `bx-vdm`, OpenNT or guest
image source changed.  The mechanism has no selector/service vocabulary,
instruction bytes, guest-memory access, CPU mutation, device action or
product lifecycle behavior.

## Exact CPU5 witness

The existing CPU5/Pentium-MMX MSVC x64 `/MT` finite closure was rebuilt for
the changed mantle, bridge and fixture objects, relinked against S36's exact
CPU5 core closure, then ran successfully (exit `0`).  Its relevant output is:

```text
history-tail count=8 predecessor=0000:0a82 seq=aad7
fault=0000:0a84 seq=aad8
cs-transition valid=1 previous=9346:4d53 seq=a74b
current=0000:0000 seq=a74d
```

Thus the latest retained CS change before the generic stop is from
`9346:4D53` to `0000:0000`; the machine then executes in segment zero through
the independently retained `0000:0A82 -> 0000:0A84` sequence.  The pair is
ordered, copied and selector-blind.  Its instruction-count values differ by
two rather than one, so it is deliberately described as consecutive
**retained** records, not as a proof that the physical transfer instruction
was captured by S36's macro.

## Attribution

The source-built NTIO aperture established by S30/S31 is `0x0700..0x8AFF`.
The observed pre-transition real-mode physical position computes to
`0x93460 + 0x4D53 = 0x981B3`, outside that aperture.  S37 already establishes
that OpenNT's `msinit` legitimately performs low-memory/stack/IVT work after
the drive service; it does not map a linked source instruction to
`9346:4D53`, nor does this witness read or identify bytes at that address.

Therefore the previous hypothesis that a low-RAM writer directly explains
the frontier is narrowed but not resolved: an earlier, unowned transfer has
entered `CS=0000` before the low-RAM sequence.  This is not evidence to add a
Bochs device, firmware map, BOP provider or adapter action.

## Disposition

The exact control-transfer instruction and its owner remain **unproven**.
The next admissible task is source/linked-input provenance of the unexpected
`9346:4D53` position and the paths that can transfer to `0000:0000`, without
guest-memory mutation or a new machine capability.
