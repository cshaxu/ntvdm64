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

The initially considered core hook is rejected: existing `pc_system`
timer callbacks run on the native CPU thread. `BX-MANTLE-093` therefore
records a project-owned mantle queue rather than a Bochs-core intrusion. A
worker may atomically publish only a physical line; the existing finite-stage
timer drains it and asks the existing PIC seam to raise that line. Once this
mechanical queue exists, S3 can retain the original source split: bx-vdm
serializes completions and requests IRQ14; `int5c.asm` performs `57:26`, ANR
and IRET on the guest thread.

## S3 implementation and verification

**Question.** Can the retained asynchronous named-pipe slice be recovered
without moving Redirector, vector or callback semantics into the adopted
machine?

**Source-first ledger.** The directly reusable guest sources are
`namepipe.asm` and `int5c.asm`; they retain the packed 20/24-byte descriptor,
the `BP:BX` SFT-derived token, and the four `57:26` dispositions. The
historical `VDMREDIR` provider translation unit is absent, so it cannot be
directly composed. `src/bx-vdm/bop/shim/redir_session_shim.c` is therefore the
smallest rung-2 seam: it retains `MAXIMUM_ASYNC_PIPES == 32`, copies the
`DOS_ASYNC_NAMED_PIPE_INFO`-equivalent fields, uses a duplicated host handle
and private buffer, and preserves the guest ownership of ANR, stack, far call
and IRET. No core intrusion and no newly invented guest protocol is used.

**Procedure.** A `57:23` or `57:24` event validates a copied descriptor,
looks up the existing opaque 32-bit token, duplicates its private Win32 handle,
issues public overlapped I/O and starts a worker. The worker never reads or
writes guest memory and publishes only physical line 14. The finite-stage
native timer drains that publication on the CPU thread; ordinary native PIC
mask/cascade/acknowledgement produces the existing vector 76h. `57:26` selects
the oldest completed record, writes checked 16-bit count/error results and
returns the original async `ZF=0, CF=1` / copied `AL`, `CX:BX`, `DS:SI` and
`ES:DI` disposition. An empty queue returns `ZF=0, CF=0`.

**Observed verification.** A fresh formally generated MSVC x64 `/MT`, CPU5
Ninja graph at `build/M0-T253-S3/formal-r1` built and ran both
`t253-s2-physical-irq-fixture.exe` and
`t251-s3-redir-ingress-fixture.exe` successfully. The first proves inactive,
invalid, masked, unmasked vector-76 and reset behavior for the queue. The
second creates a local public named pipe, publishes only its opaque token,
submits a packed async-read descriptor, observes completion through `57:26`,
checks copied `go` bytes/count/error and checks the guest continuation values.

**Limit.** This is the local named-pipe asynchronous-completion slice only.
It does not claim NetBIOS/NCB, DLC, remote Redirector protocols, `57:2F/30`
acknowledgements, or a full source-built guest reachability run. Those remain
explicit Redirector-owner work. The timer path is source-integrated and the
producer/drain/PIC stages are independently exercised; an end-to-end guest
resident IRQ76/ANR run belongs to the later Redirector integration package.
