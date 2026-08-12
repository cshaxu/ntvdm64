# T142 S1 post-DEM-entry guest-transfer correlation 001

## Original transfer

After `SVC_DEMENTRYDOSAPP`, original NTDOS `$Exec` in `msproc.asm` performs:

1. `mov SS,AX; mov SP,DI`;
2. `push DS; push SI` as a synthetic far-return target;
3. `mov ES,DX; mov DS,DX; mov AX,BX`;
4. `retf` into the application entry.

Its contract is DS:SI = entry, AX:DI = initial stack, DX = PDB, BX = initial
AX.

## T130 correspondence

| Original input | T130 `50:36` observation | Result |
| --- | --- | --- |
| DS:SI entry | `0BC1:0100` | Proves the next `retf` target is physical `0x0BD10`. |
| AX:DI initial stack | `0BC1:FFFE` | Proves the next user stack begins at `0BC1:FFFE`; after two pushes it is `0BC1:FFFA`. The following timer vector records `SS:SP=0BC1:FFFC`, consistent with the first synthetic push already occurring. |
| BX initial AX | `0000` | The app begins with AX = 0. |
| DX PDB | Not present in the BOP observation format | Unproven. The post-transfer DS/ES value is therefore unproven. |
| App image at `0BC1:0100` | Not captured by a T130 RAM/write record | Unproven. `0x0BD10` is outside static NTDOS; its contents must have been produced by the earlier app-load path. |

The later `CS:IP=0BC1:FFFF`, `SS:SP=0BC1:FFF8` fault state is thus a later
state of this source-defined app context, not a direct BOP return frame.

## Disposition

The first missing causal input is the **whole command-image load lifecycle**
(open/read/close, allocation/relocation and PDB setup), not `50:36`. T130
already reaches DEM `50:00`, `50:02`, `50:12` and `50:42` near this transition.
T143 therefore audits those original source contracts and their one coherent
adapter provider/session path. No new BOP endpoint or runtime run is admitted.
