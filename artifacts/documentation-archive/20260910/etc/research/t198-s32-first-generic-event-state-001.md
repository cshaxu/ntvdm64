# T198 S32 First Generic Event State 001

## Question

What raw predecessor-state information accompanies the S29/S31 first generic
fault, without reading guest memory or interpreting BOP semantics?

## Method

The test bridge takes an ordinary by-value copy of the already-copied
`bx_ntvdm_generic_ud_event_v1` when it accepts the first non-`C4 C4` event.
The fixture retrieves that value copy, validates its magic/version/size and
the previously observed generic-fault fields, then reports it beside S31's
terminal ordinary-RAM snapshot. Neither production source nor the copied-event
ABI changed.

## Exact MSVC x64 `/MT` Run

The existing compile/link/run recipe exits zero and reports:

```
fault=0000:0a84 mode=1 vector=6 window=15:ffffffff
state=1:0000:0a84 ss-sp=00a7:090a ds-si=bf1f:1482 es-di=00a7:0102
eax=0000035c ebx=00000000 ecx=00000000 edx=008203f2 ebp=00000002
flags=00000013
terminal=1:0000:0ad2 snapshot=1:00a80:16:ffffffff
```

The local prior-drive observation remains true in this run. The bridge's
typed STOP prevents the later LSL/VGA diagnostic path from executing.

## Interpretation

The event has a real guest stack at `00A7:090A`, physical
`0x00A7 * 16 + 0x090A = 0x137A`; it is not a reset stack or the initial NTIO
entry segment. `DS:SI` translates to physical `0xC0672` and `ES:DI` to
`0x0B72`, but their intended use cannot be inferred from raw registers alone.
No source conclusion about a BIOS, VGA, or BOP provider follows.

The raw state gives a bounded next evidence target: copy a small ordinary-RAM
range at the proven stack physical address in another deterministic fixture
run, then treat any candidate return values as data to map against the source.

## Boundary

This task copies only a value already supplied by the selector-blind core
event. It has no retained source pointer, guest-memory access, CPU mutation,
provider selection, device action, or production runtime effect.
