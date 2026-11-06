# T478 S38 clipboard formats and caret investigation

Parent f87ea8607; finding48 closes in this local P1 delivery. The original
conversion gap and incomplete rejection ownership repair are corrected;
caret requires no product change. Package818fff8c-efa7-4570-93d6-1f9c820f40ed
passes Full16/16 and is hash-published only to this worktree assets/release.
No O:/winnt write, guest modification or push. Earlier checkpoints below retain
the investigation sequence; the final closure section supersedes pending status.

## Evidence retained rather than repeated

The original wuclip.c selected providers and the T460 text/DIB guest/native
peer evidence remain relevant. The earlier caret summary is insufficient:
T457 S2 explicitly leaves its separate caret desktop-state fixture as an
environment non-pass. Edit/dialog behavior alone is not wucaret dispatch
acceptance. Current wucaret.c invokes existing mapped Create/Hide/ShowCaret
and native Destroy/SetCaretPos boundaries; a real focused guest witness remains
needed, without implementing another caret state owner.

## Bounded exact-function comparison

Read-only pinned inputs are O:/repos.external/OpenNT/base/mvdm/wow32/wuclip.c
and OpenNT-4.5/nt/private/mvdm/wow32/wuclip.c. Both source hashes are
C386AF38376945CB4FD141D878BCA8EF5140B15DCCC9695A0ADCB0941FBB0083; their extracted
WU32SetClipboardData bodies also match exactly. Current whole-source hash is
84F16B83E8D616CB56A7ACDD9F93EB1D0B8EEE38CA212AE613EE11DA7CEC04A4.
Exact input/body hashes are build/t478-s38/source-comparison.json.

Independent wow_clipboard_format_fixture.c compiles each extracted body. It
uses real HGLOBAL allocation but controlled acceptance, conversion and cache
observers; it never opens or changes the user's Clipboard. Its fake metafile
identity does not prove actual GDI conversion or guest/native transfer. Results
are evidence for the tested function's ownership decisions only.

Supervisor13468 completes current/opennt/opennt45 with 12 cases each (six
formats, accepted/rejected). Both originals return the guest handle and write
the cache even when the controlled native transfer rejects it. Current text
correctly returns0, avoids caching and frees its host copy. Current HDROP,
METAFILEPICT, DSPMETAFILEPICT and both OLE descriptor formats return0 but still
cache guest handle55h and retain the unaccepted native allocation. The two
metafile cases additionally create a native-metafile identity without cleanup.
All accepted cases return55h, cache once and retain the transferred copy.
Logs under build/t478-s38/{current,opennt,opennt45}/result.txt retain every row.

This is an inherited original rejection defect with an incomplete existing
project repair (DIV-453), not missing an unconnected original clipboard engine.
The source-defined cache owner can retire an earlier guest handle when storing
the rejected replacement. This fixture observes the bad store call, not actual
later guest freeing; do not overstate that distinction.

Initial test construction failed because `accept` conflicts with winsock's
function and `/TC` treated the trailing library as a C source. The variable was
renamed and `/link` separated linker inputs. Those failed builds did not run
any guest or product scenario; fixtures-b.exit0 is the completed comparison.

## Bounded next change and unresolved checks

Extend the existing rejection ownership correction to these copied formats,
freeing only their unaccepted host copy (and the newly created HMETAFILE when
applicable), without storing/freeing the caller's guest handle. Preserve the
original NULL delayed-render branch: native NULL return alone is not rejection
when the input handle itself is NULL. Verify that path separately instead of
blanket-applying `if (ul)` and silently changing successful delayed rendering.

Original allocation/invalid-input branches (including CopyDropFilesFrom16's
failed-lock result and unchecked metafile GlobalLock) are distinct inherited
limits, not permission for general tolerance. No change has been made to them.
Actual per-format conversion/transfer and caret evidence remain outstanding.
The native OLETHK32.DLL file is present on this host; presence alone does not
prove ConvertObjDescriptor can execute correctly through this guest context.

## Candidate ownership correction

The candidate extends DIV-453 only at the three structured-format branches.
Original normal conversion remains in place. Rejection frees the current
native HGLOBAL, and metafile-pict also deletes its newly converted HMETAFILE.
The prior guest cache is untouched. NULL-input paths preserve the original
cache operation even though native SetClipboardData returns NULL for delayed
rendering. Existing text handling is unchanged. Original diagnostic line
numbers are restored after the changed branches.

