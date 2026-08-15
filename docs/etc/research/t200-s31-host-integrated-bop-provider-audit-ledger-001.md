# T200 S31: Host-Integrated OpenNT BOP Provider Audit Ledger

## Status and decision rule

This is the admission ledger for the queue-head **Host-integrated OpenNT BOP
capability recovery** package.  It is an audit, not an implementation
backlog and not a trace-derived handler list.

`non-invasive` means unpack-and-run without rebuilding/modifying Windows,
installing drivers/services/VDDs, or installation-style Registry mutation. It
does not prohibit selected ordinary user-mode host API use under the caller's
permissions.  Therefore the target default is **direct host integration**;
`readonly`, `volatile-overlay`, and `virtual boot-volume` are explicit storage
profiles, not reasons to make direct host behavior unavailable by default.

Each endpoint will receive exactly one owner package and one implementation
disposition:

| Disposition | Meaning |
| --- | --- |
| `direct-original` | Original owner/provider can be separately composed with normal selected host APIs. |
| `original+seam` | Original ordering/layout/failure behavior is retained; bx-vdm supplies only copied ABI/platform glue. |
| `source-derived` | Historical composition cannot link, so bx-vdm rehosts the evidenced contract. |
| `profile-backend` | Behavior belongs to readonly, overlay, or virtual-volume storage profile rather than default direct mode. |
| `deferred/unavailable` | No admitted current profile; failure/defer semantics must be source-proven. |
| `workaround-action` | Existing code must be retained, migrated, replaced, or deleted before the package is called complete. |

Every `direct-original` or `original+seam` row also records an API
availability verdict: the exact historical Win32/NT API, whether it remains
available to a normal modern user-mode process, its required permission and
an equivalent only when the equivalence is source/ABI defensible.  A deleted
API, an NT4-private/system-rebuild dependency, a kernel-only entry, or an
unavailable host component is reported as an explicit endpoint blocker; it is
never silently converted into a simplified provider. Such a row never blocks
the audit of other BOPs or packages: its disposition remains visible while
the audit continues with the next owner package.

## Mechanical inventory

| Selector/family | Original dispatcher/source | Callable range evidenced by current ingress | Current package boundary | Audit state |
| --- | --- | --- | --- | --- |
| `50:00..48` DEM | `base/mvdm/dos/dem/demdisp.c` | 73 services | DEM namespace, FCB, GSET, raw-media, misc, error/lock | in progress |
| `51` WOW | `nt_bop.c:MS_bop_1`, WOW DLL composition | selector-only ingress | WOW host composition | pending; not a DOS direct-host substitute |
| `52:00..0B` XMS | `base/mvdm/xms.486/xmsdisp.c` | 12 services | XMS/A20/allocator/UMB/INT15 | pending |
| `53:00..18` DPMI | `base/mvdm/dpmi32/*` dispatcher family | 25 services | protected mode/LDT/exception/memory | pending |
| `54:00..10` COMMAND | `base/mvdm/dos/command/cmddisp.c` | 17 services | bootstrap/launch/console/environment/lifecycle | in progress |
| `56` debugger | `nt_bop.c` and debugger owners | selector-only ingress | debugger package | pending |
| `57:00..31` redirector | `vdmredir.h`/redirector owners | 50 services | redirector package | pending |
| top level (`12,15,59,5A..5F,FD,FE`) | `bop.h`, `nt_bop.c`, machine sources | selector-only ingress | machine/top-level package | pending |

The `bx_ntvdm_bop_catalog_v1` maximums are inventory bounds only.  They do
not prove a provider, ownership, or compatibility result.

## Corrected current-code disposition

| Existing bx-vdm area | Current role | Required audit action |
| --- | --- | --- |
| `bx_ntvdm_host_drive_policy`, `bx_ntvdm_host_namespace`, `bx_ntvdm_host_volume_snapshot_v1` | Direct-host foundations; selected real drives, opaque root handles, volume observation | retain and evolve into shared direct/readonly/overlay drive-view ABI |
| `readonly_namespace`, `boot_namespace_provider/plane/composition`, `profile_search_*` | BYOB deterministic fixture / virtual-volume candidate | migrate out of default direct-host path; retain only for a declared profile or fixture |
| `dem_readonly_namespace_failure_provider_v1` | Partial mutation refusal | replace with the complete readonly-profile failure family; never use as direct default |
| synthetic-root current-directory services | Source-derived minimal answer, not real host cwd | replace with direct-host OpenNT-shaped cwd provider; retain only as explicit virtual profile behavior if applicable |
| broad CLI-unavailable and redirector-unavailable providers | Current safety fallback | classify each endpoint's original failure path; retain only where current profile genuinely remains unavailable |
| duplicate root-handle/session holders | Historical experimental composition residue | identify one owner in shared drive-view ABI; migrate or delete duplicates |

