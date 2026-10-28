# Shard 2: CPU486 guarded instruction inputs

Three pending files match both original trees. The observer and JSON record current build flags, object edges/COFF section flags and a normalized Ninja dependency. The original decoder/profile analysis in `../shard-3/cpu486-profile-boundary.md` remains the broader authority.

| Entry | Evidence and bounded disposition |
| --- | --- |
| `cmpxchg.c` | Selected object is a CCPU library input, but the source's implementations are under `SPC486` and the captured object has no code section. Keep **pending** for the CPU486 behavior/profile decision; an empty compiled object must not count as restored CMPXCHG. |
| `xadd.c` | Same selected-object and guarded-body result. Keep **pending** for instruction, flags, faults and execution evidence. |
| `wbinvd.h` | Exact normalized Ninja dependency of the selected `wbinvd.obj`; that object's implementation is guarded by `SPC486` and its captured object has no code section. Keep **pending** for active profile and cache-model semantics. |

The captured `ccpu_flags` selects `SPC386` and does not directly define `SPC486` or `CPU_486`; the three objects nonetheless appear in the CCPU library input list. Source guard inspection and object sections are build-profile observations, not actual guest execution. Prior shard 3 source analysis identifies `Int6` fallback paths and the mismatch between historical `CPU_486` declaration and `SPC486` implementation guards. It does not prove runtime fault priority or whether a 486 profile should be enabled. Do not enable either switch without a coherent original-profile decision, typed declarations, decoder/implementation evidence and guest fault/flag tests.

Replay `python artifacts/mvdm-audit/shard-2/cpu486-guarded-inputs-observer.py` and compare the JSON. Build/object hashes are snapshots; no guest, product, shared ledger or build tree was modified.
