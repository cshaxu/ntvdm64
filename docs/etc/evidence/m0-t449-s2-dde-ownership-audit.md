# T449 S2 — DDE post-result ownership audit

## Question

Does the selected original 16-to-32 DDE path retain an owner when the native
`PostMessage` rejects a packet after thunking has completed?

## Source trace

The original `WU32PostMessage` in `wow32/wumsg.c` sets
`WOWDDE_POSTMESSAGE`, invokes `ThunkMsg16`, then calls native `PostMessage`.
`ThunkWMMsg16` in `wmsg16.c` creates a stack-local `DDECONVERSION` for POKE,
ADVISE, DATA and EXECUTE. It registers each original HDDE pair through
`DDETrackhandle`, packs the message parameter, and returns. On successful
delivery, the existing original receiver/ACK paths own those pairs. On a
rejected native post, no receiver or ACK exists, but the original stack-local
conversion record is already gone. That is the proved ownership gap.

`WU32SendMessage` also selects the historical DDE thunking route for AmiPro
compatibility, but it has synchronous delivery semantics and no source witness
that its ownership should be replaced by post-result cancellation. It remains
unchanged. `SetDdeHandleFlag` in the guest only changes the arena flag; it is
reached through the fixed kernel `RET_WOWDDEFREEHANDLE` return and has no
application callback or dispatch path. No product change is justified for
arbitrary application-reentry injection at that internal arena-flag operation.

The reverse native-to-Win16 DDE direction was separately traced through
`W32Win16WndProc`: it calls the selected `WM32DDEAdvise`, `WM32DDEData`,
`WM32DDEPoke`, or `WM32DDEExecute` thunk, then invokes the original `CallBack16`
before it invokes that same thunk's unthunk/free branch.  Its local
`DDECONVERSION` therefore remains live throughout conversion, delivery and
normal return.  These functions do not issue a second native `PostMessage`.
Their original `fThunkDDEmsg` / `FreeDDElParam` branches remain the sole
native-packet owner.  The only other direct DDE `PostMessage` found is an
allocation-failure `WM_DDE_TERMINATE` notification; it owns no converted DDE
packet or alias.  Thus no second post-result ownership gap was found.

The DDE operation selector (`fWhoCalled`) is already the thread-local
DIV-344 field and both `WU32PostMessage` and the legacy AmiPro send route save
and restore it.  It is not a process-global post ownership record; DIV-363
does not change its scope or use it to recover a failed synchronous send.

## Selected minimal repair

`WU32PostMessage` owns a call-local conversion record until its native result.
`wmsg16.c` uses that supplied record only for the existing DDE post branch.
When native `PostMessage` fails, the call releases the packed DDE parameter and
rolls back only the pairs recorded by that call, before the original
private-WOW-message fallback. When it succeeds, it clears the call-local count
without freeing anything, preserving the original receiver/ACK owner.

The existing `DDERollback32` also releases the generated
`DDE_EXECUTE_FREE_H16` alias, because the normal original owner is the ACK
path and a rejected post produces no ACK. It never frees the application's
original EXECUTE handle. There is no new persistent list, ABI, guest change,
alternative scheduler, or synchronous-send behavior.

## Verification plan and boundary

The focused exact-body fixture compares the immutable pre-cohort baseline with
the candidate for original list/mark/free behavior and exercises failed-post
parameter cancellation plus generated EXECUTE-alias retirement using service
doubles. It is not guest or transport proof. The actual package probe must
then exercise supported bidirectional EXECUTE/ACK, DATA/POKE/ADVISE CF_TEXT,
and an isolated rejected DATA post against the candidate package.

Uncovered: DDEML, atoms, all formats, allocation failure and arbitrary
cross-thread interleavings. These stay outside this bounded repair; they are
not silently reclassified as supported behavior.

## Current candidate evidence

The reduced candidate was compiled as x86 `WOW32.DLL` with SHA-256
`E1424CAC174C3388D293142A1A7F159634088BA07E65797408997A3B83C5BC39`.
`verify-wow-dde-alias.ps1` at `build/t449-s2-dde-alias-r14` passed its 101
candidate checks; its immutable baseline retained the expected three historical
failures. The isolated actual-package run at `build/t449-s2-dde-live-r30`
passed: two real Win16 tasks completed normal send/nesting/reply/post/filter
and retirement; both EXECUTE/ACK directions and Win16-to-native
DATA/POKE/ADVISE CF_TEXT completed; the import-injected rejected DATA post
recorded one packed-parameter release and one matching native free while the
sender guest handle remained valid. The package copied the unchanged guest and
all other host images from the preceding sealed candidate; only WOW32 changed.