### Current composition binding map (not a closure claim)

The executable path is not equivalent to the generic ingress registry.
`bx_ntvdm_vdm_generic_ud_bridge_v1.c` first offers a BOP to
`boot_namespace_composition`, then only offers an unhandled request to the
separate `native_bop_composition` (XMS/DPMI).  The following map prevents the
audit from mistaking a `mapped-deferred` ingress result for the behavior of a
bound composition.

| Owner package | Current bound handling | Audit classification/action |
| --- | --- | --- |
| DEM | `dem_package_session` routes a selected subset through boot/readonly namespace, drive/volume snapshots, synthetic CWD, selected DPB/media/name services and CLI-unavailable fallbacks. `dem_package_facade` declares its subset explicitly. | It is a mixed fixture/profile composition, not a direct-host DEM provider. Retain only testable subcontracts; migrate or replace all default host-path behavior in the DEM packages. |
| COMMAND | `command_package_session` reaches `54:01`, `02`, `04`, `05`, `0B`, `0C..10` through declared plan, boot namespace or limited helpers. | Endpoint-local source-derived evidence only. Re-audit under COMMAND package; no service is complete merely because it produces a typed result. |
| XMS | fallback `native_bop_composition` implements A20 and selected allocation/query/reallocation (`52:00,02,03,05,0B`) through mantle capabilities; other XMS services produce typed stop. | Preserve as a candidate mechanical seam, then compare full XMS ABI/failure semantics before retain/migrate/replace. |
| DPMI | fallback `native_bop_composition` classifies all 25 identities and stops them. | Correctly non-successful today; remains deferred until a complete protected-mode composition is admitted. |
| Redirector | `redir_package_facade` consumes the mapped/deferred selection through `system_plane` and returns `CF=1, AX=1, RIP+4` for all 50 slots. | This is a reachable source-derived unavailable family, not a network implementation. Retain it until an explicit network profile is admitted; delete or migrate the older unbound unavailable provider. |
| Debugger | `debugger_package_facade` consumes the three-byte selector and returns typed stop. | Retain as a deferred non-debug profile safeguard; it is not a debugger provider. |
| WOW/top-level/machine | WOW and selected terminal selectors stop; `5A` resumes; `5E` has bounded config/no-op paths; `12/15` use the machine memory helper; `5C/5D/FD` stop. | Each remains with its named owner. Terminal/stop handling is not historical composition recovery. |

## Package order after audit

The candidate T must use the proposal's package order, not BOP trace order:

1. shared CLI storage-profile and drive-view ABI;
2. DEM direct-host namespace/directory/cwd/volume;
3. DEM direct-host file/handle/FCB/search/read-write;
4. COMMAND direct-host console/environment/cwd/bootstrap/launch;
5. complete readonly profile;
6. volatile overlay profile;
7. virtual boot-volume profile;
8. remaining-family reclassification, obsolete-code deletion, family
   regressions and bounded native integration.

## Evidence anchors

- Selector definitions: `src/opennt/base/mvdm/inc/bop.h`.
- Historical top-level host dispatch composition: `src/opennt/base/mvdm/softpc.new/host/src/nt_bop.c`.
- DEM/COMMAND/XMS dispatch arrays: `demdisp.c`, `cmddisp.c`, `xmsdisp.c`.
- Current selector ingress and provider registry:
  `src/bx-vdm/bx_ntvdm_bop_ingress_v1.c` and
  `src/bx-vdm/bx_ntvdm_bop_provider_registry_v1.c`.
- Reproducible source inventory: run
  `tools/Export-T200S31BopSourceInventory.ps1` against the repository. Its
  corrected output, `t200-s31-bop-source-inventory-r8`, contains 203 original
  dispatcher/header entries and is the mandatory coverage set for the
  endpoint-level ledger. Revision r1's count of 199 is superseded: its parser
  omitted un-commented or non-comma-terminated final entries from historical
  dispatch arrays; the array index is itself an ABI fact.
- Policy decision and target storage profiles:
  [host-integrated recovery proposal](proposal-host-integrated-bop-capability-recovery-001.md).

The r8 artifact has one mechanically generated row for every one of those
203 endpoints, with original handler/source, exactly one owner package, the
current ingress state, and the actual bound-composition state derived from
the façade/session routes. It is deliberately marked `ABI/failure/API review
pending`: a mapped/deferred route is not a provider and the generated owner
is a coverage guard, not a compatibility claim. The human ledger expands
each row's caller, layout, host API, failure path, current bx-vdm code and
final disposition. Unknown is recorded as unknown; it is never inferred from
a trace hit.

### Mechanical reconciliation at r2

