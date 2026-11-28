# S15 P72 N023 USER core declaration migration

## Inputs and baseline

Start HEAD82b5986ea, published runtime82. user.h remains OpenNT in the census:
63 nonerror points/508 lines, blob c328f4cf5b26806494dcecdf8cf572a5e585fdf1.
XP source is commit a2f6d7c93aa4f11efc51147566d5961c59a8ef89,
windows/core/ntuser/inc/user.h, blob842143d337dd84330b25abb2ecc71a73262fb0dc.
Raw objects and the current file are saved under build/t485-s15/n023-user.
XP has3968 lines versus OpenNT2640; these are source lengths, not diff metrics.
The initial audit changed no production layout. Local declaration restoration
is now in progress; no new delivery is claimed.

The normalized three-way merge produces40 conflicts. This is a mechanical
merge count, not40 architecture defects or40 additional mirror diff points.
conflicts.json and upstream.diff retain complete regions and original changes.

## Concrete coupled ABI

| Area | Original XP change | Current direct consumer that must change together |
| --- | --- | --- |
| Object heads | THRDESKHEAD/PROCDESKHEAD separate ownership and desktop prefix; pSelf offsets change | wow_user_client_view_layout.h assertions; kernel/getset.c initialization and native object projections |
| WND | WW moves before links; spwndPrev added; properties become PROPLIST; removed legacy fields | kernel/getset.c and compat/mvdm_softpc_wow_page_domain.c window publishers, words copying, native state bindings |
| CLS | Adds non-versioned atom, WORD task/flags, moves count into COMMON_WNDCLASS; WOW payload is beyond CLS plus class-extra storage | class registration/lookup/query owners, page-domain class publication, class layout assertions; do not pretend the old inline adwWOW field still exists |
| MENU/ITEM | New desktop head and fields; item text and bitmap become separate lpstr/hbmp | page-domain menu allocation/publication and native menu providers; old hTypeData cannot simply be relabelled |
| Shared constants/tables | FNID, handle types, task flags and SERVERINFO/PERUSERSERVERINFO change | actual retained consumers and existing globals providers; already paired XP declarations must not revert |
| Client declarations | XP shared-pointer spellings and activation/DBCS additions | existing multipass declaration consumers and original w32wow64 header dependency |

The page-domain source currently hardcodes WND176, MENU64, ITEM64 and CLS108,
WND.pSelf12, WND.pcls76, WND.WOW144, CLS.flags24 and CLS.WOW36/40. These match
the old layout assertions, but matching two project declarations is not proof
that they match XP. Installed O:/WINNT/SYSTEM32/USER.EXE hashes to
F7323A28EA709FF05EA0F57268A4BC69400627A7912599FC4BEB1EA3F67FCD06,
the already approved SP3 binary recorded by H033. This check is identity
evidence, not proof of a specific executed guest field read or a new bug.

Required implementation order: retain XP declarations and original conditional
structure, reconcile the existing multipass include contexts, update actual
layout providers and their numeric transport contract as one chain, and then
verify real original-guest reads and native ownership. Do not preserve old
layouts merely to satisfy old tests, create a second object registry, or modify
the guest. If a current retained original owner requires a paired migration,
include it as a necessary dependency rather than inventing an old-field shim.

## Required header imported, not delivered

XP user.h:94 includes windows/core/w32inc/w32wow64.h. Import that header's raw
blob cefb8a94429b09456fb35dc7e653a686f41b7d4e byte-for-byte (6475 bytes), with
zero raw diff. Its immediate w32w64a.h dependency is already byte-identical to
XP blob d69705143936b7d2f1b412f91fff960d8af0ab70. prerequisite.json records
the import hash and source. The local user.h now selects this original header.
No build, publication, file completion or directory completion
is counted for this preparation.

## Current guest contract, verified from installed bytes

[Export evidence](n023-user-guest-exports.json) records the hash, NE entry
locations, wrapper bytes, jump targets and import relocations of the installed
SP3 USER.EXE. The read-only extractor and disassembly are under
build/t485-s15/n023-user/guest-exports. No binary was modified or executed.

| Export ordinal | Name | Segment1 wrapper -> thunk | Far-call import |
| --- | --- | --- | --- |
| 32 | GetWindowRect | 2995 -> 5016 | KERNEL.506 |
| 46 | GetParent | 2CDE -> 4FC8 | KERNEL.506 |
| 58 | GetClassName | 1E15 -> 4F39 | KERNEL.506 |
| 157 | GetMenu | 242F -> 4F87 | KERNEL.506 |
| 263 | GetMenuItemCount | 256F -> 4F94 | KERNEL.506 |