Before mutation, original/current extracted bodies, baseline fixture and logs
were preserved under build/t478-s38/before-format-fix. Candidate supervisor
12388 completed successfully: fixture-c.exit0, 24 strict cases with zero
failures in fixture-c-strict.out. These add NULL-input rows to the six-format
accept/reject matrix. NULL rows test local branch preservation, not whether a
real native delayed-render registration succeeded. The fixture observes cache
calls and controlled metafile deletion, not actual later guest reclamation.

A checked-in repeatable entrypoint is
`tests/adapter-mvdm-host-out/wow/verify_clipboard_format_ownership.py`; it requires
a fresh build-root and the installed VS VsDevCmd.bat via --compiler-env.
Its exact source/body/fixture hashes and exit statuses are recorded together.
Supervisor2036 executed this entrypoint successfully: repeatable.exit0 and
repeatable-ownership/receipt.json record 24 passed cases in8.036s, with
source/body/fixture SHA-256 identities and compile/test exit0.

Product build supervisor15308 started at 2026-11-06T04:14:45-08:00, using the
S37 coherent configure/worker/WOW32 command sequence. Its deadline is900s;
logs and exit receipt are build/t478-s38/build*. Both build.exit and
build-supervisor.exit are0. The selected wuclip object and WOW32 DLL rebuilt;
existing declaration/export warnings remain in build.out, not hidden.
Staging supervisor17504 completed the established coherent package helper;
stage.out reports PASS. Its ten-image manifest records package
1efc2b5b-656a-4eae-81a2-43b551545a84 at build/t478-s38/package-final.
This is a staged candidate, not assets/release publication.

Regression sensitivity: supervisor9744 compiled the preserved pre-fix current
function with the new 24-case strict fixture. It correctly exited1 with exactly
five rejected-format failures; baseline-control.exit0 means the negative control
matched that expected failure, not that the old implementation passed.
Candidate and baseline use identical fixture logic. No real Clipboard was used.

Historical runtime reconstruction found that T460's successful final transfer
used the explicitly authorized shared Clipboard path; merely using a private
desktop had failed with access denial. The S38 summary must not describe those
historical tests as isolated private-window-station acceptance. Reuse the exact
successful runtime construction and verify its current isolation/ownership
before a real-format run. Do not infer an unavailable capability from an old
failed token or station attempt.
Actual per-format guest conversion, caret, Full and publication remain pending.

## Real guest caret witness

Supervisor24512 completed caret-r1 through the existing medium-token/private-
desktop shell/hook runner, extended with an independent caret case. The exact
candidate package is1efc2b5b-656a-4eae-81a2-43b551545a84. The runner uses the
existing physical-stage helper, original-logon environment helper and S34
observer, retaining input/runtime hashes and runner token in caret-r1.

`win16_caret_probe.asm` is an independently authored disposable NE, not rebuilt
guest media. Original USER16 ordinals163/165/183/167/166/164 drive create,
set/get position, show/hide and destroy. A test-only CARHOST.DLL is reached by
original generic-thunk LoadLibrary/GetProcAddress/CallProc32W; it only calls
GetGUIThreadInfo/GetClassName on this thread and does not implement caret
behavior. The native observations therefore cannot make missing production
caret calls pass.

All six native observations return1: solid2x12 caret initially hidden and
owned by T478_CARET; shown at(7,9); hidden via NULL owner; gray3x11 replacement
initially hidden; shown via NULL at(4,6); no caret after explicit destruction.
Guest GetCaretPos returns(7,9), and DestroyWindow/UnregisterClass both succeed.
Guest/launcher finish normally; caret-r1/exit.txt is0. This verifies these
original mapped-window and nullable-owner paths without a product change.
It does not assert arbitrary bitmap-caret, cumulative hide-count, global blink
setting or asynchronous task-death coverage. No interactive desktop is used.

## OLE conversion dependency inspection

Read-only PE export inspection of C:/Windows/SysWOW64/olethk32.dll confirms an
x86 image with ConvertObjDescriptor exported (SHA256
FF9F468C63565AD2D9765B509EE7AA9C57144FC78A6A6FBAAF82B8A9D07E29F5).
The full list is build/t478-s38/olethk32-exports.json. Original
W32ConvertObjDescriptor dynamically resolves that exact export. This rules out
a missing-file/missing-export assertion on this host; it does not yet prove
its runtime guest-memory ABI or conversion behavior. Real format evidence
remains the next required step, followed by Full and local publication.