The corrected source table exactly covers the service-array cardinalities
already recognized by ingress: DEM `73` (`00..48`), XMS `12` (`00..0B`),
DPMI `25` (`00..18`), COMMAND `17` (`00..10`) and Redirector `50`
(`00..31`).  This confirms only that the *identity boundary* is complete; it
does not upgrade any mapped/deferred route into a provider.  Debugger is
different by original contract: the BOP is three bytes and its sixteen mode
definitions are stack-resident; an adapter must not consume a fictitious
fourth instruction byte.  WOW and top-level BOPs remain selector-only.

The r2 comparison also establishes two audit findings to be repaired by the
future owner packages, not by an ingress micro-patch:

| Finding | Evidence | Required package action |
| --- | --- | --- |
| legacy `57` unavailable provider is unbound | `bx_ntvdm_redir_unavailable_provider_v1` requires top-level explicit-unavailable while current ingress supplies mapped/deferred; `boot_namespace_composition` instead calls `redir_package_facade` | Retain the reachable façade as the family unavailable result; delete or migrate the redundant legacy provider in the Redirector package. |
| Current provider registry has only generic deferred/unavailable values | `bx_ntvdm_bop_provider_registry_v1.[ch]` | The shared storage/profile ABI and each owner package must replace this generic state with provider-capability selection; no service gets implementation merely from the generic registry. |

### First unavailable/private-host-API register

The following is an affirmative audit result, not a stop condition.  These
names must not be mistaken for ordinary Win32 APIs merely because their calls
occur in otherwise useful OpenNT owner code.  Work proceeds with unrelated
endpoints; a later package must either preserve the recorded contract through
a narrow CLI seam or retain the original unavailable/deferred result.

| Historical surface | Reached owner scope | Modern CLI availability verdict | Required treatment |
| --- | --- | --- | --- |
| `GetNextVDMCommand` and its `VDMINFO` transaction | COMMAND bootstrap/return/launch (`cmdmisc.c`, `cmdexec.c`) | historical NTVDM host-command broker; no ordinary x64 user-mode Win32 API/ABI that can be linked as its replacement | Record as a source-derived CLI command/session contract; do not substitute `CreateProcess` endpoint-by-endpoint. |
| `TerminateVDM` | COMMAND exit/config/environment and top-level unsimulate | historical VDM engine terminal hook, not public process termination semantics | Use the typed engine terminal/result ABI only after its owner package is admitted; never substitute a host-process kill. |
| `Sim32GetVDMPointer`, `Sim32FlushVDMPointer`, `Sim32FreeVDMPointer`, register macros | DEM, XMS, DPMI and `nt_bop.c` | historical VDM/CCPU/SAS composition, not a public Win32 capability | Replace only with typed, checked bx-vdm guest-memory/CPU transactions; never leak raw Bochs mappings or reintroduce CCPU. |
| `GetVDMAddr` | DEM and COMMAND services | historical VDM guest-address macro, not a user-mode Windows API | Rehost only individual fixed-width guest gathers/writes through the existing checked bx-vdm ABI; no flat guest pointer crosses the boundary. |
| `WaitIfIdle` / `WakeUpNow` | top-level `5A` (`nt_bop.c`) | internal NTVDM scheduling hooks, not public process scheduling APIs | Defer to the engine lifecycle/cancellation package; do not approximate with arbitrary sleeps or an adapter busy loop. |
| `VDMREDIR.DLL` / `VrDispatch` | Redirector `57:00..31` | historical NTVDM DLL composition absent from the admitted x64 CLI product | Keep one reachable, source-derived unavailable family until a separately admitted network profile defines its completion ABI. |
| `WOW32` `W32Init` / `W32Dispatch` hooks | WOW `51` | historical WOW/CCPU/VDD composition, not an independently composable ordinary host library | Keep the whole WOW package deferred; its absence does not affect direct DOS host-drive recovery. |
| `NtSetLdtEntries` / `NtSetInformationProcess(ProcessLdtInformation)` LDT path | DPMI selector/LDT (`i386/dpmi386.c`) | no supported x64 user-mode LDT contract; these are native historical process-mechanism calls, not a usable modern Win32 replacement | Defer selector/LDT work behind admitted bx-core/mantle protected-mode mechanics; do not build an adapter LDT emulation. |

## DEM first-pass owner groups (`50:00..48`)

The historical `apfnSVC` array in `demdisp.c` is the authoritative ordering.
The r8 inventory supplies the individual 73 source rows.  The following
partition corrects the earlier coarse ranges: each hex service is in exactly
one owner row and thereby gives the per-service audit a non-overlapping work
set.

