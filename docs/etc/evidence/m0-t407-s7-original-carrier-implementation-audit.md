# M0 T407 S7 — Original Carrier Implementation Audit

## Purpose and result

This corrective audit replaces the earlier imprecise conclusion that the
TSS/TR, descriptor-table and FastWOW carriers were simply unavailable
original code.  They are not.  OpenNT splits their implementation between
user MVDM/WOW code and the NT x86 kernel.  The selected standalone CCPU host
cannot compose the kernel half directly, but it must recover every composable
user-mode shape before retaining a compatibility carrier.

This is source evidence and a recovery design, not proof that `WRITE.EXE`
runs.  The existing `MEM.EXE` exit-zero regression proves only the ordinary
DOS descriptor path; it does not validate DOSX protected-mode entry or
FastWOW.

## Sources and procedure

All source inputs were pre-existing local mirrors, read under the source
policy.  The audit used exact symbol/caller searches and direct source review
of the paths below.

| Family | Original owner and source | What the original code does |
| --- | --- | --- |
| DPMI stack/TIB ABI | `src/mvdm-host/dpmi32/dpmiint.c:376`; `src/mvdm-host/dpmi/486/dxboot.asm:1824`; `src/opennt-abi/source/public/internal/base/inc/vdm.h:225` | `DpmiPassPmStackInfo` supplies the packed `VDM_DPMIINFO`; DOSX installs the returned `CX:DX` address as `SEL_VDMTIB`. |
| Process LDT | `src/mvdm-host/dpmi32/i386/dpmi386.c:73` | Original user-mode DPMI publishes descriptors with `NtSetLdtEntries` / `ProcessLdtInformation`. |
| Kernel VDM backing | `O:/repos.external/OpenNT/base/ntos/vdm/x86/vdminit.c:66`; `O:/repos.external/OpenNT/base/ntos/vdm/x86/strtexec.c:90` | The kernel probes the user-resident `VDM_TIB`, saves it in its VDM objects, and swaps monitor/VDM contexts. |
| FastWOW | `src/mvdm-host/wow32/i386/fastwow.asm:256` | Original assembly obtains the native TEB through `FS:[PcTeb]`, reads `TbWOW32Reserved`, then maintains PTD `vpStack` and `WtdFastWowEsp` through callback entry and return. |

The kernel source also explicitly describes the direction of ownership: the
`VDM_TIB` is user-mode memory, while kernel VDM uses it while executing and
swapping trap/monitor context.  Thus the ABI layout and DPMI/DOSX ordering are
recoverable in user mode; a modern standalone process cannot directly reuse
the original kernel trap frame, thread object or native x86 task register.

## Current-state verdict

| Surface | Current state | Original-semantic verdict | Required disposition |
| --- | --- | --- | --- |
| `VDM_DPMIINFO` / `SEL_VDMTIB` | `dpmiint.c` allocates `sizeof(VDM_DPMIINFO)` in guest memory, zeros it and fills the original DOSX-facing fields before returning `CX:DX`. | **Partially source-shaped, unproved lifecycle.** Its layout and immediate BOP ordering follow the original source, but allocation lifetime, all writers, reset and failure paths have not yet been compared exhaustively. | Audit every field/writer against `vdm.h` and original callers; retain the guest-address facade only if that ledger passes. |
| GDT/LDT/IDT | S5 correctly restored the distinction between DOSX GDT source and original process-LDT publication; CCPU receives guest table images. `modesw.c` retains a first-IDT-source latch. | **GDT/LDT source shape improved; IDT reset/republication unproved.** OpenNT has user-mode descriptor publication, so it is wrong to call all table handling a CCPU-only concern. | Keep DPMI/DOSX as table publication owner. CCPU may only bind the already-published tables. Prove successive publication, failed entry and fresh-worker reset before retaining the IDT latch. |
| TSS/TR | `modesw.c` allocates two synthetic 386 TSS records, writes private GDT entries and sets CCPU TR. | **New compatibility carrier, not yet original-equivalent.** It substitutes for the original kernel's real x86 task/trap machinery. | Do not label it restored or transfer it as accepted. Recover original task-state layout/transition expectations first; then either make the smallest CCPU binding or register a four-rung exception with negative tests. |
| FastWOW TEB/TD | `dpmi32.c` allocates a 4 KiB guest page and seeds offsets `18h` and `C0h`, plus a three-DWORD prefix at `100h`. | **Not proven semantic.** The page is a useful bounded safety measure, but the current selected formal process does not itself link the original `fastwow.asm`; therefore its assertion that FastWOW owns all prefix writes is unproved. | Establish the selected WOW32 provider/load path, compare every consumed field and callback write with `fastwow.asm`, then recover original callback logic around one explicit TEB provider facade. |

