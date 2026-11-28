# S15 P74 N024 class registration, lookup and removal

Delivered runtime88. Start HEAD32ba6b141, runtime87. N024 begins in
DIR order with class.c. Fixed XP SP1 commit
a2f6d7c93aa4f11efc51147566d5961c59a8ef89, original
Source/XPSP1/NT/windows/core/ntuser/kernel/class.c, blob
1eee60a947dfce9c76531988782d5cdd0dbc4082. Previous OpenNT blob
51c95e1f710e8c091e0e361f4d8275d340c40a46. Original guest unchanged.

## Selected source and ABI

Restore the original afClassDWord/aiClassOffset tables, xxxRegisterClassEx,
_UnregisterClass, _InnerGetClassPtr and GetClassPtr. Restore XP LF bytes in
retained regions. Registration now copies WC only for TIF_16BIT, returns the
non-versioned atom and no longer accepts or stores the removed lpfnWorker.
The original pointer-width module comparison and CSF_flags lookup are retained.

Production units are the three class.c selections in
tools/build/wow32/units.cmake. The provider table in
tools/build/wow32/templates/wow32-dll-entry-bridge.c binds them to the existing
compat class owner. The existing native RegisterClassExA provider owns native
allocation and class lifetime; the existing class binding resolves version
names. Its metadata atomClassName and atomNVClassName are two names for one
native public atom, not duplicated kernel-version state. The real original
CLS projection initializes both distinct original fields explicitly.

The private captured WNDCLASSVEREX argument remains the existing native
WNDCLASSEXA capture boundary. Name/version-name handling already happens in
wow_class_client_publish and the original client registration path. No original
versioning algorithm is copied into a second registry. Registration, lookup
and removal still borrow explicit synchronous contexts, not global state.
PWCFromPCLS binds the original WC operation to the existing two-DWORD storage.

Native USER can reject unregistering a class with windows outside enrollment.
Therefore the existing DestroyClass BOOL result remains authoritative: return
FALSE on failure, and publish returned menu pointers only after success via
wow_original_class_unregister_committed. Restoring the kernel void call would
incorrectly report success and release caller-owned menus. The class-lifetime
fixture exercises this real native ownership boundary.

## Unselected original kernel ownership

ClassAlloc/ClassFree require desktop heap/pool ownership; InternalRegisterClassEx
requires native kernel class allocation, atom versioning, cursor/icon ownership
and desktop locks. ValidateAndLockCursor requires kernel handle validation.
These are provided by the already selected native USER registration boundary,
not by rehosting a second Win32k server inside WOW32.

_GetWOWClass, ReferenceClass and DereferenceClass operate kernel desktop clones;
_GetClassInfoEx, _SetClassWord, xxxSetClassLongPtr and xxxSetClassData access
kernel-owned CLS data, handle locking and callproc allocation. The existing
native query/set owners and the selected original client algorithm retain
those boundaries. UnlockAndFreeCPDs, DestroyClassBrush, DestroyClass,
SetClassCursor and DestroyProcessesClasses require kernel handle/GDI/process
class ownership. Their omitted unconditional source is counted as mirror diff,
not hidden behind an invented exclusion condition. No new omission is claimed
to be an implemented capability.

The original FE_SB, _WIN64 and DBG conditional bodies retain explicit #error
and original #line anchors. All selected units are x86 retail, without FE_SB;
the x64 kernel class setter is not the x64 host product. These exclusions do
not exclude the surrounding unconditional kernel code from difference counts.

## Verification and accounting

Cached full build, dual publication/hash check, class-client/query/lifetime,
native-creation and genuine Win16 controls are required. The query test adds
the XP non-Win16 WC-zero assertion; existing version-name, CPD, rollback and
unregistration checks remain. All four owner cases and genuine Win16 controls pass on the final source/package.

Local measurement: class.c OpenNT1point/1926lines -> XP13points/1488lines;
delta +12points/-438lines. The former CRLF whole-file block becomes separate
real source regions after restoring original LF; this is not twelve new
behavioral adaptations. The original two tables are restored rather than
omitted just because their local consumers are outside the selected subset.
Global final census1332files,792byte-identical,803nonerror-identical,
529different,7948points/64273lines;1314XP/18OpenNT. Only class.c changes baseline.
See the [delivery receipt](n024-class-result.json) and [census](n024-class-census.json).

## Final delivery

Cached full build passes. runtime88 publishes all13 images to assets/release
and O:/WINNT/SYSTEM32 with26 matching SHA256 checks. Class-client, class-query,
class-lifetime and native-creation pass. Initial native-creation failed three
old non-Win16 WC-copy assertions; the final fixture asserts zero in that branch
and the supplied words in the Win16 branch. The same unchanged product was
retested, with prior passing cases retained. Genuine original Win16 controls
complete all reported phases with zero errors and normal exit.

Source mapping audit finds no mismatched original logical lines across the
restored tables and four selected functions. Existing #error bodies are paired
with their fixed XP originals in the census receipt. No guest change/rebuild.
N024 first file delivered; S15 remains11/14 and overall59/159 directories.
Eight completed files since P69 Full24/24; no new Full or S/T closure.
Next cleanup.c in DIR order. Governance and relative-link checks are required
before commit; final package source remains unchanged by documentation work.