Each thunk pushes its argument size and USER ordinal; the ordinal word has
the additive KERNEL.572 module relocation. XP kernel31/kernel.def (blob
0f5891bc96b4f5795464f42317d89fb07fe5199d, lines630/698) identifies these
imports as WOW16CALL and __MOD_USER. Current wow32/wutbl2.h retains the five
matching USER dispatch entries. These five actual exports therefore enter
the ordinary WOW thunk rather than the old NT4 PMODE32 direct object reader.

The historical verify-wow-user-profile.ps1 explicitly pins the different
NT4 hash6746088F... and its map; its47 instruction checks are not current
SP3 ABI evidence. Do not preserve an NT4 host layout solely on that basis,
or change the historical check's expected hash without replacing its proof.
This bounded observation does not establish every export's profile or runtime
coverage. Native host USER objects, the CCPU publication carrier and any other
remaining consumers still need a coherent layout in the completed migration.

Also verified: kernel/class.c's registration PCLS is bound by
wow_class_register_bindings.h to wow_class_lookup_entry, not the raw tagCLS
layout. Its adwWOW and lpfnWorker accesses therefore do not by themselves
require retaining those removed fields in XP tagCLS. Keep metadata ownership
separate from the existing raw identity view; no second class registry.

## Progress and next action

### Whole declaration restoration in the working tree

The subsequent restore-header.py pass now restores all XP user.h text, with
only the existing multipass purpose retained: eight complete declarations
that embed WND are deferred to view/dialog/control-extent passes. No original
declaration body is omitted in this intermediate header. The earlier
restore-layouts.py extraction and its notes below are superseded by this
whole-file candidate. Final exclusion/line audit remains incomplete.

A /Zs declaration-only check of wow_control_extent_bindings.c uses its actual
product flags and does not create a product object or deliver a build. It
identified required original KBDTABLE_MULTI declarations; public/oak/inc/kbd.h
was imported byte-for-byte from XP blob b17161c566b9aeea85e5e6d7aa89f04d4c71f8bc
(SHA256 b17564a6f3aa109c9b2d2e1f300bdee5ecbdb62d014eebd6780d6823a548029d).
The existing multipass boundary now includes it and winuserp.h; that private
boundary gains original MNS_LAST from XP winuserp.h:985. The WOW product and
control-fixture build roots gain the original w32inc include directory.

The next check reaches concrete remaining dependencies: COLOR_MAX, CURSINFO,
JIF, the native-monitor snapshot alias, and old numeric layout assertions.
Most importantly, paired wowuserp.h still retains the NT4 WW union with
adwWOW/vpfnWndProc/vpfnDlgProc. Original XP WW instead contains state/state2,
ExStyle/style, hModule, hMod16 and fnid. getset.c's added layout providers
still read the former union; this is a real shared-layout dependency, not
just a stale assertion. Reconcile the original WOW window metadata owner
and these providers in this same batch. Do not add removed NT4 fields to
XP WND or declare the header migration complete. No release or guest changed.

Further declaration reconciliation restores COLOR_MAX and JIF from XP's
private USER header and the embedded CURSINFO data contract from
windows/core/w32inc/w32gdip.h:48-59 in the existing winuserp.h boundary.
This removes those actual /Zs errors without removing any XP cursor types.
The former global PMONITOR=MONITORINFO* typedef is removed: XP's private
monitor type remains original; only control-client/precomp.h maps the local
monitor spelling to the already-existing native snapshot provider. The
actual consumers are combo.c and dialog creation/repositioning; no new
monitor owner or copied monitor implementation is introduced.

The WW chain is now bounded more precisely. Migrated mvdm-host WOW callers
use XP fields (for example wmdisp32.c reads hModule and ExStyle), while
walias.c:GetIClass currently calls wow_user_window_fnid instead of reading
XP WW.fnid. The obsolete three-DWORD prefix is retained by autonomous
window-borrow/private-access/creation owners and getset.c's provider functions.
Current wuwind.c calls the XP pfnCsCreateWindowEx signature without that
prefix. This does not alone prove every legacy private-word path unused:
cleanup and dispatch projections still expose adwWOW. Trace and reconcile
those exact remaining consumers before removing the prefix, rather than
allocating another permanent sidecar or retaining two WW layouts.

The working tree now restores the complete original XP WW declaration in
wowuserp.h. DispatchClientMessage's existing call-local view carries the
retained struct _WW pointer directly, and its caller no longer obtains a
three-DWORD prefix pointer for that callback. The public/private window-long
boundary keeps GWL_WOWWORDS but sends the obsolete NT4 GWL_WOWDWORD1..3
indices through the existing native API; no selected mirror caller uses
those three indices (the remaining references are autonomous adapters and
old fixtures). This is still a coupled work-in-progress, not a tested release.

