# Project Status

## Current Work

**Active: M0 T262 S3 P2 — OpenNT guest deployable and build-input binary
mirror.** This owner-directed expansion records the original DOS/V86 binary,
MAP/SYM and library inputs copied under `opennt-guest` before T262 proceeds to
its final cross-component closure.

## Active Packet

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
| Objective | Remove the unused project-authored `cpu/bx_ntvdm_exception_intercept.h` from `bx-core`, and make every retained modified Bochs file locally marked and individually indexed in `bx-core/README.md`. |
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
`cpu/bx_ntvdm_exception_intercept.h` is non-mirror. All 580 `opennt-guest`
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
| Scope | `opennt-host` header/source/include users, `adapter-softpc` mis-rooted exact header, `adapter-win32` direct-access UI state, BOP consumers, fixture-only controls, build manifest and local README divergence register. |
| Non-goals | No new BOP capability, no full historical CSR/VDD/ICA/SoftPC shell, no Bochs change, no unbounded pointer/handle state and no generic compatibility component. |
| Required Direction | A mirror may be a reachable original-file subset when the complete translation unit cannot compose. Each retained line/record/order remains original; every omitted/unavailable/replaced expression is statement-local `DIVERGENCE(<ID>)` and has one `opennt-host/README.md` row. A new declaration belongs only to its real adapter/session/test owner, never an OpenNT-mirror path. |
| Verification | Exact hash comparison for full imported headers; source-span/exception ledger for a subset; include and old-path negative scans; original ABI assertion for `VrTerminateNamedPipes(WORD DosPdb)`; focused BOP59/VDMREDIR fixtures; formal Ninja closure, documentation governance and `git diff --check`. |
| Exit Criteria | No `*_compat.h` remains below `src/opennt-host`; all consumers include original header paths or their actual non-original owner; original header/subset and every divergence are one-to-one registered; formal closure and focused tests pass or an explicitly owned non-host failure is reproduced. |
| Admission | Owner direction: “要求你增开一个S任务完成此事。” |

**S12 P1 completed:** exact OpenNT `error.h`, `vrputil.h`, `vrnmpipe.h` and
the newly reached `oemuni.h` now replace all three project-authored host
compatibility headers.  The BOP59 fixture passes after non-original dialog
state moved to `adapter-win32`.  A fresh `002` formal Ninja graph compiled,
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
| Scope | `adapter-bop` and `adapter-softpc` production sources, their public headers, build inputs and focused fixtures. |
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
`opennt-bop/dpmi`; the old `adapter-softpc` aliases and fixture-only XMEM
record have left production. The cached formal Ninja graph refresh rebuilt the
affected closure without recompiling `bx-core`; focused DEM and DPMI fixtures
pass. The pre-existing VDMREDIR mailslot fixture exit `8` was reproduced from
both pre- and post-move graphs and remains separately recorded as non-P4
behavior evidence.

**S9 P5a completed:** the legacy `50:xx`/`54:xx` host-transaction classifier
had one fixture consumer and no formal production-graph input. It now lives
entirely under that fixture's `tests/.../support/` directory, leaving
`adapter-softpc` without this selector/family knowledge. P5b remains to
isolate the separate generic-UD test controls in `adapter-bop`.

**S9 P5b completed:** generic-UD v2 context injection and the finite-run
UD-stop switch have left production. `adapter-bop` now has only its default
mechanical decline plus selector-blind accepted-outcome observations; the
context fixture uses an explicitly linked test-support object and the
cancellation fixture now supplies its own v1 callback. Its separate
`BX_NTVDM_CANCELLATION_TESTING` session-library variant is not falsely added
to the ordinary formal graph and remains a later test-governance item.

