# T95 S6 r6 Real-NTIO Fault-State Observation 006

## Result

The one bounded r6 observation reached exactly one narrow generic transaction:

`guest transaction committed cs=0070 eip=00000475 address=8b00 bytes=6cd2 resume=479`

The source-built NTDOS payload count (`0x6cd2`) and ordinary-RAM destination
(`0x8b00`) remain identical to the prior transaction evidence. No broad
interceptor marker occurred. The fault EIP `0x475` and resume `0x479` differ
by the expected four-byte generic boundary width.

The later native log is distinct in time: the existing x87 diagnostic reports
`cs=0000 rip=0x293`, then the original `math_abort`/IRQ13 path, then repeated
prefetch reports of `EIP [00010000] > CS.limit [0000ffff]` and a final
unmapped direct-read panic at `0xa1800`. The 30-second direct watchdog ended
only that observed process; no residual r6 process remained.

## Classification

This rejects the hypothesis that the admitted transaction was entered already
at the later invalid `0000:10000` code state. It does not prove the full
post-resume path: the remaining candidate owners are the intervening original
execution flow and the pre-existing x87/IRQ13 transition, followed separately
by the minimum-machine mapping rejection. No device or adapter capability is
therefore enabled by this observation.
