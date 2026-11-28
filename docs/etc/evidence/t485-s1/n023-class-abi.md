# S15 P71 N023 class registration ABI

Start HEAD618713a0f/runtime81. XP input commit is
 a2f6d7c93aa4f11efc51147566d5961c59a8ef89; ntcftxt.h blob
 e1fad45a3432fb5b1f3dfd59e74ed9163958738b and classc.c blob
 13ee43d5ccdcae28f5c8114b0de1a6c52f26bb60 are verified from raw Git objects.
N023 retains its original four-file inventory; classc is a required prerequisite,
not a newly migrated directory. No guest modification or rebuild.

## Implementation evidence and first attempts

The following records the working-tree audit and initial corrections. Its
pending wording describes those earlier attempts; final delivery follows below.

## Subsequent implementation work, not P69 delivery

After P70, the working-tree migration relocates ntcftxt.h to client/ntcftxt.h
and uses the pinned ClassNameToVersion body from classc.c. The retained original
registration passes original and versioned names through the existing class
publication owner, rather than adding a registry. Its current task version
comes from thread->pClientInfo, already populated by original task initialization;
the old class-context duplicate field and its production assignment are removed.
Module version remains the separate get_version callback.

The initial selected source range accidentally included XP line721, the closing
#endif of the omitted SetClassLong block. Correct the range to begin at722;
the real WOW32 client compilation then passed. This is a source-selection
correction, not a change to XP function bodies. The class-client fixture also
passes with errors=0, version_calls=28, version_domains=5, server_owner=1, cpd=2.
Evidence resides in build/t485-s15/n023-class-abi. An initial Windows PowerShell
run stopped on compiler stderr before producing a fixture result; its retained
error is not classified as a product failure. The pwsh retry compiled and ran
the fixture successfully. Subsequent current-object fixtures also pass:
class-query reports six queries and three publications, including commit
rollback; class-lifetime reports native-failure retention; native-creation
reports97 callbacks, nested/rejected/vetoed paths,13 destroyed windows and an
empty final owner table. These are native/current-object tests, not genuine
Win16 or final-package acceptance.

Outstanding before delivery: complete whole-file omission/dependency review,
activation-context and atom-name boundary coverage,
obsolete extraction-fixture disposition, exact mirror census and diagnostic
mapping, cached full product build, dual publication and final-package related
tests. Runtime81 remains installed. None of this subsection counts as a new
delivered P or as completion of N023.

### Whole-file scope review

The following ranges refer to the pinned XP objects above, not physical lines
in the selected mirror. Unconditional omissions remain visible differences;
they are not hidden behind invented exclusion conditions.

| XP ntcftxt.h range | Disposition and concrete boundary |
| --- | --- |
| 1-24 | Original ANSI selection; original Unicode arm excluded. Preserve includes and selected diagnostic mapping. |
| 25-87 | FindWindow wrappers require NtUserFindWindowEx's kernel search and private FW flags. Native USER32 owns actual windows; this registration unit does not provide the original server search. |
| 88-178 | Retain original class queries, including previously absent GetClassInfoEx; both resolve class versions before capture. |
| 179-581 | Clipboard/key names, GetMessage, keyboard layout/map and PostMessage/PostThreadMessage depend on NtUser entry points, client queue/DBCS state and private message delivery. Existing public/native message providers remain owners; no XP USER syscall client shell is imported. |
| 582-642 | Original string/menu capture. Preserve existing two frees on capture failure; otherwise duplicated menu strings leak when COPYLPTSTRID branches to errorexit after allocation. |
| 643-721 | SetClassLongPtr/SetClassLong require NtUserSetClassLong and server-owned class mutation; original menu ownership transfer is part of that syscall contract. Current native class mutation is not that ABI. |
| 722-898 | Retain complete original RegisterClassExWOW validation, versions, capture and cleanup. Update the existing publication boundary to accept both names and XP flags. |
| 899-1044 | RegisterWindowMessage, properties and asynchronous sends require NtUserRegisterWindowMessage/NtUserRemoveProp/NtUserSetProp or CsSendMessage's private FNID/argument protocol. Public USER32 owns these operations. |
| 1045-1092 | Retain original version-resolving unregister and successful menu-string cleanup. |
| 1093-1296 | VkKeyScan variants and EnumDisplayDevices require NtUserVkKeyScanEx/NtUserEnumDisplayDevices and original keyboard/client capture state. They remain native USER APIs. |
| 1297-1317 | Retain composable EnumDisplaySettings compatibility wrapper, forwarding to public EnumDisplaySettingsEx. |
| 1318-1484 | EnumDisplaySettingsEx performs private NtUserEnumDisplaySettings capture/size handling. Do not replace the modern display provider with an XP syscall ABI. |
| 1485-1502 | Retain composable ChangeDisplaySettings wrapper, forwarding to the public Ex API. |
| 1503-1763 | ChangeDisplaySettingsEx, message filters, temporary captions, real class-name and Alt-Tab queries require private NtUser calls; filter also consumes private CLIENTINFO/DBCS state. Native USER remains provider. |