Cleanup publication still uses its old prefix until the page-domain and
getset.c capture/restore provider are changed together. The exchange helpers,
legacy creation-prefix handling and corresponding fixtures must then be
removed or updated; do not leave dead APIs solely for old tests. The original
XP WW has been restored rather than adding a second WW layout or a sidecar.

The current working header restores exact XP declaration bodies for HEAD,
THROBJHEAD through PROCDESKHEAD, CALLPROCDATA, COMMON_WNDCLASS, CLSMENUNAME,
CLS, PROP/PROPLIST, WND, ITEM, MENULIST and MENU. Original declaration line
anchors and source ranges are recorded in build/t485-s15/n023-user/layout-units.json.
Existing multipass selectors remain while the rest of the header is reconciled.
This is a partial file migration, not an ABI-ready product candidate: forward
declarations, remaining old declarations/constants, actual publishers and
layout assertions still require the same coupled batch. Do not build or
publish this intermediate file or count it as a completed XP mirror.

The extraction helper was corrected before proceeding: it initially matched
forward declarations instead of bodies. It now requires an opening brace
immediately after the tag and has been rerun from the original before.h
snapshot. The resulting nine recorded declaration groups have the intended
XP ranges (CALLPROCDATA1796, CLS1880, WND1972, MENU2954); no bad candidate was
built or published. Final header-wide #line and diff accounting remain due.

### Relation publication preparation

The original XP WND declares spwndPrev between spwndNext and spwndParent
(user.h:1980). Six existing page-domain operations assumed that the public
next/parent/child/owner relation tuple occupied four adjacent DWORDs: incoming
edge validation, publication, retirement clearing, retained-edge detection,
observation and borrowed-relation lookup. Changing numeric offsets alone
would therefore still publish or read the wrong parent/child/owner fields.

The current local preparation replaces that implicit adjacency with one
four-entry constant offset table and uses it in all six operations. It keeps
the existing lookup, pin, validation, commit and retirement order and existing
memory writer. It adds no object registry, callback or lifecycle state.
Current numeric layout values remain unchanged until the complete XP layout
and actual immutable guest consumer contract are reconciled. This change is
in compat, so it adds no mirror diff. Source review and git diff --check pass;
the complete coupled batch has not yet been built, tested or published.

Delivered census remains1331/790/801/530/7984/65473, XP1310/OpenNT21;
N0232/4, S15 10/14, overall58/159. The uncommitted prerequisite adds one local
byte-identical XP mirror, not a delivered census row. Complete this file and
its coupled changes before the batch's cached full build, dual publication,
targeted tests, commit and push. Last Full checkpoint remains P69; three
completed files since then. The active T scope remains N004-G097.

### XP layout measurement and paired state-provider update

The x86 MSVC declaration-only check now accepts the complete retained USER
layout in wow_control_extent_bindings.c (zero compiler errors). A build-owned
native sizeof/offsetof probe reports WND160, MENU88, ITEM76, CLS92,
DESKTOPINFO108, PROCOBJHEAD12; WND state20, next44, previous48, parent52,
child56, owner60, class100 and menu120. Inputs and results are retained in
build/t485-s15/n023-user/layout-values.c, layout-values.cmd and layout-values.txt.
The checked-in assertions now use these XP shapes, remove nonexistent
WND/CLS inline adwWOW assertions and check the original WW24-byte shape.
The obsolete Win4 CWINHOOKS override is removed; original user.h supplies it.
This is a declaration/layout check, not a successful product build or runtime
acceptance. Existing declaration warnings remain to reconcile.

The current getset.c provider update preserves spwndPrev when promoting an
existing WND and copies XP WW state/state2, style/ExStyle, hModule, hMod16 and
fnid rather than nonexistent NT4 fields. Creation compatibility uses the
original XP createw.c nested 3.1/4.0/5.0 predicates, without the removed
WND.dwExpWinVer field. The source-shaped private version predicates come from
fixed XP windows/published/winuser.w:466-478 in the existing winuserp boundary.
No new object owner, registry or guest mutation is introduced.

The WW snapshot/publisher, obsolete three-word exchange APIs and corresponding
fixtures are still coupled work, not completed delivery. The final mirror
delta must include getset.c as a direct dependency; no candidate source-line
count is reported as original-relative mirror reduction. Runtime82 and the
last delivered census remain unchanged. No Full or commit is claimed here.
### XP WW snapshot and publication chain

