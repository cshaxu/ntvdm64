# M0 T396 S2 — WOW32 formal composition frontier 001

Date: 2026-09-06

## Question

Can the complete selected WOW32 provider be made a formal x86 DLL without
silently replacing a missing historical ABI or private-system service?

## Procedure

`New-T396Wow32ProviderNinja.ps1` now derives the build list directly from
`src/mvdm-host/wow32/sources`, rejects any list other than 77 unique C
bodies, and checks the 104 root headers plus `wow32.def` carrier count before
writing the graph.  It creates a distinct `wow32.dll` target with the
original `.def`, selected resource and all selected translation units.

The first direct x86 compile used the same CPU40/SoftPC and `opennt-abi`
include family as the current formal process graph.  It deliberately compiles
the original `wow32.c`; it does not introduce a replacement `W32Init` or
dispatcher stub.

## Observations

1. The graph generation gate passes: 77 C bodies and 105 selected declaration
   carriers.  `mapembed.c` and `wowtbl.c` compile under that graph, proving
   that selected table composition can be compiled as source rather than
   reconstructed by a generated table.
2. `wow32.c` first reached original `ntcsrdll.h`, which revealed the omitted
   public CSR message declaration carrier.  The exact source text of
   `ntcsrmsg.h` is now present in `opennt-abi`; a whitespace-at-EOL-insensitive
   comparison against selected OpenNT is clean.  This supplies declarations
   only and neither transports CSR messages nor recreates CSRSS.
3. The next unresolved declaration is `PORT_MESSAGE`, supplied historically
   by `ntlpcapi.h`.  Its OpenNT hash differs from the OpenNT-4.5/opennt-src-2
   hash because the latter adds `LPC6432`/64-bit-compatible layouts.  The
   current `ntexapi.h`, `ntcsrdll.h`, and `ntcsrsrv.h` carriers are all
   OpenNT-identical, and the selected WOW provider is x86.  S2 therefore
   selects the OpenNT x86 LPC declaration shape (source SHA-256
   `653097C73182630CE39183526334AE4027B1AEF00E2D28812A0E5E84C6164DB5`),
   not the later layout.  It declares no LPC transport provider.
4. After that declaration frontier, the compiler exposes distinct original
   interfaces, not one generic “WOW missing” category: the inbound USER
   registration callback signatures; `WOWGetVDMPointer` declaration ordering;
   historical `FastPebLock` and `ClientId` fields of the existing
   adapter-owned TLS/PEB carrier; and historical status values.  Each needs
   its own source/call-site/failure-direction disposition.

The original `wowuserp.h` is byte-identical to the local ABI carrier.  The
four callback conversion errors are instead a build-profile error: its
function pointer slots are explicitly `WINAPI`, while historical bodies rely
on the original x86 default `__stdcall` compilation convention.  The T396
formal graph now uses `/Gz`; it changes no source signature.  The missing
`WOWGetVDMPointer` prototype is recorded as `MVDM-HOST-DIV-227`, preserving
the exact later original definition signature rather than accepting obsolete
implicit-int behavior.

The remaining `FastPebLock`/`ClientId` fields occur only in WOW32's original
exception/AeDebug avoidance path.  `ADAPTER-WIN32-046` projects the adapter's
already-reached `RtlAcquirePebLock` critical section and current public Win32
process/thread IDs into its private TLS carrier; it does not read, expose, or
emulate the modern process PEB.

## Complete-cohort compile observation

The complete historical `winuserp.h` cannot be included with the current SDK:
it redefines structures now owned with incompatible layouts.  S2 therefore
admits only a provider-gated, source-derived subset in
`wow32_provider_private.h`: the exact consumed constants and
`WOW_HANDLE_TYPE`, traced to OpenNT `winuserp.h`
`B2838AE608353A14659B85C2BE290034A6BB02DEE9AC078E82C298F2F9B9B513` and
`wownt32.h` `7B3077A7DA4144ABD73494311D85F34D46ECD483D2E14AFD6619693891143CB7`.
It declares no USER entry point.