| Services | Original owners / source surface | Direct-host or profile disposition | Current workaround action |
| --- | --- | --- | --- |
| `00,01,02,03,04,05,06,08,12,16,17,1E,22,27` | `demhndl.c`, `demfile.c`, `demdir.c`: seek, attributes, close, create/delete, directories, file time, open/read/rename/write/commit | direct host namespace/file/handle package; normal mutation follows selected-drive ACL/share results | Replace boot/readonly default path; retain it only as readonly/fixture profile backend. |
| `07,09,0A,0B,0C,20,2C,2D,2E,2F,30,31,44` | `demfcb.c`, `demsrch.c`: FCB, DTA, find-first/next, file-info and path validation | direct host FCB/search package with copied DOS records and opaque host handles | Migrate deterministic boot search and FCB snippets into explicit profile fixtures; direct provider must use real selected roots. |
| `0D,0E,0F,10,13,14,15,18,19,1A,1B,1C,25,41,46` | `demgset.c`, `demdir.c`: drive list/media, CWD, date/time, DTA, DPB and computer identity | direct drive-view/CWD/volume package; time-set preserves privilege failure; guest-visible DPB derives from selected view | Retain drive/volume capture foundation; replace synthetic CWD and boot-volume answers. |
| `21,29,2A` | `demioctl.c`, `demdasd.c`: IOCTL and absolute media I/O | separately admitted direct raw-device capability; ordinary pathname provider must not emulate it | Keep profile-gated/typed failure until a device policy and volume-handle ABI exist. |
| `11,1D,23,34,35,36,37,38,39,3A,3B,3C,3D,3E,45,47,48` | `demmisc.c`, `demdisp.c`: DOS load/kernel, dispatcher/session, console/debug, VDM/WOW and symbol/pipe behavior | split by owner: source-derived CLI lifecycle/console seam, or deferred session/WOW/debug capability | Broad CLI-unavailable fallback is not final; classify each under the owning session/console/debug package. |
| `32,33,3F` | `demerror.c`, `demlock.c`: hard error, retry, locks | original+seam with direct opaque-handle context and source-specific DOS failure | Retain only source-proved errors; no generic success or generic adapter error. |
| `1F,24,26,28,2B,40,42,43` | original `demNotYetImplemented` entries | original explicit no-op/unimplemented behavior | Retain as original disposition; `42` FASTREAD compatibility remains separately labelled source-derived evidence. |

### Immediate correction derived from the table

`demfile.c`, `demhndl.c`, `demdir.c`, and `demfcb.c` call normal host file and
directory APIs in their historical composition.  Under the corrected policy,
their direct-mode descendants are **not** prohibited merely because they can
create, rename, write, change attributes or change timestamps.  Those
operations instead require an explicit drive/storage policy and normal host
permission.  The previous blanket refusal belongs only to `readonly`.

| DEM host surface evidenced in source | Availability / permission verdict | Package consequence |
| --- | --- | --- |
| `ReadFile`, `WriteFile`, `SetFilePointer`, `SetEndOfFile`, `MoveFile`, `Find*`, `GetFileAttributes` | ordinary user-mode APIs; the selected host path's ACL/share state is the intended result source | direct file/handle/FCB/search package, with copied DOS-error conversion and opaque host-handle ownership. |
| `NtQueryDirectoryFile` | native API currently used by the adapter-private host-namespace foundation; it must remain behind that private handle seam | direct directory/search package; no guest-visible native handle or structure. |
| `DeviceIoControl`, absolute media reads/writes | ordinary API but often requires a volume/raw-device handle and elevation; policy must declare device access per selected drive | raw-media/DASD subpackage, separate from pathname I/O; absence of permission is an ordinary source-mapped failure, not an emulated filesystem success. |
| `SetLocalTime` | present but normally requires `SeSystemtimePrivilege`; historical source explicitly treats `ERROR_PRIVILEGE_NOT_HELD` | retain that privilege-sensitive failure behavior; it is not a missing API and must not block unrelated file services. |

## COMMAND first-pass owner groups (`54:00..10`)

`cmddisp.c` gives the authoritative service order.  Its implementation makes
an important distinction: ordinary host context/console/file APIs are not
invasive, while historical host-process launch and session behavior is a
separate capability with its own caller, lifecycle and failure proof.

| Services | Original owners | Historical API evidence | Target disposition | API availability / action |
| --- | --- | --- | --- | --- |
| `01`, `02`, `04`, `05`, `0C`, `0D`, `0F`, `10` | `cmdmisc.c`, `cmdconf.c`, `cmdenv.c` | `SetCurrentDirectory`, `Get/SetEnvironmentVariable`, `CreateFile` | `original+seam` for declared CLI context/storage profile | APIs remain normal user-mode; map process/session scope and OEM/layout ABI before reuse. |
| `06` | `cmdredir.c` | standard handles, temporary files, `CreateFile` | direct console/handle capability only after complete DOS handle package | APIs exist; raw handles must stay inside capability seam, not guest RAM. |
| `09`, `0E` | `cmdmisc.c`, `cmdkeyb.c` | console title/layout/CP and registry queries | `original+seam` under explicit console/keyboard profile | APIs exist, Registry reads are permitted; UI/console availability and missing key data must retain source failure. |
| `07` | `cmdexec.c` | executable classification and registry lookup | source/ABI audit with direct storage dependency | APIs exist; decide data/layout seam before direct composition. |
| `08`, `0A`, `0B` | `cmdexec.c` | `CreateProcess`, worker thread, COMSPEC/host process state | deferred separate host-launch/session package | `CreateProcess` exists, but this is not evidence that guest DOS may silently spawn host processes. It needs explicit policy and lifecycle design. |
| `00`, `03` | `cmdexit.c`, `cmdmisc.c` | VDM exit/save-world state | source-derived engine lifecycle / deferred | historical composition depends on VDM/session state; do not conflate with CLI process exit. |