**S9 P5c completed:** the source-derived OpenNT INT 06 provider and its
`02h`/`06h` dispatcher now live in `opennt-bop`, leaving
`adapter-softpc` with typed RAM/port mechanics only. Its focused provider and
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
| Scope | Delete include/exclude-drive policy; use the original DEM source path in place of the project namespace; delete the misplaced DEM policy facade; migrate XMS machine mechanics to `adapter-softpc`; import reached VDMREDIR units; import `nt_error.c`; then delete `namespace`, `xms`, `redir`, and top-level shim files as their declared replacements take over. |
| Non-goals | No new BOP family/service, no Bochs semantic change, no new specialized or generic adapter, no virtual boot drive, and no recreation of an NT4 CSR/BaseSrv/CCPU product shell. |
| Reference Baseline | Closed T261/S7 session foundation; `refs/opennt/base/mvdm/dos/dem/`, `vdmredir/`, `xms.486/`, and `softpc.new/host/src/nt_error.c`; the S1/S2 host source-truth ledger. |
| Required Direction | Original BOP meaning stays in `opennt-bop`; original host VDMREDIR/`nt_error.c` bodies belong in re-rooted `opennt-host`; unavailable historical Win32 calls use `adapter-win32`; A20/IVT/RAM/SoftPC mechanics use `adapter-softpc`; neutral lifecycle uses `session`; `app` remains composition owner. |
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
are a selector-blind same-shaped `adapter-softpc` facade.  Fresh formal Ninja
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
| Non-goals | No BOP selector/provider implementation, no OpenNT host-source rewrite, no Win32 facade semantics, no SoftPC/CCPU or Bochs mechanics change, no guest change, and no preemptive `adapter-vdd`, `adapter-debugger`, `adapter-redir`, `adapter-wow`, generic `adapter-common`, `adapter-host`, or `compat` root. |
| Reference Baseline | T261/S5 closure `ab49a2d4`; current `app`, `adapter-bop`, `adapter-softpc`, `adapter-win32`, `opennt-bop`, and `opennt-host` session-facing sources; T260 mapping-manager boundary. |
| Files And ABI Surface | New `src/session/*`, precisely inventoried neutral state currently held by component composition/adapters, component public declarations, formal module manifest/generator, and focused session fixtures. |
| Applicable Rules | `session` has no product-component dependency and no BOP/DOS/WOW/VDD/Redirector/Win32/Bochs vocabulary; `app` creates and wires the instance; source-owned behavior stays in its original owner; cross-component data remains fixed-width or opaque. |
| Stop Conditions | A candidate extraction requires service/provider/guest interpretation, an OpenNT-body semantic edit, a raw Bochs object or Win32 handle in the public session ABI, an upward import of `app`, or a second mapping-manager implementation. Pause and record the owner conflict. |
| Verification | [S7 session-foundation evidence](etc/evidence/m0-t261-s7-session-foundation-001.md); former/new owner map, source-token scans, focused identity/teardown fixture, formal MSVC x64 Ninja native link, `git diff --check`, documentation inventory/governance verification. |
| Exit Criteria | Closed: `session` builds as a dependency-free ninth host-side module; `app` alone creates and wires the instance; cancellation has one neutral owner; the three adapter-softpc mapping implementations bind to a unique session identity and teardown through its lifecycle; existing source/machine semantics are unchanged; no specialized/generic adapter was introduced. |
| Original Owner Request | “非常好，请你治理好文档，以S6身份提交，我们让S7来实施session逻辑抽离。” |
| Similar-Issue Sweep | Session records, token/handle/event tables, mapping-manager instance lifecycle, app-owned composition callbacks, adapter callbacks, thread/child completion state, build manifest and fixtures. |

