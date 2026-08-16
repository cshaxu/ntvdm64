# T221 S1 P4: DEM source/ABI/failure recovery ledger

## Question

Can every callable DEM `50:00..48` service now be assigned, without trace-led
gaps, to one source-owner group with a current bound route, original ABI and
failure evidence, source-recovery rung, shared-profile disposition, and
current shim/fallback disposition?

## Procedure

Run:

```powershell
powershell.exe -ExecutionPolicy Bypass -File `
  tools/governance/Export-T221S1DemRecoveryLedger.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -OutputPath O:\repos.hobby\ntdos64\docs\etc\research\t221-s1-dem-recovery-ledger-001.json
```

The exporter starts with P1's static current-route inventory and rejects a
duplicate, unassigned, or non-73-row service set. It then attaches the
source/ABI/failure evidence already gathered by the earlier whole-package
audits, while retaining T219's current route and fallback evidence separately.
The resulting JSON is the exhaustive row-level ledger; this record explains
the non-overlapping owner groups and the limits of the conclusion.

## Complete owner partition

| Source-owner group | Services | Current recovery decision |
| --- | ---: | --- |
| file/handle/FCB/search | 29 | Original CCPU/SAS bodies are not direct x64 objects. Use checked guest copies, opaque session tokens and a source-derived owner rehost; `47/48` remain Redirector-deferred. |
| GSET/drive/CWD/volume | 15 | Retain source-shaped fixed-width capability contracts and move synthetic answers into the shared profile ABI. |
| MISC/lifecycle/console/WOW | 15 | Split at the original subfamily boundary; VDD, engine terminal and WOW semantics remain unavailable/deferred until their complete owners are admitted. |
| error/lock | 3 | Retain checked registration and source-derived retry/unavailable behavior; do not reduce hard-error/lock semantics to generic access denial. |
| DASD/IOCTL | 3 | Keep raw device/VDD/SoftPC mechanics outside ordinary DEM namespace work; only source-shaped metadata/unavailable results exist until a device profile is admitted. |
| original no-op | 7 | Preserve the original `demNotYetImplemented` CF-clear continuation, not a convenient success implementation. |
| FastRead exception | 1 | Its retained compatibility branch is distinct from the historical no-op and must later be retained or deleted as one DEM package decision. |

The totals are 73. Each JSON row names its original handler and dispatcher,
current static route, applicable prior source evidence, profile disposition,
and the T219 shim/fallback that must be retained, migrated, replaced, or
removed only through later whole-package work.

## Evidence interpretation

The ledger reuses, but does not silently upgrade, prior evidence:

- [T202 file/handle/FCB/search map](t202-s3-dem-file-handle-fcb-search-source-abi-map-001.md)
  records the 29 services' register forms, checked guest ranges, opaque-token
  rule, path/DTA/SRCHBUF layouts, source-shaped success/failure distinctions,
  and direct/readonly/overlay/virtual disposition.
- [T199 matrix](t199-s2-dem-source-abi-failure-matrix-001.md), GSET/DPB,
  MISC, error, and DASD/IOCTL records supply the remaining original owner and
  failure evidence.
- [T220 lifecycle and JFT/SFT maps](t220-s6-dem-pdb-termination-admission-map-001.md)
  and [T220 direct-token map](t220-s7-direct-token-jft-sft-representation-map-001.md)
  prohibit both premature `50:3C` token release and adapter-owned JFT/SFT
  reconstruction.

Older records that call a provider "bounded" describe their historical
fixture or limited profile result only. They do not supersede P1/T219's
current route audit or prove direct-host DEM package closure. Conversely, a
current route or a present shim does not prove the original ABI/failure
contract; that distinction is retained per row in the ledger.

## Result and next step

T221 S1 now has exhaustive service coverage and source/ABI/failure provenance.
It still requires one final audit pass over **current workaround disposition**:
for each group, state whether its current code is retained as a profile
backend, migrated into a future provider, replaced by source-derived behavior,
or deleted as stale/unbound. Only then may T221 choose an implementation S
for a complete owner package; no trace decides that ordering.
