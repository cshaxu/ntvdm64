# S15 P27 N021 RTL declarations

Status: final package built, dual-published and targeted verification passed.
N02113/17 delivered; next nturtl.h.

## Inputs and scope

Pinned XP SP1 a2f6d7c93aa4f11efc51147566d5961c59a8ef89:
`Source/XPSP1/NT/public/sdk/inc/ntrtl.h`, blob
81d18c586d38412ec8351fdecd31cfe2170e2703. Original raw bytes are in
build/t485-s15/n021-rtl/ntrtl.h.xp; prior product bytes in ntrtl.h.before.
The direct original include ntrtlstringandbuffer.h is imported byte-exact;
its738 original lines are a prerequisite, not independent XP capability work.
No guest source or binary is changed. Runtime28 is the new published baseline.

## Evidence and minimal adaptation

Raw original x86/x64 product-layout compilation rejects duplicate SDK
RTL_USER_PROCESS_PARAMETERS, MESSAGE_RESOURCE_ENTRY/BLOCK/DATA and
OSVERSIONINFOA/W/EXA/EXW definitions, plus RtlSecureZeroMemory's duplicate
body. Keep the SDK providers, omit those whole declaration/body units, and
retain original diagnostic line mappings. x64 SDK RtlCopyMemory/MoveMemory/
FillMemory/ZeroMemory macros expand the original function declarations into
invalid syntax; their four declarations are omitted while the existing SDK
bindings remain. No alternate memory algorithm is added.

The x86 compiler proves RtlFirstEntrySList lacks the NTAPI calling convention
required by the SDK DLL declaration; add that one token and preserve its
original signature. Four matching original rejected branch bodies remain
#error. Three obsolete MIPS/Alpha conditions have no XP counterpart; original
XP C++ linkage wrappers are restored instead of carrying old false exclusions.

SECTION_IMAGE_INFORMATION was already present in B base_classifier.h.
Move its equivalent original XP ntmmapi.h declaration into one shared B header,
used by classifier and NT support; do not duplicate a type or introduce a
runtime provider. SECURITY_STATUS remains the native LONG declaration.

The first full build found B ntrtlp.h's old RtlFindCharInUnicodeString import
redeclaration conflicting with the now-restored original declaration. Make B
include nt.h explicitly and remove obsolete copied declarations, constants and
forward types. Retain its existing RTL_CONSTANT_STRING initializer macro:
second full build proved original environ.c still requires it (C4013/C2099).
No environment algorithm or import provider is changed.

## Measurements and verification state

Byte-sensitive non-error ntrtl.h: OpenNT1point/12400lines -> XP17points/155lines.
LF recovery exposes separate adaptations: supplementary normalized comparison
is OpenNT33points/181lines -> XP17points/155lines. The required census remains
byte-sensitive; these supplementary values never replace it.

Candidate whole-population:1325 files,776byte-exact,787nonerror same,
538different,7766points/138569lines. Delta+1file/+1exact/+1same/0different,
+16points/-12245lines.1258XP/67OpenNT. New prerequisite accounts for one file;
ntrtl.h accounts for one baseline transition. Original-line audit:0 mismatches.
These are the verified final-package census counts.

x86 corrected layout probe and x64 initial corrected-layout probe pass.
Cached full product build and dual13-image/26-hash publication passed.
Both capture fixtures pass3475 assertions, both layouts pass, all42 service
cases pass and actual guest COMMAND/NetAPI reports NETAPI-OK.
[Receipt](n021-rtl-result.json); [census](n021-rtl-census.json).
Logs and intermediate failures remain in build/t485-s15/n021-rtl.
Two mirror files completed since P26 Full24/24; Full is not yet due.

The ntrtlp.h assembly header is consumed only by C1 error.c/environ.c.
Its explicit original nt.h include replaces their existing forced-header
assumption; no B production translation unit gains a C dependency.
