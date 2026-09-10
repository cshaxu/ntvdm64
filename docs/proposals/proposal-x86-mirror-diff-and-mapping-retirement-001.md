# Proposal: x86 mirror-diff and mapping retirement

## Status

Admitted as M0 T403 S1, Ordinary Mode, in
[`docs/states/CURRENT.md`](../states/CURRENT.md). This proposal supplies the
packet scope; the active packet is authoritative for changes and closure.

## Owner objective

For the sole Win32/x86 product, remove every current source, overlay, adapter,
test, and build diff whose purpose is native-x64 compilation or its associated
cross-width abstraction, including the session mapping manager and its token
paths. Retain the original OpenNT/SoftPC x86 ABI and control flow wherever it
is directly representable, then successfully build, link, and stage
`ntvdm32.exe` for owner testing.

## Rationale

The product supports one original MVDM/SoftPC context per Win32/x86 process.
It does not currently offer native-x64 execution, in-process multi-session
isolation, or a released stale-token safety contract. The mapping manager and
the tokenization it requires add material mirror diffs to preserve capabilities
outside that product contract. The priority is therefore source fidelity and a
working original-width product, not speculative cross-width preservation.

## Proposed scope

1. Inventory all active x64-driven divergences and classify each as removable,
   retained for an independent x86 behavior reason, or historical-only.
2. Remove the `mapping_manager` component, its three session instances, and
   unused callback-token infrastructure.
3. Restore original x86-width pointer/HANDLE carriers in the reached DEM,
   DPMI32, Redirector, VDD, SoftPC physical-memory, CCPU, and presentation
   paths; remove only the adapters, overlays, build inputs, and fixtures made
   unnecessary by that restoration.
4. Retain bounded guest-memory access only where the selected x86 source or
   real build still requires a lease/copy mechanism; it is assessed separately
   from identity tokenization.
5. Amend the current architecture/coding rules so they state the actual
   product contract: Win32/x86, one original MVDM context per process, no
   native-x64 or in-process multi-session guarantee.
6. Build the selected CCPU40 Win32/x86 product below its admitted build root,
   verify the I386 PE machine, stage only `build/output/ntvdm32.exe`, and copy
   that verified executable to `O:\ntvdm64\ntvdm32.exe` for owner testing.

## Expected reduction

The known direct removal surface is the mapping-manager core and session
wiring, plus at least 13 host-identity consumers, 4 SoftPC physical-mapping
consumers, and their focused fixtures/build roots. The exact removed-file and
removed-diff count is an admission-time inventory result, not a promise to
delete a file that has a separate current x86 purpose.

## Required proof

- Each removed mirror divergence is compared against its selected original
  x86 source form and registered as removed or retained with a reason.
- No raw native identity crosses process/broker boundaries; the candidate does
  not claim multi-session or cross-width support.
- Focused positive and negative checks cover restored pointer/HANDLE lifecycle,
  SoftPC physical-memory prepare/resolve/cancel behavior, and affected DEM,
  DPMI32, Redirector, VDD, and graphics paths.
- The formal CCPU40 Win32/x86 build and link pass; `ntvdm32.exe` is verified
  as I386 before staging. Runtime observations stay under `O:\ntvdm64\logs`.

## Non-goals and stop conditions

This package does not revive native-x64 compilation, add a second executor,
claim multi-session safety, or change guest-visible behavior merely to reduce
diffs. Pause and re-admit if an original 32-bit carrier cannot safely compose
in the selected x86 graph, if a required interface crosses process boundaries,
or if a real x86 regression requires a new substitute mechanism.
