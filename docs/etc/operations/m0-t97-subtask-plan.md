# M0 T97 Subtask Plan

## Status

Supporting plan for active `M0 T97`. It does not replace the sole active
packet in `docs/STATUS.md` or allocate later tasks.

| S | Scope | Initial disposition |
| --- | --- | --- |
| S1 | Original DEM namespace/file/search component, ABI, failure, and composition-blocker audit | Complete; see etc/research/t97-s1-dem-namespace-component-abi-map-001.md. |
| S2 | Immutable CLI namespace resource and file-token lifecycle design | Complete; the partial legacy runtime candidate and FASTREAD experiment are withdrawn; see etc/research/t97-s2-immutable-namespace-token-contract-001.md. |
| S3 | Paired pathname/FCB directory-search contract and profile disposition | Complete; pathname and FCB are form-separated, host-directory search is excluded, and immutable-resource metadata is the admitted remaining edge; see etc/research/t97-s3-dem-search-form-contract-map-001.md. |
| S4 | CLI-admitted DOS metadata and coherent pathname-provider integration | Active; P1 metadata contract and P2 focused `50:09/0B` DEM-plane integration complete. Runtime fixture replacement and real-path evidence remain. |
| S5 | Positive/negative profile fixtures and bounded real-path checkpoint | Pending S4 closure. |

These are component-scale packages. No individual BOP service is an S task.
All work preserves DEM dispatcher ownership and failure behavior, uses only
CLI-admitted immutable resources, and introduces neither Bochs features nor
ambient host-path access.