## Real structured-format rejection and OLE root cause

Supervisor22184 serially ran five independent original USER16 request cases
through the established medium-token/private-desktop observer and physical
stage. No case opens, empties or reads the interactive Clipboard. Fresh
`win16_clipreject_probe.asm` uses real guest HGLOBAL data, an original GDI
memory-metafile for the two picture formats, and a52-byte OBJECTDESCRIPTOR
header plus strings for the registered OLE formats. This is test-only input.

HDROP, METAFILEPICT and DSPMETAFILEPICT pass in9.256s,7.272s and6.615s:
original SetClipboardData returns0, guest payload bytes remain intact, caller
GlobalFree succeeds, and original DeleteMetaFile also succeeds where applicable.
The window/class and launcher retire normally. These are real rejection and
guest-ownership results, not proof of native accepted payload conversion.

Object Descriptor and Link Source Descriptor both fail normal completion,
at51.618s and52.013s including observation/cleanup. Their roots are
reject-49153-r1 and reject-49154-r1; neither is a pass. Supervisor8860 then
reuses the existing S37 x64 exception observer on the former case in
ole-debug-r1. It records first- and second-chance C0000005 execute at address0
inside the launched NTVDM tree, with ESI=0 and return OLETHK32+8c90, reached
from OLETHK32+9daf and selected WOW32+1ef67. All observed test processes exit;
the debugger itself is explicitly diagnostic-only.

Static disassembly of the exact inspected system DLL attributes the null call:
RVA8c79 loads a callback from RVA14754; RVA8c8e calls ESI. The surrounding
52-byte header conversion and E_INVALIDARG lock-failure path match original
ConvertObjDesc1632's initial WOWGlobalLock16 call. The exported registration
entry at RVA9de0 copies17 DWORD callbacks to RVA14720, but first calls
NtQueryInformationProcess(GetCurrentProcess(),19,...). If that legacy process
state is absent, or registration already exists, it calls ExitProcess(0).
Import-table inspection identifies those APIs. Thus a blind registration call
is not a usable standalone compatibility binding. No process-state spoof,
system DLL patch, exception masking or product fallback has been introduced.
Static dump retained in olethk32-disasm.txt; its first restricted-parent launch
24204 ended without output, confirmed absent before normal-context34160
produced the dump. That launch failure is not product evidence.

The original alternative is OpenNT/com/ole32/olethunk/olethk32/thoputil.cxx,
SHA2560EE4BA619F5943E6A259A2ED94BE41048817B77EF9950FC41B940C61F7971826.
ConvertObjDescriptor selects existing ConvertObjDesc1632/3216. Their finite
memory/string boundary must be audited before choosing a source recovery;
no complete OLE subsystem or new conversion algorithm is admitted by this
finding. Current S38 has a demonstrated provider integration gap, beyond the
already repaired rejection ownership. This remains open and prevents closure.

## Original conversion closure audit

The six required function bodies match byte-for-byte between pinned OpenNT and
OpenNT-4.5. Per-edition path, whole-file hash, function range and body hash are
retained in [the source inventory](../../../artifacts/mvdm-audit/shard-2/ole-descriptor-s38-source-inventory.json).
The mechanically collected call names also include comment text and are only
a search index; the manually checked boundary below is authoritative.

| Original owner | Required behavior | Outgoing boundary / disposition |
| --- | --- | --- |
| thoputil.cxx: ConvertObjDescriptor,4403-4442 | Select ANSI-to-Unicode or reverse and return the converted handle | Retain both original conversion calls and original failure return; no second format implementation |
| ConvertObjDesc1632,2604-2758 | Copy OBJECTDESCRIPTOR fields; convert both optional strings and preserve original offset/cleanup rules | Existing wcall16 WOWGlobalLock16/Unlock16; original read/string helpers; public GlobalAlloc/Lock/Unlock/Free, AreFileApisANSI and MultiByteToWideChar |
| ConvertObjDesc3216,2778-2917 | Reverse conversion, original guest allocation and cleanup | Existing wcall16 WOWGlobalAllocLock16/Unlock16/UnlockFree16; mapped output view; public IsBadReadPtr/IsBadStringPtrW/lstrlenW/WideCharToMultiByte |
| GetReadPtr16,776-794 | Propagate original descriptor-validation failure into scResult | GetPtr16Description |
| GetStringPtr16,930-996 | Original4096-character ceiling, remaining-segment bound and DBCS-aware terminator scan | GetPtr16Description; g_abLeadTable initialized in original olethk32.cxx from IsDBCSLeadByte |
| GetPtr16Description,566-756 | Original selector access and normal/expand-down length checks | NT ExpLdt, guest OLE TouchPointer16, flat guest memory: these are the unavailable boundaries, not a reason to copy the whole thunk manager |