The exact original `wowgdip.h`, `wingdip.h`, `wowshlp.h`, and `wowcmndg.h`
declaration carriers are now mirrored under `opennt-abi`; their selected
OpenNT hashes are respectively `EE7CD9C9D24C8907540D06545B44ACF92E17645C5E24B5FF87DB8D70DD6835D8`,
`FC35D359BF23F905916980740C56199D3627EB55B9C223091A75A8E4E606DD4D`,
`EA9D93FC5C49166CD408BAF8E64755EE1EBB7D99DB971A0B99FB23232E00E0B5`, and
`CE25C125139070BA30B2A99D670BDAC09FF614C14A08CE5EF0CCE2D99B43A52C`.
Direct x86 compilation now succeeds for `walias.c`, `wstruc.c`, `wcall16.c`,
`wgdi.c`, and `wcommdlg.c`; remaining implicit-declaration warnings are
inventory, not acceptance.  `wmsg16.c` reached two further source-carried
constants, now admitted in the same narrow carrier.

`wole2.c` and `wkman.c` additionally reach the original
`NtOpenThread`/`NtQueryInformationThread` thread-basic-information contract.
The existing `opennt-host` `ntpsapi.h` intentionally contains only the
previously needed `NtAlertThread` subset, so S2 adds the separate
`ntpsapi_wow32.h` source-derived x86 carrier, traced to OpenNT
`ntpsapi.h` SHA-256
`CAFD48B3A9C672C6A18503D4565E7864A284EC029F63AE04FF221C58A2DB5148`.
It preserves the original body identifiers and ABI shape, but neither
implements an NTDLL call nor constructs a TEB.  A formal Ninja invocation
for these two objects was observed waiting without compiler CPU activity or
object output in this workspace and was stopped; it is not treated as
compile evidence.  Direct compiler verification remains required.

That direct verification is now complete for both bodies.  `wole2.c` passes
after the carrier reuses the current SDK `THREADINFOCLASS` and query import,
while supplying only the OpenNT-x86 `THREAD_BASIC_INFORMATION`, `PCLIENT_ID`,
`NtOpenThread`, and `NtCurrentThread` gaps.  `wkman.c` also passes after three
separate source-shaped corrections: its `vdmdbg.h` include temporarily selects
the historical x86 branch before restoring the provider's emulated CPU marker;
the adapter-private TLS carrier gains the one `SubSystemTib` association needed
by selected `RTL_PERTHREAD_CURDIR`; and only the reached `ntrtl` declarations
for multibyte conversion/privilege adjustment are admitted.  The conflicting
whole `ntseapi.h` include is absent.  The `nturtl.h` and `ntrtl.h` source hashes
are respectively `C6B7844E2E1248AC38EE90DD8173F95AB0F7997D7A7CFBAFDA851CC07CC80921`
and `2FC698D4E078ABF7B18722F3E84712AB580BF22CF3362BA3CE9FB9486B1A7E98`.
The focused direct batch also passes `wcall16.c` and `wmsg16.c`; remaining
diagnostics are CRT deprecation/one historical format warning, not implicit
WOW cross-body declarations.

The complete 77-body x86 compilation was then run with the same flags.  The
entry body `wow32.c` compiles successfully (only historical CRT warnings).
This is a full cohort result: all 77 selected C bodies compile, and the
original `wow32.rc` compiles into the provider resource.  It is not inferred
from a subset or from a replacement `W32Init`.

The remaining failures cluster into provider-owned boundaries:

