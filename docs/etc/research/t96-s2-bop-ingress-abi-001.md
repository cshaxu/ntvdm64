# M0 T96 S2 Shared BOP Ingress ABI 001

## Decision

The common BOP ingress is
src/bx-ntvdm-adapter/bx_ntvdm_bop_ingress_v1.{h,c}. It consumes only the
already-copied instruction window plus copied exception/CPU boundary values.
It emits a fixed-width route fact and initializes one
bx_ntvdm_cpu_result_v2 to pass-through.

It contains no host pointer, guest pointer, guest-memory read, OpenNT
dispatcher call, Bochs API, or provider callback. A source provider may
replace the result only in its separately admitted plane; it is not allowed
to modify the ingress's classification semantics.

## Route meanings

| Route | Meaning | S2 CPU result |
| --- | --- | --- |
| not-bop | The copied bytes are not C4 C4, or the boundary is not #UD. | pass-through |
| incomplete | C4 C4 exists but the bounded window lacks its selector/service byte. | pass-through |
| unknown-selector | Selector is not in the pinned OpenNT top-level inventory. | pass-through |
| mapped-deferred | Selector/service has a source route but no S2 provider. | pass-through |
| not-callable-sentinel | SVC_DEMLASTSVC or SVC_CMDLASTSVC, a dispatcher boundary rather than a service. | pass-through |
| unknown-service | Service lies outside the original family table. The family plane later selects its source-proven failure contract. | pass-through |
| explicit-unavailable | Top-level route whose source meaning is an unavailable historical host feature: BOP_NOSUPPORT, debugger break, or unimplemented interrupt. | pass-through until its owner plane admits an original failure/result contract |

The final column is deliberately identical in S2. OpenNT did not give every
family the same invalid-service behavior: DEM writes carry, while DPMI,
COMMAND debug builds, and Redirector follow different paths. A generic
adapter error result would be a semantic rewrite.

## Provider selection registry

The companion adapter-local registry is
src/bx-ntvdm-adapter/bx_ntvdm_bop_provider_registry_v1.{h,c}. It consumes the
ingress record and produces a fixed-width selection record:

- mapped routes select a stable DEM/WOW/XMS/DPMI/COMMAND/Debugger/Redirector
  or top-level provider family with original-OpenNT precedence;
- explicit top-level unavailability selects the top-level family with
  original-failure-or-deferred precedence; and
- unknown, malformed, sentinel, and non-BOP routes are not applicable.

There is intentionally no callback, module handle, host pointer, or dynamic
library name in this ABI. The registry therefore fixes the route-to-provider
boundary without implying that an original dispatcher is linkable or that a
source-derived replacement is approved.

## Source evidence

- Top-level routing and selector-specific host composition:
  upstream/opennt/base/mvdm/softpc.new/host/src/nt_bop.c.
- Service bounds/dispatch forms:
  dos/dem/demdisp.c, dos/command/cmddisp.c, xms.486/xmsdisp.c,
  dpmi32/dpmi32.c, vdmredir/vrdisp.c, and dbg/dbg.c.
- Complete member and owner evidence:
  artifacts/analysis/opennt-bop-provider-module-map-001-20260811-003/.

## Verification

The independent Clang command below built and ran the focused test with exit
code zero:

    clang -std=c11 -Wall -Wextra -Werror -Isrc/bx-ntvdm-adapter [six adapter ABI sources] bx_ntvdm_bop_ingress_v1.c bx_ntvdm_bop_ingress_v1_test.c

The test covers every callable service index for DEM, COMMAND, XMS, DPMI,
Redirector, and Debugger; both original LASTSVC sentinels; each remaining
inventoried top-level selector; out-of-range services; incomplete and unknown
BOP forms; non-BOP bytes; non-#UD dispatch; deferred original-provider
selection; explicit-unavailable selection; and not-applicable selection.

The default MinGW build probe remains unavailable in this environment because
the MSYS2 runtime cannot create temporary files in C:\msys64\tmp; that is
reported as an environmental limitation, not a passing MinGW result.