The original THUNKINFO structure is defined in thop.hxx112-125. Only its
scResult field is consumed by these helpers; this does not authorize silently
redefining a shortened struct as the original ABI. Other fields name stack
records, THOP, VTBLFN, CThkMgr pointer, IIDIDX and IUnknown pointer. The selected
conversion bodies use no C++ objects, methods, COM activation or thunk manager;
normal C compilation through an owned carrier is feasible, subject to preserving
the private declaration shape. Original source stays at its upstream relative
path if admitted, not in the adapter. Source extraction is not yet import/build
admission and no new original source is selected in the product at this point.

Existing CPU40 mvdm_softpc_protected_segment already returns base, decoded limit
and access from original selector_outside_GDT_LDT/read_descriptor_linear.
Existing common/guest-memory acquire/release and WOW callback-frame leases
already bind finite native views to the session. These are candidate adapter
boundaries, not permission to reconstruct an ExpLdt singleton or return an
unbounded flat base. The selected DLL does not currently import protected_segment;
that wiring would need explicit build/ABI and current-image verification.

One original edge must be settled before implementation: non-present segments
invoke guest OLE TouchPointer16, whose source simply reads one byte (Chicago
additionally fixes the block). Recovering/building that guest routine is outside
this host task. The descriptor conversion starts by locking a guest HGLOBAL,
so the ordinary valid payload is resident, but that fact alone does not prove
all malformed cross-selector string-offset paths have the same behavior.
Do not silently remove the original checks, reinterpret memory as native, or
invent a retry/exception suppression. The next implementation decision must
bind this edge to an existing original owner or explicitly delimit the proven
original caller precondition. No new runtime tolerance has been added.

### Existing original boundary found after the closure audit

The non-present edge has an existing source-shaped route: current original
wow32.c::GetPModeVDMPointerAssert tests ExpLdt and invokes
CallBack16(RET_FORCESEGMENTFAULT) to make the selector present. That return ID
is already admitted by wow_user_native_extension.c's kernel-resource callback
boundary; no guest TouchPointer16 import or guest change is needed. This is a
candidate binding to original behavior, not yet runtime verification of the
new conversion provider.

The current WOW32 map also proves existing _GetPModeVDMPointerAssert@4,
_WOWGetVDMPointer@12 and __imp__ExpLdt. NTVDM's retained original export maps
ExpLdt to Ldt, ordinal31. Reusing this already selected original descriptor
surface is smaller than adding protected_segment to the DLL ABI or creating
another descriptor mirror. The earlier proposed new import is unnecessary.

Implementation direction is now concrete: retain the original descriptor
access/length validation and the two conversion bodies; bind the OLE-specific
touch callback to the existing original WOW segment-fault owner, and replace
inline flat-base arithmetic with the existing pointer mapping boundary inside
the synchronous conversion lifetime. Preserve the original private declaration
shape and DBCS initialization. Validate string offsets, original failures,
both conversion directions and reentrant memory-call boundaries before closure.
No adapter may absorb the conversion algorithm or add a new cache/provider
state machine. This resolves the source-selection question; implementation,
focused tests, Full and local delivery are still pending.

## Implemented original conversion and actual product verification

OPENNT-HOST-297 now selects the six audited original functions in
src/non-mvdm/com/ole32/olethunk/olethk32/thoputil.cxx. The ordinary graph compiles
this original filename explicitly as C; it adds no C++ runtime. The original
normal/expand-down selector checks, DBCS string scanning, allocation/conversion
and cleanup remain in that owner. Chicago/DBG bodies remain rejected. Retained
source continuations are anchored to original line numbers.

The finite private declaration facade retains the complete56-byte THUNKINFO
with scResult at offset28 and52-byte OBJECTDESCRIPTOR; compile-time assertions
verify those x86 shapes. Pointer-only C++ types are opaque C forward declarations.
SCODE remains LONG. Existing WOW constant macros replace the two original local
C++ const declarations because C case labels require constant expressions;
values/direction remain0/1. The small binding retains original IsDBCSLeadByte
process initialization and delegates the non-present touch to the existing
original GetPModeVDMPointerAssert owner. Inline flat-base arithmetic is replaced
by the already selected synchronous WOWGetVDMPointer boundary. No native pointer
is added to guest state, and no new descriptor table, clipboard cache, COM/OLE
activation, guest code or kernel-state spoof is introduced.