Current `54:04` and `54:0E` evidence remains endpoint-local only.  It does
not close COMMAND cwd, console, launch, environment or lifecycle packages.

### COMMAND endpoint source/API register (`54:00..10`)

This is an endpoint-level source/API pass.  `direct API` here only means that
the named host API is eligible for the CLI capability seam; it never means
that the whole COMMAND BOP is implemented or admitted.

| BOP | Original handler/source | Principal historical surface | Current disposition for package recovery |
| --- | --- | --- | --- |
| `54:00` | `cmdExitVDM`, `cmdexit.c` | `TerminateVDM` | Historical private VDM termination; source-derived engine terminal contract, not `ExitProcess`. |
| `54:01` | `cmdGetNextCmd`, `cmdmisc.c` | `GetNextVDMCommand` / `VDMINFO` | Missing private broker; source-derived declared CLI command/session transaction. |
| `54:02` | `cmdComSpec`, `cmdmisc.c` | guest CMD structures, COMSPEC state | source-derived bootstrap using declared profile/guest memory; do not infer host `COMSPEC`. |
| `54:03` | `cmdSaveWorld`, `cmdmisc.c` | guest register/image save and historical `savevdm.wld` path | deferred engine lifecycle; no host system-file write or fixed `C:\\nt\\bin86` dependency. |
| `54:04` | `cmdGetCurrentDir`, `cmdmisc.c` | current-directory state | original+CLI drive-view seam; existing response is endpoint-local and synthetic/profile-shaped, so replace for direct mode. |
| `54:05` | `cmdSetInfo`, `cmdmisc.c` | `VDMINFO`, `SetCurrentDirectory` | source-derived command-session registration plus direct current-directory seam; private broker remains unavailable. |
| `54:06` | `cmdGetStdHandle`, `cmdredir.c` | standard handles/redirection files | ordinary APIs exist, but guest handle model belongs to the future complete DOS handle/redirection capability; no raw HANDLE crossing. |
| `54:07` | `cmdCheckBinary`, `cmdexec.c` | executable classification, WOW compatibility registry key | original+CLI storage/registry seam candidate; registry reads are allowed, but WOW launch remains out of this DOS package. |
| `54:08` | `cmdExec`, `cmdexec.c` | `CreateProcess`, temporary standard-handle rebinding, re-entry count | deferred explicit host-launch/session capability; no implicit host child process. |
| `54:09` | `cmdInitConsole`, `cmdmisc.c` | console mode/title/state | original+CLI console profile candidate; headless/redirected failure must be preserved. |
| `54:0A` | `cmdExecComspec32`, `cmdexec.c` | `CreateProcess`, parent/re-entry lifecycle | deferred with `54:08`; not made available merely because `CreateProcess` exists. |
| `54:0B` | `cmdReturnExitCode`, `cmdexec.c` | `GetNextVDMCommand` return transaction | source-derived session/terminal transport; existing typed result is not a general return closure. |
| `54:0C` | `cmdGetConfigSys`, `cmdconf.c` | guest config discovery, `CreateFile` | direct drive-view/guest-startup policy seam; must not materialize host boot files. |
| `54:0D` | `cmdGetAutoexecBat`, `cmdconf.c` | guest autoexec discovery, `CreateFile` | same package as `54:0C`; direct profile and virtual boot profile need distinct answers. |
| `54:0E` | `cmdGetKbdLayout`, `cmdkeyb.c` | `GetConsoleKeyboardLayoutName`, registry reads, `GetConsoleCP`, `GetSystemDirectory` | APIs remain available, but historical `KB16.COM`/`KEYBOARD.SYS` host assets are not guaranteed on modern x64. Make their absence a source-shaped no-install/failure result; do not fabricate files. |
| `54:0F` | `cmdGetInitEnvironment`, `cmdenv.c` | environment merge and `TerminateVDM` on failure | source-derived declared environment projection; private terminal behavior cannot be linked directly. |
| `54:10` | `cmdGetStartInfo`, `cmdmisc.c` | `VDMINFO` start fields | source-derived session bootstrap; depends on the same unavailable broker as `54:01`. |