classc.c retains the original header and complete ClassNameToVersion, lines
344-491. Lines12-343 require private kernel-backed CLS/WND rebasing and object
layouts: afClassDWord/aiClassOffset read fields such as atomNVClassName, spicn,
spcur and spicnSm; _GetClassData calls NtUserCallHwndParam, server/client
procedure mappings and GetCPD; _GetClassLongPtr/GetClassWord access class-extra
storage immediately after a private CLS. The current registered identity
projection is not a mapped kernel allocation with that trailing storage.
Retaining these readers unchanged would read a different object's layout;
public class queries and existing class-word storage remain their providers.
This admission adds no substitute kernel class table.

The obsolete OpenNT extraction script and its standalone class-registration
mock have no active build/test caller (references only in historical T420
evidence). Retire both rather than rewriting a second extracted algorithm;
current-object class-client/query/lifetime/native-creation fixtures remain.
The class-client fixture additionally creates, activates, deactivates and
deletes a real activation-context manifest. Both ANSI and Unicode resolution
produce a versioned name; no-context identity and atom-zero failure are checked.
An initial test declaration missed WINAPI and failed linking against the real
stdcall symbol; correcting only that declaration yields fixture exit0.

Local census before delivery: ntcftxt.h1/2005 to14/1338; new classc.c1/333.
Global1330/790/801/529/7970/65807 to1331/790/801/530/7984/65473;
delta+1/0/0/+1/+14/-334. XP1310/OpenNT21. Two original Unicode exclusions;
classc has none. These numbers include original-byte newline effects and
visible unconditional omissions, not just executable adaptations.

## Final delivery

Cached full build and runtime82 dual13-image26hash publication pass. Related
current-object controls40/40, class-client (including real activation-context
resolution), class-query, class-lifetime and native-creation pass. Genuine Win16
controls run against runtime82 and pass. [Receipt](n023-class-abi-result.json)
and [census](n023-class-abi-census.json) retain final identities and assertions.
No new Full claimed: P69 runtime80 Full24/24 remains the checkpoint; three
completed mirror files since then, counting ntsend and this required pair.
N0232/4; S15 10/14; overall58/159. Next user.h, then winerrp.h.

Per-file raw bytes and newline-normalized metrics: ntcftxt14/1351 both;
classc1/333 both. Excluding two original Unicode bodies yields ntcftxt14/1338;
classc1/333 unchanged. #line restores original selected diagnostic locations.
Unconditional omissions remain counted. Global1331/790/801/530/7984/65473;
delta+1/0/0/+1/+14/-334;1310XP/21OpenNT. Original replacements/required
prerequisite import are covered by standing owner authorization. No S/T closure.
