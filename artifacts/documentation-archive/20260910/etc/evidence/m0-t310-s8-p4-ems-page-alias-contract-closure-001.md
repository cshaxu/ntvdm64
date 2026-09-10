# M0 T310 S8 P4 EMS Page-Alias Contract Closure

## Scope

This closes only the historical user-mode boundary reached by original
`softpc.new/host/src/nt_emm.c`:

- `VdmMapDosMemory(DosIntelPageNo, VdmIntelPageNo, PageCount)`;
- `VdmUnmapDosMemory(DosIntelPageNo, PageCount)`.

All three available OpenNT baselines contain the callers and contain no
user-mode provider body. The services were NT4 kernel-VDM operations.

## Recovered Contract

The original EMS code computes both inputs as 4 KiB Intel physical-page
numbers. Mapping aliases the conventional EMS frame destination pages to the
already allocated EMM backing pages. Unmapping removes that alias. Success is
`NTSTATUS` zero and failures preserve the original `NT_SUCCESS` branch in
`host_map_page` / `host_unmap_page`.

`adapter-mvdm-host-out/softpc/mvdm_softpc_physical_mapping.c` now owns a
session-owned, reversible destination-to-source physical alias record. It does
not publish a host pointer or a second mapping manager. The existing session
`guest_memory` mapping-manager instance remains the sole route for an external
host-backed range; an EMS alias contains only numeric guest physical addresses.

The original `c_GetPhyAdd` first translates a matching destination physical
address, then retains the established external-range resolver and its unchanged
normal SoftPC RAM fallback. Thus the adapter neither identifies DOS/EMS
services nor changes CPU instruction execution.

## Mirror Changes

- `MVDM-HOST-DIV-096`: include the source-shaped service declarations in
  original `nt_emm.c`.
- `MVDM-HOST-DIV-097`: one numeric alias translation probe in original
  `ccpusas4.c` before existing SAS lookup.
- `ADAPTER-SOFTPC-026`: adapter implementation and teardown ownership.

## Verification

Outside the sandbox, formal MSVC `/MT /W4` compilation succeeded for both
architectures:

- x64: `nt_emm.c`, `ccpusas4.c`, and
  `mvdm_softpc_physical_mapping.c` compiled successfully; the new adapter
  emitted zero warnings.
- x86: the same three translation units compiled successfully; the new
  adapter emitted zero warnings.

The original-source warnings still visible in `nt_emm.c` / `ccpusas4.c` are
unrelated source-quality diagnostics and remain visible; none was suppressed.
The formal Ninja generator now selects the adapter source in
`softpc-bindings.lib` when regenerated.
