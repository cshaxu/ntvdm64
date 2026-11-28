# S15 P66 N022 string capture and lookaside

## Fixed originals and whole-file review

XP SP1 a2f6d7c93aa4f11efc51147566d5961c59a8ef89,
Source/XPSP1/NT/windows/core/ntuser/client/rtlinit.c,
blob6ebeb702a753755f12092737569b02ec29f5c908,368 lines.
Previous OpenNT blobcf303b5e08198391a377f7f5eff2061e6fe644a0.
P65/runtime75 is the delivered baseline. No original guest changes.

Both capture functions and all three lookaside functions retain XP bodies.
The original zone macros, comments, control order and unused declarations
remain. Four original DBG branches retain #error exclusions and original
diagnostic line mapping. No new runtime selection or state machine.

XP uses pUserHeap instead of RtlProcessHeap. The existing lookaside binding
already defines pUserHeap as GetProcessHeap; allocation and release stay paired.
RtlCaptureLargeAnsiString now explicitly clears bAnsi, including NULL input.
The existing real capture fixture poisons that flag before capture and checks
the Unicode result, in addition to its TLS/forced-allocation/length cases.
KPWSTR_TO_PWSTR uses the original w32wow64.h identity form for this non-
BUILD_WOW6432 x86 host; no pointer truncation or new conversion algorithm.

The autonomous wow_original_class_capture wrapper moves unchanged from the
mirror to existing F source wow32-dll/source/wow_class_client.c. That file
already includes the exact capture/layout declarations and participates in
the product and lifecycle fixture links. The provider symbol, WINAPI signature,
forced allocation, returned descriptor and caller cleanup remain unchanged.
No B-to-C direct call is introduced; F continues to provide this callback.

Residual header bindings replace the full USER precomp with the existing
finite host headers. ntimage.h is omitted: no function, type or macro in this
complete file consumes an image declaration, and neither the selected mirror
nor installed shared/UM SDK include roots supply that header. This is not a
claim of an XP algorithm defect, nor an admission of the full USER build.

## Metrics and verification

Delivered result:1point/744lines against OpenNT becomes6points/11lines against
XP, delta+5points/-733lines. Original LF exposes the separate binding/#line
differences previously joined by whole-file CRLF differences. The four
original excluded bodies are projected separately; line mapping passes.
Global1330files,789byte-identical,800nonerror-identical,530different,
8107points/65540lines;1305XP/25OpenNT. N02237/39 delivered.
S15 9/14 directories, overall57/159. Six completed files including the earlier
direct dependency since P61 runtime70 Full24/24.
Cached full build, runtime77 dual13-image26hash publication, controls40/40,
real CCPU standard-class lifecycle and genuine Win16 controls pass.
[Receipt](n022-rtlinit-result.json), [census](n022-rtlinit-census.json).


## First verification and coupled ABI correction

The initial runtime76 product link succeeded but emitted C4028/C4029 for
the large capture signature. The control cohort then failed linking its
four-argument caller against XP's three-argument definition. This was an
incomplete ABI migration, not a passed verification. The B declaration and
fixture calls now use the original three-argument signature; no compatibility
overload or modified XP body is added. The actual production class wrapper
uses the unchanged three-argument small capture. The remaining old large
capture macro references are definitions in not-yet-migrated ntsend.h; the
current source search found no macro expansions/callers. That header remains
assigned to its ordered migration, not counted as XP migrated here.

The corrected final package was rebuilt, published and verified as runtime77.
The failed control build remains in build/t485-s15/n022-rtlinit/control.