| Cluster | Representative evidence | Required recovery direction |
| --- | --- | --- |
| private USER/GDI ABI | `GWL_WOWWORDS`, `GCL_WOWDWORD*`, `WOW_HANDLE_TYPE`, `COLOR_ENDCOLORS`, private dialog/menu/message constants | select the original declaration carrier and audit every private operation; no public numeric lookalike or invented USER implementation. |
| missing original WOW declaration carriers | `wowgdip.h`, `wowshlp.h`, `wowcmndg.h`, `wow_callback_frame_lease.h` | locate original owner/selected local adapter before changing a source body.  The lease header is a current adapter declaration and must enter only through its actual owner. |
| historical context/security ABI collision | `wkman.c` reaches SDK `vdmctxt.h` followed by selected old `ntseapi.h`, producing type redefinitions; `wole2.c` needs the selected `THREAD_BASIC_INFORMATION` carrier | establish one ordered, source-shaped x86 ABI include family; do not suppress redefinitions or use host-width structures. |

Other emitted implicit-declaration warnings (`WOWGlobal*16`, `MBToWCS`,
`RtlImageNtHeader`) are inventory inputs, not permission to rely on implicit
returns.  The full run proves that the next work is an ABI carrier/adapter
closure, not a selective deletion of difficult WOW bodies.

## First composition-boundary measurement

The selected 77 objects, original resource and original `.def` were first
force-linked in order to inventory only unresolved imports; that artifact is
not a runnable DLL.  Reusing the full set of existing T395 static x86 objects
in the same experiment resolves many project-owned names, but it also proves
that those objects are not a valid DLL dependency contract:

- WOW32 is compiled with the original callback-required x86 `__stdcall`
  profile, whereas several T395 static objects publish historical host calls
  with undecorated/cdecl names.  The linker reports, for example,
  `Sim32FlushVDMPointer@16` requested by WOW32 while the available static
  provider is `Sim32FlushVDMPointer`.
- Whole-machine static inclusion also creates an actual duplicate global,
  `VdmRedirLoaded` (`wkfileio.obj` versus `nt_bop.obj`).  This is not an
  unresolved-interface problem and cannot be hidden with `/FORCE`.
- Pulling all DEM/SoftPC objects recursively magnifies the same issue into
  their private CPU, app-observation and NTDLL dependencies.  That is evidence
  against treating an existing EXE-static composition as a DLL import library,
  not evidence for stubbing them in WOW32.

The original `.def` also exposed one real x86 export-form detail: the source
unit `wucomm.c` must retain `/Gz` for its `CreateThread` callback, but the two
legacy public names are written undecorated in the original definition file.
`New-T396Wow32ProviderNinja.ps1` therefore emits a build-local `wow32-x86.def`
that preserves every original external export name and adds only
`GetCommHandle=GetCommHandle@4` and
`GetCommShadowMSR=GetCommShadowMSR@4` internal x86 aliases.  It does not alter
the original `wow32.def`, source body, or exported surface.

The resulting next boundary is now precise: S2 must define a narrow, x86
calling-convention-correct DLL-to-session/SoftPC/BaseVDM import contract, and
compile its providers under that contract.  It may not statically absorb
`host`, `DEM`, CCPU or app objects into `wow32.dll`, and may not create a
USER/CSRSS/Win32k substitute.  The current force-link attempt remains a
diagnostic only and does not satisfy the formal-link or runtime criteria.

## Correct original EXE-to-DLL boundary and ABI recovery

The original `nt_bop.c` is the authoritative loader topology: it calls
`SafeLoadLibrary("WOW32")`, then obtains `W32Init`, `W32Dispatch`, the COMM
exports and the hung-app callback by `GetProcAddress`.  The selected original
`ntvdm.def` is therefore the correct parent-side service boundary, not an
accidental static-object bundle.  The formal T395 product already produces
the corresponding `original-softpc-process-import.lib`.

The initial full `/Gz` WOW build was wrong for that topology.  It made every
historical host import `__stdcall`, while the parent export surface and the
original declarations are cdecl.  Rebuilding the complete cohort with `/Gd`
shows exactly the small exceptional set which must be stdcall:

| Original callback contract | Explicitly retained `WINAPI` body |
| --- | --- |
| `PFNWOWHANDLERSIN` USER slots | `W32EmptyClipboard`, `SetFakeDialogClass`, `WU32ICBStoreHandle` |
| `RegisterWowBaseHandlers` global-free slot | `W32DDEFreeGlobalMem32` |
| `host_CreateThread` / `CreateThread` | `W32Thread`, `WowMsgBoxThread`, `WOWCommWriterThread` |
| USER hung-app handler | existing `W32EndTask` declaration corrected to `APIENTRY` |
| public ISV memory callback | `WOW32ResolveMemory` declaration corrected to match its existing `APIENTRY` body |

The modern SDK also declares `GetRandomRgn` as `WINAPI`; the local historical
re-declaration in `wgdi31.c` now states that ABI explicitly.  These are ABI
corrections at exact original callback/SDK boundaries, not replacement code
or a policy change.  All 77 selected bodies compile in that mixed profile;
the remaining diagnostics are legacy CRT/format warnings.

With the parent import library, the next force-link resolves the existing
original CCPU/BOP/DEM exports without embedding a second machine instance.
The resulting residual groups are now finite and owner-specific:

1. **Missing parent exports or project adapter seams:**
   `Sim32FlushVDMPointer`, guest-memory lease/effective-address helpers,
   `ExitVDM`, VDM virtual-memory/descriptor services, OEM path helpers,
   BaseVDM command delivery and VDD SFT shadow helpers.  These require a
   narrow parent export/facade audit with their existing failure direction.
2. **Private presentation/system services:** `UserRegisterWowHandlers`,
   `RegisterWowBaseHandlers`, `GdiQueryTable`, `GdiCleanCacheDC`,
   `GetETM`, `GetGlyphOutlineWow`, `SetCursorContents`, `ShowStartGlass`,
   font tracking and related private UI paths.  They cannot be satisfied by
   a synthetic USER/GDI/CSRSS provider; their source-shaped unavailable
   behavior must be designed before any runtime claim.
3. **Existing public/NT ABI mapping gaps:** `MBToWCSEx`,
   `DialogBoxIndirectParamAorW`, and the selected NTDLL aliases.  These remain
   declaration/adapter candidates, not permission to substitute a matching
   public API without call-site audit.

The parent-contract image is deliberately force-linked and is still not a
runnable DLL.  Its value is that it removes the false static-composition
frontier and gives S2 the exact export/facade ledger for the next pass.

## CPU40 SIM32 flush closure

The parent-contract measurement initially retained 38 references to
`Sim32FlushVDMPointer`.  This is not a missing cache or pointer provider in
the selected CPU40 profile: original `softpc.h` explicitly defines the
operation as `TRUE` there.  The late-loaded DLL nevertheless needs a concrete
external function for its historical declaration spelling, so S2 adds the
single cdecl `ADAPTER-WOW-002` body
`wow_sim32_pointer_compat.c`.  It ignores its four arguments and returns
`TRUE`, exactly preserving the selected source contract; it receives no raw
pointer and owns no mapping or guest state.

After compiling that body with the same mixed x86 ABI, the force-link no
longer reports `Sim32FlushVDMPointer` at any WOW caller.  The measurement is
still non-runnable because private USER/GDI/CSRSS and remaining owner seams
are intentionally unresolved.  It does, however, establish that the callback
lease stays the sole guest-pointer route and that a CPU30 provider is neither
needed nor admitted.

The formal parent-build generator now derives a build-local
`ntvdm-wow32.def` from the original host definition and appends only existing
owned seams: `GetNextVDMCommand`, `ExitVDM`,
`mvdm_softpc_effective_address`, and the two guest-memory lease operations.
The original definition remains unchanged.  A rebuild to regenerate the
parent import library was started with the required Node 22 generator and x86
compiler environment, but its Ninja process reached an idle wait without any
compiler child or updated output and was stopped.  That is recorded as an
environment/build-worker wait, not as validation of the newly appended
exports; S2 must rerun this targeted parent link under a responsive formal
worker before claiming those five imports resolved.