## XMS first-pass owner groups (`52:00..0B`)

| Services | Original owners | Historical dependency | Target disposition | Availability/action |
| --- | --- | --- | --- | --- |
| `00` A20 | `xmsa20.c` | `Sim32*VDMPointer`, A20 wrap state | `original+seam` | No missing Win32 API. Replace Sim32 pointer access with checked guest-memory transaction; mantle/core owns A20 mechanics. |
| `01..03`, `0A..0B` move/allocate/free/query/realloc | `xmsblock.c`, `i386/xmsmem86.c` | XMS block table, guest pointers, `NtAllocateVirtualMemory`/`NtFreeVirtualMemory` | `original+seam` | Native allocation APIs exist, but historical VDM-address model cannot link directly. Audit allocation ceiling and copied pointer ABI. |
| `04..05` page/ext-memory query | `xmsmisc.c` | `GetSystemInfo`, XMS state | `original+seam` | `GetSystemInfo` is available; report guest-visible XMS capacity from admitted machine/profile, not ambient host RAM. |
| `06..08` UMB lifecycle | `xmsumb.c` | DOS kernel geometry, A20 state, UMB allocation | `original+seam` | Requires source-proven DOS/UMB memory map and mantle/core RAM ownership; not a host API blocker. |
| `09` INT 15 notification | `xmsmisc.c` | real-mode interrupt/firmware delivery | `deferred/unavailable` until machine owner admits INT15/PIC/firmware path | Mechanical prerequisite, not a reason to reimplement XMS in bx-vdm. |

The existing bx-vdm XMS package/session is therefore an audit candidate for
retain/migrate/replace classification, not evidence that the full original
XMS allocator or INT15 contract is already closed.

### XMS endpoint source/API register (`52:00..0B`)

| BOP | Original handler | Required owner seam | Current composition status |
| --- | --- | --- | --- |
| `52:00` | `xmsA20` | bx-core/mantle A20 state plus checked guest-memory transaction | partial mantle-backed candidate; audit move/flush semantics before retaining. |
| `52:01` | `xmsMoveBlock` | XMS block table, two checked guest ranges and overlap/error rules | deferred; never replace with an unchecked host `memcpy`. |
| `52:02` | `xmsAllocBlock` | XMS allocator, guest-visible handle/address policy | partial mantle-backed candidate; original `NtAllocateVirtualMemory` is not by itself the guest allocation contract. |
| `52:03` | `xmsFreeBlock` | same allocation ownership and invalid-handle failure | partial mantle-backed candidate. |
| `52:04` | `xmsSysPageSize` | admitted machine/XMS geometry | deferred; host page size must not automatically become guest XMS page geometry. |
| `52:05` | `xmsQueryExtMem` | declared guest XMS capacity | partial mantle-backed candidate; `GetSystemInfo` is available but ambient host RAM is not authoritative. |
| `52:06` | `xmsInitUMB` | DOS UMB layout, A20 and machine memory owner | deferred until actual guest UMB geometry is proved. |
| `52:07` | `xmsRequestUMB` | UMB allocator and DOS ownership | deferred with `52:06`. |
| `52:08` | `xmsReleaseUMB` | UMB allocator and DOS ownership | deferred with `52:06`. |
| `52:09` | `xmsNotifyHookI15` | real-mode INT15/PIC/firmware delivery | deferred machine prerequisite; not an adapter interrupt implementation. |
| `52:0A` | `xmsQueryFreeExtMem` | XMS allocator capacity/failure semantics | deferred pending complete allocator audit. |
| `52:0B` | `xmsReallocBlock` | allocation identity, move/resize semantics | partial mantle-backed candidate; must preserve original failure and address rules. |

## DPMI first-pass owner groups (`53:00..18`)

`dpmi32/dpmi32.c` holds the 25-entry original dispatch table.  The codebase
uses `Sim32GetVDMPointer` pervasively, maintains LDT selector/base tables, and
constructs protected-mode exception and interrupt frames.  This is a complete
protected-mode composition package, not a collection of independent BOP
shims.

| Group | Original files | Historical dependency | Disposition | Availability/action |
| --- | --- | --- | --- | --- |
| dispatch/shared state | `dpmi32.c`, `data.c`, `buffer.c`, `stack.c`, `register.c` | Sim32 guest mapping, selector tables, PM stack | `original+seam` candidate | API issue is legacy VDM ABI, not missing ordinary Win32 API; define checked translated guest-memory/CPU boundary first. |
| selector/LDT | `dpmiselr.c`, `dpmimscr.c`, `i386/dpmi386.c` | LDT programming and selector mapping | deferred pending admitted bx-core/mantle PM mechanics | `NtSetInformationProcess`/historical LDT mechanics need a modern availability verdict; do not emulate LDT in bx-vdm. |
| exceptions/interrupts/mode switch | `dpmiint.c`, `modesw.c`, `int21map.c` | CR0, exception frame, IVT, PM/RM transitions | deferred pending CPU/PIC/firmware evidence | Machine prerequisite; DPMI provider must not claim success until reached mechanics are native and checked. |
| DPMI memory | `xmem.c`, `dpmimemr.c`, `i386/dpmimem.c` | virtual-memory allocation plus guest descriptor/selector state | `original+seam` candidate | `NtAllocateVirtualMemory` is available but cannot alone close DPMI; record allocation and selector failure behavior. |

