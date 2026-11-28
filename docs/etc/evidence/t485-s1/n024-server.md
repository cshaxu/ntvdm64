# S15 P80 N024 server.c migration

Verified runtime96 delivery, starting HEAD0e490c6269a24eeaf9648e9ae7eb9d63129d0022/runtime95.
N0246/9; S15 11/14; overall59/159. Four completed files since P75 Full24/24.
Current server.c is OpenNT1point/3017nonerror diff lines. Whole census is
1332files/792byte-identical/803nonerror-identical/529different/8012points/63536lines.

## Inputs and scope

Fixed XP SP1 commit a2f6d7c93aa4f11efc51147566d5961c59a8ef89,
Source/XPSP1/NT/windows/core/ntuser/kernel/server.c,
blob508bcc9ca7b813e1a8235d98ecf5ae09ccf37eff.
Registered OpenNT blob39a127ae87ceee283e6d5f11c091765a21742677.
Read original Git bytes; do not rebuild or alter guest files.

Production selects InitWindowMsgTable and InitMessageTables plus their original
message arrays. The existing B provider performs once-publication; original-layout
storage/reset stays with its C owner. The product entry bridge binds initialize,
merge and reset together. Native control admission, default processing and WOW
default-message copy consume this shared result. Preserve exception propagation,
partial-allocation cleanup and retry; do not add another initialization owner.

The existing default-message fixture fails each of twelve allocations, verifies
zero surviving allocations and empty state, rejects shell-message registration
failure, then checks successful retry and independently specified membership.
Controls40 and real Win16 controls exercise the corresponding consumers.

Inspect the complete XP table and initialization units, retain harmless original
declarations and data, and enumerate unavailable kernel startup ownership instead
of inventing exclusion branches. Complete the file before cached full build,
dual publication/hash checks, affected targeted tests, commit and push.

## Verified changes and retained boundaries

XP adds dialog/system-key and input-language messages, menu window-management
messages, scrollbar information/mouse tracking, static UI-state messages,
listbox style/middle-button/information messages and EDIT IME/style messages.
The unconditional original IME table adds one allocation, so the existing
failure/retry fixture now checks thirteen allocation points and explicit IME
membership/nonmembership. Original tables are retained, including unused property
names, OLE format names, control names and the original wrapper macro.

XP InitWindowMsgTable removes its explicit zeroing. This is not permission to
use dirty buffers: userk.h:2607 SharedAlloc delegates to Win32HeapAlloc;
kernel/heap.c:268 UserCreateHeap sets HEAP_NO_SERIALIZE | HEAP_ZERO_MEMORY.
The existing native SharedAlloc binding therefore gains HEAP_ZERO_MEMORY while
retaining its already-owned allocation-exception/rollback contract. The fixture
checks that flag rather than assuming current heap contents are zero.

INIT alloc_text pragmas and prefetch.h belong to the unselected kernel loader,
not the ordinary DLL. Original ALLOC_DATA_PRAGMA, LAME_BUTTON and #if0 excluded
bodies use #error; no invented conditional hides unconditional kernel omissions.
Original C shared-table state/reset and B INIT_ONCE ownership are unchanged.
No original guest change is involved.

P80 found CreateSetupNameArray at server.c:1673, correcting P79's reliance on
the stale original comment naming setup.c. Its FastGetProfileValue dependency,
global allocation ownership and Win32UserInitialize invocation are outside the
selected message-table initialization. It has not been silently called from
InitMessageTables or claimed implemented; the P79 optional empty-list boundary
remains explicit. This is not a new external component dependency.

Local original-line audit checks763 matching nonblank lines with no mismatch.
Unverified server.c accounting: OpenNT1point/3017lines -> XP17points/2555lines;
delta+16points/-462lines. LF recovery unfolds the prior whole-file byte span.
Whole local census1332/792/803/529/8028/63074. Five original conditional bodies
excluded; normalized and byte-sensitive final metrics agree. Build/publication/
runtime results are still pending.

First cached build reached final link and found szUSER32 defined by both
original client/clglobal.c and kernel/server.c. Original modules were separate;
the product links them together. One queue-free, server-only macro binding keeps
the XP constant as wow_kernel_szUSER32 and leaves the existing client symbol
unchanged. The original constant body remains exact. The first failure is saved
in build-first.log/build-first-exit.txt; final-source build is now running.
This adds one mirror line and no point relative to the local candidate.

## Final delivery

[Receipt](n024-server-result.json) and [census](n024-server-census.json).
Final cached full build, dual13-image publication/26hash checks, controls40/40
and genuine Win16 controls pass. The existing allocation-failure fixture now
covers all thirteen allocations and clean retry, with XP zero-allocation flags
and independent message membership checks. No original guest change or rebuild.
Original-line audit:763matching nonblank lines, no mismatch.
Original LF/EOF retained; five supported #error exclusions. Nonerror normalized
and byte-sensitive final metrics agree. Kernel startup/CSR/native heap ownership
is not imported or claimed by the selected message-table migration.

server.c: OpenNT1point/3017lines -> XP17points/2556lines.
Whole census:1332/792/803/
529/8028/63075.
Commit delta0files/0exact/0same/0different/+16points/-461lines.
1320XP/12OpenNT. N0247/9; S15 11/14; overall59/159. Five files since P75 Full.
Next taskman.c; no S/T closure.
