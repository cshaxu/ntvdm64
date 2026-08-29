# M0 T310 S8 P4: ccpusas4 x86/x64 Classification

## Question

Which tracked CCPU/SAS records remain x86/x64 ABI defects after the selected original source and formal target are rebuilt?

## Inputs and procedure

- Forced formal ccpusas4 object rebuilds on x64 and x86.
- Read the reached host_sas_term, LIM, host_simulate, physical-capacity, and BIOS virtual-read paths.

## Observations

- Current source already carries the original UTINY* host_sas_term declaration, typed LIM writes, fixed PHY_ADDR capacity, and the DIV-057 host_simulate contract. None emits the old implicit-call, pointer-level, or narrowing diagnostics.
- bios_read_byte, bios_read_word, and bios_read_double still expose no-result paths after a V86 address-translation failure. That is a virtualisation failure contract, not a host pointer or integer-width transition.

## Result

Six stale ABI records are closed. Three V86 result-path records remain visible for their functional owner; no S8 source change or warning suppression is introduced.
