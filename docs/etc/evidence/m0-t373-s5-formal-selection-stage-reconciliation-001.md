# M0 T373 S5 — Formal WOW selection and staging reconciliation

## Question

Does the selected CPU40/x86 product and its fixed runtime package select a
source-shaped original WOW32/WOWEXEC cohort that makes a WOW16 observation
eligible?

## Inputs

* The selected original dynamic ingress in
  `src/mvdm-host/softpc.new/host/src/nt_bop.c:MS_bop_1` and the S2 boundary
  record `m0-t373-s2-p2-wow32-dynamic-ingress-boundary-001.md`.
* Fresh formal graph root `build/M0-T373/S5/formal-x86`, generated with the
  repository Node 22 executable and the CPU40/x86 graph generator.
* `build/M0-T373/S5/formal-x86/build.ninja`, SHA-256
  `dc3e6850a4bc21624ef28a9202a6f6e0241d5144c4649713a964a8c771461041`.
* `build/M0-T373/S5/formal-x86/source-manifest.json`, SHA-256
  `03e992ec10e052b3d127f9cffe5b653d9b747e243796312aeb2de969025b0cbc`.
* Fixed staged package `build/M0-T373/S5/runtime/runtime-manifest.json`.
  Its product is the already formal-linked CPU40/x86 executable
  `build/M0-T371/S2/formal-x86/original-softpc-process.exe`, SHA-256
  `7b6011aaa477eff1e00a541e08fc6e0a6f294fef1cbdf67e91cdf8fdeff5d457`.

## Procedure

1. Generated the fresh CPU40/x86 Ninja graph with
   `New-T310OriginalSoftpcNinja.ps1`, explicitly supplying Node 22.
2. Ran `ninja -C build/M0-T373/S5/formal-x86 -n
   original-softpc-process.exe`; the graph resolved all 430 planned actions,
   ending in the selected final link.
3. Searched the generated graph and source manifest for `wow32`, `wkman` and
   `wcall16`; searched the staged manifest for WOW/Win16 assets.
4. Staged the fixed product with `Stage-OriginalSoftpcRuntime.mjs` and read
   the resulting immutable-media manifest.

## Observations

* Neither the formal graph nor its source manifest selects `wow32`, `wkman`
  or `wcall16`.  It selects only
  `adapter-mvdm-host-out/win32/source/wow_private_unavailable.c`, the explicit
  private-WOW boundary—not a WOW32 provider, task dispatcher or DLL.
* The staged package contains the selected DOS guest binaries, `REDIR.EXE`,
  `DOSX.EXE` and original SoftPC ROM/CMOS assets.  It contains zero WOW/Win16
  media entries.
* The original selected ingress is still `MS_bop_1` dynamic loading of
  `WOW32`.  With no selected provider or media, its load-failure direction is
  the only source-shaped product result available to this package.

## Interpretation and confidence

The formal selection, fixed media package and S1--S4 source topology agree:
the product intentionally has no original WOW32/WOWEXEC execution cohort.
Consequently a fixed Win16 observation is **not eligible**.  Running one
would require adding a DLL and/or guest media, which would violate the active
packet's stop conditions and would not prove the original package had already
been selected.

Confidence is high for the selected CPU40/x86 product disposition: it is based
on a fresh generated graph, its dry-run final link closure and the actual
staging manifest.  This is not a runtime observation and does not claim a
WOW16/NE lifecycle executes.

## Follow-up

T373 closes its intended vertical as exact original-unavailable without a
substitute.  A later admitted WOW service package must recover a complete
source-defined WOW32 product cohort—including its private USER/shared-session
dependencies and immutable guest media—before it changes either formal
selection or staging.
