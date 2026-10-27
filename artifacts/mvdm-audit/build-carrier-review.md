# Build-carrier checkpoint

This S10 source/configuration review resolves 119 formerly pending file roles:
100 historical recipe files are excluded as current product build inputs and
19 original `sources` manifests are partially integrated by current generators.
One additional reviewed item, the live MVDM divergence README, remains pending
identity reconciliation. No source-member capability is excluded or declared
working through these metadata decisions.

The disjoint per-path records are:

- [Shard 0](shard-0/build-carrier-disposition.csv): 12 historical recipes and
  four active source lists; [method](shard-0/build-carrier-disposition.md).
- [Shard 1](shard-1/historical-build-role-review.csv): 29 historical recipes,
  five active source lists and the live README;
  [method](shard-1/historical-build-role-review.md).
- [Shard 2](shard-2/build-recipe-consumption-review.csv): 36 historical recipes
  and seven active source lists;
  [method](shard-2/build-recipe-consumption-review.md).
- [Shard 3](shard-3/historical-build-recipes.csv): 23 historical recipes and
  three active source lists; [method](shard-3/historical-build-recipes.md).

The primary review corrected an initially proposed exclusion of three shard-3
manifests: configuration-time `Get-OriginalSources` reads do not appear in a
Ninja target's execution-input closure. The final review therefore inspects
both CMake/generator consumers and generated compile/link ownership. The
T310 generator consumes 18 original MVDM source manifests; its nineteenth
manifest is the project-owned VDMREDIR binding source list. T404 consumes the
nineteenth original MVDM manifest, `wow32/sources`.

The active manifests supply original C-file lists. They do not execute the
entire original NT target, platform, include, test or build environment.
`partial-build-carriers.csv` records why a numeric executable-code restoration
percentage is inapplicable to these declarative files. The source functions
listed by each manifest retain their separate functional audit requirements.
This does not change the requirement that every mirror difference be partial.

The merged 4,684-path ledger now has 224 partial, 501 excluded and 3,959 pending
entries, with no full entries. The 42,963 external audit rows remain a separate
unfinished relevance review. This is concrete classification progress, not S10
closure, runtime recovery or product acceptance. No product or guest bytes
changed; runtime regressions and republication are not claimed for this
documentation-only checkpoint.

The retained [checker](build-carrier-checkpoint.py) pins the pre-checkpoint
ledger at `a01432b08`; its [result](build-carrier-checkpoint-result.json)
verifies 4,684 unique paths, exact shard ownership, the frozen inventory hash,
mirror-difference partial status, the 119 classification changes and all live
source hashes. Expected unmirrored tools are distinguished from missing files
that should have a frozen current identity. The two observed live drifts are
the previously recorded README and concurrent `nt_bop.c` changes; neither is
accepted by this checkpoint. The owned background verification also passed
documentation governance and `git diff --check`. Runtime behavior was not
tested by these checks.