The production snapshot now names XP state/state2, ExStyle/style, module,
hMod16 and fnid. getset.c captures/restores those fields; cleanup borrows the
same window-owned WW and captures it synchronously for publication. WND
serialization uses the measured XP state20..fnid42 range, with original
WND160 and next/parent/child/owner/geometry offsets. Public module refresh and
batched graph refresh write the same original x86 handle field. No new owner,
queue, registry or state machine was added.

The old three-slot read/exchange operations have no remaining production
callers after the XP GWL_WOWWORDS transition. Their implementations, provider
slot, declarations and two NTVDM exports are removed without renumbering other
exports. XP registration and dialog creation callers now use their existing
no-prefix argument shape; the artificial trailing NULL/prefix-copy path is
removed. Fixture code still exercising the retired three slots must migrate
with the final batch, preserving its lifetime/rollback coverage rather than
retaining obsolete production APIs solely for tests.

The page-domain declaration-only check passes with its actual production
flags (/MT /W4 /we4013, cdecl, src include). An initial check accidentally
reused the WOW DLL stdcall flags and failed with calling-convention conflicts;
it is not product evidence. Corrected check produces no errors or warnings.
This proves syntax only: MENU/CLS publication and remaining fixed-profile
consumers still need paired migration; no product build/publication or tests
are claimed. The delivered census and completed-directory counts are unchanged.
### MENU/CLS publication and provider composition

The same x86 layout probe now also records CLS atomNV6/fnid8/task20/flags22,
style48/procedure52/cbclsExtra56/cbwndExtra60/module64 and WC size8. The
production layout assertions cover these fields and WORD task/flag widths.
MENU publication uses size88, ITEM stride76, self16, task8, capacity28,
count32 and items52. ITEM's four query fields retain their original offsets.
The desktop allocation uses the measured XP DESKTOPINFO108 size.

The existing sparse class query projection keeps cbclsExtra zero; its two
WOW values now occupy the original WC suffix at CLS92, followed by the owned
class-name bytes at100. Allocation bounds, zeroing, name writing and the
retained observation reader move together. Flags and task use WORD stores,
so neither overwrites the adjacent XP fields. Full native class extra data
continues to belong to the existing class owner; this carrier does not claim
to publish it or implement arbitrary class-extra access.

A real getset.c declaration compile exposed duplicate CURSINFO: the earlier
local declaration coexisted with the already-imported w32gdip.h. The duplicate
has been removed and the existing original header is reused. Both getset and
the complete layout declaration check now pass. Seven changed production
compat units (window binding, object publication, creation, borrowing, private
access, registration and dispatch) also pass /Zs using their actual Ninja
flags plus the newly required original w32inc include directory. Logs and
commands live in build/t485-s15/n023-user. No production object or published
package was generated by these declaration checks.

Still open in this coupled batch: migrate the old-slot/layout fixtures,
reconcile other USER header consumers/compile exclusions, finalize original
#line and mirror accounting, then perform the complete delivery. These syntax
results do not substitute for product build, targeted runtime tests or Full.
### Creation fixture contract migration

The active native-creation and dialog-creator-lifetime fixtures now use the XP
creation signatures without the removed three-slot input. The native fixture
keeps nested callbacks, rejected creation, counted-string/version semantics,
terminal cleanup and retained generation checks. Its explicit WW test marker
uses hMod16 rather than an invented adwWOW slot; counted-dialog pre-callback
validation is selected by the fixture's existing lpCreateParams input, not by
production special handling. Version assertions inspect XP compatibility bits
and the existing captured creation facts rather than removed dwExpWinVer.
Sampled-generation checks publish explicit XP WW snapshots and still assert
old retained storage differs from the new current generation.

Both active fixtures pass declaration compilation using their current Ninja
flags. This exposed an actual include-order collision between B's PQ alias
and original USER's opaque queue pointer. The layout boundary now isolates
PQ alongside its existing PTHREADINFO separation; no queue implementation or
runtime state was added. The original activation-context opaque pointer is
also supplied at the private declaration boundary. The task-window lifecycle
source receives the same no-prefix signature change, but has no matching
current wow32 build target and is not claimed tested.

The historical original_subclass fixture is still tied to an NT4-only hash
extractor, with no current build/test entry found. It is not evidence for XP
subclass behavior. Do not preserve obsolete production fields to compile it;
its retirement/replacement must be reconciled with the remaining test audit.
The large page-domain fixture still needs its old-slot/layout checks migrated.
No final product build, publication or runtime verification occurred in this
step; delivered counts and census remain unchanged.
### Page-domain fixture migration, first groups

