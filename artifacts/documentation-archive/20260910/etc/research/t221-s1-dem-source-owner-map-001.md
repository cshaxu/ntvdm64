# T221 S1 P2: DEM original source-owner map

## Question

Which original OpenNT DEM translation unit owns each of the 73 dispatcher
entries, so that ABI/failure recovery can be planned by actual source module
rather than by one trace-discovered selector at a time?

## Inputs and procedure

The input is the 73-row P1 route audit.  Run:

```powershell
powershell.exe -ExecutionPolicy Bypass -File `
  tools/governance/Export-T221S1DemSourceOwnerMap.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -OutputPath O:\repos.hobby\ntdos64\docs\etc\research\t221-s1-dem-source-owner-map-001.json
```

The exporter scans the checked-in `src/opennt/base/mvdm/dos/dem/*.c` source
tree for every original handler name. It rejects any missing or ambiguous C
implementation candidate and retains the P1 current bound-route record beside
the original source location.  It is a static source map, not a composition
or execution test.

The script explicitly uses `PSCustomObject` rows and normalizes the Windows
PowerShell top-level JSON-array behavior discovered during the initial run.
Those checks ensure that a grouped source-module report cannot silently turn
73 handlers into one anonymous bucket.

## Observation

The generated [source-owner map](t221-s1-dem-source-owner-map-001.json)
contains all 73 services, each with exactly one original C implementation:

| Original implementation module | Services |
| --- | ---: |
| `demgset.c` | 13 |
| `demmisc.c` | 13 |
| `demfcb.c` | 8 |
| `demhndl.c` | 8 |
| `demfile.c` | 7 |
| `demdisp.c` | 9 |
| `demsrch.c` | 5 |
| `demdir.c` | 4 |
| `demdasd.c` | 2 |
| `demerror.c` | 2 |
| `demioctl.c` | 1 |
| `demlock.c` | 1 |

This source partition differs materially from the current bx-vdm route
partition. For example, `demhndl.c` owns direct seek/close/time and pipe
operations, while current execution enters a six-service token partition and
several other paths through other fallback categories. That difference is
evidence that a present route is not proof of original-provider closure.

## Interpretation and follow-up

The next ledger pass must investigate these twelve source modules in owner
groups. For each group it must record: direct composition feasibility; every
CCPU/SAS, VDD, private-NTVDM or normal Win32 dependency; copied ABI and
failure/order contract; the earliest usable source-recovery rung; mutation
class/profile authority; and the disposition of the current bx-vdm shim.

No provider is accepted, removed, or executed by this mapping. It only fixes
the work decomposition required to recover DEM coherently.