## Four-rung recovery decision

### DPMI table and TIB path

1. **Direct original source:** retain `dpmiint.c`, `dpmi386.c` and DOSX
   ordering; these sources are present and selected in the host tree.
2. **Smallest facade:** provide guest-linear storage/table binding only where
   the original kernel pointer or process-LDT syscall cannot compose on modern
   Windows.  The facade returns the same `CX:DX` guest address and must not
   create a second DPMI policy.
3. **External-code intrusion:** none admitted.
4. **New behavior:** none admitted.

### TSS/TR path

1. **Direct original source:** original kernel VDM task/trap ownership exists,
   but cannot be linked into a user process because it depends on kernel
   thread, trap-frame and processor state.
2. **Smallest facade:** not yet proven.  The candidate must expose only the
   CCPU state required by the recovered original transition; it cannot invent
   a second DPMI state machine.
3. **External-code intrusion:** none admitted.
4. **Current status:** the synthetic pair is provisional new compatibility
   mechanics and must be traced, source-compared and either reduced or
   registered explicitly.

### FastWOW TEB/TD path

1. **Direct original source:** `fastwow.asm` and its WOW32 callers are
   present.  Their callback/return update order is the source of truth.
2. **Smallest facade:** a dedicated guest TEB/TD view may replace only the
   unavailable NT4 `FS`/native-TEB carrier.  It must present the original
   `TbWOW32Reserved` indirection and exact PTD fields, never the host's modern
   TEB address.
3. **External-code intrusion:** none admitted.
4. **New behavior:** the present anonymous page/prefix is not accepted as
   final behavior.  It either becomes the explicit source-shaped facade or is
   deleted.

## Verification required before acceptance

1. Capture scalar traces at BOP `53:00`, `53:01`, `53:02` and `53:11`:
   source/table base, selector/TI, limit, DPMI-info address and entry result.
   No host pointer may be logged or published.
2. Add a focused CCPU fixture that performs two table publications, a failed
   mode entry and a fresh-worker/reset sequence.  It must show that GDTR,
   LDTR, IDTR and TR never retain stale state.
3. Prove the actual WOW32 DLL chosen by the formal product; then trace one
   original FastWOW callback entry/return, confirming the original order of
   `vpStack`, callback-stack and `WtdFastWowEsp` updates and cleanup.
4. Run `MEM.EXE`, `COMMAND.COM`, `EDIT.COM` and `WRITE.EXE`.  The first three
   are regression coverage only; `WRITE.EXE` must reach DOSX/WOW trace points
   before it is evidence for these protected-mode carriers.

## Consequence for queue ownership

The queued “CCPU40 C-VID and event-path research and restoration” candidate
is a future **candidate**, not an admitted task and not an owner of unproved
DPMI policy.  If admitted later, it may own the narrow CCPU table/TR binding
only after this audit has fixed the original DPMI/WOW publication contract.
T407 retains the source-recovery decision until that boundary is evidenced.

## Confidence and limitation

High confidence: original source exists and ownership is split; the present
synthetic TSS and FastWOW projection are not demonstrated semantic matches.
Medium confidence: a source-shaped guest TEB/TD facade is sufficient; that
depends on the actual selected WOW32 provider and the complete field/caller
matrix.  No statement here accepts current `WRITE.EXE` behavior.