MVDM-HOST-DIV-885 replaces only W32ConvertObjDescriptor's unavailable system-DLL
lookup/call with that binding. Its obsolete function lookup table is removed.
Original helper declarations that are no longer selected are not new runtime
providers. The shared spooler loader and its other callers are unchanged.

Build29524 failed because SCODE was absent from the finite facade and the two
original local constant names collided with WOW's existing macros. Those are
retained non-passing build results in build-ole-r1; no package was produced.
Build22572 then passes all coherent worker/WOW32 steps in build-ole-r2.exit0.
The graph's map contains actual original ConvertObjDesc1632/3216/Descriptor
bodies and wow_ole_convert_descriptor. Existing unrelated declaration/export
warnings remain visible. A later appended copyright notice changes no compiled
statement, declaration, preceding source line or ABI.

Supervisor14844 stages package818fff8c-efa7-4570-93d6-1f9c820f40ed and repeats
all five real rejection cases. They pass in9.392,7.266,7.477,8.180 and8.423s.
In particular both previously crashing OLE descriptors now return0, preserve
the original guest bytes, allow caller GlobalFree, and retire window/class/task
normally. Registered clipboard IDs are host-assigned49166/49167, not assumed
from the probe's format selector. This fixes the demonstrated crash, but these
negative cases alone would not prove a non-NULL successful conversion.

Supervisor37596 adds that missing positive evidence in ole-roundtrip-r1:
original generic-thunk calls enter a test-only native peer, which invokes the
actual package conversion provider using its exact hash/map-bound RVA.
The runner requires the compiled image SHA to equal the package InputSha256,
records the exact map/image/native-peer hashes, and creates no test-only product
export or code patch. The peer does not implement conversion. Real guest input
is converted to a native Unicode descriptor, then back to a separately allocated
guest ANSI descriptor. Both strings, dimensions, offsets and original76-byte
output allocation convention match. Invalid NULL input in both directions and
an invalid direction returnNULL. The original input stays intact and the guest
frees both input and returned allocation. All four flag groups pass (15), with
normal window/class/launcher retirement. This is direct production-provider
and real guest-memory evidence, not a public Clipboard ownership/notification
round-trip claim. No interactive Clipboard is touched.

Full supervisor38528 starts at2026-11-06T04:51:11-08:00 against this same package,
using the established S37 Full path/helpers and fresh S38 evidence/cache roots.
Full is still pending at this checkpoint. Independent ownership/governance
supervisor35572 uses no product instance and therefore does not overlap another
BaseSrv runtime. Worktree release still remains the delivered S37 package until
all required gates and final review pass.

## Closure and delivery

Full16/16 passes in377985ms against the same final package. Final caret-r2
(supervisor17540) repeats all six native observations, guest position and
window/class retirement on this package. The current exact-function ownership
runner passes24 cases in7.762s; the preserved pre-fix negative control correctly
fails five. Documentation governance passes with the already documented
CURRENT size exception. Publication23092 backs up the prior ten images and
manifest under build/t478-s38/release-before, then verifies all ten worktree
release hashes against the tested package. The [retained receipt](../../../artifacts/mvdm-audit/shard-2/clipboard-caret-s38-results.json)
contains the Full rows, focused results, actual provider/image binding and
published identities. Local P1 includes the complete package and test sources;
no push or external system publication is authorized.

Finding48's concrete gaps are resolved: copied special-format rejection no
longer transfers guest ownership or leaks its native copy; modern OLETHK32's
unavailable kernel-WOW callback dependency is replaced by the required original
conversion closure; existing caret calls have current real guest evidence.
The previous independent DIB/text evidence remains bounded and is not rebranded
as all-format acceptance. New tests do not touch the interactive Clipboard or
claim public accepted-transfer notifications for every format. They combine
exact-function accept/reject decisions, real negative original caller paths,
real positive product-provider/guest-memory conversion and the unchanged public
Clipboard boundary. Original invalid-pointer/allocation failures and untested
caret bitmap/global-blink/task-death cases are not expanded into new behavior.
No known newly demonstrated defect remains open in this S. S39/finding52 follows;
broader inventory reconciliation and remaining T478 findings are still open.
