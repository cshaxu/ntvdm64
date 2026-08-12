# M0 T96 S9 BOP Profile Regression 001

## Coverage

The immutable inventory reports 16 top-level selectors, 194 service identities
(including the DEM and COMMAND non-callable sentinels), and 349 annotated
SoftPC BIOS table forms. S9 verifies those counts directly from the JSON,
including every expected named top-level selector and every service-family
entry count.

The compiled regression separately sends every callable service through the
actual adapter chain:

| Family | Callable services | Required chain |
| --- | ---: | --- |
| DEM | 73 | ingress → registry → DEM plane |
| COMMAND | 17 | ingress → registry → COMMAND plane |
| XMS | 12 | ingress → registry → XMS/DPMI plane |
| DPMI | 25 | ingress → registry → XMS/DPMI plane |
| Debugger | 16 | ingress → registry → system plane |
| Redirector | 49 | ingress → registry → system plane |
| Total | 192 | no service skips its family plane |

The two remaining service identities are sentinels and intentionally reject.
Top-level selector classification is route-only: no test interprets it as a
provider or a CPU result.

## First CLI profile outcome

The regression proves classification/ownership, not that all deferred source
components execute. Original no-op DEM slots remain the sole admitted
source-derived rehost class. Redirector and DPMI VCD are explicit unavailable;
all other unavailable/deferred forms cannot silently receive a result from the
plane classifier. S8's no-bypass test separately prohibits a legacy endpoint
from becoming an unregistered execution route.

## Verification

`bx_ntvdm_bop_profile_regression_test.c` compiled and ran with strict Clang
warnings. `Test-BopInventoryProfileRegression.ps1` consumed the immutable
inventory and printed `selectors=16 service-identities=194 BIOS-forms=349`.
