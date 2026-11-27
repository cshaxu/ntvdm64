# S15 P28 N021 user-mode RTL declarations

Status: final cached build, dual publication and targeted verification passed.

## Original and ownership

Pinned XP SP1 a2f6d7c93aa4f11efc51147566d5961c59a8ef89,
Source/XPSP1/NT/public/sdk/inc/nturtl.h, blob
978808f498bcfc4924079ec737192ef740c79a43. Raw original, prior product and
registered OpenNT objects are retained in build/t485-s15/n021-user-rtl.
Restore the complete original declaration framework, C++ linkage, warnings,
32/64-bit critical-section carriers, heap/debug/thread-pool, stream and
activation-context contracts. Unused harmless original declarations remain.
No original guest or RTL implementation is changed.

## Necessary residuals

Raw x86 and x64 native-service-layout object compilations both fail C2011
against SDK types. Eight complete duplicate units are omitted, with original
#line anchors: RTL_CRITICAL_SECTION_DEBUG, RTL_CRITICAL_SECTION,
HEAP_INFORMATION_CLASS, ACTIVATION_CONTEXT_INFO_CLASS,
ACTIVATION_CONTEXT_QUERY_INDEX, ASSEMBLY_FILE_DETAILED_INFORMATION,
ACTIVATION_CONTEXT_ASSEMBLY_DETAILED_INFORMATION and
ACTIVATION_CONTEXT_DETAILED_INFORMATION. SDK supplies these types; no copied
replacement layout is introduced. Original aliases, constants and API
prototypes surrounding each duplicate remain unchanged.

The new RtlPushFrame/RtlPopFrame/RtlGetFrame declarations require only
PTEB_ACTIVE_FRAME. B's existing native declaration carrier adds an opaque
pointer with the original pebteb.h structure tag. No TEB layout, state,
callback or runtime implementation is imported or rewritten. Consumers do
not dereference it. Fixed source identity is recorded in the final receipt.

The temporary first extraction incorrectly treated a form-feed as a physical
newline; both failed compiler logs are retained as final32-r1/final64-r1.
Extraction now uses physical LF positions. Both corrected probes pass and
matched original nonempty lines have zero diagnostic mapping mismatches.
Existing thread-pool APC typedef warning also existed in the old header;
it is not a new adaptation or an executed thread-pool implementation.

## Measurements

Required byte-sensitive nonerror comparison: OpenNT1point/2180lines ->
XP8points/99lines; delta+7points/-2081lines. No #error exclusion exists in
this file. Original LF/EOF are restored and eight #line anchors remain.
Supplementary normalized comparison is OpenNT3points/32lines -> XP8points/
99lines: XP adds declarations colliding with SDK types; their necessity is
proven by both raw original compilation logs. Do not interpret the LF-based
line reduction as removal of those real ABI boundaries.

Final global census:1325 files,776byte-exact,787nonerror same,
538different,7773points/136488lines. Delta0files/0exact/0same/0different,
+7points/-2081lines.1259XP/66OpenNT. One file changes baseline; none added.
Runtime29:13 images dual-published,26 hashes verified; both capture fixtures
pass3475 assertions, both layouts pass,42 service cases pass and actual guest
COMMAND/NetAPI returns NETAPI-OK. No original guest change.
[Receipt](n021-user-rtl-result.json); [census](n021-user-rtl-census.json).
N02114/17; next ptypes32.h. Overall56/159 and S15 8/14 remain. Three files
completed since P26 Full24/24; Full is not yet due.
