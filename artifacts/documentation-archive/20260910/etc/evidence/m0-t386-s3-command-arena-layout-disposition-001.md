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

The bounded runs later stop in the immutable COMMAND resident address range,
but the map offsets are link-map addresses rather than direct COM-file/runtime
offsets. They must not be used as a direct `IP -> message-label` diagnosis
without an explicit source/binary relocation correlation. In particular, the
original `ConProc` version check occurs before the reached `54:0F`
environment calls; the read-only `PDB_Version=0005` record is consistent with
that source order, but does not independently prove every DOS interrupt
return.

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
MVDM-CMD-HMA-CODE target=FFFF:5E5E bytes=1F 83 C4 02 80 3E A6 02 state=copied
```

The three values are the original patched resident jump-table entries.  They
exclude the earlier hypothesis that `LodCom_Trap` jumps to transient message
data: its `LodCom_Entry` target is the resident HMA entry at `FFFF:5E5E`, not
the conventional `BadVerMsg` location.  The first eight bytes at that exact
target decode as the opening source instructions of `command2.asm::LodCom`
(`pop ds; add sp,2; cmp ExtCom,0`), proving the HMA far target is executable
resident code rather than an alias into text.  `stub.asm::CheckA20` requires
A20 when `ComInHMA` is nonzero; the CPU40 read-only query reports
`a20-wrap=0`, which means 20-bit wrapping is disabled and A20 is already
enabled.  The observer uses short mapping-manager read leases only; it neither
changes the stub table, A20, CPU state nor any BOP result.

## Resident-block scalar discrepancy observation

A later fixed-container run records the source-owned scalar immediately after
each original `54:0F` table return. On the first return, the immutable guest
has `BX=0254h` and `DS:EnvSiz=0010h`, so its unchanged code correctly enters
the free/reallocate/retry branch. On the second return, `BX` is still `0254h`,
and `DS`, `SS`, and `SP` remain `03F4h`, `03F4h`, and `060Dh`; the BOP has not
clobbered the guest register contract. However, the unchanged comparison word
is then `DS:203Ch = 6570h`, rather than the `0254h` written by the original
`mov EnvSiz,bx` before the second BOP.

The follow-up selected-image observation establishes the missing identity:

```text
MVDM-CMD-ENV-DS msw=0010 selector=03F4 base=00003F40 limit=0000FFFF expected-base=00003F40
MVDM-CMD-ARENA ds=03F4 ressize=00A2 mcb-size=00A2
```

CPU40 is in real mode and its hidden `DS` base exactly equals the mapping
manager's `03F4h << 4`; the original COMMAND `ResSize` input and the NTDOS MCB
result also agree.  Thus NTDOS has correctly performed the source-defined
`SETBLOCK` request, and the large `0254h`-paragraph environment beginning at
`049Fh` does reach the no-longer-retained `DS:203Ch` initialization scalar.
This is not a BOP, CPU-cache, or generic NTDOS allocator failure.

OpenNT does contain the expected provider-side retry protocol in
`cmdenv.c::cmdGetInitEnvironment`: it snapshots/transforms the inherited
environment, returns its paragraph count in `BX`, and lets immutable
`COMMAND.COM` free/reallocate/retry.  `base/win32/client/vdm.c::BaseCreateVDMEnvironment`
also projects the process environment before VDM creation, but it preserves the
complete variable set while converting path values; it is not an arbitrary
size cap.  The historical composition therefore does not supply a safe
placement mechanism once a modern inherited environment exceeds the space the
released initialization layout happens to leave below its live data.

The original `COMMAND.COM` retry remains correct and immutable.  No allocator
repair, environment truncation, or guest patch has been selected.

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
- The completed `54:0F` records, resident table, resident HMA code and A20
  state exclude an environment-capacity failure, BOP-return-stack change,
  table-patching failure and HMA-enable failure as explanations for the
  observed retry.  CPU40 real-mode binding and the original `SETBLOCK` MCB
  result have now also been verified; the remaining issue is historical
  initial-environment placement, not an unproven CPU or allocator defect.
  No environment projection or guest allocator substitute is admitted on the
  basis of the withdrawn observation.

## Immediate next audit

Select a source-shaped placement design for the complete environment before
the immutable `EndInit` retry runs.  The design must preserve the complete
environment, leave `COMMAND.COM`/NTDOS/NTIO bytes untouched, and state which
historical composition boundary owns any placement policy.  Only then trace
the original relocation/control sequence: maximum-size DOS allocation, reverse
`rep movsb` transient copy, `DEALLOC`, resident re-allocation, checksum and the
first command-loop handoff.  It must establish an explicit
source-to-immutable-binary address relation and must not use a reduced-
environment run as acceptance evidence.
