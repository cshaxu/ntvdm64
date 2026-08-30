# M0 T310 S37 — DPMI Bochs-era cleanup

S37 returned the selected DPMI32 mirror bodies to their original SoftPC
`sim32`/SAS/CCPU calling form and removed the nine private DPMI overlay files
that existed solely to stage Bochs-era protected-memory, descriptor, frame and
session-state transactions.  No BOP or DPMI algorithm was replaced: the
original pointer decoding, descriptor loop, interrupt/fault table order and
real-mode stack sequence are again in their original mirror files.

The sole retained DPMI identity divergence is `MVDM-HOST-DIV-018`: the
host-owned `VdmTib.PmStackInfo` address still crosses the original `CX:DX`
guest ABI through the session mapping manager.  That is a host-object identity
crossing, not a Bochs mechanism.

The original `dpmi32/sources` `i386_SOURCES` variant is retained in the mirror
but deliberately not selected by the CCPU40 product graph.  Its
`i386/dpmi386.c` body requires NT4 kernel-VDM operations
`NtSetLdtEntries`, `NtSetInformationProcess(ProcessLdtInformation)`, fixed
NTVDM V86 state and VDM feature bits.  Selecting it reproduced those concrete
missing declarations; it is evidence for the established no-kernel-VDM/V86
boundary, not an invitation to add a second machine route.

The selected portable `SOURCES` package compiles as
`original-mvdm-dpmi32.lib` in the formal Win32/x86 CCPU40 graph after the
narrow `MVDM-HOST-DIV-139` declaration guard prevents the identical modern
`winnt.h` `LDT_ENTRY` carrier from being declared twice.  This changes no
descriptor field or DPMI control flow.

The r10 formal graph also builds the original CCPU40 archive and the original
`cvidc` archive beside the selected DPMI archive.  `cvidc/accessfn.c` is
compiled with its historical non-`PROD` configuration carrier, because that
original conditional body is the source-defined `getXX`/`setXX`, segment,
control-register and SAS bridge.  Symbol inspection confirms exports including
`getEAX`/`setEAX`, `getEBP`/`setEBP`, and `getCR0`/`setCR0`; no replacement
adapter register facade is selected.

The independent r10 whole-graph forced-link audit completed all 154 graph
edges and produced `original-softpc-forced-closure.dll` plus its link log.
The previous DPMI virtual-memory, DPMI lifecycle/fast-BOP, and CCPU
register/segment/control-register bridge misses are absent.  The remaining
unresolved symbols belong to already-separate owners: EMS/LIM writes,
DEM module/debug facilities, host startup UI and VM allocation, and debugger
initialization/hard-error facilities.  They are recorded as their owners'
future boundaries rather than DPMI substitutes.

The production-source negative scan found no `adapter-bochs`, `bochs-core`,
`machine_facade`, or removed DPMI private-overlay reference beneath
`src/mvdm-host` (excluding documentation).  That scan is specific to the
selected DPMI cleanup; it does not authorize broad deletion outside this
owner package.
