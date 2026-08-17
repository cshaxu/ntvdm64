# T225 S3 P1 EXEC File-I/O Source, ABI, and FastRead Map

## Scope

This map covers the one owner package required by guest DOS EXEC file access:
DEM 50:12 open, 50:00 seek, 50:42 fast read, 50:16 slow read, and 50:02
close. It does not implement EXEC, COM/MZ loading, PSP, MCB, or parent return.

## Original Guest Contract

OpenNT DOS uses the SFT field sf_NTHandle as a two-word host token. file.asm
stores BP as the low word and AX as the high word; dem.h GETHANDLE(AX,BP)
reconstructs the corresponding historical host handle.

handle.asm FastOrSlow selects slow only for a pipe or a MIPS state bit. The
normal x86 file path invokes 50:42. If it returns carry set, the guest invokes
50:16. On either successful read, the guest, not DEM, advances sf_position by
AX bytes and returns CX bytes to its caller. msproc.asm ExecRead calls the
ordinary guest READ path around its temporary EXEC allocation management.

The retained demdisp.c table maps SVC_DEMFASTREAD to demNotYetImplemented,
whose body clears carry. That establishes neither a usable Direct fast-read
transfer nor a source-proven Direct carry-failure disposition: it leaves AX
unmodified although handle.asm treats AX as transferred byte count.

## Current Route Comparison

| Service | Direct current route | Readonly current route | S3 decision |
| --- | --- | --- | --- |
| 50:12 | Whole-provider namespace, typed PDB-owned opaque token | Declared startup namespace, same AX:BP token ABI | Retain. |
| 50:00 | Whole-provider handle partition | Readonly namespace handle path | Retain. |
| 50:42 | Not owned by whole-provider handle partition | Existing readonly fast-I/O provider and checked bulk write | Add Direct to the same whole-provider handle owner. |
| 50:16 | Whole-provider checked RAM read | Readonly checked bulk write | Retain as slow path. |
| 50:02 | Whole-provider token close | Readonly namespace close | Retain. |

## Selected Direct FastRead Contract

The smallest composable contract is a source-derived Direct fast transfer, not
an invented carry fallback:

- input token remains opaque AX:BP and resolves only inside the Direct file
  session;
- DS:DX destination, CX requested count, ZF-directed supplied position, and
  checked guest RAM write use the same copied ABI as 50:16;
- output is RIP+4, AX transferred count, and carry clear; no SFT position is
  changed by bx-vdm;
- invalid token, bad guest range, or host read error use the existing typed
  Direct read failure shape;
- Readonly retains its existing 50:42 provider; Overlay and Virtual remain
  outside this S3 change and may not fall through to Direct.

This aligns with guest ownership: FastOrSlow and handle.asm remain responsible
for path selection, position update, and slow-read continuation. The adapter
performs only the selected host transfer.

## DTA and PDB Admission

The existing whole-provider fixture proves that 50:1B's copied DTA registration
can read the CurrentPDB word and supply the Direct namespace opener. S3 must
extend that same fixture through 50:12 then 50:42 then 50:02 under Direct and
Readonly, including a stale-token negative. It must not infer that this alone
proves a real guest EXEC/PSP path; COM loading remains a later owner package.

## Recovery Ledger

| Rung | Decision |
| --- | --- |
| Original source | Retain guest SFT token layout, FastOrSlow selection, guest position update, and ordinary READ ownership. |
| Smallest adapter seam | Extend one existing whole-provider read owner with a typed Direct 50:42 transfer. |
| External code intrusion | Rejected. No Bochs or guest DOS change. |
| New behavior | Rejected. No adapter EXEC loader or synthetic child process. |