The existing `bx_ntvdm_dpmi_package_session_v1` is preliminary routing
evidence. It must be classified endpoint-by-endpoint before it can be called
a DPMI provider.

## Redirector first-pass owner groups (`57:00..31`)

The historical route is `nt_bop.c:MS_bop_7` → dynamically composed VDM
redirector `VrDispatch`, with service definitions in `rdrsvc.h` and APIs in
`vrnetapi.h`/`vrnmpipe.h`.  It covers LAN-manager/network use, named pipes,
mailslots, NetBIOS and asynchronous completion—not ordinary local host-drive
file operations, which remain DEM's responsibility.

| Group | Historical owner/dependency | Target disposition | Current workaround action |
| --- | --- | --- | --- |
| local `NetUse*`, workstation and enumeration calls | redirector package, historical network API composition | deferred until a selected network capability profile and modern API availability map exist | do not substitute direct host-drive namespace for redirector semantics |
| named pipes/mailslots/NetBIOS/asynchronous callbacks | redirector plus VDD/SoftPC callback state | deferred/unavailable | native `HANDLE`/callback objects cannot cross into guest; requires a dedicated copied completion seam |
| `57` ingress/failure path | `nt_bop.c:MS_bop_7` continuation adds the service byte | source-derived unavailable only while no redirector profile is admitted | retain `redir_package_facade` (`RIP+4`, `CF=1`, `AX=1`) and delete/migrate the unbound legacy `redir_unavailable_provider`. |

The first recovery proposal does not make redirector implementation a
prerequisite for direct local drives. It requires the existing fallback to be
honest, reachable and source-derived until a separately selected network
profile is admitted.

### Redirector endpoint partition (`57:00..31`)

`rdrsvc.h` supplies all fifty identity rows in the r8 inventory.  They are a
single network/IPC composition package, but the following non-overlapping
partition records their eventual provider and failure work rather than
allowing local DEM files to accidentally satisfy them.

| Services | Original function family | Modern capability status / required disposition |
| --- | --- | --- |
| `00,01,0F` | redirector lifecycle / reset | historical `VDMREDIR` lifetime is unavailable; source-derived family-unavailable continuation until a network profile exists. |
| `02..08` | named-pipe query/state/peek/transact/call/wait | Win32 named-pipe APIs may exist, but guest request/response buffers and asynchronous completion require a dedicated copied IPC seam; deferred. |
| `09..0E` | mailslot creation/info/read/write | historical mailslot composition and callback state; deferred/unavailable as a family. |
| `10..1A` | remote Net API/server/use/workstation/message | old LAN Manager API layouts and remote marshalling; deferred behind an explicit network profile, not replaced by local-drive operations. |
| `1B..22` | network identity, NetHandle and DC queries | network identity/authorization semantics; deferred; ordinary host computer-name is DEM `50:41`, not a redirector substitute. |
| `23,24` | asynchronous named-pipe I/O | callback/interrupt completion ABI missing; deferred, no raw OVERLAPPED/HANDLE crosses into guest. |
| `25..27` | NetBIOS/DLC request and interrupt post | device/interrupt ownership absent; deferred behind separate machine/device admission. |
| `28..2A` | memory window / pause-continue mode | historical redirector-memory and scheduler coupling; deferred. |
| `2B..2E` | legacy NetUse/NetService aliases | same profile as `10..1A`; no isolated service implementation. |
| `2F..31` | redirector interrupt acknowledgement/PM NetBIOS check | machine callback and protected-mode prerequisites; deferred. |

The existing cleanup task is narrower than the whole package:
`system_plane` classifies `57:00..31` as explicit unavailable after the
generic deferred ingress selection, and `redir_package_facade` correctly
turns it into `CF=1, AX=1, RIP+4`.  The separate older
`redir_unavailable_provider` is not on that bound path and should be deleted
or migrated during the Redirector package cleanup.  No direct host filesystem
behavior is a permitted workaround.

The existing focused source regression
`tests/bx-vdm/bx_ntvdm_redir_package_facade_v1_test.c` iterates all fifty
defined service bytes and separately rejects `57:32`; it is the retained
family-level assertion for this unavailable disposition. A future network
profile replaces that assertion only with a complete package regression, not
with per-service trace fixes.

