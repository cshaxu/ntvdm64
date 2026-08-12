# M0 T97 Subtask Plan

## Status

Supporting plan for active `M0 T97`. It does not replace the sole active
packet in `docs/STATUS.md` or allocate later tasks.

| S | Scope | Initial disposition |
| --- | --- | --- |
| S1 | Original DEM namespace/file/search component, ABI, failure, and composition-blocker audit | Complete; see etc/research/t97-s1-dem-namespace-component-abi-map-001.md. |
| S2 | Immutable CLI namespace resource and file-token lifecycle design | Complete; the partial legacy runtime candidate and FASTREAD experiment are withdrawn; see etc/research/t97-s2-immutable-namespace-token-contract-001.md. |
| S3 | Paired pathname/FCB directory-search contract and profile disposition | Active; P1 form-separated source map complete in etc/research/t97-s3-dem-search-form-contract-map-001.md. |
| S4 | Coherent provider integration through the T96 DEM plane, or source-proven unavailable result | Pending S1--S3 closure. |
| S5 | Positive/negative profile fixtures and bounded real-path checkpoint | Pending S4 closure. |

These are component-scale packages. No individual BOP service is an S task.
All work preserves DEM dispatcher ownership and failure behavior, uses only
CLI-admitted immutable resources, and introduces neither Bochs features nor
ambient host-path access.