### M0 T261 S6 — closed: architecture contract-consistency governance

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S6`, Ordinary Mode with a single-person dual-role review. |
| Objective | Reconcile the newly admitted `session` component across current architecture, goal, coding and rule authorities without moving production code or claiming that `session` is already built. |
| Scope | Correct component cardinality and adapter terminology; distinguish ten source components from the current eight linkable modules plus the guest-image input; define `session` as lifecycle/identity owner and `adapter-softpc` as mapping-facade implementation owner; correct production/test-root terminology; and record remaining selector-aware observation/fixture residue as later cleanup rather than silently legitimizing it. |
| Non-goals | No `src/session` implementation, source move, BOP/provider change, Bochs change, adapter semantic migration, Ninja module addition, or test behavior change. |
| Verification | [S6 governance evidence](etc/evidence/m0-t261-s6-architecture-contract-consistency-001.md); authority cross-read, targeted terminology/dependency scan, documentation inventory regeneration, documentation-governance verification and `git diff --check`. |
| Exit Criteria | Architecture, goal, coding and rules agree on the ten-component target; S7 is the sole active implementation packet for `session`; no current build claim is inflated; and the follow-on cleanup has an explicit S owner. |
| Follow-on | S7 implements only dependency-free `session`; S8 performs the owner-directed OpenNT-host source replacement; the former adapter-root cleanup is deferred to S9. |

### M0 T261 S5 — closed: adapter-bop generic ingress ownership split

| Field | Record |
| --- | --- |
| Identifier Mode | `M0 T261 S5`, Ordinary Mode with a single-person dual-role review. |
| Admission And Approval | The owner explicitly authorized S5: “好的 接下来S5开工？”.  T261’s approved proposal admits this exact split after S4 closure. |
| Objective | Establish `adapter-bop` as the sole selector-blind fixed-width generic-UD ingress/completion owner; retain every BOP selector/service recognition, provider dispatch and BOP-aware observation in `opennt-bop`. |
| Non-goals | No new BOP enablement, no provider semantic change, no new CPU-frame ABI, no Bochs change, and no migration of family/session/provider logic into `adapter-bop`. |
| Reference Baseline | T261/S4 P7 closure `694cf087`; existing `adapter-softpc/bx_ntvdm_generic_ud_bridge.h`; `opennt-bop/ingress/dem_v2_composition_bridge.c`; T260 formal component manifest and architecture/coding authorities. |
| Files And ABI Surface | New `src/adapter-bop/*`; the generic-UD external bridge symbol; exported `opennt-bop` route callback; app session-composition bind/reset calls; manifest/generator module and link order; focused bridge fixture. |
| Applicable Rules | `adapter-bop` is selector/service/family/DOS/WOW/provider blind; OpenNT route semantics remain in `opennt-bop`; `app` owns wiring; `adapter-softpc` remains the only SoftPC/CCPU-to-Bochs boundary; fixed-width copied data only. |
| Verification | Function-level owner map; selector-token negative scan of `adapter-bop`; focused bind/decline/dispatch/reset fixture; formal Ninja closure; component-direction scan; `git diff --check`; documentation governance. |
| Expected Markers | No selector/service literal or provider include in `adapter-bop`; no exported CPU generic-UD bridge implementation in `opennt-bop`; an unbound bridge declines, a bound route receives the unchanged copied event/outcome, and reset removes the binding. |
| Asset Needs | Existing fixed-width generic-UD ABI, current route entry, app engine lifecycle, formal Ninja manifest/generator and focused C fixtures. |
| Reporting Requirements | Exact function ownership map; no-new-ABI proof; retained route/observation source; binding lifetime; source/list/link changes; focused and formal results. |
| Stop Conditions | A proposed adapter file reads a selector/service/window payload for meaning, names a provider/family/DOS/WOW concept, requires a raw guest/Bochs object pointer, or forces a provider semantic change. Pause and record the boundary conflict. |
| Exit Criteria | `adapter-bop` owns the externally called generic ingress symbol and only generic registration/validation/forwarding; `opennt-bop` owns one exported route callback and all selector-aware logic; `app` binds/unbinds around one engine session; formal graph names eight linkable libraries and passes stated focused checks. |
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
| Files And ABI Surface | `opennt-host/dem/*`, direct OpenNT `dem*.c` callers in `opennt-bop/dem`, their host file/directory/error/handle declaration seams, and their adapter-win32/adapter-softpc includes. |
| Applicable Rules | Original source and call shape first; every imported-body edit has `DIVERGENCE`; adapter-win32 owns historical public API facades; adapter-softpc owns mapping/mechanics; no component may depend upward on BOP routing. |
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
facade to `adapter-win32`; both moved objects compile in fresh `r002`. S3 will
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
composition to `adapter-win32` without a reverse BOP dependency.  Fresh
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
source-visible host APIs, BOP composition, adapter-softpc mechanics and
unavailable VDD/FDC paths, and fixes the dependency order before any source
move; see the [DEM boundary map](etc/evidence/m0-t261-s4-p1-dem-host-boundary-map-001.md).
S4 P2 has split the independent DEM directory group: its source-body invoke
now belongs to `opennt-bop`, same-named OEM directory calls belong to
`adapter-win32`, and fresh r010 formal build plus the direct-import fixture
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
`adapter-softpc`; a generic post-body hook removes its former direct demerror
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
selector in `adapter-bop` or change the already-closed DEM host boundary.
S5 P1 is closed in `ab49a2d4`: `adapter-bop` owns the generic CPU entry and
the unchanged copied record; `opennt-bop` retains the renamed selector route;
and `app` owns bind/reset around one engine run.  The actual formal r002 native
link and focused ingress fixture pass; see
[S5 ingress evidence](etc/evidence/m0-t261-s5-adapter-bop-ingress-split-001.md).

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
| S3 | `bx-core` confirmation plus `bx-mantle` purification: retain Bochs-only code and assign every foreign-semantics file to its target owner. |
| S4 | `app` and `opennt-guest` migration. |
| S5 | `opennt-bop` mirror, ingress and route migration. |
| S6 | `opennt-host` and `adapter-win32` classification/split migration. |
| S7 | `adapter-softpc` same-shaped SoftPC/CCPU boundary and shared-mapping-manager migration. |
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

S7 established `adapter-softpc` as the unique same-shaped mechanical facade,
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
`bin/ntdos64-native.exe`; final actual incremental and dry-run invocations
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
`opennt-host`; restrict mantle to Bochs-only mechanics; and declare each future
library/public-include boundary. No product source or formal Ninja input has
moved.

S3 has moved all 42 former `bx-mantle/bx_ntvdm_*` bridge files to
`adapter-softpc` with Git history, repaired product include ownership, and
made the remaining mantle source scan free of external product semantics.
The local bx-core exception index now points to every active exception family.
The one coherent formal Ninja/fixture path switch remains S8 work; see the
[S3 evidence](etc/evidence/m0-t260-s3-mantle-purification-001.md).

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
| Objective | Recover each reached original OpenNT/SoftPC interface shape and its observable semantics through the smallest source-shaped compatibility facade or direct public API, while independently minimizing edits to OpenNT mirrors and `bx-core`/`bx-mantle`. |
| Non-goals | No BOP/provider enablement; no wholesale CCPU/SAS/CSR/BaseSrv product-shell recreation; no Bochs semantic intrusion; no invented behavior without source evidence. A facade is not permission to substitute a convenient new ABI. |
| Reference Baseline | `MACH-01..09` in the T259 catalog; OpenNT `softpc.new/host/src/{nt_cpu.c,nt_cprgs.c,nt_aorc.c}`, `keybd_io.c`; active SoftPC/DEM/XMS source mirrors; T258 mapping manager. |
| Files And ABI Surface | The source-shaped SoftPC/CCPU facade contract in `bx-vdm`; affected `host_simulate`, register-frame, checked-memory, A20, IVT/keyboard and typed machine-action seams. |
| Applicable Rules | Source-first recovery, public-Win32/x64 decision rule, guest/machine hard boundary, one shared mapping manager, and no family-private product shell. |
| Verification | Original call-site and fixed-width ABI audit; focused seam fixture; negative selector/DOS/CCPU boundary scan of bx-core/bx-mantle; formal Ninja build/link; documentation gate and independent review. |
| Expected Markers | Reached historical spelling remains at the mirror boundary; `bx-vdm` performs copied-state translation; bx-core/mantle remain selector/DOS/CCPU blind; no CCPU loop or host pointer crosses the boundary. |
| Asset Needs | OpenNT SoftPC/CCPU source and active mirrors, current bx-vdm/mantle API, T258 mapping manager, catalog, exception register and formal build manifest. |
| Reporting Requirements | Record original interface spelling and call site, calling convention/width/lifetime, typed mechanical request/result, no-op/failure disposition and any exception ID. |
| Stop Conditions | A helper exposes host pointers to the guest, changes imported OpenNT control flow, recreates a CCPU/product shell, imports BOP/DOS/SoftPC terms into bx-core/mantle, or lacks a source/ABI evidence entry. |
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
decline; imported OpenNT mirrors and bx-core/mantle remain unmodified. See
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
DPMI package semantics, not a selector-blind mantle lifecycle. Native Bochs
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
COMMAND body returns Direct-host `O:\repos.hobby\ntdos64`, then source-built
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
  compiles the explicit SIM/mantle/memory and source-proven reset/decode CPU
  candidates, then stops at the original complete instruction-handler table
  and retained product-shell methods.  No GUI, plugin/device archive, adapter
  or OpenNT input entered the probe; see
  [T197 S6 link probe 001](etc/research/t197-s6-cpu5-minimal-link-probe-001.md).
- M0 T197 S6 r11 now source-builds and links the whole CPU5 adopted core with
  the finite mantle, without a Bochs product target.  This is a native link
  closure only; machine initialization/execution still requires separate
  lifecycle verification.
- M0 T197 S6 r12 verifies the native bare-machine lifecycle in process:
  `initialize(1 MiB, 1 MiB) -> CPU5 reset/APIC/CPUID -> cleanup()` returns
  zero.  No instruction-loop, BIOS/reset image, BOP or guest-running claim is
  made by that lifecycle witness.
- M0 T198 S1 finds the retained BOP runtime is a historical product derivative:
  it retains `main.cc`, GUI/device archives and product configuration.  It is
  not link-compatible with the finite mantle.  The first second-stage closure
  is therefore a selector-blind native execution contract, not a per-BOP
  implementation; see
  [T198 S1 composition map](etc/research/t198-s1-bop-runtime-composition-map-001.md).
- M0 T198 S2 is complete: native CPU and PC-time sources prove a
  mantle-private reset/copy/entry/run contract.  Its bound is a finite
  instruction-tick budget rather than an exact retired-instruction counter;
  the existing product `benchmarkTimer` is rejected because it also mutates
  GUI exit state.  See
  [T198 S2 execution-contract map](etc/research/t198-s2-native-execution-contract-map-001.md).
- M0 T198 S3 is complete: r8 source-builds and runs neutral `HLT` bytes
  through CPU5 reset, checked RAM copy, real-mode entry, 64-tick native stop
  and cleanup.  The needed A20 initialization is now a finite mantle
  lifecycle invariant; no guest/BOP runtime claim follows.  See
  [T198 S3 finite-run witness](etc/research/t198-s3-finite-native-run-witness-001.md).
- M0 T198 S4 is complete: the current `#UD` seam map separates retained
  copied-state/resume mechanics from legacy `bx-vdm` dispatch, raw callback,
  environment and product-lifecycle coupling.  S5 will extract one fixed
  core/mantle bridge before any BOP plane attachment; see
  [T198 S4 generic #UD seam map](etc/research/t198-s4-generic-ud-seam-map-001.md).
- M0 T198 S5 P1 is complete: mantle now owns a standalone fixed generic-UD
  copied record and default-decline bridge, independently MSVC/x86 compiled
  with no adapter/product vocabulary or dependency.  Core has not yet been
  switched; see
  [T198 S5 P1 bridge record](etc/research/t198-s5-p1-generic-ud-bridge-record-001.md).
- M0 T198 S5 P2 is complete: a new default-off core gate source-builds under
  CPU5/MSVC and invokes only the fixed mantle record/typed outcome path; its
  stop uses the finite PC stop latch.  Historical parallel gates remain for
  P3 removal and are not part of the new composition; see
  [T198 S5 P2 core hook](etc/research/t198-s5-p2-core-mantle-ud-hook-001.md).
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
  corrected into a mantle-owned, selector-blind copied ordinary-RAM action
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
  core/mantle ABI or semantic addition is indicated. See [T198 S11 P1 map](etc/research/t198-s11-p1-synchronous-composition-map-001.md).
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
  finite mantle source-build, link and run as one MSVC x64 `/MT` PE32+ process
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
  a new selector-blind mantle request, not a startup BOP or Bochs semantic;
  entry changes only CS:IP and stops at the already-admitted `50:11` boundary.
  See [S21 pre-entry map](etc/research/t198-s21-ntio-preentry-contract-map-001.md).
- M0 T198 S22 closes the selector-blind pre-entry primitive: a versioned,
  copied v2 request preflights a one-MiB aperture, captures/restores opaque
  bytes across image publication, and then applies CS:IP.  The full CPU5
  MSVC x64 `/MT` fixture proves restoration and rejection before mutation; see
  [S22 witness](etc/research/t198-s22-mantle-preentry-witness-001.md).
- M0 T198 S23 P1 source-builds the `bx-vdm` NTIO request builder under MSVC
  x64 `/MT`: it copies only the exact `0x8400` image into mantle v2 and
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
- M0 T198 S24 P3 admits and verifies a default-off mantle-private terminal
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
- M0 T198 S31 adds a one-shot, opaque, checked mantle terminal-RAM snapshot.
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
  be a separately admitted fixed-width core-to-mantle mechanic, never a BOP or
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
- M0 T198 S38 is complete: the registered, default-off mantle-only query
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
  mantle mechanical action. The old all-`FF` `0A84` frontier is gone; the new
  reproducible `0000:0047` frontier follows the still-unexplained
  `9346:4D53 -> 0000:0000` transfer. See
  [S40 closure](etc/research/t198-s40-preentry-input-closure-001.md).
- M0 T198 S41 is complete: the registered copied position record proves that
  the unexpected CS transition changes SS:SP from `00A7:0918` to
  `00A7:090E`. The frame owner remains unproven. See
  [S41 witness](etc/research/t198-s41-stack-position-witness-001.md).
- M0 T198 S42 is complete: the registered bounded mantle provenance record
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
