# M0 T386 S3 — COMMAND arena/layout disposition

## Question

Can the selected immutable original `COMMAND.COM` consume the complete modern
host environment through its original `SVC_GETINITENVIRONMENT` retry without
overwriting still-live transient code or disturbing its return stack?

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

## Superseded direct observation

The first direct run retained the records below:

```text
MVDM-CMD-CALL svc=0F stage=0 ax=049F cf=0
MVDM-CMD-CALL svc=0F stage=1 ax=049F cf=0
MVDM-BOP-RETURN 54:0F cs=03F4 ip=03AA ax=049F cf=0
MVDM-BOP-RETURN 54:0F cs=03F4 ip=03CF ax=049F cf=0
```

That run linked stale experimental environment objects which are not in the
selected source manifest.  It cannot establish the selected product's DOS
arena contract and is retained only as a rejected observation.

## Corrected direct observation

After removing the stale `mvdm_command_environment.obj` links from the
ignored formal build graph and relinking the selected source set, the same
fixed console-owning CPU40/x86 container produced:

```text
MVDM-CMD-ENV svc=0F stage=0 es=049F bx=0010 ax=049F cf=0
MVDM-CMD-ENV svc=0F stage=1 es=049F bx=0161 ax=049F cf=0
MVDM-CMD-ENV svc=0F stage=0 es=049F bx=0161 ax=049F cf=0
MVDM-CMD-ENV svc=0F stage=1 es=049F bx=0161 ax=049F cf=0
```

This is the original `COMMAND.COM` retry contract: the initial sixteen
paragraph buffer is rejected, the guest reallocates 0x0161 paragraphs, and
the original provider writes its result on the second call.  No BOP resume
failure or `EndInit` overwrite is observed in this run.

The bounded run subsequently stops at `CS:03F4 IP:20F7`, whose matching
immutable `COMMAND.map` location is `BadVerMsg` (offset `20F4`), not the
environment payload.  That correlation identifies the next investigation as
the source-owned DOS-version/PSP and stack-continuity chain.  It is not proof
that the version mismatch itself is the sole fault, because reaching message
data as an instruction address can also result from an earlier stack/control
transfer failure.

A second fixed-container run, with the same original provider and a
default-off SS:SP observer, produced `0010 -> 015F` for its recorded inherited
environment.  `SS:SP` remained `03F4:060D` at entry and return for both
`54:0F` calls.  The required size may legitimately vary with the inherited
environment; the source contract is the two-call required-size protocol, not
a fixed numeric constant.

## Resident HMA binding observation

A subsequent selected-image run retained the following read-only, child-only
records immediately after each unchanged `54:0F` table return:

```text
MVDM-CMD-STUB name=TrnLodCom1 entry=03F4:011C target=FFFF:60F1 state=copied
MVDM-CMD-STUB name=LodCom entry=03F4:0120 target=FFFF:5E5E state=copied
MVDM-CMD-STUB name=MsgRetrv entry=03F4:0124 target=FFFF:644A state=copied
MVDM-CMD-HMA cominhma=1 a20-wrap=0 state=copied
```

The three values are the original patched resident jump-table entries.  They
exclude the earlier hypothesis that `LodCom_Trap` jumps to transient message
data: its `LodCom_Entry` target is the resident HMA entry at `FFFF:5E5E`, not
the conventional `BadVerMsg` location.  `stub.asm::CheckA20` requires A20
when `ComInHMA` is nonzero; the CPU40 read-only query reports `a20-wrap=0`,
which means 20-bit wrapping is disabled and A20 is already enabled.  The
observer uses short mapping-manager read leases only; it neither changes the
stub table, A20, CPU state nor any BOP result.

## Disposition

- `MVDM-HOST-DIV-200` stays removed. Returning `BX=0` is not the original
  provider contract and cannot be the final recovery.
- The corrected selected-source runs prove that the original environment
  provider and guest retry execute as designed for their observed required
  paragraph requests, and that the BOP service does not alter SS:SP. The
  former conclusion that this retry necessarily overwrites `EndInit` is
  withdrawn.
- No in-scope repair may modify `COMMAND.COM`, NTDOS/NTIO guest media, or
  substitute the DOS allocator.
- `base/win32/client/vdm.c::BaseCreateVDMEnvironment` is the original
  BaseCheckVDM pre-launch environment projection. It is relevant when a new
  historical NTVDM process is created, but it must not replace
  `cmdStart`/`cmdGetInitEnvironment` inside the already-running CLI process.
  Its source contract preserves the source environment and adds VDM drive
  state; it is not a generic environment-shortening workaround.
- The completed `54:0F` records, resident table and A20 state exclude the
  environment, BOP return stack, table-patching and HMA-enable hypotheses.
  No environment projection or guest allocator substitute is admitted on the
  basis of the withdrawn observation.

## Immediate next audit

Trace the original post-`EndInit` relocation/control sequence after the
now-proven `LodCom_Trap` HMA transfer: maximum-size DOS `ALLOC`, reverse
`rep movsb` transient copy, `DEALLOC`, resident re-allocation, checksum and
the first command-loop handoff.  The audit must distinguish a guest arena
return, CPU40 string/control execution and the subsequent process/PSP
precondition; it must not use a reduced-environment run as acceptance
evidence.
