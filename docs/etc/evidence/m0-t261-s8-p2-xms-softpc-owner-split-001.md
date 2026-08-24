# M0 T261 S8 P2 — XMS SoftPC Owner Split

## Question

Can the reached OpenNT XMS package retain its original provider bodies while
removing the mixed XMS/CCPU/SAS shim from `opennt-host` and assigning the
machine-only portion to `adapter-softpc`?

## Inputs

- Original-source mirrors under `src/opennt-bop/xms/`, derived from
  `refs/opennt/base/mvdm/xms.486/` and `refs/opennt/base/mvdm/suballoc/`.
- The former mixed `src/opennt-host/xms/xms_shim.{c,h}` implementation.
- Existing `adapter-softpc` A20, IVT-watch, checked-RAM and CPU-result
  contracts.
- Formal manifest `tools/build/t260-s8-component-manifest.json`.

## Procedure

1. Moved the XMS compatibility header and source-route composition into
   `opennt-bop/xms/` as `opennt_xms_compat.h` and
   `opennt_xms_composition.c`.
2. Kept selector/service validation, source package configuration,
   initialization, `XMSDispatch` and typed BOP resume in that composition.
3. Added `adapter-softpc/opennt_xms_softpc_facade.{h,c}` for only the reached
   CCPU/SAS register shape, A20 mechanism, IVT-read mechanics, checked RAM
   copy and XMS suballocator backing callbacks.  The facade has no selector,
   service, provider or DOS interpretation.
4. Redirected all XMS mirror and ingress includes to the local BOP
   compatibility header; removed the old `opennt-host/xms` manifest input.
5. Generated fresh formal root `build/t261/s8-r003` and ran the full graph,
   then ran the two XMS focused fixtures.

## Observations

- Formal MSVC x64 `/MT` Ninja graph completed all **467/467** compile, archive
  and link edges from `build/t261/s8-r003`.
- `t237-s2-xms-source-mirror-fixture.exe` exited `0`.
- `t237-s3-xms-a20-source-mirror-fixture.exe` exited `0`.
- A production/test/manifest scan for `opennt-host/xms/xms_shim`,
  `src/opennt-host/xms`, `xms_shim.h` and `xms_shim.c` returned no live match.
- `git diff --check` passed.

## Interpretation

The source-owned XMS package is now fully in `opennt-bop`; its existing direct
OpenNT source bodies continue to use source-shaped calls.  The only replacement
surface is a bounded `adapter-softpc` implementation of unavailable historical
SoftPC/CCPU/SAS mechanics.  No new mapping manager, XMS dispatcher, BOP
recognizer, DOS service or Bochs semantic change was introduced.

The retained `GetVDMAddr` shape deliberately yields no escaped native pointer
for this reached XMS route; the imported block-move divergence continues to
use the bounded checked-copy facade.  This is a declared x64 safety divergence,
not a replacement allocator or an unbounded guest-pointer channel.

## Follow-up

P3 replaces the remaining `opennt-host` Redirector and top-level shim inputs
with their original OpenNT owner packages.  This P does not claim UMB mapping,
Redirector completion or new XMS service behavior.
