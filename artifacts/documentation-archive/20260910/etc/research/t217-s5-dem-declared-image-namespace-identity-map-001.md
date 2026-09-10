# T217 S5 — DEM declared-image namespace identity package map

## Question

Why does the installed whole DEM provider return DOS `file not found` for the
second SYSINIT Open despite a declared immutable `COMMAND.COM` image? This
record maps the whole identity package before a behavior change.

## Original Contract

OpenNT supplies rooted `\\COMMAND.COM` in `sysinit2.asm`. SYSINIT obtains a
one-based boot drive through `SVC_DEMGETBOOTDRIVE`, converts it to DOS's
zero-based default drive through INT 21h `set_default_drive`, and opens that
rooted name. The expected identity is therefore root `COMMAND.COM` on the
selected boot drive, not a process-directory path.

`demGetBootDrive` in `demgset.c` returns a one-based drive number (C: is 3)
and defaults to C: if its registry-derived answer is absent or unsuitable.
`demSetDefaultDrive` receives the drive and DOS current directory and calls
the host current-directory API. `file.asm` routes DOS Open to `demfile.c`.

## Current Identity Chain

| Handoff | Current source / ABI | Result |
| --- | --- | --- |
| Profile declaration | `guest_command_placement={ path: "\\COMMAND.COM", drive_index: 2 }` | C: root is explicit. |
| Immutable namespace | `readonly_namespace_v1_initialize` stores command bytes at that path and drive | Declared image owns C:`\\COMMAND.COM`. |
| Boot-drive BOP | `dem_boot_drive_service_v2_dispatch` returns `drive + 1` | Index 2 becomes required AL=3. |
| Guest default drive | SYSINIT decrements AL and invokes INT 21h set-default-drive | Guest selects C: before normal Open. |
| Session context | `capture_command_host_context(..., 2)` and `seed_command` set DEM default drive | Adapter resolution selects C:. |
| Path canonicalization | `dem_path_v1_resolve` removes rooted separator and uppercases 8.3 components | Expected relative identity is `COMMAND.COM`. |
| Declared match | `match_startup_path` compares drive/path and returns immutable token/size | Expected carry-clear result. |
| Failure fallback | A non-match calls direct local-file backend | Its Win32 error 2 matches S4 AX=2/CF=1. |

## Direct Evidence And Remaining Boundary

S4 directly proves that the second normal `50:12` route was accepted and
returned `AX=2`, carry set. This excludes a selector miss, missing BOP,
machine fault, and pre-Open guest EXEC failure. Profile declaration, session
default drive, boot-drive result shape, image bytes, rooted-path contract, and
canonical matching rule are individually aligned.

The S4 observer deliberately did not copy the guest pathname. It cannot prove
which identity reached the provider or distinguish an unexpected guest
pathname/drive, a namespace attachment/lifetime mismatch, or a path match
defect exposed only by the copied request. No evidence supports changing DOS
EXEC, adding a special Open service, or changing Bochs.

## Recovery-Ladder Disposition

| Rung | Disposition |
| --- | --- |
| Original OpenNT source | Normative owner/contract exist, but guest DOS and DEM require historical real-mode/CCPU/SAS composition and cannot directly link into the x64 process. |
| Smallest seam | Preserve the whole DEM namespace provider and its checked pathname gather. Add only a package-scoped, default-off identity result record; then repair a mapped seam if indicated. |
| External intrusion | Rejected: the second Open reaches the source-derived provider and produces a valid DOS result. |
| Newly authored behavior | Rejected: OpenNT drive/path/Open/error contracts and a current whole provider already exist. |

## Next Package Action

The next S may extend the **existing DEM namespace transaction** with a
default-off fixture-only classification record. It must not introduce a new
BOP handler or retain/export arbitrary guest memory. It may classify the
already copied bounded pathname as declared match, other admitted root,
invalid, or fallback, plus namespace attachment generation. Only then may a
source-backed repair proceed, with Open/seek/read/close and native composition
regression.
