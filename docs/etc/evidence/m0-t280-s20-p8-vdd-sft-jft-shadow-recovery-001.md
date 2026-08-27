# M0 T280 S20 P8 — VDD SFT/JFT shadow recovery evidence

## Result

The reached native `pSFTHead` global is removed.  Its original DS:offset
location is retained numerically as `sft_head_location`.  The reached VDD
functions keep their original names, parameter forms, return forms and source
failure direction, but receive SFT/JFT host shadows from `adapter-softpc`.
No persistent guest pointer remains in DEM state.

`DOSSFT.SFT_NTHandle` now holds a session-owned opaque host-resource identity,
not a truncated x86/x64 native `HANDLE`.  It is resolved only by the same
session's VDD retrieval path.

## Reached direct callers

The only reached source callers which modify returned SFT/JFT pointers are
the WOW open, create and close paths in `wow32/wkfileio.c`.  Each now commits
after its final original write, or discards on an unchanged early exit.
Read-only retrieval callers continue to receive no shadow pointers.

The old direct-pointer VDD body is retained in the mirror file as source
provenance while disabled; the active exported definitions are the bounded
source-shaped wrappers.  No provider/BOP route is enabled by this recovery.

## Formal verification

Both generated graphs compiled and ran their executable fixture successfully
outside the sandbox under MSVC Build Tools 2022:

* `build/M0-T280/x64-vdd-sft-shadow/mvdm_vdd_sft_shadow_fixture.exe`
* `build/M0-T280/x86-vdd-sft-shadow/mvdm_vdd_sft_shadow_fixture.exe`

The fixture proves exact SFT/JFT snapshot/commit behavior, native host-handle
identity round trip, cross-session identity rejection, stale shadow rejection,
and cleanup of an uncommitted shadow through the one session teardown
registration.

The formal DEM `demfile.c` syntax target also passed on x64 and x86.  Its
existing historical warnings are outside P8; it has no P8 conversion error.

## Deliberate limit

This is an adapter/DEM source-recovery closure, not activation of the DEM or
WOW provider.  The inactive original VDD body remains source evidence until
the mirror-component cleanup wave moves its unselected form to the approved
historical evidence tree.
