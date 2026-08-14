# M0 T199 S3 Whole DEM Provider Disposition

## Package boundary

DEM is one `bx-vdm` host-service package. It consumes common ingress, provider selection, copied CPU state and checked guest-memory actions. It must not call `DemDispatch`, retain SAS/CCPU pointers, inspect Bochs state, or transfer DOS/DEM vocabulary into bx-core or bx-mantle.

The existing DEM plane classifies all callable services into `NAMESPACE`, `FCB`, `GSET`, `RAW_MEDIA`, `MISC`, `ERROR_LOCK`, `V86_BRIDGE` and `ORIGINAL_NOOP`. T199 preserves that source-derived grouping as the package layout; runtime reachability cannot regroup it.

## All-service disposition

| Plane component | Services | T199 disposition | Required prerequisite |
| --- | --- | --- | --- |
| Namespace | `00-06,08-0C,11-13,16-18,1E,22,27,30,34,39,3C,44,47-48` | Capability-rehost candidate; existing read-only transactions are inputs, while missing mutation, FCB and pipe semantics remain deferred. | Immutable namespace/session state; checked path, open/read/write and continuation ABI. |
| FCB | `07,20,2C-31` | Deferred subprovider. | FCB/DTA binary layout and search continuation contract. |
| GSET | `0D-10,14-15,19-1C,25,41,46` | Capability-rehost candidate; immutable drive/DPB/DTA fragments are inputs only. | Immutable drive snapshot, DOS data/DPB layout, date/time and CWD policy. |
| Raw media | `21,29-2A` | Deferred to machine/device composition. | Native PIC/device/DASD ownership and checked I/O completion ABI. |
| Misc | `23,34-3B,3D-3E,45` | Deferred; existing non-debug/system-symbol fragments require package migration. | CLI lifecycle, symbols, console and DOS-app state contracts. |
| Error/lock | `32-33,3F` | Capability-rehost candidate; current registration is only an input. | Fixed registration/result ABI and lock/retry state model. |
| V86 bridge | `1D` | Machine-owned deferred. | DOS kernel address and bx-mantle lifecycle ownership. |
| Original no-op | `1F,24,26,28,2B,40,43` | Preserve `demNotYetImplemented`: CF clear, four-byte continuation, no fabricated capability. | Existing exact no-op plane regression. |
| FASTREAD compatibility exception | `42` | Retain only the current source-derived read-only sequential fast-read contract. Original `demdisp.c` selects `demNotYetImplemented`, so this is an explicit compatibility exception, never an original no-op. | It must migrate behind the DEM facade, retain its read-only/profile/checked-RAM limits and have separate positive/negative regression. |
| Sentinel | `49` | Not callable; decline through dispatcher boundary. | Negative ingress/plane regression. |

The table covers all `00-48` callable values exactly once. Ranges are inclusive; original no-op values are removed from adjacent semantic groups and retained in the dedicated row. `42` is deliberately separated because current composition contradicts the original dispatcher selection.

## Implementation and regression gate

S4 may create one DEM package facade, not new recognizers. It receives an already classified DEM plane record, selects only the listed component subprovider, and preserves pass-through for deferred members. It migrates partial current providers behind the facade without changing their source contracts. The package regression enumerates all 73 callable values plus the sentinel, asserting component ownership, original no-op result, deferred no-mutation behavior, the separately registered `42` compatibility behavior, and existing positive/negative candidate tests. No trace is admitted until that regression passes.
