# T228 S1 P25 — BOP Return and INT 21 Termination Source Audit

## Question

Does P24's zero completed-IRET result indicate an incorrect `bx-vdm` BOP
resume contract, or does original OpenNT source retain a distinct guest
COMMAND/NTDOS return and control-transfer owner domain?

## Inputs

- `src/opennt/base/mvdm/inc/bop.h:25..50`
- `src/opennt/base/mvdm/inc/cmdsvc.h:7..34`
- `src/opennt/base/mvdm/softpc.new/host/src/nt_bop.c:314..322`
- `src/opennt/base/mvdm/dos/v86/cmd/command/rdata.asm:450..686`
- `src/opennt/base/mvdm/dos/v86/cmd/command/command2.asm:292..467`
- `src/opennt/base/mvdm/dos/v86/cmd/command/stub.asm:92..128,186..190`
- `src/opennt/base/mvdm/dos/v86/cmd/command/init.asm:2376..2418`
- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msdisp.asm:365..675`
- `src/opennt/base/mvdm/dos/v86/doskrnl/dos/msctrlc.asm:1083..1085`
- P18, P19, P21, P22, P23 and P24 retained results.

## Source Facts

`BOP(BOP_CMD)` emits the three-byte `C4 C4 54` instruction and `CMDSVC`
places its service byte immediately after it.  In the historical host
composition, `MS_bop_4` reads the byte at the already advanced guest IP,
calls `CmdDispatch`, then advances IP by one more byte.  The present bridge
receives the fault at the first `C4` and resumes at `fault_rip + 4`: the same
four-byte instruction-plus-service consumption.  P18's `54:0F` provider also
only supplies its source-derived `BX`/checked write result before that same
resume.  There is no source basis for changing that BOP resume length or for
adding an IRET to it.

Original `EndInit` is different.  It uses guest `INT 21h` for `SETBLOCK`,
`ALLOC` and `DEALLOC`; NTDOS `COMMAND` saves the caller state, dispatches on
its internal stacks, and normal completion reaches `LeaveDos`, then
`DOIRET`, then `FIRET`.  Thus a completed real-mode IRET is an expected
outcome only for an INT 21 path which reaches that ordinary NTDOS completion
route; it is not the BOP return mechanism.

After the `EndInit` transient placement, original COMMAND jumps through
`LodCom_Trap`.  The low stub then jumps through `LodCom_Entry`.  `Patch_stub`
constructs that far pointer from the relocated resident code and the
`Reloc_Table`; `LodCom` performs the subsequent largest-block allocation,
optional transient relocation, checksum/reload decision, `HeadCall` copy and
far jump to `Trans`.  This is one guest COMMAND package with NTDOS MCB/INT21
preconditions, not a host BOP provider.

## Correlation With Current Evidence

P18 already repaired the only demonstrated source-derived initial-environment
provider divergence.  P22 confirms the selected DEM file lifecycle finishes
before EndInit and does not recur after the two mapped `54:0F` calls.  P21
observes generic real-mode software interrupts only at delivery; P24 records
no successful real/V86 IRET post-commit.  Neither record contains the
function identity, stack frame or guest-memory contents required to name an
individual NTDOS instruction.

Accordingly, P24's zero is not evidence that BOP `+4` resume is wrong.
It only excludes a bad state after a successfully completed observed IRET,
and leaves the complete guest `EndInit → NTDOS allocation → LodCom stub /
relocation → transient` control domain unresolved.

## Recovery Ledger and Disposition

| Rung | Disposition |
| --- | --- |
| Original source reuse | Active: hash-locked historical COMMAND and NTDOS guest images execute these routines. |
| Smallest adapter/shim | Already limited to the P18 source-derived `54:0F` provider.  It cannot own guest MCBs, stub relocation, transient copies, or INT 21 return. |
| External-code intrusion | No new intrusion selected.  P20/P23 observations are default-off and completed; their results do not select a semantic machine repair. |
| Newly authored behavior | Rejected: BOP resume change, adapter IRET, guest allocator, synthetic `LodCom_Entry`, transient seed/copy, or host-child surrogate would replace original guest behavior. |

## Result and Follow-up

P25 closes only this source interpretation.  It confirms that the active
repair boundary remains the complete original guest COMMAND/NTDOS transient
lifecycle, now explicitly including the resident-stub relocation/transfer
leg.  It selects no BOP, provider, CPU or Bochs change.

A follow-on must first admit one whole-package source/ABI/failure comparison
that can distinguish the original NTDOS completion/MCB/relocation states
without a service-specific trace rule.  It must not turn `54:0F`, an INT 21
function number, or a BOP selector into a leaf repair task.
