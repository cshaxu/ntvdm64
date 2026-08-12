# T140 S1 NTDOS late-fault image/source correlation 001

## Address calculation

The T130 fault frame is `CS:IP=0BC1:FFFF`. In real mode its physical address
is `0x0BC10 + 0xFFFF = 0x1BC0F` (the attempted next fetch is `0x1BC10`).

The source-built NTDOS artifact is SHA-256
`957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84`,
27,858 bytes. The admitted `50:11` load contract computes its destination
solely from `DI:0000`; T130 proves that transaction wrote NTDOS at physical
`0x8B00`. Its complete static interval is therefore
`[0x8B00, 0xF7D2)`.

## Result

`0x1BC0F` is outside the NTDOS image interval by `0xC43D` bytes. It has no
file offset in the source-built NTDOS artifact and cannot be assigned an NTDOS
module, symbol, or instruction purpose from that artifact. The fault is not a
fetch of a static NTDOS byte; it is a runtime control-state transfer into
ordinary guest RAM outside the loaded NTDOS image.

The immediately preceding `50:36` identity is source-defined as
`SVC_DEMENTRYDOSAPP` in `base/mvdm/inc/dossvc.h`, not a DOS INT 21h function.
That identifies a host-service lifecycle boundary, but does not by itself
prove why `0BC1:FFFF` later occurs.

## Disposition

Static image/source correlation rules out a NTDOS instruction-byte patch or a
source-level NTDOS control-flow diagnosis at this address. The next useful
read-only question is the original `SVC_DEMENTRYDOSAPP` dispatcher contract:
whether it transfers/suspends host control rather than returning as the
current generic resume path does. No implementation is admitted by T140.
