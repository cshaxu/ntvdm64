# M0 T474 S1 — shared ownership range integration

## Scope and provenance

The owner directed the entire shared-ownership worktree range onto main while preserving commit history. All 51 commits from common ancestor `09b58f5c646cbcbf797748c45f32833a28a48b1d` to sibling head `5ccff9f0d216295ad390b795e4c43c5db7f7b0c1` were applied in order on the T473 closure baseline. The [delivery record](../../../artifacts/mvdm-audit/t474-shared-integration-delivery.json) maps each source commit to its applied commit and records hashes. No guest source or image was changed or rebuilt.

Conflicting `docs/` files used the sibling side and conflicting `assets/` used the existing side, as directed. The single source conflict retained the shared WOW page-domain header and main's original `#line`; the mirror divergence register retained both sides. Later the queued mirror-conditional candidate and this active packet were reconciled into the sibling documentation, retaining the owner's task handoff. A temporarily mixed asset tree was returned to the prior coherent package before testing and then replaced only with the fully verified candidate.

## Integration fixes and verification

The NTVDM resource compiler now receives `PROD`, matching its production C graph; without it, the real resource rebuild selected an explicitly excluded non-PROD `error.h` branch. WOW32's graph was regenerated through the actual CMake target after its private CRT source moved. Product version was advanced to `0.0.474` for the mainline admitted T; protocol stays at 65. An initial Full attempt failed its version gate against the older `0.0.473`; the version fix was rebuilt and the full suite rerun.

Cached x86 and x64 production targets, NTVDM, WOW32, VDMREDIR and both hooks built. The Full selected product suite passed from the fresh isolated package, including DOS/native nested handoff, frontend/worker loss, version negative controls, management, native GUI and DOS4GW startup. Winmine, SOL and WRITE were observed as windows; this is not gameplay acceptance. MSVC same-width x86/x64 process-hook fixtures, all four cross-width payload pairs, and x86/x64 late-guard thread-change tests passed. Shared guest-memory, redirector, worker-thread and WOW compiler boundary checks passed, including their negative controls. Documentation governance passed.

The coherent ten-component package and hook license were published to `assets/release` and `O:/winnt/system32`; every published image hash was checked against the package manifest. Detailed package identities, individual commit mapping, evidence hashes and limits are in the delivery record. These results do not prove untested configurations or resolve the separately queued mirror audit.
