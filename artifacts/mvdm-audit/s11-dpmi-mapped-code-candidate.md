# S11 DPMI mapped instruction fetch: local candidate

The selected CPU40 carrier calls `Sim32GetVDMPointer(CS:0)` and adds EIP to
the returned host pointer. SIM32 translates only the selector base. A guest
page remapped independently from that base can therefore cause the decoder to
read a different instruction from the one visible to the CPU. This is an
adapter boundary defect, not evidence of an original guest defect.

On the previously built selected CCPU fixture, cases 93–101 and 104–106
failed in eight of twelve controls: the remapped and cross-page positive
cases were declined, while two negative cases accepted the decoy instruction.
The identity-mapped positive and negative controls passed. The candidate
resolves each used code byte via the selected descriptor and CCPU page
translation, retaining the existing original decoder and its register effects.
The candidate suite adds those 12 cases and four mapped/cross-page `0F 20`
positive and negative controls. The selected-object tests exercise a separate
SIM32 seam and do not themselves prove real guest execution.

The rebuilt selected CCPU suite passed 147 cases, including 18 mapped
positive/negative controls and a missing-second-page control that rejects
mixing the corrected first opcode with operands from the old alias. The
`0F 20` and `0F 22` controls also check the returned or written CR0 value. The selected DPMI object
suite, including its non-CCPU isolation compile, passed with the final source.
These are controlled tests, not a claim that real DPMI paging faults work.

This is not complete exception or concurrency recovery. An unresolved
non-faulting translation falls back to the prior SIM32 path; code in or
translated to non-RAM/ROM and SIM32 linear video aliases also retain that
path. A mapping change between accesses, descriptor lookup reentry, actual
page-fault delivery, and real guest execution remain unproved. The limited
fallback preserves a pre-existing weakness rather than establishing NT4
equivalence. No guest files were modified.

The SIM32 alias and linear base are obtained in one descriptor lookup, so
the candidate does not introduce a second selector walk in the fault path.
The normal SIM32 entry retains its original null, video and address-resolution
branches through that shared implementation. The DPMI decoder uses the mapped
path only for ordinary RAM/ROM; if any needed byte is unresolved it restarts
the entire original decode, rather than mixing bytes from the two paths.
The same ten-component package passed the Full 16-gate suite, Winmine, SOL,
WRITE and four SOL Options rounds. Win3.1 standard/enhanced × DOSONLY 0/1
passed normal startup and exit; 86 binary hashes in both the source and
isolated trees were unchanged. Both release destinations were updated and all
20 published hashes matched the tested manifest. Evidence is under
[s11-dpmi-mapped-code-evidence](s11-dpmi-mapped-code-evidence/). These product
checks do not demonstrate that any guest executed the mapped decoder path or
close the boundaries above. Mirror exception: MVDM-HOST-DIV-488.

The refreshed mirror ledger classifies both changed source files as partial:
`dpmi32/dpmiint.c` retains an estimated 97.97% of original code lines and
`softpc.new/host/src/sim32.c` 99.31%. These are textual matching estimates,
not execution coverage. The C/H inventory check found no unlisted differences
or stale estimates; role-pending entries remain unresolved as before.
