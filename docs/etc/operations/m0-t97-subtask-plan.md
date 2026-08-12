# M0 T97 Subtask Plan

## Status

Supporting plan for active `M0 T97`. It does not replace the sole active
packet in `docs/STATUS.md` or allocate later tasks.

| S | Scope | Initial disposition |
| --- | --- | --- |
| S1 | Original DEM namespace/file/search component, ABI, failure, and composition-blocker audit | Active. |
| S2 | Immutable CLI namespace resource and file-token lifecycle design | Pending S1 evidence. |
| S3 | Paired pathname/FCB directory-search contract and profile disposition | Pending S1/S2 evidence. |
| S4 | Coherent provider integration through the T96 DEM plane, or source-proven unavailable result | Pending S1--S3 closure. |
| S5 | Positive/negative profile fixtures and bounded real-path checkpoint | Pending S4 closure. |

These are component-scale packages. No individual BOP service is an S task.
All work preserves DEM dispatcher ownership and failure behavior, uses only
CLI-admitted immutable resources, and introduces neither Bochs features nor
ambient host-path access.
