# T198 S18 Boot-Namespace Native Matrix 001

## Scope

This matrix covers every endpoint currently executed by the one existing
`bx_ntvdm_boot_namespace_plane_v1`; it is not a claim about the wider BOP
inventory.

| BOP | Current plane behavior | Required copied guest state | Present evidence | Native status |
| --- | --- | --- | --- | --- |
| `54:0C` | Boot-file pathname write, typed resume | DS:DX target buffer | S17 native `C4 C4 54 0C; HLT` | Native-proven |
| `54:0D` | Same boot-file transaction for AUTOEXEC | DS:DX target buffer | Plane and composition tests | Source-tested; native fixture can reuse S17 shape with service byte `0D` |
| `50:12` | Prepare pathname read, then open completion | DS:ESI 128-byte pathname | Composition test copies `C:\CONFIG.SYS` and receives token | Source-tested; requires native two-event fixture and preserved RAM |
| `50:16` | Read open file into guest RAM | Open token plus registers/range | Plane test after `50:12` | Source-tested; depends on native completion of prior `50:12` |
| `50:00` | Seek current open file | Open token and seek registers | Composition/plane tests | Source-tested; depends on native open session |
| `50:02` | Close current open file | Open token | Plane test | Source-tested; depends on native open session |
| `50:09` | Path-search first: gather guest pathname/DTA, then write DTA | Registered DTA plus copied pathname/metadata | Plane test | Deferred: current composition session has no DTA registration initializer |
| `50:0B` | Path-search next: gather DTA then write next DTA | Prior search DTA image and registered DTA | Plane test including mismatched action rejection | Deferred: same missing composition-owned DTA registration contract |

## Result

Only `54:0C` has native proof. `54:0D` has the same source path and no new
provider behavior, but must receive its own native fixture before it can be
classified native. DEM open/read/seek/close are one dependent lifecycle, not
four independent routes: their first missing native prerequisite is a bounded
two-event fixture that retains the session and supplies the pathname from
ordinary RAM. Search first/next are deliberately deferred because the current
composition constructor exposes no DTA-registration input; adding one would be
an ABI change and requires a new admitted S.