The retained-class/retained-WND case now publishes an explicit XP WW snapshot,
compares all fields before and after logical retirement, verifies the class
cannot retire until both borrow owners release, and verifies failed reads
leave the previous snapshot unchanged after physical release. The obsolete
three-slot mutation/index assertions were removed because that production API
no longer exists; lifetime, duplicate-release and class-pinning assertions
remain.

The observed-enrollment case preserves one alias across nested scopes,
creator WW adoption, duplicate-attachment rejection and terminal retirement.
Its marker uses the actual XP hMod16 field. It no longer calls the deleted
three-slot mutation helper. The main class publication case checks XP's
adjacent WORD atom/task/flag fields, zero class-extra count, two WC suffix
values and owned name address. Main window-graph offsets and menu projection
stride/offset checks were updated against the independently measured original
XP layouts. Test source is still incomplete: 22 old read/exchange call sites
remain across other page-domain scenarios. No fixture compile or runtime pass
is claimed for this file; finish the file before final product verification.

### Page-domain fixture migration, remaining ownership cases

The 22 obsolete read/exchange call sites recorded above are now removed.
Observed-to-managed promotion, native class ownership, failed reclamation,
managed/native HWND slot reuse and observed retirement use the existing XP
WW snapshot API. Retained generations compare the complete snapshot; fresh
generations retain their own full HWND and hMod16 marker. Duplicate adoption,
failed publication, host-storage disposal, class pins and final-release
rejection assertions remain. No old three-slot API was restored to production.

The subclass callback checks the explicit procedure argument, not the removed
WW.vpfnWndProc slot or GWL_WOWDWORD2. WND rectangle/class/menu relations and
CLS atom/fnid/task/WC/name projections in these cases now follow the measured
XP layout. The refresh preservation test uses original WND cbwndExtra,
lpfnWndProc, dwUserData and pActCtx fields; the removed NT4 expected-version
field is not recreated. Original-layout assertions remain independent of the
production numeric offset constants.

Declaration-only check uses the current Ninja lifecycle_window-borrow flags
and /Zs, with the paired w32inc include. Final owned CMD PID14876 started
2026-11-28T08:48:10+01:00 and completed with compiler exit0. Input fixture SHA256
455fcfdc049ea05cfc07038590c7fc90c038dc36fc60a3ad779ef25fc13394fb;
command/log/exit are under build/t485-s15/n023-user/page-fixture-check.*.
There are101 warnings, including USER/private macro and parameter differences;
this is not a warning-clean, linked or runtime result. The first detached
diagnostic attempt had no completion receipt and is retained as interrupted;
subsequent runs kept their supervising shell alive and captured compiler exit.

Still required: audit remaining fixture layouts and header-consumer warnings,
complete original-line/diff review, build and publish the final batch, then
run its affected acceptance gates. No new product package, completed-file
count, mirror census or delivery is claimed by this fixture work.

### XP declaration and system-colour boundary follow-up

The fixture's remaining WND/menu offsets and diagnostic CLS field reads have
been updated to XP. The desktop fnid assertion checks the WORD at42 and its
neighbours; the class-name projection uses84/100, and desktop interval checks
use160-byte WND and100-byte CLS-plus-WC storage. These are projection tests,
not a claim that the installed USER16 export executes that pointer chain.

WNDPROC_PWND now uses the original XP LRESULT/WPARAM/LPARAM declaration in
both compatibility declaration owners. The fixture's modal dialog declaration
uses the production UINT parameter. CSF constants retain the same values with
the XP token spellings; input attachment does not redefine an already supplied
TIF_16BIT. These remove the new declaration conflicts without mirror edits.
The subsequent window-borrow /Zs check (owned PID24040) returns0. Remaining
SDK/private redeclarations and the pre-existing DispatchClientMessage adapter
view declaration are not claimed resolved or runtime-verified.

The colour warning exposed a real coupled boundary: XP's selected USER range
ends at COLOR_MENUBAR, while the alias allocator previously reserved only
through COLOR_INFOBK. Shared WOW_GDI_SYSTEM_COLOR_END now supplies the XP
WINVER-dependent range to both private-header bindings and alias allocation.
This preserves the one alias table and its original shifted-index semantics;
no extra identity or lifecycle is introduced. No mirror diff is added.

