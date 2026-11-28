# S15 P58 N022 ListBox interaction - delivered

## Scope and source

Start HEAD111b0a43a/runtime66, N02228/39. Restore lboxctl2.c from fixed XP
SP1 a2f6d7c93aa4f11efc51147566d5961c59a8ef89, blob
a89e42eceed8263fac946ed2b818c42e8c388bd8. Original3808lines, SHA256
fa9ec9f8bdb7ff713e98972b217fd72d26b9c57fb3bfb8cc6f8c4ff614b1f9b7.
The whole XP file supplies drawing, focus, keyboard/mouse selection, scrolling,
accessibility events and pointer-sized item data. No original guest changes.

## Explicit native boundary

Owner approves omission of the XP1327-1339 same-position mouse filter.
Its bLastRITWasKeyboard input is session-wide kernel RIT state, set by original
kernel/ntinput.c2633,5854 and kernel/hidevice.c3399,3564, declared user.h1321.
This process does not own that state. Current message classification is not
an equivalent last-input snapshot. Do not invent a constant, duplicate a global
input state machine, or claim restoration of that filter. Normal preexisting
mouse tracking remains; all other source is XP with original diagnostic lines.
Provider blob identities/excerpts: build/t485-s15/n022-lboxctl2/provider-evidence.json.

## Direct dependency and ABI review

usercli.h pairs original LONG_PTR LBSetItemData and xxxNewITopEx declaration;
whole header remains OpenNT until its own migration. Existing B context capture
adds native SPI_GETLISTBOXSMOOTHSCROLLING to PUSIFlags, with original bit8.
Remote-session input uses native GetSystemMetrics(SM_REMOTESESSION).
Original caret now returns VOID with UINT_PTR/DWORD parameters. Update B's
provider to TIMERPROC, native SetTimer wrapper and both production/test
provider declarations together. The existing scoped borrow/task/cleanup and
native timer remain the only lifecycle; no new registry, queue or state machine.
Existing LBEvent, DbcsCombine, drawing and layout owners are reused.

## Mirror accounting

lboxctl2.c OpenNT1/7039 -> XP1/18; original LF and EOF restored. No XP conditional
branches exist to exclude. The old NEVER branch no longer exists in XP.
usercli.h168/589 ->170/592; two original declarations, seventeen exclusions
unchanged. Total1329files/785byte-identical/796nonerror-identical/533different;
8071/77907 ->8073/70889, delta+2points/-7018lines.1296XP/33OpenNT.
Two declaration points add three changed lines, within standing small-increment
approval. #line mapping audit passed. Seven mirrors since P51 Full after delivery.

## Initial build correction

First cached build exit2: existing WEFLAYOUTRTL definition was dialog-only.
Move that exact XP bit definition to the shared control scope; the retained
WND ExStyle already carries WS_EX_LAYOUTRTL. No mirror or new state change.
Retain build-first.log and build-first-exit.txt; rebuild final source.

## Final verification and delivery

runtime67 cached full build, dual13-image26hash publication, controls40/40,
real CCPU window-borrow, genuine SOL Options and Win16 controls including
sorted-list insertion/text pass. [Receipt](n022-lboxctl2-result.json),
[census](n022-lboxctl2-census.json). No original guest rebuild or modification.
N02229/39; overall57/159 and S15 directories9/14 unchanged. Seven completed
mirrors since P51 runtime60 Full24/24; no new Full or S/T closure claimed.
Global1329/785/796/533/8073/70889; delta0/0/0/0/+2/-7018.
Baselines1296XP/33OpenNT. Newline/EOF counted; existing #error/#line retained.
