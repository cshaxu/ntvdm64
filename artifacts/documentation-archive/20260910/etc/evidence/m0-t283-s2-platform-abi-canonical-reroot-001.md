# M0 T283 S2 — platform-ABI canonical re-root evidence

## Question

Can the selected OpenNT platform declarations move from provenance-named
source roots to one logical `public/`/`private/` tree without changing a
declaration or breaking the existing original-source carrier closures?

## Inputs

- T283 S1 candidate inventory and generated selection ledger;
- the canonical import manifest generated from that ledger;
- pinned `OpenNT` and `OpenNT-4.5` declaration inputs; and
- original DEM and COMMAND source carriers.

## Procedure and observations

1. The S1 exporter classified all 29 current headers: 18 one-sided, three
   byte-identical dual-source, and eight content conflicts. The eight
   conflicts form the single OpenNT public-SDK ABI cohort; the ledger selects
   the complete OpenNT cohort and rejects the older non-identical OpenNT-4.5
   alternatives without creating a hybrid header.
2. `git mv` re-rooted the 29 selected files to `source/public/...` or
   `source/private/...`. A manifest-driven SHA-256 check proved all 29 moved
   product files equal their selected inputs.
3. A live `src`/`tools` sweep found no remaining edition-named platform-ABI
   root. Historical evidence retains its original paths by design.
4. With the installed MSVC Build Tools environment loaded, Ninja completed
   the formal `build/M0-T283/S2/x86` and `build/M0-T283/S2/x64` graphs. Each
   graph compiled 11 original COMMAND and 16 original DEM translation units,
   then created `original-command-carrier.lib` and
   `original-dem-carrier.lib`.

## Interpretation

The platform-ABI component now has one product declaration topology while
preserving exact selected/rejected provenance outside product paths. Existing
diagnostics are warnings emitted by original source carriers; no declaration
body, provider route, BOP semantics, machine behavior or adapter behavior was
enabled or changed by this task.

## Follow-up

T283 may close after documentation-gate verification. The next ordered task
may use only this canonical declaration tree when converging historical
interface adapters.
