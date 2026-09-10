# r13 Ordinary-RAM Observation Closure

## Claim

Bochs now has a generic `BX_MEM_C::copy_from_ordinary_ram` primitive that can
copy an adapter-owned bounded output span only after every affected page is
proved to be ordinary readable RAM. It is a prerequisite for a future stopped
machine observation transaction; it is not yet callable by the adapter.

## Source Contract

The method is the read-side companion to `copy_to_ordinary_ram`. It rejects a
null destination, zero length, address overflow, A20 remapping, memory-handler
overlap (including direct-access handlers), monitor pages, and missing ordinary
read mappings before copying any byte. It performs no physical-read, DMA, ROM,
VGA, PCI or device-handler fallback and retains no mapping after return.

The implementation names no profile, physical address, selector, BOP, OpenNT,
DOS/WOW/DEM/COMMAND term, session, adapter ABI or host capability. Its only
inputs are `addr`, `len` and an adapter-owned byte span.

## Reproduction

The r12 UCRT container was refreshed only with the changed Bochs memory source
and compiled directly:

```text
make -j1 memory/misc_mem.o
ar r memory/libmemory.a memory/misc_mem.o
ranlib memory/libmemory.a
```

The compile succeeded. No recursive Bochs build was started and no executable
or runtime claim is made from this object check.

`tests/bx-ntvdm-adapter/Test-BochsOrdinaryRamBoundary.ps1` independently
asserts the complete-preflight safety tokens and rejects physical/DMA fallback,
fixed startup addresses and all forbidden semantic vocabulary.

## Limit

No adapter request/response API currently calls this method. Defining that API
requires one shared, typed observation transaction with explicit requester,
range, stop epoch and output lifetime rules. Until then, it cannot observe
NTIO, IVT, BDA or arbitrary guest memory.