## Interpretation and follow-up

The formal graph is valid and complete at the source-list level, but is not a
linked DLL yet.  The immediate next S2 action is an `ntlpcapi` union audit,
then source-shaped declaration admission or an existing matching carrier.
Only after that can the next compilation frontier be measured.  This record
does not claim a resolved import, a functional `W32Init`, or a runnable Win16
workload.

## Final private-boundary measurement

The parent import-library and the already-admitted public/source-shaped facade
work remove every non-private unresolved import from the complete 77-body
cohort.  The final x86 diagnostic link, against the regenerated parent import
library, has exactly these five physical unresolved names:

```
UserRegisterWowHandlers
RegisterWowBaseHandlers@4
GdiQueryTable
RegisterWowExec@4
SetCursorContents@8
```

They are not five versions of one absent Win32 API.  The retained source and
the local carrier establish four irreducible private-system contracts:

| Unresolved edge | Retained source/call site | What the source requires | S2 disposition |
| --- | --- | --- | --- |
| `UserRegisterWowHandlers` | `wow32.c:521-547`; `opennt-abi/.../wowuserp.h` | NTUSER must retain the 21 inbound WOW callbacks and return an ordered 20-slot `PFNWOWHANDLERSOUT` table.  Its slots perform server window creation, class/WOW-word identity, task registration, resource objects, cleanup, yields and private message waiting. | No public single-API equivalent; constructing a replacement table is explicitly excluded by S2. |
| `GdiQueryTable` | `wow32.c:249-255`; `walias.c:658-685` | A pointer to the private GDI shared handle table. `hConvert16to32` directly indexes its `FullUnique` generation field to manufacture a 32-bit GDI handle. | No public table exists; a public handle lookup cannot preserve the index/generation ABI. |
| `SetCursorContents` | `wcuricon.c:701-710` | NTUSER replaces the contents of an old cursor/icon object *in place*, destroys the newly created object and preserves the old handle's identity. | Public `CopyImage`/destroy forms cannot preserve that identity; a private USER object/alias manager would be a new provider. |
| `RegisterWowBaseHandlers` | `wow32.c:545-547,272`; OpenNT `base/client/gmem.c:27-35` | Kernel32 records `W32DDEFreeGlobalMem32` as the process global-free callback. | The original body is a one-field registration, but its invoked consumer is the system global-memory path; no current parent-side hook owns that path.  A local storage-only stub would fabricate registration success. |
| `RegisterWowExec` | `wkman.c:...WK32RegisterShellWindowHandle`; OpenNT `base/client/vdm.c:1014-1048`, `base/server/srvvdm.c:569-646` | A CSR client call registers the WOWEXEC window; BaseSrv records process/thread/CSR sequence identity and later posts launch notifications. | This is a CSRSS/BaseSrv cross-process protocol. A single-process facade must be separately designed and admitted; it cannot be reduced to `RegisterClass` or a no-op. |

`GdiCleanCacheDC`, `GetETM`, the network-font functions, `ShowStartGlass`,
`GetHFONT`, `GetGlyphOutlineWow`, `MBToWCSEx` and
`DialogBoxIndirectParamAorW` are absent from this list because their exact
call sites admitted smaller facades: the current public facade source compiles
and the final measurement resolves each of those names.  In particular,
OpenNT `ntgdi/client/local.c` shows `GdiCleanCacheDC` has no required mutation
after its private alternate-DC check; this is not evidence that the GDI shared
handle-table requirement is optional.

