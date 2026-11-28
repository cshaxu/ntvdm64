# S15 P70 N023 capture macro prerequisite

## Scope and original identities

Start HEAD58a284096/runtime80. N023 starts with the relocated ntcftxt.h;
that file needs XP ntsend.h. The latter is also directly included by the
already migrated XP client/ntstubs.c, so restore the complete shared prerequisite
before changing the registration provider. This does not complete ntcftxt.h
or its new class-name versioning behavior. No original guest changes.

Source: XP SP1 a2f6d7c93aa4f11efc51147566d5961c59a8ef89,
windows/core/ntuser/inc/ntsend.h, blob520ed2ce9d9b2f46905dc7360e2789fa0a3e87b6,
282lines. Previous OpenNT blob a1fac66e1674a2798734d0f0a00fe0c15141596b.
The final file is byte-identical to the pinned XP object, including original
whitespace, line endings and EOF. No exclusions, adapters or #line additions.

## Complete-file and provider review

XP uses ULONG_PTR return temporaries, LRESULT/WPARAM/LPARAM message carriers,
IS_PTR ordinal tests, the three-argument RtlCaptureLargeAnsiString and the
original pUserHeap/KPVOID_TO_PVOID cleanup. The current x86 product keeps its
native pointer width. The existing wow_user_capture_bindings.h already declares
the three-argument capture; wow_edit_lookaside_bindings.h binds pUserHeap to
GetProcessHeap, the same heap used by capture allocation. No new heap or
ownership path is introduced. Cleanup order is unchanged.

The first cached link failed with undefined IS_PTR and KPVOID_TO_PVOID in the
class-client/query contexts. Add their original x86 definitions to the existing
wow_user_capture_bindings.h, which those two contexts already share. Providers
are XP user.h:467 and windows/core/w32inc/w32wow64.h:92 (not BUILD_WOW6432).
No mirror hook, callback, private layout or runtime code is added. The failed
build and its log remain in build-first.log/build-first-exit.txt. No publication
or tests were attempted with that failed candidate.

The new BEGINCALL_CLASSV macro calls ClassNameToVersion. It remains original
and unexpanded until ntcftxt.h migration; its mere presence is not runtime
validation of Fusion/class versioning. BEGINCALLCONNECT's USERTHREADCONNECT
is likewise not reached by current selected source. Removed limit macros have
no current retained consumer. Retain the complete unused macros and both
Unicode/ANSI branches instead of deleting them to reduce compilation surface.

Production include consumers are client/ntstubs.c and the current inc/ntcftxt.h
assembled by wow32-dll/source/wow_class_client.c. The historical extraction
fixture uses pinned OpenNT macros and is not evidence for this XP change.
Use current product-object class tests and actual guest controls instead.

## Targeted test correction

The final product control cohort passed40/40. Class-client and class-query initially failed
before capture: their fixtures supplied 0x92340020/0x92340056, single-high-bit NT4
markers, to wow_class_client_register_guest. Current XP requires both bits in
WOW_WINDOW_PROC_TAG (0xc0000000); its input validation correctly returns87.
Use the production tag plus the same test address payload in the existing
fixtures. Keep all version-domain, duplicate registration, callback cleanup,
procedure encoding and class lifetime assertions. Product code is unchanged
by this test correction. Preserve the failed stdout/stderr/exit evidence and
retry only class-client plus the previously unexecuted query/lifetime/guest
controls gates. Passing controls are not rerun.

The class-lifetime fixture likewise retained a single-bit callback tag and
hard-coded -28 for GCL_WOWMENUNAME. XP windows/published/winuser.w:1731 and
the current actual provider both use -29. Use the existing interface constant
and production guest tag; retain shared storage, replacement, rollback and
release assertions. Its initial invalid-index failures and native dispatch
fault remain recorded; no production recovery path was added. Class-client
and class-query now pass and are not rerun after this test-only correction.

## Difference accounting

Old raw OpenNT-relative difference:1point/572lines. New XP-relative:0/0.
The old source matches OpenNT after newline normalization:0/0. Compared against
the same XP baseline after normalization, the old file differs26points/68lines;
final0/0. Thus the raw572-line reduction is formatting/baseline accounting,
not removal of572lines of custom behavior. No mirror addition or removal.

Final population:1330files,790byte-identical,801nonerror-identical,
529different,7970points,65807lines; delta0/+1/+1/-1/-1/-572.
Baseline ownership becomes1308XP/22OpenNT.

## Verification and delivery

Final cached full build and runtime81 dual13-image26hash publication pass.
Current-product control cohort40/40, class-client/query/lifetime and genuine
Win16 controls pass. [Receipt](n023-ntsend-result.json) and
[census](n023-ntsend-census.json) retain final identities and assertions.
P69/runtime80 Full24/24 is the last Full checkpoint; one completed mirror
file since it. N0231/4 files delivered, S15 10/14 directories, overall58/159.
Continue the first file's registration ABI with this prerequisite complete.
No original guest change or S/T closure.
