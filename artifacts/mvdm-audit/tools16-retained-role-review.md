# Relocated historical tools16 roles

The [observer](tools16-retained-observer.py) verifies all 33 missing original
mirror paths have exact SHA-256-matched copies under
`tools/historical/opennt-4.5`, and checks the original mirror paths against
four captured current Ninja graphs and VALID dependency records. It does not
execute a tool or establish behavior of a guest or historical build.

These are **excluded original-mirror file roles**: the runtime product does
not use files at those absent paths. The relocated tools are retained; their
historical build roles and potential use by separate scripts are not excluded.
Per-tool [source-chain](shard-0/tools16-chain-review.md), [compiler](shard-1/historical-tools16-recheck-review.md),
[DOS](shard-2/tools16-unmirrored-review.md) and [tool](shard-3/tools16-build-input-role-review.md)
reviews preserve established relationships and unproved invocation details.
No original guest image or tool was run or modified for this classification.
