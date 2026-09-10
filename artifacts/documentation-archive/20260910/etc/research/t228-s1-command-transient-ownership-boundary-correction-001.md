# T228 S1 P8 - COMMAND transient ownership-boundary correction

## Question

Which owner may repair the zero-filled TrnSeg=5A5F destination proved by P5
and P6? In particular, is the initial transient allocation/move a bx-vdm BOP
provider responsibility, or guest COMMAND/NTDOS behavior?

## Original-source facts

The original COMMAND initial path in
src/opennt/base/mvdm/dos/v86/cmd/command/rdata.asm, EndInit, does all of the
following in guest code:

1. invokes DOS INT 21h AH=SETBLOCK to resize its resident block;
2. invokes AH=ALLOC, first with BX=FFFFh to discover the largest available DOS
   block and then with the returned paragraph count to allocate it;
3. derives TrnSeg from the top of that temporary allocation;
4. copies RESGROUP:TranStart..TRANGROUP:TranSpaceEnd to the chosen segment
   using STD; REP MOVSB; CLD;
5. invokes AH=DEALLOC to release the temporary block; then
6. transfers through LodCom_Trap, where resident COMMAND allocates its final
   block and either preserves/moves the transient or verifies/reloads it.

The OpenNT guest DOS owner for those three INT 21h services is
src/opennt/base/mvdm/dos/v86/doskrnl/dos/alloc.asm ($ALLOC, $SETBLOCK,
$DEALLOC). Its original dos/makefile includes alloc.obj in the NTDOS link
list. The retained primary source-built NTDOS map identifies one complete
DOSCODE/DOSDATA image; it does not substitute a host allocation provider.

command2.asm makes the distinction explicit. LodCom/LodCom1 use the same
guest DOS allocation services and preserve or relocate the existing transient.
Only after ChkSum fails does LoadCom run the OPEN, LSEEK, READ, CLOSE sequence
in ReadCom; that latter sequence can exercise the existing DEM file-service
family (50:12, 50:00, 50:16, 50:02).

## Corrected ownership disposition

| Lifecycle portion | Original owner | Current repair authority | What P5/P6 prove |
| --- | --- | --- | --- |
| Initial allocation, TrnSeg calculation, reverse copy and release | Guest COMMAND plus guest NTDOS allocator | Guest control-flow / NTDOS memory-arena / bx-core-bx-mantle ordinary-RAM continuity diagnosis | The selected destination was zero later; not which initial operation failed. |
| Final resident allocation and optional transient-to-transient relocation | Guest COMMAND plus guest NTDOS allocator | Same guest owner domain | The observed pointer names the resulting TrnSeg; it does not prove preservation or relocation. |
| Checksum decision | Guest COMMAND | Guest owner domain | No checksum result is yet observed. |
| Reload open/seek/read/close | Guest COMMAND through guest NTDOS file calls and DEM provider | bx-vdm file provider only after the original reload branch is proven reached | The current provider has local lifecycle evidence, but native branch reach is unproven. |
| Far transfer through Trans | Guest COMMAND / bx-core execution | Guest control-flow continuity; bx-core only for demonstrated neutral mechanics error | Pointer is source-consistent; target was zero. |

## Consequence

The P6 owner-package map remains valid in its statement that the whole
COMMAND transient lifecycle must be considered together. Its statement that
the repair belongs in bx-vdm as one COMMAND transient composition provider is
superseded for the initial allocation and relocation stages: that would
incorrectly move original guest DOS semantics into the adapter.

No new BOP provider, adapter RAM seed, guest-image mutation, or Bochs
intrusion is admitted by this correction. The next diagnostic must determine
whether native execution reached EndInit and whether guest NTDOS allocation
preserved ordinary RAM across the original allocate/free/reallocate sequence.
Only evidence of the checksum/reload branch permits exercising the existing
DEM file lifecycle as a candidate repair domain.

## Confidence and limitation

Confidence is high for the ownership split because the control flow and
service numbers are explicit in the retained original assembly. The current
observation cannot yet identify the first failed stage. It therefore does not
justify a repair, and it does not re-open trace-selected BOP work.