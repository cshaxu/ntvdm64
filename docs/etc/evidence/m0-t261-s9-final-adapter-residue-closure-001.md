# M0 T261 S9 — final adapter production-residue closure

## Question

Do `adapter-bop` and `adapter-softpc` now contain only selector-blind,
fixed-width mechanical interfaces, rather than OpenNT BOP/NTDOS/COMMAND
interpretation or fixture-only providers?

## Inputs and procedure

- Reviewed the S9 owner/consumer map and every P1--P5c move.
- Scanned adapter production C/C++/headers for BOP, service-family, DOS, WOW,
  DEM, COMMAND, DPMI, XMS and Redirector vocabulary, then classified each
  result by source rather than by a keyword alone.
- Refreshed the existing formal Ninja graph and verified the moved INT06
  provider and ingress fixtures.
- Ran the component-manifest, production-tree, generic-UD bridge-boundary,
  documentation-governance and diff-format gates.

## Result

- Eight source-owned observation pairs and the real-mode INT06 provider/
  `02h`/`06h` dispatcher live under `opennt-bop`; their build inputs now link
  from that component.
- The legacy `50:xx`/`54:xx` host-transaction experiment and generic-UD
  context injection are fixture support only. The latter no longer changes a
  production bridge or finite-run switch.
- The remaining adapter scan results are deliberately non-interpreting:
  retained historical ABI headers under `include/`, source provenance and
  negative-boundary comments, plus same-shaped XMS/SoftPC facades that accept
  only copied typed mechanical requests.
- Refreshing `build/t261/s9-r001` retained cached objects. The final INT06
  fixture rebuild was two Ninja edges (fixture object and link); it did not
  recompile `bx-core`.

## Verification

- `t225-s23-opennt-int06-provider-fixture.exe`: pass.
- `t246-s2-softpc-machine-interrupt-ingress-fixture.exe`: pass after its
  fixture activates the ordinary port-action lifecycle. The former exit `3`
  was the missing fixture precondition, not a provider change.
- `Test-T260S8ComponentManifest.ps1`: pass (299 sources, 79 fixtures,
  one target).
- `Test-T260S8ProductionTree.ps1`: pass (ten production component roots).
- `bx-mantle-generic-ud-bridge-boundary.ps1`: pass.
- Documentation governance and `git diff --check`: pass.

## Interpretation

S9 is closed. The residual historical headers are ABI closure, not provider
ownership, and the remaining same-shaped facades do not identify a BOP
selector/service/family. Configuration-source classification proceeds as S10;
it does not reopen S9 or authorize new BOP behavior.
