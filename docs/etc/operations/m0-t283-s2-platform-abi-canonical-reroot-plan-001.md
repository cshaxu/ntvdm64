# M0 T283 S2 — platform-ABI canonical re-root

## Objective

Move the selected declaration mirrors, unchanged, from provenance-named paths
to the single logical `public/` and `private/` tree selected by S1.

## Inputs

- S1 candidate inventory and generated selection ledger;
- existing T274/T280 import manifests; and
- the platform-ABI canonical-union proposal.

## Scope

S2 may use `git mv` only for the 29 selected declaration files and may update
their manifests, README and live include/build inputs to the canonical roots.
It does not edit declaration text, synthesize headers, enable a provider, or
change any BOP/machine behavior.

## Exit

No live product/build/test input names an edition-specific platform-ABI source
root. Every moved file is hash-equal to its S1 selected source, and manifests
retain the selected/rejected provenance rather than encoding it in the path.
