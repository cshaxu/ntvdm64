# Shard 2: C-VID generated-header batch

Seventeen pending headers match both original trees. The observer/JSON capture each file hash and scan all **VALID** records returned by `ninja -t deps` for the current worker, resolving every relative dependency against the worker build directory. No matching exact paths were found in those records. This is a captured dependency-log observation, not proof of absence from stale/unsupported entries, another configuration, generated code or all possible include paths.

| Entries | File contents and bounded disposition |
| --- | --- |
| `bound_c.h`, `c2cpug_c.h`, `conv_c.h`, `dec_c.h`, `gdinit_c.h`, `inc_c.h`, `light_c.h`, `nanoa_c.h`, `pm_c.h`, `simple_c.h`, `stub_c.h` | Each contains only its three-line include guard. Candidate exclusion **only for the declaration-less file role**, contingent on coordinator verification that the guard macro itself is not used by another relevant profile; do not infer that the corresponding CPU/video feature is unused or implemented. |
| `cpnote_c.h` | Contains a constraint-change enumeration. Actual consumer/profile not established; keep pending. |
| `hpphst_c.h` | Contains host-register and related constants. Actual consumer/profile not established; keep pending. |
| `nano_c.h` | Contains operand-size and segment-related constants. Actual consumer/profile not established; keep pending. |
| `page_c.h` | Contains page-table-related constants. Actual consumer/profile not established; keep pending. |
| `prot_c.h` | Contains protection/alignment-related constants. Actual consumer/profile not established; keep pending. |
| `vglob_c.h` | Defines video function-count constants. Actual consumer/profile not established; keep pending. |

The selected C-VID video providers and previously bound generated functions are covered independently by `../../cvidc-generated-role-review.md`; this header-content audit does not establish mode selection, plane/latch operations, reset, callback ownership or guest rendering. For the six nonempty files, identify actual macro/include expansion or a supported profile before classification. For the eleven guards, first inspect cross-file use of guard macros and any alternate generated dependency path. No guest, product, installation, shared ledger or build tree was modified.

Replay `python artifacts/mvdm-audit/shard-2/cvidc-header-batch-observer.py` with `PYTHONDONTWRITEBYTECODE=1` and compare the JSON. Dependency-log and graph observations are snapshots.