## WOW, debugger and top-level/machine selector audit

| Selector | Original behavior/evidence | Correct disposition | Current workaround action |
| --- | --- | --- | --- |
| `51` WOW | `nt_bop.c:MS_bop_1` loads `WOW32`, resolves `W32Init`/`W32Dispatch` and several communication hooks, then terminates VDM on failure | deferred as complete WOW host-composition package; ordinary `LoadLibrary` exists but the historical DLL/CCPU/VDD ABI is not an admitted CLI composition | replace `top_level_package_facade` terminal-stop mapping; it is not source-equivalent to WOW execution or failure. |
| `56` debugger | `MS_bop_6` → `DBGDispatch`, parameters on VDM stack | deferred/debug profile | retain typed stop only as a clearly labelled non-debug profile terminal, not debugger implementation. |
| `59` no-support, `5B` debug breakpoint | historical warning dialog / `OutputDebugString` + `DbgBreakPoint` | explicit unavailable or opt-in diagnostic profile | normal APIs exist, but UI/debugger activation is not default guest behavior. |
| `5A` wait-if-idle | `WaitIfIdle`/`WakeUpNow` host scheduling | original+CLI lifecycle seam | migrate from simple resume to bounded engine idle/cancellation policy after its ABI is admitted. |
| `5C`, `5D`, `FD` keyboard/video/RM switch | temporary VDD/video/machine mechanisms | deferred machine/device profiles | retain no default device claim; current stop is an observation safeguard, not source closure. |
| `5E` notification | config completion → UMB/DASD initialization | original+seam | retain current config-zero path only where source ABI is proved; audit nonzero no-op separately. |
| `5F` unimplemented interrupt | historical handoff/module registration paths | explicit unavailable/deferred | do not use it as generic adapter extension point. |
| `FE` unsimulate | `TerminateVDM` | source-derived engine terminal | retain/migrate only after fixed-width DOS terminal-result transport is designed. |
| `12`, `15` memory selectors | historical machine/BIOS handlers | bx-core/bx-mantle mechanical owner with typed bx-vdm request | keep selector blind Bochs rule; no OpenNT/BOP terms enter Bochs. |

### Machine/BIOS selector correction (`12`, `15`)

`xbios.h` names these as historical BIOS interrupt handlers (`BIOSINT_MEMORY`
and `BIOSINT_OSHOOK`), and the SoftPC BIOS initializer owns their mechanics.
They are not entries in OpenNT's `bop.h` host-service selector namespace.  The
current `bx_ntvdm_bios_memory_service_v1` is a legacy, adapter-local
source-derived workaround: it returns the recorded profile values for `12`
and `15/AH=88`, but its hard-coded values must not become an adapter-owned
BIOS implementation.

| Selector / path | Current evidence | Required repair disposition |
| --- | --- | --- |
| `12` memory-size BOP | current helper returns r20 conventional-memory profile evidence | migrate value ownership to a typed bx-mantle machine-profile/memory response; bx-vdm may only issue/receive a selector-blind fixed-width request. |
| `15` with `AH=88` | current helper returns r25 extended-memory profile evidence | same bx-mantle migration; future INT15/XMS paths must use one admitted machine memory geometry. |
| other `15` forms | no current provider | deferred to their native BIOS/PIC/firmware owner; no adapter no-op or fabricated success. |

This is a retain/migrate action for the existing helper, not permission to
teach bx-core any BOP, OpenNT, DOS or SoftPC selector names.

### Debugger mode partition (`56`, stack-resident `00..0F`)

`56` is a three-byte BOP.  Its fourth byte belongs to the following guest
instruction; the debugger mode and arguments are read from the guest stack by
`DBGDispatch`.  The 16 modes in `dbgsvc.h` are therefore inventory identities,
not a fourth-byte ingress protocol.

| Modes | Original role | Current / future disposition |
| --- | --- | --- |
| `00..04` | segment/module load, move, free | deferred debugger package; requires debug event and guest address/frame ABI. |
| `05..09` | single-step, breakpoint, GP/divide/instruction faults | deferred; CPU exception ownership stays bx-core/bx-mantle and only a copied debug event could cross bx-vdm. |
| `0A..0D` | task and DLL start/stop notifications | deferred debugger profile; no GUI/debugger activation in default CLI profile. |
| `0E,0F` | attach and ToolHelp | deferred; historical debugger/UI composition is absent. |

The current typed stop is a safe non-debug profile result for selector `56`.
It neither consumes an invented service byte nor constitutes debugger support.

This table completes the first-pass family coverage.  It does not close the
per-service audit: the next revisions expand DEM/COMMAND/XMS/DPMI/REDIR rows
to ABI and failure-field level, then reconcile each current provider and
workaround with a concrete retain/migrate/replace/delete action.
