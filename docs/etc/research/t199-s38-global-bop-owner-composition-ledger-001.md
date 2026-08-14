# T199 S38: global BOP owner and composition ledger

## Scope and rule

This is the package-level audit required before further BOP implementation.
It accounts for every selector/service identity admitted by
`src/bx-vdm/bx_ntvdm_bop_ingress_v1.c`; it is not a claim that every identity
has a successful CLI capability.  An identity is covered only when its
adapter owner and disposition are explicit: original provider, original
provider with CLI capability, source-derived rehost, original failure/no-op,
or deferred terminal.

`src/opennt/base/mvdm/inc/bop.h` defines the historical selector families.
`src/opennt/base/mvdm/softpc.new/host/src/nt_bop.c` is the structural host
dispatch reference; it is not linked into the CLI composition.

## Identity inventory

| Historical selector form | Callable inventory | Original component reference | bx-vdm package owner | Current disposition class | Runtime composition member |
| --- | ---: | --- | --- | --- | --- |
| `50:ss` DEM | `00..48` (73) | `dos/dem/demdisp.c` | DEM package session | per-service original/source-derived/capability/failure ledger | boot namespace |
| `51` WOW | one selector | `nt_bop.c` WOW branch | top-level facade | explicit unavailable | boot namespace |
| `52:ss` XMS | `00..0B` (12) | `nt_bop.c` / XMS branch | XMS package session | A20/allocator capability or package-deferred | native BOP session |
| `53:ss` DPMI | `00..18` (25) | `nt_bop.c` / DPMI branch | DPMI package session | explicit unavailable or package-deferred | native BOP session |
| `54:ss` COMMAND | `00..10` (17) | `nt_bop.c` / command dispatch | COMMAND package session | per-service original/source-derived/capability/failure ledger | boot namespace |
| `56` debugger | selector only; byte 4 is guest code | `nt_bop.c` debugger branch | debugger facade | package-deferred | boot namespace |
| `57:ss` redirector | `00..31` (50) | `nt_bop.c` redirector branch | redirector facade | explicit unavailable/deferred, never false success | boot namespace |
| `12`, `15`, `5A`, `5C`, `5D`, `5E`, `FD`, `FE` | selector only | `nt_bop.c` and BIOS/host callouts | top-level and machine facades | package-specific source-derived/deferred/failure | boot namespace |
| `59`, `5B`, `5F` | selector only | `nt_bop.c` historical host boundary | top-level/machine facades | explicit unavailable or constrained handoff | boot namespace |

The non-callable DEM and COMMAND sentinels (`50:49`, `54:11`) and all
out-of-range XMS/DPMI/redirector services are deliberately classified as
sentinel/unknown, never silently forwarded.  Unknown selectors are also a
typed decline.

## Composition correction

The generic bridge always consults the boot-namespace composition first and
the native XMS/DPMI composition second.  Before S38, the production runtime
only initialized/bound the former, so `52` and `53` were compiled but
unreachable in a normal installed CLI session.  S38 makes both sessions
members of `bx_ntvdm_composition_runtime_v1`, with symmetric unbind on reset.
This changes membership only; it does not alter ingress classification,
provider semantics, or Bochs.

## Verification

- `tests/bx-vdm/bx_ntvdm_bop_ingress_v1_test.c` exhaustively classifies each
  callable service range and the sentinels.
- `tests/bx-vdm/bx_ntvdm_bop_profile_regression_test.c` traverses the five
  service families plus every selector-only identity into its provider plane.
- New `tests/bx-vdm/bx_ntvdm_global_bop_composition_v1_test.c` binds the boot
  and native sessions together, then proves one `52:00` event declines from
  the first and resumes from the installed native XMS owner through the sole
  generic #UD bridge.
- `artifacts/build/t199-s38-global-bop-r5/` is the fresh MSVC x64 `/MT`
  source-build/link/run witness for the last assertion.  Its test doubles are
  limited to mantle capability ABI responses; no Bochs device and no guest
  trace were used.

## Consequence for T199

T199 remains the single global BOP package task.  Future S work is admitted
by the owner package (COMMAND, XMS, DPMI, redirector, debugger, top-level or
machine), never because a native trace happened to reach an individual
selector/service.  Trace resumes only after a package's source/ABI/failure
map, provider disposition, implementation and family regression are ready.
