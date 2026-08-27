# Project Status

## Current Work

**Active: M0 T289 S5 — DPMI stack, interrupt and fault composition.**

T286 is closed. Its final Bochs-core minimization evidence is in
[the closure record](history/m0-t286-closure-20260827.md). T287 admits the
queue-head original XMS owner package using a complete source-first plan.

## Active Packet

### M0 T289 S5 — DPMI stack, interrupt and fault composition

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T289 S5`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | S4 composes original initialization, descriptor and xmem bodies on formal x86/x64 local fixtures. |
| Objective | Compose original DPMI stack, interrupt and fault source groups through the existing atomic frame contract. |
| Non-goals | No `53:xx` route, DOSX host link, raw pointer, host LDT/IDT mutation, protected-mode transition provider, second executor, new mapping manager or `src.old` use. |
| Reference Baseline | [Wave 6](etc/operations/mvdm-package-recovery-wave-plan-001.md#wave-6-dpmi), `bop-list.md`, source policy and mapping-manager rule. |
| Files And ABI Surface | Exact `mvdm-host/{dpmi,dpmi32}` sources, DOSX artifacts, `adapter-mvdm-host-out`, `adapter-bochs`, session and the BOP/dependency tracker. |
| Applicable Rules | Architecture, coding, execution, source policy, mirror/overlay standard and mapping-manager rule. |
| Verification | Source identity before/after ledger, same-shaped binding matrix, original-body local cases on x86/x64, documentation governance and `git diff --check`. |
| Expected Markers | Original bodies retain their source order; no direct Bochs dependency enters an OpenNT mirror; no partial frame commit, LDT/IDT mutation, CPU executor or mapping manager is introduced. |
| Asset Needs | Canonical MVDM roots, current adapter inventory, session mapping rule and Bochs mechanical surface. |
| Reporting Requirements | Original-source identity, every binding/exception, x86/x64 result and each deliberately unavailable descriptor/memory branch. |
| Stop Conditions | A source body needs a semantic rewrite, direct Bochs call, raw pointer, host LDT or new mapper is proposed, a second CPU/product shell is selected, or a non-direct carrier is silently linked. |
| Exit Criteria | Admitted original stack/interrupt/fault bodies compile and preserve atomic source-defined failure outcomes; no BOP ingress is enabled. |
| Original Owner Request | “单人双角色模式，按照QUEUE.md规定的顺序，将全部队列任务执行完毕。” |
| Similar-Issue Sweep | DOSX artifacts, dpmi32 source groups, selector table/memory/stack/interrupt/fault forms, host pointers, Win32 imports, Bochs-facing calls, session leases and x86/x64 ABI forms. |

**T289 S4 closure:** original `dpmi32.c` initialization, `xmem.c` allocation
lifecycle, `dpmiselr.c` descriptor normalization and `dpmimemr.c` memory-info
bodies now compose on formal x86/x64 Ninja fixtures.  Session-owned numeric
resource identities replace only persisted x86 host-pointer coincidences;
checked copied protected spans replace transient aliases.  The historical
host-LDT final operation retains its original failure direction, and no
`53:xx` ingress or dispatcher is enabled.  See the [S4 evidence](etc/evidence/m0-t289-s4-dpmi-composition-001.md) and [ledger](etc/operations/m0-t289-s4-dpmi-composition-ledger.tsv).

**T289 S5 P1:** original `DpmiSetProtectedmodeInterrupt` and
`DpmiSetFaultHandler` now compose through a source-shaped current-worker
`VDM_TIB` and checked fixed `SS:SP` copies. Formal x86/x64 fixtures prove both
registrations and no-mutation invalid indexing; the entire selected S5 source
group also static-compiles on both architectures. S5 remains active: dispatch,
IRET and stack switching require a prevalidated atomic CS/SS/EIP/ESP/EFLAGS
transition rather than field-by-field setters. See [P1 evidence](etc/evidence/m0-t289-s5-p1-dpmi-registration-001.md)
and [ledger](etc/operations/m0-t289-s5-dpmi-stack-interrupt-disposition-ledger.tsv).

**T289 S3 closure:** `adapter-bochs` now exposes a selector-blind copied
protected-frame, copied active-segment inspection and checked protected-span
contract.  A frame commit is comparison-guarded, only changes GPR/EIP/native
FLAGS while the CPU loop is returned, and rejects CR0/mode/segment changes.
Formal x86 and x64 Ninja fixtures execute a real-to-protected transition and
prove valid commit, stale rejection and no-mutation invalid change rejection;
no DPMI source or `53:xx` route was selected. See [S3 evidence](etc/evidence/m0-t289-s3-protected-machine-foundation-001.md) and [ledger](etc/operations/m0-t289-s3-protected-machine-foundation-ledger.tsv).

**T288 S1 closure:** the reproducible union ledger records 78 selected source
files, all exact in both canonical OpenNT roots. It identifies the seven exact
keymouse files still under the temporary `mvdm-host` root for S2 re-rooting,
partitions 15 monitor/firmware interface groups by original owner and recovery
ladder, and transfers SIM16/WOW plus VDD/debugger-specific behavior without
inventing substitutes. See [S1 evidence](etc/evidence/m0-t288-s1-monitor-source-abi-audit-001.md).

**T288 S4 closure:** the source-shaped SAS scalar/span facade now composes
through checked `adapter-bochs` memory mechanics on formal x86 and x64 Ninja
fixtures. Its complete source disposition records the intentionally excluded
low-host-address mapper, raw-pointer forms, memory handler table and CCPU
descriptor stubs; no second allocator, mapper or CPU executor was added. See
[S4 evidence](etc/evidence/m0-t288-s4-softpc-sas-mechanics-001.md) and the
[S4 ledger](etc/operations/m0-t288-s4-softpc-sas-disposition-ledger.tsv).

**T288 S5 closure:** the source-shaped `UpdateKbdInt15` IVT 15h
compare/update/reset contract and its original initialization capture are now
locally proven on formal x86 and x64 Ninja fixtures. The full keyboard/IRQ and
CPU re-entry path is explicitly transferred rather than emulated. See [S5
evidence](etc/evidence/m0-t288-s5-keyboard-ivt-lifecycle-001.md) and the [S5
ledger](etc/operations/m0-t288-s5-keyboard-ivt-lifecycle-ledger.tsv).

**T287 S7/T287 closure:** the exact XMS source package is code-complete and
locally regression-closed. The corrected source audit records eleven exact
files and four registered minimal same-shaped modifications; it confirms the
sole session mapping-manager implementation, the sole same-shaped SoftPC
facade, and the explicit non-runtime direct-pointer backends. Successful UMB
and production keyboard/IVT work transfer to the renamed next queue candidate,
not to an invented XMS path. See [S7 closure evidence](etc/evidence/m0-t287-s7-xms-owner-package-closure-001.md).

**T287 S1 baseline note (superseded by S7):** the original source selection is
identical in both canonical roots. The final per-file SHA-256 ledger initially
overstated the current mirror identity; S7 corrects it to eleven exact files
and four registered minimal same-shaped modifications. The direct-address
`i386/xmsmem86.c` body remains exact but is not a product input on either
architecture; it has a named same-shaped successor in
`adapter-mvdm-host-out/softpc`. The remaining `bx-vdm` wording in the
historical tracker is evidence only, never a live input.

**T287 S2 closure:** the formal MSVC `/MT` Ninja graph rebuilt the six exact
common XMS units as `original-xms-common.lib` on x86 and x64.  Archive members
and SHA-256 manifests match the S1 ledger; `i386/xmsmem86.c` is excluded.
Historical pointer-width warnings remain explicit S4 binding debt, not a
source change or a runtime-success claim; see [S2 evidence](etc/evidence/m0-t287-s2-original-xms-static-composition-001.md).

**T287 S3 closure:** original `xmsa20.c` and the reached `xmsumb.c` binding
now retain AX:BX and A20 ordering while the matching private overlay holds
only numeric guest location data and acquires a fresh one-byte session lease
per write. Formal x86 and x64 `/MT` fixtures both pass valid location,
invalid location, lease-expiry and AX/BL-result checks. No `52:xx` route or
direct-address `xmsmem86.c` backend is enabled; see [S3 evidence](etc/evidence/m0-t287-s3-xms-a20-lease-binding-001.md).

**T287 S4 closure:** original `suballoc.c` and `xmsblock.c` now compose on
x86/x64 through same-shaped numeric XMS commit/decommit/move exports. The
only material source divergence is the matching private bounded-lease overlay
for the source `SS:BP-12` move structure. Formal fixtures cover allocation,
move, malformed input, reallocation/free and capacity/range failures; see
[S4 evidence](etc/evidence/m0-t287-s4-xms-block-binding-001.md).

**T287 S5 closure:** original `xmsumb.c` and `xmsmisc.c` now compile and run
locally on x86/x64. The original empty-physical-span `B1`/`B2` outcomes,
page-size/ext-memory results, and `UpdateKbdInt15(CS,AX)` then `CX` ordering
are proven without enabling a selector route or test-only success policy.
Successful UMB ownership transfers to the queued monitor substrate, and the
real keyboard IVT state algorithm transfers to its existing firmware plan;
see [S5 evidence](etc/evidence/m0-t287-s5-xms-umb-ivt-binding-001.md).

**T287 S6 closure:** exact original `xms.c:XMSInit` and `xmsdisp.c` now invoke
every `52:00..0B` table entry in a single local family matrix on formal x86 and
x64 `/MT` Ninja graphs. A20, move, allocation, free, reallocation,
page/free-memory, UMB and INT15 paths retain their original bodies and their
existing source-proven unavailable or transferred outcomes. No BOP ingress is enabled; see
[S6 evidence](etc/evidence/m0-t287-s6-xms-dispatch-matrix-001.md).

**T286 S1 closure:** the complete [divergence disposition ledger](etc/operations/m0-t286-s1-bochs-core-divergence-disposition-ledger-001.md) measures 29 differing upstream-relative mirror files (431 additions / 353 deletions) and 15 private overlay bodies (1,164 lines). It classifies all groups as build bindings, required private CPU/memory/device hooks, optional diagnostics, or B2/D4 external-composition candidates. No source behavior changed.

**T286 S2 closure:** CPU5 and host-pointer-width macros remain necessary generated Bochs configuration differences. Finite `bx_pc_system_c::Reset/exit` bodies require private Bochs state, so `minimal_product_shell.cc` moved from `adapter-bochs` to the matching private `bochs-core-overlay/pc_system_lifecycle.cc`; the mirror retains only its registered whole-method crop. The formal manifest's retired `session_input`/`session_cancellation` entries were also replaced by the actual neutral session sources before verification. The next stale pre-rebootstrap manifest input prevented graph generation; its exact limitation is recorded in the [S2 evidence](etc/evidence/m0-t286-s2-product-shell-overlay-001.md). No lifecycle semantics changed.

**T286 S3 closure:** all minimal port-space, PIC and headless-8042 bodies are confirmed private Bochs-state mechanics with no safe upstream no-product alternative or duplicate adapter lifecycle. The [S3 review](etc/evidence/m0-t286-s3-device-lifecycle-review-001.md) retains D1--D3 unchanged and transfers the only safe reduction opportunity to unconsumed CPU observers.

**T286 S4 closure:** six unconsumed default-off observer/gate files, every mirror call/declaration and all formal diagnostic switches are deleted. The [S4 evidence](etc/evidence/m0-t286-s4-observer-retirement-001.md) records the retained #UD/profile boundary and post-removal totals: 29 core diffs (393 additions / 356 deletions) and nine overlay bodies (978 lines). Full graph generation remains independently blocked by stale manifest roots.

**T286 S5 closure:** SIM-free allocation and checked ordinary/backing RAM remain the minimum private `BX_MEM_C` mechanics. `adapter-bochs` already owns only size/lifecycle selection and typed copied requests; it cannot safely own A20, MMIO/ROM vetoes, sparse vectors, handler tables or write stamps. The ordinary-RAM boundary fixture now follows the private checked-RAM body; see the [S5 evidence](etc/evidence/m0-t286-s5-memory-boundary-review-001.md).

**T285 S1 closure:** `New-T285S1Wave2CurrentWorkset.ps1` revalidated all 81
selected current mirror paths (22 DEM + 59 host-control), recorded their
SHA-256 identities and canonical adapter owners, and rejected stale root
names. Its disposition counts are 49 adapter-backed, five binding-only, six
overlay-required and 21 not-host-runtime. The complete original DEM static
package and the original `nt_bop.c` static package rebuilt on both x86 and
x64 after the formal Ninja recipes began loading the repository MSVC
environment. Historical compiler warnings remain evidence, not runtime
claims. See the [S1 evidence](etc/evidence/m0-t285-s1-wave2-current-workset-001.md).

**T285 S2 closure:** the exact original `nt_bop.c` has 21 reviewed external
contract groups in its [binding ledger](etc/operations/m0-t285-s2-nt-bop-binding-ledger.tsv).
All dynamic provider, host-control, monitor and machine-facing paths now have
one same-shaped declaration, adapter boundary, or explicit later owner with
its original unavailable behavior.  The only two missing declaration carriers
(`DbgBreakPoint`, `getPE`) are now source-shaped and selector-disabled.  The
formal original BOP static archive rebuilt on x86 and x64; see [S2
evidence](etc/evidence/m0-t285-s2-nt-bop-binding-001.md).


**T284 S1 closure:** the generated owner inventory records 33 final file/
boundary dispositions (22 production inputs, seven root documents and four
empty specialist families), 71 public callable declarations/carriers and all
seven MVDM host consumer directions. No production interface changed. See the
[S1 evidence](etc/evidence/m0-t284-s1-adapter-convergence-inventory-001.md)
and its three TSV inventories.

**T284 S2 closure:** all eight approved component roots, the required support
overlay and matching fixture roots now use their canonical names. Four
owner-authorized Win16 mirror directories moved and were staged with the
component. Active source/build/test/current-document scans find zero obsolete
root references; the formal x86/x64 original DEM/COMMAND carrier archives
both rebuild successfully. See the
[S2 evidence](etc/evidence/m0-t284-s2-component-root-rename-closure-001.md).

**T284 S3 closure:** `adapter-mvdm-host-out` now has the seven required named
family boundaries and no implementation source, build input or enabled
capability. The root's source-facing and Bochs-prohibition contract is fixed
in [S3 evidence](etc/evidence/m0-t284-s3-outgoing-adapter-root-admission-001.md).

**T284 S4 closure:** all nine audited Win32 headers/facades, together with
their two focused fixtures, now live under `adapter-mvdm-host-out/win32`.
The component root `adapter-win32` is absent. The formal x86/x64 original
COMMAND/DEM carriers, RTL support archive, and NT I/O facade fixture rebuild
without an interface or behavior change; see the
[S4 evidence](etc/evidence/m0-t284-s4-win32-family-migration-001.md).

**T284 S5–S8 closure:** the four actual SoftPC/CCPU/SAS façade sources and
five declaration carriers moved to `adapter-mvdm-host-out/softpc`; monitor's
two declaration carriers and two session-bound sources moved to `monitor`.
The empty Redirector, WOW, VDD and debugger roots merged into their named
outgoing families. Twelve focused x86/x64 fixture executables passed. The
intermediate result, including the unselected historical T260 graph debt, is
in the [T284 closure record](history/m0-t284-closure-20260826.md).

**T284 S9 closure:** the two selected guest carries are now one load-only
`mvdm-guest` component with `dos/` and `win16/` interior roots. All 676 DOS,
44 WOW16 and 10 bin86 source paths remain hash-identical; eight selected
Win16 driver artifacts are hash-identical to their `build/output/wow16`
sources. No host link input selects the guest component. See the
[S9 evidence](etc/evidence/m0-t284-s9-mvdm-guest-root-merge-001.md).

The detailed execution plans are [S1 original COMMAND package source surface](etc/operations/m0-t282-s1-original-command-package-surface-plan-001.md), [S2 COMMAND declaration carriers](etc/operations/m0-t282-s2-command-declaration-carrier-plan-001.md), [S3 COMMAND console declaration carrier](etc/operations/m0-t282-s3-command-console-declaration-carrier-plan-001.md), [S4 complete original COMMAND static package](etc/operations/m0-t282-s4-complete-original-command-static-package-plan-001.md), [S5 VDM command-acquisition source audit](etc/operations/m0-t282-s5-vdm-command-acquisition-source-audit-plan-001.md), [S6 local VDM command transport](etc/operations/m0-t282-s6-local-vdm-command-transport-plan-001.md), [S7 original COMMAND initialization composition](etc/operations/m0-t282-s7-original-command-initialization-plan-001.md), [S8 COMMAND owner dependency partition](etc/operations/m0-t282-s8-command-owner-dependency-partition-plan-001.md), [S9 COMMAND environment guest-memory seam audit](etc/operations/m0-t282-s9-command-environment-guest-seam-audit-plan-001.md), [S10 COMMAND register and bounded-lease binding](etc/operations/m0-t282-s10-command-register-and-lease-binding-plan-001.md), [S11 COMMAND OEM support composition](etc/operations/m0-t282-s11-command-oem-support-composition-plan-001.md), and [S13 COMMAND current-directory bounded mirror binding](etc/operations/m0-t282-s13-command-current-directory-bounded-binding-plan-001.md).

**T282 S1 P1:** both static-only architecture probes reached the same original
`vdmapi.h` declaration boundary. The verified 11-unit/17-service ledgers and
[surface evidence](etc/evidence/m0-t282-s1-command-source-surface-001.md)
record the exact original carriers and successor owners; no route was enabled.

**T282 S2 P1:** byte-exact original `vdmapi.h` and `pif.h` are now registered
in the platform-ABI manifest. Focused x86/x64 static probes compile original
`cmdexec.c` and `cmdpif.c` through those carriers. The whole package remains
selector-disabled; `cmdkeyb.c` stops at the separately-owned private
`winconp.h` declaration boundary.

**T282 S2 closure:** both admitted declaration carriers are hash-proven and
selected on x86/x64 without adding any behavior. `GetNextVDMCommand` remains
for the session/monitor binding package; PIF behavior and the `winconp.h`
console-private boundary remain later named work. See the [S2 evidence](etc/evidence/m0-t282-s2-command-declaration-carriers-001.md).

**T282 S3 P1:** byte-exact `winconp.h` is registered in the platform-ABI
manifest and selected through an adapter declaration carrier. Original
`cmdkeyb.c` compiles on x86/x64; keyboard service, registry, console policy
and guest-memory behavior remain unenabled.

**T282 S3 closure:** the complete admitted console declaration carrier is
hash-proven and both architecture probes pass without source-body changes.
The remaining keyboard provider policy belongs to its later COMMAND/session
binding work. See the [S3 evidence](etc/evidence/m0-t282-s3-command-console-declaration-carrier-001.md).

**T282 S4 P1:** both formal MSVC graphs archive the exact eleven original
COMMAND units as `original-command-surface.lib`; archive member and graph
verification reject runtime ingress, executable and Bochs inputs.

**T282 S4 closure:** the complete original COMMAND source package has a
source-identity-preserving static archive on x86/x64. Callable session,
monitor, stream, PIF and child-lifecycle boundaries remain unresolved by
design; no `54:xx` route or provider is enabled. See the [S4 evidence](etc/evidence/m0-t282-s4-complete-original-command-static-package-001.md).

**T282 S5 P1:** the original client/BaseSrv command-acquisition contract is
now partitioned into seven source/ABI slices. Single-session DOS request,
re-entry and retry semantics have a named same-shaped session/monitor owner;
CSR first-VDM and WOW/broker branches remain explicit later owners.

**T282 S5 closure:** no callable behavior is enabled. The next implementation
may recover only the one-session copied request/response binding while
preserving original size, retry and failure semantics. See the [S5 evidence](etc/evidence/m0-t282-s5-vdm-command-acquisition-audit-001.md).

**T282 S6 P1:** the one-session source-shaped `GetNextVDMCommand` transport
now carries only a copied app record through a bound session dispatch and
retains bounded copies, required-size failures, environment-only response,
re-entry operations and the terminal no-command outcome. The focused formal
x86/x64 Ninja fixtures pass with no BOP route, Bochs input or historical CSR
composition.

**T282 S6 closure:** the local DOS request/response branch is closed as an
adapter-backed original contract. First-VDM, BaseSrv/CSR wait/broker, PIF and
WOW branches remain explicit non-routes, not fallback behavior. See the
[S6 evidence](etc/evidence/m0-t282-s6-local-vdm-command-transport-001.md).

**T282 S7 P1:** exact original `cmd.c` and `cmddata.c` now compose directly
as one initialization slice on x86/x64. The fixture verifies the original
globals and `CMDInit` home-drive update; the only diagnostics are the retained
K&R and unused-parameter warnings from the unchanged original source.

**T282 S7 closure:** this initialization slice is directly source-complete;
it does not enable COMMAND dispatch or any guest route. See the [S7 evidence](etc/evidence/m0-t282-s7-original-command-initialization-001.md).

**T282 S8 P1:** all 17 original COMMAND dispatcher services and 13 reached
helper groups now have one source/ABI/recovery disposition in the verified
32-row owner ledger. The resulting order chooses an environment/current-
directory guest-query group next, not a trace-hit leaf service.

**T282 S8 closure:** no implementation was enabled. The partition gives every
reached COMMAND dependency a named owner and legal successor boundary; see
the [S8 evidence](etc/evidence/m0-t282-s8-command-owner-dependency-partition-001.md).

**T282 S9 P1:** all selected COMMAND guest accesses are reduced to four
source-backed records. The existing session `guest_memory` mapping-manager
instance and adapter-mvdm-host-out/softpc bounded leases are the sole legal route.

**T282 S9 closure:** the later group may use only exact, synchronous leases:
64-byte COMSPEC input, 67-byte current-directory output and source-calculated
initial-environment output. Naked `GetVDMAddr` remains forbidden; see the
[S9 evidence](etc/evidence/m0-t282-s9-command-environment-guest-seam-audit-001.md).

**T282 S10 closure:** the reached original `softpc.h` COMMAND register imports
now bind through source-shaped adapter-mvdm-host-out/softpc functions to selector-blind
adapter-bochs mechanics. The formal x86/x64 fixture and real adapter-bochs
source compiles pass; guest memory remains S9's exact bounded lease. See the
[S10 evidence](etc/evidence/m0-t282-s10-command-register-and-lease-binding-001.md).

**T282 S11 closure:** exact original OEM process/current-directory helper
bodies compose and execute on x86/x64 through the existing adapter-mvdm-host-out/win32 RTL
facade and public Kernel32/GDI32 imports. Unreached historical pointer-width
branches remain separately tracked; no COMMAND mirror body changed. See the
[S11 evidence](etc/evidence/m0-t282-s11-command-oem-support-composition-001.md).

**T282 S12 P1:** bounded C-string read and exact host-to-guest write are now
adapter-mvdm-host-out/softpc operations over the sole session guest-memory lease. The formal
x86/x64 fixture covers positive and negative spans; COMMAND mirror source is
still unchanged. See the [S12 P1 evidence](etc/evidence/m0-t282-s12-bounded-copy-binding-001.md).

**T282 S12 closure:** the source-independent bounded C-string and exact-write
forms are verified on x86/x64. The next packet may bind only a source-backed
span with an explicit original failure disposition; S13 admits current
directory output alone.

**T282 S12 P2:** the same reached original register boundary now includes
`getAL`/`setAX`, required by `cmdGetCurrentDir`; the x86/x64 regression passes.

**T282 S13 closure:** the original `cmdGetCurrentDir` now retains its source
drive/environment order and CF/AX contract while replacing only the unsafe
`DS:SI` alias with S12's exact synchronous descriptor write. The original
COMMAND source archive and bounded-copy fixture pass on x86/x64. This is not
a callable `54:04` claim; see the [S13 evidence](etc/evidence/m0-t282-s13-command-current-directory-bounded-binding-001.md).

**T282 S14 closure:** the original `cmdComSpec` has no observable service
failure branch for a malformed/unmappable COMSPEC string; the bounded
descriptor correctly rejects it but cannot choose AL/CF/stop semantics. No
mirror body changed and `54:02` remains disabled; see the [S14 audit](etc/evidence/m0-t282-s14-command-comspec-bounded-input-audit-001.md).

**T282 S15 closure:** original `cmdGetInitEnvironment` preserves an explicit
`BX` shortage result but has no result for a post-capacity guest-lease fault.
No mirror body changed or `54:0F` route was enabled; see the [S15 audit](etc/evidence/m0-t282-s15-command-init-environment-bounded-output-audit-001.md).

**T282 S16 closure and T282 closure:** the full 32-row original COMMAND
ledger has no ownerless or directly recoverable local source item remaining.
T282 closes as source-composition work only; see the [S16 audit](etc/evidence/m0-t282-s16-command-local-package-closure-audit-001.md) and [T282 closure](history/m0-t282-closure-20260827.md).

**T283 S1/S2 P1:** the 29-header platform-ABI inventory and selection ledger
classify 18 one-sided, three byte-identical dual-source and eight conflict
records. All selected declarations are now hash-proven under one logical
`source/public` or `source/private` topology; live source/build/test inputs no
longer name edition roots. Formal x86/x64 Ninja carrier archives passed. See
the [S2 evidence](etc/evidence/m0-t283-s2-platform-abi-canonical-reroot-001.md).

### Closed packet reference — M0 T281 S5 — original host-control package disposition

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T281 S5`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | T281/S4 is complete as `252c2a81`; S5 applies the existing complete 56-source host/src owner audit to form the entire ordered control-plane recovery partition. |
| Objective | Give every original `softpc.new/host/src` control-plane source and interface one final near-term disposition: eligible static composition group, explicit existing-adapter prerequisite, later specialist owner package, or Bochs-replacement exclusion. |
| Non-goals | Compiling an unaudited all-host mega-library, enabling BOP selectors, reviving a CCPU/SoftPC executor, adding a generic shim, executing a guest trace, changing a mirror body, or treating one compiler diagnostic as a leaf feature task. |
| Reference Baseline | T276 S12 complete six-family owner ledgers, T277 package-cycle ledger, T280 ingress/machine-facade ledger, T281/S1–S4 DEM/static control evidence, and the Wave 2 plan. |
| Files And ABI Surface | Original `softpc.new/host/src` source and six T276 S12 owner ledgers; adapter-mvdm-host-out/win32, adapter-mvdm-host-out/softpc, adapter-mvdm-host-out/monitor, adapter-mvdm-host-in, session, app, adapter-mvdm-host-out/vdd and adapter-mvdm-host-out/debugger boundary records; S5 consolidation ledger/evidence only. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component, build-hygiene and recovery-ladder rules. |
| Verification | Every host/src source appears once in the consolidation ledger; disposition cross-checks the six existing detailed ledgers; no new implementation input is selected; documentation governance and `git diff --check`. |
| Expected Markers | Explicit recovery groups and prerequisite adapters; device/executor exclusions remain separate; no source body is activated by the audit. |
| Asset Needs | Pinned original mirror, T277/T280 ledgers, existing component headers, MSVC x64/x86 and Ninja. |
| Reporting Requirements | Report per-import source form, owner/binding, architecture result, compile/link state, excluded branch and any boundary that requires a re-admission. |
| Stop Conditions | Any row lacks an existing tracker owner, a proposed group would mix Bochs device/executor semantics with MVDM control semantics, or a new adapter is assumed without a named source interface. |
| Exit Criteria | Met: all 56 sources are assigned once to an ordered recovery group or explicit non-eligibility owner and each prerequisite is named. See [M0 T281 closure](history/m0-t281-closure-20260826.md). |
| Original Owner Request | “我都叫你按顺序执行了”。 |
| Similar-Issue Sweep | All six T276 S12 owner families, every host/src path, startup/termination/event/monitor/BOP forms, and all machine/device/external product exclusions. |

**T281 S1 P1:** `558a9626` selected and imported exact OpenNT `vdm.h`, made the three historical function-pointer conversions explicit under `MVDM-HOST-DIV-008`, and proved a selector-disabled original `nt_bop.c` static-library island on x86 and x64. The dispatcher is not linked into a runtime image and no selector/provider was enabled.

**T281 S2 P1:** the formal x86/x64 Ninja islands now compile and archive the
four original DEM common-control units as `original-dem-common-control.lib`.
The graph retains original `WIN_32`/`DEVL` declaration form, uses only existing
same-shaped declaration carriers, and is verifier-proven to exclude provider,
ingress, executable, and Bochs source inputs. No `50:xx` route is enabled.

**T281 S2 closure:** the admitted four-file common-control group has met every
exit criterion. Its scope is closed as a static source composition island;
provider bodies are explicitly transferred to S3, not treated as an unplanned
trace-derived backlog.

**T281 S3 P1:** the complete remaining twelve-unit original DEM provider
matrix now compiles and archives on x86 and x64 as
`original-dem-providers.lib`. The existing same-shaped
`adapter-mvdm-host-out/monitor/include/vdm.h` declaration carrier closes the only
matrix-specific declaration gap (`NtVdmControl(VdmQueryDir, ...)`) in
`demsrch.c`; its implementation body is excluded. The archive is
selector-disabled and does not invoke a provider or enable `50:xx`.

**T281 S3 closure:** every remaining provider unit has its x86/x64 source
composition disposition and the only matrix-specific declaration gap is closed
by an existing, implementation-excluded same-shaped boundary. Full-package
archive composition transfers to S4.

**T281 S4 P1:** x86 and x64 now each archive exactly the sixteen original DEM
objects as `original-dem-package.lib`. The archive-member verifier proves the
complete source set and rejects executables, runtime rules, BOP ingress,
Bochs, `nt_bop.c`, and the VDM control implementation body.

**T281 S4 closure:** the full original DEM package now has a single
source-identity-preserving static archive per architecture. Runtime dispatch,
provider invocation and host control composition remain deliberately outside
that archive and transfer to the host-control package sequence.

**T281 S5 P1:** the reproducible consolidation ledger now assigns all 56
original `host/src` paths exactly once: 23 `adapter-prerequisite`, 18
`bochs-replacement`, 15 `specialist-owner`, and zero `eligible-next`. The
generator and verifier prove unique source/owner records; no source body,
selector, provider, machine executor, or guest route was enabled. See the
[S5 disposition evidence](etc/evidence/m0-t281-s5-host-control-disposition-001.md).

The detailed execution plans are [S1 original BOP dispatch composition](etc/operations/m0-t281-s1-original-bop-dispatch-composition-plan-001.md), [S2 original DEM common-control composition](etc/operations/m0-t281-s2-original-dem-common-control-composition-plan-001.md), [S3 original DEM provider source composition](etc/operations/m0-t281-s3-original-dem-provider-source-composition-plan-001.md), [S4 complete original DEM static package composition](etc/operations/m0-t281-s4-complete-original-dem-static-package-composition-plan-001.md), and [S5 original host-control package disposition](etc/operations/m0-t281-s5-original-host-control-package-disposition-plan-001.md).

### Closed packet reference — M0 T280 S20

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T280 S20`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | S19 proved that original `demgset.c` retains native GetVDMAddr results beyond a legal lease lifetime, blocking a raw-pointer facade. P7 then proved SFT/JFT requires a distinct VDD shadow lifecycle; the owner approved its source-shaped P8 recovery. |
| Objective | Recover the reached DEM persistent guest-location globals as numeric descriptors and fresh per-dereference guest-memory leases. **Closed.** |
| Non-goals | No generic GetVDMAddr raw-pointer shim, DEM provider/BOP route, guest trace, directory backend, new mapping-manager implementation, Bochs change, broker value or async guest-pointer retention. P8 may use the existing session `host_resource` instance only to own host-local SFT shadow/HANDLE identities. |
| Reference Baseline | Original demgset/demsrch consumers, S19 lease ledger/evidence, session guest-memory API and adapter-mvdm-host-out/softpc/adapter-bochs ownership rules. |
| Files And ABI Surface | Reached original DEM globals/consumers, adapter-mvdm-host-out/softpc source-shaped descriptor/lease boundary, focused fixtures/evidence and Status. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component, build-hygiene and recovery-ladder rules. |
| Verification | All reached global assignments/uses are inventoried; descriptor persistence, x86/x64 range rejection, lease release and no-native-pointer-retention tests pass. |
| Expected Markers | Persistent guest locations remain numeric; every actual dereference uses a fresh exact read/read-write lease and no source global is a native pointer. |
| Asset Needs | S19 ledger, original DEM source/layouts, current session lease API, adapter-mvdm-host-out/softpc/adapter-bochs contracts and formal Ninja/MSVC matrix. |
| Reporting Requirements | Original global/caller, numeric descriptor layout, lease extent/access/lifetime, source failure preservation, all consumer coverage and remaining provider limit. |
| Stop Conditions | A descriptor is tokenized, a host pointer/HANDLE enters MVDM global state, a lease crosses an async callback, a generic mapper is created, or a provider/BOP route is enabled. |
| Exit Criteria | Met: every reached retained guest pointer is removed/recovered as a numeric descriptor with focused x86/x64 evidence; no actual guest pointer survives its source callback. See [M0 T280 S20 closure](history/m0-t280-s20-closure-20260826.md). |
| Original Owner Request | “单人双角色模式执行队列任务”。 |
| Similar-Issue Sweep | All demSetDTALocation globals, every reached dereference in demgset/demsrch/demfile/demfcb, DTA/PDB paths, descriptor arithmetic, lease cleanup and invalid/stale guest address behavior. |

P1 inventory is recorded in the [persistent-location ledger](etc/operations/m0-t280-s20-dem-persistent-guest-location-ledger-001.tsv).
P2 fixes the numeric descriptor, packed-layout and lease conversion contract in
the [descriptor/lease design](etc/operations/m0-t280-s20-p2-descriptor-layout-and-lease-design-001.md).
P3 has independently verified the source-facing bounded lease facade on x86
and x64 in [fixture evidence](etc/evidence/m0-t280-s20-p3-mvdm-guest-location-fixture-001.md).
P4 has converted only the extended-error retained location and records its
source/layout evidence in [extended-error recovery](etc/evidence/m0-t280-s20-p4-extended-error-location-recovery-001.md).
P5 has removed the reached DTA retained pointer and records its focused
source/fixture evidence in [DTA recovery](etc/evidence/m0-t280-s20-p5-dta-location-recovery-001.md).
P6 has removed the reached current-PDB pointer across DEM and selected SoftPC
consumers; its source recovery and transferred product-shell interface gaps
are recorded in [PDB recovery](etc/evidence/m0-t280-s20-p6-current-pdb-source-recovery-001.md).
P7 proves that SFT/JFT is a distinct VDD host-buffer lifecycle boundary, not
a legal scalar lease conversion; see the [SFT boundary audit](etc/operations/m0-t280-s20-p7-sft-chain-boundary-audit-001.md).
P8 recovers that boundary as one session-owned, source-shaped VDD SFT/JFT
shadow/commit contract; x86/x64 evidence is in [SFT/JFT recovery](etc/evidence/m0-t280-s20-p8-vdd-sft-jft-shadow-recovery-001.md).

**S20 closure:** the extended-error, DTA, current-PDB and SFT-chain retained
guest locations now preserve numeric source data and use only fresh bounded
leases or bounded synchronous shadows. The historical raw-pointer body remains
inactive provenance; no provider or BOP route is enabled.

### Closed packet reference — M0 T280 S13

**S13 closure:** the shared NT/RTL declaration surface makes all three original
DEM bodies syntax-clean on x86/x64; remaining host identity casts are
explicitly transferred to S14. See [S13 closure](history/m0-t280-s13-closure-20260826.md).

### Closed packet reference — M0 T280 S14

**S14 closure:** original `demsrch.c` persisted find-list identity is now
classified as a session `host_resource` surrogate, distinct from DTA/FCB
`guest_memory` leases. The existing scoped session binding and the missing
adapter-mvdm-host-out/softpc identity facade are explicit successor evidence; no DEM mirror
body or provider was enabled. See [S14 closure](history/m0-t280-s14-closure-20260826.md).

### Closed packet reference — M0 T280 S15

**S15 closure:** `adapter-mvdm-host-out/softpc` now exposes the one bounded host-resource
surrogate facade over the thread-bound session. Its x86/x64 fixture proves
identity, stale-token, isolation and disposal outcomes; it has no
guest-memory route. The original DEM mirror remains unchanged. See
[S15 closure](history/m0-t280-s15-closure-20260826.md).

### Closed packet reference — M0 T280 S16

**S16 closure:** original DOS DTA/FCB find fields now retain the exact x86
layout on both architectures and persist only session `host_resource`
surrogates. Formal x86/x64 source and layout evidence passed; remaining
unrelated `demsrch.c` width warnings transfer to S17 as one source audit. See
[S16 closure](history/m0-t280-s16-closure-20260826.md).

### Closed packet reference — M0 T280 S17

**S17 closure:** all remaining `demsrch.c` x64 warning/cast groups have a
source-owner, value-class, mapping and follow-on disposition. Host-local
directory state is not tokenized; only the separately bounded `GetVDMAddr`
family is a future guest-memory lease. See
[S17 closure](history/m0-t280-s17-closure-20260826.md).

### Closed packet reference — M0 T280 S18

**S18 closure:** all selected host-local `demsrch.c` x64 groups compile on
x86/x64 without width diagnostics, and the same-shaped VDM control family now
belongs solely to `adapter-mvdm-host-out/monitor`. The guest `GetVDMAddr` boundary
remains deliberately unimplemented for S19 source-first audit. See
[S18 closure](history/m0-t280-s18-closure-20260826.md).

### Closed packet reference — M0 T280 S19

**S19 closure:** `demsrch.c` guest accesses are fully classified as bounded
lease candidates, and the retained pointer bug is traced to
`demSetDTALocation` across owner files. S20 must recover numeric descriptors
before any raw source-facing guest pointer can exist. See
[S19 closure](history/m0-t280-s19-closure-20260826.md).

### Closed packet reference — M0 T280 S12

**S12 closure:** all three original reached DEM bodies have x86/x64 object
observations; shared NT/RTL declarations and the distinct find-list identity
boundary are classified without enabling a provider. See
[S12 closure](history/m0-t280-s12-closure-20260826.md).

### Closed packet reference — M0 T280 S11

**S11 closure:** one source-shaped adapter-mvdm-host-out/win32 NT I/O facade parses original
`dem.h`, compiles, links and runs on x86/x64. It preserves the original
32-bit status record at the host-local `ntdll` boundary and has no guest-memory
mapping route. See [S11 closure](history/m0-t280-s11-closure-20260826.md).

### Closed packet reference — M0 T280 S10

**S10 closure:** the 11-row reachable `ntioapi` declaration/call ledger and
source-first implementation plan are complete and verified. The exact original
carrier remains unchanged; host-local NT records and handles are explicitly
outside every mapping-manager instance. See
[S10 closure](history/m0-t280-s10-closure-20260826.md).

### Closed packet reference — M0 T280 S9

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T280 S9`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | S8 closed in `c3359863`; S7/S8 prove a declaration-only, same-shaped binding is the next smallest recovery rung. |
| Objective | Make the exact original storage declaration carrier visible through adapter-mvdm-host-out/win32 so original DEM header form can parse without selecting a modern SDK substitute. |
| Non-goals | No DEM body build/link, provider/BOP route, host API implementation, SoftPC mapping call, private NT import, guest trace or generic shim. |
| Reference Baseline | T280 S7 source-form ledger, S8 exact import manifest, existing `devioctl.h` mirror and adapter-mvdm-host-out/win32 `nt.h` facade. |
| Files And ABI Surface | `adapter-mvdm-host-out/win32/include/nt.h`, adapter README, S9 plan/fixture/verifier and Status. No behavior ABI is introduced. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component and recovery-ladder rules. |
| Verification | Focused x86/x64 syntax fixture includes original `dem.h`, verifies source identity of exact carriers and rejects a DEM body/provider/mapping input. |
| Expected Markers | Original `devioctl.h` supplies `DEVICE_TYPE`; original nested `ntddstor.h` is selected; original `dem.h` parses. |
| Asset Needs | S8 exact mirrors, MSVC x86/x64, current adapter-mvdm-host-out/win32 facade and original `dem.h`. |
| Reporting Requirements | Smallest declaration divergence, architecture results, hash/provenance proof, and explicit no-behavior/no-mapping limitation. |
| Stop Conditions | A modern storage header is substituted, an original carrier is edited, a DEM body is admitted, a private API is called, or mapping manager code appears. |
| Exit Criteria | Both architectures pass the declaration-only fixture with exact source carriers; README registers the facade-only divergence; no source body is built. |
| Original Owner Request | “单人双角色模式执行队列任务”。 |
| Similar-Issue Sweep | Include ordering, nested original headers, `DEVICE_TYPE` source, modern SDK collisions, adapter divergence register and mapping isolation. |

**S9 closure:** formal Ninja x64/x86 syntax fixtures select exact
`devioctl.h`, `ntdddisk.h` and `ntddstor.h` through the existing adapter-mvdm-host-out/win32
facade and parse original `dem.h`. The sole divergence removes a modern
same-name macro collision. Full `ntioapi.h` is explicitly not included: it
requires a whole same-shaped NT I/O ABI package, not an unreviewed facade
expansion. No DEM body, provider, host API call or mapping operation is added.

### Closed packet reference — M0 T280 S8

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T280 S8`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | S7 closed in `ffe95bcf`; it selected exact OpenNT `ntdddisk.h`, `ntddstor.h` and `ntioapi.h` as the first recovery rung. |
| Objective | Import the three byte-identical original declaration carriers with complete provenance and no body/provider admission. |
| Non-goals | No original DEM edit/link, adapter behavior, Win32/NT API call, SoftPC mapping implementation, BOP route, guest trace or private kernel import. |
| Reference Baseline | T280 S7 five-family ledger, source policy, existing platform ABI import manifest and pinned OpenNT source. |
| Files And ABI Surface | Three exact platform declaration files, import ledger/manifest, plan, Status and documentation inventory. No product ABI behavior is introduced. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component and recovery-ladder rules. |
| Verification | Verify byte hashes against pinned sources, source/destination manifest entries, required include roots and no source-body/binding change. |
| Expected Markers | Exact DDK storage carrier pair, exact SDK NT IO carrier and reproducible source provenance. |
| Asset Needs | Pinned OpenNT public DDK/SDK headers and existing `mvdm-platform-abi` source root. |
| Reporting Requirements | Exact source/destination/hash, direct consumer evidence, retained declaration-only role and no-mapping/no-provider limitation. |
| Stop Conditions | A carrier is edited, a modern SDK header substitutes for it, a body is enabled, a host pointer is mapped, or an adapter implementation is smuggled into the import. |
| Exit Criteria | Three files are byte-identical and manifest-backed; all are declaration-only; no DEM source body or provider is built/linked. |
| Original Owner Request | “单人双角色模式执行队列任务”。 |
| Similar-Issue Sweep | Nested original headers, source-tree provenance, existing platform ABI destination paths, host-local record ownership and mapping-manager isolation. |

**S8 closure:** exact OpenNT `ntdddisk.h`, `ntddstor.h` and `ntioapi.h` are
now imported under the platform ABI mirror with source path/hash/consumer
manifest evidence. They are declaration-only carriers. This does not compose
a DEM body, bind a historical NT call, create a mapping-manager route or
enable any BOP.

### Closed packet reference — M0 T280 S7

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T280 S7`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | S6 closed in `43d26f8d`; its whole-package evidence requires one source-first ABI recovery design, not a trace-selected implementation. |
| Objective | Recover exact source/header/compile-condition evidence and one owner disposition for the five shared DEM ABI families before any declaration or adapter body is added. |
| Non-goals | No original DEM edit/link, provider/BOP route, adapter implementation, private NT call, generic compatibility layer, guest trace or new mapping manager. |
| Reference Baseline | T280 S4 include ledger, S5 VDM directory-control adapter, S6 compile-surface ledger/evidence, exact OpenNT `dos/dem`, `inc`, SDK and SoftPC sources. |
| Files And ABI Surface | S7 source/consumer/shape/failure ledger, plan, Status and documentation inventory only. No product ABI is introduced. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component, build-hygiene and recovery-ladder rules. |
| Verification | Compare every cited spelling/form/condition with the exact original source; verify five unique owner dispositions and mapping-manager isolation; run documentation governance and diff checks. |
| Expected Markers | `DEVL` source form, x86 `GetDiskSpaceInformationA` collision, NT directory/storage declarations, DASD storage boundary and source-proven SoftPC guest-memory operations. |
| Asset Needs | Original OpenNT/OpenNT-4.5 source evidence, current exact mirrors, adapter-mvdm-host-out/win32/session/adapter-mvdm-host-out/softpc contracts and S6 source logs summarized in evidence. |
| Reporting Requirements | Original path/line/shape, DEM consumers, architecture difference, modern availability, failure behavior, future owner and mapping disposition per family. |
| Stop Conditions | A missing shape is guessed, a body is enabled, a host-local pointer enters a mapping manager, a private kernel VDM path is invoked, or a family-private mapper is proposed. |
| Exit Criteria | A five-family source-first ledger identifies the smallest exact-mirror, same-shaped adapter or adapter-mvdm-host-out/softpc follow-on package; no unresolved source definition remains. |
| Original Owner Request | “单人双角色模式执行队列任务”。 |
| Similar-Issue Sweep | Historical build macro forms, x86 public SDK name collisions, NT data-layout sources, host versus guest pointer classification, SoftPC/CCPU mechanical names and all future consumers. |

**S7 closure:** five source-shaped families now have original source paths,
historical compile conditions, consumer sets, current modern findings, unique
future owner and mapping disposition. The next admissible implementation is
one shared package: exact historical declaration carriers where available,
same-shaped adapter-mvdm-host-out/win32 bindings for modern collisions, and one complete
adapter-mvdm-host-out/softpc guest-memory lease surface. No original DEM translation unit is
enabled by this closure.

### Closed packet reference — M0 T280 S6

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T280 S6`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | S5 completed and was pushed in `3cddbc0f`; the active owner goal requires a package-led, non-trace-led MVDM recovery. |
| Objective | Derive one complete, source-proven next adapter/declaration workset from a reproducible syntax survey of all sixteen original DEM bodies. |
| Non-goals | No body enablement/edit, provider or BOP route, private NT import, guest trace, Bochs modification, or one-off per-BOP workaround. |
| Reference Baseline | T280 S1 workset, S4 include ledger, S5 VDM control boundary and original `dos/dem/sources`. |
| Files And ABI Surface | S6 plan, 16-row source ledger, formal survey/verifier and Status. No product ABI is introduced. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component, build-hygiene and recovery-ladder rules. |
| Verification | MSVC `/Zs /MT /W4` survey for x64/x86 under a disposable run root; verify all selected bodies occur once in the ledger; review each owner/mapping disposition. |
| Expected Markers | Whole-package source selection; original DEM build-form globals; historical NT directory/storage ABI families; explicit `adapter-mvdm-host-out/softpc` guest-memory mapping boundary. |
| Asset Needs | Existing exact host/support/platform mirrors, adapter-mvdm-host-out/win32/session contracts, MSVC x86/x64 and original sources evidence. |
| Reporting Requirements | Architecture-specific syntax outcome, exact blocker families, unique owner, host-local versus guest-numeric classification, and next package disposition. |
| Stop Conditions | A source body is enabled from syntax success, a private NT kernel call is attempted, a new family-private mapper appears, or a compiler error is patched without whole-package classification. |
| Exit Criteria | Both architecture surveys and the 16-row owner/mapping ledger are verified; next implementation is a bounded shared ABI package, not a trace-selected leaf. |
| Original Owner Request | “单人双角色模式执行队列任务”。 |
| Similar-Issue Sweep | DEM globals/precompiled header assumptions, NT directory/device declaration families, DASD boundaries, CCPU/SAS mechanics, and mapping-manager isolation. |

**S6 closure:** the sixteen exact DEM bodies were syntax-surveyed on x64 and
x86.  The source/ABI ledger reduces every observed blocker to five shared
owner families: original DEM declaration form, one x86 SDK declaration
collision, NT directory/device record forms, DASD public storage forms, and
adapter-mvdm-host-out/softpc guest mechanics.  Syntax-clean bodies are only original-body
candidates—not linked providers.  The next admission must address one shared
source-shaped compatibility package; it may not patch a trace-selected BOP.

### Closed packet reference — M0 T280 S5

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T280 S5`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | S4 closed in `33265acd`; recover only the source-proven `demsrch.c` private VDM declaration/control boundary. |
| Objective | Provide same-shaped `vdm.h` declarations and a session-bound `NtVdmControl(VdmQueryDir, VDMQUERYDIRINFO)` adapter contract, plus behavior-free `winbasep.h` include preservation. |
| Non-goals | No original-body edit, full NT4 kernel VDM service, directory-query backend, BOP enablement, DEM body link, app link, guest trace, second executor, or private product-shell import. |
| Reference Baseline | T280/S4 gap evidence, OpenNT `vdm.h`, `ntos/vdm/vdm.c`, selected `demsrch.c`, session lifecycle and adapter-mvdm-host-out/win32 public facade. |
| Files And ABI Surface | Reached VDM declarations, session generic synchronous control seam, `NtVdmControl` adapter and focused contract fixture. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component, build-hygiene and recovery-ladder rules. |
| Verification | Original enum/structure/function spelling is available; only `VdmQueryDir` reaches a bound session handler; no handler and all other VDM services return `STATUS_NOT_IMPLEMENTED`; no host-local control pointer enters guest mapping. |
| Expected Markers | Same-shaped `VDMQUERYDIRINFO`; session synchronous control callback; behavior-free `winbasep.h`; focused no-session/unregistered/registered/unsupported fixture. |
| Asset Needs | Existing session component, adapter-mvdm-host-out/win32 facade, public Windows types and OpenNT source evidence. |
| Reporting Requirements | Source semantics, bounded modern divergence, session ownership, negative capability outcomes and formal x86/x64 test results. |
| Stop Conditions | A full private VDM/Base header is imported, an unregistered call succeeds, a raw host pointer is mapped as guest identity, a kernel VDM call is attempted, or a DEM body is enabled. |
| Exit Criteria | Same-shaped reached ABI and session route compile/test on x86/x64; all unimplemented service classes fail explicitly; no provider/backend is falsely claimed. |
| Original Owner Request | “单人双角色模式执行队列任务”。 |
| Similar-Issue Sweep | DEM vs COMMAND ownership, host-control executors vs façades, overlay transport seams, session/mapping boundary, known adapter ownership, and non-runtime historical product shells. |

### Closed packet reference — M0 T279 S4

### M0 T279 S4 — support-package closure audit

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T279 S4`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | T279/S3 is complete and pushed as `7679ae9c`; audit the complete support package before T279 closure. |
| Objective | Prove that all 112 exact support paths retain their correct source/build role, both original libraries have bounded x86/x64 build evidence, and no provider/BOP/guest route was accidentally enabled. |
| Non-goals | No new provider, adapter family, BOP, guest runtime, suballoc callback binding, or host-control implementation. |
| Reference Baseline | T278 exact movement closure plus T279/S1–S3 ledgers, bindings, build plan and build evidence. |
| Files And ABI Surface | Closure verifier, closure history record, Status and documentation inventory only. |
| Verification | Re-run T278, T279/S1, S2 and S3 verifiers; inspect formal artifacts; scan support/overlay build inputs for forbidden provider/guest/component edges. |
| Exit Criteria | Met: one closure record distinguishes source mirror, x86 original library, x64 four-export overlay, static-only callback boundary and excluded tool/test inputs; all gates pass. |
| Original Owner Request | “单人双角色模式执行队列任务”。 |

### Closed packet reference — M0 T279 S3

### M0 T279 S3 — dual-architecture original-library build closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T279 S3`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | T279/S2 is source-form closed and pushed as `f6a2a996`; enable only the audited support-library build islands. |
| Objective | Build `oemuni.lib` and `suballoc.lib` from original selected bodies on x86 and x64 with Ninja/MSVC `/MT`; provide the four source-derived x64 OEM wrappers required to avoid pointer narrowing. |
| Non-goals | No edit to original mirror sources, provider/BOP activation, raw pointer mapping, suballoc callback backend, host control plane, or guest trace. |
| Reference Baseline | T279/S1 ledger, T279/S2 binding audit, original `sources` files and exact mirror hashes. |
| Files And ABI Surface | Generated Ninja graph, `mvdm-support-overlay` four exported OEM wrappers, focused host-local contract fixture and formal build evidence. |
| Verification | x86 links only original OEM bodies; x64 compiles renamed original unsafe bodies plus overlay exports; both build `oemuni.lib` and `suballoc.lib`; fixture proves four x64 wrapper contracts without native pointer narrowing. |
| Stop Conditions | An overlay grows beyond the four audited functions, an original mirror is edited, a second mapping manager or guest pointer route appears, or a warning is suppressed to fabricate closure. |
| Exit Criteria | Reproducible Ninja manifests build both architecture library forms with no duplicate exports; original hashes remain exact; x64 overlay behavior has local tests; warning disposition is explicit. |
| Original Owner Request | “单人双角色模式执行队列任务”。 |
| Similar-Issue Sweep | x86/x64 export uniqueness, offset reconstruction after OEM conversion, find-data header copy size, static-library link order, original compiler warnings, and excluded tests/tools. |

### Closed packet reference — M0 T279 S2

### M0 T279 S2 — same-shaped Win32/RTL binding closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T279 S2`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | S1 source-form interface closure is complete; recover only the named same-shaped adapter-mvdm-host-out/win32 bindings before support-library build admission. |
| Objective | Supply the exact reached NT4 RTL/PEB/TEB declaration and implementation surface for byte-exact `oemuni/file.c` and `oemuni/process.c`, using public modern Win32 APIs while keeping native pointers private. |
| Non-goals | No edit to original support-library bodies, provider/BOP route, XMS operation, raw host-pointer boundary, legacy `CreateProcessOem` body currently excluded by its original `#if 0`, or guest trace. |
| Reference Baseline | T279/S1 71-row source-form interface ledger and source-form audit. |
| Files And ABI Surface | `adapter-mvdm-host-out/win32` compatibility headers and implementation; an explicit exception record for x64-only pointer representation seams. |
| Verification | All reached bindings have source-shaped declarations; original bodies compile against them without source edits; x64 pointer-derived output is never narrowed through `DWORD`. |
| Stop Conditions | A binding requires a private NT kernel/CSRSS API, a native pointer crosses guest/session ABI, an original body is modified without mirror approval, or an adapter reimplements OEM provider logic. |
| Exit Criteria | The smallest audited same-shaped facade exists, its PEB/TEB/RTL state is local to the adapter, and the source/build-island admission record distinguishes any required overlay from direct source compilation. |
| Original Owner Request | “单人双角色模式执行队列任务”。 |
| Similar-Issue Sweep | Counted-string allocation/free pairs, thread-local static Unicode buffer, current-directory snapshot/lock, OEM/ANSI conversion error mapping, and `SearchPathOem` x64 pointer reconstruction. |

### Closed packet reference — M0 T279 S1

### M0 T279 S1 — original support-library ABI/build-form closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T279 S1`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | Owner directed single-person dual-role execution of the next dependency-ordered queue task. |
| Objective | Audit the exact original library translation units (`oemuni/file.c`, `oemuni/process.c`, `suballoc/suballoc.c`), their original `sources` selection, headers and all required x86/x64 boundary symbols before any modern build target is enabled. |
| Non-goals | No provider/BOP route, XMS body, MVDM tool/test product, anonymous rehost, runtime execution or guest trace. |
| Reference Baseline | T276 W0/W1 final dispositions, T277 reviewed interface ledger, T278 physical mirror closure and original source/build files. |
| Files And ABI Surface | S1 source-form interface ledger/audit, Status and documentation inventory. No runtime ABI changes. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component, build-hygiene and recovery-ladder rules. |
| Verification | Exact original source selections match `sources`; every external declaration/call from the three library units has source location, original shape, unique owner, x86/x64 disposition and failure rule; tests/tools remain excluded. |
| Expected Markers | Two original OEM library bodies adapter-mvdm-host-out/win32-bound; one suballocator body caller-bound through adapter-mvdm-host-out/softpc; no additional runtime translation unit. |
| Asset Needs | Re-rooted support mirror, original `sources`/makefiles, T276 W0/W1 records, T277 interface closure and [S1 plan](etc/operations/m0-t279-s1-support-library-abi-build-form-plan-001.md). |
| Reporting Requirements | Exact original library source list, dependency-symbol list, adapter/mapping owner, excluded tool/test source list and build-form limits. |
| Stop Conditions | A dependency is inferred from only a name, a new generic compatibility layer is proposed, a native pointer enters historical 32-bit storage, or a source body/build target would be enabled before closure. |
| Exit Criteria | Reproducible source-form ledger covers all three library bodies and distinguishes all external interfaces from internal/CRT calls, with no unresolved owner. |
| Original Owner Request | “单人双角色模式执行队列任务”。 |
| Similar-Issue Sweep | Test versus library body, public Win32 versus NT RTL/PEB use, original source order, x86/x64 pointer representation, allocator callback ownership and stale pre-re-root include path. |

### Closed packet reference — M0 T276 S25

### M0 T276 S25 — final expected-integration closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T276 S25`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | T276/S24 is closed: every 47-path VDD sample entry is final `not-host-runtime`, not an admitted VDD provider. Earlier S11–S23 package evidence was consolidated by S25. |
| Objective | Replace process-only dispositions for every previously audited package path with one evidence-backed final expected-integration disposition in the global 1,689-row tracker. **Closed.** |
| Non-goals | No source move/edit, adapter implementation, build edge, Ninja run or guest execution. Do not infer `direct` from a file being selected by a historical build. |
| Reference Baseline | T276/S1–S24 ledgers, package baselines, final-disposition taxonomy, source policy and architecture rules. |
| Files And ABI Surface | Global final tracker, package final-disposition evidence, Status and documentation inventory. No runtime ABI changes. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component and build-hygiene rules. |
| Verification | All 1,689 paths have one taxonomy-valid, non-`unresolved` final disposition with owner/link, change, named-adapter, mapping and source-evidence fields. The closure verifier reruns every package gate. |
| Expected Markers | No previously-audited package remains represented only by a `provisional-not-enabled` record; each final tracker row is independently traceable to original/package evidence. |
| Asset Needs | T276/S11–S24 ledgers, [final-disposition taxonomy](etc/operations/m0-t276-final-integration-disposition-taxonomy-001.md), global tracker and [S25 plan](etc/operations/m0-t276-s25-final-disposition-backfill-plan-001.md). |
| Reporting Requirements | Exact per-package and global final-disposition counts, adapter/mapping consequences and next package admission criterion. |
| Stop Conditions | A path has no selected/build evidence, raw host pointers cross a boundary, a second mapping manager is proposed, a body is enabled from a trace, or classification requires invented adapter behavior. |
| Exit Criteria | Met: all selected paths have a non-`unresolved` final disposition; no path is left with only a current-enable state, and no source body is enabled. See [M0 T276 closure](history/m0-t276-closure-20260826.md). |
| Original Owner Request | “好的 开始审计任务”。 |
| Similar-Issue Sweep | Historical build selection versus final product treatment, mirror change class, adapter ownership, guest/firmware/tool exclusion, legacy private product shells, x86/x64 mapping and unresolved evidence. |

**S21 closure:** all 25 selected `vdmredir` paths have reproducible non-enabled
dispositions. Fifteen selected DLL provider bodies remain one owner package;
any recovery retains its protocol ordering and uses only session-owned handles,
`adapter-mvdm-host-out/win32` and checked mapping-manager copies at the named boundaries.
See the [S21 Redirector baseline](etc/operations/m0-t276-s21-vdmredir-package-disposition-baseline-001.md).

**S22 closure:** all 201 selected `wow32` paths have reproducible non-enabled
dispositions. Seventy-seven active C bodies are separated into eight original
provider subdomains, and nine source-proven interface families name their sole
adapter/session boundary and mapping-manager rule. `gdi32p`, `user32p`, VDD,
NT4 monitor and product-shell dependencies remain explicit historical
requirements; no WOW body is enabled. See the [S22 subdomain baseline](etc/operations/m0-t276-s22-wow32-subdomain-baseline-001.md) and [interface-family ledger](etc/operations/m0-t276-s22-wow32-interface-family-ledger.tsv).

**S23 closure:** all 21 selected `fax` paths have reproducible non-enabled
dispositions. The one historical GDI driver and four printer-UI DLL bodies
remain separate source products, with six explicit printer/WOW/shared-section
interface families. Their pointer-packed NT4 shared-section layout may not
cross x86/x64 directly: a future owner must use session mapping-manager
identity plus copied broker records or retain a source-derived failure. See the
[S23 fax baseline](etc/operations/m0-t276-s23-fax-disposition-baseline-001.md).

**S24 closure:** all 47 selected `vdd` paths are final `not-host-runtime`.
They are sample trees (including sample guest/host halves), not a VDD product
provider. The source mirrors remain evidence, while `adapter-mvdm-host-out/vdd` remains only
the named boundary for a future separately audited product package. See the
[S24 plan](etc/operations/m0-t276-s24-vdd-sample-package-disposition-plan-001.md)
and the global [final expected-integration tracker](etc/operations/m0-t276-final-integration-disposition-ledger.tsv).

**S25 P1:** all 140 W0/W1 paths now have final expected-integration
dispositions: two `adapter-backed` OEM library bodies, 105 `binding-only`
declaration/library carriers, 31 `tool-only` historical products and two
`not-host-runtime` build-control inputs. This is a final planning conclusion,
not a source enablement claim; see the [W0/W1 final baseline](etc/operations/m0-t276-s25-w0-w1-final-disposition-baseline-001.md).

**S25 P2:** 558 conclusively classified `softpc.new` paths now have final
dispositions: 489 Bochs-replaced executors and four profile/build-control
inputs are `not-host-runtime`, 58 ROM/BIOS/data inputs are `firmware-only`,
and seven historical utilities are `tool-only`. The host control, monitor,
declaration and app-composition subdomains remain deliberately unresolved;
see the [conclusive SoftPC baseline](etc/operations/m0-t276-s25-softpc-conclusive-final-disposition-baseline-001.md).

**S25 P3:** all 15 `xms.486` paths now have a final disposition: six selected
original provider bodies are `adapter-backed`, `xms.h` is `binding-only`, and
the direct-host-address/non-x86/build-evidence paths are `not-host-runtime`.
The session `guest_memory` mapping-manager instance is the only XMS address
resolution route; see the [XMS final baseline](etc/operations/m0-t276-s25-xms-final-disposition-baseline-001.md).

**S25 P4:** all 59 `dpmi/*` paths are `guest-only`: the original package is
one 16-bit `DOSX.EXE` guest product, never a host static library. Its future
machine and BOP contracts remain at the declared guest boundary; see the
[DPMI final baseline](etc/operations/m0-t276-s25-dpmi-final-disposition-baseline-001.md).

**S25 P5:** all 13 `sim16`/`sim32` paths now have final dispositions. The
private serial peer/product-shell bodies are `not-host-runtime`; `sim32.h`
and the SIM16 declaration forms are `binding-only` evidence for specialised
monitor/WOW adapters, never a second mapping manager or serial machine.
See the [SIM final baseline](etc/operations/m0-t276-s25-sim-final-disposition-baseline-001.md).

**S25 P6:** all 23 `ieuvddex/*` paths are `not-host-runtime`: they are the
historical NTSD/WinDbg extension product, not a VDD or app host provider. A
future same-shaped debugger feature has one `adapter-mvdm-host-out/debugger` boundary and
uses copied session diagnostic snapshots; see the
[IEUVDDEx final baseline](etc/operations/m0-t276-s25-ieuvddex-final-disposition-baseline-001.md).

**S25 P7:** all 35 `v86/*` paths now have final dispositions: old kernel VDM
monitor and standalone `ntvdm.exe` scaffold sources are `not-host-runtime`,
nine declaration forms are `binding-only`, and the independent quit utility
is `tool-only`. See the [V86 final baseline](etc/operations/m0-t276-s25-v86-final-disposition-baseline-001.md).

**S25 P8:** all 25 `vdmredir/*` paths now have final dispositions: 15 original
provider bodies are `adapter-backed`, six protocol declarations are
`binding-only`, and four DLL build/export inputs are `not-host-runtime`. The
package remains one original provider under `adapter-mvdm-host-out/redir`, not a file-system
shim; see the [Redirector final baseline](etc/operations/m0-t276-s25-vdmredir-final-disposition-baseline-001.md).

**S20 closure:** all 25 selected `dpmi32` paths have reproducible non-enabled
dispositions. Sixteen selected x86 bodies form one host-provider package; all
guest addresses are constrained to the unique session mapping-manager lease or
copy path, and two RISC bodies remain evidence only. See the [S20 DPMI32 baseline](etc/operations/m0-t276-s20-dpmi32-package-disposition-baseline-001.md).

**S19 closure:** all 59 selected `dpmi` paths have reproducible non-enabled
dispositions. The original build produces 16-bit `DOSX.EXE`; its 22 selected
provider bodies and all supporting source remain one guest product prerequisite,
while products and map/sym files remain guest artifacts. No DPMI source is a
host composition input. See the [S19 DPMI baseline](etc/operations/m0-t276-s19-dpmi-package-disposition-baseline-001.md).

**S18 closure:** all 23 selected `ieuvddex` paths have reproducible non-enabled
dispositions. The package is an NTSD/WinDbg debugger extension, not a VDD
runtime provider: all future recovery belongs to `adapter-mvdm-host-out/debugger` through a
copied session diagnostic snapshot. Six Alpha/MIPS/PPC paths remain source
evidence only. See the [S18 IEUVDDEx baseline](etc/operations/m0-t276-s18-ieuvddex-package-disposition-baseline-001.md).

**S17 closure:** all 35 selected `v86` paths have reproducible non-enabled
dispositions. Seven kernel-VDM monitor bodies retain same-shaped monitor API
recovery evidence but remain excluded; six standalone `ntvdm.exe` scaffold
bodies remain app-owned product-shell evidence; and the 16-bit quit utility
remains a tool-only input. Every future monitor guest address resolves through
the unique session mapping manager, never a raw host pointer or a second
manager. See the [S17 V86 baseline](etc/operations/m0-t276-s17-v86-package-disposition-baseline-001.md).

### M0 T276 S11 — W0/W1 file and interface disposition baseline

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T276 S11`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | T276/S1–S10 establish exact source identity, boundary evidence, non-body ABI/guard evidence and six non-final package waves. No provider body or adapter ABI is enabled. |
| Objective | For W0/W1 (`dirs`, `makefil0`, `inc`, `oemuni`, `suballoc`, `vdmutils`), classify every selected file's original composition role and reached interface dependencies; identify direct, adapter-bound, minimal-binding-diff or profile-excluded candidates without enabling a build. |
| Non-goals | No provider/adapter implementation, source-body edit, enabled build edge, final recovery admission, BOP behavior, Bochs change or guest execution. |
| Reference Baseline | T276 S1/S2/S3/S4/S5/S10 ledgers, exact selected mirrors and source policy. |
| Files And ABI Surface | W0/W1 file/interface disposition ledger and evidence, Status and documentation inventory. No runtime ABI changes. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component and build-hygiene rules. |
| Verification | Account for every selected W0/W1 file and all reached source declarations/call candidates under those roots; preserve exact original build evidence; reject a file classification derived from path name alone. |
| Expected Markers | One provisional composition class and interface/dependency state per W0/W1 file, with any unresolved ABI explicitly linked to a named later review. No buildability is implied. |
| Asset Needs | T276 S1/S2/S3/S4/S5/S10 ledgers and exact W0/W1 source mirrors. |
| Reporting Requirements | Exact file and source/build-control counts, interface candidates, direct/profile-excluded candidates, unresolved ABI dependencies and W2 handoff. |
| Stop Conditions | A file cannot be tied to selected source/build evidence, a classification hides an unresolved external interface, or a direct classification requires an invented adapter. |
| Exit Criteria | Every W0/W1 selected path has a reproducible provisional composition/disposition record; unresolved interfaces are assigned to a later package/adapter review without changing mirror source. |
| Original Owner Request | “好的 开始审计任务”。 |
| Similar-Issue Sweep | Definition provenance, C/C++ versus assembly forms, unique declaration candidates, source-call aggregation, duplicate IDs and accidental provider/owner claims. |

**S1 closure:** the clean source topology initially contained thirteen
production roots and no premature production service body. The
[owner/source manifest](etc/operations/m0-t272-s1-production-owner-manifest-001.md)
records every root, its provenance/reuse class, intended build role and
owner packet; it also excludes `src.old/` and existing build outputs from
formal inputs.

**S2 closure:** the existing lifecycle reference was audited. The new
`session` component now has explicit caller-owned identity, instance-owned
cancellation, context-bearing LIFO teardown and bounded TLS bind/unbind;
global cancellation and COMMAND input payload were not carried forward. Its
focused lifecycle test passes under both MSVC x64 and Win32/x86 `/MT`.

**T272 closure:** the neutral session/mapping/lease/broker foundation is
complete and pushed through `d94d46ae`; its authoritative record is
[M0 T272 closure](history/m0-t272-closure-20260826.md).

**T274 closure:** the canonical platform ABI, local DOS/WOW16 guest carries,
and all selected MVDM host inputs are exact and hash-verified. This establishes
source supply only; later adapter/composition packets retain all behavior,
build and runtime claims. See [M0 T274 closure](history/m0-t274-closure-20260826.md).

**T276 S1 closure:** all 1,689 selected `mvdm-host` paths now have
reproducible source-to-mirror SHA-256 identity evidence, spanning exactly 23
package roots.  The file ledger records `byte-exact-verified` for every row;
composition and build-profile dispositions deliberately remain `unreviewed`.
See [T276 S1 file identity evidence](etc/operations/m0-t276-s1-file-identity-taxonomy-001.md).
The next S expands original build descriptions, translation units and include
edges; it does not recover interfaces or enable a package.

**T276 S2 closure:** 612 selected source translation units, 127 selected
original build-control inputs and 5,171 literal include edges are now recorded
without macro expansion or artificial build claims.  Of the source files, 487
are observed in 58 original NT `sources` descriptions; 125 are deliberately
recorded as not observed by this literal parser, pending build-description and
symbol analysis.  See [T276 S2 build-input evidence](etc/operations/m0-t276-s2-build-input-include-graph-001.md).
The next S expands reached declarations and external interfaces; it does not
enable a package merely because a textual include was observed.

**T276 S3 closure:** all 857 selected declaration files were scanned into
4,803 source-located lexical `extern` candidates, while the twelve historical
family debts remain explicitly unexpanded.  These are provenance records only:
they do not assert caller reachability, ABI availability, adapter ownership or
buildability.  See [T276 S3 declaration evidence](etc/operations/m0-t276-s3-interface-declaration-baseline-001.md).
The next S must resolve actual translation-unit callers against these candidates
and record ambiguities rather than guessing a package owner.

**T276 S4 closure:** every selected translation unit now has an explicit scan
state.  The case-sensitive C/C++ lexical scan records 89,924 call-or-definition candidates,
split into 3,697 unique-name, 6,920 ambiguous-name and 79,307 unrepresented
declaration correlations; 58 assembly units are explicitly not projected into
that model.  See [T276 S4 caller/callee evidence](etc/operations/m0-t276-s4-caller-callee-correlation-001.md).
S5 reviews only source-proven external boundaries and preserves unresolved
assembly/macro cases as evidence rather than inventing owners.

**T276 S5 closure:** all 3,697 S4 unique-name candidates now have a recorded
definition/boundary disposition: 1,429 have a selected-source definition-name
observation and 2,268 remain external-boundary candidates.  The 12,855
definition records include a comment-stripped K&R form and are deliberately
lexical; they do not make a linkage claim.
See [T276 S5 boundary evidence](etc/operations/m0-t276-s5-external-boundary-baseline-001.md).
S6 must review the 2,268 candidates by original declaration shape and caller
package, grouping real historical interface families before assigning owners.

**T276 S6 closure:** after K&R and function-pointer-aware parser corrections,
2,268 external-boundary calls reduce to 25 original declaration worklist rows:
23 SoftPC-shape and two debugger-shape review families. See
[T276 S6 worklist](etc/operations/m0-t276-s6-boundary-family-worklist-001.md).
The next S reviews each original declaration and its callers before any final
owner, adapter, availability or failure-contract entry is made.

**T276 S7 closure:** all 25 declaration worklist rows now have selected-source
occurrence navigation evidence. This evidence deliberately distinguishes no
source form yet: an occurrence may be a body, call, macro, callback/global or
comment-adjacent token. See [T276 S7 occurrence evidence](etc/operations/m0-t276-s7-provider-occurrence-baseline-001.md).
S8 performs that source-form review before any adapter or recovery conclusion.

**T276 S8 closure:** all 25 worklist rows now have original declaration and
provider-form evidence: 13 selected original body/dispatch forms, two callback
globals, one CRT import, three guarded no-provider historical options, and
six other no-provider forms. A case-sensitive regeneration removed the false
`CMDRebootVDM`/`cmdRebootVDM` correlation before this closure. See [T276 S8 provider-form evidence](etc/operations/m0-t276-s8-original-provider-form-review-001.md).
S9 records the source-derived ABI, guard and failure-contract evidence for the
non-body forms before a recovery wave is selected.

**T276 S9 closure:** all 12 non-body rows now have source-derived calling form,
availability/guard, result/failure evidence and package prerequisite. The five
CPU flag setters remain one CPU-status compatibility surface; the three guarded
optional historical paths remain profile questions; the debugger globals remain
callback bindings. See [T276 S9 ABI/failure evidence](etc/operations/m0-t276-s9-actionable-boundary-abi-failure-baseline-001.md).
S10 derives package waves from those records, not from a guest trace or compile
failure.

**T276 S10 closure:** all 23 selected roots now have an original role,
build/profile disposition, predecessor set and non-final candidate wave. The
six waves make the mixed `softpc.new` machine source and later product
extensions explicit rather than pretending they are one direct library. See
[T276 S10 package-wave evidence](etc/operations/m0-t276-s10-package-profile-wave-baseline-001.md).
S11 begins the required file/interface disposition from W0/W1 rather than
enabling a package on the first compile error.

**T276 S11 P1:** the six W0/W1 roots now have complete provisional file,
source-interface-family and lexical-call accounting: 140 selected paths, 14
source-interface families and 2,270 call candidates.  Only the exact original
`oemuni` library pair and `suballoc.c` remain runtime-support candidates;
all original utility products are explicit profile exclusions.  This remains
audit-only: no mirror source, provider body, adapter ABI or build edge is
enabled.  See [T276 S11 W0/W1 disposition evidence](etc/operations/m0-t276-s11-w0-w1-disposition-baseline-001.md).

**T276 S11 P2:** the three original W1 runtime-support translation units now
have an explicit binding handoff. `oemuni/file.c` and `process.c` retain their
original OEM/NT RTL algorithms pending same-shaped `adapter-mvdm-host-out/win32` bindings;
`suballoc.c` retains its `ULONG`/callback algorithm and defers only its
machine-facing callers to `adapter-mvdm-host-out/softpc`. No binding or source edit is
enabled. See the same S11 disposition evidence and runtime-support ledger.

**T276 S11 closure:** the six W0/W1 source-carrier/support/tool roots have a
complete provisional file owner/disposition record: 112 exact paths are
assigned to the future `mvdm-support` mirror and 28 to
`mvdm-tools`; all 2,270 lexical calls remain accounted without being
mistaken for enabled ABI. The only runtime-support candidates have exact
same-shaped binding handoffs. No source has moved or entered a build. S12 now
audits W2 `softpc.new` under the same source-first constraints.

**T276 S12 P1:** all 998 selected `softpc.new` paths now have a provisional
original-subpackage disposition. The baseline isolates 489 historical machine
execution/device paths as Bochs-replacement exclusions, 59 `host/src` mixed
control/device candidates, 156 `base/inc` machine declaration inputs, 58
firmware/ROM/data inputs assigned to the separate future `mvdm-softpc-firmware`
mirror, plus separate debugger/tool/Unix/composition inputs.
This is a boundary ledger only: no original machine/host source, adapter ABI
or build edge is enabled. See [T276 S12 SoftPC baseline](etc/operations/m0-t276-s12-softpc-disposition-baseline-001.md).

**T276 S12 P2:** the 56 original `softpc.new/host/src` source units are now
split by boundary rather than treated as one recoverable library: one BOP
dispatcher, ten SoftPC/SAS facade units, four monitor/reset/event units, one
session/worker unit, one cross-owner startup-composition unit, one
launch/configuration-composition unit, seven Win32 host-capability units, one
video/machine-composition unit, two VDD/debugger units, 18 device bridges and
ten literal-build-unobserved auxiliaries. No source or
adapter becomes enabled; the next slice audits complete owner groups at the
function/interface level. See [T276 S12 host/src baseline](etc/operations/m0-t276-s12-host-src-disposition-baseline-001.md).

**T276 S12 P3:** all 56 `host/src` source units now have call/definition/
boundary coverage: 7,079 lexical calls, 380 selected-definition observations
and 37 external-boundary candidates. This is a source-review ordering aid only,
not an ABI or reachability claim. Complete monitor/session, SoftPC facade and
Win32 host groups are the next audit order; unobserved auxiliaries remain
non-enabled. See [T276 S12 interface-density baseline](etc/operations/m0-t276-s12-host-src-interface-density-baseline-001.md).

**T276 S12 P4:** all 58 selected original `softpc.new/base/bios`, `bios`,
`roms` and `data` paths are now assigned to the future
`mvdm-softpc-firmware` mirror. The component is an immutable machine-input
carrier only: no source is compiled or linked as host runtime, no parallel
SoftPC machine is admitted, and only a future explicit `adapter-bochs`
manifest can select an input. The regenerated 998-row S12 ledger and verifier
record this owner boundary.

**T276 S12 P5:** the four monitor/reset sources, one session-worker source and
one cross-owner startup-composition source now have fifteen source-span-level
original-interface dispositions. `nt_msscs.c` is explicitly separated from
monitor ownership: it sequences COMMAND/DEM/XMS/debugger startup, guest-image
loading and VDD hooks, so it cannot be independently enabled. The ledger names
the applicable adapter/package predecessor for every group and records no
source or adapter enablement.

**T276 S12 P6:** all ten SoftPC/SAS facade sources now have twelve
source-span-level interface dispositions. `nt_cpu.c` CPU execution and direct
CPU-internal paths are explicit Bochs-replacement exclusions; SIM32/SAS,
memory, EMM and UMB paths retain their original contracts but require named
guest-memory, physical-page, VDD or session prerequisites. Historical build
stubs remain excluded rather than becoming default adapter bodies. No source
or adapter is enabled.

**T276 S12 P7:** the nine original host-capability files now have eleven
source-span-level Win32/NTDLL dispositions. `config.c` and `nt_sec.c` are
explicit cross-owner launch and video/machine composition prerequisites, not
generic Win32 helpers. The remaining diagnostic, NLS, keyboard, serial, PIF,
error and wait paths retain their original interface contracts with named
public-binding/session gates. No source or adapter is enabled; see the
[P7 Win32/NTDLL capability baseline](etc/operations/m0-t276-s12-win32-capability-interface-baseline-001.md).

**T276 S12 P8:** all eighteen mixed device-bridge source files now have
device-family dispositions. Historical CGA/EGA/VGA, FDC/DMA/IRQ, speaker/PPI
and timer/PIC bodies are explicit Bochs-replacement exclusions; presentation,
input, serial/printer, media and resources retain only named public endpoint
or product-owner prerequisites. No generic device adapter, source body or
build edge is enabled; see the [P8 device-bridge baseline](etc/operations/m0-t276-s12-device-bridge-interface-baseline-001.md).

**T276 S12 P9:** original `nt_bop.c`, `nt_vdd.c` and `nt_yoda.c` now have
twelve dispatcher/VDD/debugger interface dispositions. BOP selector meaning
remains in the OpenNT mirror, VDD requires the existing specialist adapter and
typed machine requests, and Yoda stays debugger-profile-gated. No source or
adapter is enabled; see the [P9 BOP/VDD/debugger baseline](etc/operations/m0-t276-s12-bop-vdd-debugger-interface-baseline-001.md).

**T276 S12 P10:** all ten literal-build-unobserved auxiliary sources now have
source-form dispositions. Alternative CCPU/A2CPU access and virtual-FDC bodies
are explicit Bochs-replacement exclusions; error data, historical stubs,
debug/settings UI, cleanup and configuration paths await their named original
build/profile and owner prerequisite. No source or adapter is enabled.

**T276 S12 P11:** the six detailed owner ledgers now cover all 56 original
`host/src` source paths exactly once. The combined gate reruns all family
checks, rejects omissions/duplicates and records the W3 package-wave handoff;
see the [host/src completion baseline](etc/operations/m0-t276-s12-host-src-completion-baseline-001.md).

**T276 S13 P1:** all 40 original `dos` paths now have one package-led
provisional disposition: 27 literal `SOURCES` translation units (16 DEM and
11 COMMAND), seven declarations, four build descriptions, one `dirs` metadata
file and one historical test. DEM and COMMAND remain intact source-owner
packages; no trace-selected BOP, source body or build edge is enabled. See the
[DOS package disposition baseline](etc/operations/m0-t276-s13-dos-package-disposition-baseline-001.md).

**T276 S13 P2:** the retained DOS source reaches eight named boundary families:
BOP transport, SoftPC/SAS guest state, public Win32/NTDLL capability,
session-owned state/identity, Redirector/pipe, VDD, OEM/NLS support and
BIOS-style raw media/IOCTL. Every family has an existing named owner or an
explicit Bochs-machine profile gate. The audit records later package order as
DEM foundation/provider first, then COMMAND on its declared session and
Redirector prerequisites; no provider is enabled.

**T276 S14 P1:** all 15 original `xms.486` paths have one non-enabled
disposition. The exact package contains six top-level literal XMS sources, one
separately selected i386 source, one profile-unselected RISC source, one public
header and six build descriptions. `xmsmem86.c` requires a same-shaped
mapping-manager replacement: its original three exported functions preserve
their contract but resolve 32-bit values to checked Bochs guest ranges.

**T276 S14 P2:** XMS reaches eight owner boundaries: XMS BOP dispatch, A20,
extended-memory/block move, UMB, INT 15, suballoc, historical direct virtual
memory and XMS shared state. A20/guest range/UMB/IVT remain
`adapter-mvdm-host-out/softpc -> adapter-bochs` prerequisites; UMB stays gated until a
durable bounded guest-byte seam is actually proved. No source body is enabled.

**T276 S15 P1:** all eight original `sim16` paths now have a non-enabled
disposition. The original makefile proves a 16-bit host-side WOW simulator DLL
using historical WOW libraries, not a Bochs-loadable guest image. Its two
assembler bodies remain `adapter-mvdm-host-out/wow` profile-gated: serial/PIC mechanics stay
with `adapter-bochs`, VDM-memory transport with `adapter-mvdm-host-out/monitor`, and no
generic protocol or device implementation is created.

**T276 S16 P1:** all five original `sim32` paths now have a non-enabled
disposition. Its original `sources` selects no translation unit: `sim32.c` is
a private serial-IOCTL/`sim16` WOW product shell, not the general SIM32
facade. The historical public `Sim32Get/Set/PSZ/Free/Send` API shape is retained
as recovery evidence for `adapter-mvdm-host-out/monitor`, but every guest address must
resolve through the one session mapping manager to checked Bochs RAM; no raw
pointer, serial product shell or second manager is admitted.

### M0 T275 S1 — Complete interface and reuse ledger

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T275 S1`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | The owner’s standing direction to implement the ordered queue as a globally planned, minimalist recovery program; T274 source supply is closed. |
| Objective | Freeze a complete original-caller/interface/ABI/failure/reuse disposition for BOP, SoftPC/CCPU/SAS, Win32/NTDLL, VDM-monitor and broker families before any adapter body is recovered. |
| Non-goals | No new adapter ABI, source-body import, provider enablement, Bochs change, guest change, broker IPC implementation or host build claim. |
| Reference Baseline | T274 exact MVDM mirror; adapter-recovery proposal; `src.old/adapter-*`, `app` and `session` as non-formal recovery references; BOP tracker and component owner manifest. |
| Files And ABI Surface | A reproducible family/caller/reuse ledger, candidate-file inventory, S1 evidence, Status and documentation inventory. No runtime ABI becomes active. |
| Applicable Rules | Execution, historical recovery, source policy, architecture, coding, document, mirror-component and build-hygiene rules. |
| Verification | Enumerate every old adapter candidate; identify its original caller/contract or disposition it as reference-only; scan the exact host mirror for family declarations/callers; prove no source copy or formal build input is enabled. |
| Expected Markers | One owner and one disposition for every candidate file and each reached family; exact source references, calling-convention/layout/failure requirements, reuse decision and successor S. |
| Asset Needs | T274 manifests, `mvdm-host`, `mvdm-platform-abi`, `src.old` reference tree, existing source-adapter proposal and tracker. |
| Reporting Requirements | Candidate and family counts, direct caller evidence, source-first rung decision, retained/rejected recovery candidates, unavailable historical dependencies, boundary and next-S handoff. |
| Stop Conditions | A candidate has no discoverable owner, a caller requires a behavior body before the ledger is complete, an interface crosses a prohibited Bochs boundary, or an ambiguous historical ABI cannot be resolved. |
| Exit Criteria | Every candidate and every reached interface family has a reproducible source/ABI/failure/reuse disposition; S2--S6 can each recover only the frozen family surface without reviving a parallel product shell. |
| Original Owner Request | “单人双角色模式实施当前队列中的所有任务，要求全局统筹，追求结果极简主义…避免添油战术。” |
| Similar-Issue Sweep | Existing adapter/app/session mechanics, BOP tracker dependencies, MVDM declarations/callers, source mirror package roots, old-path build leaks and Bochs direct-access risks. |

**S1 P1:** the reproducible recovery-candidate inventory contains 453 files:
61 implementation candidates, 385 declaration candidates and seven supporting
references. It is a source-reference baseline only, not permission to copy a
file or claim an adapter ABI. See the [P1 inventory note](etc/operations/m0-t275-s1-adapter-candidate-inventory-001.md).

**S1 P2:** the original-caller ledger now freezes 11 interface dispositions:
one BOP transport, five SoftPC/CCPU/SAS surfaces, three Win32/NTDLL surfaces,
one VDM-monitor surface and the minimal broker boundary. It explicitly retires
the old machine shell and project-only observation surfaces from adapter
recovery. See the [P2 interface ledger note](etc/operations/m0-t275-s1-historical-interface-ledger-001.md).

**S1 closure:** all 453 historical adapter candidates now have a unique,
reproducible disposition and all 61 implementation candidates are linked to
one of the 11 original-caller interface surfaces. The matrix has no duplicate
path, empty interface ID or empty disposition; the exact host mirror has zero
direct Bochs reference. S2 may establish only the missing-interface inventory
and its component boundaries; adapter-source recovery begins only after that
inventory closes. See the [S1 closure evidence](etc/operations/m0-t275-s1-adapter-reuse-ledger-closure-001.md).

### M0 T275 S2 — specialist adapter component inventory

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T275 S2`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | The owner requires original MVDM source to be imported before function-body alteration; every missing boundary must be assigned to a named adapter and recorded before source recovery. |
| Objective | Establish the complete missing-interface and specialist-adapter inventory: original caller/declaration/contract, unique owner adapter, modern recovery or original-style failure disposition, and successor ownership for every unavailable historical external interface. |
| Non-goals | No adapter source implementation, mirror-source modification, BOP selector/service decode, provider enablement, guest-memory implementation, Bochs change, guest execution or host build claim. |
| Reference Baseline | T275/S1 ledger; exact `mvdm-host` VDMREDIR, WOW32, VDD, debugger/monitor, SoftPC and host packages; current architecture/rules; old adapter references. |
| Files And ABI Surface | Adapter component inventory and architecture/proposal/rule updates only. No runtime ABI becomes active. |
| Applicable Rules | Execution, historical recovery, architecture, coding, mirror-component, source policy and build-hygiene rules. |
| Verification | Scan selected MVDM callers/declarations for unavailable external/product interface families; classify each as existing adapter, new specialist adapter or original internal owner; verify every enabled-source edit is only a source-recorded binding change and no generic catch-all adapter exists. |
| Expected Markers | One named owner adapter or explicit non-adapter owner per external family; each future symbol row records caller, declaration, ABI/layout, failure contract, adapter, binding edit and x86/x64 evidence; `adapter-mvdm-host-out/redir`, `adapter-mvdm-host-out/wow`, `adapter-mvdm-host-out/vdd` and `adapter-mvdm-host-out/debugger` each have original package evidence and directed-boundary rules. |
| Asset Needs | Exact host mirror, source-supply manifests, existing adapter ledger, current architecture/rules and tracker. |
| Reporting Requirements | State component additions, original callers/packages, retained interface shape, public-modern/API or machine dependency, explicit unavailable rule, successor S/T and why no generic adapter is introduced. |
| Stop Conditions | An interface has no original owner, requires a generic adapter, crosses prohibited Bochs/mirror boundaries, or needs source-body implementation before its component boundary is approved. |
| Exit Criteria | Architecture, coding/rule documents, proposal and inventory name every specialist adapter needed by the selected source universe; later implementation imports original MVDM bodies and uses the inventory rather than modifying mirrors to bypass a missing interface. |
| Original Owner Request | “单人双角色模式实施当前队列中的所有任务，要求全局统筹，追求结果极简主义…避免添油战术。” |
| Similar-Issue Sweep | VDMREDIR, WOW32, VDD, debugger/monitor, SoftPC, Win32, BOP, broker, existing adapter roots, mirror includes, package build descriptions and forbidden generic adapter roots. |

**S1 closure:** all 453 historical adapter candidates now have a unique,
reproducible disposition and all 61 implementation candidates are linked to
one of the 11 original-caller interface surfaces. The matrix has no duplicate
path, empty interface ID or empty disposition; the exact host mirror has zero
direct Bochs reference. S2 may establish only the missing-interface inventory
and its component boundaries; adapter-source recovery begins only after that
inventory closes. See the [S1 closure evidence](etc/operations/m0-t275-s1-adapter-reuse-ledger-closure-001.md).

**S2 closure:** nine adapter owner boundaries and the twelve-family
missing-interface ledger are recorded. The detailed package/symbol tracker is
the required successor; no adapter ABI or mirror source body was enabled. See
the [S2 inventory](etc/operations/m0-t275-s2-specialist-adapter-inventory-001.md).

**S1 P2:** the original-caller ledger now freezes 11 interface dispositions:
one BOP transport, five SoftPC/CCPU/SAS surfaces, three Win32/NTDLL surfaces,
one VDM-monitor surface and the minimal broker boundary. It explicitly retires
the old machine shell and project-only observation surfaces from adapter
recovery. See the [P2 interface ledger note](etc/operations/m0-t275-s1-historical-interface-ledger-001.md).

**S1 P1:** the reproducible recovery-candidate inventory contains 453 files:
61 implementation candidates, 385 declaration candidates and seven supporting
references. It is a source-reference baseline only, not permission to copy a
file or claim an adapter ABI. See the [P1 inventory note](etc/operations/m0-t275-s1-adapter-candidate-inventory-001.md).

### M0 T274 S6 — MVDM host package source recovery

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T274 S6`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | S1 establishes a unique selected source for every host path; S2--S5 are closed. |
| Objective | Recover every S5-deferred, selected original MVDM host provider body and remaining package input under `mvdm-host`, retaining the selected source-relative path and hash. |
| Non-goals | No body modification, compilation, linking, provider enablement, adapter implementation, guest change, or replacement shim. |
| Reference Baseline | S1 union ledger; S5 topology manifest; pinned OpenNT/OpenNT-4.5 MVDM roots; source policy and T274 proposal. |
| Files And ABI Surface | Exact selected body/other host inputs, reproducible S6 manifest/evidence, component README, Status and documentation inventory. No enabled runtime ABI. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component and build-hygiene rules. |
| Verification | Copy only the S5-deferred rows with non-identical-conflict refusal; rehash every destination; prove all selected host paths are represented and guest roots/build inputs remain isolated. |
| Expected Markers | Complete 1,689-row selected host manifest, zero source/destination hash mismatches, no unregistered non-original host file and no guest build/link consumer. |
| Asset Needs | S1 TSV ledger, S5 manifest, both pinned external MVDM roots, `mvdm-host` root and component source policy. |
| Reporting Requirements | State imported/deferred counts, bytes/types/package roots, source selections, conflicts, exactness exceptions and host/guest isolation. |
| Stop Conditions | Ambiguous provenance, a non-identical destination conflict, a selected source unavailable locally, or an attempted behavior-changing edit. |
| Exit Criteria | Every S1-selected `mvdm-host` path is byte-identical to its selected source or explicitly registered as an allowed mirror exception; the source-supply task then closes without claiming compilation or runtime enablement. |
| Original Owner Request | “单人双角色模式实施当前队列中的所有任务，要求全局统筹，追求结果极简主义…避免添油战术。” |
| Similar-Issue Sweep | All S5 deferred bodies/non-topology inputs, package roots, source selections, destination conflicts, external reference leaks and host/guest boundaries. |

**S6 closure:** all 639 S5-deferred selected host inputs were directly
recovered and rehashed with zero mismatches: 612 C/C++/assembly provider bodies
and 27 other inputs. Together with S5, `mvdm-host` now contains all
1,689 selected paths across 23 original package roots, with no compilation,
linking or provider enablement claim. See the [S6 closure
evidence](etc/operations/m0-t274-s6-mvdm-host-package-supply-closure-001.md).

### M0 T274 S5 — MVDM host topology recovery

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T274 S5`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | Owner-approved canonical source-supply proposal; S1 selected union ledger and S2–S4 source-supply closures are complete. |
| Objective | Recover the selected original MVDM host header, resource and build-topology inputs under `mvdm-host` with one source/hash disposition per copied path. |
| Non-goals | No provider C/C++/assembly body import, compilation, link closure, adapter implementation, external source mix, guest relocation, or source-body modification. |
| Reference Baseline | S1 union ledger, pinned OpenNT/OpenNT-4.5 MVDM roots, source policy, T274 proposal and current component rules. |
| Files And ABI Surface | Byte-identical headers, declarations, resources and build-topology files in `mvdm-host`; topology manifest, component README, S5 evidence, Status and documentation inventory. No enabled runtime ABI. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component and build-hygiene rules. |
| Verification | Filter the selected host ledger by topology/header/resource class; copy with non-identical-conflict refusal; rehash every destination; verify package-relative paths and no provider body import. |
| Expected Markers | Complete selected topology manifest, zero source/destination hash mismatches, exact source-relative paths and explicit deferred provider-body count. |
| Asset Needs | S1 TSV ledger, both pinned external MVDM roots, `mvdm-host` root and component source policy. |
| Reporting Requirements | State file/byte/type/package counts, provenance selections, deferred body count, conflicts, exceptions and host/guest isolation. |
| Stop Conditions | A selected path has ambiguous provenance, a destination conflicts, a needed item is behavior-bearing provider body, or an import would create a parallel edition root. |
| Exit Criteria | Exact selected host topology/header/resource inputs are mirrored with a reproducible manifest and body import remains explicitly deferred to S6. |
| Original Owner Request | “单人双角色模式实施当前队列中的所有任务，要求全局统筹，追求结果极简主义…避免添油战术。” |
| Similar-Issue Sweep | All selected MVDM headers/includes, makefiles/sources/dirs/def/resource forms, package roots, OpenNT edition paths, existing host candidates and guest/host link boundaries. |

**S5 closure:** 1,050 selected topology/header/resource inputs across 23
original package roots were imported and rehashed without a mismatch. 612
provider-body and 27 non-topology rows remain explicit S6 inputs; zero C/C++/
assembly body files entered the S5 destination. See the [S5 closure
evidence](etc/operations/m0-t274-s5-mvdm-host-topology-closure-001.md).

### M0 T274 S4 — WOW16/bin86 direct guest recovery

### M0 T274 S4 — WOW16/bin86 direct guest recovery

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T274 S4`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | Owner direction: directly copy existing guest files; do not compile, reimport external sources, or treat source-cardinality as a blocker. |
| Objective | Recover the existing local WOW16 and bin86 guest carry into `mvdm-guest/win16` and record the unchanged `build/output/wow16` products as immutable load-only inventory. |
| Non-goals | No compilation, external OpenNT/OpenNT-4.5 source recovery, source-completeness claim, host linking, BOP/provider change, or output relocation. |
| Reference Baseline | `src.old/opennt-guest/wow16`, `src.old/opennt-guest/bin86`, `build/output/wow16`, S1 ledger, source policy and S4 local-inventory disposition. |
| Files And ABI Surface | Byte-identical WOW16/bin86 carried files, source/output hash manifests, component README, S4 evidence, Status and documentation inventory. No host ABI. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component and build-hygiene rules. |
| Verification | Copy every local carried file with conflict refusal; rehash source/destination; hash the output tree in place; prove no output move/copy and no host build input. |
| Expected Markers | Two source manifests, one in-place output manifest, zero source/destination hash mismatches and explicit load-only ownership. |
| Asset Needs | Existing local WOW16/bin86 carry, existing output tree, S1 ledger and Git path inventory. |
| Reporting Requirements | State carried source/product counts and bytes, output location/hash inventory, exclusions, source-cardinality limitation and no-host-link result. |
| Stop Conditions | A local file conflicts with its destination, a command attempts output relocation, or a host manifest consumes a guest object/library. |
| Exit Criteria | Every current carried WOW16/bin86 file is byte-identical in its guest root, output products are manifested in place, and the source-completeness limitation is explicit without blocking load-only use. |
| Original Owner Request | “你只是把guest文件直接复制进来，难点是啥 又不用你编译 难点是啥？不是分分钟就做完了？” |
| Similar-Issue Sweep | WOW16/bin86 files, output binaries, source manifests, source/link consumers, external source references and guest-root placement. |

**S4 closure:** 44 WOW16 and 10 bin86 carried files were directly recovered
and rehashed with zero mismatches. The 87 products already in
`build/output/wow16` were hash-manifested in place without copy, move or build.
The 1,261 external-only source paths are explicitly not an S4 requirement. See
the [S4 closure evidence](etc/operations/m0-t274-s4-wow16-guest-recovery-closure-001.md).

### M0 T274 S3 — DOS guest mirror direct recovery

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T274 S3`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | Owner-approved canonical source-supply proposal and explicit direction to reuse the existing complete DOS guest rather than re-copy it from external OpenNT sources. |
| Objective | Recover the complete repository-local DOS/V86 guest mirror into `mvdm-guest/dos`, preserving every carried source/resource/build/intermediate/product input as an immutable guest-only inventory. |
| Non-goals | No external guest reimport, guest-source modification, guest compilation, host linking, BOP/provider enablement, output relocation, or WOW16 recovery. |
| Reference Baseline | `src.old/opennt-guest/dos-v86`, S1 selected union ledger, source policy, T274 source-supply proposal and current build-output isolation. |
| Files And ABI Surface | Byte-identical `mvdm-guest/dos` mirror paths, source hash manifest, component README, S3 evidence, Status and documentation inventory. No host ABI. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component and build-hygiene rules. |
| Verification | Enumerate and SHA-256 every carried source path; copy only into the matching guest root; prove source/destination equality, no host build manifest reference, and unchanged `build/output/dos`/`build/output/wow16` placement. |
| Expected Markers | One source-relative DOS/V86 inventory, zero source/destination hash mismatches, no divergence entries, and explicit guest-load-only ownership. |
| Asset Needs | Existing repository-local DOS/V86 mirror, S1 ledger, Git path inventory and prior build-output roots. |
| Reporting Requirements | Record files/bytes/types, source/destination hashes, excluded output paths, source provenance, any absent local ledger rows and no-host-link conclusion. |
| Stop Conditions | A carried path conflicts with a non-identical destination, a supposed guest item requires host linkage, an external source is needed to fill the local inventory, or a source provenance issue is found. |
| Exit Criteria | Complete byte-identical DOS/V86 mirror and reproducible manifest/evidence; `build/output/*` remains in situ and excluded from formal host inputs; S4 can repeat the same direct route for WOW16. |
| Original Owner Request | “opennt-guest里面的dos和wow16，其实可以直接复制到位，不需要从opennt opennt-4.5再次复制…build/output/dos, build/output/wow16里面的二进制…留着就行。” |
| Similar-Issue Sweep | DOS/V86 source/resource/build/intermediate/product files, guest DPMI forms, carried artifacts, manifest consumers, output paths, host source lists and external-guest references. |

**S3 closure:** all 676 local DOS/V86 inventory rows were copied into
`mvdm-guest/dos` and rehashed without a mismatch. The second recovery run
accepted all 676 destinations as already identical. `build/output/dos` and
`build/output/wow16` remain in place and are not host inputs. See the
[S3 closure evidence](etc/operations/m0-t274-s3-dos-guest-mirror-closure-001.md).

### M0 T274 S2 — Platform ABI mirror audit

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T274 S2`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | Owner-approved canonical OpenNT source-supply proposal; S1 closed with the selected non-guest union ledger. |
| Objective | Establish the smallest exact-declaration platform ABI inventory and production root needed to compile later selected MVDM host packages without inventing replacement behavior. |
| Non-goals | No MVDM provider body import, host compilation, Win32/NTDLL replacement, adapter implementation, BOP execution, guest reimport, or guest-output relocation. |
| Reference Baseline | S1 selected union ledger, pinned OpenNT/OpenNT-4.5 trees, source policy, mirror rules and T274 source-supply proposal. |
| Files And ABI Surface | `mvdm-platform-abi` README, exact declaration/disposition ledger, optional byte-identical upstream declarations, S2 evidence, Status and documentation inventory. No behavior-bearing source file. |
| Applicable Rules | Execution, source policy, architecture, coding, document, mirror-component and build-hygiene rules. |
| Verification | Scan the selected `mvdm-host` universe for direct include/declaration needs; distinguish MVDM-local, upstream-platform, public SDK and unresolved rows; hash every imported declaration; run governance and scoped diff review. |
| Expected Markers | Every direct external include/declaration requirement has a unique disposition, every material file is byte-identical upstream and no platform behavior is linked or authored. |
| Asset Needs | S1 TSV ledger, both pinned external source trees, source policy and existing component ownership manifest. |
| Reporting Requirements | State requirement cardinalities, direct versus transitive boundary, selected source path/hash, exclusions, unresolved API/declaration risks and future package consumers. |
| Stop Conditions | A required item needs a behavior body, an ambiguous cross-generation header mix, a proprietary SDK import without source-policy approval, or an ABI source cannot be identified. |
| Exit Criteria | A reproducible complete direct-platform ABI disposition ledger, an audited minimal `mvdm-platform-abi` root and no unregistered declaration import; S5/S6 can consume it without reopening the S1 union decision. |
| Original Owner Request | “单人双角色模式实施当前队列中的所有任务…追求结果极简主义…避免添油战术。” |
| Similar-Issue Sweep | All selected MVDM header/include forms, platform declarations, `windows.h`/NTDLL families, internal MVDM headers, source/build descriptions, pre-existing shims and architecture-specific forms. |

**S2 closure:** the reproducible direct-include ledger covers all 637 include
forms found in 1,467 selected MVDM code inputs.  The minimal
`mvdm-platform-abi` mirror contains 22 uniquely resolved, byte-identical
original declaration headers; its import manifest reports zero SHA-256
mismatches on an idempotent second import.  CRT/SDK forms remain toolchain
inputs, while same-name and generated historical forms remain explicit owner
package rows.  See the [S2 closure evidence](etc/operations/m0-t274-s2-platform-abi-mirror-closure-001.md).

### M0 T274 S1 — Non-guest package-scope union ledger

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T274 S1`, Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | Owner-approved canonical OpenNT source-supply proposal; T273 closed after its S4 boundary audit. |
| Objective | Create the authoritative package-scope path/provenance/hash/disposition ledger for the non-guest OpenNT/OpenNT-4.5 MVDM union. |
| Non-goals | No guest reimport, provider enablement, dormant-body compilation, interface replacement, BOP execution, or host composition. |
| Reference Baseline | `O:\repos.external\OpenNT\base\mvdm`, `O:\repos.external\OpenNT-4.5\nt\private\mvdm`, source policy, T272 owner manifest and source-supply proposal. |
| Files And ABI Surface | Source-supply ledger/evidence, proposal/status/README provenance records and documentation inventory only. No production source body is changed in S1. |
| Applicable Rules | Execution, source policy, mirror-component, architecture, coding, document and build-hygiene rules. |
| Verification | Enumerate both trees; hash matching relative paths; classify every one-sided/conflicting file by owning package/build/resource/artifact lineage; governance and scoped diff review. |
| Expected Markers | One selected source or explicit rejection for each target-relative non-guest path, variant hash evidence, package owner and next-subtask disposition. |
| Asset Needs | Both pinned external source trees, existing guest carry-forward inventories and the repository-local source policy. |
| Reporting Requirements | State cardinalities, conflict rules, selected/rejected variants, unresolved package ambiguities and host/guest isolation. |
| Stop Conditions | A path cannot be assigned to a complete owner package, provenance is ambiguous, or a selection would silently mix incompatible build generations. |
| Exit Criteria | Complete non-guest union ledger with every target path classified; S2 can import exact platform ABI declarations without reopening selection decisions. |
| Original Owner Request | “opennt-guest里面的dos和wow16，其实可以直接复制到位…已有的app, session, bochs-core和adapter-*组件，可作为参考来源…” |
| Similar-Issue Sweep | All MVDM package roots, headers, build files, resources, binaries, tools, duplicate variants, guest/host boundaries and obsolete current roots. |

**S1 closure:** the reproducible 3,671-path union ledger gives every non-guest
host path a unique source/owner disposition. It retains guest/tree evidence
without reimporting it: 688 current carried guest paths match exactly, while
1,261 external WOW paths are explicitly outside the current carried inventory
and cannot become implicit source inputs. See the
[S1 ledger closure](etc/operations/m0-t274-s1-non-guest-mvdm-union-ledger-closure-001.md).

### M0 T273 S4 closure — Boundary and architecture verification

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T273 S4`, closed; Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | Owner-approved Bochs foundation proposal; S3 closed in `f364eccf`; S4 is its final declared verification subtask. |
| Objective | Prove the recovered Bochs foundation has the required directed dependencies, mirror/overlay privacy and x86/x64 focused lifecycle closure. |
| Non-goals | No guest execution, firmware product boot, BOP provider, OpenNT service, SoftPC API recovery, feature enablement, or new Bochs semantic change. |
| Reference Baseline | T273 S1 audit, S2 P2--P4 evidence, S3 closure, current architecture/rules and the pinned Bochs 2.6 baseline. |
| Files And ABI Surface | Source/include/build manifests, component READMEs, boundary tests, status/evidence/history/queue only. No new consumer ABI is admitted. |
| Applicable Rules | Execution, architecture, coding, mirror-component, build-hygiene, documentation and source-policy rules. |
| Verification | Full production include/reference scan, mirror-difference/register reconciliation, overlay reachability audit, focused x64/x86 rerun, governance and scoped diff review. |
| Expected Markers | `adapter-bochs` is the sole `bochs-core` consumer; no project dependency from `bochs-core`; overlay only supports its mirror; app only sees opaque adapter API. |
| Asset Needs | Recovered roots, registered README/evidence records, disposable P3/P4 x64/x86 roots, MSVC toolchains and governance scripts. |
| Reporting Requirements | State command inputs/results, exact remaining registered Bochs differences, negative dependency findings and the boundary of later source-supply work. |
| Stop Conditions | A prohibited consumer, unregistered mirror deviation, overlay external caller or host-width failure is found; it must be separately admitted rather than concealed by S4. |
| Exit Criteria | Both host-width fixtures and all directed-boundary/mirror checks pass, or every found violation is explicitly dispositioned; T273 closure then records only proven minimal-machine facts. |
| Original Owner Request | “单人双角色模式实施当前队列中的所有任务，要求全局统筹，追求结果极简主义…避免添油战术。” |
| Similar-Issue Sweep | All production include edges, Bochs/adapter vocabulary, old-root build inputs, overlay external consumers, CPU configuration, host-width assumptions and fixture source lists. |

**T273 closure:** the minimal CPU5/Pentium-MMX Bochs construction/reset/RAM/
port-space/timer-stop/resume foundation is proven under x64 and Win32/x86;
`adapter-bochs` is its only external caller and `bochs-core` has no project
dependency. The closure does not claim guest execution. See the
[S4 audit](etc/operations/m0-t273-s4-bochs-boundary-architecture-audit-001.md)
and [T273 closure](history/m0-t273-closure-20260826.md).

### M0 T273 S3 closure — Minimal app machine shell

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T273 S3`, closed; Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | Owner-approved Bochs foundation proposal; S2 is closed in `9645cdd9`, and the proposal’s next bounded subtask is admitted. |
| Objective | Recover the smallest existing app/session composition that can create and deterministically destroy an opaque `adapter-bochs` machine endpoint. |
| Non-goals | No guest image loading, BOP decoding, OpenNT/DOS/WOW/SoftPC/CCPU behavior, CLI feature surface, Win32 host service, or independent machine scheduler. |
| Reference Baseline | T272 neutral session closure; T273/S2 P4 lifecycle closure; quarantined `src.old/app` and `src.old/session` candidates; current component architecture. |
| Files And ABI Surface | Audited `app`/`session` source candidates, their minimal opaque composition API, one focused fixture, status/evidence/README records. No Bochs object type may enter either public surface. |
| Applicable Rules | Execution, architecture, coding, source policy, component mirror, build-hygiene and documentation rules. |
| Verification | Per-file owner/provenance audit, dependency/vocabulary scan, focused x64/x86 fixture, `git diff --check` and documentation governance. |
| Expected Markers | A caller-owned opaque lifecycle handle, deterministic create/destroy ordering, no `bochs-core` include outside `adapter-bochs`, and explicit rejected candidates. |
| Asset Needs | Existing T272 session sources, audited app candidates, P4 mechanical API, MSVC x64/x86 and disposable T273 build roots. |
| Reporting Requirements | Identify reused/copied candidates, excluded application/service semantics, machine ownership order, host-width result and remaining S4 boundary checks. |
| Stop Conditions | Candidate requires guest/provider semantics, an app/session public ABI exposes a Bochs type, or a needed ownership source lacks provenance. |
| Exit Criteria | A minimal app shell creates/destroys an opaque machine endpoint under both host widths; no app/session file adds Bochs/OpenNT service meaning beyond this assembly boundary. |
| Original Owner Request | “已有的app, session, bochs-core和adapter-*组件，可作为参考来源，在符合大局观的前提下参考实现、或复制进入，加以利用，来节省我们的大量时间。” |
| Similar-Issue Sweep | All current and quarantined app/session lifecycle candidates, direct Bochs includes, cancellation/worker/CLI/service vocabulary and test ownership. |

**S3 closure:** current neutral `session` is retained as the previously
audited replacement; the old app/session composition candidates are rejected
because they carry CLI/BOP/guest/host-service semantics. The new minimal app
shell owns only opaque machine open/teardown and passes focused x64/x86 tests.
See [S3 shell record](etc/operations/m0-t273-s3-minimal-app-machine-shell-closure-001.md).

### M0 T273 S2 closure — Mechanical lifecycle

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T273 S2`, closed; Ordinary Mode with single-person dual-role implementation and review. |
| Admission And Approval | Owner-approved Bochs foundation proposal; S1 provenance and mirror audit is closed in `a4ad30a0`. |
| Objective | Recover the smallest Bochs-only construction, reset, default-port-space, finite-run, stop and resume lifecycle through `adapter-bochs`. |
| Scope | Reviewed exact/subset Bochs files, registered private overlay boundaries, audited `adapter-bochs` lifecycle candidates, focused construction/reset/budget fixtures and evidence. |
| Non-goals | No app shell, guest loading, BOP decoding, OpenNT/DOS/WOW/SoftPC/CCPU provider semantics, Win32 service behavior or full Bochs product-shell build. |
| Verification | Per-file source/reuse audit, adapter vocabulary/dependency scan, focused lifecycle fixture under x64/x86, mirror exception review, governance and `git diff --check`. |
| Exit Criteria | An opaque minimal machine is constructed/reset/run for a finite budget/stopped/resumed by `adapter-bochs`; `bochs-core` stays project-dependency-free and all copied/modified source meets mirror rules. |
| Reference Baseline | T273 S1 recovery audit, pinned Bochs 2.6 baseline, quarantined mechanical candidates and prior Bochs exception evidence. |
| Files And ABI Surface | Reviewed `bochs-core`/private overlay files and `adapter-bochs` lifecycle API only; no caller outside the adapter receives a Bochs type. |
| Applicable Rules | Execution, source policy, mirror-component, architecture, coding, build-hygiene and documentation rules. |
| Expected Markers | Selected source manifest, private overlay isolation, one adapter-owned opaque lifecycle API and x86/x64 focused evidence. |
| Asset Needs | Pinned Bochs source, audited candidate files, CPU5 configuration evidence, MSVC x64/x86 and disposable S2 build roots. |
| Reporting Requirements | Identify every copied source, exception/overlay link, build closure and intentionally excluded Bochs product-shell feature. |
| Stop Conditions | Lifecycle needs an unregistered Bochs semantic intrusion, imports non-Bochs vocabulary, or cannot form a defined minimal object/link closure. |
| Similar-Issue Sweep | SIM/logging/PC-time/CPU decode/RAM/default-port-space/reset/run lifecycle, old adapter candidate dependencies and all core include edges. |
| Original Owner Request | Rebootstrap from zero using existing app/session/Bochs/adapter files only after global provenance and owner review, while retaining a minimal source-audited Bochs machine boundary. |

**S2 P2 closure:** the recovered generated Bochs configuration is now the
owner-approved CPU5/Pentium-MMX, non-x86-64-guest profile and selects native
pointer width correctly for each host architecture. Focused x64/x86 `/MT`
compilation passes; no minimal-machine link or runtime result is claimed. See
[S2 P2 configuration record](etc/operations/m0-t273-s2-p2-cpu5-host-width-config-closure-001.md).

**S2 P3 closure:** the Bochs-only construction/reset/decode/RAM/empty-port
space/timer-stop object closure now links and runs on both x64 and x86. It
contains no `iodev/devices.cc`, product plugin, GUI, guest or OpenNT input.
The proven bounded operation destroys its machine after the budget result, so
resume remains the next explicit S2 boundary. See [S2 P3 link record](etc/operations/m0-t273-s2-p3-minimal-machine-link-closure-001.md).

**S2 P4 closure:** a retained opaque machine can now complete two native
finite-budget runs without reconstruction, then destroy cleanly, under both
MSVC x64 and Win32/x86 `/MT`. The old broad runtime stage was audited but not
copied; only its selector-blind construction/resume skeleton is admitted. See
[S2 P4 lifecycle record](etc/operations/m0-t273-s2-p4-resumable-machine-lifecycle-closure-001.md).

The latest closed packet remains the technical baseline below.

### M0 T271 closure — OpenNT BOP source-recovery and mirror-boundary audit

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T271`, closed; all S1--S5 work was executed in Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner-approved T271 plan; final owner direction: revise the exit criterion and close the task at the audited, committed boundary. |
| Objective | Recover the admitted DEM, COMMAND and XMS source-owner packages; audit the complete BOP root and isolate the removable non-mirror bodies, while producing an explicit disposition for residual cross-component work. |
| Non-goals | No new BOP family, no DPMI/WOW/Redirector/VDD/debugger recovery, no new Bochs semantics, no trace-led leaf handler, and no unadmitted replacement of the remaining cross-component interfaces. |
| Scope | `opennt-bop`, `opennt-bop-overlay`, private include boundaries, source provenance, manifests, focused family fixtures, README divergence registers, tracker/evidence and the documented residual-owner disposition. |
| Verification | Per-file source classification and line-ledger; production external-consumer scan; focused DEM/COMMAND/XMS matrices; formal Ninja closure for committed P deliveries; governance and `git diff --check`. |
| Exit Criteria | S1--S4 source-owner recovery is committed; S5 has audited all residual non-mirror bodies/headers, moved every safely private DEM body/header, and recorded the target owner plus a future admission path for every residual. This closure does **not** claim that all residual work is completed. |
| Reference Baseline | T271 proposal; S1--S4 closures; pinned OpenNT sources; current BOP tracker and all component divergence registers. |
| Files And ABI Surface | Source layout/provenance only. Existing public selector-blind ingress and provider ABI are not expanded. |
| Applicable Rules | Source policy, source-first ladder, mirror-component standard, adapter/session boundaries, production-only roots, build hygiene and documentation governance. |
| Expected Markers | Every reviewed non-mirror input has a local marker/register row and target-owner disposition; the eliminated `ingress` tree and moved DEM-private surfaces have no production consumer. |
| Asset Needs | Pinned local OpenNT source tree, S1--S4 closures, component manifest, current README registers, fixtures and formal Ninja graph. |
| Reporting Requirements | Identify every remaining non-original file/body, its target owner, source/evidence baseline, disposition and focused verification; transfer remaining work without a false closure claim. |
| Stop Conditions | A live body lacks source provenance, a move would change a provider failure contract, or a move crosses an unadmitted owner boundary. These became residual/debt records rather than implicit follow-on work. |
| Similar-Issue Sweep | All BOP family roots, ingress, source-derived bridges, composition/session files, private overlays, manifests, tests and READMEs. |
| Original Owner Request | “opennt-bop 应当符合镜像审计标准；附属 overlay 应当极小或消失。” Final direction: “将当前T任务修改退出条件，直接收口。” |

**T271 result:** S1 non-mirror composition extraction, S2 DEM original package
recovery, S3 COMMAND original package recovery, S4 XMS original package
recovery and S5 whole-component audit/disposition are closed. The authoritative
closure record is [M0 T271 closure](history/m0-t271-closure-20260825.md).

**S3 progress:** P1 restores the reached OpenNT `MS_bop_4` entry, `cmddata.c`
globals and `CMDInit` as true source subsets. The native fixture writes an
actual `C4 C4 54 00` instruction to checked RAM and proves source entry into
the original 17-slot `CmdDispatch` table. Remaining COMMAND provider/session
composition recovery is still active; see [S3 P1 evidence](etc/evidence/m0-t271-s3-p1-command-entry-globals-recovery-001.md).

**S3 P2:** same-shaped OEM environment/drive and system-directory facades
now belong to `adapter-mvdm-host-out/win32`, not the OpenNT BOP mirror; focused direct and
native COMMAND fixtures pass. See [S3 P2 evidence](etc/evidence/m0-t271-s3-p2-command-win32-facade-extraction-001.md).

**S3 P3:** `host_lpt_flush_initialize` now belongs to selector-blind
`adapter-mvdm-host-out/softpc`, preserving OpenNT `nt_lpt.c`'s reached three-port state
clear without leaving a COMMAND composition implementation. The remaining
historical host API owner map, including the required future `adapter-mvdm-host-out/win32`
VDM API migration, is recorded in [S3 P3 evidence](etc/evidence/m0-t271-s3-command-host-api-owner-map-001.md).

**S3 P4:** the remaining temporary COMMAND composition now has a complete
per-block final-owner disposition. The next implementation order is neutral
session payload ownership, same-shaped VDM API recovery in `adapter-mvdm-host-out/win32`,
reached OpenNT host subsets, host-child lifecycle, then the generic frame
transaction. See [S3 P4 disposition](etc/evidence/m0-t271-s3-p4-command-composition-owner-disposition-001.md).

**S3 P5:** copied startup/environment/directory payload lifetime is now
neutral `session_input` state, not `opennt-bop` composition fields. Focused
get-next, dynamic-environment and native-entry regressions pass; the next P
moves the historical VDM API definitions themselves into `adapter-mvdm-host-out/win32`.
See [S3 P5 evidence](etc/evidence/m0-t271-s3-p5-session-input-extraction-001.md).

**S3 P6:** the reached historical `GetNextVDMCommand` and
`SetVDMCurrentDirectories` declarations and implementations now belong to
`adapter-mvdm-host-out/win32` as a same-shaped VDM API facade.  It preserves `VDMINFO`,
the re-entry state transitions and original caller/worker order while binding
only neutral copied `session_input` state; it does not claim the unavailable
NT4 BaseSrv/CSR broker.  Formal direct, dynamic-environment, native-session
and local-child regressions pass.  See [S3 P6 evidence](etc/evidence/m0-t271-s3-p6-vdm-api-facade-recovery-001.md).

**S3 P7:** reached original host entries `nt_std_handle_notification`,
`nt_block_event_thread`, `nt_resume_event_thread` and
`cmdPushExitInConsoleBuffer` now come from source-shaped `nt_msscs.c` /
`nt_event.c` subsets in `opennt-host`, rather than COMMAND composition.
Their original state ordering is active; VDD/display/BIOS/console-process
product-shell tails are explicitly deferred and never reported as success.
See [S3 P7 evidence](etc/evidence/m0-t271-s3-p7-command-host-event-recovery-001.md).

**S3 P8a:** the modern child record and pure host capability portions of the
reached `cmdexec.c` path now belong to `opennt-host`: stream endpoint lookup,
ANSI `CreateProcessA`, Job ownership, completion signaling and result
recording.  COMMAND retains its source call ordering and the BOP-facing
worker/re-entry seam.  The direct lifecycle fixture passes; the native
machine fixture currently stops before reaching PENDING and is separately
classified as a machine/dispatch prerequisite.  See [S3 P8a evidence](etc/evidence/m0-t271-s3-p8a-command-child-host-recovery-001.md).

**S3 P8b:** worker/event creation, completion polling/release and
dispose/cancellation now also belong to the same `opennt-host` child
capability. COMMAND retains only the original-body callback and scoped
session-input bind required for `cmdCreateProcess`; it no longer directly
creates or waits on host threads/events. See [S3 P8b evidence](etc/evidence/m0-t271-s3-p8b-command-child-worker-recovery-001.md).

**S3 P9:** the generic copied CPU-frame / checked-RAM / typed-outcome
transaction has moved from COMMAND ingress to selector-blind `adapter-mvdm-host-in`.
COMMAND retains only its original entry envelope, staged service byte and
`MS_bop_4` call. Focused transaction, native COMMAND-entry and local-child
regressions pass after a formal Ninja relink; the remaining source-shaped
COMMAND composition blocks are still active S3 work. See [S3 P9 evidence](etc/evidence/m0-t271-s3-p9-selector-blind-frame-transaction-001.md).

**S3 P10:** the last pure COMMAND Win32 helpers have moved to the existing
same-shaped `adapter-mvdm-host-out/win32` facade: hidden drive-directory environment lookup
and the unavailable private console keyboard-layout export; the existing
error-dialog facade now also exports source-shaped `RcErrorDialogBox` directly.
Console/keyboard, dynamic-environment and `cmdconf` terminal-failure
regressions pass after the formal relink. The remaining CCPU/SAS,
copied-address, worker callback, Redirector and terminal blocks remain
distinct active S3 owner work. See [S3 P10 evidence](etc/evidence/m0-t271-s3-p10-command-win32-facade-completion-001.md).

**S3 P11:** source-shaped `nt_init_event_thread` is now the reached
`opennt-host/nt_event.c` subset, which records only the admitted one-session
event initialization request. COMMAND no longer owns host-console state;
the console/keyboard regression passes after the formal relink. VDD/display/
BIOS/window/thread product-shell tails remain the existing explicit
`HOST-DIV-030` deferral. See [S3 P11 evidence](etc/evidence/m0-t271-s3-p11-command-host-event-initialization-001.md).

**S3 P12:** generic copied CCPU register access, typed result edits and
single-byte `sas_load` now belong to the selector-blind
`adapter-mvdm-host-out/softpc` frame context, not COMMAND composition.  Original source
call shapes remain private macros; COMMAND-specific `GetVDMAddr` allocation
and service ordering remain under the active source-body review.  Focused
CCPU context, direct COMMAND import and native COMMAND-entry regressions pass
after the formal relink. See [S3 P12 evidence](etc/evidence/m0-t271-s3-p12-ccpu-frame-context-extraction-001.md).

**S3 P13:** owner audit confirms that the original `cmdCreateProcess` worker
callback and `GetVDMAddr` pointer/writeback order must remain source-specific
COMMAND glue, while generic host child mechanics remain in `opennt-host`.
The last generic terminal operation, typed controlled-stop, now belongs to
the selector-blind CCPU frame context; only the original non-returning
COMMAND convention remains locally. Redirector and WOW-specific tails retain
their named future owner packages. See [S3 P13 evidence](etc/evidence/m0-t271-s3-p13-command-residual-owner-audit-001.md).

**S3 P14:** `cmdExec32` is now a registered private same-signature overlay,
not a large hidden rewrite inside the `cmdexec.c` mirror.  The mirror retains
the original callers and a small explicit crop boundary; lifecycle, child and
native-entry regressions pass after formal Ninja relink. See [S3 P14 evidence](etc/evidence/m0-t271-s3-p14-command-exec-overlay-isolation-001.md).

**S3 P15:** the generic checked guest multisz-copy loop has moved from
COMMAND composition to the selector-blind CCPU frame facade.  COMMAND retains
only the source-specific `GetVDMAddr` allocation/writeback order; CCPU and
COMMAND lifecycle regressions pass after formal Ninja relink. See [S3 P15 evidence](etc/evidence/m0-t271-s3-p15-ccpu-multisz-extraction-001.md).

**S3 P16:** the remaining project-authored COMMAND composition body is now a
mirror-private overlay behind one registered linkage boundary; no other
component consumes it.  Formal lifecycle, child and native-entry regressions
remain green. See [S3 P16 evidence](etc/evidence/m0-t271-s3-p16-command-composition-overlay-isolation-001.md).

**S3 P17:** binary, config and environment helper bodies are now registered
mirror-private overlays, leaving one-linkage boundaries in the COMMAND mirror.
Config, dynamic-environment and lifecycle regressions remain green.  The PIF
parser is separately audited by P18. See [S3 P17 evidence](etc/evidence/m0-t271-s3-p17-command-helper-overlay-isolation-001.md).

**S3 P18:** the exact OpenNT PIF parser remains in the COMMAND owner-family
mirror because it shares global state with `cmdpif.c` while the two historical
`PIF_DATA` layouts are incompatible.  Its project-authored binding is now a
registered private overlay behind the sole mirror boundary; no invalid
`opennt-host` to COMMAND dependency was introduced.  The formal PIF parser
fixture passes. See [S3 P18 evidence](etc/evidence/m0-t271-s3-p18-command-pif-owner-and-overlay-isolation-001.md).

**S3 P19:** app-only COMMAND startup/session binding has left
`opennt-bop/ingress` for `app/command_session_binding`; it now has app-owned
names and is compiled into the app library.  Original COMMAND source entry,
table and provider ownership remain unchanged. Formal native-session, PIF and
final-app links pass. See [S3 P19 evidence](etc/evidence/m0-t271-s3-p19-command-app-session-binding-extraction-001.md).

**S3 P20 / closure:** the full formal COMMAND source-provider matrix passes.
Fixture-only dialog isolation and opaque callback binding were corrected
without altering product dialogs or BOP routing. The imported `MS_bop_4` and
17-slot COMMAND table are the active dispatcher path; remaining cross-owner
capabilities are explicitly transferred. See [S3 P20 evidence](etc/evidence/m0-t271-s3-p20-command-final-matrix-and-fixture-composition-001.md)
and [S3 closure](history/m0-t271-s3-closure-20260825.md).

**S4 P1:** XMS package configuration/call composition is now a registered
private overlay behind the sole `xms/opennt_xms_composition.c` mirror boundary.
The original 12-slot XMS table and provider bodies are unchanged; formal source
and A20 fixtures pass. See [S4 P1 evidence](etc/evidence/m0-t271-s4-p1-xms-composition-overlay-isolation-001.md).

**S4 P2:** the reached `xmsa20.c` / `xmsumb.c` HIMEM state path now uses the
sole session `guest_memory` mapping-manager instance with bounded leases;
no native guest pointer survives the source call. Both formal XMS fixtures
remain green. See [S4 P2 evidence](etc/evidence/m0-t271-s4-p2-xms-a20-mapping-manager-migration-001.md).

**S4 P3:** source-unit audit classifies `xmsmemr.c` as the non-i386 RISC
variant and `i386/xmsmem86.c` as an NT4-private host-pointer implementation;
the existing same-shaped three-entry SoftPC facade is retained instead of
importing either. See [S4 P3 audit](etc/evidence/m0-t271-s4-p3-xms-source-unit-admission-audit-001.md).

**S4 P4:** non-original XMS package bind/reset lifecycle now belongs to
`app/xms_session_binding`, not `opennt-bop/ingress`; formal XMS fixtures pass
with it linked from `app.lib`. See [S4 P4 evidence](etc/evidence/m0-t271-s4-p4-xms-app-session-binding-extraction-001.md).

**S5 P1:** complete production classification finds 22 remaining project
`ingress` sources. They are now individually grouped by source owner; none is
silently moved into selector-blind `adapter-mvdm-host-in`. See [S5 P1 classification](etc/evidence/m0-t271-s5-p1-opennt-bop-production-classification-001.md).

**S5 P2:** the 13 exceptional overlay bodies each have exactly one matching
`opennt-bop` mirror include boundary; no production component outside that
mirror root includes the private overlay.  This is structural isolation only:
the 22 non-mirror ingress inputs remain active owner-disposition work.  See
[S5 P2 audit](etc/evidence/m0-t271-s5-p2-overlay-consumer-and-boundary-audit-001.md).

**S5 P3:** DEM, COMMAND and XMS one-session entry/context composition now
belongs to `app/bop/`, leaving `adapter-mvdm-host-in` selector-blind and the imported
provider tables unchanged.  Formal Ninja build plus the focused source-path
fixtures pass; the non-mirror ingress inventory drops from 22 to 13 named
deferred owner inputs.  See [S5 P3 extraction](etc/evidence/m0-t271-s5-p3-app-composition-extraction-001.md).

**S5 P4:** the `0x5E` fixed-frame wrapper for the already mirrored OpenNT
`MS_bop_E` is also app composition, not a mirror body.  It now lives in
`app/bop/`; the focused formal fixture passes and 12 non-mirror ingress bodies
remain for owner-package disposition.  See [S5 P4 extraction](etc/evidence/m0-t271-s5-p4-config-complete-app-entry-extraction-001.md).

**S5 P5:** all residual SoftPC/Redirector/top-level fixed-frame and session
wrappers now also belong to `app/bop/`; the test-only selector map moved to
tests, and `opennt-bop/ingress` is gone. Nine focused residual fixtures pass.
The final mirror/compatibility/overlay ledger audit remains active; this is not
yet S5 closure. See [S5 P5 extraction](etc/evidence/m0-t271-s5-p5-mirror-root-ingress-removal-001.md).

**S5 P6:** the root-level CCPU/SAS header was only a redundant alias of the
existing DEM source facade. It is removed; focused formal Redirector rebuild
and regression pass. The family-local compatibility/overlay ledger review
continues. See [S5 P6 audit](etc/evidence/m0-t271-s5-p6-redundant-ccpu-sas-alias-removal-001.md).

**S5 P7:** the complete family ledger confirms that all 13 private overlays
have one mirror boundary and no external consumer, and that the removed
`ingress` tree has no live consumer. It also found task-labelled divergence
markers and five non-original SoftPC fragment inputs still inside the mirror
root; both are mandatory remaining S5 remediation, so this is explicitly not
closure. See [S5 P7 ledger](etc/evidence/m0-t271-s5-p7-family-compatibility-and-overlay-ledger-001.md).

**S5 P8:** P7's remaining SoftPC fragment/marker defects are remediated:
original-name mirror boundaries now include private source fragments, typed
interrupt-06 mechanics belong to selector-blind `adapter-mvdm-host-out/softpc`, and every
remaining marker has a stable register ID. The formal five-fixture matrix,
boundary script and production-tree gate pass. Final full-family verification
and ledger refresh remain before S5 closure. See [S5 P8 evidence](etc/evidence/m0-t271-s5-p8-softpc-mirror-boundary-remediation-001.md).

**S5 P9 / closure admission:** full source-path and private-overlay matrix passes;
every remaining BOP mirror input has a registered source disposition, and no
external overlay consumer or unregistered divergence marker remains. The S5
technical exit evidence is ready for owner closure; see
[P9 final matrix](etc/evidence/m0-t271-s5-p9-final-mirror-closure-matrix-001.md).

**S5 P10:** P9's source classification is corrected: the production BOP root
still contains non-OpenNT composition/facade bodies and app still owns selector
routing. The dormant DPMI experiment has been removed from every formal
production/test input and archived as evidence. The whole-root re-audit gives
each remaining custom class a unique target owner. The subsequent
owner-directed closure transfers those moves and original-dispatcher boundary
recoveries to the debt ledger rather than claiming them complete. See
[P10 re-audit](etc/evidence/m0-t271-s5-p10-opennt-bop-mirror-reaudit-and-disposition-001.md).

**S5 P11:** an attempted test-only extraction of eight DEM composition bodies
was rejected by formal native `MS_bop_0 -> DemDispatch` linking: the original
provider table requires their same-shaped symbols. They remain production
dependencies and must be owner-split, not hidden under tests. The formal graph
now also gives intentionally shared test-support sources fixture-specific
object paths.

**S5 P12:** five non-original COMMAND overlay include stubs have been deleted.
Their private bodies are now included only by original `cmd.c`, `cmdmisc.c`,
`cmdconf.c`, `cmdenv.c` and `cmdpif.c` translation-unit boundaries. Refreshed
formal DEM/COMMAND source-path links and executions pass. The non-original
COMMAND compatibility headers are recorded as transferred source-recovery
work in the final closure.

**S5 P13:** the non-original XMS composition wrapper has been deleted and its
private body is included only by original `xms.c`. Formal XMS source-mirror
and A20 source-mirror fixtures relink and execute successfully. The remaining
`opennt_xms_compat.h` header is transferred owner-split work.

**S5 P14:** all eight non-original DEM composition `.c` bodies now live in
the private BOP overlay and are included solely by their original DEM source
unit (or its original dispatcher overlay). Refreshed formal DEM/COMMAND links
and executions pass. The remaining 17 project-named inputs are compatibility
headers requiring owner recovery, not provider/route bodies.

**S5 P15:** ten DEM-private compatibility/composition headers have joined the
eight P14 bodies below `opennt-bop-overlay`; the root now retains only seven
project-named headers with production cross-component consumers. Focused
formal DEM source-path rebuild and execution pass. Those seven headers and
the remaining app-held selector/service composition are recorded as deferred
owner-recovery work; final closure does not claim they are complete.

**S5 P16:** the two DEM root compatibility headers with no remaining app
consumer now also live under the matching private overlay.  `DemInit` retains
its original allocation/GetSystemDirectory flow through a same-shaped
adapter-mvdm-host-out/win32 facade, so app no longer writes the imported DEM global.  The
root residual set is now five cross-component headers (COMMAND, XMS and
VDMREDIR); focused formal DEM fixture rebuild and execution pass. Tests that
still include private overlay declarations remain a deferred boundary-removal
item; the owner-directed closure records it without claiming resolution.

**S2 closure:** the active `C4 C4 50 xx` route now stages its copied CPU frame
into the imported OpenNT `MS_bop_0` subset, which alone reads the guest service
byte and calls the original `DemDispatch` table.  The former project-authored
DEM ingress and its fixture are archived evidence. Fresh formal Ninja links
the complete graph; original-entry, original-table, checked-RAM, direct-host,
runtime-session and DEM provider fixtures pass. The retained DASD/IOCTL
direct-import fixture times out in its host-volume exercise and is recorded as
a machine/device-owner limitation, not passing dispatcher evidence. See the
[S2 closure](history/m0-t271-s2-closure-20260825.md).

## Closed Packet Archive

### M0 T266 S1 — repository-local `refs` consumer inventory and disposition

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T266 S1`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner objective: “执行T任务：消灭refs，实现src闭包和artifacts报告归档。” The owner-selected first queue package is admitted as T266; its former unnumbered row is removed from Queue. |
| Objective | Produce the exhaustive, reproducible ledger of every live `refs/` build, configuration, include, tool, probe, test, source and report consumer; assign each one an owned destination or an explicit deletion/archive disposition without changing runtime semantics. |
| Non-goals | No premature `refs/` deletion; no wholesale import of dormant OpenNT/Bochs trees; no BOP/provider/machine feature; no historical source alteration; no build result claimed before the later migration packets. |
| Reference Baseline | [source-closure proposal](etc/operations/proposal-repository-local-source-closure-refs-retirement-001.md), M0 T265 closure, the current formal Ninja graph, all tracked `refs/` path references, and the source policy. |
| Files And ABI Surface | Status, Queue, the T266 proposal, a machine-readable consumer/disposition manifest, source-closure evidence and documentation inventory only. No production ABI in S1. |
| Applicable Rules | Execution and documentation closure, source policy, mirror-component standard, build-output hygiene, component ownership and no external formal dependency. |
| Verification | Tracked-file and formal-manifest scans; every row has consumer class, exact source path, owner destination, disposition and S-packet; independent count/reachability review; documentation governance and `git diff --check`. |
| Expected Markers | No ambiguous live consumer; every reachable Bochs input maps to `bochs-core`/`adapter-bochs`, OpenNT input to its named component, tool to `tools/`, fixture to `tests/`, and retained report only to `artifacts/research/`. |
| Asset Needs | Existing local `refs/` snapshots for read-only inventory, current owned component roots, tracked build/test/tool sources and formal Ninja generator. No new external import. |
| Reporting Requirements | Separate live, dormant historical, provenance-only and archive/report rows; state exact file counts and migration order; identify blockers and do not call a documentation reference a runtime dependency. |
| Stop Conditions | A reachable input has no valid owner, a proposed move would violate mirror provenance, a binary lacks approval/manifest, a formal path is not reproducible, or deletion would remove an unarchived required report. |
| Exit Criteria | The manifest covers every live `refs/` consumer and all eight snapshot roots, gives a non-destructive disposition/order for each, and its evidence is internally consistent. |
| Original Owner Request | “执行T任务：消灭refs，实现src闭包和artifacts报告归档。” |
| Similar-Issue Sweep | CMake/Ninja generators, configuration projections, `tools/build`, `tools/probe`, `tools/governance`, tests, component READMEs, literal source provenance comments, `.gitignore`, and archived report inputs. |
**S1 closure:** the consumer ledger covers 49 tracked non-document consumers and all eight local `refs/` snapshot roots. It separates live formal/build/configuration input from retained historical provenance and assigns every live path to S2, S3 or S4 before S5 may remove the tree. See the [S1 ledger](etc/evidence/m0-t266-s1-refs-consumer-disposition-001.md) and [machine-readable disposition](etc/operations/m0-t266-s1-refs-consumer-disposition.tsv).

### M0 T266 S2 — repository-local Bochs formal-input recovery

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T266 S2`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T266/S1 closure and the owner-approved T266 objective. |
| Objective | Recover the exact Bochs header/configuration closure proven by the existing formal Ninja dependency database into `src/bochs-core`, then eliminate `refs/bochs` from the current formal generator, manifest and current Bochs boundary fixtures. |
| Non-goals | No Bochs semantic edit, no product-shell/device enablement, no broad Bochs import, no OpenNT migration, no historical-tool migration and no `refs/` deletion. |
| Reference Baseline | T266/S1 ledger; formal `build/M0-T265-S7/r002` Ninja dependency database; pinned local Bochs 2.6 input; `bochs-core` mirror rules. |
| Files And ABI Surface | The exact 11 dependency headers/configuration inputs, `bochs-core` provenance/README records if required, formal Ninja generator/manifest, current Bochs boundary tests and S2 evidence. No runtime ABI change. |
| Applicable Rules | Source policy, mirror-component standard, Bochs adoption/non-rewrite rule, formal-build hygiene and T266 proposal. |
| Verification | SHA-256 identity of each imported input, no `refs/bochs` formal manifest/generator/current-test path, fresh external Ninja closure and focused Bochs boundary fixtures, governance and diff checks. |
| Expected Markers | The formal graph's `showIncludes` dependencies resolve only to `src/bochs-core` plus generated build config; no direct test/formal consumer reads `refs/bochs`. |
| Asset Needs | Existing `refs/bochs` only as the read-only exact import source during S2; existing formal Ninja graph and current fixtures. |
| Reporting Requirements | List all 11 source/destination hashes, prove imported files are exact originals, distinguish current from retained historical tools, and record any missing dependency before broadening scope. |
| Stop Conditions | An input differs from its pinned source, requires an unregistered Bochs edit, exposes an additional missing header outside the known closure, changes formal machine behavior, or a test needs historical product-shell code. |
| Exit Criteria | Every known formal Bochs dependency is an exact owned input; generator/manifest/current fixtures have no `refs/bochs`; fresh formal closure and focused current tests pass. |
| Original Owner Request | “执行T任务：消灭refs，实现src闭包和artifacts报告归档。” |
| Similar-Issue Sweep | Generated config input, root/nested headers, formal include roots, CMake current target use, top-level Bochs boundary fixtures, current manifest and `showIncludes` dependency list. |

**S2 closure:** The exact eleven formal Bochs header/config inputs are now byte-identical owned src/bochs-core mirrors. The re-rooted formal graph, two focused fixture executables, and the no-work dry-run all exit 0 outside the sandbox. See [S2 evidence](etc/evidence/m0-t266-s2-bochs-formal-input-recovery-001.md).
### M0 T266 S3 — repository-local reached OpenNT input recovery

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T266 S3`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Completed T266/S2 and the owner-approved T266 objective. |
| Objective | Re-root every current guest-staging and OpenNT governance/source-inventory consumer identified by the S1 ledger to an owned named `src/opennt-*` component root; create only an auditable controlled-import requirement where the selected original is absent. |
| Non-goals | No broad OpenNT import, no BOP/provider semantic implementation, no migration of historical tools/fixtures, no source alteration, and no `refs/` deletion. |
| Reference Baseline | T266/S1 disposition ledger; source policy; named OpenNT component ownership; current guest-staging and provider-map tools. |
| Files And ABI Surface | Guest staging and governance tools, any exact selected owned original input, S3 controlled-import ledger/evidence and Status. No runtime ABI change. |
| Applicable Rules | Source policy, mirror-component standard, component ownership, build-output hygiene, documentation governance and T266 proposal. |
| Verification | Every S3-owned tool resolves no `refs/opennt`; each selected original maps to one named owner root or an explicit import row; tool outputs remain reproducible; governance and diff checks pass. |
| Expected Markers | Current guest staging, BOP/provider inventory and owner manifest use only owned `src/opennt-*` locations. Historical comparison prose may retain provenance only. |
| Asset Needs | Existing local `refs/opennt` only as read-only selection baseline; current named component roots and tools. No external fetch. |
| Reporting Requirements | List every changed current consumer, selected owner path, exact hash for every new mirror import, and each unresolved source with reason/owner/disposition. |
| Stop Conditions | A required selected original has no justified component owner, requires a non-minimal mirror divergence, changes runtime behavior, or a consumer is actually historical and belongs in S4. |
| Exit Criteria | Current guest staging and OpenNT governance/source-inventory consumers contain no `refs/opennt` input; every reached selected original is owned or explicitly controlled for later import. |
| Original Owner Request | “执行T任务：消灭refs，实现src闭包和artifacts报告归档。” |
| Similar-Issue Sweep | Staging manifests, BOP/DEM/provider inventories, owner manifest, original-source provenance comments and tool default paths. |

**S3 closure:** 57 exact selected OpenNT inputs now have named owned locations; their identity ledger records all origin paths and SHA-256 values. BOP inventory/provider/DEM map export succeeds from those local inputs; guest staging produces 113 artifacts without a `refs/` path; the current owner manifest succeeds without `refs/opennt`. See [S3 evidence](etc/evidence/m0-t266-s3-opennt-input-recovery-001.md).

### M0 T266 S4 — historical consumer retirement and research-report archive

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T266 S4`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Completed T266/S3 and the owner-approved T266 objective. |
| Objective | Eliminate every remaining tracked executable tool, probe, test, CMake/configuration and report consumer of repository-local `refs/`; retain only approved immutable research outputs below `artifacts/research/` with a reproducible archive manifest. |
| Non-goals | No `refs/` tree deletion, no production semantic change, no broad source import, no revival of historical fixture behavior, and no new BOP/machine capability. |
| Reference Baseline | T266/S1 disposition ledger; S2/S3 closures; current historical `tools/`, `tests/`, CMake and report-path scan; source and artifact policy. |
| Files And ABI Surface | Historical tool/probe/test/CMake configuration paths, report archive manifests, source provenance records, Status and evidence only. No production ABI. |
| Applicable Rules | Execution, source policy, build-output hygiene, component ownership, documentation governance and artifact/archive rules. |
| Verification | Tracked executable-consumer scan; current/historical classification review; report source/role/hash/size manifest; prove retained historical scripts no longer resolve `refs/`; governance and diff checks. |
| Expected Markers | No tracked executable tool/probe/test/configuration consumer contains a `refs/` path; each preserved report is under `artifacts/research/` with origin/disposition evidence; S5 gets a zero-consumer deletion precondition. |
| Asset Needs | Existing `refs/` snapshots read-only until S5; local historical scripts/tests/reports and source-closure ledger. No external fetch. |
| Reporting Requirements | List every retired/re-rooted historical consumer, distinguish immutable textual provenance from executable dependency, list archive files/hashes/sizes, and state any source that cannot be safely archived. |
| Stop Conditions | A historical script is still a declared formal input, a proposed archive includes executable build input or unapproved binary, a move changes semantics, or a retained report lacks source/role/hash/size provenance. |
| Exit Criteria | Every S4 ledger row has a verified re-root/retirement/archive disposition; no tracked executable historical consumer reads `refs/`; artifact archive evidence is complete; S5 has no ambiguous residual consumer. |
| Original Owner Request | “单人双角色模式执行任务：消灭refs，实现src闭包和artifacts报告归档。” |
| Similar-Issue Sweep | CMake, all `tools/` subtrees, tests, fixture preparation, historical manifests, import utilities, report generators, `.gitignore` and docs/report links. |
**S4 closure:** 41 historical inputs have a source/destination/hash disposition: 7 fixture inputs now live under `tests/legacy/reconstruction/`, and 34 former executable scripts/manifests are non-executable `docs/etc/legacy_code` evidence. The archive report/manifest is under `artifacts/research/`. Global CMake and a retained fixture have documented, pre-existing non-refs stale-name failures; neither is claimed passing. See [S4 evidence](etc/evidence/m0-t266-s4-historical-consumer-retirement-001.md).

### M0 T266 S5 — repository-local `refs` removal and zero-reference closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T266 S5`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Completed T266/S4 and the owner-approved source-closure objective. |
| Objective | Remove the repository-local `refs/` tree and its configuration treatment after proving every remaining executable/build/configuration consumer has a named local owner or S4 retirement record. |
| Non-goals | No source semantic rewrite, no historical fixture repair, no change to immutable documentation provenance, and no claim that unrelated global CMake stale paths now build. |
| Reference Baseline | T266 S1 consumer ledger; S2/S3/S4 closure evidence; current tracked path/reference scan; formal Ninja graph and source policy. |
| Files And ABI Surface | `refs/` tracked content removal, `.gitignore`, any remaining live literal consumer, Status, zero-reference evidence and documentation inventory. No runtime ABI. |
| Applicable Rules | Execution closure, source policy, build/output hygiene, artifact archive policy and documentation governance. |
| Verification | Pre-delete tracked consumer scan; exact Git removal; post-delete zero-reference scans separated into live code/config versus immutable docs provenance; repository-local formal Ninja closure and focused current fixture checks; governance/diff checks. |
| Expected Markers | No `refs/` directory; no tracked executable/configuration/include/build input names that path; only classified immutable provenance text may retain the token `refs/`. |
| Asset Needs | Current owned source roots, S1 ledger, formal Ninja generator/manifest and S2/S3/S4 evidence. No external fetch. |
| Reporting Requirements | Record deleted root count/bytes, each permitted textual provenance exception, all verification commands/results, and every unrelated failure separately. |
| Stop Conditions | A live consumer remains, a delete target is not the exact repository `refs/` root, a formal path reads external source, or a required artifact/provenance record is missing. |
| Exit Criteria | `refs/` is absent; live-zero-reference scan and repository-local formal Ninja/current fixtures pass; permitted documentation provenance is enumerated; docs/artifact manifests and final T266 closure evidence are complete. |
| Original Owner Request | “单人双角色模式执行任务：消灭refs，实现src闭包和artifacts报告归档。” |
| Similar-Issue Sweep | Git ignore/configuration, CMake, formal Ninja, `tools/`, `tests/`, component READMEs, archive manifests, root documentation and repository directory scan. |
**S5 / T266 closure:** The exact local `refs/` root is absent: 1,887 tracked files (30,364,329 bytes) were removed, followed by checked cleanup of untracked residue. Current source/tool/configuration scans have no `refs/` input; only enumerated immutable provenance text remains. Fresh formal Ninja r001 dry-run resolves 485 owned local edges. See [S5 evidence](etc/evidence/m0-t266-s5-refs-removal-zero-reference-001.md).

### M0 T265 S1 — Bochs component identity and dependency governance

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T265 S1`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner: “准入。” following the approved queue brief for Bochs component identity and mirror-overlay convergence. |
| Objective | Make the component rename, directed dependency rules, and private-overlay ownership rules authoritative before any source-tree move. |
| Non-goals | No `git mv`, source edit, Bochs/OpenNT behavior change, feature admission, or formal build execution. |
| Reference Baseline | [Bochs convergence proposal](etc/operations/proposal-bochs-component-identity-overlay-convergence-001.md), completed T264 closure, current component roots, and the mirror-component review standard. |
| Files And ABI Surface | Architecture/coding/rule authorities, queue, proposal, status, evidence and documentation inventory only; no runtime ABI. |
| Applicable Rules | Task admission and closure; component ownership; mirror/overlay review; source-first and document governance rules. |
| Verification | Cross-authority terminology/dependency scan, proposal consistency review, documentation governance, and `git diff --check`. |
| Expected Markers | Only `app` and `adapter-mvdm-host-out/softpc` call the declared `adapter-bochs` mechanical facade; only `adapter-bochs` calls `bochs-core`; only a native mirror calls its own overlay. |
| Asset Needs | Existing component-layout evidence and source-tree ownership registers; no external source import. |
| Reporting Requirements | State exact renamed component names, permitted directed edges, overlay privacy, and every deferred source move. |
| Stop Conditions | An authority implies a prohibited reverse/direct dependency, a source move becomes necessary to state policy, or an overlay needs a generic/shared consumer. |
| Exit Criteria | All listed authorities and proposal agree on final names/directions; queue no longer contains the admitted candidate; no source move exists in the S1 diff. |
| Original Owner Request | “新增队列最优先T任务，包含以下S任务：S1. 治理文档：组件改名：bx-machine => adapter-bochs，… bx-core => bochs-core，… `*-overlay` 组件仅允许被它的原生镜像组件调用。” |
| Similar-Issue Sweep | Old component names in authorities, direct core consumers, overlay reachability, and accidental OpenNT/Win32 vocabulary in the assembly component. |

**S1 closure:** `ff2d0d70` establishes the target names, direct dependency
directions and overlay privacy rule across all current authorities. The S1
diff contains no production source or build input.

### M0 T265 S2 — Bochs production-root rename

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T265 S2`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner-approved T265 proposal S2 after completed S1 governance closure. |
| Objective | Rename `src/bx-core` to `src/bochs-core` and `src/bx-machine` to `src/adapter-bochs` with `git mv`, then make every live production/build/document consumer resolve the new roots. |
| Non-goals | No source-body/ABI/behavior change, no test-history mass rename, no component reassignment beyond the two roots, and no overlay implementation. |
| Reference Baseline | T265/S1 authority `ff2d0d70`, formal Ninja manifest, root ownership scan and current exact Git paths. |
| Files And ABI Surface | Both source roots, direct include/build/manifests, focused fixture paths and authority references; pathname/build graph only, no runtime ABI. |
| Applicable Rules | T265 proposal, `git mv`, component ownership and dependency direction, source identity, production-only roots, and build hygiene. |
| Verification | Pre/post live-reference inventory, no old production-root input, regenerated formal Ninja graph, exact target build/no-work, governance and `git diff --check`. |
| Expected Markers | No live source/build input below `src/bx-core` or `src/bx-machine`; all formal component/module names and include paths resolve the renamed roots. |
| Asset Needs | Current formal manifest, generator, focused fixture set and existing Bochs provenance register. |
| Reporting Requirements | Report moved-path counts, every retained historical/test old-name exclusion, build graph repair, and whether any root content changes were required. |
| Stop Conditions | A move would alter imported content, produce a destination collision, expose an unclassified reverse dependency, or require behavior change. |
| Exit Criteria | Both roots are Git renames, source/build consumers are repaired, source identity unchanged, stated formal closure passes, and no prohibited old production path remains. |
| Original Owner Request | “S2. 实施这两个bx组件的重命名工作”. |
| Similar-Issue Sweep | Formal manifest module names, include roots, generator checks, fixture sources, README/doc links, and stale root-path string literals. |

**S2 closure:** Both production roots are recorded by Git as renames.  Live
source, formal build inputs and support-header references use
`bochs-core`/`adapter-bochs`; the historical fixture directory names are not
production roots.  Fresh `build/M0-T265-S2/r001` completed its final 471-step Ninja
closure with exit `0` and 81 linked executables.  See
[S2 evidence](etc/evidence/m0-t265-s2-bochs-production-root-rename-001.md).

### M0 T265 S3 — adapter-Bochs provenance migration

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T265 S3`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner-approved T265 proposal S3 after S2 closure. |
| Objective | Classify every `adapter-bochs` source by pinned Bochs provenance and return upstream-original, true-subset or same-shaped material to its upstream-relative `bochs-core` path or to `bochs-core-overlay` when the mirror rule requires it. |
| Non-goals | No new CPU/device/BOP/OpenNT/Win32 semantics, no generic compatibility layer, and no expansion beyond the declared `adapter-mvdm-host-out/softpc` mechanical-facade edge to `adapter-bochs`. |
| Reference Baseline | T265 proposal S3, pinned Bochs 2.6 baseline, `adapter-bochs` source inventory and T265/S2 build closure. |
| Files And ABI Surface | `adapter-bochs`, affected `bochs-core`/`bochs-core-overlay` paths, formal manifest, provenance ledger, README exception register and focused fixtures; existing mechanical ABI only. |
| Applicable Rules | Mirror identity/subset/divergence rules; private-overlay rule; `adapter-bochs` Bochs-only vocabulary; source-first provenance and formal-build closure. |
| Verification | File-by-file baseline comparison, destination collision check, include/link edge scan, README/`DIVERGENCE:` review, focused formal Ninja closure, governance and `git diff --check`. |
| Expected Markers | Every retained adapter file is self-authored Bochs assembly; every moved file has an upstream-relative mirror or private-overlay disposition and recorded provenance. |
| Asset Needs | Existing pinned Bochs reference, current component manifest and no new external import. |
| Reporting Requirements | Report each file disposition, all subset/modified markers, retained self-authored assembly, and any deferred overlay move. |
| Stop Conditions | A file needs non-Bochs vocabulary, an overlay would need an external consumer, provenance cannot be established, or behavior expansion is required. |
| Exit Criteria | `adapter-bochs` contains only self-authored Bochs-only assembly; moved mirror/overlay files and all required registration/build edges are verified. |
| Original Owner Request | “S3. adapter-bochs里面的原版镜像和镜像修改送入 bochs-core 和bochs-core-overlay”. |
| Similar-Issue Sweep | Root-level Bochs files, `iodev`/`gui` origin material, true subsets, source list ownership, and accidental direct consumers of a private overlay. |

**S3 progress:** P1/P2 returned `gui/paramtree.{cc,h}`,
`iodev/scancodes.{cc,h}`, and root `logio.cc` to `bochs-core`; see the
[provenance ledger](etc/evidence/m0-t265-s3-adapter-bochs-provenance-ledger-001.md).
The required sweep found direct `adapter-mvdm-host-out/softpc` consumption of
`adapter-bochs`/`bochs-core` objects.  S3 permits `adapter-mvdm-host-out/softpc` to consume
the declared selector-blind, fixed-width `adapter-bochs` mechanical facade,
but it must not import a `bochs-core` header, type, object or global.  This
facade correction is recorded in the proposal rather than deferred as an
undocumented exception.

**S3 closure:** `adapter-bochs` now contains only self-authored Bochs-only
assembly. P1/P2/P3 returned `paramtree`, `scancodes`, `logio`, `pc_system`,
PIC, and keyboard ownership to the proper core/overlay boundary. The focused
PIC and headless-8042 fixtures compile, link and run; the provenance ledger
records each true subset and private-overlay registration. No production
component directly includes `bochs-core-overlay`.

### M0 T265 S4 — bochs-core mirror/overlay convergence

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T265 S4`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner-approved T265 proposal S4 after S3 provenance closure. |
| Objective | Audit every differing `bochs-core` mirror file and move every over-three-executable-line semantic intrusion or over-50-percent divergence to private `bochs-core-overlay`, retaining only the smallest marked mirror boundary. |
| Non-goals | No BOP, OpenNT, DOS, VDM, WOW, Win32, device expansion, or CPU semantic expansion. |
| Reference Baseline | T265 proposal S4, local pinned Bochs 2.6 source, S3 provenance ledger and established mechanical fixtures. |
| Files And ABI Surface | `bochs-core`, `bochs-core-overlay`, `adapter-bochs` only for existing assembly calls, formal manifest, per-file ledger, README exception indexes and focused fixtures. |
| Applicable Rules | Mirror identity/subset/divergence standard; private-overlay rule; Bochs-only component vocabulary; source-first recovery and no behavior expansion. |
| Verification | Per-file baseline measurement; direct-overlay-consumer scan; `DIVERGENCE:` and README review; focused formal Ninja builds/fixtures; governance and `git diff --check`. |
| Expected Markers | Every non-exact core file is exact, true subset, same-shaped marked change, or a smallest core boundary to a private overlay. |
| Asset Needs | Existing pinned Bochs baseline, S3 formal graph and current fixture set. |
| Reporting Requirements | Record every differing file, ratio, retained core hunk, overlay location, exception ID and verification result. |
| Stop Conditions | An overlay needs a non-core caller, a source needs non-Bochs vocabulary, a behavior expansion is required, or baseline provenance is unavailable. |
| Exit Criteria | Complete per-file ledger and exception register prove no oversized self-authored body remains in `bochs-core`; private overlay is invoked only by core and focused formal evidence passes. |
| Original Owner Request | “S4. 按照镜像组件审计规则，把bochs-core里面的大幅度修改部分，送进 bochs-core-overlay”. |
| Similar-Issue Sweep | CPU #UD/observer paths, memory helpers, real-mode entry/profile, PIC/keyboard lifecycle bridges, build-root redirects and overlay privacy. |

**S4 closure:** all admitted oversized Bochs bodies now live in private `bochs-core-overlay`: #UD/first-fault, SIM-free checked RAM, real/V86 entry/profile, and all admitted copied observers. `bochs-core` has no direct external include; the overlay has no external consumer; `adapter-mvdm-host-out/softpc` has no direct Bochs type/header/global import. Formal changed-object and library links pass, and physical-write, checked-RAM, RAM-preflight and opaque-BOP-ingress fixtures exit zero. The known generic-context diagnostic exit 2 remains non-pass evidence. See the [S4 overlay audit](etc/evidence/m0-t265-s4-bochs-core-overlay-audit-001.md).

### M0 T265 S5 — opennt-host mirror/overlay convergence

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T265 S5`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner-approved T265 implementation; S4 is closed and S5 is the next approved packet in the admitted proposal. |
| Objective | Audit every differing `opennt-host` mirror file against its pinned OpenNT baseline; move only an oversized new semantic body to private `opennt-host-overlay`, while retaining valid original subsets and same-shaped minimal edits. |
| Non-goals | No BOP enablement, provider expansion, Win32/Redirector/WOW recovery, or removal of valid original source merely to improve a diff statistic. |
| Reference Baseline | T265 proposal S5, `refs/opennt`, current `opennt-host` README register and T262/T263 host evidence. |
| Files And ABI Surface | `opennt-host`, a possible private `opennt-host-overlay`, README/register, formal manifest and existing host-focused fixture only. |
| Applicable Rules | Mirror subset/modification/overlay standard; source recovery ladder; no generic compatibility owner; overlay privacy. |
| Verification | Per-file source comparison, `DIVERGENCE`/README correspondence, source-first disposition, manifest closure, focused host fixture, governance and `git diff --check`. |
| Expected Markers | Every host file is exact original, valid true subset, same-shaped minimal modification, or a smallest mirror call boundary to its own private overlay. |
| Stop Conditions | A move changes reached host behavior, requires a new generic facade, loses original control/failure semantics, or would create an overlay consumer outside `opennt-host`. |
| Exit Criteria | Complete host ledger and README register show no oversized self-authored body remains in `opennt-host`; any overlay is private; formal focused evidence passes. |
| Original Owner Request | “S5. 按照镜像组件审计规则，把 opennt-host 里面的大幅度修改部分，送进 opennt-host-overlay”. |
| Similar-Issue Sweep | Original path identity, cropped product-shell blocks, header locations, adapter boundaries, link names, direct Bochs access and false source-derived providers. |
**S5 closure:** seven host files remain byte-identical OpenNT sources; four are registered source-shaped subsets/minimal edits. Three previously oversized NT4 product-binding declaration blocks are now private `opennt-host-overlay` fragments, with no external consumer. Formal Ninja compiled the changed host objects and `t263-s8-opennt-host-mirror-fixture.exe` linked and exited zero. See the [S5 audit](etc/evidence/m0-t265-s5-opennt-host-overlay-audit-001.md).

### M0 T265 S6 — bochs-core bridge-file retirement

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T265 S6`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner: “加一个S任务做好这一条”, after reviewing the four self-authored bridge files retained in `bochs-core`. |
| Objective | Retire `cpu/opaque_callback.{h,cc}` and `iodev/keyboard_bridge.{h,cc}` from the Bochs mirror. Preserve their required minimal mechanical boundary only in existing pinned-Bochs paths; keep all self-authored state and bodies private to `bochs-core-overlay`. |
| Non-goals | No new #UD/BOP/device behavior, no CPU or keyboard semantic expansion, no public overlay API, and no adapter/OpenNT vocabulary in `bochs-core`. |
| Reference Baseline | Pinned `O:\repos.external\bochs-2.6-compat\bochs-2.6`, T265/S4 overlay audit, T265/S3 keyboard provenance ledger, and the T265 proposal S6. |
| Files And ABI Surface | The four retiring bridge files; their smallest existing Bochs-file replacement boundaries; overlay-private bodies; `adapter-bochs` facade/include consumers; formal manifest and focused fixtures. |
| Applicable Rules | Mirror component review standard; native-mirror-only overlay rule; `adapter-bochs` as sole production caller of `bochs-core`; Bochs-only vocabulary; no behavior expansion. |
| Verification | Baseline file-presence scan proves no non-upstream production `.c/.cc/.h` remains under `bochs-core`; include/link scan proves overlay privacy and no external core consumer; changed-object Ninja build plus opaque-ingress and headless-8042 fixtures; governance and `git diff --check`. |
| Expected Markers | The two bridge pairs are absent; callback registration and keyboard headless boundary are represented only by registered, minimal `DIVERGENCE:` points in original Bochs paths; overlay owns self-authored implementation. |
| Asset Needs | Existing pinned Bochs baseline and existing source/fixture inputs only; no import or new product capability. |
| Reporting Requirements | Report the exact destination original paths, each retained core hunk, deleted bridge paths, resulting non-upstream-file count, source/README exception updates, and focused verification. |
| Stop Conditions | A replacement needs more than a minimal core boundary, makes the overlay externally callable, requires a new non-Bochs file in `bochs-core`, or changes a reached machine result. |
| Exit Criteria | All four self-authored bridge files are removed; the production mirror contains only upstream-relative source names; retained boundaries are locally marked and README-registered; focused build/fixtures and governance checks pass. |
| Original Owner Request | “overlay 的目的就是为了压缩 diff… opaque_callback, keyboard_bridge… 我觉得有点过分” and “加一个S任务做好这一条”. |
| Similar-Issue Sweep | Every file under `bochs-core` absent from the pinned tree, bridge-named public headers, direct overlay consumers, manifest entries, README exception paths, and newly expanded original-file diffs. |

**S6 closure:** `bochs-core` no longer contains self-authored bridge source files. The formal Ninja `bochs-core` and `adapter-bochs` libraries build; opaque BOP ingress and the corrected two-round native 8042 lifecycle fixture exit zero. See the [S6 evidence](etc/evidence/m0-t265-s6-bochs-core-bridge-file-retirement-001.md).
### M0 T264 S1 — project code filename inventory

### M0 T265 S7 — bochs-core retained-diff consolidation

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T265 S7`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner-approved after the complete post-S6 Bochs 2.6 baseline audit. |
| Objective | Move duplicated default-off observer gates/macros into `bochs-core-overlay`; re-evaluate the keyboard creation boundary with the existing two-round lifecycle fixture; retain only required minimal core decisions, declarations and valid true subsets. |
| Non-goals | No guest/BOP/OpenNT semantics, no CPU or device expansion, no restoration of Bochs SIM/GUI/plugin product shell, no generic overlay interface, and no removal of required real/V86, #UD, RAM, PIC or lifecycle mechanics merely to improve a statistic. |
| Reference Baseline | Pinned `O:\repos.external\bochs-2.6-compat\bochs-2.6`, the S6 closure, the retained-diff audit, and the T265 proposal S7. |
| Files And ABI Surface | Existing adopted CPU/memory observer call sites, private overlay gate header and keyboard lifecycle body, formal manifest, README/register, and existing focused observer/8042 fixtures only. No public ABI is added. |
| Applicable Rules | Mirror identity/subset/divergence rule; native-mirror-only overlay rule; Bochs-only vocabulary; source-first recovery; no behavior expansion. |
| Verification | Baseline numstat before/after; direct-overlay-consumer scan; `DIVERGENCE`/README consistency; formal Ninja changed-library build; physical-write/opaque-ingress and two-round headless-8042 fixture execution; governance and `git diff --check`. |
| Expected Markers | Observer call sites no longer define local gates; any keyboard creation boundary in `keyboard.cc` is one marked call; overlay owns all moved bodies and has no external consumer. |
| Stop Conditions | Extraction changes a reached machine result, requires SIM/GUI/plugin facilities, produces a non-core overlay consumer, or cannot preserve the existing fixture lifecycle. |
| Exit Criteria | All duplicated observer scaffolding is private-overlay code; keyboard extraction is either formally verified or retained with a documented source/fixture reason; every remaining core hunk is classified, marked and README-registered; stated checks pass. |
| Original Owner Request | “支持，批准进入S任务实施以上方案，目标是进一步减少不必要的diff，送入overlay”. |
| Similar-Issue Sweep | Repeated feature-gate macros, observer no-op bodies, keyboard creation/teardown split, stale README exception descriptions, and hidden overlay consumers. |

**S7 closure:** duplicated default-off observer gate/no-op scaffolding is private `bochs-core-overlay` code, and the private keyboard lifecycle now owns both creation and teardown. Fresh formal MSVC x64 `/MT` libraries and the opaque-ingress plus two-round headless-8042 fixtures pass; see [S7 evidence](etc/evidence/m0-t265-s7-bochs-core-retained-diff-consolidation-001.md).

**T265 closure:** the S1--S7 package is closed. `adapter-bochs` is now
Bochs-only assembly; `bochs-core` retains only upstream-relative source names
and smallest registered boundaries to its private overlay; and `opennt-host`
has no oversized self-authored product-binding body. The closure makes no BOP
feature, provider, or continuous guest-runtime claim. See the
[T265 closure record](history/m0-t265-closure-20260824.md). No successor is
admitted by this closure.


| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T264 S1`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner direction: “请你执行。app, session, adapter-* 这些组件。清理”. |
| Objective | Produce the complete in-scope filename/consumer manifest needed to remove `runner`, `ntvdm64`, `bx`, and `bochs` from project-authored and mechanical-adaptation code filenames. |
| Non-goals | No content/ABI/behavior change; no rename of the six source mirrors or any `*-overlay`; no rename of an imported upstream filename merely because it is temporarily retained as a non-live input. |
| Reference Baseline | [Filename-normalization proposal](etc/operations/proposal-project-code-filename-normalization-001.md), current formal Ninja manifests, and live source/build consumers. |
| Files And ABI Surface | `app`, `session`, `adapter-mvdm-host-in`, `adapter-mvdm-host-out/softpc`, `adapter-mvdm-host-out/win32`, in-scope fixtures/build manifests, filename inventory evidence, status and documentation inventory only; no ABI change. |
| Applicable Rules | Execution closure, component ownership, mirror source identity, `git mv` requirement, and production-only component rules. |
| Verification | Recursive filename scan, include/build/document consumer scan, live-versus-imported classification review, documentation governance and `git diff --check`. |
| Expected Markers | Every in-scope matching filename has one retain/rename disposition, proposed neutral basename and consumer set; excluded original files name their source-identity reason. |
| Asset Needs | Current source tree, Git path history, formal Ninja manifest and source-root ownership registers. |
| Reporting Requirements | Report separate counts for project-authored code, fixtures/build inputs, and original/external identity exclusions; do not silently omit an adapter path. |
| Stop Conditions | A rename would alter an original mirror identity, collide with another path, or require an ABI/content change. |
| Exit Criteria | The manifest completely assigns every matching in-scope file and consumer; S2/S3 can execute only its approved `git mv` rows. |
| Original Owner Request | “我们文件名不应该包含 runner, ntvdm64，bx，bochs之类的；它们的来源都已经规范到各自的组件目录里面了。请你修复所有类似代码名字；6个镜像组件及其overlay不需要管。” |
| Similar-Issue Sweep | Imported SoftPC/MVDM headers accidentally treated as project files, stale build/source-list paths, fixture-only stale includes, forbidden mirror renames, and product artifact names mistaken for code filenames. |

The preceding T263/S9 record is closed historical context, retained below until
the status-history migration supersedes the old compact records.

### M0 T264 S2 — app/session filename normalization

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T264 S2`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T264/S1 inventory closure under the owner-approved filename-normalization task. |
| Objective | Rename the 38 `app` and four `session` project-code files from redundant former project-specific basenames to neutral owner-local names and repair direct consumers. |
| Non-goals | No source symbol, ABI, behavior, component-directory, mirror, overlay, or historical-header rename. |
| Reference Baseline | [S1 inventory](etc/evidence/m0-t264-s1-project-code-filename-inventory-001.md) and the formal Ninja source list. |
| Files And ABI Surface | `src/app`, `src/session`, direct runner/fixture/build references and naming evidence only; filename paths, no ABI. |
| Applicable Rules | T264 proposal; `git mv`; no mirror filename change; focused build closure. |
| Verification | No prohibited basename in `app`/`session`; stale-reference scan; affected formal Ninja target; `git diff --check`. |
| Expected Markers | All 42 approved paths are Git renames and all direct includes/source lists name the new paths. |
| Stop Conditions | A collision, non-path semantic replacement, or discovered upstream-source identity file pauses the rename. |
| Exit Criteria | The scoped scan is clean, direct formal targets compile, and all renamed paths remain traceable as Git moves. |
| Original Owner Request | “app, session, adapter-* 这些组件。清理”. |
| Similar-Issue Sweep | Runner tests, Ninja manifests, include paths and stale historical current-path aliases. |

**S2 closure:** all 42 approved paths were moved with `git mv`; 55 direct
consumer paths changed without a symbol or ABI rename. The fresh formal Ninja
graph at `build/M0-T264-S2/r001` compiled to completion and a final dry run had
no work. See the S1/S2 inventory evidence.

### M0 T264 S3 — adapter filename normalization

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T264 S3`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T264/S1 inventory and completed S2 under the owner-approved cleanup task. |
| Objective | Rename the 77 project-authored `adapter-mvdm-host-in` and top-level `adapter-mvdm-host-out/softpc` files from redundant former project-specific basenames to neutral owner-local names, then repair direct source, fixture and build consumers. |
| Non-goals | No rename of `adapter-mvdm-host-out/softpc/include/mvdm/**`, `include/softpc.new/**`, original mirror/overlay code, source symbols, ABI, or behavior. |
| Reference Baseline | [S1/S2 inventory](etc/evidence/m0-t264-s1-project-code-filename-inventory-001.md) and `tools/build/t260-s8-component-manifest.json`. |
| Files And ABI Surface | `adapter-mvdm-host-in`, top-level project-owned `adapter-mvdm-host-out/softpc` files, direct tests/build references and evidence only; filename paths, no ABI. |
| Applicable Rules | T264 proposal; `git mv`; source-identity exclusions; no semantic rewrite. |
| Verification | No prohibited basename in scoped project-owned adapter files; stale-reference scan; fresh formal Ninja closure; `git diff --check`. |
| Expected Markers | All 77 approved paths are Git renames and direct consumers name only their new paths. |
| Stop Conditions | A collision, an imported-source identity ambiguity, or a required non-path behavior change pauses the packet. |
| Exit Criteria | The scoped scan is clean, formal Ninja reaches no-work after completion, and historical headers remain unchanged. |
| Original Owner Request | “app, session, adapter-* 这些组件。清理”. |
| Similar-Issue Sweep | Adapter headers, formal source lists, direct component-integration fixtures and imported MVDM/SoftPC headers. |

**S3 closure:** all 77 project-owned adapter paths were moved with `git mv` and
217 live path consumers were updated without source symbol or ABI changes. The
fresh formal graph at `build/M0-T264-S3/r001` compiled to completion and a
final dry run had no work. See the S1/S2/S3 inventory evidence.

### M0 T264 S4 — filename normalization closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T264 S4`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T264/S1 inventory plus completed S2 and S3 under the owner-approved cleanup task. |
| Objective | Prove that the in-scope production component roots contain no prohibited project/backend filename prefix and that their live consumers no longer name a moved path. |
| Non-goals | No test-directory or historical-evidence mass rename; no original mirror/overlay filename or content change beyond an unavoidable consumer include-path update; no ABI/behavior change. |
| Reference Baseline | [S1/S2/S3 inventory](etc/evidence/m0-t264-s1-project-code-filename-inventory-001.md), formal Ninja manifests, and committed Git rename set. |
| Files And ABI Surface | In-scope component roots, direct consumers, closure evidence/status/history/document inventory; no ABI. |
| Applicable Rules | T264 proposal, source identity, component boundaries, documentation and closure rules. |
| Verification | Recursive basename scan; stale moved-path scan over live source/test/build inputs; formal Ninja no-work check; governance and diff checks. |
| Expected Markers | Zero prohibited basenames in project-authored in-scope component code and zero live reference to a moved filename. |
| Stop Conditions | A matching path is an unclassified live source input, a consumer cannot be updated without semantics changing, or a mirror filename would need renaming. |
| Exit Criteria | The T264 inventory is closed with scoped zero results, formal build evidence passes, unrelated historical matches are classified, and the reviewed result is committed/pushed. |
| Original Owner Request | “app, session, adapter-* 这些组件。清理”. |
| Similar-Issue Sweep | Build manifests, source-list JSON, test includes, generated graph inputs and imported-interface exclusions. |

**S4 closure:** the final recursive component scan found zero
prohibited basenames in the five in-scope production roots. The 77 staged
adapter historical basenames had zero live reference under `src`, `tests`, or
`tools/build`. The previously recorded S2 and S3 formal Ninja roots both end
at no-work dry runs after their completed builds. Documentation governance and
`git diff --check` pass. The later owner-directed temporary-suffix sweep reopens
T264 as S5; it does not invalidate the S4 product/backend-prefix result.

### M0 T264 S5 — implementation-version filename normalization

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T264 S5`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner direction: “本T任务还有一个忘了说了，增加: 文件名里面有v1 v2这玩意的也清理干净”. |
| Objective | Remove standalone temporary generation filename segments, then classify and remove non-semantic project/backend/OpenNT source-provenance identifier prefixes from all in-scope project-authored production code and repair direct consumers. |
| Non-goals | No source-symbol change where an identifier actually names DOS/VDM guest semantics, Bochs/register mechanics, an OpenNT provider/guest contract, a public configuration contract, or a preserved historical interface; no function-contract, ABI, behavior, component-directory, mirror, overlay, historical-interface, or test-directory rename. |
| Reference Baseline | T264 proposal and S1/S4 inventory; the 70-row versioned-filename scan and source token disposition audit. |
| Files And ABI Surface | `src/app`, `src/session`, `src/adapter-mvdm-host-in`, top-level project-owned `src/adapter-mvdm-host-out/softpc`, their direct consumers, and closure evidence only; private project identifiers and filename paths, no ABI. |
| Applicable Rules | `git mv`, component ownership, source-identity exclusions, production-only component rule, and no semantic rewrite. |
| Verification | Scoped recursive basename scan; stale old-path scan; identifier disposition audit; fresh formal Ninja closure; documentation governance and `git diff --check`. |
| Expected Markers | Zero standalone `v1` or `v2` filename segments; no non-semantic project/backend/OpenNT source-provenance prefix remains in scoped private identifiers; and zero live direct reference to moved versioned paths. |
| Stop Conditions | A destination collision, imported-source identity ambiguity, or a consumer that requires a semantic change rather than an exact path replacement. |
| Exit Criteria | All 70 inventoried paths are Git moves; non-semantic identifier prefixes are removed with direct consumers repaired; excluded historical headers remain unchanged; each retained lexical term has an auditable semantic disposition; and stated checks pass. |
| Original Owner Request | “文件名里面有v1 v2这玩意的也清理干净”. |
| Similar-Issue Sweep | Filename-only version suffixes versus actual source symbols, fixture target names, source-list JSON, and retained historical interface paths. |

**S5 P1 result:** all 70 in-scope temporary-suffix filenames were refined with
`git mv`, and the exact source/test/build path consumers were repaired. The
reviewed non-semantic private project-prefixed and source-provenance
`opennt_*` identifier families were normalized while strings/comments,
original-interface names, DOS/VDM/Bochs mechanics, and OpenNT contract-facing
facades were retained. A synchronous formal Ninja attempt reached the genuine
compiler boundary but is blocked before project-owned targets complete by the
pre-existing `FLOAT128` macro collision in Bochs FPU source when the current
Windows SDK's `winnt.h` is present. This preliminary diagnosis was corrected
by S5 P2: the collision was caused by T264's newly generic app-local
`config.h` shadowing Bochs's generated `<config.h>` in the global include
search order. `config.h` was renamed to `bundle_config.h`, the one non-app
consumer was repaired, and the fresh formal graph completed before a final
`ninja -n` reported no work. See the S5 evidence update and the T264 closure
record.

### M0 T263 S9 — OpenNT-host three-file original-body recovery

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T263 S9`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner direction: “opennt-host这3个文件肯定都不及格了，打回重写吧…批准，请你开始实施。” |
| Objective | Rebuild `nt_error.c`, `vrinit.c`, and `vrnmpipe.c` from their exact OpenNT originals: retain the reached original function bodies, associated data and explanatory comments; delete unconnected function blocks with their dedicated support/commentary; use only statement-local same-shaped `adapter-mvdm-host-out/win32` or `adapter-mvdm-host-out/softpc` substitutions where a retained call cannot compose. |
| Non-goals | No VDD/NetBIOS/DLC/ICA/CSR/WOW product-shell recreation; no new BOP service admission; no fabricated `VrInitialize` success; no Bochs semantic change. |
| Reference Baseline | `O:\\repos.external\\OpenNT\\base\\mvdm\\{softpc.new\\host\\src\\nt_error.c,vdmredir\\vrinit.c,vdmredir\\vrnmpipe.c}`, current reached callers, `HOST-DIV-012..025`, and the source-first recovery rules. |
| Files And ABI Surface | The three host mirrors; needed declared `adapter-mvdm-host-out/win32`/`adapter-mvdm-host-out/softpc` same-shaped facades; focused host/Redirector fixtures; host README/evidence/status. `adapter-mvdm-host-in` remains selector-blind ingress only and is not a provider dependency. |
| Applicable Rules | Source-first ladder; original body/order/layout preservation; statement-local divergence/register rule; no fabricated success; component ownership and bounded mapping rules. |
| Verification | Per-retained-block original-span ledger; source diff review; positive and negative focused fixture coverage; formal Ninja closure; documentation governance; `git diff --check`. |
| Expected Markers | Each retained function has a cited original span and unchanged body except registered adapter call sites; every deleted block names its unavailable dependency and owner; `VrInitialize` either retains its required original prerequisites or is not admitted. |
| Asset Needs | Pinned OpenNT sources, existing public-Win32 facade, bounded SoftPC/CCPU facade, session-owned handle/mapping services, formal Ninja graph. |
| Reporting Requirements | Separately report original retained lines, adapter substitutions, deleted unconnected blocks, and any capability that must remain unavailable. Do not call a cropped function connected if its original success prerequisites are absent. |
| Stop Conditions | A retained block would require a new product-shell adapter, an adapter would acquire BOP/Redirector semantics, an original success path cannot be preserved, or a crop bisects retained control flow. |
| Exit Criteria | The three files are reviewable original-body subsets; no project-authored replacement algorithm remains in a retained block; all substitutions are local and README-indexed; no incomplete historical success is exposed; stated checks pass. |
| Original Owner Request | “原版文件，保留大段注释部分，删去不需要接通的那些部分及配套注释；保留所有需要接通的部分；然后，这些部分如果依赖 adapter-mvdm-host-out/win32, adapter-mvdm-host-in, adapter-mvdm-host-out/softpc，就在对应地方实现需要的替代接口。” |
| Similar-Issue Sweep | Rewritten same-name source, blanket divergence claims, include-only but behavior-changing edits, hidden fabricated success, unbounded host pointers, and adapter ownership leaks. |

**S9 closure:** all three files were re-rooted from cited original source
spans. `nt_error.c` retains the original direct-access body through the
same-shaped dialog/TLS facade, with only pointer-width-safe transport casts;
`vrinit.c` retains only original `VrInitialized` and no longer reports an
unsupported initialization success; and `vrnmpipe.c` retains the original
synchronous interceptors and source-order list helpers. The former Redirector
`57:00/57:01` consumers and `LoadVdmRedir` now return explicit failure until
their full original lifecycle can be admitted, rather than reviving deleted
host success stubs.

The fresh `build/M0-T263-S9/r001` formal Ninja graph reached `no work to do`.
Its `opennt-host` mirror, BOP 59, DEM file, and Redirector disposition fixtures
all exited `0`. The detailed retained/deleted/dependency and verification ledger
is [S9 recovery evidence](etc/evidence/m0-t263-s9-three-file-original-body-recovery-ledger-001.md).

### M0 T263 S8 — closed: reachable OpenNT host-source similarity repair

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T263 S8`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner direction: “你现在增开一个S任务做这个修复。” This reopens T263 after S7 closure solely for the four composed host subsets. |
| Objective | Restore OpenNT structure/logic for every currently admitted feature block in `nt_error.c`, `vrinit.c`, `vrnmpipe.c`, and `vrputil.c`; retain only statement-local adapter seams and explicit unconnected feature-block crops. Similarity is judged per needed logical block: restore the original module and algorithm comments together with a retained block where practical, but do not import otherwise-unreachable product code merely to raise a byte count. |
| Non-goals | No dormant VDD/debug/FAX/WOW package import, no new BOP family, no Bochs semantic change, no recreation of NT4 CSR/CCPU product shells and no fabricated unavailable success. |
| Reference Baseline | `refs/opennt/base/mvdm/{softpc.new/host/src/nt_error.c,vdmredir/vrinit.c,vdmredir/vrnmpipe.c,vdmredir/vrputil.c}`, `HOST-DIV-012..024`, and T263/S7 P2 withdrawal record. |
| Files And ABI Surface | The four host mirrors, their original/reached headers, same-shaped `adapter-mvdm-host-out/win32`/`adapter-mvdm-host-out/softpc` facade declarations and implementations, focused fixtures, README/evidence/status; no new cross-component ABI shape. |
| Applicable Rules | Source-first ladder; component production-only rule; original order/layout/failure preservation; adapter ownership; explicit `DIVERGENCE` and README one-to-one register. |
| Verification | Per-file original block/line ledger; include/call graph; source similarity review excluding explicit crops; formal Ninja closure; focused positive/negative fixture for every retained provider block. |
| Expected Markers | Each retained feature block maps to an original span; each platform departure has a same-shaped adapter call and `HOST-DIV` row; each crop names source span, missing dependency and future owner. |
| Asset Needs | Pinned OpenNT source, current adapter/session interfaces, MSVC/Ninja formal graph and existing focused host fixtures. |
| Reporting Requirements | Distinguish exact original blocks, local adapter call substitutions and omitted whole feature blocks; do not state an unconnected historical feature is runnable. |
| Stop Conditions | A required adapter would absorb Redirector/DOS/BOP meaning, a crop bisects a retained control-flow block, an unregistered divergence appears, or a dormant source package would be imported. |
| Exit Criteria | The four source files contain no project-authored replacement algorithm for an admitted block; all retained blocks preserve original control/data/failure structure through same-shaped facades; crops are source-cited and locally tested; formal closure passes. No unneeded historical feature is restored merely to raise line similarity. |
| Original Owner Request | “允许裁剪掉暂时没有联通的部分，但是联通的部分不能大规模 diverge。” |
| Similar-Issue Sweep | Adapter logic leaking into mirrors, partial-block crops, replacement data structures, private Bochs access, hidden host-pointer state and untested original failure paths. |

**S8 P2 complete:** original module/algorithm comments and the reached
direct-access source form are restored for all four host subsets; the four
formal MSVC object targets rebuilt cleanly. The exact retained/deferred split
is recorded in the [P2 recovery
evidence](etc/evidence/m0-t263-s8-p2-retained-host-comment-and-block-recovery-001.md).

**S8 P3 complete:** a fresh formal graph rooted at `build/M0-T263-S8/r001`
linked and ran the focused `opennt-host` mirror fixture with exit `0`. It
exercises the retained path/error helpers, static provider lifecycle, and a
local named-pipe round trip without relying on the unrelated VDMREDIR mailslot
fixture branch. See the [P3 focused-closure
evidence](etc/evidence/m0-t263-s8-p3-focused-host-mirror-fixture-001.md).

**S8 closure:** the four-file repair now satisfies its exit criteria. The
`nt_error.c` composed BOP 59 route and the direct VDMREDIR mirror fixture both
exit `0`; all retained local divergences are marked in source and indexed in
`src/opennt-host/README.md`. Deferred VDD/remote/mailslot work remains out of
scope, source-cited and owner-bound rather than being represented as a failure
of the restored synchronous host subset.

**S8 P4 complete:** the owner-required source-layout sweep restored original
whitespace/layout wherever a retained line has no semantic divergence, and
removed historical includes or optional diagnostics that were merely present
but not actually connected. The resulting exact/semantic/cropped split and
fresh focused fixture result are recorded in the [P4 formatting and dependency
evidence](etc/evidence/m0-t263-s8-p4-mirror-layout-and-reached-dependency-audit-001.md).

**S8 P4 complete:** the owner-required source-layout sweep restored original
whitespace/layout wherever a retained line has no semantic divergence, and
removed historical includes or optional diagnostics that were merely present
but not actually connected. The resulting exact/semantic/cropped split and
fresh focused fixture result are recorded in the [P4 formatting and dependency
evidence](etc/evidence/m0-t263-s8-p4-mirror-layout-and-reached-dependency-audit-001.md).

### M0 T263 S7 — MVDM owner-expansion closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T263 S7`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T263 proposal S7 after completed S1 through S6. |
| Objective | Reconcile all T263 source-preservation imports, component ownership/README registers and default formal-build non-admission. |
| Non-goals | No new runtime feature, BOP implementation, tool invocation, static library, firmware activation or source behavior change. |
| Reference Baseline | T263 S1–S6 evidence; OpenNT Git `5c5b979e`; formal root `build/M0-T262-S5/001`. |
| Files And ABI Surface | T263 closure evidence/history/status and documentation inventory only; no ABI. |
| Applicable Rules | Execution closure, source policy, original mirror rule, production-only roots, no generic component and formal-build hygiene. |
| Verification | Per-packet SHA-256 records, source-owner scan, README register review, `ninja -C build/M0-T262-S5/001 -n`, documentation governance and scoped diff checks. |
| Expected Markers | Four admitted production package groups plus one non-component tool closure, six original component registers, no added formal object/archive/link owner. |
| Asset Needs | Pinned OpenNT source, all T263 evidence, existing formal Ninja root and Git metadata. |
| Reporting Requirements | State exact admitted counts, build-boundary result and deferred original owner packages; ensure dormant host extension packages remain outside production components. |
| Stop Conditions | Missing owner/provenance, unregistered divergence, a generic-root leak or accidental new build edge pauses closure. |
| Exit Criteria | Every admitted package has exact source/hash evidence and named owner; no new component crosses the mechanical/runtime boundary; closure history is recorded. |
| Original Owner Request | “将以上架构修改，进行治理文档修正，并开始着手建立 opennt-guest 完整组件、opennt-softpc、opennt-utils 组件，并做好 tools/opennt 工具增补。” |
| Similar-Issue Sweep | Missing package provenance, source mirror mixed with tests/reference copies, accidental runtime linkage, generic utility growth and firmware/runtime conflation. |

**S7 P1 completed:** T263 package counts, owner registers and the unchanged
formal graph boundary are reconciled in the [closure evidence](etc/evidence/m0-t263-s7-mvdm-owner-expansion-closure-001.md).

**S7 P2 completed:** the mistakenly preserved-but-unconnected VDD/debug/FAX
packages have been withdrawn from `opennt-host`; their future owner remains
recorded without treating source preservation as production admission. See the
[withdrawal record](etc/evidence/m0-t263-s7-unconnected-host-package-withdrawal-001.md).
T263 is closed; see the [history record](history/m0-t263-closure-20260824.md).

### M0 T263 S6 — OpenNT `tools16` historical tool-closure admission

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T263 S6`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T263 proposal S6; owner classified `tools16` under non-component `tools/opennt`. |
| Objective | Preserve and source-verify the complete original `tools16` package as a potential guest-build tool closure. |
| Non-goals | No modern-host link, Ninja edge, implicit tool invocation, guest rebuild or migration of unrelated historical tool material. |
| Reference Baseline | `O:\\repos.external\\opennt\\base\\mvdm\\tools16`, Git `5c5b979e`; T263/S1 census. |
| Files And ABI Surface | Seven original historical tool inputs, tool README, S6 evidence and status only; no product ABI. |
| Applicable Rules | Source policy, historical-tool isolation, guest build provenance and no-runtime-link rule. |
| Verification | File count plus SHA-256 equality for all seven files; formal graph remains unchanged. |
| Expected Markers | Seven exact inputs; zero runtime consumers; no Ninja graph expansion. |
| Asset Needs | Pinned OpenNT source root and Git provenance. |
| Reporting Requirements | Record the deferred guest-build consumer and distinguish `tools/opennt` from unrelated historical tool material. |
| Stop Conditions | Hash/path mismatch, tool linked into modern runtime, unapproved invocation or unrelated tool migration pauses the packet. |
| Exit Criteria | Package is exact, README/evidence agree and no runtime/build behavior is inferred. |
| Original Owner Request | “`tools16`：历史编译工具资料…属于 tools/opennt，不是组件。” |
| Similar-Issue Sweep | Historical tool mistaken for host runtime input, hidden tool invocation, provenance collision and broad historical-directory cleanup. |

**S6 P1 completed:** all seven `tools16` files are source-identical and
SHA-256 verified. See the [admission evidence](etc/evidence/m0-t263-s6-opennt-tools16-closure-admission-001.md).

### M0 T263 S5 — OpenNT optional host extension-package preparation

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T263 S5`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T263 proposal S5; owner classified VDD, debugger and fax extension packages as `opennt-host`. |
| Objective | Retain qualifying original host product packages source-identically with their original internal paths and an explicit non-link disposition. |
| Non-goals | No VDD/debugger/fax/WOW runtime capability, default link, product-shell recreation, adapter creation or BOP implementation. |
| Reference Baseline | `O:\\repos.external\\opennt\\base\\mvdm\\{bde,dbg,fax,ieuvddex,vdd,vdmdbg,vdmexts}`, Git `5c5b979e`; T263/S1 census. |
| Files And ABI Surface | 138 source/input files across seven original host packages, host README, S5 evidence and status only; no ABI change. |
| Applicable Rules | Source policy, original host mirror rule, optional-provider ownership and no preemptive adapter rule. |
| Verification | Recursive source-relative counts and SHA-256 equality for all seven packages; no added Ninja edge. |
| Expected Markers | 138 exact files, zero new `HOST-DIV` entries, zero default link/runtime capability claim. |
| Asset Needs | Pinned OpenNT source root and Git provenance. |
| Reporting Requirements | Record package counts, exact owner and explicit absence of a capability claim. |
| Stop Conditions | Hash/path mismatch, host package mixed into a mechanical adapter, source change without register, new default link or runtime claim pauses the packet. |
| Exit Criteria | All seven original packages are exact and indexed, host README/evidence agree, and no product behavior expands. |
| Original Owner Request | “`vdd`、`ieuvddex`、`fax`…属于 opennt-host；`bde`、`dbg`、`vdmdbg`、`vdmexts`…同理。” |
| Similar-Issue Sweep | Debug source mistaken for Bochs logic, VDD package auto-enable, new generic adapter, host-source divergence and implicit link drift. |

**S5 P1 superseded:** the seven original host extension packages were verified
source-identical, but the owner subsequently ruled that unconnected packages
must not reside in production `opennt-host`. They were withdrawn in T263/S7
P2; their earlier provenance record remains
[historical evidence](etc/evidence/m0-t263-s5-opennt-host-extension-package-admission-001.md).

### M0 T263 S4 — OpenNT reusable utility-package decision

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T263 S4`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T263 proposal S4; MVDM census identifies `suballoc` as a reusable original utility and rejects a bulk `vdmutils` move. |
| Objective | Import the complete source-identical `suballoc` package into `opennt-utils` and record its original consumers and deferred compilation boundary. |
| Non-goals | No new allocator, modern replacement, XMS/DPMI implementation, static library, default link or mixed `vdmutils` import. |
| Reference Baseline | `O:\\repos.external\\opennt\\base\\mvdm\\suballoc`, Git `5c5b979e`; T263/S1 census. |
| Files And ABI Surface | Eight original utility files, component README, S4 evidence and status; no product ABI. |
| Applicable Rules | Source policy, source-first utility ownership, original mirror zero-difference rule and no generic common root. |
| Verification | Same-relative-path count plus SHA-256 equality for all eight files; source consumer scan; no Ninja graph change. |
| Expected Markers | Eight exact files; zero exceptions; `xms.486`/`dpmi32` recorded as future original consumers. |
| Asset Needs | Pinned OpenNT source root and Git provenance. |
| Reporting Requirements | Record the exact split decision for `vdmutils` as well as `suballoc` provenance. |
| Stop Conditions | Any utility without a named original consumer, mixed-product bulk import, local edit or build/link expansion pauses the packet. |
| Exit Criteria | `suballoc` is exact and indexed; `vdmutils` has explicit per-product disposition; no substitute allocator is introduced. |
| Original Owner Request | “`suballoc`、`vdmutils`：历史辅助库和独立工具…缺少一个 opennt-utils 的组件。” |
| Similar-Issue Sweep | Generic common-code growth, XMS/DPMI semantics moved into utilities, inferred runtime link and mixed guest/host product ownership. |

**S4 P1 completed:** all eight `suballoc` files are source-identical and
SHA-256 verified; mixed `vdmutils` remains explicitly split. See the
[admission evidence](etc/evidence/m0-t263-s4-opennt-utils-suballoc-admission-001.md).

### M0 T263 S3 — OpenNT SoftPC firmware/ROM initial mirror

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T263 S3`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T263 proposal S3; owner approved `softpc.new/bios` and `roms` as the new component's first package boundary. |
| Objective | Import and source-verify the original firmware/ROM source/input packages below `opennt-softpc`. |
| Non-goals | No historical SoftPC runtime, Bochs change, adapter behavior, BIOS activation, default load, static library or formal-build edge. |
| Reference Baseline | `O:\\repos.external\\opennt\\base\\mvdm\\softpc.new`, Git `5c5b979e`; T263/S1 census. |
| Files And ABI Surface | 27 `bios` and 8 `roms` source/input files, component README, S3 evidence and status only; no host ABI. |
| Applicable Rules | Source policy, original mirror zero-difference/README register rule and Bochs/OpenNT hard boundary. |
| Verification | Recursive same-relative-path count plus SHA-256 equality for all 35 files; no local divergence. |
| Expected Markers | 35 exact files, zero local exceptions and zero Ninja graph expansion. |
| Asset Needs | Pinned OpenNT source root and Git provenance. |
| Reporting Requirements | State source package counts, opaque-input boundary and absence of a machine-runtime assertion. |
| Stop Conditions | Hash/path mismatch, unregistered source change, machine semantic change, `bx-machine` OpenNT dependency or default link admission pauses the packet. |
| Exit Criteria | Both complete original packages are exact, README/evidence agree and architecture boundary remains unchanged. |
| Original Owner Request | “`softpc.new/bios`、`roms`：属于机器固件/装配输入…应该放在 opennt-softpc 模块。” |
| Similar-Issue Sweep | Firmware mistaken for an emulator, default BIOS activation, OpenNT naming leaked into machine/core and build-input provenance drift. |

**S3 P1 completed:** 27 `bios` plus 8 `roms` files were copied
source-identically and recursively SHA-256 verified. See the [admission
evidence](etc/evidence/m0-t263-s3-opennt-softpc-firmware-rom-admission-001.md).

### M0 T263 S2 — OpenNT `bin86` guest-package admission

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T263 S2`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T263 proposal S2; owner classified `bin86` as guest-owned. |
| Objective | Import the complete source-identical original `base/mvdm/bin86` package below `opennt-guest` and record its source/hash identity. |
| Non-goals | No driver/configuration auto-loading, guest behavior change, host provider, build-graph expansion or source edit. |
| Reference Baseline | `O:\\repos.external\\opennt\\base\\mvdm\\bin86`, Git `5c5b979e`; T263/S1 census. |
| Files And ABI Surface | Ten guest companion/input files, guest README, S2 evidence and status only; no host ABI. |
| Applicable Rules | Source policy, guest mirror zero-difference rule, production-only root rule and artifact/input provenance rule. |
| Verification | File count plus SHA-256 equality for every source/target pair; no local divergence row. |
| Expected Markers | Ten exact files; zero guest-source divergence entries; zero formal-Ninja edges added. |
| Asset Needs | Pinned OpenNT source root and Git provenance. |
| Reporting Requirements | Record every file role and state that runtime use remains separately admitted. |
| Stop Conditions | Missing/hash-mismatched file, non-guest owner, local edit or accidental runtime-link input pauses the packet. |
| Exit Criteria | Complete package is source-identical, README/evidence agree and no default runtime behavior changes. |
| Original Owner Request | “`bin86` => 如果属于 opennt-guest，应予搬迁。” |
| Similar-Issue Sweep | Configuration input mistaken for an auto-loaded boot file, host-provider leakage, source/build companion loss and artifact provenance drift. |

**S2 P1 completed:** all ten `bin86` files were copied source-identically and
SHA-256 verified. See the [admission evidence](etc/evidence/m0-t263-s2-bin86-guest-input-admission-001.md).

### M0 T263 S1 — OpenNT MVDM owner/provenance census and component-root admission

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T263 S1`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner approved the addition of complete `opennt-guest`, `opennt-softpc`, `opennt-utils` and `tools/opennt` recovery, following the MVDM ownership discussion. |
| Objective | Establish a reproducible owner/provenance census, ratify the 12-component architecture and create empty production-only roots with local exception registers. |
| Non-goals | No wholesale MVDM import, BOP implementation, Bochs semantic change, guest behavior change or default Ninja-link expansion. |
| Reference Baseline | `O:\\repos.external\\opennt` Git `5c5b979e`; OpenNT-4.5 Git `98bd914d2`; source policy; T262 mirror closure. |
| Files And ABI Surface | Architecture/coding rules, T263 proposal/evidence, new component/tool READMEs and status only; no product ABI. |
| Applicable Rules | Source-first recovery, original mirror/README exception rules, production-only component roots and formal-build hygiene. |
| Verification | 24-package MVDM census, source identity check, component owner scan, documentation governance and scoped diff checks. |
| Expected Markers | Six original-code component READMEs; 12 named component owners; zero source bodies admitted by skeleton creation. |
| Asset Needs | Pinned OpenNT and OpenNT-4.5 comparison trees and Git provenance. |
| Reporting Requirements | Record every top-level MVDM package's owner/disposition and identify the next per-package import steps. |
| Stop Conditions | An import without unique owner/provenance, an unregistered mirror divergence, a generic catch-all component, or a new default link dependency pauses the packet. |
| Exit Criteria | Census/evidence, proposal, architecture/rule updates and empty roots agree; every future import has an explicit packet and owner. |
| Original Owner Request | “建立 opennt-guest 完整组件、opennt-softpc、opennt-utils 组件，并做好 tools/opennt 工具增补。” |
| Similar-Issue Sweep | Misowned `mvdm` files, firmware mistaken for a second emulator, utility catch-all growth, host-extension auto-linking and historical tools leaking into runtime. |

**S1 P1 completed:** the 24-package MVDM provenance census, 12-component
ownership model and empty component/tool roots are recorded in the
[S1 evidence](etc/evidence/m0-t263-s1-mvdm-owner-provenance-census-001.md).

### M0 T262 S5 — cross-component final mirror audit and formal-build regression

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T262 S5`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T262 proposal S5, after completed S2, S3 P2 and S4. |
| Objective | Re-run the three mirror classifications and local exception-index checks, then source-build the formal nine-module graph without changing component boundaries. |
| Non-goals | No BOP/provider implementation, no Bochs semantic edit, no guest rebuild, no host-subset expansion, no cleanup of unrelated historical tools or artifacts. |
| Reference Baseline | Pinned Bochs 2.6; OpenNT and OpenNT-4.5 guest roots; named OpenNT host paths; S2/S3/S4 evidence and `tools/build/t260-s8-component-manifest.json`. |
| Files And ABI Surface | The three component trees, their READMEs, T262 evidence/status/history and the disposable formal Ninja root only; no product ABI change. |
| Applicable Rules | Mirror classification/marker/register rules, source policy, formal-build hygiene and documentation-governance closure rules. |
| Verification | Full hash/origin scans; bx-core local-marker scan; host local-marker/README set equality; new `build/M0-T262-S5/001` formal Ninja graph; #UD boundary and focused host fixtures; documentation governance and scoped diff checks. |
| Expected Markers | 164/18/0 bx-core exact/edit/non-mirror classification; 678/0 matched/unmatched guest inputs; 7/4/0 host exact/subset/extra classification; equal HOST-DIV code/README sets. |
| Asset Needs | Pinned Bochs/OpenNT trees, the component manifest, Ninja/MSVC x64 `/MT`, focused fixture executables and Git metadata. |
| Reporting Requirements | Record exact audit totals, build root/edge result, passed fixtures and any failed fixture with an owner-qualified limitation. |
| Stop Conditions | An unmatched path, unmarked/local-register drift, component-boundary change, or source-build failure pauses closure. |
| Exit Criteria | Every production file has an allowed mirror classification; no component has an unregistered exception/non-mirror file; formal graph compiles, archives and links; evidence records any unrelated runtime fixture limitation without misclassifying it as a mirror defect. |
| Original Owner Request | “审核已知的3个镜像包： bx-core, opennt-guest, opennt-host…所有子集文件或者少部分修改的点位，需要在本文件中加入注释，并且README.md提供该例外的索引。” |
| Similar-Issue Sweep | Imported guest binaries/objects, source-root version collision, local README/register drift, stale component paths, build-output leakage and fixture/environment conflation. |

**S5 P1 completed:** 182 `bx-core` source files classify as 164 exact plus
18 locally indexed Bochs edits; all 678 `opennt-guest` inputs match an OpenNT
or OpenNT-4.5 peer; `opennt-host` remains seven exact plus four
marker/index-equal subsets.  A fresh formal graph completed all 469 edges and
emitted 80 executables.  The #UD boundary and BOP59 fixtures pass.  The
Redirector mailslot fixture returns its pre-existing environment-sensitive
exit `8` in both the new and prior formal roots, which is recorded rather than
misattributed to the mirror task.  See the [S5 closure evidence](etc/evidence/m0-t262-s5-final-component-mirror-and-build-closure-001.md).

### M0 T262 S4 — opennt-host subset and minimal-edit closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T262 S4`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T262 proposal S4; T262/S1 identified four original-host subsets/minimal edits and 13 existing HOST-DIV rows. |
| Objective | Prove one-to-one code-marker/README coverage for every non-exact `opennt-host` file, while retaining exact files byte-identical and excluding non-mirror additions. |
| Non-goals | No provider behavior, BOP route, Win32 facade, SoftPC seam or historical source expansion. |
| Reference Baseline | T261/S12 header closure, T262/S1 ledger, `refs/opennt` source paths, and `opennt-host/README.md`. |
| Files And ABI Surface | `nt_error.c`, `vrinit.c`, `vrnmpipe.c`, `vrputil.c`, exact host headers/bodies, README and evidence/status only. |
| Applicable Rules | Reachable original subsets retain original order; each omitted/replaced expression has local `DIVERGENCE(<ID>)`; each ID has exactly one README row. |
| Verification | Hash exact files; enumerate marker IDs per subset; compare every README HOST-DIV ID to source; negative scan for non-mirror files/compat headers; focused host fixtures remain linked. |
| Expected Markers | Seven exact files, four subset/edit files, 13 matching HOST-DIV entries, and zero non-mirror host files. |
| Asset Needs | Current host mirror, original OpenNT files and existing S12 fixture graph. |
| Reporting Requirements | Report one-to-one marker/index result and preserve any unresolved source span as a later owner dependency rather than silently omitting it. |
| Stop Conditions | A README ID lacks a local marker, a local marker lacks a README row, or a subset contains an unmarked project-authored branch. |
| Exit Criteria | All host subset/edit deviations are locally marked/indexed and exact files remain hash-identical. |
| Original Owner Request | “所有子集文件或者少部分修改的点位，需要在本文件中加入注释，并且README.md提供该例外的索引。” |
| Similar-Issue Sweep | `*_compat` names, static-DLL selectors, TLS/dialog state, VDMREDIR lifecycle, private-header replacements and test-only declarations. |

**S4 P1 completed:** exact-host hashes, subset marker IDs, README IDs and
production-tree inventory agree one-to-one. See the [S4
closure](etc/evidence/m0-t262-s4-opennt-host-subset-marker-closure-001.md).

### M0 T262 S3 — opennt-guest zero-difference mirror closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T262 S3`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T262 proposal S3 and the owner-approved mirror standard; T262/S1 proves `sysmac.lib` is exact OpenNT material but is untracked. |
| Objective | Make the `opennt-guest` mirror complete and versioned while preserving byte-identical DOS/V86 and WOW16 source/input trees and their original deployable binaries. |
| Non-goals | No guest source edit, guest rebuild, artifact replacement, toolchain change, packaging policy or use of unrelated historical tools. |
| Reference Baseline | T262/S1 hash audit; `refs/opennt/base/mvdm/dos/v86` and `refs/opennt/base/mvdm/wow16`; `opennt-guest/README.md`. |
| Files And ABI Surface | The exact `sysmac.lib` input plus owner-copied original OpenNT DOS/V86 `*.COM`, `*.EXE`, `*.SYS`, `*.BIN`, `*.LIB`, `*.MAP`, `*.SYM` and `*.OBJ` inputs under the guest source root; guest README, T262 evidence/status and documentation inventory only. |
| Applicable Rules | Original guest inputs are byte-identical; zero source changes means zero local divergence entries; artifact provenance remains explicit. |
| Verification | Full guest-path hash comparison, direct hash proof for `sysmac.lib` and every owner-copied binary, build library and MAP/SYM companion, Git tracked-file check, no `DIVERGENCE` marker scan, documentation governance and scoped diff check. |
| Expected Markers | 580/580 source/input paths and all 99 admitted DOS/V86 package/build inputs match OpenNT or OpenNT-4.5 and are tracked; no non-mirror or modified guest file exists. |
| Asset Needs | Existing OpenNT reference tree, Git index and current guest README. |
| Reporting Requirements | Record `sysmac.lib` source path/hash, distinguish it from unrelated untracked artifacts, and state that the empty divergence register remains correct. |
| Stop Conditions | A byte mismatch, unknown binary provenance, or a build-generated file masquerading as original input. |
| Exit Criteria | All guest files are exact/recorded mirrors, the sole untracked original input is governed, and no guest exception row is needed. |
| Original Owner Request | “大部分文件应当1-1一致；部分文件可以是原版文件的子集，或者是少部分修改；所有子集文件或者少部分修改的点位，需要在本文件中加入注释，并且README.md提供该例外的索引。” |
| Similar-Issue Sweep | DOS/V86 and WOW16 source roots, untracked binary/source-build inputs, build-output leakage and README exception count. |

**S3 P1 completed:** all 580 guest files hash-match OpenNT; the exact
`sysmac.lib` build input is now governed as a mirror file, with no guest
source edit or local exception. See the [S3 evidence](etc/evidence/m0-t262-s3-opennt-guest-zero-difference-closure-001.md).

**S3 P2 completed:** the owner-copied DOS/V86 package binaries, build
libraries, objects and MAP/SYM companions are each byte-identical to an
OpenNT or OpenNT-4.5 source-root peer; their 99-file provenance ledger is
[S3 binary-import evidence](etc/evidence/m0-t262-s3-opennt-guest-binary-import-001.md).

### M0 T262 S2 — bx-core local-exception and non-mirror cleanup

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T262 S2`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T262/S1 audit plus owner direction that non-mirror files “不应该存在于该组件内，我们需要逐个审核并决定它们的去向.” |
| Objective | Remove the unused project-authored the former project-specific exception-intercept header from `bx-core`, and make every retained modified Bochs file locally marked and individually indexed in `bx-core/README.md`. |
| Non-goals | No change to any Bochs executable statement, feature predicate, include dependency, build target, runtime result, or central exception decision. |
| Reference Baseline | [T262/S1 audit](etc/evidence/m0-t262-s1-original-component-mirror-audit-001.md), the pinned Bochs 2.6 tree, and the existing central external-intrusion register. |
| Files And ABI Surface | The 18 audited `src/bx-core/cpu/*` and `memory/*` files, `src/bx-core/README.md`, the obsolete header and its focused boundary fixture, T262 evidence/status. |
| Applicable Rules | `bx-core` is original Bochs with only registered minimum intrusions; every retained changed expression carries a nearby `DIVERGENCE(<ID>)`; a component README indexes each source file/exception; non-mirror files leave the component. |
| Verification | Re-run file hash comparison, prove the obsolete header has no consumer before deletion, scan all non-identical files for local marker coverage, compare README file rows with diff files, run the existing #UD boundary fixture and formal affected Ninja closure. |
| Expected Markers | Zero unmatched non-mirror `bx-core` files; 18/18 modified files have an explicit local marker and README index; 164 unchanged files retain exact source hashes. |
| Asset Needs | Pinned Bochs 2.6 source, central exception register, existing #UD boundary fixture and formal Ninja manifest. |
| Reporting Requirements | State whether each marker covers an existing central exception or a newly registered documentation-only index; report deletion of the obsolete header and every changed fixture path. |
| Stop Conditions | Any purportedly unused header has a generated/indirect consumer, or any required marker would need a new CPU semantic exception rather than documenting an existing one. |
| Exit Criteria | The obsolete non-mirror header is deleted or relocated to a proven owner; all 18 changed Bochs files have local markers and exact README rows; focused boundary verification and formal closure pass. |
| Original Owner Request | “大部分文件应当1-1一致；部分文件可以是原版文件的子集，或者是少部分修改；所有子集文件或者少部分修改的点位，需要在本文件中加入注释，并且README.md提供该例外的索引。” |
| Similar-Issue Sweep | Generated/include-only references, test-only headers, central/register drift, observation-only code, segment-limit profile branches, copied #UD state, and memory lifecycle helpers. |

**S2 P1 completed:** the zero-consumer non-mirror header is deleted; all 18
retained modified Bochs files have local markers and README rows. The focused
boundary test and a 130-edge formal Ninja relink pass. See the [S2
closure](etc/evidence/m0-t262-s2-bx-core-local-exception-closure-001.md).

### M0 T262 S1 — original-component mirror conformance audit

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T262 S1`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | Owner: “审核已知的3个镜像包： bx-core, opennt-guest, opennt-host…审核并治理所有涉及的代码文件和README并汇报。” Owner clarification: non-mirror files do not belong in an original component and require an individual destination decision. |
| Objective | Establish a complete, reproducible per-file mirror ledger and repair plan for the three original components: exact original, original subset, original with minimal edits, or non-mirror relocation/delete. |
| Non-goals | No BOP feature work, no Bochs semantic expansion, no source import merely to raise file counts, no modification to an original file before its source/diff and owner disposition are recorded. |
| Reference Baseline | Bochs 2.6 at `O:\repos.external\bochs-2.6-compat\bochs-2.6`; OpenNT at `refs/opennt`; T261/S12 exact-host-header closure; component READMEs and external-intrusion register. |
| Files And ABI Surface | `src/bx-core/**`, `src/opennt-guest/**`, `src/opennt-host/**`, their three READMEs, one audit evidence ledger, and the T262 proposal/status governance only. |
| Applicable Rules | Source-first recovery, original-component ownership, statement-local `DIVERGENCE(<ID>)`, README one-to-one exception index, and no non-mirror product file under an original-component root. |
| Verification | Hash every retained file against its recorded origin; inspect every non-identical diff hunk and marker; compare README IDs against local markers; enumerate Git-untracked production inputs; run governance and scoped diff checks. |
| Expected Markers | A ledger with every file's origin/disposition; an explicit list of all 18 current bx-core modifications and its one non-mirror header; zero-difference guest result; host subset/exception matrix. |
| Asset Needs | Existing Bochs/OpenNT source roots, component READMEs, `adapter-external-intrusion-exceptions.md`, Git status and formal source-tree inventory. |
| Reporting Requirements | Distinguish observed facts from planned remediation; every non-mirror file gets one of move-to-owner, merge-into-original, delete, or retain only after a new architecture decision. |
| Stop Conditions | An unmatched source identity, a change that would alter machine/BOP semantics, a proposed new adapter, or a non-mirror file with no safe owner pauses implementation and is recorded for owner decision. |
| Exit Criteria | All three component trees have a complete file-level ledger; every exception has an origin, local-marker/README status and a concrete remediation disposition; no audit result is inferred from directory naming alone. |
| Original Owner Request | “对于无法和原始文件完全一致的：镜像文件可以是原始文件的子集；不同的实现部分可以存在但是要加注释并标记例外。” |
| Similar-Issue Sweep | New headers, imported build inputs, untracked guest files, generated artifacts, `*_compat` paths, central versus component exception registers, include-path-only edits and source subsets. |

**S1 baseline observation:** 164/183 non-README `bx-core` files are
byte-identical to the pinned Bochs source; 18 differ and
the former project-specific exception-intercept header is non-mirror. All 580 `opennt-guest`
files match their OpenNT path byte-for-byte, including one untracked original
input. `opennt-host` has seven exact files and four declared source
subsets/minimally edited files. The detailed ledger is the required first P.

**S1 P1 completed:** the complete per-file result and remediation disposition
are recorded in the [T262 mirror audit](etc/evidence/m0-t262-s1-original-component-mirror-audit-001.md).
The audit admits no source alteration: `bx-core` marker/non-mirror repair is
S2, the guest input decision is S3, and host hunk validation is S4.

### M0 T261 S12 — closed: OpenNT-host original-header and mirror replacement

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S12`, Ordinary Mode with a single-person dual-role review. |
| Objective | Eliminate project-authored `nt_error_compat.h`, `vrputil_compat.h` and `vrnmpipe_compat.h` from `opennt-host` by recovering their original OpenNT header paths, names and applicable content. |
| Scope | `opennt-host` header/source/include users, `adapter-mvdm-host-out/softpc` mis-rooted exact header, `adapter-mvdm-host-out/win32` direct-access UI state, BOP consumers, fixture-only controls, build manifest and local README divergence register. |
| Non-goals | No new BOP capability, no full historical CSR/VDD/ICA/SoftPC shell, no Bochs change, no unbounded pointer/handle state and no generic compatibility component. |
| Required Direction | A mirror may be a reachable original-file subset when the complete translation unit cannot compose. Each retained line/record/order remains original; every omitted/unavailable/replaced expression is statement-local `DIVERGENCE(<ID>)` and has one `opennt-host/README.md` row. A new declaration belongs only to its real adapter/session/test owner, never an OpenNT-mirror path. |
| Verification | Exact hash comparison for full imported headers; source-span/exception ledger for a subset; include and old-path negative scans; original ABI assertion for `VrTerminateNamedPipes(WORD DosPdb)`; focused BOP59/VDMREDIR fixtures; formal Ninja closure, documentation governance and `git diff --check`. |
| Exit Criteria | No `*_compat.h` remains below `src/opennt-host`; all consumers include original header paths or their actual non-original owner; original header/subset and every divergence are one-to-one registered; formal closure and focused tests pass or an explicitly owned non-host failure is reproduced. |
| Admission | Owner direction: “要求你增开一个S任务完成此事。” |

**S12 P1 completed:** exact OpenNT `error.h`, `vrputil.h`, `vrnmpipe.h` and
the newly reached `oemuni.h` now replace all three project-authored host
compatibility headers.  The BOP59 fixture passes after non-original dialog
state moved to `adapter-mvdm-host-out/win32`.  A fresh `002` formal Ninja graph compiled,
archived and linked both focused fixtures; BOP59 and the Redirector fixture
each return `0`.  Full details and exact hashes are in the [S12
evidence](etc/evidence/m0-t261-s12-opennt-host-original-header-replacement-001.md).

**M0 T261 closed:** all twelve admitted source-layout packets are complete.
The resulting `opennt-host` tree has no project-authored compatibility header;
it retains only re-rooted original source, declared reachable subsets, and
README-registered statement-level divergences. The final closure record is in
[M0 T261 history](history/m0-t261-closure-20260824.md).

### M0 T261 S11 — closed: final OpenNT-host manifest, include and exception closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S11`, Ordinary Mode with a single-person dual-role review. |
| Objective | Prove that the final production `opennt-host` layout, public includes, static-library manifest and local exception register are coherent as one original-source-first build closure. |
| Scope | `src/opennt-host`, its README, current consumers/includes, component manifest/Ninja graph, all reached focused fixtures and S1/S8/S10 evidence; recover original `error.h`, `vrputil.h` and `vrnmpipe.h` rather than retaining project-authored compatibility headers. |
| Non-goals | No new host provider/BOP capability, no configuration or machine feature, no Bochs change, no import merely to increase source count, and no VDD/CSR/WOW product-shell recovery. |
| Required Direction | Each `opennt-host` production body and header is either a re-rooted original OpenNT file or carries one statement-level README exception. No `*_compat.h` may replace an existing original OpenNT header; modern BOP/fixture/session state belongs to its actual adapter, `session` or test owner. The formal manifest may compile only the admitted reached host bodies; exact retained originals remain production source but are not falsely claimed as reached. |
| Verification | One-to-one source/README divergence audit, include/old-path/manifest scan, full existing-Ninja graph execution, direct host fixtures and explicit classification of cross-owner fixture tails, documentation governance and `git diff --check`. |
| Exit Criteria | All production `opennt-host` paths meet the re-rooted-original or explicit-exception rule; original `error.h`, `vrputil.h` and `vrnmpipe.h` replace the three project-authored compatibility headers; manifest/includes/tests agree; no generic or unclassified host shim remains; the formal graph passes; direct host portions pass or a later `opennt-bop`/machine tail is explicitly separated. |
| Admission | T261 proposal S11, after S10 residual-source closure. |

**S11 P2 completed:** the formal graph executed all pending
affected compile/link work and ended with `ninja -n all` reporting no work;
the final closure compiled no `bx-core` source. Documentation governance,
component-manifest, production-tree and generic-UD boundary gates pass. The
technical-closure record distinguishes the two remaining cross-owner fixture
tails from host-layout evidence: [T261 technical closure](etc/evidence/m0-t261-s11-final-technical-closure-20260824.md).

**S11 P3 superseded by S12:** the owner rejected the interim
`nt_error_compat.h`, `vrputil_compat.h` and `vrnmpipe_compat.h` headers. The
prior technical-closure evidence is superseded as a final source-fidelity
claim; S12 imports their original OpenNT header counterparts, restores
original ABI declarations, and moves non-original BOP choice/fixture state to
its actual owner before rebuilding the affected closure.

**S11 P1 completed:** the final one-to-one audit finds three byte-identical
re-rooted PIF files; four reached original host C bodies; three include-only
same-shaped compatibility declarations; and exactly the README-registered
`HOST-DIV-012..023` edits. The refreshed formal graph compiles no `bx-core`
source. PIF and BOP59 fixtures pass; the Redirector mailslot (`exit=8`) and
configuration DASD/FDC (`exit=2`) fixture tails are explicitly classified as
separate `opennt-bop`/machine work, not host closure. See the
[S11 audit](etc/evidence/m0-t261-s11-opennt-host-final-manifest-exception-audit-001.md).

### M0 T261 S10 — closed: configuration and residual host-source-truth closure

S10 proves that no configuration or unclassified host shim remains. Its
reached `UMBNotify` fragment preserves the original no-op ABI with one
registered `/W4 /WX` adjustment; it does not represent UMB allocation or a
machine feature. See the [S10 audit](etc/evidence/m0-t261-s10-configuration-residual-source-truth-audit-001.md).

### M0 T261 S10 — historical packet

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S10`, Ordinary Mode with a single-person dual-role review. |
| Objective | Close the proposal's residual configuration and later-discovered host-source-truth check without enabling a new provider or recreating a product shell. |
| Scope | The S1/S2 host ledger, `src/opennt-host`, the reached configuration-complete `UMBNotify` fragment and its BOP-owned caller, README exception registration, formal build manifest and focused fixture. |
| Non-goals | No UMB allocator, FDC/DASD, VDD/ICA/CSR product shell, new BOP enablement, Bochs change, or generic configuration adapter. |
| Required Direction | Configuration route meaning remains in `opennt-bop`; the original empty `UMBNotify` body must remain source-identifiable and must not be presented as UMB allocation/device setup. Host-only source rows use the same four-rung disposition as S8. |
| Verification | Full current host-tree/manifest scan, source comparison with `nt_umb.c`, direct caller review, focused configuration fixture, refreshed formal Ninja target, documentation governance and `git diff --check`. |
| Exit Criteria | No generic configuration or unclassified host shim remains; every discovered row has a direct original path, statement-level registered divergence, declared adapter seam, or explicit later-owner disposition. |
| Admission | T261 proposal S10, following S9 closure. |

**S10 P1 completed:** the residual host-tree and configuration audit finds
eleven live `opennt-host` files, no generic configuration root or `*_shim`
path, and only the README-registered `HOST-DIV-012..023` source edits. The
reached `UMBNotify(UCHAR)` fragment now preserves its original signature and
empty result with the sole MSVC `/W4 /WX` `(void)code` adjustment
(`BOP-DIV-081`). The incrementally rebuilt configuration fixture reproduces
its prior `exit=2`: its later original `demDasdInit` invokes the explicitly
unimplemented FDC/INT13 owner. That cross-owner tail is not a configuration
or host-layout success claim. See the [S10 audit](etc/evidence/m0-t261-s10-configuration-residual-source-truth-audit-001.md).

### M0 T261 S9 — closed: adapter production-root semantic-residue cleanup

S9 moved every admitted source-owned INT06/DEM/DPMI/BOP interpretation to
`opennt-bop`, deleted fixture-only production providers, and left both
mechanical adapter roots selector/service/family/DOS/WOW blind. Its final
evidence is the [S9 residue closure audit](etc/evidence/m0-t261-s9-final-adapter-residue-closure-001.md).

### M0 T261 S9 — historical packet

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S9`, Ordinary Mode with a single-person dual-role review. |
| Objective | Remove selector-aware BOP/NTDOS/COMMAND observation and fixture-only residue from adapter production roots; retain all source-owned interpretation in `opennt-bop` and generic test hooks in `tests/`. |
| Scope | `adapter-mvdm-host-in` and `adapter-mvdm-host-out/softpc` production sources, their public headers, build inputs and focused fixtures. |
| Non-goals | No provider semantic change, no BOP enablement, no OpenNT source rewrite, no new adapter, no Bochs change, and no VDD/Redirector lifecycle implementation. |
| Required Direction | The mechanical adapters remain selector/service/family/DOS/WOW blind. `opennt-bop` owns every source/provider interpretation; fixture-only code belongs under `tests/`. |
| Verification | Owner/function map, negative semantic-token scan, focused fixtures, formal Ninja closure, `git diff --check`, and documentation governance. |
| Exit Criteria | No adapter production root owns selector/service/family/DOS/WOW interpretation or a fixture-only provider; all moved interfaces retain their prior fixed-width mechanical contract. |
| Admission | T261 proposal S9, after S8 final audit. |

**S9 P1 completed:** a complete source/consumer owner map is recorded in the
[S9 adapter-residue map](etc/evidence/m0-t261-s9-adapter-residue-owner-map-001.md).
It separates eight BOP-family observation pairs for path-only re-rooting,
three fixture-only observations for production deletion, the DEM/DPMI
source-interpretation splits, and the retained mechanical ABI closure.

**S9 P2 completed:** the eight selector-aware BOP/COMMAND/DEM/NTDOS
observation pairs now live under `opennt-bop/observation`; their route, CLI,
Win32-facade and fixture includes plus formal manifest ownership moved with
them without an ABI or behavior change. A fresh immutable `s9-r001` formal
Ninja graph completed its build. The next P handles only fixture-only
production residue, not the DEM/DPMI semantic split.

**S9 P3 completed:** the three observation implementations used solely by
their own isolated fixtures (`normal_return`, `normal_terminal_sequence` and
`terminal`) have been removed from production, manifest and obsolete fixture
inputs. The direct engine fixture now asserts only the machine-engine terminal
contract it actually owns; it no longer introduces BOP observation state.

**S9 P4 completed:** DEM direct-context/CCPU-SAS source interpretation and
the reached DPMI source closure now live under `opennt-bop/dem` and
`opennt-bop/dpmi`; the old `adapter-mvdm-host-out/softpc` aliases and fixture-only XMEM
record have left production. The cached formal Ninja graph refresh rebuilt the
affected closure without recompiling `bx-core`; focused DEM and DPMI fixtures
pass. The pre-existing VDMREDIR mailslot fixture exit `8` was reproduced from
both pre- and post-move graphs and remains separately recorded as non-P4
behavior evidence.

**S9 P5a completed:** the legacy `50:xx`/`54:xx` host-transaction classifier
had one fixture consumer and no formal production-graph input. It now lives
entirely under that fixture's `tests/.../support/` directory, leaving
`adapter-mvdm-host-out/softpc` without this selector/family knowledge. P5b remains to
isolate the separate generic-UD test controls in `adapter-mvdm-host-in`.

**S9 P5b completed:** generic-UD v2 context injection and the finite-run
UD-stop switch have left production. `adapter-mvdm-host-in` now has only its default
mechanical decline plus selector-blind accepted-outcome observations; the
context fixture uses an explicitly linked test-support object and the
cancellation fixture now supplies its own v1 callback. Its separate
`RUNTIME_CANCELLATION_TESTING` session-library variant is not falsely added
to the ordinary formal graph and remains a later test-governance item.

**S9 P5c completed:** the source-derived OpenNT INT 06 provider and its
`02h`/`06h` dispatcher now live in `opennt-bop`, leaving
`adapter-mvdm-host-out/softpc` with typed RAM/port mechanics only. Its focused provider and
ingress fixtures pass; the final ingress rebuild took two cached Ninja edges,
not a Bochs rebuild. The final ownership scan accepts only historical ABI
headers, source-provenance/negative-boundary comments and selector-blind
same-shaped facades beneath adapter production roots.

### M0 T261 S8 — closed: OpenNT-host original-source exception closure

S8 restored every admitted host exception through its original owner body or
an already-declared same-shaped seam, removed the drive policy and obsolete
host shims, and established statement-level README registration for each
remaining host difference.  The result and deliberate VDMREDIR/VDD lifecycle
boundary are recorded in the [S8 final closure audit](etc/evidence/m0-t261-s8-final-opennt-host-exception-closure-001.md).

### M0 T261 S8 — historical packet

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S8`, Ordinary Mode with a single-person dual-role review. |
| Objective | Replace every currently admitted non-original `opennt-host` exception by its original OpenNT owner body and the smallest same-shaped existing adapter; remove the obsolete drive-filter feature and delete the replaced shims. |
| Scope | Delete include/exclude-drive policy; use the original DEM source path in place of the project namespace; delete the misplaced DEM policy facade; migrate XMS machine mechanics to `adapter-mvdm-host-out/softpc`; import reached VDMREDIR units; import `nt_error.c`; then delete `namespace`, `xms`, `redir`, and top-level shim files as their declared replacements take over. |
| Non-goals | No new BOP family/service, no Bochs semantic change, no new specialized or generic adapter, no virtual boot drive, and no recreation of an NT4 CSR/BaseSrv/CCPU product shell. |
| Reference Baseline | Closed T261/S7 session foundation; `refs/opennt/base/mvdm/dos/dem/`, `vdmredir/`, `xms.486/`, and `softpc.new/host/src/nt_error.c`; the S1/S2 host source-truth ledger. |
| Required Direction | Original BOP meaning stays in `opennt-bop`; original host VDMREDIR/`nt_error.c` bodies belong in re-rooted `opennt-host`; unavailable historical Win32 calls use `adapter-mvdm-host-out/win32`; A20/IVT/RAM/SoftPC mechanics use `adapter-mvdm-host-out/softpc`; neutral lifecycle uses `session`; `app` remains composition owner. |
| Verification | Per-file replacement ledger; original path/name/hash or statement-level divergence proof; route/use-site and negative old-path scans; focused DEM/XMS/Redirector/top-level tests; formal Ninja closure; `git diff --check`; documentation governance. |
| Exit Criteria | No include/exclude-drive policy remains; all host drives use the OpenNT-compatible default projection; the named namespace/XMS/Redirector/top-level shims and misplaced header are gone; original providers and only declared existing-adapter seams are linked and tested; every imported edit is README-registered. |
| Admission | Owner explicitly directed this reprioritization and source replacement. The former adapter-root cleanup is deferred intact to S9. |

**S8 P1 progress:** the self-authored host namespace and all include/exclude
drive-policy production/test inputs have been removed.  The preserved original
DEM paths now directly query present host drives; the unrelated configuration-
complete fixture remains a separately recorded `exit=2` limitation.  See the
[P1 removal evidence](etc/evidence/m0-t261-s8-p1-host-namespace-drive-policy-removal-001.md).

**S8 P2 progress:** XMS source package lifecycle and `52:xx` route composition
now live in `opennt-bop`; its reached CCPU/SAS/A20/IVT/checked-RAM mechanics
are a selector-blind same-shaped `adapter-mvdm-host-out/softpc` facade.  Fresh formal Ninja
root `r003` completed 467/467 edges and both XMS focused fixtures passed; see
the [P2 owner-split evidence](etc/evidence/m0-t261-s8-p2-xms-softpc-owner-split-001.md).

**S8 P3 progress:** the reached `nt_error.c:host_direct_access_error(ULONG)`
is re-rooted under its original OpenNT host path; its three unavoidable NT4
product-shell divergences are registered in `opennt-host/README.md`.  Fresh
formal Ninja root `r004` completed 467/467 edges and the BOP 59 focused fixture
passed; see the [P3 `nt_error` evidence](etc/evidence/m0-t261-s8-p3-nt-error-original-owner-recovery-001.md).

**S8 P4 progress:** Redirector selector/session composition has left
`opennt-host`; the reached original VDMREDIR named-pipe helper surface is
re-rooted under `opennt-host/vdmredir/`.  The fresh uncontended formal Ninja
root `r008` completed 463/463 edges, and Redirector plus declared top-level,
XMS and DEM focused fixtures pass.  The historical VDD/ICA/DLC and DOS-mailslot
product dependencies remain explicitly owned by their original units rather
than being misrepresented as a direct host import; the selector/session
composition remains in `opennt-bop`.  See the [P4 VDMREDIR owner-split
evidence](etc/evidence/m0-t261-s8-p4-vdmredir-owner-split-001.md).

**S8 P5 completed:** P4 did not initially close the original VDMREDIR dispatcher or
mailslot bodies.  The source-owner intake places `vrdisp.c` and the
guest-facing portion of `vrmslot.c` in `opennt-bop/vdmredir/`, with only their
mechanical and modern-API seams below the declared adapters.  Its recovery
order and explicit limitations are recorded in the [P5 VDMREDIR
intake](etc/evidence/m0-t261-s8-p5-vdmredir-source-owner-intake-001.md).
The original `vrdisp.c` table is now the actual bounded `57:xx` route source;
its admitted local provider bodies were subsequently recovered.  The first
direct `vrmslot.c` body, `VrPeekMailslot`, now replaces the former cached-peek
rehost with OpenNT's explicit `ERROR_NOT_SUPPORTED` result; formal `r010`
compiled it into `opennt-bop` and the Redirector fixture passed.  The original
`VR_MAILSLOT_INFO` record/list, token lookup and process-cleanup lifecycle now
replace the former fixed array.  `BOP-DIV-058` retains the one session-owned
opaque handle map rather than duplicating OpenNT's private bitmap; the focused
r011 lifecycle fixture passes.  See the [P5 lifecycle
evidence](etc/evidence/m0-t261-s8-p5-vrmslot-record-lifecycle-recovery-001.md).

**S8 P6 completed:** P5 closes the admitted local `vrmslot.c` body group, but the
live `opennt-host/vdmredir/vrnmpipe.c` and `vrinit.c` files remain reached
surfaces rather than complete original translation units. P6 records each
function's exact direct-recovery or later-package disposition. Its first two
direct bodies, `VrIsNamedPipeName` and `VrConvertLocalNtPipeName`, now retain
original flow and pass their focused formal fixture checks; only the necessary
x64-safe spellings are `HOST-DIV-020` registered. Its original named-pipe
metadata record/list and `VrAdd/RemoveOpenNamedPipeInfo` bodies are now active
too. `VrReadNamedPipe` and `VrWriteNamedPipe` now retain their original
synchronous `OVERLAPPED_PIPE_IO` issue/wait/result/error ordering; only their
VDD raw-pointer cancellation registration and timed-out native-handle
retirement remain `HOST-DIV-015/016` lifecycle divergences. The historical
empty `VrTerminateNamedPipes(DosPdb)` is not misrepresented as the current
session all-record cleanup. VDD/ICA/NetBIOS/DLC and async cancellation
lifecycle are not silently folded into this helper work. See the [P6 host-body
disposition](etc/evidence/m0-t261-s8-p6-vdmredir-host-body-disposition-001.md).
The retained `VrTerminateMailslots` body now also closes the PDB-owned record
cleanup with the original no-AX-write result; its copied-frame/token divergence
is `BOP-DIV-059`.  `VrGetMailslotInfo` now uses its original body, backed by
the re-rooted original `opennt-host/vdmredir/vrputil.c`; fresh r012 completed
314/314 actions and the Redirector fixture passes. `VrDeleteMailslot` is now also direct, retaining original PDB,
ES:DI/DX and no-AX-write behavior; `BOP-DIV-061` replaces only its obsolete
private handle-bitmap release. `VrMakeMailslot` now also retains its original
allocation, creation and record-link order; `BOP-DIV-062` copies its ASCIZ
input through the existing bounded CCPU/SAS facade and `BOP-DIV-063` replaces
only the obsolete private handle bitmap with the shared session mapper.
Incremental formal r012 rebuilt and passed. `VrReadMailslot` now retains its
original `SetMailslotInfo` → `ReadFile` → `GetMailslotInfo` ordering through a
record-sized output lease. `VrWriteMailslot` now retains its original
validation → local-name formation → `CreateFile` → `SetMailslotInfo` →
`WriteFile` → close ordering; its composition predeclares only the exact
eight-byte descriptor and independently addressed source span. The former
hand-written write provider is deleted, and r012 compiles, links and executes
the Redirector fixture successfully. See the [P5 lifecycle
evidence](etc/evidence/m0-t261-s8-p5-vrmslot-record-lifecycle-recovery-001.md).

### M0 T261 S7 — closed: dependency-free session owner split

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S7`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | S6 closed the component-count, ownership and terminology governance. The owner directed that S7, rather than S6, implements the `session` owner split. |
| Objective | Establish `src/session` as the dependency-free per-VDM lifecycle foundation and move only neutral lifecycle, resource/token, capability-registration, completion/event and teardown ownership into it. |
| Non-goals | No BOP selector/provider implementation, no OpenNT host-source rewrite, no Win32 facade semantics, no SoftPC/CCPU or Bochs mechanics change, no guest change, and no preemptive `adapter-mvdm-host-out/vdd`, `adapter-mvdm-host-out/debugger`, `adapter-mvdm-host-out/redir`, `adapter-mvdm-host-out/wow`, generic `adapter-common`, `adapter-host`, or `compat` root. |
| Reference Baseline | T261/S5 closure `ab49a2d4`; current `app`, `adapter-mvdm-host-in`, `adapter-mvdm-host-out/softpc`, `adapter-mvdm-host-out/win32`, `opennt-bop`, and `opennt-host` session-facing sources; T260 mapping-manager boundary. |
| Files And ABI Surface | New `src/session/*`, precisely inventoried neutral state currently held by component composition/adapters, component public declarations, formal module manifest/generator, and focused session fixtures. |
| Applicable Rules | `session` has no product-component dependency and no BOP/DOS/WOW/VDD/Redirector/Win32/Bochs vocabulary; `app` creates and wires the instance; source-owned behavior stays in its original owner; cross-component data remains fixed-width or opaque. |
| Stop Conditions | A candidate extraction requires service/provider/guest interpretation, an OpenNT-body semantic edit, a raw Bochs object or Win32 handle in the public session ABI, an upward import of `app`, or a second mapping-manager implementation. Pause and record the owner conflict. |
| Verification | [S7 session-foundation evidence](etc/evidence/m0-t261-s7-session-foundation-001.md); former/new owner map, source-token scans, focused identity/teardown fixture, formal MSVC x64 Ninja native link, `git diff --check`, documentation inventory/governance verification. |
| Exit Criteria | Closed: `session` builds as a dependency-free ninth host-side module; `app` alone creates and wires the instance; cancellation has one neutral owner; the three adapter-mvdm-host-out/softpc mapping implementations bind to a unique session identity and teardown through its lifecycle; existing source/machine semantics are unchanged; no specialized/generic adapter was introduced. |
| Original Owner Request | “非常好，请你治理好文档，以S6身份提交，我们让S7来实施session逻辑抽离。” |
| Similar-Issue Sweep | Session records, token/handle/event tables, mapping-manager instance lifecycle, app-owned composition callbacks, adapter callbacks, thread/child completion state, build manifest and fixtures. |

### M0 T261 S6 — closed: architecture contract-consistency governance

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S6`, Ordinary Mode with a single-person dual-role review. |
| Objective | Reconcile the newly admitted `session` component across current architecture, goal, coding and rule authorities without moving production code or claiming that `session` is already built. |
| Scope | Correct component cardinality and adapter terminology; distinguish ten source components from the current eight linkable modules plus the guest-image input; define `session` as lifecycle/identity owner and `adapter-mvdm-host-out/softpc` as mapping-facade implementation owner; correct production/test-root terminology; and record remaining selector-aware observation/fixture residue as later cleanup rather than silently legitimizing it. |
| Non-goals | No `src/session` implementation, source move, BOP/provider change, Bochs change, adapter semantic migration, Ninja module addition, or test behavior change. |
| Verification | [S6 governance evidence](etc/evidence/m0-t261-s6-architecture-contract-consistency-001.md); authority cross-read, targeted terminology/dependency scan, documentation inventory regeneration, documentation-governance verification and `git diff --check`. |
| Exit Criteria | Architecture, goal, coding and rules agree on the ten-component target; S7 is the sole active implementation packet for `session`; no current build claim is inflated; and the follow-on cleanup has an explicit S owner. |
| Follow-on | S7 implements only dependency-free `session`; S8 performs the owner-directed OpenNT-host source replacement; the former adapter-root cleanup is deferred to S9. |

### M0 T261 S5 — closed: adapter-mvdm-host-in generic ingress ownership split

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S5`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | The owner explicitly authorized S5: “好的 接下来S5开工？”.  T261’s approved proposal admits this exact split after S4 closure. |
| Objective | Establish `adapter-mvdm-host-in` as the sole selector-blind fixed-width generic-UD ingress/completion owner; retain every BOP selector/service recognition, provider dispatch and BOP-aware observation in `opennt-bop`. |
| Non-goals | No new BOP enablement, no provider semantic change, no new CPU-frame ABI, no Bochs change, and no migration of family/session/provider logic into `adapter-mvdm-host-in`. |
| Reference Baseline | T261/S4 P7 closure `694cf087`; existing the former generic-UD bridge header; `opennt-bop/ingress/dem_generic_ud_bridge.c`; T260 formal component manifest and architecture/coding authorities. |
| Files And ABI Surface | New `src/adapter-mvdm-host-in/*`; the generic-UD external bridge symbol; exported `opennt-bop` route callback; app session-composition bind/reset calls; manifest/generator module and link order; focused bridge fixture. |
| Applicable Rules | `adapter-mvdm-host-in` is selector/service/family/DOS/WOW/provider blind; OpenNT route semantics remain in `opennt-bop`; `app` owns wiring; `adapter-mvdm-host-out/softpc` remains the only SoftPC/CCPU-to-Bochs boundary; fixed-width copied data only. |
| Verification | Function-level owner map; selector-token negative scan of `adapter-mvdm-host-in`; focused bind/decline/dispatch/reset fixture; formal Ninja closure; component-direction scan; `git diff --check`; documentation governance. |
| Expected Markers | No selector/service literal or provider include in `adapter-mvdm-host-in`; no exported CPU generic-UD bridge implementation in `opennt-bop`; an unbound bridge declines, a bound route receives the unchanged copied event/outcome, and reset removes the binding. |
| Asset Needs | Existing fixed-width generic-UD ABI, current route entry, app engine lifecycle, formal Ninja manifest/generator and focused C fixtures. |
| Reporting Requirements | Exact function ownership map; no-new-ABI proof; retained route/observation source; binding lifetime; source/list/link changes; focused and formal results. |
| Stop Conditions | A proposed adapter file reads a selector/service/window payload for meaning, names a provider/family/DOS/WOW concept, requires a raw guest/Bochs object pointer, or forces a provider semantic change. Pause and record the boundary conflict. |
| Exit Criteria | `adapter-mvdm-host-in` owns the externally called generic ingress symbol and only generic registration/validation/forwarding; `opennt-bop` owns one exported route callback and all selector-aware logic; `app` binds/unbinds around one engine session; formal graph names eight linkable libraries and passes stated focused checks. |
| Original Owner Request | “好的 接下来S5开工？” |
| Similar-Issue Sweep | Generic-UD v1/v2 symbol definitions, handler registration lifetime, BOP observations, fixtures with bridge stubs, manifest module/source/link lists, old `bx-vdm`/ingress path references, and component READMEs. |

### M0 T261 S4 — closed: DEM host-support original-layout recovery

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S4`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | T261/S3 is closed by the formal layout evidence; the owner-approved T261 proposal admits its next DEM owner-recovery sequence. |
| Objective | Recover or relocate DEM host support by original source owner while preserving host-file API shape, error order, opaque-handle mapping and guest-memory lease contracts. |
| Non-goals | No new DEM BOP enablement, redirector protocol completion, DASD/FDC implementation, new mutation policy, guest/Bochs change, or source-provider redesign. |
| Reference Baseline | T261/S1 source truth, T261/S2 binding ledger rows for `dem/*`, T230 direct-source mirrors, T232 shared handle manager and T261/S3 formal root. |
| Files And ABI Surface | `opennt-host/dem/*`, direct OpenNT `dem*.c` callers in `opennt-bop/dem`, their host file/directory/error/handle declaration seams, and their adapter-mvdm-host-out/win32/adapter-mvdm-host-out/softpc includes. |
| Applicable Rules | Original source and call shape first; every imported-body edit has `DIVERGENCE`; adapter-mvdm-host-out/win32 owns historical public API facades; adapter-mvdm-host-out/softpc owns mapping/mechanics; no component may depend upward on BOP routing. |
| Verification | Per-group original-source/ABI/failure ledger, component-direction scan, focused DEM fixtures, fresh formal Ninja build, `git diff --check` and documentation governance. |
| Expected Markers | Each surviving host file has a direct original path or named source-derived exception; no generic DEM shim remains merely because it combines BOP state with host capability; original source callers preserve failure order. |
| Asset Needs | Existing OpenNT DEM mirror/reference sources, adapter interfaces, shared mapping manager, focused fixtures and formal Ninja generator. |
| Reporting Requirements | Exact caller/function map, retained original interface/layout/order, extracted adapter boundary, source divergence/exception, focused result and any deferred machine/Redirector owner. |
| Stop Conditions | A split requires BOP service interpretation in `opennt-host`, a host component call upward into `opennt-bop`, a second mapping/handle manager, or a semantic rewrite of an imported body. Pause that group and record the dependency. |
| Exit Criteria | Every DEM host-support file has moved to an original-owner path, a declared adapter, BOP composition, or an explicit source-derived host exception; no reverse dependency is introduced; all repaired consumers build and declared focused tests pass or retain an owner-specific limitation. |
| Original Owner Request | “实施opennt-host组件的原生化”。 |
| Similar-Issue Sweep | DEM file/dir/gset/handle/error/search/DASD seams, VDD/debug/FDC exceptions, Redirector cross-calls, app and fixture include paths, static manifest and component README rows. |

**Current progress:** S1 closed in `25f817ad`; S2 P1 closed in `6c0313a8`
with a 47/47 recovery ledger. S3 P1 has re-rooted the byte-identical PIF
triplet and compiled the parser object through its new exact include path; see
[PIF evidence](etc/evidence/m0-t261-s3-p1-pif-original-re-root-001.md). The
full-graph run root was interrupted externally and is explicitly non-evidence;
S3 P2 has moved the parser layout bridge to `opennt-bop` and its OEM Win32
facade to `adapter-mvdm-host-out/win32`; both moved objects compile in fresh `r002`. S3 will
use that fresh root for later formal closure. No provider behavior has changed.
S3 P3 has additionally moved the COMMAND native session state machine from
`opennt-host` to `opennt-bop/ingress`, compiled its exact object in fresh
`r003`, and passed the component-manifest and production-tree gates; see
[session-ingress evidence](etc/evidence/m0-t261-s3-p3-command-session-ingress-rehome-001.md).
S3 P4 has moved the remaining project-owned COMMAND composition sources to
`opennt-bop/command`; fresh `r004` builds all five moved objects and the full
`opennt-bop` static library.  The remaining `opennt-host` COMMAND input is
only the unchanged PIF triplet; see [composition-owner evidence](etc/evidence/m0-t261-s3-p4-command-composition-owner-rehome-001.md).
S3 P5 has extracted the reached RTL/NTDLL compatibility surface from that
composition to `adapter-mvdm-host-out/win32` without a reverse BOP dependency.  Fresh
`r005` builds both affected libraries and passes the direct check-binary and
dynamic-environment fixtures; see [RTL-facade evidence](etc/evidence/m0-t261-s3-p5-command-rtl-facade-extraction-001.md).
S3 P6 has moved DEM session and dispatcher composition to `opennt-bop`; fresh
`r006` compiles its production and affected fixture objects while retaining
host drive/namespace capability in `opennt-host`; see [DEM composition evidence](etc/evidence/m0-t261-s3-p6-dem-composition-owner-rehome-001.md).
S3 P7 has similarly moved the XMS native `#UD` session to `opennt-bop/ingress`;
fresh `r007` compiles it and passes component gates; see [XMS session evidence](etc/evidence/m0-t261-s3-p7-xms-session-owner-rehome-001.md).
S3 P8 has moved the source-parity configuration-complete `UMBNotify` unit to
`opennt-bop/softpc`; r008 compiles it with component gates passing; see [configuration-complete evidence](etc/evidence/m0-t261-s3-p8-config-complete-owner-rehome-001.md).
S3 P9 has now completed fresh formal root `r009` (458 Ninja edges), confirmed
the PIF/COMMAND-focused fixtures and recorded the two unrelated DASD/Redirector
fixture limitations without treating either as passing evidence; see the
[formal layout closure](etc/evidence/m0-t261-s3-p9-formal-layout-closure-001.md).
S4 P1 has completed the function-level DEM host boundary map: it separates
source-visible host APIs, BOP composition, adapter-mvdm-host-out/softpc mechanics and
unavailable VDD/FDC paths, and fixes the dependency order before any source
move; see the [DEM boundary map](etc/evidence/m0-t261-s4-p1-dem-host-boundary-map-001.md).
S4 P2 has split the independent DEM directory group: its source-body invoke
now belongs to `opennt-bop`, same-named OEM directory calls belong to
`adapter-mvdm-host-out/win32`, and fresh r010 formal build plus the direct-import fixture
pass; see the [DEM directory split](etc/evidence/m0-t261-s4-p2-demdir-owner-split-001.md).
S4 P3 has split the DEM file/label composition from same-shaped OEM Win32
facades while retaining Redirector as its separately owned lower host
capability; fresh r011 completes 462 Ninja edges and both direct file/OEM
fixtures pass; see the [DEM file split](etc/evidence/m0-t261-s4-p3-demfile-owner-split-001.md).
S4 P4 has split DEM global-setting service composition/DTA state from the
same-shaped public-Win32 host queries while retaining host drive admission;
fresh r012 completes 460 Ninja edges and the direct `demgset` fixture passes;
see the [DEM global-setting split](etc/evidence/m0-t261-s4-p4-demgset-owner-split-001.md).
S4 P5 has split DEM search/FCB BOP composition and historical find/NT facade;
the removed invasive `NtVdmControl` remains explicit `STATUS_NOT_IMPLEMENTED`
for the imported fallback. Fresh r013 completes 461 Ninja edges and the
direct search/FCB fixture passes; see the [DEM search split](etc/evidence/m0-t261-s4-p5-demsrch-fcb-owner-split-001.md).
S4 P6 has moved the common DEM CCPU/SAS frame, mapping and handle facade to
`adapter-mvdm-host-out/softpc`; a generic post-body hook removes its former direct demerror
link. Fresh r014 completes 461 Ninja edges and focused handle/error fixtures
pass; see the [CCPU/SAS migration](etc/evidence/m0-t261-s4-p6-dem-ccpu-sas-facade-migration-001.md).
S4 P7 has completed the final owner cleanup: the remaining DEM
compatibility/composition surfaces now reside under `opennt-bop/dem`, while
`opennt-host/dem` retains only named drive/namespace capabilities and its
drive-policy facade.  Fresh r016 completes 462 Ninja edges; the DASD/IOCTL,
error/lock and MISC direct-import fixtures pass.  The old r015 concurrent
build root is explicitly excluded from evidence.  See the
[S4 owner closure](etc/evidence/m0-t261-s4-p7-dem-host-owner-closure-001.md).
S5 is now admitted as the next bounded owner split.  It must not interpret a
selector in `adapter-mvdm-host-in` or change the already-closed DEM host boundary.
S5 P1 is closed in `ab49a2d4`: `adapter-mvdm-host-in` owns the generic CPU entry and
the unchanged copied record; `opennt-bop` retains the renamed selector route;
and `app` owns bind/reset around one engine run.  The actual formal r002 native
link and focused ingress fixture pass; see
[S5 ingress evidence](etc/evidence/m0-t261-s5-adapter-mvdm-host-in-ingress-split-001.md).

## Previous Active Packet — M0 T260

T260 is closed. Its retained detail below is historical closure evidence and
does not authorize follow-on implementation.

### M0 T260 S8 P2 — closed: production-tree purification, tests and final closure

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T260 S8`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | S7 is closed. Owner requires one complete migration of tests, formal Ninja inputs, generated manifests and import/governance scripts to the eight component roots before any build result is claimed. |
| Objective | Make every eight-component `src/` root production-only: externalize reference/example/fixture/legacy inputs, place live OpenNT code by its original owner-family composition, individually annotate/register every production divergence, then retain the formal Ninja graph and final link closure. |
| Non-goals | No BOP behavior implementation or enablement; no new Bochs semantic intrusion; no source-provider redesign. |
| Reference Baseline | S1 manifest, source policy, current component READMEs, the completed r014 formal graph, and the now-superseded original/mirror/overlay staging tree. |
| Files And ABI Surface | All eight component roots: seven linkable host-side components plus the `opennt-guest` artifact input; their component manifest, formal Ninja graph, test ownership and original OpenNT provenance/include edges. |
| Applicable Rules | Component roots are production-only; references/examples/fixtures belong outside `src`; every nonzero imported-source edit is source-marked `DIVERGENCE:` and individually registered; BOP owns route/meaning but never Bochs-object mechanics. |
| Verification | Git-rename/content identity scan; route/mirror/original owner scan; retained-shim negative scan; `git diff --check`; documentation governance verification. |
| Expected Markers | No legacy source root remains; no `original/`, `mirror/`, `overlay/` or generic capability staging tree remains as a product-code category; source-shaped host/SoftPC/Win32 seams name their owner directly. |
| Asset Needs | OpenNT source roots, current BOP mirror/route tree, owner manifest and formal module manifest. |
| Reporting Requirements | Record exact moved roots/counts, no-source-edit proof, retained later-owner shim/observation inventory and remaining S8 build switch. |
| Stop Conditions | A file combines BOP meaning with host capability/Win32/SoftPC mechanics such that it cannot be split without behavior change, or an original source identity cannot be proved. Pause for a targeted split decision. |
| Exit Criteria | Formal graph, tests and tools use only target component paths; all eight static library boundaries and final-link ordering are explicit; no component root contains non-production reference/example/fixture/overlay code; every retained divergence has source and README evidence; no legacy `src/opennt`, `src/bx-vdm` or `src/cli` root remains; documentation, static gates and an actual formal Ninja build verdict are recorded. |
| Original Owner Request | “应使用 1 个 T 任务，例如：M0 T260：七/八组件源码布局重整与构建闭合……这个 T 应拆成约 8 个 S。” |
| Similar-Issue Sweep | Headers, fixtures, generated Ninja input, include roots, legacy v1/test-only code, OpenNT source provenance and the shared mapping-manager placement. |

### Planned T260 S sequence

| S | Bounded outcome |
| --- | --- |
| S1 | Complete owner/provenance/build manifest. |
| S2 | Target roots, READMEs, exception registers and public library/include boundaries. |
| S3 | `bx-core` confirmation plus `bx-machine` purification: retain Bochs-only code and assign every foreign-semantics file to its target owner. |
| S4 | `app` and `opennt-guest` migration. |
| S5 | `opennt-bop` mirror, ingress and route migration. |
| S6 | `opennt-host` and `adapter-mvdm-host-out/win32` classification/split migration. |
| S7 | `adapter-mvdm-host-out/softpc` same-shaped SoftPC/CCPU boundary and shared-mapping-manager migration. |
| S8 | Tests, formal Ninja graph, legacy-path exit and whole-package acceptance. |

### T260 S5 completion record

S5 moved OpenNT BOP identity, minimal-change mirrors and BOP routes into
`src/opennt-bop/{original,mirror,route,overlay}` without a source-body edit.
The content-preserving move and inherited-overlay register are recorded in the
[S5 layout evidence](etc/evidence/m0-t260-s5-opennt-bop-layout-migration-001.md).
The formal Ninja source declarations are intentionally still historical until
S8; S6/S7 next classify and migrate the remaining host/API and SoftPC/mechanical
seams.

### T260 S6 completion record

S6 separated the pre-existing OpenNT host-capability files and same-shaped
Win32/NTDLL facade from the former generic shim tree. All affected DEM,
COMMAND and XMS mirrors now name their host component dependency directly.
The S6 evidence records the deliberately deferred formal Ninja-graph update
and the exclusive S7 ownership of the remaining mechanical seam files; see
[S6 layout evidence](etc/evidence/m0-t260-s6-host-win32-layout-migration-001.md).

### T260 S7 completion record

S7 established `adapter-mvdm-host-out/softpc` as the unique same-shaped mechanical facade,
including the one mapping-manager implementation with its three session
instances. `src/bx-vdm` contains no live product file after the move. The
remaining old-path references are exclusively S8 test/tool/build-graph work;
see [S7 layout evidence](etc/evidence/m0-t260-s7-softpc-adapter-layout-migration-001.md).

**S8 P2 amendment:** the prior staging labels `original/`, `mirror/`,
`overlay/` and generic `capability/` are no longer acceptable inside a
component root when they contain reference-only or non-linkable code.  S8 now
purifies those roots, relocates reference/example/test-only material to
`docs/etc/legacy_code/` or `tests/`, and records every retained production
divergence at statement level.  The r014 formal Ninja link is a pre-purification
baseline, not S8 closure evidence.

**S8 P2 verification:** the reclassified graph passed its manifest gate at
291 sources, 78 fixtures and one native target. Formal Ninja run `r018`
linked all seven static libraries, every declared fixture and
`bin/ntvdm64.exe`; final actual incremental and dry-run invocations
reported no work. The production-tree gate also passed: no non-production
staging root or legacy build input remains, and all 46 source divergences are
individually README-registered. See the [T260 closure](history/m0-t260-closure-20260823.md).

**Current progress:** S1 is closed. The current reproducible manifest covers
all 3,542 `src/` files in 3,735 rows with zero missing paths, duplicate paths
or unresolved owners; 2,332 non-product OpenNT inputs are explicitly destined
for `refs/opennt`. See the
[baseline record](etc/operations/m0-t260-s1-owner-manifest-baseline-001.md).

S2 P1 has created all eight target-root README contracts. They declare the
exception registers for `bx-core`, `opennt-guest`, `opennt-bop` and
`opennt-host`; restrict machine to Bochs-only mechanics; and declare each future
library/public-include boundary. No product source or formal Ninja input has
moved.

S3 has moved all 42 former project-prefixed bridge files to
`adapter-mvdm-host-out/softpc` with Git history, repaired product include ownership, and
made the remaining machine source scan free of external product semantics.
The local bx-core exception index now points to every active exception family.
The one coherent formal Ninja/fixture path switch remains S8 work; see the
[S3 evidence](etc/evidence/m0-t260-s3-machine-purification-001.md).

S4 has moved the 24 former `src/cli` implementation files into `app` and 579
original DOS V86/WOW16 source files into `opennt-guest`, all as 100% Git
renames.  The live owner map now records 580 guest candidates with no
DEM/COMMAND entry and separately classifies 38 DEM/COMMAND source inputs for
future `opennt-bop` migration.  Formal Ninja paths remain intentionally
unchanged until S8; see the
[S4 evidence](etc/evidence/m0-t260-s4-app-guest-migration-001.md).

## Previous Active Packet — M0 T259

### M0 T259 S4 — Host/machine source-shaped facade reconciliation

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T259 S4`, Ordinary Mode with a single-person dual-role review: source-first facade reconciliation. |
| Admission And Approval | T259 S1 catalog, S2 RTL/environment seams and S3 machine-contract closure are established. S4 reconciles the remaining reached host/machine facades and their formal static-link closure. |
| Objective | Recover each reached original OpenNT/SoftPC interface shape and its observable semantics through the smallest source-shaped compatibility facade or direct public API, while independently minimizing edits to OpenNT mirrors and `bx-core`/`bx-machine`. |
| Non-goals | No BOP/provider enablement; no wholesale CCPU/SAS/CSR/BaseSrv product-shell recreation; no Bochs semantic intrusion; no invented behavior without source evidence. A facade is not permission to substitute a convenient new ABI. |
| Reference Baseline | `MACH-01..09` in the T259 catalog; OpenNT `softpc.new/host/src/{nt_cpu.c,nt_cprgs.c,nt_aorc.c}`, `keybd_io.c`; active SoftPC/DEM/XMS source mirrors; T258 mapping manager. |
| Files And ABI Surface | The source-shaped SoftPC/CCPU facade contract in `bx-vdm`; affected `host_simulate`, register-frame, checked-memory, A20, IVT/keyboard and typed machine-action seams. |
| Applicable Rules | Source-first recovery, public-Win32/x64 decision rule, guest/machine hard boundary, one shared mapping manager, and no family-private product shell. |
| Verification | Original call-site and fixed-width ABI audit; focused seam fixture; negative selector/DOS/CCPU boundary scan of bx-core/bx-machine; formal Ninja build/link; documentation gate and independent review. |
| Expected Markers | Reached historical spelling remains at the mirror boundary; `bx-vdm` performs copied-state translation; bx-core/machine remain selector/DOS/CCPU blind; no CCPU loop or host pointer crosses the boundary. |
| Asset Needs | OpenNT SoftPC/CCPU source and active mirrors, current bx-vdm/machine API, T258 mapping manager, catalog, exception register and formal build manifest. |
| Reporting Requirements | Record original interface spelling and call site, calling convention/width/lifetime, typed mechanical request/result, no-op/failure disposition and any exception ID. |
| Stop Conditions | A helper exposes host pointers to the guest, changes imported OpenNT control flow, recreates a CCPU/product shell, imports BOP/DOS/SoftPC terms into bx-core/machine, or lacks a source/ABI evidence entry. |
| Exit Criteria | Every remaining reached `HOST-*`/`MACH-*` seam has a source-shaped facade, direct public-API composition, or explicit source-proven decline; the formal static-library closure is sound for each affected fixture; all residual product-shell semantics are queued rather than silently recreated. |
| Original Owner Request | “接下来，是关于我们全面恢复opennt的win32api宿主方面和 softpc/ccpu 核心机方面的接口规范的任务。你已经审计过。” |
| Similar-Issue Sweep | `ANSI_STRING`, `OEM_STRING`, `UNICODE_STRING`, `Rtl*` conversion/heap/environment helpers, `NTSTATUS` to DOS error mapping and existing COMMAND/DEM declaration copies. |

**Current progress:** S1 closed the canonical
[`HOST-*`/`MACH-*` interface catalog](etc/evidence/m0-t259-s1-host-machine-interface-catalog-001.md)
and [active-facade reconciliation](etc/evidence/m0-t259-s1-current-facade-reconciliation-001.md).
S2 P1 has centralized the active COMMAND `HOST-07` declaration surface and
recovered source-visible NTSTATUS-to-DOS error differentiation through the
current same-named `ntdll` export; its formal 286-node link and source-mirror
fixture pass are recorded in [P1 evidence](etc/evidence/m0-t259-s2-p1-rtl-contract-status-recovery-001.md).
P2 has restored caller-owned ANSI output semantics, and P3 proves the imported
`cmdenv.c` dynamic session-environment transform in a fresh formal graph. S2's
COMMAND slice is closed; any additional DEM consumer migration is tracked as
S4 reconciliation work rather than a reason to fork another RTL contract.
S3 now records the SoftPC/CCPU mechanical contract before correcting its first
reached facade. This remains neither a new BOP survey nor an NT4 product-shell
recreation.

S3 P1 has recovered the call-local CCPU register/flag accessor semantics used
by directly imported DEM code: writes to CF/ZF now update the copied image as
well as the existing typed result, and CS:IP staging remains explicitly local
until a future source-proven continuation contract consumes it. A fresh
286-object formal graph and the direct DASD/IOCTL fixture pass; see
[S3 P1 evidence](etc/evidence/m0-t259-s3-p1-ccpu-copied-state-facade-001.md).
This is deliberately not a claim that the recursive `host_simulate` lifecycle
is already restored.

S3 P2 closes the reached machine-facade disposition audit.  It proves that
original CCPU `host_cpu_reset`/`host_cpu_interrupt` are no-ops, that the
recursive `host_simulate` branch is a truthful unavailable result rather than
a false CPU success, and that finite SAS/IVT/A20 paths retain source-shaped
checked facades.  It also records the combined recovery rule: retaining the
original interface contract and minimizing intrusion are simultaneous gates.
See [S3 P2 evidence](etc/evidence/m0-t259-s3-p2-machine-facade-disposition-closure-001.md).

S4 P1 has replaced the false DEM hard-error NTDLL stubs with same-named,
host-only `ntdll.dll` facades for Unicode append/compare and DOS-device
symbolic-link open/query. The original `demerror.c` ordering remains intact;
a fresh 286-object formal graph and source-mirror fixture pass. See
[S4 P1 evidence](etc/evidence/m0-t259-s4-p1-demerror-ntdll-facade-001.md).

S4 P2 closes formal static-library reconciliation: a fresh 453-node Ninja
graph links all 78 fixtures plus the native target using exact per-fixture
module closures, and focused regressions pass.  The canonical RTL facade now
keeps its counted-string layout checks in both C11 and C++14; no OpenNT mirror
or Bochs source changed. See [S4 P2 evidence](etc/evidence/m0-t259-s4-p2-formal-static-closure-001.md).

S4 P3 restores reached `RcErrorDialogBox`/`RcMessageBox` host facades through
public Win32 and makes `demmisc` reuse the existing OpenNT-shaped
OEM-to-Unicode file facade. The fresh 454-node formal graph, three focused
fixtures and all 79 outputs pass; custom editable PIF UI, VDD/debugger,
printer and floppy product semantics remain explicitly outside this facade.
See [S4 P3 evidence](etc/evidence/m0-t259-s4-p3-host-dialog-oem-facade-closure-001.md).

S4 P4 completes the final reached-seam reconciliation.  It restores the
directly composable `FdiskTerminatePDB` half of the original
`HostTerminatePDB` order, makes the unavailable VDD/floppy halves explicit,
and records direct parity for the original empty `UMBNotify` body.  The fresh
454-node r16 formal graph is clean; focused DEM search/FCB and DASD fixtures
pass.  S4 is complete; see [S4 P4 evidence](etc/evidence/m0-t259-s4-p4-final-host-machine-reconciliation-001.md).

S4 P5 closes a reverse-audit correction in the directly imported OpenNT PIF
parser: its `RcMessageBox` wrapper now uses the one shared source-shaped
facade, and the common historical `RMB_ABORT` option/result value is restored
from the erroneous `2` to OpenNT's `1`.  A full fresh r17 graph produces all
79 declared outputs, and PIF parser, COMMAND misc and DEM hard-error fixtures
pass; see [S4 P5 evidence](etc/evidence/m0-t259-s4-p5-command-pif-dialog-facade-closure-001.md).

**T259 technical closure state:** all admitted S1--S4 exit criteria are met,
and the [formal T259 closure](history/m0-t259-closure-20260823.md) is
recorded. T260 succeeds it; this retained record does not reopen T259's scope.

## Latest Closure

M0 T260 closes the eight-component production source layout.  All component
roots are production-only, legacy/reference code is externalized under
`docs/etc/legacy_code`, and the formal Ninja closure plus source/README
divergence register gate pass. See [T260 closure](history/m0-t260-closure-20260823.md).

M0 T259 closes the source-first host/machine compatibility-facade baseline.
Every reached `HOST-*`/`MACH-*` seam now has direct original composition, a
smallest OpenNT-shaped public-API facade, or an explicit source-proven
decline; imported OpenNT mirrors and bx-core/machine remain unmodified. See
[T259 closure](history/m0-t259-closure-20260823.md).

M0 T258 closes the shared 16/32 guest-to-32/64 host mapping-manager
prerequisite. One implementation supplies the session's separate
`guest_memory`, `host_handle`, and `session_data` instances; the outer engine
owns their reset. Its 16:16/32-linear lease, opaque host-handle/data IDs,
private-instance rejection and affected DEM/COMMAND regressions have focused
proof. See [T258 closure](history/m0-t258-closure-20260823.md).

M0 T257 closes as a source-first design rejection. Td S2 P7 proves that a
new generic protected-frame ABI would bypass unresolved DPMI pointer,
frame/IRET, LDT and memory-lifecycle owner dependencies. Its retained
source-import and failure-map evidence transfers to a future whole DPMI
package; it enables no new `53:xx` route. M0 T258 now owns only the
cross-family synchronous mapping prerequisite.

M0 Td S2 P1 closes the governing source-recovery decision: an original OpenNT
translation unit now requires direct composition or a recorded OpenNT-shaped
facade before a project-defined replacement is permitted. It also fixes the
single bounded guest-pointer mapping-lease contract. S5 is frozen pending the
separate Td S2 P2 whole-tracker interface-conformance audit; see
[guest-pointer governance](etc/operations/td-guest-pointer-mapping-governance-001.md).

M0 Td S2 P3 closes the shared-interface audit baseline: the tracker now has
321 unique rows (203 BOP plus 118 dependencies), including the previously
untracked historical guest-pointer interface family.  It classifies the
historical dispatcher, bounded keyboard facade, host-token exception, generic
DPMI mechanics and pointer migration debt without enabling a new route; see
the [P3 audit](etc/evidence/td-s2-p3-shared-interface-conformance-audit-001.md).
T257 S5 remains frozen until P4--P7 complete the whole-tracker audit.

M0 Td S2 P4 closes the DEM/COMMAND interface audit: all 90 BOP slots and 33
direct dependencies have an explicit source-interface disposition.  It finds
64 pointer-mapping migration debts, 43 retained OpenNT-shaped facades, six
approved opaque-handle exception routes and ten explicitly deferred product
paths; see the [P4 audit](etc/evidence/td-s2-p4-dem-command-interface-conformance-audit-001.md).
No BOP was enabled or reimplemented.  P5 next audits XMS/DPMI; S5 remains
frozen.

M0 Td S2 P5 closes the XMS/DPMI interface audit: all 37 BOP slots and 27
direct dependencies have an explicit disposition.  It retains only generic
ordinary-RAM mechanics under existing exceptions, records 17 pointer/seam
migration debts, and defers all 30 DPMI product-shell/frame/LDT/VDD paths;
see the [P5 audit](etc/evidence/td-s2-p5-xms-dpmi-interface-conformance-audit-001.md).
No `53:xx` ingress is admitted and S5 remains frozen.

M0 Td S2 P6 closes the remaining owner-package audit: every remaining 76 BOP
and 53 dependency rows has an explicit disposition.  It separates 28
guest-owned source/build rows from host composition, records 21 active
source-derived migration debts, and keeps 69 unavailable product-shell paths
deferred; see the [P6 audit](etc/evidence/td-s2-p6-remaining-interface-conformance-audit-001.md).
P7 must reconcile the whole 321-row matrix before S5 can be reconsidered.

M0 Td S2 P7 closes the full interface-conformance audit: all 321 tracker IDs
are uniquely classified and exception references reconcile to the register.
It rejects re-admission of T257 S5 because DPMI still has migration debt plus
whole-package deferred frame/IRET/LDT dependencies; see the
[P7 closure](etc/evidence/td-s2-p7-full-interface-conformance-closure-001.md).

M0 T256 closes the selector-blind protected virtual-access and guest-linear
substrate. Its final fresh 278-node formal Ninja build and XMEM fixture pass;
it deliberately does not claim DPMI activation. See [S15 evidence](etc/evidence/m0-t256-s15-protected-substrate-closure-001.md) and the [T256 closure](history/m0-t256-closure-20260823.md).

M0 T257 S1 closes the complete non-WOW DOSX/`dpmi32` owner, ABI and failure
map: all `53:00..18` enter one package, with historical NT4 hooks identified
as explicit source-composition gaps rather than authorization for leaf shims.
See [S1 evidence](etc/evidence/m0-t257-s1-dpmi-dosx-owner-abi-failure-map-001.md).

M0 T257 S2 closes the DOSX/DPMI source import boundary.  It fixes the exact
future mirror set and names every NT4 product import that requires a
source-derived seam or explicit refusal; the boundary scanner proves no DPMI
body or mirror is already an accidental product input.  See the
[S2 ledger](etc/operations/m0-t257-s2-dpmi-dosx-source-import-ledger-001.md).

M0 T257 S3 closes DOSX/DPMI startup and descriptor-table source composition.
`dpmi32.c` and `dpmiselr.c` now formally link through bounded named seams;
native Bochs consumes guest GDT/LDT entries directly, and no `53:xx` ingress
was enabled. See the [S3 closure](etc/evidence/m0-t257-s3-dosx-startup-table-closure-006.md).

M0 T257 S4 closes the DOSX/DPMI protected interrupt, fault and IRET source,
ABI and failure map. It proves that the current result ABI cannot atomically
restore the source frame, refuses direct old-NT4 hooks, and retains native
Bochs as the architectural exception/IRET owner; see the [S4 map](etc/evidence/m0-t257-s4-dosx-protected-frame-map-001.md).

M0 T256 S14 closes protected interrupt/fault/IRET classification. OpenNT uses
historical `NtVdmControl`, emulator hooks and locked DPMI stack frames, while
native Bochs remains CPU exception/IRET owner; see [S14 evidence](etc/evidence/m0-t256-s14-protected-interrupt-fault-iret-continuity-map-001.md).

M0 T256 S13 closes native descriptor-table lifecycle admission. DOSX `selGDT`
is a historical process-LDT alias: table mutation plus `53:00` host install is
DPMI package semantics, not a selector-blind machine lifecycle. Native Bochs
retains GDT/LDT/LLDT behavior; see [S13 evidence](etc/evidence/m0-t256-s13-native-descriptor-table-lifecycle-admission-001.md).

M0 T256 S12 closes the DPMI descriptor/guest-linear identity map. OpenNT's
`FlatAddress[]`, `Ldt`, `IntelBase` and x86 process-LDT calls are historical
host composition, while native Bochs already owns guest descriptor mechanics.
The next question is guest GDT/LDT lifecycle only; see [S12 evidence](etc/evidence/m0-t256-s12-dpmi-descriptor-guest-linear-identity-map-001.md).

M0 T254 closes the premature DPMI implementation admission.  The retained
OpenNT `dpmi32` package requires protected selector/LDT/context/IRET and
guest-linear memory substrate which the current copied-state boundary does not
provide; no `53:xx` leaf is independently admissible.  See the
[T254 closure](history/m0-t254-closure-20260823.md).

M0 T253 closes the selector-blind local async named-pipe completion slice.
`57:23/24` and the pipe-only `57:26` disposition recover the retained
descriptor/queue/ANR contract through a bounded bx-vdm seam; the native PIC
remains unchanged and NetBIOS/DLC remain deferred. See the
[T253 closure](history/m0-t253-closure-20260823.md).

M0 T251 closes the composable Redirector lifecycle/mailslot slice and the
source-shaped async-pipe boundary.  `57:09..0F` are fully recovered through a
session/PDB-owned provider; `57:23/24` correctly remain explicit failure
terminals until guest SFT opaque-token and `int5c` delivery owners exist. See
the [T251 closure](history/m0-t251-closure-20260823.md).

M0 T250 closes the apparent low-memory far-target question.  `0x0A87` is
original relocated `DOSDATA:OPENBUF+0x0F`, not an NTDOS code entry; the
subsequent far call is accidental execution of uninitialised guest storage.
It selects no BOP, Bochs or machine repair.  See the
[T250 closure](history/m0-t250-closure-20260823.md).

M0 T249 closes low-memory writer provenance.  The initial `0x0A87 <- FF`
mutation is a source-built original NTDOS `DOSCODE` CPU write at
`CS:base:EIP=9386:93860:7424`, not a Bochs, adapter or BOP-provider write.
See the [T249 closure](history/m0-t249-closure-20260823.md).

M0 T248 closes selector-blind post-EXEC real-mode continuity attribution. The
source-built checkpoint reaches accepted `50:36` and `50:1E`, then makes a
guest far indirect call from runtime-modified low NTIO memory
`0000:0A87 → 5859:5F04`; the target is zero RAM and reaches the bounded
execution budget. This is neither a BOP provider nor a missing Bochs machine
mechanic. See the [T248 closure](history/m0-t248-closure-20260823.md).

M0 T247 closes the NTDOS guest EXEC entry/parent-return boundary.  Its
source-built native checkpoint proves original `$Exec` PDB construction and
the accepted `50:36` child-entry handoff; the pre-return unexpected-loop
terminal transfers to active M0 T248's selector-blind machine-continuity
package, not to `50:1E demWrite` or an adapter DOS implementation. See the
[T247 closure](history/m0-t247-closure-20260822.md).

M0 T246 closes the shared SoftPC `02/06` machine interrupt package. Its
adapter-only active route preserves the source-derived stack/PIC/BDA mechanics
and records typed native resume before the distinct later `50:1E` owner. Later
native continuity is transferred to active M0 T247; see the [T246 closure](history/m0-t246-closure-20260822.md).

M0 T246 S1 closes the `02/06` owner/ABI map. The repeated `06` is an active
registry omission: the original shared operation stream, its named provider,
typed RAM/port actions and local transaction fixture already exist. No Bochs
intrusion is justified; see [S1 evidence](etc/evidence/m0-t246-s1-softpc-machine-interrupt-owner-map-001.md).

M0 T246 S2 closes active generic-ingress recovery for the shared original
SoftPC `02/06` pair.  The adapter-only bridge converts the existing typed
provider result; its native fixture proves both source paths, controlled
preflight stop and selector rejection.  S3 alone owns the bounded
source-built checkpoint; see [S2 evidence](etc/evidence/m0-t246-s2-softpc-machine-interrupt-active-ingress-closure-001.md).

M0 T246 S3 closes package regression and the bounded native checkpoint. Every
observed `06` now has typed resume disposition and the first group reaches a
later `50:1E` owner. Later `06` calls and the final unexpected-loop return
are not a registry-miss recurrence; see [S3 evidence](etc/evidence/m0-t246-s3-softpc-machine-interrupt-native-checkpoint-001.md).

M0 T245 closes source-built permanent-command bootstrap continuity through the
first distinct post-COMMAND machine edge. It fixes the actual Direct host path
admission without guest rewrite/copy or Bochs change, then transfers the
independent BOP-06 machine owner to M0 T246; see [T245 closure](history/m0-t245-closure-20260822.md).

M0 T245 S22 closes the session-scoped DOS-safe startup alias.  The CLI first
retains a suitable host short spelling, otherwise creates only a temporary
directory link to the real bundle root and removes that link on teardown.
The focused fixture exercises the no-symlink-privilege junction fallback; the
bounded native source-built run clears the former permanent-command
`ERROR_PATH_NOT_FOUND` and reaches original COMMAND services before an
independent machine `BOP 06` edge. See [S22 evidence](etc/evidence/m0-t245-s22-dos-safe-bootstrap-alias-closure-001.md).

M0 T245 S21 closes the OpenNT-shaped OEM `CreateFileOem` recovery. The
imported DEM body now uses the original OEM-to-Unicode/`CreateFileW` ordering
and generic-read attribute retry through a named public-API shim; fixtures
pass. Native call facts prove the remaining `AX=3` comes earlier: DOS
canonicalizes `REPOS.HOBBY` to nonexistent `REPOS.HOB` and the volume has no
short alias. The next owner is CLI/startup alias admission, not DEM/BOP/Bochs;
see [S21 evidence](etc/evidence/m0-t245-s21-opennt-oem-createfile-recovery-001.md).

M0 T245 S20 closes permanent-command DEM-open attribution. The bounded
adapter-only record retains four accepted `50:12` calls; the final original
`demOpen` receives the full source-built `COMMAND.COM` host path but returns
source-shaped `AX=3/CF`. The path exists and equivalent public host open
succeeds, assigning the issue to the current OEM file shim rather than BOP,
guest EXEC or Bochs; see [S20 evidence](etc/evidence/m0-t245-s20-permanent-command-dem-open-observation-001.md).

M0 T245 S19 closes post-CONFIG interrupt classification.  After the
successful temporary SCS open, the permanent `commnd` `INT 21h/AX=3D00h` is
still the final file operation: no seek, close or `4B00` EXEC follows before
the original `comerr → 50:3D` terminal.  The remaining owner is the guest
DOS-name to Direct-host namespace boundary, not cmdExec32 or Bochs; see
[S19 evidence](etc/evidence/m0-t245-s19-post-config-interrupt-classification-001.md).

M0 T245 S18 closes native DEM `50:12` observation. It opens the original
temporary SCS configuration file with CF clear, so neither `demOpen` nor
COMMAND current-directory is the retained failure; see [S18 evidence](etc/evidence/m0-t245-s18-dem-open-observation-001.md).

M0 T245 S17 closes native `54:04` output observation. The existing imported
COMMAND body returns Direct-host `O:\repos.hobby\runner`, then source-built
execution reaches/resumes `50:12 demOpen`; see [S17 evidence](etc/evidence/m0-t245-s17-command-current-dir-observation-001.md).

M0 T245 S16 closes the relative-root bootstrap defect. CLI admission now
canonicalizes DOS/WOW16 roots before the unchanged original COMMAND source
route; a relative-input native run observes an absolute guest `commnd` and
newly reaches/resumes `54:04`. The later `50:3D` is a distinct unmodified
path; see [S16 evidence](etc/evidence/m0-t245-s16-canonical-command-bootstrap-root-001.md).

M0 T245 S15 closes classification of the `comerr` incoming edge: existing
default-off real-mode interrupt observation shows `INT 21h AX=3D00h` at
`DS:DX=commnd`, with no `AX=4B00h` before `50:3D`.  Thus the current failure
is permanent COMMAND *open*, caused by the relative bundle-root descriptor;
see [S15 evidence](etc/evidence/m0-t245-s15-command-open-failure-classification-001.md).

M0 T245 S14 closes the passive source-mapped `commnd` observation.  A formal
fixture and one source-built native Direct run prove the original S13
`SHELL=` value reaches guest `sysinit2.asm:commnd` before accepted `5E`;
the retained `50:3D` maps to `sysinit1.asm:comerr → stall`.  The observation
does not distinguish the original pre-EXEC open-error and EXEC-error incoming
branches, so that later guest interrupt/error classification remains separate;
see [S14 evidence](etc/evidence/m0-t245-s14-command-bootstrap-observation-001.md).

M0 T245 S13 closes source-built `COMMAND.COM` publication through the original
`cmdconf.c → SHELL= → sysconf.asm:commnd` route. Its formal source-mirror
fixture passes bounds/grammar cases and the full native graph links. The one
native run remains at the pre-existing `5E → 50:3D` source terminal, so it
transfers only the guest `commnd`/`INT 21h` predecessor observation to S14;
see [S13 evidence](etc/evidence/m0-t245-s13-command-bootstrap-shell-recovery-001.md).

M0 T245 S12 closes the bootstrap-path attribution only: the original
`50:3D` `demExitVDM` route is a typed source terminal following accepted `5E`,
not a missing BOP result. Its former claim that publication necessarily
requires a new path policy is superseded by active S13's newly verified
`cmdconf.c → SHELL= → sysconf.asm:commnd` source route.

M0 T245 S11 closes the source-shaped `5E` config-complete body: the active
Direct DEM session executes `UMBNotify(0); demDasdInit();`, resumes at `RIP+3`
and preserves the source zero-floppy path.  One source-built run reaches and
accepts `5E`; its subsequent original `50:3D` terminal is separately owned by
the active S12 bootstrap-path disposition, not a 5E defect.

M0 T244 closes the headless native 8042 lifecycle. Its formal fixture proves
ports `60h/64h`, timer, ordinary IRQ1/PIC and two clean lifecycles without the
Bochs product shell; see the [T244 closure](history/m0-t244-closure-20260822.md).

M0 T243 closes the source-shaped, headless `STREAM_IO=2` BOP-5F sub-contract:
the `AX=BEEF` table guard and original `useHostInt10` publication now reach the
source-built path, then resume with CF to the original NTIO branch. Physical
keyboard/IVT/PIC/timer behavior remains explicitly transferred to M0 T244;
see the [T243 closure](history/m0-t243-closure-20260822.md).

M0 T242 closes the local guest EXEC prerequisite recovery: original
`INT 15h AH=88h` and x86 `50:42` fast-read source groups are composed and
observed, while the reached `5F/AX=BEEF` handoff is transferred intact to the
now-active keyboard/IVT machine package. See the
[T242 closure](history/m0-t242-closure-20260822.md).

M0 T241 closes the XMS `52:09` IVT-watch dependency: the direct
`UpdateKbdInt15` fragment now preserves original compare/clear/update order
through selector-blind stage state, and `xmsNotifyHookI15` returns source CX.
The formal native fixture passes; the one product-bundle run stops before any
BOP and makes no continuity claim. See the [T241 closure](history/m0-t241-closure-20260822.md).

M0 T240 closes the OpenNT-shaped SoftPC conventional-memory query:
the active `mem_size.c` mirror reads mutable BDA `0040:0013` through the
existing selector-blind checked-RAM action, then returns AX/RIP+3. No Bochs
intrusion was required. Its one bounded native run stopped before a BOP was
reached, so it is not a guest-continuity claim; see the
[T240 closure](history/m0-t240-closure-20260822.md).

M0 T239 closes the working-tree retirement of every experimental BOP-v1
provider, catalog, facade, legacy fixture and dead build/probe helper. The
sole useful fixed-width request record is current provider-neutral ABI, not a
legacy route. See the [T239 closure](history/m0-t239-closure-20260822.md).

M0 T238 closes the no-external OpenNT BOP validation slice: the frozen 75 BOP
and 30 dependency local contracts have a fresh 353-edge formal Ninja build and
31/31 focused fixture sweep, with no selected `bop-v1` product route. Its
Common Controls v6 activation repair is confined to the source-derived BOP 59
dialog seam. See the [T238 closure](history/m0-t238-closure-20260822.md).

M0 T237 closes the XMS source-mirror package: `52:06` now reaches the
imported `xmsInitUMB` source body through a checked AX:BX state-byte seam;
later original A20 calls publish to that byte; and `52:07/08` retain their
empty-map `B1/B2` behavior. Fresh formal Ninja r7, both XMS fixtures, the
v1-retirement gate and the machine-semantic boundary gate pass. The detailed
record is [T237 S5 closure evidence](etc/operations/m0-t237-s5-xms-umb-a20-closure-001.md);
the closed package is in [T237 history](history/m0-t237-closure-20260822.md).

Successful physical UMB reservation and `52:09` keyboard/IVT behavior remain
explicitly transferred to the future selector-blind machine/BIOS owner.

T237 P2 closes top-level `59`: `MS_bop_9` retains the original per-thread
AX-category warning and Terminate/Ignore result, using public Win32 UI for the
non-WOW single-session path. Ignore resumes at `RIP+3`; Terminate is a typed
bx-vdm controlled stop, never a host-process termination.

## Current Work Record

M0 T236 S4 is closed as the direct OpenNT DEM DASD/IOCTL recovery: imported
`50:21`/`50:29`/`50:2A` source now has a full direct/source-failure/machine
transfer ledger, a guarded malformed-IOCTL boundary, and a real disposable
VHDX fixed-volume lifecycle witness. See the [S4 closure](history/m0-t236-s4-closure-20260821.md).
M0 T236 S6 is closed as the OpenNT DEM search fallback disposition: the
removed `VdmQueryDir` fast path is explicitly unavailable, while the imported
slow reset resumes and exhausts search correctly after a source-owned handle
eviction. See the [S6 closure](history/m0-t236-s6-closure-20260821.md).
M0 T236 S5 is closed as the COMMAND WOW source disposition: direct imported
`cmdGetNextCmd` remains the normal local path; its original `VDMForWOW` branch
has a verified source-shaped controlled stop rather than a fabricated WOW
launch, while kernel publication and shortcut consumption transfer to the
queued WOW16 owner package. See the [S5 closure](history/m0-t236-s5-closure-20260821.md).
M0 T236 S3 is closed as the direct OpenNT DEM `50:32`/`50:33` recovery: its
single imported dispatcher table, copied-register retry seam and unified
opaque-handle route are recorded in the [S3 closure](history/m0-t236-s3-closure-20260821.md).
M0 T234 is closed as the OpenNT-shaped dynamic COMMAND-environment and
32-bit opaque-handle correction package; see the [T234 closure](history/m0-t234-closure-20260820.md).
T235 subsequently closed the PIF CONFIG/AUTOEXEC startup selection and fixed
guest bundle. M0 T236 now owns the remaining minimum-modification DEM/COMMAND
source recovery. M0 T231 closed as the COMMAND `54:00..10` code-complete/v2
product-route package; see the [T231 closure](history/m0-t231-closure-20260820.md).
Native `54:xx` reachability remains transferred to the queued bx machine/BIOS
owner package. XMS remains a candidate package.

M0 T232 is closed as the initial unified host-handle cleanup package; its
16-bit ID conclusion was corrected by closed M0 T234. M0 T233/S1 was admitted
but received no source or evidence work before the owner superseded it with
this ABI correction; XMS returns to the unnumbered candidate queue.
## Current Technical Baseline

- T95 is closed by owner-authorized reprioritization. Its incomplete file/search
  capability and full real-path closure remain uncompleted.
- M0 T197 S6 has a reproducible CPU5/non-x86-64 native link frontier: r4
  compiles the explicit SIM/machine/memory and source-proven reset/decode CPU
  candidates, then stops at the original complete instruction-handler table
  and retained product-shell methods.  No GUI, plugin/device archive, adapter
  or OpenNT input entered the probe; see
  [T197 S6 link probe 001](etc/research/t197-s6-cpu5-minimal-link-probe-001.md).
- M0 T197 S6 r11 now source-builds and links the whole CPU5 adopted core with
  the finite machine, without a Bochs product target.  This is a native link
  closure only; machine initialization/execution still requires separate
  lifecycle verification.
- M0 T197 S6 r12 verifies the native bare-machine lifecycle in process:
  `initialize(1 MiB, 1 MiB) -> CPU5 reset/APIC/CPUID -> cleanup()` returns
  zero.  No instruction-loop, BIOS/reset image, BOP or guest-running claim is
  made by that lifecycle witness.
- M0 T198 S1 finds the retained BOP runtime is a historical product derivative:
  it retains `main.cc`, GUI/device archives and product configuration.  It is
  not link-compatible with the finite machine.  The first second-stage closure
  is therefore a selector-blind native execution contract, not a per-BOP
  implementation; see
  [T198 S1 composition map](etc/research/t198-s1-bop-runtime-composition-map-001.md).
- M0 T198 S2 is complete: native CPU and PC-time sources prove a
  machine-private reset/copy/entry/run contract.  Its bound is a finite
  instruction-tick budget rather than an exact retired-instruction counter;
  the existing product `benchmarkTimer` is rejected because it also mutates
  GUI exit state.  See
  [T198 S2 execution-contract map](etc/research/t198-s2-native-execution-contract-map-001.md).
- M0 T198 S3 is complete: r8 source-builds and runs neutral `HLT` bytes
  through CPU5 reset, checked RAM copy, real-mode entry, 64-tick native stop
  and cleanup.  The needed A20 initialization is now a finite machine
  lifecycle invariant; no guest/BOP runtime claim follows.  See
  [T198 S3 finite-run witness](etc/research/t198-s3-finite-native-run-witness-001.md).
- M0 T198 S4 is complete: the current `#UD` seam map separates retained
  copied-state/resume mechanics from legacy `bx-vdm` dispatch, raw callback,
  environment and product-lifecycle coupling.  S5 will extract one fixed
  core/machine bridge before any BOP plane attachment; see
  [T198 S4 generic #UD seam map](etc/research/t198-s4-generic-ud-seam-map-001.md).
- M0 T198 S5 P1 is complete: machine now owns a standalone fixed generic-UD
  copied record and default-decline bridge, independently MSVC/x86 compiled
  with no adapter/product vocabulary or dependency.  Core has not yet been
  switched; see
  [T198 S5 P1 bridge record](etc/research/t198-s5-p1-generic-ud-bridge-record-001.md).
- M0 T198 S5 P2 is complete: a new default-off core gate source-builds under
  CPU5/MSVC and invokes only the fixed machine record/typed outcome path; its
  stop uses the finite PC stop latch.  Historical parallel gates remain for
  P3 removal and are not part of the new composition; see
  [T198 S5 P2 core hook](etc/research/t198-s5-p2-core-machine-ud-hook-001.md).
- M0 T198 S5 P3 is complete: the legacy machine-composition callback,
  product-stop path and every quarantined legacy exception implementation are
  physically removed from core.  Fresh CPU5 r3 source-builds the minimal
  fixture and traverses `UD2` through the fixed bridge to native controlled
  stop; see [T198 S5 P3 physical cleanup witness](etc/research/t198-s5-p3-physical-core-cleanup-witness-001.md).
- M0 T198 S6 P1 is complete: the alternative `bx-vdm` bridge converts the
  copied generic record through common ingress and provider registry while
  declining every outcome.  Its UCRT64 C-island test covers one mapped entry
  and three negative routes; see [T198 S6 ingress bridge witness](etc/research/t198-s6-global-ingress-bridge-witness-001.md).
- M0 T198 S6 P2 is complete: the toolchain audit proves the available MinGW
  producer is x64-only, so the classification bridge is not an in-process
  MSVC/x86 artifact.  A next composed bridge requires an owner-selected x86
  producer or a separately designed process ABI; see [T198 S6 x86 toolchain
  audit](etc/research/t198-s6-x86-vdm-toolchain-audit-001.md).
- M0 T198 S7 P1 is complete: eight i686 MinGW C objects replace the default
  bridge in the MSVC/x86 CPU5 finite link; the macro-enabled HLT fixture
  links and returns zero without entering #UD.  See [T198 S7 cross-island
  witness](etc/research/t198-s7-cross-island-link-witness-001.md).
- M0 T198 S8 P1 is complete: the first adapter-local pending descriptor was
  corrected into a machine-owned, selector-blind copied ordinary-RAM action
  ABI.  A native CPU5/x86 fixture proves read, max-size bulk write/read and a
  rejected out-of-aperture multi-range write with no partial write; an i686 C
  witness proves the fixed layout.  No provider is attached; see [T198 S8
  mechanical action boundary witness](etc/research/t198-s8-mechanical-action-boundary-witness-001.md).
- M0 T198 S8 is complete: its generic action ABI is source-built and native
  CPU5/x86 verified, while provider and legacy-runtime linkage remain absent;
  see [T198 S8 closure](history/m0-t198-s8-closure-20260813.md).
- M0 T198 S9 P1 is complete: the whole boot-namespace facade, its exact
  read/write action dispositions and its sole pathname prerequisite are
  source-mapped; the leaf closure is i686 source-built without the retained
  runtime.  See [T198 S9 composition closure map](etc/research/t198-s9-boot-namespace-composition-closure-map-001.md).
- M0 T198 S9 is complete: the complete family closure and DTA prerequisite
  are recorded without linking the historical runtime; see [T198 S9
  closure](history/m0-t198-s9-closure-20260813.md).
- M0 T198 S10 P1 is complete: a source-built i686 whole-family plane owns the
  immutable namespace and pending read state through common classification;
  it covers COMMAND boot write, DEM open/read and no-DTA pathname deferral.
  See [T198 S10 P1 witness](etc/research/t198-s10-p1-provider-plane-witness-001.md).
- M0 T198 S10 P2 is complete: the same plane fixture now verifies DEM seek
  and close direct-result routes after the open/read lifecycle; valid-DTA
  pathname and malformed-action cases remain P3.
- M0 T198 S10 P3 is complete: explicit valid DTA now admits pathname first
  `50:09` through its 134-byte read and provider-produced write.
- M0 T198 S10 P4 is complete: pathname next `50:0B` now consumes the first
  provider write as an addressed DTA image, rejects a mismatched action ID
  without consuming the pending read, then completes the valid fixed action.
  See [T198 S10 P4 witness](etc/research/t198-s10-p4-path-next-witness-001.md).
- M0 T198 S10 is complete: its selected whole-family plane is source-built
  and tested without legacy runtime linkage; its closure transfers only the
  generic-UD composition question to S11. See [T198 S10 closure](history/m0-t198-s10-closure-20260813.md).
- M0 T198 S11 P1 is complete: the generic event/outcome and mechanical action
  records already close the synchronous inner transaction. The only gaps are
  adapter-owned explicit session lifetime and action-result propagation; no
  core/machine ABI or semantic addition is indicated. See [T198 S11 P1 map](etc/research/t198-s11-p1-synchronous-composition-map-001.md).
- M0 T198 S12 P1 is complete: an explicit adapter-owned session now executes
  copied write/read actions synchronously and returns typed generic outcomes;
  unbound and executor-failure paths decline. COMMAND write results now resume
  instead of re-faulting. See [T198 S12 P1 witness](etc/research/t198-s12-p1-adapter-composition-witness-001.md).
- M0 T198 S12 P2 is complete: the core-facing alternative bridge is now a
  single adapter-session delegate, retaining default decline until bind. Its
  i686 transaction fixture and updated boundary scan pass. See [T198 S12 P2
  witness](etc/research/t198-s12-p2-composed-bridge-witness-001.md).
- M0 T198 S12 P3 reached and records the true native cross-island frontier:
  complete MinGW provider objects cannot safely link into the MSVC/x86 CPU5
  executable because they import a second CRT/helper universe. The probe did
  not run; no Bochs defect is inferred. See [T198 S12 P3 frontier](etc/research/t198-s12-p3-native-cross-island-link-frontier-001.md).
- M0 T198 S13 is complete: the full CPU5/Pentium-MMX source membership and
  finite machine source-build, link and run as one MSVC x64 `/MT` PE32+ process
  lifecycle. The x64 configuration projection corrects host pointer width
  without changing guest CPU identity; see [T198 S13 witness](etc/research/t198-s13-msvc-x64-minimal-lifecycle-witness-001.md).
- M0 T198 S14 is complete: a neutral one-byte HLT fixture enters the CPU5
  instruction loop and reaches finite controlled stop under the same MSVC x64
  `/MT` closure, with no adapter/provider or product input; see [T198 S14
  witness](etc/research/t198-s14-msvc-x64-finite-hlt-witness-001.md).
- M0 T198 S15 is complete: r6 repeats the neutral UD2 fixture under MSVC x64
  `/MT` and observes the selector-blind copied-event bridge's typed controlled
+  stop. Earlier r1--r5 observations are retained as rejected diagnostics;
  no BOP/provider/OpenNT claim follows. See [T198 S15 observation](etc/research/t198-s15-msvc-x64-generic-ud2-observation-001.md)
  and [closure](history/m0-t198-s15-closure-20260813.md).
- M0 T198 S16 is complete: the retained boot-namespace composition and its
  full session fixture now source-build, link and run as one MSVC x64 `/MT` C
  closure. The only platform library is `bcrypt.lib`, required by the existing
  CLI BYOB startup-contract model; no native-machine or historical runtime
  object entered. See [T198 S16 witness](etc/research/t198-s16-msvc-x64-boot-namespace-composition-witness-001.md)
  and [closure](history/m0-t198-s16-closure-20260813.md).
- M0 T198 S17 is complete: the existing adapter session is now attached to
  the CPU5 native generic-`#UD` mechanics under one MSVC x64 `/MT` process.
  A real `C4 C4 54:0C` BOP form completes its existing checked ordinary-RAM
  transaction and resumes to the bounded terminal fixture. This is not a DOS
  continuity claim; see [T198 S17 witness](etc/research/t198-s17-msvc-x64-native-bop-composition-witness-001.md)
  and [closure](history/m0-t198-s17-closure-20260813.md).
- M0 T198 S18 P1 is complete: the existing DEM readonly lifecycle `50:12 ->
  50:16 -> 50:00 -> 50:02` now executes as one native real-mode transaction
  under the S17 x64 closure. See [S18 DEM witness](etc/research/t198-s18-native-dem-lifecycle-witness-001.md).
- M0 T198 S18 P2 is complete: the sibling COMMAND `54:0D` boot-file service
  now has a separate parameterized native fixture and x64 zero-exit witness;
  see [S18 COMMAND witness](etc/research/t198-s18-native-command-bootfile-witness-001.md).
- M0 T198 S18 P3 is complete: the existing DTA registration and pathname
  search branches run natively as `50:1B -> 50:09 -> 50:0B -> HLT`, including
  their checked guest-RAM reads/writes and typed resumes.  See [S18 DTA/search
  witness](etc/research/t198-s18-native-dta-search-witness-001.md) and the
  updated [S18 matrix](etc/research/t198-s18-boot-namespace-native-matrix-001.md).
- M0 T198 S19 P1 establishes the first original NTDOS guest-entry map:
  `50:11` is a live `DI:0000` request after NTIO entry, not an initial preload;
  its unavailable historical composition has one bounded source-derived
  provider boundary.  See [S19 guest-entry map](etc/research/t198-s19-first-ntdos-guest-entry-map-001.md).
- M0 T198 S20 P1 composes source-derived DEM `50:11` into the one active
  `bx-vdm` session. Its x64 C closure and CPU5 native fixture prove one checked
  NTDOS publication, typed resume and negative controlled stop; see [S20
  witness](etc/research/t198-s20-native-dem-load-dos-witness-001.md).
- M0 T198 S21 P1 closes the NTIO pre-entry contract: pre-entry publication is
  a new selector-blind machine request, not a startup BOP or Bochs semantic;
  entry changes only CS:IP and stops at the already-admitted `50:11` boundary.
  See [S21 pre-entry map](etc/research/t198-s21-ntio-preentry-contract-map-001.md).
- M0 T198 S22 closes the selector-blind pre-entry primitive: a versioned,
  copied v2 request preflights a one-MiB aperture, captures/restores opaque
  bytes across image publication, and then applies CS:IP.  The full CPU5
  MSVC x64 `/MT` fixture proves restoration and rejection before mutation; see
  [S22 witness](etc/research/t198-s22-machine-preentry-witness-001.md).
- M0 T198 S23 P1 source-builds the `bx-vdm` NTIO request builder under MSVC
  x64 `/MT`: it copies only the exact `0x8400` image into machine v2 and
  source-locks `0x700`, `0x714`, `0070:0000`; execution is pending. See
  [S23 P1 builder record](etc/research/t198-s23-p1-ntio-request-builder-001.md).
- M0 T198 S23 P2 native-runs the exact source-built NTIO through `0070:0000`
  and observes real `50:11` before a later adapter STOP; see [S23 P2
  witness](etc/research/t198-s23-p2-native-ntio-first-boundary-001.md).
- M0 T198 S23 P3 restores generic accepted-STOP result observability: the
  finite CPU5 x64 UD2 regression returns typed controlled stop rather than
  conflating it with the watchdog timer.
- M0 T198 S23 now has a strict native first-handoff result: exact NTIO reaches
  `50:11`, then a later unimplemented boundary returns
  `COMPLETED_UD_STOP`; no DOS-continuity claim follows.
- M0 T198 S23 P4 maps that later boundary as top-level BIOS `BOP 12h`
  `memory_size`; the logged trailing `B1` is the next guest instruction, not
  a service ID. See [S23 P4 map](etc/research/t198-s23-p4-post-dem-bop12-map-001.md).
- M0 T198 S23 closes the exact NTIO first-handoff slice: its admitted `12h`
  result is native-resumed under MSVC x64 `/MT`, while `15h/AH=88h` remains
  explicitly declined; the next real boundary is DEM `50:3B`. See [S23
  closure](history/m0-t198-s23-closure-20260813.md).
- M0 T198 S24 P1 moves original DEM `50:3B` into a dedicated DEM-MISC plane
  entry and proves its non-debug `AL=0`/`RIP+4` result through the current
  MSVC x64 `/MT` CPU5 composition. Its post-resume HLT is now known to be the
  fixture mock NTDOS endpoint, not a later guest boundary; see [S24
  P1](etc/research/t198-s24-p1-dem-isdebug-native-resume-001.md).
- M0 T198 S24 P2 retains the two original HLT source sites as static facts,
  but its runtime attribution is superseded by the mock-NDTOS finding; see
  [S24 P2](etc/research/t198-s24-p2-native-hlt-source-triage-001.md).
- M0 T198 S24 P3 admits and verifies a default-off machine-private terminal
  CS:IP snapshot. The corrected `956B:0001` result is the deliberate mock
  NTDOS endpoint, not an NTIO/sysinit failure; see [S24
  P3](etc/research/t198-s24-p3-terminal-snapshot-001.md).
- M0 T198 S25 P1 embeds the exact locked source-built NTDOS input and reaches
  the first post-NDTOS real boundary, `54:05`, under MSVC x64 `/MT` CPU5;
  see [S25 P1](etc/research/t198-s25-p1-source-built-ntdos-next-boundary-001.md).
- M0 T198 S25 P2 composes original COMMAND `54:05` through a bounded launch
  state plane and reaches the next original boundary `50:1B`; see [S25
  P2](etc/research/t198-s25-p2-command-setinfo-native-001.md).
- M0 T198 S25 P3 corrects the existing DTA prerequisite's original DEM GSET
  gate: exact NTIO/NTDOS now completes `50:1B` through its checked two-byte
  guest-RAM read and typed resume, then reaches unimplemented original DEM
  `50:32`; see [S25 P3](etc/research/t198-s25-p3-dem-gset-dta-native-001.md).
- M0 T198 S26 P1 composes original DEM error-lock registration `50:32` through
  its existing two-locator provider and reaches the next unimplemented
  boundary `50:45`; see [S26 P1](etc/research/t198-s26-p1-dem-error-lock-native-001.md).
- M0 T198 S26 P2 composes non-debug `50:45` and snapshot-backed `50:0F`, then
  reaches the fixture's controlled stop; a separately retained extension run
  exposes an unmapped VGA machine frontier rather than authorizing devices.
  See [S26 P2](etc/research/t198-s26-p2-dem-init-trio-native-001.md).
- M0 T198 S27 establishes that `50:0F` itself cannot justify VGA: the retained
  extension reaches real-mode `LSL` before its `0xA2E63` VGA-aperture prefetch
  veto, while the original DOS init order is not yet proven continuous. See
  [S27 audit](etc/research/t198-s27-post-drive-machine-audit-001.md).
- M0 T198 S28 proves the retained unrestricted run does not first reach the
  proposed post-drive capture point: it hits six real-mode `LSL` reports and
  the same VGA-aperture panic before any observed drive resume. The temporary
  nonpassing probe was removed. See [S28 probe](etc/research/t198-s28-post-drive-generic-ud-probe-001.md).
- M0 T198 S29 captures the actual first non-BOP fault after a `50:0F` resume:
  real-mode `0000:0A84`, vector 6, with a copied all-`FF` window. It stops
  before the former VGA panic and moves the frontier to low-RAM control flow.
  See [S29 capture](etc/research/t198-s29-first-generic-ud-capture-001.md).
- M0 T198 S30 proves `0x0A84` lies inside the loaded `0x0700..0x8AFF` NTIO
  aperture, whose source bytes there are not all `FF`; the current CS also
  differs from the entry CS. The frontier is a guest mutation/transfer target,
  not an IVT, firmware, VGA, or device-admission request. See
  [S30 audit](etc/research/t198-s30-low-ram-frontier-ownership-audit-001.md).
- M0 T198 S31 adds a one-shot, opaque, checked machine terminal-RAM snapshot.
  The exact CPU5 run records `0x0A80..0x0A8F` as differing from its published
  NTIO bytes and confirms `0x0A84..0x0A87 = FF FF FF FF` at generic stop; no
  core, BOP, device, or CLI behavior changed. See
  [S31 snapshot](etc/research/t198-s31-terminal-ram-snapshot-001.md).
- M0 T198 S32 captures the complete existing copied event without pointer
  retention: its first generic fault has `SS:SP=00A7:090A` (physical
  `0x137A`), separate DS/ES state, and the same all-FF window/snapshot. The
  next evidence target is the raw stack range, not a device or service. See
  [S32 state](etc/research/t198-s32-first-generic-event-state-001.md).
- M0 T198 S33 captures the raw stack range around that pointer:
  `00A7 090E FFFF BF1F 0046 4D58 9346 76F6`. It proves a nonempty stack but
  cannot safely label a return pair; next is a read-only audit for existing
  selector-blind native instruction-history facilities. See
  [S33 stack](etc/research/t198-s33-first-generic-stack-snapshot-001.md).
- M0 T198 S34 rejects all existing instrumentation candidates: the CPU5 build
  compiles `BX_INSTRUMENTATION=0`, while the available active modules require
  a broader feature closure and raw CPU/logging coupling. A future trace must
  be a separately admitted fixed-width core-to-machine mechanic, never a BOP or
  debugger path. See [S34 audit](etc/research/t198-s34-instruction-history-seam-audit-001.md).
- M0 T198 S36 is complete: the default-off registered chained-step history
  rebuilt the full CPU5 source closure under MSVC x64 `/MT` and captured the
  first non-BOP generic fault `0000:0A84` with immediate predecessor
  `0000:0A82` on consecutive counts. This confirms a low-RAM
  mutation/control-transfer frontier, not a missing BOP handler. See
  [S36 witness](etc/research/t198-s36-chained-step-history-witness-001.md).
- M0 T198 S37 is complete: OpenNT `msinit` and DEM source prove current
  `50:0F` returns into legitimate guest low-memory/stack/IVT initialization,
  while current T198 evidence proves the old unhandled-`50:0F` explanation is
  stale. The exact CS-to-zero transfer and low-RAM writer remain unproven; no
  device, firmware or BOP change is admitted. See
  [S37 attribution](etc/research/t198-s37-low-ram-transfer-attribution-001.md).
- M0 T198 S38 is complete: the registered, default-off machine-only query
  proves the latest retained code-segment transition before the generic stop
  is `9346:4D53 -> 0000:0000`; the known `0000:0A82 -> 0000:0A84` pair occurs
  later. The transfer instruction and owner remain unproven, and no machine
  capability was added. See
  [S38 witness](etc/research/t198-s38-cs-transition-witness-001.md).
- M0 T198 S39 is complete: source/evidence audit proves the finite fixture
  restores an undefined bare-RAM virtual-interrupt span and omits the other
  evidenced NTIO pre-entry inputs. Its startup trace is therefore input
  contaminated, not evidence for a missing device or BOP. See
  [S39 attribution](etc/research/t198-s39-preentry-contract-attribution-001.md).
- M0 T198 S40 is complete: the selected profile's complete currently
  evidenced pre-entry snapshot is atomically installed through the existing
  machine mechanical action. The old all-`FF` `0A84` frontier is gone; the new
  reproducible `0000:0047` frontier follows the still-unexplained
  `9346:4D53 -> 0000:0000` transfer. See
  [S40 closure](etc/research/t198-s40-preentry-input-closure-001.md).
- M0 T198 S41 is complete: the registered copied position record proves that
  the unexpected CS transition changes SS:SP from `00A7:0918` to
  `00A7:090E`. The frame owner remains unproven. See
  [S41 witness](etc/research/t198-s41-stack-position-witness-001.md).
- M0 T198 S42 is complete: the registered bounded machine provenance record
  proves that the preceding guest instruction is `SS: CALL FAR [037A]` and
  retains the matching `9346:4D58` far return address; S44 corrects its
  non-direct `0000:0000` operand inference. It does not establish a BOP,
  device, or adapter requirement. See [S42 control-transfer
  provenance witness](etc/research/t198-s42-control-transfer-provenance-001.md).
- M0 T198 S43 is complete: the original NTDOS `DEVIOCALL2` source and locked
  image map identify `SS:[037A]` as the DOSDATA `CALLDEVAD` scratch slot;
  S44 corrects the earlier unproven missing-device-chain inference. See
  [S43 pointer attribution](etc/research/t198-s43-devio-call-pointer-attribution-001.md).
- M0 T198 S44 is complete: the existing terminal snapshot observes
  `CALLDEVAD=BF1F:FFFF`, moving the remaining question to original
  end-of-segment execution continuity. See [S44 terminal
  witness](etc/research/t198-s44-call-dev-ad-terminal-witness-001.md).
- M0 T198 S45 is complete: the original split-at-`FFFF` strategy layout and
  unchanged upstream CPU prefetch path correlate with the `EIP=0x10000`
  diagnostic; a neutral reproduction is required before repair. See [S45
  source audit](etc/research/t198-s45-realmode-ip-wrap-source-audit-001.md).
- M0 T198 S46 is complete: the neutral split-instruction fixture repeats the
  exact `EIP [00010000] > CS.limit [0000ffff]` prefetch diagnostic with no
  guest composition input. S47 is therefore a generic core repair, not an
  adapter workaround. See [S46 neutral witness](etc/research/t198-s46-neutral-realmode-ip-wrap-witness-001.md).
- M0 T198 S47 is complete: one selector-blind code-offset-width helper repairs
  the neutral `CS:FFFF` instruction path; its HLT and generic UD2 STOP
  regressions pass after finite early-stop cleanup. See [S47 repair
  witness](etc/research/t198-s47-realmode-ip-width-repair-001.md).
- M0 T189 S5 is complete: the four-object source closure proves that the
  previous `54:0C` preparation decline came from stale retained provider and
  command-service objects, not a proven OpenNT or Bochs defect.  The valid
  frozen run commits the existing boot-file transaction and reaches controlled
  stop; it does not claim continuous execution.
- M0 T189 is closed: its valid trace transfers original COMMAND `54:02`
  `cmdComSpec` rather than a Bochs/machine issue; T190 begins with the paired
  `54:02`/`54:0F` source contract map.
- T96 S1 P1 is complete locally: all inventoried selector, service, and BIOS
  forms now have source routing/owner evidence without an enabled provider.
- Bochs remains generic-machine owner. The adapter owns a bounded copied BOP
  ingress and typed result transition, not DOS, BIOS, CCPU, or device behavior.
- Provider precedence is mandatory: original OpenNT host provider; original
  provider with contained CLI backend; source-derived provider only after a
  recorded blocker; otherwise unavailable/deferred.
- M0 T96 S3 is closed: its DEM plane classifies every callable service, and
  all admitted DEM slices are plane-gated. Legacy runtime endpoint migration
  remains expressly deferred to S8.
- M0 T96 S5 is closed: all XMS/DPMI services have a machine-owner disposition;
  only DPMI VCD is explicitly unavailable, and no native-machine operation is
  represented as adapter behavior.
- M0 T96 S6 is closed: all Redirector and debugger services have source-backed
  profile dispositions; selector-level VDD, mouse and device boundaries remain
  explicitly outside adapter execution.
- M0 T96 S7 is closed: the complete SoftPC BIOS inventory has native-owner,
  unavailable or collision disposition, without importing a BIOS dispatcher.
- M0 T96 S8 is closed: legacy runtime identities are registered and gated or
  rejected, with a scan preventing unapproved direct BOP recognizers.
- M0 T96 S9 is closed: complete inventory/profile regression covers all
  selectors, service identities and BIOS forms without claiming runtime.
- M0 T96 is closed: its real-path checkpoint exposes the original-unavailable
  DEM FASTREAD edge, without changing Bochs or fabricating a BOP outcome.
- M0 T198 S78 corrects the retained #UD display interpretation:
  `window=15:ffffffff` is a 15-byte window beginning `FF FF FF FF`, not BOP
  `15h`. Its ROM/IVT closure map identifies absent approved ROM payloads as
  the current machine-input gap; no BIOS/BOP route is added. See [S78 closure
  map](etc/research/t198-s78-minimum-rom-ivt-closure-map-001.md).
- M0 T198 S79 finds no current source-authorized C000 image producer: OpenNT
  retains names only, while Bochs VGABIOS is prebuilt and requires excluded
  VGA lifecycle. S80 therefore audits the original display-mode selection
  before any profile or machine decision. See [S79 producer
  audit](etc/research/t198-s79-machine-image-producer-audit-001.md).
- M0 T198 S80 establishes the original display-state chain: BOP `5F` supplies
  the resident mode byte, and only fullscreen jumps to the saved native INT 10
  vector. S81 is observation only; it will not select a CLI mode or attach a
  provider. See [S80 chain audit](etc/research/t198-s80-display-mode-vector-chain-audit-001.md).
- M0 T198 S81 confirms that the current source-built full trace does not reach
  BOP `5F` before the C000 transfer. S82 now audits the guest-image and
  initialization path, without forcing a mode byte or adding a provider. See
  [S81 passive observation](etc/research/t198-s81-bop5f-passive-observation-001.md).
- M0 T198 S82 proves that `073B:0939` is a dynamically patched SpcKbd native
  INT10 far jump, not random Bochs control flow. M0 T198 S83 brackets the
  pointer write after first `50:11`; M0 T198 S84 proves BOP-`5F` was consumed
  before the passive recorder; M0 T198 S85 classifies its host-contract delta;
  M0 T198 S86 selects the explicit stream profile. S87 implements only
  its source-derived K.SYS state publication. See [S86 profile design](etc/research/t198-s86-cli-stream-profile-design-001.md).
- M0 T198 S87 is complete: an explicit `STREAM_IO=2` input permits only the
  exact BOP-`5F` two-byte K.SYS offset read followed by one typed ordinary-RAM
  byte write and the retained CF/RIP continuation. The new API is intentionally
  unbound because no BYOB profile field exists yet; see [S87 transaction
  record](etc/research/t198-s87-spckbd-stream-state-transaction-001.md).
- M0 T198 S88 is complete: profile v7 now selects only `stream-io-v1`, which
  becomes the immutable value `2` in composition v2; that composition performs
  S87's exact BOP-`5F` transaction or declines it rather than falsely resuming.
  See [S88 profile/composition record](etc/research/t198-s88-profile-stream-composition-001.md).
- M0 T198 S89/S90 are complete: a current-object MSVC x64 native run proves
  that the exact BOP-`5F` CS:SI table / DS-relative state transaction selects
  the guest stream branch and avoids the former C000 transfer.  Its next
  boundary is `50:3D`, which S91 source-classifies as fatal `demExitVDM`, not a
  normal return; no provider or runtime behavior was added.  See [S89/S90
  closure](etc/research/t198-s89-s90-native-spckbd-cs-si-closure-001.md) and
  [S91 classification](etc/research/t198-s91-dem-exit-vdm-classification-001.md).
- M0 T198 S92 is complete: the profile-declared SHARE normal-return candidate
  is present and hash-verified, but the current native fixture contains only
  synthetic COMMAND/target bytes.  Repeating it cannot test `50:3C` or
  `54:11`; S93 owns identity-locked fixture inputs before any next run. See
  [S92 prerequisite map](etc/research/t198-s92-normal-return-candidate-prerequisite-map-001.md).
- M0 T198 S93 is complete: the exact source-built COMMAND.COM and SHARE.EXE
  inputs, plus an explicit v7 stream/TARGET.EXE profile, source-build into the
  native fixture branch without link or guest execution.  S94 may make one
  bounded exact-input observation; see [S93 preparation](etc/research/t198-s93-source-built-normal-return-fixture-preparation-001.md).
- M0 T97 S1 is complete: no original standalone provider exists; the frozen
  read-only and search candidates are only implementation inventory.
- M0 T97 S2 is complete: partial runtime dispatch and the contradictory
  FASTREAD experiment were withdrawn; the immutable token design is retained.
- M0 T97 S3 is complete: pathname and FCB searches have separate source
  contracts, and the host-directory fixture is expressly excluded.
- M0 T97 S3 P1 is complete: pathname has a source-shaped future-provider
  boundary; FCB remains explicitly deferred pending its distinct ABI.
- M0 T97 S3 P2 records that the existing host-directory search fixture is
  outside profile and not linkable on the current SDK; it will not be patched.

## Recent Progress

| Scope | Compact result |
| --- | --- |
| M0 T95 | Closed/replanned; closure facts, transferred backlog, and limitations are in history/m0-t95-closure-20260811.md. |
| M0 T189 S5 | Complete: four-object derivative and one manifest-bound observation commit `54:0C` and reach controlled stop; S6 is source-only audit. |
| M0 T189 | Closed: stale object closure is proven; `54:02` COMMAND bootstrap contract transfers to T190. |
| M0 T96 S1 P1 | Complete static BOP selector/service/BIOS provider and module map; local commit pending because no Git remote is configured. |
| M0 T96 S2 P1--P2 | Complete shared ingress, typed pass-through gate, and provider selection registry; legacy endpoint migration remains S8. |
| M0 T96 S3 | Complete: all 73 callable DEM services have a component/disposition, and every admitted DEM slice is plane-gated; legacy runtime migration remains S8. |
| M0 T96 S4 | P1--P5 complete: COMMAND map/plane/no-op are closed; CONFIG/AUTOEXEC and target-launch legacy endpoints are reconciled as S8 migration objects. |
| M0 T96 S5 | Complete: 12 XMS and 25 DPMI services are source-mapped and plane-classified without linking historical dispatchers. |
| M0 T96 S6 | Complete: Redirector is source-proven unavailable; debugger and native device selectors remain deferred to their proper owners. |
| M0 T96 S7 | Complete: 41 definitions and 349 historical BIOS table forms have a mechanically verified ownership disposition. |
| M0 T96 S8 | Admitted in Ordinary Mode; existing endpoint migration and no-bypass verification begins. |
| M0 T96 S8 P1 | Complete: legacy endpoint register and no-bypass scanner are active; `host_service_catalog` now consumes common ingress rather than decoding BOP bytes. |
| M0 T96 S8 P2 | Complete: a generic DEM/COMMAND plane gate is verified; the first six runtime endpoints are gate-only migrations with no semantic expansion. |
| M0 T96 S8 P3 | Complete: all DEM/COMMAND legacy endpoint entry stages with a proven service identity now pass the common plane gate; runtime syntax verification passed. |
| M0 T96 S8 P4 | Complete: the conflicting legacy DEM FASTREAD path is removed from runtime, preserving its S3 original-no-op disposition. |
| M0 T96 S8 | Complete: legacy recognizers are plane-gated, frozen or rejected; no-bypass verification is active. |
| M0 T96 S9 | Admitted in Ordinary Mode; full inventory/provider/capability regression begins. |
| M0 T96 S9 | Complete: 192 callable services traverse ingress, registry and plane classification; inventory counts are independently verified. |
| M0 T96 S10 P1 | Complete checkpoint: r49 linked all T96 plane/gate objects and a fresh bounded NTIO/NTDOS run reached the original-unavailable `50:42` boundary; see etc/research/t96-s10-real-path-checkpoint-001.md. |
| M0 T96 | Closed: source ownership, the holistic BOP plane, and the current real-path boundary are recorded in history/m0-t96-closure-20260812.md. |
| M0 T97 S1 | Complete: component/ABI/failure and composition map records the source-derived replacement boundary. |
| M0 T97 S2 | Complete: provider withdrawal and immutable token/resource contract recorded; no FASTREAD helper remains. |
| M0 T97 S3 | Active: pathname/FCB directory-search ABI audit. |
| M0 T97 S3 P1 | Complete: separate DTA and FCB buffer/error/continuation map recorded; no search runtime provider is enabled. |
| M0 T97 S3 P2 | Complete limitation: host-directory fixture is excluded from T97 and its SDK-native query branch is not repaired. |
| M0 T97 S4 P1 | Complete: v4 BYOB profiles admit an exact, immutable four-entry DOS metadata snapshot; pathname routing remains disabled pending the common-plane integration slice. |
| M0 T97 S4 P2 | Complete focused closure: common DEM-plane `50:09/0B` routing consumes only copied guest inputs and the immutable v4 snapshot; FCB and ambient host namespace remain excluded. The legacy runtime fixture needs replacement because it asserts the withdrawn file-service path. |
| M0 T97 S4 P3 | Complete: default runtime regression no longer asserts withdrawn file-token services; its focused v4 mode passes DTA registration, `50:09` success, and `50:0B` no-more. |
| M0 T97 S5 | Complete: v4 acceptance/rejection fixtures pass; the retained engine rejects the v4 execution plan before CPU loop, so zero BOP/transaction is correctly classified as stale engine composition. |
| M0 T97 | Closed: finite profile namespace and pathname provider are source/runtime closed; engine recomposition is transferred to T98. |
| M0 T98 S1 | Complete: retained engine/adapter object and link-closure inventory. |
| M0 T98 S1 P1 | Complete: r5 manifest/makefile/map establishes the exact missing current adapter/profile object set; a narrow derivative relink is the only admitted next build candidate. |
| M0 T98 S1 P2 | Complete: r2 generator/dry-run retains r5 Bochs inputs and exposes only 65 adapter/CLI compilations plus one final link; inherited archive recursion was rejected. |
| M0 T98 S2 | Complete: r3 compiled 66 adapter/CLI objects (including BOP observation) and linked the current composition without a Bochs rebuild. |
| M0 T98 S3 | Complete: bounded v4 trace passed the stale pre-CPU rejection, observed 15 BOPs and three commits, and ended at the benchmark budget. |
| M0 T98 | Closed: current adapter-to-engine composition is source-built and bounded-runtime observed; continuous guest execution remains unproven. |
| M0 T99 S1 | Complete: reached service families map to original components; finite read-only boot namespace is the selected whole provider slice. |
| M0 T99 S2 P1 | Complete: one provider-owned finite namespace state now combines paired config-path production, normal read-only lifecycle and search state; focused producer/consumer test rejects FCB routing. |
| M0 T99 S2 P2 | Complete: v4 runtime now consumes the provider for CONFIG/AUTOEXEC and pathname-search state; narrow engine relink and one bounded `54:0C` trace pass without a Bochs rebuild. |
| M0 T99 S2 P3 | Complete: normal DEM open/seek/read/close moved into the same provider/session family; focused lifecycle passes and the r2 bounded run reaches `54:0C`, `50:12`, `50:00`, then original-unavailable `50:42`. |
| M0 T99 | Closed: finite read-only boot namespace provider is source/test/build closed; one real consumer reaches the next original-unavailable DEM boundary, with no continuous-execution claim. |
| M0 T100 S1 | Complete: whole original-unavailable DEM provider composes through ingress/registry/plane and source/test/build/runtime evidence. |
| M0 T100 | Closed: all eight original-unavailable DEM slots compose through common selection; trace proves `50:42` resume and later close before the separate Bochs prefetch-map stop. |
| M0 T101 | Closed: `0xAF6AF` is a native VGA-aperture execution veto, not a missing mapping/device; upstream transfer remains unobserved. |
| M0 T102 | Closed: retained original diagnostics already prove the zero INT10 vector/direct-entry cause; no new Bochs diagnostic is needed. |
| M0 T103 | Closed: profile payload and handoff pairing are correct; direct/deferred consumer mutual exclusion and the two-object closure are proven. |
| M0 T104 | Closed: current-adapter deferred derivative rebuilt exactly two Bochs objects and reached the real native POST BOP path without an aperture panic. |
| M0 T105 | Closed: one longer unchanged-binary observation reached a stable source-owned INT6/BOP-06 re-entry loop; no service behavior was changed. |
| M0 T106 | Closed: common gather/read/write/resume bridge is connected; the repeated predecessor is original `MS_bop_2` VdmRedir-unavailable behavior. |
| M0 T107 | Closed: exact unavailable result is source/test/build closed; short trace did not reach it. |
| M0 T108 | Closed: selector 02 is runtime-reachable, but its accepted result exposed a source-identity error: `02` is XMS, not VdmRedir. |
| M0 T109 | Closed: selector 02 is unclaimed; the focused static test proves `52h` XMS and `57h` VdmRedir identities, with no runtime claim. |
| M0 T110 | Closed: selector 02 is SoftPC `unexpected_int` plus guest IRET, with PIC/BIOS-data side effects; it is not an adapter service. |
| M0 T111 | Closed: original semantics require Bochs-owned PIC port dispatch and RAM; direct link and adapter reimplementation are both rejected. |
| M0 T112 | Closed: source closure is finite but current ABI cannot synchronously transport multi-step native I/O; generic port API is rejected pending original compile proof. |
| M0 T113 | Closed: unmodified original handler compiles in fresh clang-cl/x86 OBJECT island; only `_inb`, `_outb`, `_c_sas_store` remain semantically unresolved. |
| M0 T114 | Closed: three wrapper ABI is finite and same-binary viable, but no owner may currently select/enter an original machine BOP handler. |
| M0 T115 | Closed: T115 selects a catalogue-driven machine-composition component distinct from the adapter host-service plane and records its exact future seam. |
| M0 T116 | Closed: `02h` contract, original object and three wrappers are static/x86-link closed without handler invocation. |
| M0 T117 | Closed: selector-blind default-off seam source-builds in a two-object derivative with unchanged decline behavior. |
| M0 T118 | Closed: original `02h` caller/IRET continuation and generic three-byte resume contract are source-proven. |
| M0 T119 | Closed: original `unexpected_int` and its exact three mechanics imports link into the default-off, selector-blind derivative; no runtime result is claimed. |
| M0 T120 | Closed: the one direct-start r3 observation ends at the known aperture veto with BOP identity observation disabled; it neither proves nor disproves `02h` reachability. |
| M0 T121 | Closed: deferred POST, the selector-blind machine seam and existing BOP listener source-build in one no-archive-rebuild derivative. |
| M0 T122 | Closed: the frozen deferred image reaches 31 observed BOPs and normal CPU-loop shutdown; `02h` is not observed and no machine handler result is claimed. |
| M0 T123 | Closed: thin CLI-to-Bochs shim startup descriptor is frozen. |
| M0 T124 | Closed: CLI-only shim source/test closure verifies the fixed, manifest-identified bundle contract with a fake native parser; real Bochs remains T125 evidence. |
| M0 T125 | Closed: fixed bundle reaches the native parser/POST and a repeated `50:11` BOP path; watchdog terminates the bounded process tree, so no continuous-execution claim is made. |
| M0 T126 | Closed: `50:11` is original `demLoadDos`; its existing source-derived transaction is linked, but T121 explicitly disabled the generic CPU result bridge required to commit/resume it. |
| M0 T127 | Closed: one source-identical derivative enables only the generic CPU-result bridge; `cpu/exception.o` and final map build with no archive or device expansion. |
| M0 T128 | Closed: CPU-result bridge alone remains inert because its call site is nested below `STARTUP_TRANSACTION=0`; the observed `50:11` loop remains a composition fact, not a new BOP. |
| M0 T129 | Closed: the obsolete parallel startup/result bridge is rejected; its unresolved imports cannot become a second adapter composition. |
| M0 T130 | Closed: selector-blind machine composition reaches original `unexpected_int` and resumes through native Bochs mechanics; continuous execution remains unproven. |
| M0 T131 | Closed: full SoftPC BIOS inventory has verified native, unavailable, or collision ownership without a BIOS dispatcher. |
| M0 T132 | Closed: the sole original handler island has an unresolved CRT-model conflict and cannot be copied as a general provider recipe. |
| M0 T133 | Closed: the deferred composition already has native POST/IVT/INT10 lifecycle; no reset/device correction is admitted. |
| M0 T134 | Closed: the late fault is original INT 0Dh/BOP 02 return looping on guest control state; its upstream control transfer remains unobserved. |
| M0 T145 | Closed: no retained exact COMMAND source-offset map/listing binds the controlled COM image tail to a source label; heuristic attribution remains rejected. |
| M0 T146 S1 | Complete: one r5-pinned CPU archive enables only BX-TRACE-056 and links against byte-identical T130 non-CPU/mechanics inputs; see etc/research/t146-s1-single-diagnostic-cpu-archive-closure-001.md. |
| M0 T146 | Closed: its single observation stopped early at `50:36`, so it neither reaches nor explains the late prefetch fault; retry and semantic changes are rejected. |
| M0 T147 | Closed: full CPU archive recompilation leaves an unresolvable provenance variable; exactly-one archive-member replacement is the next minimal comparison. |
| M0 T148 | Closed: a one-member CPU archive derivative retains 92 byte-identical CPU members and enables only BX-TRACE-056. |
| M0 T149 | Closed: the sole extended observation reaches and repeats the known prefetch/#GP/BOP02/IRET loop; watchdog prevents a clean completion claim. |
| M0 T150 | Closed: original INT 0Dh/BOP02/IRET source proves replay of the existing fault frame, not a new adapter or machine-service failure. |
| M0 T151 | Closed: exact staged COMMAND image ends at `0BC1:C5CF`; the `0BC1:FFFF` fault lies outside it. |
| M0 T152 | Closed: original x86 fast-read selects the visible no-op `50:42` result, explaining why the current profile cannot populate the COM image. |
| M0 T153 | Closed: source-derived fast-I/O contract selects contained `50:42` read only and retains `50:43` unavailable. |
| M0 T154 | Closed: adapter-only `50:42` fast-read provider is source/test/build and runtime-plane closed; `50:43` remains original-unavailable. |
| M0 T155 | Closed: a source-built current composition reaches full COMMAND fast-read and EOF; a later stack-prefetch triple fault is separately admitted to T156. |
| M0 T156 | Closed: recursive stack #SS is correctly enforced by Bochs; the unobserved first vector/control predecessor is transferred to T157 diagnostic design. |
| M0 T157 | Closed: existing BX-TRACE-049 provides the required generic real-mode vector record; T158 may enable it without a new source intrusion. |
| M0 T158 | Closed: generic vector trace proves the known direct-pre-POST zero-INT10 path; T159 audits the existing POST/deferred alternative. |
| M0 T159 | Closed: current adapter retains a no-source-change native POST/deferred closure with exactly two Bochs object replacements; T160 may build and observe that form once. |
| M0 T160 | Closed: fresh x86 two-object composition reaches full COMMAND fast-read and normal `50:36` resume through native POST; T161 may observe the unchanged binary once at a longer budget. |
| M0 T161 | Closed: unchanged composition reaches accepted `FE` controlled stop after `50:36`; T162 must source-classify its CLI completion meaning. |
| M0 T198 S126 | Complete: the explicit CLI launch plan, `54:05` registration, CMDINFO gather, checked response writes and one-slot `54:01` commit are source/test/build closed in the adapter-only composition. |
| M0 T198 S127 | Complete: the sole locked native observation resumes past assembled `54:01` and reaches `54:0E`; it does not claim target completion. |
| M0 T198 S128 | Complete: `54:0E` is mapped to original `cmdGetKbdLayout`; the existing finite `DX=0` helper is admissible only as a globally classified CLI rehost. |
| M0 T198 S129 | Complete: the finite `54:0E` provider is composed after global classification; the focused adapter-only closure verifies `DX=0`, `+4` resume and protected-mode decline. |
| M0 T198 S130 | Complete: the sole locked native observation resumes past composed `54:0E` and reaches `54:04`; it does not claim target or console completion. |
| M0 T198 S131 | Complete: `54:04` is mapped to original `cmdGetCurrentDir`; its immutable-root candidate is admissible only as a globally classified CLI rehost. |
| M0 T198 S132 | Complete: the finite `54:04` provider is composed after global classification; its focused closure verifies root write, typed no-write failure and protected-mode decline. |
| M0 T198 S133 | Withdrawn before acceptance by owner policy correction; no native result is retained. |
| M0 T198 S134 | One native observation stopped at `54:01`, not `54:04`; it is non-closure evidence only. |
| M0 T198 | Closed by owner-directed reprioritization; it does not claim complete BOP, COMMAND or runtime closure. |
| M0 Td S1 P1--P3 | Documentation governance remains one active S in Status, T-only Queue, indexed supporting evidence, and a hash-verified full-document inventory. |

**S25 P9:** all 25 `dpmi32/*` paths now have final dispositions: the 16
original x86 DPMI host-provider bodies are `adapter-backed`, five private
declaration forms are `binding-only`, and two RISC-only bodies plus two build
descriptions are `not-host-runtime`. The original 32-bit pointer-shaped
contracts bind only to the session `guest_memory` mapping-manager instance and
the `adapter-mvdm-host-out/softpc -> adapter-bochs` mechanical path; see the [DPMI32 final
baseline](etc/operations/m0-t276-s25-dpmi32-final-disposition-baseline-001.md).

**S25 P10:** all 40 `dos/*` paths now have final dispositions: all 27 original
DEM/COMMAND provider bodies are `adapter-backed`, seven declaration carriers
are `binding-only`, and package build/test metadata is `not-host-runtime`.
Recovery remains whole original DEM and COMMAND provider packages—not
trace-selected services—through the named BOP, SoftPC, Win32, session and
Redirector boundaries; see the [DOS final baseline](etc/operations/m0-t276-s25-dos-final-disposition-baseline-001.md).

**S25 P11:** all 21 `fax/*` paths now have final dispositions: the four
original user-mode WOWFAXUI providers are `adapter-backed`, four declarations
are `binding-only`, and the private CSRSS/GDI driver plus all product/build
resources are `not-host-runtime`. The UI package keeps its original flow while
`adapter-mvdm-host-out/wow`, `adapter-mvdm-host-out/win32` and session copied broker records replace only
the private shared-section transport; see the [FAX final baseline](etc/operations/m0-t276-s25-fax-final-disposition-baseline-001.md).

**S25 P12:** all 47 `bde`, `dbg`, `vdmdbg` and `vdmexts` paths are `tool-only`:
they are independent original NTSD/WinDbg extensions, debug-support library or
debugging-support DLL, never a main host-runtime library. Their remote-process,
symbol and fixed-image-address logic remains byte-exact evidence for a future,
separately admitted debugger-tool package; see the [debugger-tools final baseline](etc/operations/m0-t276-s25-debugger-tools-final-disposition-baseline-001.md).

**S25 P13:** all 339 selected `softpc.new` declaration carriers are
`binding-only`. They retain the original interface shapes for source-parity
MVDM bodies and same-shaped adapters but never authorize a second SoftPC
executor or a direct Bochs-object dependency; see the [SoftPC declaration final baseline](etc/operations/m0-t276-s25-softpc-declarations-final-disposition-baseline-001.md).

**S25 P14:** the 25 remaining `softpc.new` build/resource rows now have final
dispositions: debugger/disassembler inputs are `tool-only`, while standalone
SoftPC/NTVDM launch, link and resource inputs are `not-host-runtime` because
`app` owns final composition. No source body is enabled; see the [SoftPC resource final baseline](etc/operations/m0-t276-s25-softpc-resources-final-disposition-baseline-001.md).

**S25 P15:** the twelve original `softpc.new` debugger/disassembler source
bodies are `tool-only`; the two historical `obj.vdm` standalone product-entry
sources are `not-host-runtime`, with `app` owning final composition. Remaining
machine/control sources stay under their source-span owner evidence rather than
being inferred from a directory name; see the [SoftPC tool/product-source final baseline](etc/operations/m0-t276-s25-softpc-tool-source-final-disposition-baseline-001.md).

**S25 P16:** `softpc.new/base/support/get_env.c` is an `adapter-backed`
original environment helper. The other five support bodies are
`not-host-runtime`: they own an alternate IO table, CPU/register state, BIOS
timer or standalone SoftPC lifecycle and would create a second executor beside
Bochs; see the [SoftPC support-source final baseline](etc/operations/m0-t276-s25-softpc-support-source-final-disposition-baseline-001.md).

**S25 P17:** the 18 original SoftPC device bridge bodies now have final owner
dispositions: ten native video/FDC/DMA/timer/PIC/PPI-sound executors are
`not-host-runtime` because Bochs owns those devices; eight host
endpoint/control providers are `adapter-backed` through public host capability
and typed `adapter-bochs` operations. See the [SoftPC device-source final baseline](etc/operations/m0-t276-s25-softpc-device-source-final-disposition-baseline-001.md).

**S25 P18:** the remaining 38 `softpc.new/host/src` control bodies have final
dispositions: 25 original monitor/BOP/session/host-capability providers are
`adapter-backed`; six mixed CPU/RAM/page/section control bodies are
`overlay-required`; and seven alternate-executor/profile/product-shell bodies
are `not-host-runtime`. No source is enabled by the audit; see the [SoftPC control-source final baseline](etc/operations/m0-t276-s25-softpc-control-source-final-disposition-baseline-001.md).

**S25 P19:** the initial WOW32 carrier split is final: all 105 declaration
carriers are `binding-only`, while eight condition-unselected source files and
eleven build/resource inputs are `not-host-runtime`. The remaining 77 active
original provider bodies stay grouped by their eight original subdomains for
the next package decision; see the [WOW32 carrier final baseline](etc/operations/m0-t276-s25-wow32-carrier-final-disposition-baseline-001.md).

**S25 P20:** all 77 active WOW32 provider bodies now have final dispositions:
61 private frame/message/GDI/kernel-session/USER bodies are
`overlay-required`, while 16 shell/OLE/printing, multimedia and network
providers are `adapter-backed`. The overlays retain original provider flow and
replace only private NT4 transport or unsafe identity conversion; see the [WOW32 provider final baseline](etc/operations/m0-t276-s25-wow32-provider-final-disposition-baseline-001.md).
