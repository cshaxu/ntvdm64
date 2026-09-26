# Dtmgr EDIT-style presentation

## Request and boundary

The owner requests an independent white-background blue-text title, table top
on row two, body rows five through 23, vertical scrollbar instead of the body
right edge, horizontal scrollbar on row 24, and unchanged status on row 25.
Four arrows and border junctions must match EDIT. This is dtmgr-owned native
Console presentation; no historical guest or broker behavior is changed.

## Read-only reference audit

Reference root: O:/repos.hobby/ms-dos-archaeology (not a build/runtime input).
Observed reference HEAD: 257a517451b6d4c4e504a8da0ce4c5dcf66991c6; per-file
hashes below identify the actual inspected inputs.
Read edit2/src3/evidence/rectangle_border_9aae.md and the recovered rectangle
routine, then inspect edit2/src2/unpacked/runtime-data-segment.bin at 0x13A6.
The disassembled Win98 EDIT sample has an eight-byte style table: style zero
is C4 C4 B3 B3 DA BF C0 D9; style one is C4 20 B3 B3 DA BF B3 B3.
The order is top, bottom, left, right, top-left, top-right, bottom-left,
bottom-right. Style one omits the bottom rule and retains vertical lower ends.
Data SHA-256: EA166F87E536860296625FD2E01DFF0BC3D95BC8EB763DAD0342729C7A8F9C9F.

The supplied screenshot is DOS EDIT/QBasic, not proof of Win98 implementation
identity. Its matching reference character table is
edit2/src1/45/beef/drv/csd/src/fx_data.asm: arrows 24,25,27,26 (up, down, left,
right), track B0, elevator space. SHA-256:
7ABAAE91053A4017876EF1CFA66C83F415FC0614FB7517F5D514F04A36DE4506.
edit2/src1/45/beef/cw/user/scroll.c paints the track, both arrow endpoints,
then the space elevator with its distinct attribute. Unicode output uses
U+2191/U+2193/U+2190/U+2192 and U+2591, not literal Console control bytes.

Recovery disposition: retain the existing dtmgr Console renderer and selection
owner. Historical DOS 16-bit CW windows/driver drawing cannot directly compose
with the native Console HANDLE and BaseSrv snapshot. No historical code body
or binary is imported; only the reviewed visual contract guides the existing
product-owned renderer. No external-code intrusion or new ABI is needed.
Reference notices and files remain untouched. Confidence is high for the
character tables; matching layout follows the owner's explicit row allocation.

## Verification

Run `powershell -NoProfile -ExecutionPolicy Bypass -File tests/observation/verify-dtmgr-layout.ps1`
from the repository. The script freshly generates the current RPC client and
compiles/links dtmgr and its fixture using VS 2022 MSVC Win32/x86 /MT, /W4,
/we4013 and SDK 10.0.22621.0. Outputs stay in
build/M0-T423/S1/dtmgr-edit-style. This is native presentation evidence, not
CCPU40 guest acceptance.

The first run exposed a fixture setup error: a new buffer inherited a window
larger than 80x25. Shrinking that private window before sizing the buffer fixed
the fixture. The repeat passed: EDIT frame, four arrows, colors, 25 rows,
overflow selection, horizontal extent, shrink, empty and confirmation.

The fixture calls the product renderer and reads character/attribute cells
from an inactive Console buffer. It asserts all four arrows, upper corners
and vertical lower ends, track/thumb attributes, title/status rows, selection
of task 24 in a 19-row viewport, horizontal endpoint after a 200-character
image, viewport reset when the list shrinks, connected/disconnected empty
states and kill-confirmation footer. It does not claim pixel-level font match
or mouse interaction. Similar-issue review covered header/body horizontal
alignment, endpoint exclusion, stale viewport clamping and footer placement.

Candidate dtmgr.exe SHA-256:
95357B2E29D63A7FFE567F7E28BEBEB41FE599566305DD5236BD239AB0A7354F.
Fixture SHA-256:
592E2E11E46B0C5AAD76B25DA4219014C8F126A24D6CBCD1E819915CC03DFF77.
Documentation governance, relative links and diff whitespace checks pass.
Production publication requires the separate DOS17/WOW and coherent six-binary
packet gate; those checks were not run here. No deployment, commit/push P or
lifecycle closure is claimed. Unrelated lifecycle observation changes were
preserved. Follow-up is packet-wide acceptance before publication.

## Owner-directed row-four correction and deployment

The subsequent owner instruction delegates commits to another session and
authorizes this session to compile, test and deploy its dtmgr change to O:/winnt.
The owner corrects the up-arrow position to row four (header separator), leaving
row five available for the initial black thumb. The down arrow stays on row 23;
the track now has 18 cells. Renderer and assertions were updated together,
including the selected first-row case.

The verification script with
`-BuildRoot build/M0-T423/S1/dtmgr-edit-style-arrow-row4` passed fresh x86 /MT
compilation and all layout assertions. Corrected dtmgr SHA-256:
3F7144C35C8B15CC175E5EEB773753A8820F68FA75AB3FBCDBC4F531C71F4985.
Corrected fixture SHA-256:
7DF5FC6672BD17FFB40C45F4382C36C84A982AD576382D426CC4E0F7DA4098C1.

The six installed binaries were backed up under
build/M0-T423/S1/dtmgr-edit-style-arrow-row4/publish-20260925-180209.
An earlier attempt stopped on concurrent package changes. The new backup
preserves that newer baseline. File.Replace was denied with the installed hash
unchanged; ordinary copy subsequently succeeded after checking no dtmgr process
and rechecking baseline hashes. Only dtmgr.exe was replaced; its installed hash
matches the tested artifact and the other five hashes remain unchanged.
publication.json in the backup directory records before/after hashes. No active
owner process was terminated.

This owner-requested side-test deployment supersedes the undeployed status
above. DOS17/WOW packet acceptance and commit/P coordination remain outside
this session; no completed P is claimed.

## Selection-following thumb correction

The owner observed that selecting the second content row did not move the
thumb. The prior renderer used first_visible (viewport position), which stays
zero while selection moves within a page. The vertical thumb now follows the
selected item's visible row, capped before the reserved down-arrow cell.
The horizontal thumb retains its horizontal-offset semantics.

Fresh x86 /MT compilation and the layout fixture pass with
`-BuildRoot build/M0-T423/S1/dtmgr-selection-thumb`. Added assertions exercise
first-to-second-to-third selection with three items, moving back to the second
with 24 items while the viewport remains zero, and removal of the old thumb.
Existing overflow, endpoint, empty, shrink and confirmation checks still pass.

Deployed O:/winnt/dtmgr.exe SHA-256:
426EAE8415869D9E0DA478CBE9AD1E2E69D25145F66B61092837CFC5CAE27281.
All six prior binaries and before/after publication.json are retained under
build/M0-T423/S1/dtmgr-selection-thumb/publish-20260925-182210.
Installed dtmgr matches the tested artifact; the other five hashes are
unchanged. No commit was made; the other session retains that responsibility.