The existing verify-wow-gdi-alias.ps1 runner was executed with modern pwsh,
owned PID24220, starting2026-11-28T08:53:52+01:00. Its x86 CMD build and runtime
passed with exit0 and WOW_GDI_ALIAS errors=0. Evidence is under
build/t485-s15/n023-user/gdi-xp-range. Stable identity, retirement, slot reuse,
real brush conversion and exhaustion remain tested against the enlarged
reserved range. This is an affected native diagnostic, not final-package
acceptance. Runtime82 and all delivered census/progress counts remain unchanged.

### First complete-product builds and paired coordinate prerequisite

The first cached x64/x86 build (owned PID23688) failed with link1120:
private-exports.def still named the two removed three-slot APIs. Those stale
export declarations are removed; original ordinal gaps remain unchanged.
The second build (PID2876) exposed MAXUSHORT hidden inside the IS_PTR fallback
and the EDIT IME macro falling through to the restored kernel SERVERINFO.
The existing precomp now independently provides MAXUSHORT and explicitly
binds IME availability to its existing GetSystemMetrics provider. No new
state or mirror-body workaround was added. First/second logs are retained.

The third build (PID27128) exposed four real XP prototype dependencies in
the still-OpenNT rtl/wow.c: _ClientToScreen, _GetClientRect, _GetWindowRect and
_ScreenToClient. XP returns VOID, and its bodies also preserve desktop-window
and right-to-left coordinate semantics. The candidate copies the complete
fixed-XP bodies at893-909,921-952,964-981,992-1008 instead of merely changing
return types. Their original XP lines and the return to the surrounding
OpenNT diagnostic lines are recorded in build/t485-s15/n023-user/
wow-coordinate-import.json; the fixed original file is wow-rtl-xp.c there.
All reached production callers ignore the former Boolean result. The shared
native system-metric binding also supplies SYSMETRTL. Test declarations and
the button fixture double use VOID; the original-view fixture adds desktop
identity, RTL round-trip and both desktop/ordinary rectangle assertions.

This is a paired four-function prerequisite, not full rtl/wow.c migration.
The whole-file census still compares it to pinned OpenNT:1/1404 becomes1/1434.
The XP-original replacement is covered by standing owner approval. user.h is
63/508 ->18/65, wowuserp.h5/68 ->3/9, kernel/getset.c1/1224 ->1/1234. Both
new original headers are byte-identical. Local full-population candidate is
1333files/792byte-identical/803nonerror-identical/530different/7937points/
65011lines, delta+2/+2/+2/0/-47/-462;1313XP/20OpenNT. Raw newline/EOF effects
remain included. No candidate values have replaced the delivered census.
Fourth cached build started as owned PID23504; final verification and delivery
remain pending. The failed attempts are evidence, not passing acceptance.

Fourth cached build failed (exit2): clglobal lacked the XP keyboard/private-menu declaration prerequisites; class query's local WC pointer typedef conflicted with XP PCLS; the compiled-out assertion left an unused parameter under the selected warning policy. The existing multipass include is now supplied to clglobal by its unit build binding; the class-query macro uses an explicitly named WC pointer view; UserAssert uses unevaluated sizeof. No mirror body or runtime state added. Fifth cached build PID9708 is in progress; no publication or final acceptance claimed.

Fifth build PID9708 completed exit2. Earlier class-query and unused-parameter errors are gone. Remaining failures include class registration/removal header prerequisites, clglobal PQ/PTHREADINFO name collisions, original draw.c ClientFrame VOID versus XP BOOL, and cleanup/taskman/queue/exitwin NT4 adapter types colliding with newly complete XP declarations. These require a unified declaration-owner review before the next build, not deleting valid XP declarations or claiming an ABI pass. No build is currently running. runtime82 remains the published package; candidate census remains uncommitted.

Paired declaration-owner resolution after fifth build:
- XP opaque PTDB/PPROCESSINFO/PWOWTHREADINFO spellings are scoped to client
  layout names, alongside the existing PTHREADINFO/PQ separation. The native
  scheduler continues to use its actual owner views; no cross-layout cast.
- Class/global units include the existing complete client-view prerequisite
  boundary before their direct original-header inclusion.
- Private USER kernel declaration prerequisites have their own inclusion guard,
  so a previous ordinary USER inclusion cannot suppress them for later cleanup.
  Legacy HQ and physical-device pointers and key-array constants retain the
  original OpenNT declaration contract, without new runtime objects.
- XP moves CURSOR/ACON from userk.h to user.h. The paired old definitions are
  removed from still-OpenNT userk.h, preserving its #line1834 continuation.
  Fixed XP userk.h:3335-3338 proves the original absence at that location.
  This is a prerequisite move, not full userk.h migration.
