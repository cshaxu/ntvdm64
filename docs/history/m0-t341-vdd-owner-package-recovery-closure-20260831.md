# M0 T341 — Original VDD owner-package recovery closure

## Closed scope

T341 recovered the VDD **host-service owner package** as a complete selected
original source and formal-build surface, while preserving the current
profile's explicit selection of no external installable VDD.

The source/ABI/failure audit is [S1's VDD ledger](../etc/operations/m0-t341-s1-vdd-owner-contract-ledger.tsv).
The selected formal closure is [S2's evidence](../etc/evidence/m0-t341-s2-vdd-no-provider-formal-closure-001.md).

## What is closed

- Original VDD IO hooks, IRQ reservation, DMA, UMB memory hooks, lifecycle
  callbacks, synchronous guest re-entry, termination and DEM DOS-handle APIs
  remain in their original MVDM source owners and are selected in the formal
  CPU40/x86 product link.
- The only selected configuration divergence preserves original query/error
  structure but interprets a missing retired NT4 VDD registry key as a
  deliberate no-provider session selection.
- The existing VDD UMB numeric-address adaptation remains the sole mapping
  boundary; no new VDD mapper, raw pointer identity or generalized adapter was
  added.

## Explicit transfers

- A specific VDD DLL and its guest stub/device contract: future VDD provider
  owner package, admitted per provider.
- VDMREDIR's VDD user-hook use: Redirector owner package.
- Kernel monitor page faults, LPT interception and private CSRSS/monitor
  behavior: hard non-user-mode boundary; no user-mode recreation by T341.
- `ieuvddex` and device samples: debugger/tool/reference owners only.

## Verification

The fixed formal CPU40/x86 Ninja target completed with no required work, and
the final link map contains the source-owned VDD symbols and the narrow
configuration binding.  No runtime observation was needed or used to choose
an API leaf.
