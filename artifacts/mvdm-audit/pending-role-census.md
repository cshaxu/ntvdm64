# Pending source inventory by frozen role

This is a planning census, not a full/partial/excluded classification or a claim of runtime behavior. It joins the 4,684-row frozen union to `combined-initial-status.csv` by exact path at this checkpoint. The frozen `audit_disposition` and product-selection fields are initial inventory hints: current graph discrepancies and original-owner/source decisions must still be resolved. Every pending path remains in scope.

| Frozen role | Pending paths |
| --- | ---: |
| `immutable-guest-or-guest-build-input` | 2613 |
| `declaration-or-build-carrier` | 856 |
| `selected-product-source` | 293 |
| `retained-build-artifact` | 224 |
| `unselected-host-source-requires-s2-disposition` | 110 |
| `tooling-or-build-carrier` | 108 |
| `retained-historical-binary-or-build-artifact` | 69 |
| `unmirrored-tool-build-input` | 33 |
| `unselected-host-noncode-requires-s2-disposition` | 25 |
| `unselected-host-module-definition-requires-s2-disposition` | 17 |

| Frozen product selection | Pending paths |
| --- | ---: |
| `not-product-selected` | 4055 |
| `ntvdm-product` | 234 |
| `wow32-product` | 46 |
| `vdmredir-product` | 13 |

The 2,613 immutable-guest-or-guest-build-input rows require explicit distinction between deployed guest inputs, source/build provenance, and unused resource/tooling; guest immutability is not evidence that any capability works. The 293 selected-product-source rows require actual owner, dispatch, lifecycle and behavior reconciliation. Build carriers, historical binary artifacts and unselected host-source rows require a role/closure decision rather than automatic exclusion. Counts overlap across the two tables and must not be added.

The separate 42,963 external audit rows are not included here; they remain subject to relevance and owner-boundary review. Mirror-different files already classified partial still require semantic proof and recovery work. Current selection must be rechecked against actual graphs and maps because earlier frozen claims have known discrepancies.