- ClientFrame uses the complete XP body at rtl/draw.c:1332-1406, source blob
  16f15ca95b18b7efd7d05dd89e75f252bde85885: BOOL, six parameters, border scaling
  and actual PolyPatBlt result. No existing production call site was found.
  The compiled retained function matches the new XP declaration; whole draw.c
  remains OpenNT-baselined until its directory migration.
Candidate census is now1333/792/803/530/7937/64991;
delta+2/+2/+2/0/-47/-482. No delivery claimed.

Sixth cached build (PID5012) failed only on clglobal.c re-including the
unguarded original wowuserp.h after its forced client-view prerequisite.
The class, scheduler, cleanup and drawing units now compile. clglobal.c's
existing adapted include block now selects wow_shared_user_abi.h and
wow_user_client_view_layout.h directly, removing its forced-include workaround.
This keeps one inclusion owner and the original callback calling convention.
Seventh cached build is owned PID5148. The focused drawing fixture now checks
XP ClientFrame border dimensions, propagated GDI failure and failed mapping
without a drawing side effect. These assertions are not yet run.

Seventh build PID5148 failed on the analogous PPCLS collision: the new XP CLS declares its pointer-to-pointer type, while class removal uses the native class-entry list. Its original-view spelling is now scoped with the other pointer identities. No layout cast, new storage or mirror change. Eighth cached build starts after this fix.

Eighth build PID24416 found that legacy userk prototypes still require original PPCLS. The broad PPCLS scoping experiment is withdrawn; original PPCLS remains declared. Only class removal's existing native-list binding uses a local named pointer alias and PPCLS macro, consistent with its other scoped original-name bindings. No new representation or mirror diff. Ninth build follows.

Ninth cached build PID20076 reached remaining RTL/access units. Fixed chartran/random declaration prerequisites through the existing multipass owner, removed obsolete provider.words admission check (capture/restore still required), included the existing window creation interface to preserve WINAPI, and restored the OpenNT ARRAY_SIZE macro in the caller precomp. The two C4013 warnings were real missing declarations, not suppressed. Matching retained fixture include paths are updated. No additional mirror edits; tenth cached build follows.

Tenth cached full build PID21172 completed exit0. Final WOW32 link succeeds.
Delivery PID9768 stages runtime83 from runtime82 and current compiled images;
dual publication succeeded and all26 destination/image hashes match the new
manifest. Publication precedes targeted testing as required. Controls cohort
is running; lifecycle and real guest gates follow. This is published, not yet
accepted or committed. The modal fixture's fixed x86 DLG expectation is36,
matching the complete XP declaration without SYSMODALWINDOWS (nine four-byte
slots); its behavioral lifetime assertions remain intact.

Control cohort compiled and ran40 cases:38passed, button resources and conversion failed. Conversion fixture retained NT4 literal FNID0x2b2; use original FNID_CALLWINDOWPROC (XP0x2b3). Actual button provider incorrectly treated new XP popup-menu bitmap indices as button entries. Limit the existing16-row button table, render/init loops and bitmap height through OBI_3STATE_CDI; keep original full index space. No new rendering path. Product rebuild/publication required; prior38pass evidence retained, affected tests rerun against final code.

Eleventh cached full build PID7568 passed. Delivery PID19832 publishes runtime84 with the repaired button-only range; dual26hash pass. Original XP menu bitmap declarations remain intact and the test additionally checks their unowned geometry remains zero. Product code changed, so the affected control cohort runs against final source, followed by unexecuted lifecycle/guest gates. The census remains1333/792/803/530/7937/64991, delta+2/+2/+2/0/-47/-482.

Final targeted retries preserve control3's 39 passing cases. The only failed
button-lifetime test traversed XP's added non-button bitmap indices; limiting
its pixel-by-pixel exercise to OBI_3STATE_CDI passed (button_lifetime_failures=0).
Class-client and class-query passed. Class-lifetime's old adwWOW field and
three NT4 negative indices were obsolete: the callback identity now compares
the actual WW pointer; the index checks prove ERROR_INVALID_INDEX and unchanged
WW rather than resurrecting the removed storage. Its lifetime, replacement,
rollback and 32 native class-removal failures all pass.
The page-domain fixture also retained an erroneous menu-array offset80 and
four old offset56 reads. All five now use XP MENU.rgItems52, independently
asserted by the compiled original layout. Initial runtime failures are retained
as native-creation-first-runtime.txt and native-creation-second-runtime.txt.
These are fixture-only corrections; runtime84 remains unchanged. Remaining
native/guest verification is still pending; no passing delivery is claimed.

