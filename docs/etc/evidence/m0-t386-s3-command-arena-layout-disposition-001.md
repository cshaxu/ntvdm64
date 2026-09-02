# M0 T386 S3 — COMMAND arena/layout disposition

## Question

Can the selected immutable original `COMMAND.COM` consume the complete modern
host environment through its original `SVC_GETINITENVIRONMENT` retry without
overwriting still-live transient code?

## Fixed inputs

- Selected guest artifact: `COMMAND.COM`, 50,384 bytes, SHA-256
  `908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`.
- Selected product: CPU40/x86 formal product.
- Guest source: `cmd/command/rdata.asm`, `doskrnl/dos/alloc.asm` and their
  original maps.
- Host source: `dos/command/cmdenv.c`, with its original insufficient-buffer
  `BX` contract restored.

## Source facts

1. `rdata.asm::EndInit` first resizes COMMAND's PSP block with `SETBLOCK`,
   then allocates the initial environment and invokes
   `SVC_GETINITENVIRONMENT`.
2. If the provider reports a larger `BX`, the same original body frees the
   initial allocation, allocates `BX` paragraphs, and invokes the provider
   again. It does not move the transient before that second invocation.
3. The same source explicitly overlays `EndInit` on `Pipe1` so this code can
   be discarded only after initialization. `COMMAND.map` identifies both
   `EndInit` and `Pipe1` at `0000:0332`.
4. `alloc.asm::$ALLOC` begins a low-memory scan at `arena_head` unless the
   already-selected guest `AllocMethod` carries an UMB/high flag. Its normal
   allocator and MCB ownership are guest/NTDOS behavior; the host BOP must
   not invent an alternative allocator.
5. `cmdenv.c::cmdGetInitEnvironment` enumerates the complete process
   environment through the original `GetEnvironmentStrings` loop. It does
   not impose a COMMAND transient-safe capacity limit.

## Direct observation

The fixed direct run retained these source-owned records:

```text
MVDM-CMD-CALL svc=0F stage=0 ax=049F cf=0
MVDM-CMD-CALL svc=0F stage=1 ax=049F cf=0
MVDM-BOP-RETURN 54:0F cs=03F4 ip=03AA ax=049F cf=0
MVDM-BOP-RETURN 54:0F cs=03F4 ip=03CF ax=049F cf=0
```

The first allocation is `0010` paragraphs. The restored original provider
requests `0270` paragraphs and the guest retries at the same `ES=049F`. The
resulting `[0x49f0, 0x70f0)` destination covers later transient execution;
the observed stop is `CS:03F4 IP:1F94` with overwritten environment text as
instruction bytes. The BOP return itself at `03F4:03CF` is intact, so this is
not a BOP-resume defect.

## Disposition

- `MVDM-HOST-DIV-200` stays removed. Returning `BX=0` is not the original
  provider contract and cannot be the final recovery.
- The selected original environment provider plus the selected original
  guest first-fit arena does **not** establish safe operation for the current
  host-environment size. This is a proven compatibility boundary, not an
  unresolved BOP selector.
- No in-scope repair may modify `COMMAND.COM`, NTDOS/NTIO guest media, or
  substitute the DOS allocator.
- Before code selection, the remaining owner audit must determine whether an
  original host startup environment projection exists outside `cmdenv.c`.
  If none exists, a session-owned, explicitly bounded host-environment
  projection is the only remaining non-guest repair class. It would be a
  registered host divergence, not a claim of byte-for-byte OpenNT behavior,
  and requires a separate admission before implementation.

## Immediate next audit

Trace the original host startup path that creates the permanent COMMAND
process environment and compare it with the local app/session launch input.
The audit must distinguish an existing original Base/host composition policy
from a new projection policy; it must not use a reduced environment run as
acceptance evidence.
