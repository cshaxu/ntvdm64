# T97 S4 Pathname Provider Integration 001

## Scope and result

The first-profile, source-derived pathname provider is integrated through the
existing DEM plane gate for exactly these two identities:

| BOP | OpenNT entry | admitted action |
| --- | --- | --- |
| `50:09` | `demFindFirst` | copied `DS:DX` pathname, registered DTA far pointer and current PDB; finite profile snapshot → DTA transaction |
| `50:0B` | `demFindNext` | copied DTA and current PDB; saved continuation → DTA transaction or source-shaped no-more |

`50:0A/0C` FCB forms remain unconnected.  Directories below the admitted drive
root, unlisted drives, and every non-profile entry return no provider result;
they never enter a host namespace.

## Boundary

The v4 BYOB snapshot supplies the four possible entries and their declared DOS
metadata.  `bx_ntvdm_search_plan_v1_first_entries` applies existing DOS 8.3
matching, attribute filtering, private continuation ownership, and DTA result
construction to that snapshot.  It contains no root handle or directory query.

The former ambient-host `path_first` and FCB-first interfaces are explicitly
unavailable in the runtime closure.  The current SDK cannot build their old
`FILE_ID_BOTH_DIR_INFORMATION` fixture, and repairing it would violate the
T97 profile boundary.  This is an intentional disposition, not a fallback.

## Verification

Successful focused tests:

```text
profile-search-snapshot-test: finite BYOB projection verified
bx_ntvdm-search-transaction-v1-test.exe: exit 0
bx_ntvdm_dem_path_search_service_v1_test.exe: exit 0
```

The last test verifies real-mode `C4 C4 50 09` preparation reads exactly 128
pathname bytes + 4 DTA-pointer bytes + 2 PDB bytes; it then verifies a matching
entry's DTA transaction, `C4 C4 50 0B` continuation, the no-more AX `0x12`
transaction, and rejection of FCB `50:0A`.

The full `bx-ntvdm-adapter-runtime-test` CMake target compiles and links with
the new provider closure.  Its pre-existing execution fixture still expects
the T97 S2-withdrawn `50:12/00/16/02` readonly-file lifecycle and therefore
fails its own obsolete assertions.  It is not evidence against this provider
and was not re-enabled to make that test pass.

## Explicit non-goals

No Bochs changes; no host directory scan, stat, time query, or handle; no FCB;
no mutation; no DOS filesystem implementation; and no direct BOP recognizer.
