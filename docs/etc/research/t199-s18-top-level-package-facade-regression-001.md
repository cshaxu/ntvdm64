# M0 T199 S18 Top-Level Package Facade Regression

## Question

Can the admitted top-level BOP group enter one `bx-vdm` facade through common
ingress, preserving each selector's selected bounded result without creating a
new WOW, UI, debugger, scheduler, CONFIG, keyboard, PIC, or BIOS provider?

## Inputs

- T199 S17 grouped disposition and the retained OpenNT owner evidence.
- `src/bx-vdm/bx_ntvdm_bop_ingress_v1.c` and provider registry.
- Existing bounded `config_done` and `spckbd` composition helpers.
- New `bx_ntvdm_top_level_package_facade_v1` and the bound boot-namespace
  composition fixture.

## Procedure

1. Implement one selector-routing facade. It accepts only copied `#UD`
   ingress facts with no service byte and verifies both ingress route and
   provider selection before returning a result.
2. Retain BOP `51`'s WOW family identity; the grouped facade owns its current
   CLI terminal result but does not relabel WOW as a top-level owner.
3. Route `5E` and `5F` through the facade before their existing limited
   composition providers.
4. Build and run the focused MSVC x64 `/MT` facade fixture with
   `tools/probe/Invoke-T199S18TopLevelFacadeX64Probe.ps1`, build root
   `artifacts/build/t199-s18-top-level-facade-r4`.
5. Build and run the bound MSVC x64 `/MT` composition fixture with
   `tools/probe/Invoke-T198S16BootNamespaceVdmX64Probe.ps1`, build root
   `artifacts/build/t199-s18-top-level-composition-r4`.

## Observations

- The focused fixture source-builds and exits zero. It covers every admitted
  selector: `51,59,5A,5B,5E,5F,FE`.
- `51,59,5B,FE` return typed controlled stop. `5A` resumes at `RIP+3` with no
  GPR or flags mutation. `5E` and `5F` classify to their retained, separately
  bounded provider routes and do not receive a fabricated facade result.
- The bound fixture source-builds, links, and exits zero as one MSVC x64 `/MT`
  process. It proves the four terminal selectors, idle continuation, `5E`'s
  existing `AL=0` continuation, and the existing checked-action `5F` path all
  cross common ingress before composition handling.
- Both tests deliberately provide a fourth byte after each top-level selector;
  the facade consumes only `C4 C4 selector`.

## Interpretation And Limits

S18 closes the admitted routing package, not the historical products behind
it. It does not implement WOW (`51`), UI/direct access (`59`), debugger
(`5B`), a host idle scheduler (`5A`), UMB/DASD initialization (`5E`), or
keyboard/PIC/BIOS semantics (`5F`). `5C`, `5D`, and `FD` remain outside this
facade under their separately mapped machine/DPMI admission boundary. No guest
trace was run or used to select work.

## Follow-Up

T199 S19 maps the remaining machine-owned top-level and BIOS forms as one
owner package before admitting any native machine capability implementation.
