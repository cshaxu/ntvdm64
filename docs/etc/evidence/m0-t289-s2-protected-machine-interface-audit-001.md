# M0 T289 S2 protected-machine interface audit

The two S2 ledgers establish the only permitted successor shapes for every
reached original DPMI protected-machine import. Existing adapter operations
cover only checked memory and a small 16-bit register subset. All 32-bit CPU,
mode, descriptor and exception mechanisms are named S3 work, not assumed
available.

The audit explicitly refuses three historical compositions: raw
`Sim32GetVDMPointer`, process-host `LDT`/`VdmTib` pointer publication, and
NT4 `NtVdmControl`/`VdmInstall*Handler` monitor hooks. The first becomes a
bounded lease/copy by source group; the second has no current successor; the
third remains with a named monitor/VDD/debugger owner.

S3 may implement only selector-blind typed mechanics and must prove them on
x86 and x64 without compiling or routing a DPMI provider. It may not add a
mapping manager, decode BOP selectors or expose Bochs types to MVDM source.