The five-edge result triggers S2's declared stop condition.  A normal link
cannot produce a loadable `wow32.dll`; `/FORCE:UNRESOLVED` remains only an
import-inventory artifact.  S2 must not add a synthetic callback table, fake
GDI uniqueness table, cursor identity substitution, local registration-success
stub, or CSRSS lookalike merely to make the linker quiet.  A new owner decision
is required either to admit one or more narrowly specified private-boundary
recovery packages, or to accept a formal source-cohort closure whose loader
entry is deliberately unavailable.  No Win16 runtime-success claim follows
from this measurement.

## System WOW ABI correction

The preceding conclusion was corrected by inspecting the actual x86 system
DLL export directories on the active host, rather than relying on the reduced
modern SDK import libraries.  SysWOW64 `USER32.DLL` exports
`UserRegisterWowHandlers` and `SetCursorContents`; SysWOW64 `GDI32.DLL`
exports `GdiQueryTable`; and SysWOW64 `KERNEL32.DLL` exports both
`RegisterWowBaseHandlers` and `RegisterWowExec`.  These are retained Windows
WOW compatibility exports, not newly provided project behavior.

`New-T396Wow32ProviderNinja.ps1` now emits three task-local `.def` files and
uses MSVC `lib /def` to make import libraries only for those exports.  COFF
symbol inspection proves their exact match to the selected source's x86
references:

| System DLL | WOW32 reference | generated import symbol |
| --- | --- | --- |
| USER32 | `_UserRegisterWowHandlers`, `_SetCursorContents@8` | `__imp__UserRegisterWowHandlers`, `__imp__SetCursorContents@8` |
| GDI32 | `_GdiQueryTable` | `__imp__GdiQueryTable` |
| KERNEL32 | `_RegisterWowBaseHandlers@4`, `_RegisterWowExec@4` | `__imp__RegisterWowBaseHandlers@4`, `__imp__RegisterWowExec@4` |

An ordinary (non-`/FORCE`) DLL link using those libraries no longer reports
any of the five names.  It does expose a separate, legitimate composition
frontier: the cohort's historical CRT/NTDLL and project adapter objects must
be split from the existing parent-machine static archives before a DLL can
link without duplicating parent owners.  Adding the whole T395 static archive
is explicitly rejected by ordinary-link duplicate definitions (`VdmRedirLoaded`,
machine services, OEM services and existing parent exports) and recursive
machine dependencies.  That is a graph/ownership issue, not a reason to
recreate USER, GDI or CSRSS.

Consequently the former “final private-boundary measurement” is retained as
the SDK-import-library observation, but its claim that the five interfaces
have no usable host provider is superseded.  S2 resumes at the narrower
owner-DLL/CRT/NTDLL dependency split.  Runtime success remains unclaimed.

## Public USER facade closure measurement

The remaining public-looking USER names were audited at their exact selected
WOW32 call sites before adding any adapter body.  The source authorities are
the OpenNT copies of `ntuser/rtl/chartran.c`
(`SHA256 0F9F0B7FDE3AAA475492A50F333745508698228C38FDBF9A21CBD4BFBEAD383B`)
and `ntuser/client/clres.c`
(`SHA256 C11265557B8406D60928032F3CAA0C106301B5847930C98E356E8C72372808F5`).

`wucursor.c` and `wumenu.c` use `MBToWCSEx` only with a nul-terminated ANSI
input, allocation requested, and a subsequent `LocalFree`.  The narrow
`ADAPTER-WIN32-050` body consequently preserves the original `-1` length and
failure rules, conversion count, caller-owned returned pointer and local-heap
lifetime using `LocalAlloc` plus `MultiByteToWideChar`; it has no hidden USER
heap, cache or guest-memory authority.  `wudlg.c` supplies an already
converted 16-bit dialog template to `DialogBoxIndirectParamAorW` with
`SCDLG_ANSI`.  `ADAPTER-WIN32-051` maps that selected client-dialog choice to
the public `DialogBoxIndirectParamA` API (and uses W only for the other flag
state), retaining the modal API's `-1` failure direction.  It does not
recreate NTUSER's internal dialog parser, callback registration, CSRSS
protocol or GUI policy.