Additional real ABI corrections, not test waivers:
- Observed-to-enrolled WW.hMod16 was overwritten by the older resource WND
  during refresh. Refresh now publishes the WW-owned module16 into that WND.
  The real CCPU nested alias/retirement assertions now pass; mirror size unchanged.
- OpenNT cleanup class slot7 is Dialog, but XP control ICLS_COMBOLISTBOX is7.
  Copying the cleanup table verbatim created a dialog instead of ComboLBox.
  A single explicit ABI index map now projects existing cleanup atoms into the
  existing per-invocation XP control/dialog snapshot. No registry or native
  class registration is added; original OpenNT cleanup stays unchanged.
  Actual diagnostic evidence changed child FNID02a4 to02a6, restored alpha./folder
  selection and reset, and removed the related lifetime failures.
- The remaining EDIT style assertion compared GetWindowLong's filtered public
  ExStyle with the provider's GetWindowInfo result (native ANSI creator bit0800).
  It now compares the same complete WINDOWINFO contract used by the provider.
Diagnostic FNID/text prints were removed after confirming the cause. The two
production corrections still require final cached build and new publication;
runtime84 is the previous published candidate, not proof of these corrections.

runtime85 final controls40/40 and all eight native/CCPU cases passed. Genuine
Win16 controls failed at phase0 with error0x11 (BM_GETCHECK), despite observer
exit0. Its report has76bytes rather than342; this is a real failed acceptance.
The registration aiClassWow table retained NT4 FNID ordering: XP inserted
MESSAGEWND and SWITCH ahead of BUTTON, so the real Win16 message thunk chose
ComboListBox for BUTTON. The fixed XP client/client.c blob
0a965dba8e47e9ba7ec9d42a8d1d50c74988423f:22-54 supplies the original table body;
only the existing static linkage is retained. The table stays in its existing
registration owner, without a second dispatcher. Thirteenth cached product
build PID27896 follows; final publication and affected verification pending.
Existing init-notification assertions enumerate13 registered class identities.
The native/controls gates from runtime85 cover unchanged implementations;
init-notification, native-creation and genuine guest controls will rerun against
the new table/product. No unrelated Full rerun is required by this delta.

## Final delivery and resolved failures

The preceding chronological pending/failure records describe earlier candidates.
Thirteenth cached full product build passed. runtime86 is dual-published with all
13 images and 26 destination hashes checked. Final controls40/40, class-client,
class-query, class-lifetime, init-notification, native-creation, window-borrow, window-reclaim,
graph-snapshot, menu-resource and genuine Win16 controls pass. The latter uses
the final runtime86 manifest and verifies normal guest exit. See the complete
[receipt](n023-user-result.json) and [mirror census](n023-user-census.json).

The final standalone dialog test initially omitted its new production atom
converter. Linking the whole registration object introduced unrelated owner
references; the existing checked source-slice mechanism now compiles the exact
converter and table for that narrow fixture. Real lifecycle and guest tests
still link/use the complete production owner. No copied algorithm or product
workaround was introduced to satisfy that fixture.

The unified XP WW/WND/CLS/MENU layouts replace old adapters in place. No second
registry, queue or lifecycle was added. Typed capture/restore replace obsolete
raw WW slots, including removal of unused private exports. The final ownership
and class-number fixes are described above. No original guest was changed.

Per-file nonerror points/lines before -> after:

| File | Before | After |
| --- | ---: | ---: |
| inc/user.h (OpenNT -> XP) | 63/508 | 18/65 |
| wowuserp.h | 5/68 | 3/9 |
| client/btnctl.c | 15/73 | 15/73 |
| client/clglobal.c | 8/158 | 8/162 |
| kernel/getset.c | 1/1224 | 1/1234 |
| kernel/userk.h | 1/10216 | 1/10185 |
| rtl/draw.c | 1/2077 | 1/2088 |
| rtl/wow.c | 1/1404 | 1/1434 |
| new w32inc/w32wow64.h | absent | 0/0 |
| new public/oak/inc/kbd.h | absent | 0/0 |

Both new headers are raw XP objects. Paired kernel/RTL files retain OpenNT
whole-file baselines; their necessary XP provider/declaration slices do not
silently migrate their directories. Original conditional exclusions and #line
continuations remain, and raw line-ending/EOF effects remain in the census.
Global non-guest mirror:1333files,792byte-identical,803nonerror-identical,
530different,7937points,64995lines; delta+2/+2/+2/0/-47/-478 against P71.
Baselines1313XP/20OpenNT. N0233/4; S15 10/14; overall58/159. Six completed
files since P69 Full24/24; this P has targeted acceptance, no S/T closure.
Next file: winerrp.h in the admitted DIR order.
