# T198 S33 First Generic Stack Snapshot 001

## Question

What raw bytes surround the deterministic first-generic-fault stack pointer,
and can they safely identify a predecessor without imposing a guest or BOP
interpretation?

## Inputs And Procedure

S32 proves `SS:SP = 00A7:090A`, so the real-mode physical stack position is
`0x137A`. The existing S31 one-shot mantle capture is configured for the
opaque 32-byte ordinary-RAM range `0x1370..0x138F`; this encloses the stack
position at capture offset 10. No production source changes; the exact CPU5
fixture remains responsible only for printing raw bytes.

## Exact MSVC x64 `/MT` Result

The existing build/link/run recipe exits zero, retains S32's identical copied
event, and prints:

```
fault=0000:0a84 mode=1 vector=6 window=15:ffffffff
state=1:0000:0a84 ss-sp=00a7:090a ds-si=bf1f:1482 es-di=00a7:0102
snapshot=1:01370:32 stack=a7000e09ffff1fbf4600584d4693f676
```

`stack=` begins exactly at physical `0x137A`. In little-endian words the
reported sixteen bytes are:

```
00A7 090E FFFF BF1F 0046 4D58 9346 76F6
```

## Interpretation And Limitation

The result confirms a concrete nonempty guest stack at the accepted generic
fault, which rules out the simplistic claim that the stack itself was merely
uninitialized all-FF RAM. It does **not** prove which word is a return IP,
return CS, saved segment, or other frame field: the instruction and call
convention immediately preceding the fault are still absent. Treating any
pair as a return address now would be an unsupported guest interpretation.

`exception.cc` supplies both `fault_rip` and copied `eip` from the same
`prev_rip`; neither is a predecessor instruction address. The next evidence
must therefore come from a bounded instruction-history mechanism, if an
existing Bochs CPU/instrumentation capability can provide it without adding
OpenNT/BOP knowledge to `bx-core`.

## Follow-Up

Perform a read-only audit of the adopted CPU/instrumentation sources for an
existing default-off instruction trace or execute callback that can record a
small, fixed-width native history in mantle-owned storage. Only after deciding
whether such a selector-blind facility exists may a trace implementation be
admitted.