Both bodies compile in the provider's `/Gd` profile.  The renewed
`/FORCE:UNRESOLVED` parent-contract measurement exports local definitions for
`_MBToWCSEx`, `_DialogBoxIndirectParamAorW@24`, and
`_Sim32FlushVDMPointer`; none is reported as an unresolved caller edge.
The remaining unresolved list is now confined to the known owner groups:
private USER/GDI registration and presentation services, OEM/file/font
wrappers, bounded project session/VDM seams not yet in the regenerated parent
import library, and BaseVDM/VDD services.  The produced image remains a
diagnostic-only `/FORCE` image and is not evidence of a runnable `W32Init`.

## Existing OEM parent-service contract

The OEM/file group is not an implementation gap.  Every reached function is
already defined by selected original `mvdm-host/oemuni/file.c` or `process.c`,
is declared in the original `mvdm-host/inc/oemuni.h`, and is linked into the
parent product as `original-mvdm-host-oemuni.lib`.  It was absent only from
the late-loaded DLL export surface.  The build-local `ntvdm-wow32.def` now
adds aliases with the original x86 `WINAPI`/`APIENTRY` decoration for the 14
reached services: `CreateFileOem`, file attribute/delete/move forms,
full-path/temp/volume/current-directory forms, the two environment forms,
and `RemoveFontResourceOem`.  The original source and the original parent
definition remain unchanged; no duplicate filesystem or OEM-conversion layer
is admitted to WOW32.

`New-T310OriginalSoftpcNinja.ps1` successfully generated this derived
definition in the task-owned `build/M0-T396/S2/parent-contract-x86` root.
A single-worker parent rebuild was then started there.  The `ninja.exe`
process remained alive for 90 seconds without a `cl.exe` or `link.exe` child
and without output, and was stopped.  Thus definition syntax/generation is
validated, while the regenerated executable/import-library exports remain
unverified.  The next valid measurement must use a responsive formal build
worker; it may then re-link WOW32 against the regenerated import library to
close this whole already-owned OEM group at once.

## Verified parent import closure and x86 name correction

The first direct parent re-link exposed an important x86 detail: a `.def`
target must name the actual COFF symbol with its leading underscore, and the
exported name used to build the import library must retain that same underscore
for WOW32's direct historical imports.  `dumpbin /linkermember:1` then showed
both `_CreateFileOem@28` and `_GetNextVDMCommand@4` (and their `__imp__`
forms) in the generated parent import library.  This was verified by linking
the complete 77-body WOW32 cohort against that library.

The renewed map imports all 14 OEM services plus `_GetNextVDMCommand@4` and
`_ExitVDM@8` from the one parent `ntvdm.exe`; none remains in the unresolved
diagnostic list.  This validates the intended late-loaded-DLL topology and
removes 16 unresolved caller groups without copying source bodies into
WOW32.  It is a direct link of the already-completed T395 object/library set
with the generated T396 definition, not a claim that the stalled full Ninja
source rebuild completed.

The next host item is intentionally not auto-exported: the parent provides
`_host_CreateThread@24`/`_host_ExitThread@4`, while the selected `/Gd` WOW32
cohort requests undecorated cdecl `_host_CreateThread`/`_host_ExitThread`.
That is an ABI mismatch, not an omission.  S2 must audit the original
declarations and define a source-compatible lifetime bridge before resolving
it.  The still-unresolved private USER/GDI, VDD and font-tracking groups
remain outside this closure.

## Further host and public-GDI closure

The historical `softpc.h` declaration omitted `WINAPI` for
`host_CreateThread` and `host_ExitThread`, although both original
`nt_thred.c` definitions and the original `ntvdm.prf`/`wow32.prf` publish
`@24` and `@4` respectively.  Restoring that declaration ABI and recompiling
the one affected original caller (`wkman.c`) resolves both through the
unchanged parent providers.  This is a source declaration correction, not a
new thread owner or bridge.

