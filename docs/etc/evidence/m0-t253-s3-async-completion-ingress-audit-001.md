# M0 T253 S3 — async completion ingress audit

`namepipe.asm:426..494` retains the original packed 20-byte base descriptor
and 24-byte Type2 extension. At the BOP boundary `DS:SI` names the descriptor,
while `MapNtHandle` has already converted the DOS JFN/SFT value to the opaque
32-bit token in `BP:BX`. The previously closed SFT seam therefore supplies the
correct token; no new guest handle format is required.

The imported `demfile.c:251..302` already retains the original
`FILE_FLAG_OVERLAPPED` named-pipe open and `VrAddOpenNamedPipeInfo` ordering.
The missing part is completion delivery, not pipe-open semantics.

`vrnmpipe.h:255..270` requires a serialized record containing copied Type2,
buffer, bytes/error destinations, ANR and semaphore addresses; `int5c.asm`
later owns all use of those addresses. Thus a worker may retain only host
event/token and copied record data. It cannot call the PIC directly: native
PIC/CPU state is owned by the execution thread.

`BX-CORE-093` is registered for the minimal selector-blind cross-thread
physical-IRQ queue. It admits no BOP/provider semantic in bx-core. Once this
mechanical queue exists, S3 can retain the original source split: bx-vdm
serializes completions and requests IRQ14; `int5c.asm` performs `57:26`, ANR
and IRET on the guest thread.
