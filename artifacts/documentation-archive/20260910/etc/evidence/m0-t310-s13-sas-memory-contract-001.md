# M0 T310 S13 — SAS memory-contract owner review

## Question

Can the selected original SoftPC SAS-memory owner retain its CPU_40 ordinary
Intel-RAM lifecycle and scalar/string operations on both host widths without
turning guest addresses into host identities or pre-implementing A20, EMS,
ROM, video or controller semantics?

## Original contract

`softpc.new/base/ccpu386/ccpusas4.c` owns the selected `Sas` vector and the
original `c_sas_*` ordinary-memory implementation.  Its `sas_init` installs
`cSasPtrs`, creates the scratch buffer, calls the original CPU_40
`host_sas_init`, marks the requested RAM span, then follows the original ROM
initialization order.  Under the selected profile, `sim32.c` supplies the
CPU_40 host lifecycle through `InitIntelMemory`/`FreeIntelMemory` in
`host/src/nt_mem.c`.

The body keeps a native internal `Start_of_M_area` backing pointer private to
the original SAS/SoftPC implementation.  Public Intel addresses, SAS vector
arguments and memory sizes remain fixed-width guest values.  The only reviewed
cross-identity route—an externally published physical-page binding—remains
the already closed mapping-manager operation.  It is not duplicated by S13.

## Disposition

The original CCPU SAS vector, RAM type map and ordinary `c_sas_*` algorithms
remain direct.  No new adapter, overlay or pointer cast is needed for their
ordinary RAM path.  The tested direct surface is:

- initialization/termination and `c_sas_memory_size`;
- RAM type-map installation and a reversible `SAS_RAM`/`SAS_ROM` transition;
- byte, word and dword scalar reads/writes;
- original string load/store, forward move, byte fill and word fill.

`c_sas_enable_20_bit_wrapping` and
`c_sas_disable_20_bit_wrapping` retain their original callable shapes but are
not accepted as an XMS/A20 capability here; S15 owns that package.  EMS page
aliasing stays with S14's mapping bindings.  ROM/video type consequences and
their controller callbacks belong to their later firmware/video packets.

## x86/x64 evidence

The formal MSVC `/MT` selected CCPU Ninja graph rebuilt and ran the bounded
fixture on x64 and x86.  Before CCPU execution, each run verified the original
SAS vector and RAM map, round-tripped a four-byte string, moved it forward,
filled byte/word ranges, read back a dword, and made/reverted a type-map
transition.  It then retained the previously closed default extended-BOP
breakpoint observation and bounded CCPU/FPU execution through
`returned-recursive`, exiting zero on both hosts.  No Bochs, MONITOR, kernel
VDM or raw external-pointer route was selected.
