# T484 S4 Clipboard preparation

## Change and retained contract

P28 moves both fresh and sealed-input copying to the existing CMD alias owner.
The shared physical stage accepts build-owned NativePeer DLLs, places them in
its resolved binary directory and rejects preexisting destination files. It
also rejects non-DLL, directory, missing and outside-build inputs. This is an
isolated test input facility, not a product publication overwrite interface.

Clipboard uses that owner for GENHOST.DLL and the existing TestFiles option
for CLIPPRB.EXE. Remove the duplicate unused tests/GENHOST.DLL; keep the sealed
build-root copy for provenance. Original LoadLibraryEx32W continues resolving
the native peer from the worker system directory. CLIPRET remains a host-side
retirement witness in the evidence root. Selected package inputs now include
the product manifest, which is hash-verified and retained before execution.

No product/guest code, original media, input validation or runtime assertions
change. No new compiler wrapper, native helper or Clipboard retry is added.

## Verification, including failed attempts

All receipts are under build/t484-s4-clipboard. Owned process12056 runs the
cached full build and package/staging contracts successfully. Its first real
case calls use an incorrect absolute BuildRoot argument and are rejected before
product execution. The corrected caller uses the documented relative root.

Process22680 exposes the selected-stage manifest omission; it is corrected in
the final code. Its last case reaches guest but fails at initial Clipboard
access. Process24776 reuses the historical shared-desktop invocation with an
inherited environment; all cases time out. These are retained failed attempts,
not acceptance. Existing native clipboard-access.exe reports OpenClipboard
error5 at WinSta0/Default, medium integrity, no opener, no job UI restrictions.
After the owner reconnects/unlocks RDP, the same native control succeeds.

Final current-token Userenv process14752,2026-11-24T19:49:55Z to19:52:35Z,
passes all six real shared-Clipboard cases: transfer, callback-return and
owner-task-exit, each with CF_TEXT and CF_DIB. Transfer covers18 checkpoints,
callback return13 and task exit7. The original data/handle/rejection/callback
and native owner-window retirement assertions remain. Its frozen-input case
fails at phase5 with native OpenClipboard error5, despite identical input hashes.
No assertion is waived. After a successful native access control, process6628
retests only that frozen case at19:53:31Z to19:53:43Z: all18 checkpoints pass.

Shared stage tests cover structured/flat peer placement and type/path/collision
rejections, alongside all previous identity/default/subdirectory contracts.
Completed isolated stages are removed by their ownership helper. All11 raw
host images and22 dual-published hashes remain unchanged; reuse that verified
publication. This is targeted acceptance, not a new Full run. Governance,
parser and diff checks gate delivery.

## Progress

Frozen census345/355 disposed,10 pending. Five supplemental preparation chains
remain; they overlap the frozen inventory and are not additive file counts.
S4 and final clean/interpreter-free S5 acceptance remain open.
