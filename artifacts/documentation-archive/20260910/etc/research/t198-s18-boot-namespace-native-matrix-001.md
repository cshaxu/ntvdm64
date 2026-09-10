# T198 S18 Boot-Namespace Native Matrix 001

## Scope

This matrix covers every endpoint currently executed by the one existing
`bx_ntvdm_boot_namespace_plane_v1`; it is not a claim about the wider BOP
inventory.

| BOP | Current plane behavior | Required copied guest state | Present evidence | Native status |
| --- | --- | --- | --- | --- |
| `54:0C` | Boot-file pathname write, typed resume | DS:DX target buffer | S17 native `C4 C4 54 0C; HLT` | Native-proven |
| `54:0D` | Same boot-file transaction for AUTOEXEC | DS:DX target buffer | S18 parameterized native `C4 C4 54 0D; HLT` witness | Native-proven |
| `50:12` | Prepare pathname read, then open completion | DS:ESI 128-byte pathname | S18 native `50:12 -> 50:16 -> 50:00 -> 50:02` lifecycle fixture | Native-proven as part of the dependent lifecycle |
| `50:16` | Read open file into guest RAM | Open token plus registers/range | Same native lifecycle fixture | Native-proven as part of the dependent lifecycle |
| `50:00` | Seek current open file | Open token and seek registers | Same native lifecycle fixture | Native-proven as part of the dependent lifecycle |
| `50:02` | Close current open file | Open token | Same native lifecycle fixture | Native-proven as part of the dependent lifecycle |
| `50:1B` | Register DTA variable/PDB/error/SFT locations through a bounded read | DS:AX/DX/CX/SI and DOSWOWDATA SFT offset | S18 native DTA/search fixture | Native-proven |
| `50:09` | Path-search first: gather guest pathname/DTA, then write DTA | Registered DTA plus copied pathname/metadata | S18 native DTA/search fixture | Native-proven |
| `50:0B` | Path-search next: gather DTA then write next DTA | Prior search DTA image and registered DTA | S18 native DTA/search fixture | Native-proven |

## Result

Every endpoint implemented by this plane now has native evidence.  The DEM
open/read/seek/close sequence remains one dependent lifecycle, not four
independent compatibility claims.  The DTA/search fixture is likewise bounded:
it proves the `50:1B -> 50:09 -> 50:0B` transaction and its ordinary guest-RAM
preconditions, not a broad DOS filesystem or boot-continuity result.
