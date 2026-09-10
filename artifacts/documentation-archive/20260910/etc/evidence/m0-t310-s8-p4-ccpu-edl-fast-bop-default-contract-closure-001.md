# M0 T310 S8 P4 — CCPU `EDL_fast_bop` default-contract closure

## Original contract and source boundary

The selected original `softpc.new/base/ccpu386/c_main.c` dispatches extended
`0xc4c?` BOP forms to `EDL_fast_bop(immed)`.  `0xc4fe` remains the separate
`c_cpu_unsimulate` case; the default carries the full fixed-width `ULONG`
immediate.

No definition of `EDL_fast_bop` exists in the available OpenNT, OpenNT 4.5,
or OpenNT source-2 baselines.  The registered NTVDMx64 patch does preserve a
source-shaped definition in `mvdm-softpc-patch/patches/common/fmstubs.c`:
the historical body deliberately enters the debugger because this reached
operation has no normal runtime provider.

## Minimal recovery

`c_main.c` declares only the original `void(ULONG)` call contract and records
`MVDM-HOST-DIV-127`.  The patch remains otherwise an evidence archive: its
unrelated empty placeholders are not admitted to the runtime closure.

The formal graph compiles a separate one-symbol archive from the same imported
`fmstubs.c` under `MVDM_SOFTPC_PATCH_EDL_FAST_BOP_ONLY`.  That archive exports
only the original debug-break disposition for `EDL_fast_bop`; it does not add
a BOP provider, alter CPU state, or replace the original failure contract.
The selective patch divergence is registered as `MVDM-SOFTPC-PATCH-004`.

## Verification

The refreshed formal MSVC `/MT` Ninja graphs completed their full selected
`359/359` x64 and x86 builds, including their forced original-machine link.
The final x64 and x86 link logs contain no unresolved `EDL_fast_bop` symbol.
Other `/FORCE:UNRESOLVED` entries remain visible for their own owner-contract
clusters; no warning suppression or unrelated placeholder was added.

## Cluster rule

This repair belongs to the CCPU executor-dispatch cluster.  It is not exempt
because it has been repaired: a later full-cluster review must re-read this
caller, the selected patch body, its build selection, and all sibling CCPU
contract repairs before it can mark the cluster closed.

## Scope boundary

Executing an unmatched extended BOP still deliberately breaks into the
debugger.  This is not BOP semantic recovery and does not authorize an
adapter, guest resume, or host-service route.
