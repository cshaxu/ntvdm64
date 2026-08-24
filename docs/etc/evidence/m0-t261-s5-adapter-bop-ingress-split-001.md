# M0 T261 S5 — adapter-bop ingress split evidence

## Inputs

- Baseline: S4 closure commit `694cf087` and
  `m0-t261-s4-p7-dem-host-owner-closure-001.md`.
- Former combined route:
  `src/opennt-bop/ingress/dem_v2_composition_bridge.c`.
- Existing mechanical fixed-width record:
  `src/adapter-softpc/bx_ntvdm_generic_ud_bridge.{h,cc}`.
- Governing S5 brief in `docs/STATUS.md` and T261 proposal.

## Owner map and recovery decision

| Surface | Before | After | Reason |
| --- | --- | --- | --- |
| Fixed generic-UD event/outcome declarations and default mechanical observation | `adapter-softpc` | `adapter-bop/bx_ntvdm_generic_ud_bridge.{h,cc}` | These records are the CPU-to-route copied ingress/completion contract, not a SoftPC/CCPU API.  Their existing names/layout remain unchanged. |
| Exported CPU symbol `bx_ntvdm_mantle_generic_ud_bridge_v1` | OpenNT selector route | `adapter-bop/bx_ntvdm_bop_ingress_v1.c` | The adopted CPU can now reach only a validated, unbound-or-forwarding selector-blind ingress. |
| Selector recognition, service/family precedence, provider dispatch and BOP-aware observations | Same 113-line former combined entry | `opennt-bop/ingress/opennt_bop_route.c` as `bx_ntvdm_opennt_bop_route_dispatch_v1` | This is OpenNT BOP meaning and therefore must remain outside the generic adapter. |
| One-session bind/unbind lifetime | Implicit direct static linkage | `app/ntdos64_bop_composition_v1.{c,h}` called by `bx_ntvdm_engine_run_v1` | `app` performs final composition; reset occurs before bind and at every admitted engine exit. |

No OpenNT provider body, selector precedence, CPU record field, or Bochs source
semantics changed.  The only new project-authored ABI is the same-runtime
route-registration function pointer recorded as `ADAPTER-BOP-001`; it carries
the pre-existing fixed-width copied records and never crosses a guest or
toolchain boundary.

## Procedure and results

1. Moved the generic-UD header and default mechanical source with Git history
   to `src/adapter-bop/`; moved the selector-bearing route to its explicit
   `opennt_bop_route.c` name.
2. Added `adapter-bop` as the eighth linkable module and as an explicit final
   static-library input for each fixture/target in the formal manifest.  The
   final order places it after `bx-core`, so the adopted CPU's external symbol
   resolves without relying on a library rescan.
3. Ran `tests/bx-mantle-generic-ud-bridge-boundary.ps1` after retargeting it
   to the actual owner.  Result:
   `adapter-bop-generic-ud-bridge-boundary: selector-blind fixed bridge verified`.
4. Built and ran the focused `t261-s5-bop-ingress-fixture`: unbound decline,
   single bind, unchanged copied event forwarding, typed resume result,
   double-bind rejection and reset/decline all passed.
5. Generated formal graph `build/t261/s5-r002` and ran its actual native link
   with MSVC x64 `/MT`, CPU5/P-MMX projection and Ninja `-j1`:
   `ninja -C build/t261/s5-r002 -j 1 bin/ntdos64-native.exe`.
   The run compiled 305 scheduled edges and produced
   `build/t261/s5-r002/bin/ntdos64-native.exe`.
6. Re-ran the native target and the focused fixture target through Ninja; both
   reported `ninja: no work to do`, and the focused fixture exited zero.

## Negative boundary review

- `adapter-bop` has no selector-byte literals, family/provider includes or
  DOS/WOW/OpenNT call paths; validation only checks the existing event magic,
  version, structure size, vector and copied window extent.
- `opennt-bop` no longer defines the CPU-called
  `bx_ntvdm_mantle_generic_ud_bridge_v1` symbol; it exports the route callback
  and retains all selector-aware branches.
- `bx-core` retains only `BX-UD-001`'s copied mechanical include, now with a
  statement-level `DIVERGENCE` comment naming the selector-blind adapter.
- No generic `adapter-common`, `adapter-host`, `compat`, VDD, Redirector or
  WOW component was introduced.

## Limitation

This is a component-boundary closure, not BOP-provider completion or a native
guest-continuity claim.  Existing owner packages retain their own provider and
runtime acceptance criteria.
