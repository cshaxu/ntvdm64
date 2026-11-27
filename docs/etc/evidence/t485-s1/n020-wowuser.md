# N020 WOW USER registration - S15 P19

Starting HEAD7244e8ef6. Fixed XP SP1 commit
`a2f6d7c93aa4f11efc51147566d5961c59a8ef89`, original
Source/XPSP1/NT/public/internal/windows/inc/wowuserp.h,
blob5be3126f26e295b939694fed1db0383bc106009a.
Raw inputs and complete byte-diff audit: build/t485-s15/n020-wowuser.
[Census](n020-wowuser-census.json); [residuals](n020-wowuser-residuals.json).

## Entire-file and call-chain review

Restore XP declaration order, WPARAM/LPARAM forms, native-width registration
return, original InitTask prototype, WOW64 include and all unused declarations.
Preserve original pack(1)/pack() and complete selected/unselected original
branches. No guest change, new feature guard, queue or second registration
state. The three residual ABI groups are explicit in the linked record.

Remove obsolete pfnInitDlgCb and pfnWowSetFakeDialogClass input slots and their
unused B accessor macros: current XP W32Init never fills them and the actual
WOWDlgInit implementation uses pfnWOWTellWOWThehDlg. Update the two fixture
NULL initializations, not their assertions. Retain their typedefs only because
not-yet-migrated original USER globals still compile those declarations.

The removed pfnWOWGetIdFromDirectory output has no current WOW caller. Remove
its one clglobal.c assignment while preserving physical line count. Keep the
actual resource operation for its own consumers; no algorithm is deleted.
Change the B UserRegisterWowHandlers return declaration to original ULONG_PTR;
the existing x86 shared-view identity and ownership remain unchanged. The
fixture prototype follows the same ABI. Original W32Init and registered
provider share one XP table; no shadow table or version-index mapping.

PFNWOWCLEANUP retains four parameters because current wuser.c ModuleUnload
passes the NE selector list into the still-OpenNT original USER cleanup. The
XP two-argument task callback and separate module callback require that owner's
paired migration. Do not drop selectors, declare a two-argument function over
a four-argument call, or fabricate an unimplemented module-unload provider.
The new unused XP module slot remains original; it is not a claim of support.

WW retains the selected original USER union. getset.c copies its three WOW
words and style/module fields; user.h computes STATEOFFSET from that layout;
cleanup/client dispatch still use the old procedure marker. Replacing only
this header would violate those actual field/layout contracts. Keep them
here pending paired USER migration rather than moving a duplicate to B.

## Mirror and line evidence

wowuserp.h before: registered OpenNT1point/586lines. After: XP5points/68lines,
including original diagnostic anchors; no #error branches. Original LF
restoration exposes separate actual ABI changes rather than the old full-file
block. Delta+4points/-518lines falls under approved LF-restoration reporting.
No new behavioral adaptation is introduced by these retained declarations.

clglobal.c remains OpenNT:1point/2702lines before and after. Removing its unused
assignment preserves physical rows. Exact raw opcode ranges against its pinned
OpenNT object and every existing excluded body are unchanged, so projected
counts remain valid; its hash is updated, not reused without checking.

Global1318files,767byte-exact,777nonerror same/541different,
7731points/156836lines. Delta0files,0exact,0same,0different,+4points/-518lines.
1241XP/77OpenNT. One baseline transition and no mirror population change.

## Verification

Final cached build, runtime19 dual13-image26hash publication, init-notification
and actual CCPU window-borrow lifecycle fixtures passed. Same-package
[Full24/24](n020-wowuser-full.json) passed. [Receipt](n020-wowuser-result.json).
The ten-file checkpoint resets to zero; N0207/7 delivered. Overall56/159,
S15 8/14. Next N021 SDK. Win95 approved exit1 boundary remains unchanged.
No claim that the retained USER ABI groups are fully XP-restored; their
explicit paired-migration obligations remain in the residual register.