The existing parent already owns the original VDM allocation/free/add/remove
and descriptor-mapping bodies, `SetWOWforceIncrAlloc`, and the bounded VDD
SFT shadow.  Adding only those cdecl names to the build-local parent export
definition resolves all eight corresponding WOW32 references.  The provider
does not acquire a second memory manager, descriptor store, or VDD state.

Two GDI names then admitted narrow public mappings.  OpenNT's `GreGetHFONT`
returns the currently selected DC font, which public
`GetCurrentObject(hdc, OBJ_FONT)` supplies.  `GetGlyphOutlineWow` receives
the same caller-owned ANSI glyph code, metrics, buffer, `MAT2`, count and
error contract as public `GetGlyphOutlineA`.  `ADAPTER-WIN32-052/053` supply
only those mappings; the renewed link removes both names.  `GetETM` was not
folded into this mapping because its private `EXTTEXTMETRIC` construction is
not proven equivalent to a public outline-metric structure.

After this measurement the unresolved set contains exactly: private USER
handler registration and cursor replacement, private GDI table/cache/ETM,
WOWEXEC registration, and network-font tracking/unload.  It no longer
contains OEM, BaseVDM, host-thread, VDM-memory, descriptor, VDD SFT, current
font or glyph-outline edges.

## Ordinary complete-provider link and loader disposition

The remaining entries in the preceding measurement were not missing project
providers.  They are retained x86 exports of the actual system `USER32`,
`GDI32`, `KERNEL32`, and `NTDLL` DLLs.  The task-local import definitions
therefore name only the observed system exports; they contain no replacement
implementation.  The parent-owned stateful forms remain imports from the
running product through the derived `ntvdm-wow32.def` contract.

The complete 77-body cohort, original resource, four admitted support bodies,
task-local system import libraries and regenerated parent import library then
linked ordinarily (without `/FORCE`) as:

```
build/M0-T396/S2/system-wow-import-v2-x86/wow32.dll
SHA-256 4333E8D705B017AF784C3855B1DBDA87E17C6EB2EBFB11D82BD8D8B5CA58F9A6
```

PE import inspection records `ntvdm32.exe`, `USER32.DLL`, `GDI32.DLL`,
`KERNEL32.DLL`, and `NTDLL.DLL` among the actual dependency names.  The map
records original-provider definitions for `W32Init`, `W32Dispatch`,
`W32HungAppNotifyThread`, `GetCommHandle`, and `GetCommShadowMSR`.  Thus the
late-load contract has one parent machine and one WOW32 provider; no SoftPC,
DEM, USER, GDI, CSRSS, or BaseSrv implementation was copied into the DLL.

`New-T396Wow32ProviderNinja.ps1` now requires an explicit architecture-named
parent import library, derives a task-local x86 `.def` only for the two
historical undecorated COMM aliases, and performs an ordinary link.  This
removes the obsolete `/FORCE:UNRESOLVED` rule from the formal target.  A fresh
single-worker Ninja invocation generated the graph and compiled its first
object, then again remained alive without a `cl.exe` or `link.exe` child; it
was stopped.  That runner failure is not counted as a fresh graph pass.  The
earlier full 77-body compilation and the ordinary direct link above are the
retained S2 composition evidence.

One task-local staged probe was deliberately not treated as `W32Init`
evidence.  Its executable-relative root was 57 characters long and the app
correctly returned `69` (`APP_STARTUP_COMMAND_REJECTED`) before original
SoftPC entry, because unchanged original `cmdconf` permits only the existing
64-byte `SHELL=` value.  No product package was overwritten and no short-root
deployment was manufactured.  Loader execution and a selected Win16 workload
remain the next runtime subtask; this S2 record makes no such claim.
