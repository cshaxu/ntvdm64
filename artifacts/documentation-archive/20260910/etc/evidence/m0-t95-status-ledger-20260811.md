# Project Status

## Current Work

**Active: M0 T95 S6 — MSVC/Win32 native minimal-target closure decision.**

**Standalone governance: M0 Td S1.** Restore the M/T/S/P identifier,
STATUS/QUEUE, gate, and execution-mode rules. It is documentation-only and
does not allocate, suspend, or close T95.

## Historical Packet: M0 T95 S1

| Field | Record |
| --- | --- |
| Identifier mode | Existing implementation package: `M0 T95 S1`, Ordinary Mode. `M0 Td S1` is the concurrent standalone governance record. |
| Historical run mapping | `r1`--`r23` are pre-governance build/fixture/evidence iterations within `M0 T95 S1`; none is a T, S, or backfilled P. Future accepted commits use `M0 T95 S<n> P<n>` and link their r evidence. |
| Admission and owner approval | T95 is the existing Phase 0 backend-admission package. The owner requested the Td governance correction on 2026-08-10. |
| Objective | Replace the unavailable OpenNT x86 V86/CCPU execution composition with a pinned Bochs 2.6 guest-machine backend while preserving OpenNT as the guest/service owner. |
| Scope | Manifest-backed import from `O:\repos.external\bochs-2.6-compat\bochs-2.6`; retain Bochs's native `SIM`/init/exception lifecycle as an intact `nogui` bare-machine container; derive its build closure and native reset/termination fixture; then design the Bochs/OpenNT hard-boundary contract and focused fixtures. |
| Non-goals | Recreating the NT4 V86/CCPU product composition; importing a Bochs GUI/plugin tree; broad device emulation; DOS/BOP/DEM/WOW reimplementation; `ntvdm64` integration. |
| Reference baseline | Current Bochs 2.6 Phase 0 records and the retained `ntvdm64` execution-governance model. |
| Files and ABI surface | Bochs import/configuration, adapter C ABI, runner handoff, focused fixtures, and their governance/evidence records; no public product ABI is admitted. |
| Current evidence | `etc/research/bochs-26-backend-adoption.md`, `etc/research/bochs-26-minimum-mechanics-closure.md` (micro-M0 rejection evidence), `etc/research/bochs-26-native-bare-machine-container.md`, `etc/research/adapter-external-intrusion-exceptions.md`, `etc/research/bx-ntvdm-adapter-exception-abi-v1.md`, `etc/research/bx-ntvdm-cpu-state-abi-v1.md`, `etc/research/bx-ntvdm-guest-write-abi-v1.md`, `etc/research/bx-ntvdm-instruction-window-abi-v1.md`, `etc/research/adapter-bios-selector-map-v1.md`, `etc/research/bochs-ud-instruction-window-seam.md`, `etc/research/opennt-bop-entry-contract.md`, `artifacts/analysis/m0/`, `artifacts/build/bochs-2.6-native-nogui-ucrt-r3/{configure,build}.log`, the local Bochs 2.6 `CHANGES` release record, and the retained OpenNT source/guest closure ledgers. |
| Applicable rules | `rules/EXECUTION.md`, `rules/ARCHITECTURE.md`, `rules/CODING.md`, `design/ARCHITECTURE.md`, and `etc/operations/policy/source-policy.md`. |
| Verification | Verify imported source identity; review the M0 symbol-to-object closure against the rejected set; retain the declared UCRT native-container configuration and its r3/r4/r9 build evidence; use the native reset/bounded-stop fixture; then run adapter tests proving no pointer/object crossing and no adapter-owned DOS/BOP/DEM policy. |
| Expected markers | Pinned Bochs 2.6 identity, native lifecycle fixture evidence, fixed-width bridge records, and explicit rejection of pointer/object crossing and adapter-owned guest policy. |
| Asset needs | The approved local Bochs 2.6 source input and retained OpenNT/BYOB evidence only. |
| Risks | Accidental 3.0 import; mistaking the historical full executable graph for a minimum; Bochs feature creep; leakage of OpenNT semantics into the adapter; false claim that a machine backend proves NTVDM runtime. |
| Stop conditions | The rejected micro-M0 triple-fault issue is resolved by retaining the native Bochs lifecycle. Stop if the exact local 2.6 import cannot be identity-verified, a native container feature cannot be disabled through original Bochs configuration/build structure, the first bridge requires invented OpenNT service behavior, or progress requires changing/replacing Bochs CPU decode, instruction semantics, memory subsystem, generic device framework, or firmware behavior. The owner has admitted only adapter recovery of the exact `0x50 -> MS_bop_0` missing selection record; the unavailable full-`BIOS[]` generic-`Video` composition remains closed. See `etc/research/bochs-first-opennt-owner-closure-decision.md` and `etc/research/adapter-bios-selector-map-v1.md`. |
| Exit criteria | `refs/bochs` is a pinned 2.6 import with source/patch record; M0 has a reviewed symbol-to-object closure and a declared build target; adapter negative tests pass; the first original OpenNT-owner bridge contract is documented. No DOS runtime claim is required. |
| Original owner request | Adopt and govern a narrow Bochs-backed OpenNT reconstruction path without expanding into a generic emulator or external-runtime integration. |
| Similar-issue sweep | For any architecture or boundary repair, inspect all active backend imports, bridge records, and runner handoffs; Td work is documentation-only and records this governance-wide sweep instead of a defect-class code sweep. |

## Evidence-Complete Packet: M0 T95 S2 (P Pending)

| Field | Record |
| --- | --- |
| Identifier mode | `M0 T95 S2`, Ordinary Mode. The preceding `M0 T95 S1` packet and r1--r23 records remain historical evidence; no P is backfilled. |
| Admission and owner approval | The owner approved `--include-drives` and `--exclude-drives`, exclusion precedence, and all logical drives eligible by default. `docs/QUEUE.md` reserves this as the first post-S1 package. |
| Objective | Install one immutable adapter-local snapshot of the real host logical-drive inventory at adapter-session creation, after applying the CLI policy, without exposing enumeration to Bochs or to guest requests. |
| Scope | `src/cli` child-only handoff supplies normalized policy. S2 adds adapter parsing, one Win32 logical-drive observation at session install, immutable copied snapshot access for later adapter capabilities, deterministic unit tests, and lifecycle regression evidence. |
| Non-goals | DOS search, directory/file enumeration, path opening, drive mapping, guest-visible DPBs, DEM behavior changes, Bochs source changes, and any guest-triggered rescan. Static BYOB profile `host_drive_inventory` remains separate. |
| Reference baseline | `docs/design/HOST-NAMESPACE-CAPABILITY.md`, `docs/QUEUE.md`, existing CLI policy handoff, and installed adapter runtime lifecycle. |
| Files and ABI surface | Adapter-local `bx_ntvdm_host_drive_policy` C11 API and runtime copy accessor only. No Bochs ABI, guest ABI, BOP selector, or CLI public argument change. |
| Applicable rules | `docs/rules/EXECUTION.md`, `docs/rules/ARCHITECTURE.md`, `docs/rules/CODING.md`, `docs/etc/operations/task-identifier-governance.md`, and `docs/design/HOST-NAMESPACE-CAPABILITY.md`. |
| Verification | Build and execute policy tests for include-only, exclude-only, conflict/preference, default selection, malformed input, and missing observed type; execute runtime regression proving an installed snapshot survives caller-side copy mutation. |
| Expected markers | `bx-ntvdm-host-drive-policy-test: deterministic include/exclude snapshot policy verified`; `bx-ntvdm-adapter-runtime-test: identity-to-pending-transaction path verified`. |
| Asset needs | Local Windows logical-drive APIs and existing BYOB profile test assets only; no external source or device import. |
| Stop conditions | Stop if this requires a Bochs change, guest-visible drive result, host-path authority, a second enumeration during a guest request, or redefinition of existing DEM/profile semantics. |
| Exit criteria | Policy is applied once per adapter session, exclusion has priority, default admits all observed drives, snapshot is immutable/session-local, tests pass, and separation from static profile inventory is recorded. |
| Original owner request | `--include-drives=c,d,e` / `--exclude-drives=e`; exclusions win; no supplied flags means all drives are passed to the adapter. |
| Similar-issue sweep | Reviewed CLI handoff, adapter runtime, profile inventory, DEM drive service, Bochs boundary, and namespace design. No service or Bochs expansion belongs in S2. |

## Evidence-Complete Packet: M0 T95 S3 (P Pending)

| Field | Record |
| --- | --- |
| Identifier mode | `M0 T95 S3`, Ordinary Mode. It has reproducible source-contract evidence and awaits its P lifecycle. |
| Admission and owner approval | The owner authorized a holistic, adapter-owned host-capability route and explicitly rejected one-off DEM/BOP hacks. The queue admits the paired path/FCB search family as one source-closure package. |
| Objective | Produce the exact source-to-guest-ABI contract for `SVC_DEMFINDFIRST`, `SVC_DEMFINDFIRSTFCB`, `SVC_DEMFINDNEXT`, and `SVC_DEMFINDNEXTFCB`, including shared continuation state and all implementation gates. |
| Scope | OpenNT `demsrch.c`, `dosdef.h`, `demexp.h`, `mvdm.h`, service IDs, and NTDOS `search.asm` staging/copy behavior. The result is a design/evidence record only. |
| Non-goals | Host directory enumeration, wildcard engine, file I/O, token-table code, generic read ABI changes, guest writes, runtime trace, or any Bochs change. |
| Reference baseline | `docs/etc/research/r23-fcb-search-capability-boundary.md`, `docs/design/HOST-NAMESPACE-CAPABILITY.md`, S2 frozen drive snapshot, and original OpenNT/NTDOS sources. |
| Files and ABI surface | Documentation only. It specifies the future adapter transaction boundary but introduces no new runtime ABI or source object. |
| Applicable rules | `docs/rules/EXECUTION.md`, `docs/rules/ARCHITECTURE.md`, `docs/rules/CODING.md`, and the T95 hard-boundary/adapter rules. |
| Verification | Re-read source call sites, derive byte offsets from 16-bit-compatible fields, reconcile them with declared NTDOS buffer sizes, and record source contradictions/unresolved points rather than silently resolving them. |
| Expected markers | A 43-byte path DTA layout, a 53-byte FCB staging allocation with its 52-byte known semantic prefix, the four-service lifecycle, and the 64-byte generic-read insufficiency. |
| Asset needs | Existing OpenNT/NTDOS source tree only. |
| Stop conditions | Stop if contract closure would require guessing host pointers, reading unbounded guest strings, treating BYOB root as a host namespace, or claiming that a source table is a runnable filesystem. |
| Exit criteria | A reviewed contract names every service input/output/state transition, marks each fact or unresolved observation, specifies cookie safety and errors, and lists S4/S5 gates without authorizing their code. |
| Original owner request | Preserve NTVDM semantics in OpenNT/adapter boundaries, make the Bochs boundary minimal, and avoid incremental reconstruction disguised as isolated handlers. |
| Similar-issue sweep | Reviewed FCB and pathname families together, DTA registration, DOS staging buffers, host-drive snapshot, existing 64-byte generic reader, and Bochs isolation. |

## Active Packet: M0 T95 S5

| Field | Record |
| --- | --- |
| Identifier mode | `M0 T95 S5`, Ordinary Mode. It is the sole active subtask; S2--S4 retain evidence-complete records awaiting their P lifecycle. |
| Objective | Materialize one paired, adapter-owned DOS search capability for pathname and FCB first/next, using S2's frozen drives and S4's read-only namespace while retaining the S3 source contract. |
| Completed increments | The one-range generic read is bounded at 128 bytes; a separately versioned, semantics-free gather-read record is now tested for up to four validated ranges / 256 total bytes. The adapter session binds a read to an internal completion owner. A private snapshot/cookie table validates pair and PDB once supplied by the future gather completion. A source-shaped request decoder accepts only bounded explicit-drive 8.3 OpenBuf input and the documented DTA/FCB continuation slots. A source-shaped atomic DTA/FCB success encoder is unit-tested; FCB byte 52 remains untouched. Pathname no-more now atomically clears DTA tokens and returns source-defined AX/CF; FCB error behavior remains closed. One adapter transaction core now composes raw source bytes, namespace/session lookup, success output and pathname no-more without selector/runtime access. The generic gather seam is source-connected before the legacy one-range consumer and has a passing semantic-boundary check, but no native fixture claim. See `docs/etc/research/t95-s5-guest-read-transport.md` and `docs/etc/research/t95-s5-gather-read-admission.md`. |
| Present hard boundary | Bochs sees only generic read/write/result mechanics. The consumer label is adapter-local; OpenNT service semantics and host paths do not cross into Bochs. No search selector is dispatched yet. |
| Next admission gate | A same-version native gather fixture must compile/link before a search service may consume copied guest bytes. The first mechanics prerequisite is an admitted MSVC recipe for the already declared minimum fixture: the imported tree exposes only its historical full VS2008 `bochs.exe` solution, while r24 UCRT evidence is not an accepted Bochs island. The remaining adapter gates are the source-defined FCB no-match behavior, volume labels, and current-directory ownership. Do not choose them without authoritative runtime evidence. |
| Verification so far | `bx-ntvdm-guest-read-action-v1-test`, `bx-ntvdm-guest-gather-read-action-v1-test`, `bx-ntvdm-host-session-test`, and `bx-ntvdm-adapter-runtime-test` rebuilt and passed on 2026-08-10. r24 configuration completed, but its `AC_CHECK_SIZEOF(int *)` link hit an external `conftest.exe` permission-denied reopen and wrote `SIZEOF_INT_P=0`; its one corrected declared-fixture build then reached `iodev/devices.cc` and stopped at that generated `config.h` error, before any adapter/gather source compiled. This is retained external toolchain/process evidence and is not repaired or retried in r24. |
| Exit criteria | Both forms have first/next, no-match, exhaustion, invalid/stale cookie, and concurrent-session evidence; no host pointer reaches guest memory; all unresolved source gates are explicitly resolved or fail-closed for the profile. |

## Evidence-Complete Packet: M0 T95 S6 (P Pending)

| Field | Record |
| --- | --- |
| Identifier mode | `M0 T95 S6`, Ordinary Mode. The owner reported that the former `executor` session has stopped; `/root` completed the admission, evidence review and the S6 decision. S5 remains evidence-complete only through its native-fixture gate and may not dispatch a search selector. |
| Admission and owner approval | The owner approved the MSVC/x86 direction on 2026-08-10 after the UCRT r24 failure was retained as external configuration evidence. This admits closure analysis and a documented recipe decision only; it does not admit `bochs.exe`, `all`, a configure/config.h patch, device enablement, or OpenNT semantics. |
| Objective | Establish whether an MSVC Win32 invocation can express the already-declared Bochs 2.6 minimum native observation fixture without inheriting the historical full-product VS2008 configuration. |
| Scope | Audit the imported VS2008 project/configuration metadata, correlate it with the M0 source-to-object ledger, record required/default/rejected inputs, and decide whether the missing configuration and object closure are stable enough for a separate minimal build recipe. |
| Non-goals | Compiling or repairing full `bochs.exe`; invoking the VS2008 solution; changing Bochs source, generated configuration, CPU semantics, SIM/CMOS behavior, firmware, device policy, or adapter/OpenNT semantics; producing a runnable search service. |
| Evidence | `docs/etc/research/t95-s6-msvc-x86-nonproduct-compiler-probe.md` records a standalone `/EP` + `/Zs` x86 observation: `_MSC_VER=1943`, `_WIN32=1`, compiler `WIN32=1`, x86 widths, and selected UCRT declarations. It proves no `BX_HAVE_*` semantic value or Bochs GUI configuration. The prior S6 evidence and boundary tests remain unchanged. |
| Verification | Reproduce the metadata audit without invoking a build. If and only if the object/configuration ledger becomes stable, record one exact x86 command, artifact path/hash plan, CRT model, C ABI direction, and negative feature checks before one fresh-root fixture invocation. |
| Risks | Treating the full VS2008 `config.h` as minimal would silently enable PCI, USB, networking, sound, VGA, GUI/configuration support and other denied features. Treating a partial CPU object list as a minimum would reimplement Bochs execution. |
| Stop conditions | Stop with an explicit unavailable/rejected conclusion if the minimum configuration requires an unbounded SIM/product-support graph, a Bochs semantic patch beyond registered exceptions, or an unproven object closure. Do not substitute UCRT evidence for the required Bochs toolchain island. |
| Exit criteria | Met. The source-to-object/link and configuration evidence classifies the MSVC route as available through the intact native tuple/container, while rejecting configuration reconstruction and full-product expansion. The next allowed action is the separately admitted S7 guest-placement/read-only-file capability; S5's generic seam remains unclaimed. |

### S6 Native-Container Manifest Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Freeze the inputs and rejection predicates for one future MSVC/x86 native-container observation without creating a build graph or invoking a compiler, linker, or executable. |
| Scope | Add one PowerShell manifest generator under `tools/` and run it once into a new `artifacts/analysis/` root. It may hash the imported Bochs source, the existing CPU3 projection, original MSVC metadata, and test ROM inputs; it writes only a declarative profile and JSON manifest. |
| Non-goals | No source copy, configuration projection, compile, link, archive, `bochs.exe`, VS solution, adapter/CLI/OpenNT object, firmware execution, or guest-runtime claim. The historical UCRT container tool is not an input to this observation. |
| Required evidence | Fresh-root refusal on collision; immutable `refs/bochs/config.h` identity; CPU3 projection identity; original Makefile link-template identity; original executable project identity; x86 MSVC/`/MT` declaration; profile hash; explicit adapter/CLI/OpenNT exclusion; and native reset/controlled-stop predicates. |
| Verification | Inspect the emitted JSON/profile, verify all referenced inputs/hashes and absence of a build command, then run `tools/governance/Verify-DocumentationGovernance.ps1`. |
| Risks and stop conditions | Stop before any build if the projection is missing, inputs differ from the admitted hashes, a profile adds an unapproved device/feature, or the manifest would imply object pruning or adapter composition. A manifest does not itself authorize the future build. |
| Exit criteria | A reproducible manifest records the only candidate inputs and leaves a single later decision: whether those inputs justify one fresh-root MSVC container build observation. |

### S6 Fresh-Root Build-Shim Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Define a reproducible, non-product `nmake` target which preserves the original Bochs lifecycle and link membership while producing a separately named native-container artifact in a copied root. |
| Scope | Add a generator under `tools/`. It may copy the pinned Bochs tree to one fresh build root, verify the prior manifest hashes, replace only that copied root's `config.h` with the accepted v2 CPU3 projection, materialize the two declared ROM inputs/profile, and write a root-local `ntdos64-native-container.mak` whose prerequisites and link recipe are mechanically derived from the original `bochs.exe` target. It may perform one `nmake /n` dry-run only after static review. |
| Non-goals | No modification of `refs/bochs`, `Makefile`, `Makefile.in`, VS projects, Bochs source, adapter, CLI, or OpenNT. No configure, compile, archive, link, executable, firmware, guest, BOP, DEM, DOS/WOW, search, or CLI runtime action. The shim must not use `all`, invoke a VS solution, or create/copy `bochs.exe`. |
| Required evidence | Fresh-root collision refusal; source and projection hash checks; only copied-root `config.h` replacement; byte-for-byte copied ROM/prospective profile identity; source-derived original prerequisites/link order; a named `ntdos64-native-container.exe` target; and `nmake /n` output containing no `all`, `bochs.exe`, adapter, CLI, or OpenNT path. |
| Verification | Static parse/line comparison of the shim against the original `Makefile` target, then one dry-run only. Preserve emitted recipe/log as evidence and run documentation governance verification. |
| Risks and stop conditions | Stop if the original target cannot be mechanically transformed without changing its source membership/link inputs, if any generated command admits a denied component, or if NMAKE dry-run attempts a non-dry action. A future actual target invocation requires a new admission after dry-run review. |
| Exit criteria | A fresh root contains the intact original source/build graph, the accepted copied-root projection, exactly one separately named target, and dry-run evidence proving its intended command surface without claiming any build result. |

**Result (2026-08-11):** The admitted fresh root is
`artifacts/build/bochs-2.6-native-nogui-msvc-cpu3-r1`.  Its copied-root
configuration hash is accepted v2 CPU3
`C47030F79880C894A346F56E3AB3242924308983E00B5845EDCE849450C6C19D`.
`ntdos64-native-container.mak` was statically equal to a target-name-only
transform of the original `bochs.exe` recipe.  One `nmake /n` produced the
original archive subtargets, root lifecycle commands and final separately named
link line; it created no object or target artifact.  See
`etc/research/t95-s6-msvc-native-container-build-shim-001.md`.  An actual
`nmake` invocation remains separately unadmitted.

### S6 First Native-Container Build Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Obtain exactly one MSVC/x86 compile/archive/link observation of the now-defined `ntdos64-native-container.exe` target, classify the first actual boundary, and retain complete logs. |
| Scope | In the existing fresh root only, call the recorded BuildTools x86 environment and exactly `nmake /f ntdos64-native-container.mak ntdos64-native-container.exe`. Capture stdout/stderr and exit code. Read-only inspection of outputs, tool versions and link/map artifacts is allowed only after the call returns. |
| Non-goals | No retry, flag change, configuration/projection/source/shim edit, `all`, VS solution, `bochs.exe`, configure, target execution, reset, firmware, guest, adapter, CLI, OpenNT, DOS/WOW/BOP/DEM action. No successful executable is a runtime claim. |
| Required evidence | Exact command, x86 tool identities, copied-root config and shim hashes, full build log, exit code, first terminal compiler/archive/link diagnostic (or target hash if it reaches link), and a classification against the declared native-container feature manifest. |
| Verification | Verify pre-call hashes and absence of target output; after completion preserve log, inspect only generated artifact/map identities, confirm that no excluded path appears in the final link input, and run documentation governance verification. |
| Risks and stop conditions | Stop permanently after this one invocation regardless of result. If it builds an excluded `bochs.exe`, reaches `all`, requests adapter/CLI/OpenNT input, or changes any input, reject the observation and do not repair it. A compiler/archive/link failure is evidence, not authorization for another attempt. |
| Exit criteria | One immutable observation is classified as unavailable/external/toolchain-blocked, source/configuration-blocked, link-blocked, or target-built-without-runtime-claim, with the next allowed decision stated explicitly. |

**Result (2026-08-11):** The single invocation exited `1` in the original
`iodev` submake at `gameport.cc`, before archive, link, target executable or
runtime.  Accepted v2 config denies `BX_SUPPORT_GAMEPORT`, while the copied
generated `iodev/Makefile` unconditionally retains `gameport.o` in the
non-plugin object list.  This is classified **source/configuration
build-closure blocked**, not a toolchain or adapter failure.  The preceding
device objects and complete log are retained; no retry occurred.  See
`etc/research/t95-s6-msvc-native-container-build-observation-001.md`.  The
only next analysis is a full original-configuration-to-Makefile substitution
manifest; enabling gameport or hand-pruning objects is not admitted.

**Follow-up audit (2026-08-11):** Original `Makefile.in` inputs contain 105
distinct configure substitutions, all materialized by the retained historical
`config.status`.  They include the complete optional-device/plugin split and
platform link inputs, not only compiler flags.  The next configuration must
therefore be one original configure-derived tuple (`config.h`, all generated
Makefiles and `config.status`), never a macro or per-object overlay.  See
`etc/research/t95-s6-msvc-configure-substitution-audit-001.md`.  No new
configuration or build is admitted by this audit.

**Native configuration reconciliation (2026-08-11):** The CPU3 `config.h`
projection route is now rejected as an implementation route, because rebuilding
its matching generated Makefile family would recreate Bochs configuration
machinery.  The selected route retains the original matching MSVC
`config.h`/`config.status`/Makefile tuple and puts all minimization in the
Bochs runtime profile (`nogui`, declared machine inputs and existing
`plugin_ctrl` denial).  This retains compiled support without declaring it as
guest capability and avoids enabling devices merely to satisfy a build.  See
`etc/research/t95-s6-native-config-reconciliation-001.md`.  A new intact
fresh-root manifest/target admission is required before any further build.

### S6 Intact-Native-Tuple Manifest Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Freeze the matching imported MSVC configuration tuple and the minimum runtime profile without applying a source or configuration overlay. |
| Scope | Add one manifest-only generator under `tools/` and invoke it once into a fresh `artifacts/analysis/` root. It may hash imported `config.h`, `config.status`, every generated `Makefile`, `Makefile.in` and the original executable project; it writes only a declarative profile and JSON manifest. |
| Non-goals | No configure, source copy, config projection, build root, nmake, compile, archive, link, executable, adapter/CLI/OpenNT input, or guest/runtime operation. This does not revive the rejected CPU3 config-header route. |
| Required evidence | Complete generated-Makefile family inventory with hashes; paired original config/status identity; original target/VS metadata identity; no overlay files; named runtime-profile hash; explicit distinction between compiled support and enabled machine capability. |
| Verification | Inspect tuple inventory coverage against the imported tree, verify the profile only uses original Bochs configuration mechanisms, verify fresh-root collision refusal, then run documentation governance verification. |
| Risks and stop conditions | Stop if any configuration-derived file is omitted, a profile claims compiled support is enabled, or a generator must synthesize configure substitutions. The next build root may copy this tuple only as a whole. |
| Exit criteria | A deterministic manifest names one intact native configuration tuple, one minimum runtime profile, and the exact inputs required for a later separately named target. |

**Result (2026-08-11):**
`artifacts/analysis/t95-s6-intact-native-tuple-manifest-001-20260811-001`
now pins matching imported `config.h`, `config.status`, root Makefile/template,
executable project and all 41 generated Makefile/template inputs.  Its hashed
profile uses only original runtime configuration controls and explicitly
distinguishes compiled support from enabled capability.  The manifest root
refuses overwrite.  See
`etc/research/t95-s6-intact-native-tuple-manifest-001.md`.  No configuration
or build occurred.

### S6 Intact-Native Build-Root Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Materialize one fresh root that copies the frozen intact native tuple unchanged and derives only a separately named native-container target. |
| Scope | Add one generator under `tools/`, invoke it once into a new build root, verify every 41-file tuple identity before and after copy, copy the declared ROM/profile inputs, and create a root-local target-name-only transform of the original executable recipe. One later `nmake /n` dry-run is permitted after static equality review. |
| Non-goals | No config replacement, generated-Makefile edit, configure, source patch, compile, archive, link, executable, reset, adapter/CLI/OpenNT/guest input, `all`, VS solution or `bochs.exe` output. |
| Required evidence | Complete tuple hash equality source-to-copy; named profile/ROM identity; target line equality except its name; fresh-root collision refusal; and dry-run output that reaches only the separately named target command surface. |
| Verification | Verify the record and copied tuple, static-compare target lines, then inspect one `/n` log for prohibited target/component names and zero generated object/target output. |
| Risks and stop conditions | Stop if any copied tuple input diverges or any source/build configuration file must be edited. Do not treat dry-run as a build result. Actual target build needs another admission. |
| Exit criteria | One reproducible intact fresh root and dry-run prove the exact future native target command surface without changing Bochs's configuration system. |

**Result (2026-08-11):**
`artifacts/build/bochs-2.6-native-intact-msvc-r1` is a hash-equal copy of all
41 tuple Makefile/template inputs plus matching `config.h`/`config.status`.
Its renamed target is a target-name-only transform of the original recipe.
The one permitted NMAKE dry-run names original archive families and ends at
`/out:ntdos64-native-container.exe`; it contains no excluded component/target
and generated zero `.o`/target files.  See
`etc/research/t95-s6-intact-native-build-root-001.md`.  Actual compilation
remains separately unadmitted.

### S6 Intact-Native First Build Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Obtain one no-retry MSVC/x86 native-container build observation using the intact configuration tuple and separately named target. |
| Scope | In `artifacts/build/bochs-2.6-native-intact-msvc-r1` only, call BuildTools x86 and exactly `nmake /f ntdos64-native-container.mak ntdos64-native-container.exe`, capture a complete log and inspect output identities after it returns. |
| Non-goals | No retry, config/source/generated-Makefile/shim edit, configure, `all`, VS solution, `bochs.exe`, executable run/reset, firmware, adapter/CLI/OpenNT/guest semantics or runtime claim. |
| Required evidence | Pre-call tuple/profile/shim hashes; command/tool identity; complete log/exit code; first terminal compiler/archive/link result; target/hash only if produced; and a check that the final link input has no adapter/CLI/OpenNT path. |
| Verification | Preserve all output, inspect artifact/map/log identities read-only, classify the first terminal boundary, and run documentation governance verification. |
| Risks and stop conditions | Stop permanently after this call. Any failure is evidence, not authorization to patch or rerun. Any unexpected `all`, `bochs.exe` output or excluded input rejects the observation. |
| Exit criteria | One immutable classification: target built without runtime claim, or explicit source/toolchain/archive/link boundary with the next decision recorded. |

**Result (2026-08-11):** The sole foreground invocation was externally
terminated at the host's approximately 65-second command limit while compiling
the original CPU archive (`xsave.cc` was the last logged unit).  No Bochs
diagnostic, archive, link, target executable or map was reached; 74 copied-root
`.o` files and the complete log are retained, and no build process remains.
This is **external execution-time-limit interruption**, not a build failure or
success.  The root is immutable and will not be resumed/retried.  See
`etc/research/t95-s6-intact-native-build-observation-001-timeout.md`.  Any
complete observation requires a newly admitted fresh root and one detached,
monitored no-retry invocation.

### S6 Default-Off-Isolation Rebuild Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Verify the registered BX-EXEC-032 default-off isolation in one new, intact native detached build observation. |
| Scope | Generate a new intact-tuple manifest after the registered source change, then create one new root and invoke the existing detached launcher once with that manifest. Monitor read-only to terminal exit. |
| Non-goals | No retry, source/config/generated-Makefile/shim change after manifesting, configure, `all`, `bochs.exe`, target execution/reset, adapter/CLI/OpenNT/guest semantics or runtime claim. |
| Required evidence | New tuple hashes; default path static isolation proof; new root/PID/wrapper command identity; terminal exit/log; target/map identity only if created; confirmation no adapter input appears in default final link. |
| Verification | Use the generator/launcher/monitor only; inspect final log and outputs read-only, update the intrusion record and run governance verification. |
| Risks and stop conditions | The new root must be distinct from r1/r2. Stop permanently after its one terminal outcome; any unexpected default adapter input rejects the boundary and is not repaired in place. |
| Exit criteria | The default isolated native target is classified from one complete detached execution, with no runtime claim. |

**Result (2026-08-11):** r3 (`bochs-2.6-native-intact-msvc-r3`) completed its
one detached invocation with exit `2`.  The renamed final link is free of
adapter/CLI/OpenNT inputs and the prior missing adapter header does not recur;
the new first stop is exactly the four definitions moved by `BX-IO-029` and
`BX-IO-030`, absent from the unchanged `libiodev.a` object list.  No target was
created or run.  `etc/research/t95-s6-intact-native-build-observation-002.md`
retains the command, PID, log result and classification.  `BX-BUILD-033` is
now registered to repair only that archive membership; r3 will not be retried.

### S6 Isolated-Port Archive-Membership Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Verify whether the registered `BX-BUILD-033` same-archive membership repair resolves the complete r3 four-symbol closure set in one new intact native build. |
| Scope | Static-check the two matching I/O Makefile entries, freeze a new tuple manifest, then create exactly one new r4 root and launch the existing detached observer once. |
| Non-goals | No r3 reuse/retry, source-body/header/API change, configuration/profile/device/plugin change, configure, `all`, `bochs.exe`, target execution/reset, adapter/CLI/OpenNT/guest input, or runtime claim. |
| Required evidence | Exact membership in source Makefile/template; new tuple hashes; r4 wrapper/PID/command; terminal log/exit; target identity only if created; explicit classification of the first stop or successful link. |
| Verification | Run the focused static membership check, then only the existing manifest generator/launcher/monitor.  At terminal exit inspect outputs read-only, update the exception record/result, and run governance/diff checks. |
| Risks and stop conditions | A new error is evidence, not permission for an in-place correction.  Stop after r4's one terminal outcome; any additional source/build-graph correction needs its own registered exception and a separate admission. |
| Exit criteria | `BX-BUILD-033` is classified by one fresh-root observation without a reset, firmware, guest, or product-runtime claim. |

**Result (2026-08-11):** r4 (`bochs-2.6-native-intact-msvc-r4`) completed its
single detached MSVC/x86 invocation with exit `0` and produced
`ntdos64-native-container.exe` (SHA-256
`F8200EF546C6760816C28E76F0DF90CD93860844ECFE82F59AC7A21D7A3F9AFD`).
The original I/O archive now contains `minimal_port_space.o`; the complete r3
four-symbol set is resolved.  The recorded final link and full log contain no
adapter, CLI, or OpenNT input.  `BX-BUILD-033` is implemented and
link-verified.  This is source-build evidence only: no executable, reset,
firmware, guest, adapter, or CLI path was run.  See
`etc/research/t95-s6-intact-native-build-observation-003.md`.

### S6 Native Reset-to-Controlled-Stop Design (2026-08-11)

**Result:** `etc/research/t95-s6-native-reset-controlled-stop-design.md`
source-traces the only accepted original bounded stop: `-benchmark n` creates
a one-shot `n * 1,000,000` emulated-tick timer, whose native callback sets the
ordinary CPU-loop kill request.  It is explicitly not an instruction budget.
The proposed first observation is r4 only, with `-q`, the r4 immutable profile,
and `-benchmark 1`; it requires Bochs's benchmark activation and native
CPU-loop shutdown markers, while rejecting adapter/BYOB/guest inputs and
host-watchdog termination as success.  This is design evidence only; no
process has been started.

### S6 Native Reset-to-Controlled-Stop Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Establish one bounded, adapter-free native lifecycle observation of the r4 container from original reset through Bochs's own benchmark-timer controlled stop. |
| Scope | Add one fresh-root-refusing observer under `tools/`; copy only the r4 profile and two ROM inputs to a new evidence root; start exactly one r4 process with `-q -f <copied-profile> -benchmark 1`; retain command, input hashes, stdout, stderr, Bochs log, exit code and elapsed wall time. |
| Non-goals | No rebuild/relink, Bochs source/config/build-graph change, adapter/CLI/OpenNT object or environment input, BYOB/guest payload/disk, #UD/BOP/DEM/DOS/WOW path, firmware replacement, device/plugin enablement, product launcher, or runtime claim. |
| Required acceptance | The copied inputs match r4 hashes; original benchmark-mode and `cpu loop quit, shutting down simulator` log markers both occur; no watchdog termination occurs; command/environment evidence contains no adapter/BYOB/guest term. |
| Stop conditions | Configuration prompt/panic, parse failure, absent marker, non-returning process, watchdog termination, unexpected adapter/guest input, or an exit result outside documented native behavior is terminal evidence.  Do not retry or repair in place. |
| Verification | Inspect the one result read-only; write an evidence record; run documentation governance and safe diff checks.  A result proves only bounded native lifecycle, never DOS/guest execution. |
| Exit criteria | One immutable observation is uniquely classified as native controlled-stop reached or unavailable/failed, with all negative-boundary evidence retained. |

**Result (incomplete, 2026-08-11):** The first fresh observation root was
created and its r4 profile/ROM inputs copied, but the observer's asynchronous
stream collector raised a null-valued-expression error before it retained
stdout, stderr, a Bochs log, exit code or terminal record.  It is therefore
not runtime evidence and r4 was not rebuilt or changed.  The partial root is
retained; `etc/research/t95-s6-native-reset-controlled-stop-observation-001-tool-failure.md`
records the limitation.  The observer is corrected, unexecuted, to use a
root-local CMD wrapper with direct redirects and an exit-code file.  The
current admission prohibits an in-place retry; any later execution needs a
separate admission and fresh root.

### S6 Corrected-Transport Native Controlled-Stop Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Obtain the one missing terminal evidence chain after observation-001's observer-only transport failure, using a corrected collector and a distinct fresh root. |
| Scope | Invoke the corrected observer once under `artifacts/analysis/t95-s6-native-reset-stop-observation-002-20260811-001`, with only r4 executable/profile/ROM inputs, `-q`, and `-benchmark 1`.  It may create its own CMD wrapper, stream files, exit-code file and JSON record. |
| Non-goals | No reuse or alteration of observation-001, rebuild, relink, Bochs/adapter/OpenNT/CLI source change, guest/BYOB/disk input, adapter environment opt-in, firmware replacement, device/plugin enablement, or product runtime claim. |
| Required acceptance | The root-local wrapper/exit-code/stdout/stderr/JSON chain exists; input hashes equal r4; original benchmark activation and CPU-loop shutdown markers are retained; watchdog does not terminate the process; and the command/environment contain no adapter/guest input. |
| Stop conditions | Any absent evidence file, wrapper failure, configuration failure, panic, timeout, missing marker, unexpected boundary term, or unsupported exit code is terminal evidence.  Do not run a third instance under this packet. |
| Verification | Perform one syntax/static review before the invocation, then inspect the terminal root read-only and update the supporting evidence.  The resulting classification is limited to native bounded lifecycle. |
| Exit criteria | A complete, uniquely classified r4 reset-to-controlled-stop record, or a terminal unavailable/failed conclusion with no third run. |

**Result (2026-08-11):** The r002 wrapper/stream/exit chain is complete.  Its
stdout proves original benchmark activation, `Reset(HARDWARE)`, CPU reset and
the original CPU-loop controlled-stop marker before the 30-second watchdog;
exit `1` is the observed native nogui exit stub path.  No adapter or guest
input is present.  The observer's first JSON predicates were false only
because it omitted stdout and mistook the project binary name `ntdos64` for a
guest term; the retained streams control and the observer is corrected without
another run.  Crucially, the same output proves the existing profile is not
minimal: PCI/440FX/PIIX3, CMOS, DMA, PIC/PIT, floppy, VGA/VBE, ACPI, I/O APIC,
keyboard and IDE initialize.  Thus S6 has native bounded-lifecycle evidence
but rejects the profile as the project runtime minimum.  See
`etc/research/t95-s6-native-reset-controlled-stop-observation-002.md`.

### S6 Native Minimum-Profile Boundary Audit (2026-08-11)

**Result:** `etc/research/t95-s6-native-minimum-profile-boundary-audit.md`
proves from the original parser and device manager that `pci: enabled=0`, ATA
channel settings, VGA extension selection and `plugin_ctrl` are declarative
controls, but CMOS/DMA/PIC/PIT/floppy/core-VGA/keyboard and compiled I/O APIC
are original unconditional lifecycle loads.  Therefore a bare CPU/RAM/ROM
machine is not expressible without a new `devices.cc` composition or parallel
lifecycle and is rejected as an impermissible Bochs rewrite.  The admissible
minimum is the coherent native **PC reset floor**, with optional groups denied
declaratively, not an ever-growing series of source-level device removals.
No profile file, source change or new execution occurred in this audit.

### S6 PC Reset-Floor Profile Design (2026-08-11)

**Result:** `etc/research/t95-s6-pc-reset-floor-profile-design.md` defines one
coherent source-parser-only profile: deny PCI/i440FX, all ATA channels,
VBE/Cirrus extension and listed optional plugins while retaining the native
firmware-reset floor as an inseparable Bochs-owned unit. It records each
directive, ownership classification and trace predicates for a future single
execution. It adds no source/configuration artifact or execution; a new
fresh-root admission is required before materializing or testing it.

### S6 PC Reset-Floor Profile Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Verify one coherent declarative PC reset-floor profile against the already source-built r4 native container. |
| Scope | Extend the existing observer only to materialize its documented `pc-reset-floor-v1` profile in one fresh evidence root, then start r4 once with the unchanged ROM pair, `-q`, and `-benchmark 1`. |
| Non-goals | No r4 rebuild/relink, Bochs source/build/configuration-system change, per-device patch, adapter/CLI/OpenNT/BYOB/guest input, attached media, firmware replacement, or product runtime claim. |
| Required acceptance | Retain generated profile/wrapper/streams/exit/JSON hashes; prove original reset and controlled-stop; prove absence of PCI/440FX/PIIX3/ACPI/PCI IDE and VBE initialization; prove no adapter/guest input. |
| Stop conditions | Any parse error, watchdog stop, absent lifecycle marker, optional-device initialization, adapter/guest term, or unsupported exit is terminal evidence.  Do not run a second profile observation under this admission. |
| Exit criteria | The profile is uniquely classified as a declarative PC reset-floor minimum or rejected with its exact original limitation. |

**Result (2026-08-11):** The one all-disabled-ATA run successfully denied
PCI/440FX/PIIX3/ACPI/PCI IDE and VBE, but `main.cc`'s unconditional nogui
drive-status query reached the missing hard-drive stub and blocked at a Bochs
panic prompt until watchdog termination. It is a valid rejection of that
profile, not a runtime success. Source audit proves one enabled **empty** ATA
controller is the smallest original way to retain the hard-drive plugin, with
no attached media or host path. The profile design is revised accordingly; no
source patch or second run occurred. See
`etc/research/t95-s6-pc-reset-floor-profile-observation-001.md`.

### S6 Empty-ATA PC Reset-Floor Profile Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Verify the one source-proven correction to the rejected all-disabled-ATA profile: retain exactly one empty original ATA controller so intact `main.cc` lifecycle remains valid. |
| Scope | Run the corrected `pc-reset-floor-v1` observer once in a new root. `ata0` is enabled but no disk/CD image, host path, PCI IDE, adapter or guest payload is permitted; all other profile denials remain unchanged. |
| Non-goals | No retry of profile observation 001, Bochs source/build/configuration-system patch, attached media, host filesystem integration, adapter/CLI/OpenNT input, PCI/i440FX/ACPI/PCI IDE/VBE enablement, or product claim. |
| Required acceptance | Original reset and controlled-stop markers; no watchdog; no PCI/440FX/PIIX3/ACPI/PCI IDE/VBE initialization; hard-drive plugin may initialize but must report no attached media/path; no adapter/guest input. |
| Stop conditions | Any new panic, timeout, optional device initialization, media/host path, missing marker or unexpected boundary input is terminal evidence. Do not run another under this admission. |
| Exit criteria | The corrected declarative PC reset-floor profile is classified from one immutable trace. |

**Result (2026-08-11):** The corrected empty-ATA profile completes in 323 ms
without watchdog termination. It proves original hardware reset and bounded
CPU-loop stop, PCI disabled, and absence of PCI/440FX/PIIX3/ACPI/PCI IDE/VBE
initialization. The trace retains only the original PC reset floor plus one
empty hard-drive controller needed by `main.cc`; it contains no media, host
path, adapter, BYOB or guest input. `pc-reset-floor-v1` is the accepted
runtime-minimum profile, not a DOS-runtime claim. See
`etc/research/t95-s6-pc-reset-floor-profile-observation-002.md`.

### S6 Stable-Machine Composition Design (2026-08-11)

**Result:** `etc/research/t95-s6-stable-machine-composition-design.md` fixes
the next boundary: existing CLI BYOB validation and child-only handoff feed a
separate same-toolchain adapter observation target; existing `startup_plan_v1`
may carry only a pointer-free NTIO preload/real-mode entry. r4 remains default
adapter-free. NTDOS/COMMAND identities are not yet loaded, and no DOS/BOP/DEM
or filesystem semantics are admitted. The next gate is read-only adapter
same-island closure audit before any companion target or guest run.

### S6 Adapter Same-Island Closure Audit (2026-08-11)

**Result:** `etc/research/t95-s6-adapter-same-island-closure-audit.md` rejects
direct linkage of the present 38-object `bx_ntvdm_adapter_runtime` into r4:
its direct service headers and runtime surface would import unreached
DEM/BOP/CMD/namespace/search capability just to obtain a startup plan. Earlier
UCRT object evidence is not an MSVC/r4 link proof. No build occurred. The next
gate is a design-only adapter `startup-session` extraction that reuses typed
ABI records but excludes every host-service semantic path before any companion
target is considered.

### S6 Minimal Adapter Startup-Session Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Materialize one adapter-owned startup-session component that stages verified BYOB identities and creates the existing pointer-free NTIO startup plan without importing the broad runtime/service plane. |
| Scope | Add `bx_ntvdm_startup_session.c/.h` under the adapter, using only BYOB profile/image APIs and CPU-state, guest-write and startup-plan ABI records. It may own three immutable image buffers; only NTIO may become a startup payload. |
| Non-goals | No Bochs source/build change, adapter-runtime modification, host session, service catalog, DEM/BOP/CMD/namespace/search/drive policy, guest execution, profile format change, CLI change, target/link/run, NTDOS/COMMAND load, DOS semantics or host filesystem exposure. |
| Required evidence | Focused static test proves allowed include/value surface, releases all images on failure/destruction, requires all three verified identities, permits only NTIO payload at declared `0x700`/`0070:0000` with `0x714..0x717` preservation, and rejects prohibited wide-runtime/service terms. |
| Stop conditions | Stop if implementation needs a guest loader, a raw host/Bochs pointer ABI, a new Bochs hook, host-service behavior, CLI parsing, or a change to existing startup-plan semantics. |
| Exit criteria | Source-boundary implementation and focused static test only. A same-island compile/link or runtime still requires separate admission. |

**Result (2026-08-11):** `bx_ntvdm_startup_session.c/.h` now owns only BYOB
identity validation/loading and the existing NTIO startup-plan preparation.
It validates and retains all three component identities but writes/enters only
NTIO at the pre-existing `0x700` / `0070:0000` values. The focused boundary
test passes and rejects broad runtime, host-service, DEM/BOP/CMD, namespace,
search, drive-policy and Bochs dependencies. No compile, link, target, guest
write or execution occurred. See
`etc/research/t95-s6-minimal-adapter-startup-session.md`.

### S6 Startup-Session MSVC Object Audit Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Establish whether the minimal startup-session slice has a compilable MSVC/x86 object closure independent of the broad adapter runtime. |
| Scope | In one fresh `artifacts/analysis/` root, copy exactly `startup_session`, BYOB profile/identity/image, CPU-state, guest-range, guest-write and startup-plan C/header inputs. Invoke BuildTools x86 `cl /nologo /c /MT /W3 /DWIN32` once for each of the eight declared C translation units, with only copied adapter/CLI include directories. Capture commands, output and object hashes. |
| Non-goals | No Bochs object/source/configuration, exception-plan macro, archive/link/executable/run, broad runtime/host-session/service source, CLI executable, guest payload, BYOB profile input, Windows API link, or runtime claim. |
| Stop conditions | First compiler error ends the pass; no retry, additional source, altered flags or alternate configuration is allowed. A successful object set remains neither a same-island link nor an adapter target. |
| Verification | Compare copied source hashes to the worktree, ensure exactly eight object results, run the existing startup-session boundary test and governance checks. |
| Exit criteria | One reproducible object-only classification of the narrow startup-session closure. |

**Result (pre-compiler tool failure, 2026-08-11):** The first fresh analysis
root was created, but the audit PowerShell tool failed while calculating a
copy-source path (`if` was parsed as a command). No `cl.exe` invocation,
object, archive or link occurred. The root is retained and not reused; the
tool is corrected without an object claim.

### S6 Corrected Startup-Session MSVC Object Audit Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Execute the still-unperformed single MSVC/x86 object audit after the pre-compiler tool correction. |
| Scope | Use the same eight source list/flags and all prior non-goals in a distinct fresh analysis root. |
| Stop conditions | The first compiler error ends the pass. No further object audit follows this admission without a new decision. |
| Exit criteria | One terminal compile-only classification with manifest/log evidence. |

**Result (tool failure before compilation):** The distinct corrected root
`artifacts/analysis/t95-s6-startup-session-msvc-objects-002-20260811-001`
was created, but the audit tool encountered a second PowerShell conditional
expression in the header-copy stage. As with the first root, no `cl.exe`
invocation, object, archive, or link occurred. Both roots are retained as
non-compiler tool-failure evidence. The tool is corrected for later use, but
this admission is terminal: a further object audit requires a new admission
after review of the repeated harness failure.

### S6 Prevalidated Startup-Session MSVC Object Audit Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Make one final, compile-only determination of the eight-source startup-session slice after review and correction of both pre-compiler harness failures. |
| Scope | Before creating its fresh root, parse the audit script with the PowerShell parser. Then use one distinct root and the same eight declared source files, copied headers, BuildTools x86 environment, `cl /nologo /c /MT /W3 /DWIN32`, copied-only includes, per-object commands, hashes and terminal log. |
| Preconditions | The two prior roots are retained and terminal; review located and corrected both `Join-Path (if ...)` expressions. The corrected script must parse cleanly before this admission starts. |
| Stop conditions | First compiler error, missing toolchain, or any remaining harness failure ends this pass. No archive, linker, executable, Bochs target, guest input or runtime invocation is allowed. |
| Exit criteria | Exactly one terminal object-only classification with the fresh-root manifest/log; a successful compile proves neither link closure nor execution. |

**Result (object closure available):** The parser preflight passed and the
single fresh root
`artifacts/analysis/t95-s6-startup-session-msvc-objects-003-20260811-001`
completed with BuildTools 2022 x86 `cl` exit code zero. Its
`object-audit.json` preserves the source/object SHA-256 manifests: all eight
declared translation units produced eight objects under the copied-only
include boundary. `compile.log` names no other source. This is an available
MSVC/x86 **object** closure for the narrow startup-session slice only; it made
no archive, link, executable, Bochs object, guest write or run claim. See
`etc/research/t95-s6-minimal-adapter-startup-session.md`. **Next gate:** a
separately admitted static symbol/dependency inventory of these exact objects;
do not propose a companion link until that inventory identifies the required
own and platform edges.

### S6 Startup-Session Static Symbol Inventory Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Classify the internal and Windows-platform symbol edges of the exact successful eight-object startup-session closure before any archive or companion-link proposal. |
| Scope | In one fresh analysis root, hash the eight `.obj` inputs from `t95-s6-startup-session-msvc-objects-003-20260811-001`, run the installed MSVC x86 `dumpbin /symbols` and `/directives` against each, preserve raw outputs and a machine-readable manifest, then classify unresolved externals as slice-internal, declared Win32/UCRT platform, or prohibited/unresolved. |
| Non-goals | No compilation, archive, link, executable, Bochs source/object/configuration, adapter-runtime source, guest/host capability, profile, CLI behavior, guest write or execution. The prior object root is input-only and must not be modified. |
| Stop conditions | Missing exact objects/dumpbin, nonzero dumpbin result, any input hash discrepancy, a Bochs/OpenNT/broad-runtime edge, or classification ambiguity stops the investigation and records no link claim. |
| Exit criteria | One retained evidence root and source-backed inventory sufficient to decide whether a separately admitted link-design study is lawful; never a linkability or runtime claim. |

**Result (bounded pre-link edges):** The fresh root
`artifacts/analysis/t95-s6-startup-session-symbols-001-20260811-001` preserves
hashes plus raw `dumpbin /symbols` and `/directives` output for all eight exact
objects. Their only non-slice edges are MSVC/UCRT support, `Kernel32` file/heap
imports and `BCrypt` SHA-256 imports. `uuid.lib` appears only as a default
directive with no UUID unresolved symbol. No Bochs, OpenNT, SoftPC,
DEM/BOP/CMD, broad adapter-runtime or host-service symbol is present. The
evidence is recorded in
`etc/research/t95-s6-startup-session-static-symbol-inventory-001.md`. This
authorizes a design-only, separately admitted same-island link study; it is not
a linkability, cross-island ABI, guest or runtime result.

### S6 Narrow Opt-In Execution-Plan Owner Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Remove the broad adapter-runtime dependency from the existing default-off Bochs execution-plan seam, replacing it with an adapter-owned, same-island endpoint that creates only the already verified NTIO startup plan from the CLI child environment. |
| Scope | Register `BX-EXEC-034` before changing `refs/bochs/main.cc`. Add one adapter C endpoint and header that read only `NTDOS64_ADAPTER_PROFILE` and `NTDOS64_ADAPTER_ROOT`, retain a process-local narrow startup session, and return absent/ready/rejected plan status. In the guarded Bochs block, replace only the included header and preparation call. Update the focused static boundary test. |
| Non-goals | No Bochs CPU/memory/lifecycle algorithm change; no default target build-vector change; no broad runtime, drive snapshot, host session/service, DOS/WOW/DEM/BOP/CMD/search/namespace semantics; no CLI change; no Bochs build, link, guest write or execution. NTDOS and COMMAND remain verified identities only. |
| Stop conditions | Stop if the narrow endpoint needs a Bochs header/object, a host-service/drive-policy dependency, profile parsing beyond existing BYOB selection, a new guest-memory primitive, a Bochs link-vector change, or any behavior outside absent/ready/rejected selection. |
| Verification | Focused static test proves the guarded Bochs helper has no runtime header/call and preserves its original generic RAM/entry ordering; startup-session boundary test rejects wide dependencies; governance and safe diff checks pass. |
| Exit criteria | Source-boundary replacement and negative tests only. An MSVC object rebuild, companion link and guest run still require separate admissions. |

**Result (source boundary complete):** `BX-EXEC-034` was registered before
the one-line Bochs source substitution. The default-off block in `main.cc` now
includes and calls the narrow startup-session environment endpoint, while its
generic RAM-copy, preservation and real-mode-entry sequence is unchanged. The
new adapter endpoint reads only the two child-environment values, owns the
narrow verified BYOB session, and reports absent/ready/rejected; it has no
Bochs header, broad runtime, host session/service, drive-policy or guest
semantic dependency. `Test-BochsExecutionPlanBoundary.ps1` and the expanded
`Test-StartupSessionBoundary.ps1` pass. No MSVC rebuild, Bochs target, link,
guest write or execution occurred. The next gate is a separately admitted
companion-link design and ABI review, not a build retry.

### S6 Narrow Companion-Link Design (2026-08-11)

**Result (design-only):** The accepted pre-link inventory and source boundary
now resolve to the independent
`ntdos64-native-adapter-observation.exe` design in
`etc/research/t95-s6-narrow-companion-link-design.md`. It preserves the r4
default target and tuple, recompiles only opt-in `main.o`, adds exactly the
nine narrow adapter/CLI sources in the same MSVC/x86 `/MT` island, and appends
only `kernel32.lib`/`bcrypt.lib` to the original link vector (with `uuid.lib`
still treated as a recorded default directive). The first prospective runtime
is explicitly no-environment/no-plan only. This is a build-design/ABI result,
not a build admission, target, link or execution claim. **Next gate:** a
fresh-root companion-link admission must name the exact r4 tuple hash, copied
source manifest, one explicit main-object recipe, exact link argument vector,
map/import/undefined artifacts, and first-failure stop condition.

### S6 Narrow Companion-Link Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Determine once whether the explicit narrow companion design links in the same MSVC/x86 `/MT` island without widening the adapter or changing the default Bochs target. |
| Inputs | The r4 source-built `ntdos64-native-container.exe` and its build-root libraries are immutable prebuilt inputs. A fresh root copies that exact r4 root, replaces only its local `main.cc` with the registered `BX-EXEC-034` source, removes only its local stale `main.o`, and copies the nine design-listed adapter/CLI C inputs and their headers. Every copied input receives a SHA-256 manifest. |
| Scope | One `nmake /f ntdos64-native-adapter-observation.mak ntdos64-native-adapter-observation.exe` invocation. The shim explicitly recompiles `main.o` with `BX_NTVDM_ENABLE_EXECUTION_PLAN=1`, compiles exactly nine narrow C objects with `/MT /W3 /DWIN32`, and links the r4 vector plus only those objects, `kernel32.lib`, `bcrypt.lib`, `/MAP` and `/VERBOSE:LIB`. |
| Non-goals | No r4/default-root mutation, Bochs Makefile/configure/source change, broad runtime object, OpenNT object, MinGW object, executable run, guest input/environment, guest write, CLI behavior, host drive/service, DOS/WOW/DEM/BOP semantics, retry or alternate link vector. |
| Stop conditions | Any source/hash/copy/toolchain failure, first C/C++ compiler failure, or first link failure is terminal for this admission. Preserve the new root, shim, command, logs, map when available and output metadata; do not rerun, patch or try libraries/objects beyond the declared vector. |
| Exit criteria | One source-built companion-link classification only. A successful link proves neither default-target isolation nor no-plan startup nor guest handoff; those require later admissions. |

**Result (pre-compiler shim failure):** The fresh r1 root was prepared with
the declared r4 and source manifests, but its single `nmake` invocation stopped
at `ntdos64-native-adapter-observation.mak(5)` before any compiler or linker
command: PowerShell array-expression precedence emitted the `ADAPTER_OBJS`
value as a second makefile line. The root, wrapper, exit code and log are
retained at `artifacts/build/bochs-2.6-native-adapter-msvc-r1`; no companion
object, map or executable exists. The root-generator is corrected and now
asserts one complete variable line. This admission is terminal; any build
attempt requires a new root and a new admission.

### S6 Prevalidated Narrow Companion-Link Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Execute the still-unperformed one companion-link observation after correcting and structurally validating the build-root shim generator. |
| Scope | Use a new r2 root with the same r4 input, source list, compiler flags, link vector, negative scope and first-failure rule as r1. The generator must pass PowerShell parsing and assert one complete `ADAPTER_OBJS` line before the root is accepted. One `nmake /n` dry run may verify only the command shape before the single actual `nmake` call; it must not compile, link or modify inputs. |
| Stop conditions | The first compiler, link, tool or remaining shim error is terminal. No retry in r2, extra object/library, source change, default target change or runtime action follows this admission. |
| Exit criteria | One retained r2 object/link classification only; success remains neither a no-plan startup nor a guest execution result. |

**Result (same-island companion link available):** The r2 `nmake /n`
preflight emitted the declared ten compile commands and one exact r4-derived
link vector without execution. Its single real wrapper invocation wrote exit
code `0`, produced
`ntdos64-native-adapter-observation.exe` (2,494,976 bytes,
`1B7283123409720FB0700BCCC526E9563E54A5C089A70C33F45BD398E113CBED`) and
a link map. The outer PowerShell wait hit its 63-second transport timeout only
after the root-local wrapper completed; no process remained and no second
build was started. The retained map resolves the narrow environment/NTIO-plan
functions and has no broad-runtime, host-session, DEM/BOP or OpenNT symbol.
See `etc/research/t95-s6-narrow-companion-link-observation-001.md` and the
root's post-run JSON record. This proves one MSVC/x86 companion link only.
**Next gate:** a separately admitted no-environment PC-reset-floor execution
of this exact executable; no guest payload is yet allowed.

### S6 Narrow Companion No-Environment Reset Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Verify that the linked opt-in companion takes the original no-plan path when the CLI child profile/root environment is absent. |
| Scope | In one fresh evidence root, copy the accepted PC-reset-floor-v1 profile and r2 ROM inputs; clear all `NTDOS64_ADAPTER_*` values in a wrapper; invoke the exact r2 companion with `-q -f ... -benchmark 1` once and a 30-second watchdog. Record stdout/stderr, exit, hashes and stop predicates. |
| Non-goals | No profile/root BYOB values, synthetic payload, NTIO/NTDOS/COMMAND guest bytes, CLI runner, drive policy, service/DEM/BOP behavior, binary rebuild or link. |
| Stop conditions | Watchdog termination, missing benchmark/reset stop, adapter/BYOB input evidence, unexpected exit, or missing artifact ends the observation. No rerun under this admission. |
| Exit criteria | One no-environment original reset-floor classification only; it does not prove the endpoint is exercised or any guest handoff. |

**Result (no-plan reset-floor available):** The single fresh observation root
`artifacts/analysis/t95-s6-narrow-companion-noenv-reset-001-20260811-001`
ran the r2 companion once with profile/root cleared. It completed in 3,156 ms,
without watchdog termination, with native nogui exit `1`; the trace contains
the original hardware reset, benchmark mode and `cpu loop quit, shutting down
simulator` at tick `10,000,005`, ending real-mode halted. The only failed
tool predicate was a false positive: its broad word `adapter` matched the
companion executable's own name in the command string, despite no adapter
environment or BYOB input. The tool now tests only actual payload-input forms;
the root is not rerun. Therefore the retained stdout/wrapper/JSON establish
the intended no-environment/no-plan result, not endpoint execution or guest
handoff. **Next gate:** define a single synthetic verified payload admission
for generic write/entry mechanics before source-built NTIO.

### S6 Synthetic HLT Startup-Plan Mechanics Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Prove the linked narrow endpoint's generic verified-BYOB -> RAM-copy -> real-mode-entry mechanics once, without attributing any DOS/NTIO semantics. |
| Inputs | In one new evidence root, generate an internal, non-distributable BYOB package: `NTIO.SYS` is exactly byte `F4` (`HLT`); `NTDOS.SYS` and `COMMAND.COM` are required one-byte identity placeholders that are verified but never placed. Its manifest uses the existing fixed schema/roles and only `machine_startup_plan=true`, `machine_startup_entry=ntio-v0`. A local artifact manifest records role, bytes and SHA-256. |
| Scope | Run the exact r2 companion once against the accepted PC-reset-floor-v1 profile with only the package profile/root environment values. Preserve wrapper, stdout/stderr, package manifest, exit, hashes and timing. Require the generic `requested execution plan applied` log, bounded benchmark stop and real-mode halted result. |
| Non-goals | No source-built OpenNT payload, NTDOS/COMMAND guest load, DOS/DEM/BOP service, host-drive/namespace capability, CLI runner, binary build/link, firmware/device change, new Bochs intrusion or reusable DOS test image. |
| Stop conditions | Package validation rejection, absent plan-applied log, watchdog, unexpected exit/stop, any NTDOS/COMMAND placement evidence or additional environment value terminates the observation. No rerun or profile variant follows this admission. |
| Exit criteria | One synthetic generic-mechanics classification only. It authorizes neither a real NTIO attempt nor an NTVDM/DOS runtime claim. |

**Result (generic handoff available):** The single synthetic package root
accepted all three verified identities. Its one-byte `F4` NTIO payload was the
only placed byte; the companion logged `requested execution plan applied`,
then stopped with `CS:0070` and `EIP=00000001` after HLT, followed by the
original benchmark stop (327 ms, no watchdog, exit `1`). The artifact manifest
records that NTDOS/COMMAND were identity-only placeholders. See
`etc/research/t95-s6-synthetic-hlt-startup-observation-001.md`. This closes
only generic verified payload placement/entry mechanics. **Next gate:** one
separately admitted source-built NTIO first-prefix attempt with fixed
identity-only NTDOS/COMMAND inputs and no synthetic machine state.

### S6 Source-Built NTIO First-Prefix Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Make one bounded, evidence-only first-prefix attempt with the historical source-built NTIO image through the now proven generic handoff path. |
| Inputs | Copy the 33,792-byte source-built Tools16 `NTIO.SYS` with recorded SHA-256 `CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937`. Copy the fixed 27,858-byte NTDOS and 50,384-byte COMMAND identities from the prior r20 research fixture; they remain verification-only inputs. Generate only the existing BYOB profile with `machine_startup_plan=true` / `ntio-v0`; no synthetic guest state or byte patch is allowed. |
| Scope | One fresh evidence root and one r2 companion invocation against PC-reset-floor-v1 with a 30-second watchdog. Record all input/output hashes, profile, wrapper, stdout/stderr and exit. Classify the first terminal condition as native guest, adapter/mechanics or observation failure. |
| Non-goals | No source-built claim for NTDOS/COMMAND, no NTDOS/COMMAND placement/load, no DOS command, host drive/namespace, CLI runner, BOP/DEM handler, device/firmware change, Bochs patch/build/link, retry or input variant. |
| Stop conditions | Missing/hash-mismatched input, endpoint rejection, watchdog, unbounded run, first native guest fault/stop or any unexpected mechanics state is terminal. Do not alter guest bytes, add an interrupt/BIOS/host shim, or repeat this attempt. |
| Exit criteria | One reproducible source-built-NTIO first-prefix classification only. Neither a plan-applied log nor progress past it authorizes a DOS/NTVDM runtime claim. |

**Result (early guest/entry-state stop):** The single root
`artifacts/analysis/t95-s6-source-built-ntio-prefix-001-20260811-001`
verified and placed the source-built 33,792-byte NTIO image exactly, then
logged `requested execution plan applied`. NTDOS/COMMAND were verified
identity-only inputs. The process terminated without watchdog at the native
benchmark stop (387 ms, exit `1`), but the post-plan trace contains repeated
`math_abort: MSDOS compatibility FPU exception` and ends real-mode halted at
`CS:IP=0000:0001`. This is neither endpoint rejection nor DOS startup.
`etc/research/t95-s6-source-built-ntio-prefix-observation-001.md` records the
evidence and explicitly leaves the cause unproven. **Next gate:** read-only
correlation of this first post-entry trace with the source-derived NTIO
initial-state requirements and the selected native CPU/FPU profile; no retry,
shim or additional runtime input is admitted.

### S6 Source-Built NTIO First-Prefix Correlation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Produce one source-and-artifact-only causal ledger for the accepted NTIO first-prefix stop, separating established facts from candidate inputs and rejected explanations. |
| Scope | Read the preserved first-prefix log/package and map, the exact source-built `NTIO.SYS` entry bytes, OpenNT `msbio1.asm`/`msinit.asm`/interrupt macros, and the Bochs FPU pending-exception path. Record exact locations, observations, and a bounded next diagnostic question. |
| Non-goals | No executable invocation, build/link/object work, source or configuration change, Bochs intrusion, guest-byte change, BIOS/interrupt/host shim, CPU-level selection, BOP/DEM/NTDOS/COMMAND behavior, or new runtime input. |
| Evidence | A numbered research record with input paths and hashes already preserved by the first-prefix observation; source and binary/map locations; an explicit fact/candidate/rejected classification table. |
| Verification | Re-read every cited source location and verify the documentation governance checker plus safe diff check. |
| Risks and stop conditions | Stop and mark unknown if a conclusion would require an unrecorded trace, an inferred historical host state, or a changed execution profile. Do not convert a candidate into a repair authorization. |
| Exit criteria | A cause-boundary ledger that can decide only whether a future diagnostic proposal is warranted; it cannot select or implement a fix. |

**Result (cause bounded, no repair admitted):**
`etc/research/t95-s6-source-built-ntio-prefix-correlation-001.md` establishes
that NTIO offset zero intentionally jumps to mapped `init`, and that the
Bochs log means an x87 pending-summary condition reached the original
`CR0.NE=0` IRQ13 branch. It rejects neither an instruction-level cause nor a
historical-state hypothesis: fixed VDM state, reset FPU state, and later host
service reachability remain candidates or out of scope. No runtime/build/source
action occurred. **Next gate:** a separate design-only proposal for a passive,
bounded instruction/x87-state diagnostic with no state mutation or guest retry.

### S6 Passive x87 Compatibility-Diagnostic Design Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Decide, from existing source and build configuration, the smallest reusable diagnostic mechanism capable of identifying the first already-reached x87 compatibility-IRQ13 condition without changing guest execution. |
| Scope | Compare the active r2 configuration, Bochs `cpu_loop`, instrumentation hooks, debugger trace gate, `FPU_check_pending_exceptions`, and existing adapter `#UD` observation contract. Produce an ownership/negative-case design only. |
| Non-goals | No Bochs/OpenNT/adapter source change; no exception-register entry; no build/link/run; no `#UD` injection, instruction instrumentation enablement, debugger/device admission, guest patch, state override, BIOS/interrupt/host shim, profile selection, or x87 behavior change. |
| Evidence | One numbered design record naming exact source/configuration locations, the rejected existing mechanisms, the selected default-off candidate, its fixed record fields, and its potential exception-registration conditions. |
| Verification | Re-read cited locations; documentation governance verification and safe diff check. |
| Risks and stop conditions | Stop at design-only if an option requires an ambient address, arbitrary trace stream, guest semantic branch, enabled debugger/instrumentation/product feature, adapter pointer, or a modified CPU/FPU result. |
| Exit criteria | An implementation-ready-or-rejected diagnostic boundary; no execution evidence or repair authorization. |

**Result (one minimal diagnostic candidate):**
`etc/research/t95-s6-passive-x87-compatibility-diagnostic-design-001.md`
rejects the existing #UD observer, disabled instrumentation framework and
debugger trace as boundary-expanding choices. The only candidate is a
default-off, Bochs-local fixed x87/CR0 snapshot immediately before its existing
compatibility IRQ13 delivery. It has no adapter ABI or guest mechanism. A
separate implementation admission must register the precise intrusion, prove
the default-off negative object/link surface, and allow one immutable-input
runtime observation only; this design itself made no source/build/run change.

### S6 Default-Off x87 Compatibility-Diagnostic Source Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Implement the registered, default-off Bochs-local x87 compatibility snapshot with no guest or adapter behavior. |
| Scope | Before the patch, register `BX-TRACE-035`. Change only `refs/bochs/fpu/fpu.cc`: add one local default-off macro and, within the already-existing `CR0.NE==0` compatibility branch, emit fixed CPU/x87 fields immediately before the unchanged original diagnostic and `DEV_pic_raise_irq(13)`. Add one focused PowerShell static boundary test. |
| Non-goals | No adapter/OpenNT/CLI/Makefile/configuration change; no cross-island ABI, CPU/FPU/PIC algorithm change, event suppression, guest memory/address access, instruction tracer, debugger/instrumentation, device/firmware/profile change, build/link/run, or guest retry. |
| Evidence | Exception-register entry; diff limited to the named Bochs source plus its focused test; test output proving default-off and sequence/negative tokens. |
| Verification | Run the focused new test together with the existing execution-plan/default-off boundary tests, documentation governance verification, and safe diff check. |
| Risks and stop conditions | Stop if the source needs a new header/object, a counter/state allocation, an adapter/OpenNT term, a guest semantic condition, any alteration of the existing predicate or IRQ call, or a build-vector change. A later object/link/run requires a separate admission. |
| Exit criteria | Static source-boundary proof only; the macro remains disabled in all current targets. |

**Result (implemented, default-off):** `BX-TRACE-035` was registered before
the patch. `refs/bochs/fpu/fpu.cc` now has a local
`BX_NTVDM_ENABLE_X87_COMPAT_DIAGNOSTIC=0` default and an opt-in fixed native
state log immediately before the original compatibility `math_abort` log and
unchanged IRQ13 delivery. It has no new header/object, adapter or guest term.
`Test-BochsX87CompatibilityDiagnosticBoundary.ps1`, the existing #UD
default-off test, and execution-plan boundary test pass. No object, link,
native target or guest runtime was invoked. **Next gate:** a separate
same-island default-off object/link observation, then (only if unchanged) one
fresh-root immutable-input opt-in diagnostic execution.

### S6 Default-Off x87 Same-Island Link Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Determine once whether the default-off `BX-TRACE-035` source can replace only r2's FPU object/archive in a fresh companion root and preserve a same-island companion link with no new dependency surface. |
| Inputs | Immutable r2 companion root/executable/map as prebuilt base; current registered `refs/bochs/fpu/fpu.cc`; existing r2 companion shim and its nine adapter/CLI objects. Record SHA-256 identities for base executable, old/new FPU source, rebuilt archive, output, map, wrapper and log. |
| Scope | Add one root-generator/observer tool that refuses collisions; copies r2 into `artifacts/build/bochs-2.6-native-adapter-msvc-r3`; replaces only copied `fpu/fpu.cc`; removes only copied `fpu/fpu.o`, `fpu/libfpu.a`, old companion executable/map; then invokes one named companion target under the existing MSVC/x86 setup. The macro remains absent/zero. |
| Non-goals | No opt-in diagnostic, runtime, guest input, source/configuration/Makefile change, adapter/CLI object replacement, device/profile/ROM change, full Bochs target, `all`, `bochs.exe`, configure, or retry. |
| Evidence | Fresh-root manifest, wrapper/log/exit, FPU object/archive hashes, output/map hashes, and a map/import search proving no diagnostic-only external, adapter expansion, debugger or instrumentation reference. |
| Verification | Validate root/tool structural predicates before invocation; on success inspect map/import symbols and run focused boundary tests plus documentation governance/diff checks. |
| Risks and stop conditions | Any copy/hash/first compiler/link failure, unexpected target/object, missing default-off macro, or missing artifact is terminal. Do not repair or rerun in this root. |
| Exit criteria | One default-off same-island link classification only; it is not an opt-in runtime or NTIO result. |

**Result (r3 pre-execution harness failure):** The new r3 root was created and
its FPU replacement/old-object removal completed, but it retained r2's old
`narrow-companion-link-observation.json`. The observer correctly refused to
overwrite that record before starting `nmake`; no compiler, archive, linker or
executable ran. An earlier manual dry-run wrapper also failed in PowerShell
quoting before `nmake`. Both are retained harness evidence. This admission is
terminal: r3 must not be repaired or reused.

### S6 Prevalidated Default-Off x87 Same-Island Link Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Perform the still-unattempted default-off FPU replacement/link observation after correcting the copied-stale-observation-record generator defect. |
| Scope | Correct only `New-T95S6DefaultOffX87LinkRoot.ps1` so a freshly copied root removes the inherited observer record, wrapper, log and exit-code evidence in addition to the previously named stale FPU/output artifacts; make a new r4 root and invoke the existing observer exactly once. |
| Non-goals | The prior admission's non-goals remain binding: no opt-in macro, runtime, guest input, configuration/Makefile/device/profile change, full target, configure, or retry in r3. |
| Verification | Parse the generator, prove the new root lacks inherited observer artifacts before the call, then retain the observer's new wrapper/record/log/exit. On a successful link, inspect map/import surface and run boundary/document checks. |
| Risks and stop conditions | Any preflight mismatch or first compiler/link failure is terminal for r4. Do not alter r3, retry r4, or create a further root under this admission. |
| Exit criteria | One new-root link classification, or preserved preflight/compiler/link failure evidence. |

**Result (default-off link closed):** r3 is retained as a pre-`nmake`
generator-record collision and was not reused. The corrected generator made
r4, which preflighted inherited observer-artifact absence. Its one actual
same-island link rebuilt `fpu.o`/`libfpu.a`, produced the companion executable
and map, and the child wrapper recorded exit `0`. The foreground observer
transport timed out only after the child completed, so the root-local wrapper,
log, hashes and recovered JSON record are the terminal evidence; no second
invocation occurred. `etc/research/t95-s6-default-off-x87-companion-link-observation-001.md`
records the hashes and limitations. **Next gate:** a separately admitted new
root may compile with the one diagnostic macro enabled and run the immutable
source-built NTIO input once; it must retain the same reset profile and make no
guest/host state change.

### S6 Opt-In x87 Compatibility-Diagnostic Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Obtain one bounded native x87 state snapshot for the already accepted source-built NTIO first-prefix input, without changing any guest or host input. |
| Inputs | The immutable r2 companion base; current registered `fpu.cc`; exactly the prior source-built NTIO, identity-only NTDOS/COMMAND hashes, PC-reset-floor-v1 runtime profile, ROM pair, child environment fields and `-benchmark 1` stop policy. |
| Scope | Make a fresh r5 companion root. In that root only, append `/DBX_NTVDM_ENABLE_X87_COMPAT_DIAGNOSTIC=1` to the copied FPU Makefile's compile flags; rebuild only `fpu.o`/`libfpu.a` and link the existing companion target once. Then create one fresh evidence root using the existing source-built-NTIO observation tool with a 30-second watchdog. |
| Non-goals | No change to repository/global Bochs configuration, default target, adapter/OpenNT/CLI source, BYOB profile schema or payload bytes, reset profile/ROM/device/CPU setting, host drive/capability, BIOS/interrupt/VDM state, benchmark policy, retry, or any repair based on the result. |
| Evidence | Root manifest must record the explicit macro and all rebuilt hashes; runtime evidence must preserve wrapper, input/output hashes, logs, exit, timing and the first structured x87 record. Classify terminal behavior against the prior observation. |
| Verification | Before run: static macro/source check and root artifact absence. After run: verify exactly the same guest/runtime profile hashes as the prior observation except executable, require one x87 diagnostic line before original `math_abort`, compare stop predicates, then run boundary/document checks. |
| Risks and stop conditions | Missing/multiple diagnostic records, changed inputs/profile, failure to link, watchdog, changed endpoint/plan status, or changed terminal mechanics is terminal evidence. Do not patch, retry or create an input variant. |
| Exit criteria | One reproducible x87-state classification only; no DOS/NTVDM success claim and no implementation change is authorized by the observation. |

**Result (r5 opt-in compile failure):** The one r5 invocation reached the
declared `fpu.cc` compile and stopped with MSVC `C2059` at the new log. Source
review shows the diagnostic passed `BX_CPU_THIS_PTR RIP`, while `RIP` is
already a CPU-access macro; it therefore duplicates the owner prefix only
when the macro is enabled. No archive/link/executable/runtime occurred; r5 is
terminal evidence and must not be reused.

### S6 Corrected Opt-In x87 Compatibility-Diagnostic Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Perform the still-unattempted opt-in diagnostic observation after one source-level macro-access correction. |
| Scope | Change only the diagnostic log operand from `BX_CPU_THIS_PTR RIP` to the existing direct `RIP` CPU-access macro; retain all fields/ordering/guard. Update the focused static test if needed. Make a fresh r6 root with the same root-local FPU compiler opt-in and make one link invocation, then one source-built-NTIO observation only if it links. |
| Non-goals | All r5 non-goals remain binding. No change to diagnostic semantics, fields, original FPU/PIC behavior, profile, guest/host input, or runtime policy; r5 is not modified or retried. |
| Verification | Focused test must prove direct `RIP` use and reject the duplicated prefix. Preserve compiler/link/runtime evidence in separate fresh roots. |
| Risks and stop conditions | A r6 preflight/compiler/link/runtime failure is terminal; no r7 or input variant follows under this admission. |
| Exit criteria | One corrected opt-in build-and-run classification, or retained first failure evidence. |

**Result (x87 state classified, no repair admitted):** r5 is retained as the
first opt-in compiler failure; r6 corrected only the duplicated `RIP` macro
prefix, linked successfully through its root-local wrapper, and then performed
the one immutable-input runtime observation. The run preserved all guest and
runtime profile identities, applied the plan, avoided watchdog, and reached
the same native stop. It emitted 26 snapshots before the original IRQ13 logs;
the first has non-reset `swd=82c1`, `foo=0701`, and nonzero `fip/fdp`.
`etc/research/t95-s6-optin-x87-ntio-prefix-observation-001.md` therefore
rejects reset FPU non-initialization as the direct cause, but does not identify
the reached instruction/owner or authorize any repair. **Next gate:**
read-only source/binary correlation of the observed FPU last-operation values
and first-prefix reachable code; no further run, state injection, interrupt
or host-service change is admitted.

### S6 x87 Last-Operation Address Correlation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Determine whether the observed x87 `fcs:fip`/`fds:fdp` values can belong to the admitted NTIO image at its actual startup placement, without inferring a repair. |
| Scope | Read the r6 log, execution-plan entry implementation, source-built NTIO map/image and OpenNT first-prefix source. Compute only segment:offset physical addresses and compare them to the declared NTIO payload interval. |
| Non-goals | No run/build/link/source/configuration change; no disassembler feature, instruction trace, guest-byte/state change, IVT mutation, interrupt/BIOS/host shim, BOP/DEM service or CPU-profile decision. |
| Evidence | One numbered record with exact segment:offset arithmetic, image-placement facts, map/source anchors, and fact/candidate/rejected classifications. |
| Verification | Re-read every cited map/source/log location; run documentation governance and safe diff checks. |
| Risks and stop conditions | Mark unknown if the plan placement or FPU address units cannot be evidenced. Do not label an absent vector or host component as causal without a reached trace. |
| Exit criteria | An address-domain classification only; it may narrow the next diagnostic question but cannot implement a transition. |

**Result (control-transfer mismatch established):**
`etc/research/t95-s6-x87-last-operation-address-correlation-001.md` computes
the admitted NTIO interval as `[0x700,0x8b00)`. The first snapshot's current
`0000:0590`, last x87 `0000:00fa`, and operand `0000:0d41` all lie outside it;
the current CS is not the admitted entry CS `0x70`. This rejects the claim
that the first observed x87 path remains inside copied NTIO, but leaves the
responsible control transfer/vector owner unknown. **Next gate:** source-only
control-transfer provenance through the first NTIO prefix and the already
admitted initial IVT/state inputs; no extra runtime or state repair is
admitted.

### S6 First-Prefix SVC/BOP Control-Transfer Provenance Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Identify whether the admitted source-built NTIO first prefix contains an evidence-backed OpenNT host transition before NTDOS can load, and classify the current Bochs handling without implementing it. |
| Scope | Read only the preserved NTIO image/map evidence, OpenNT `msinit.asm`, `BOP.INC`, `DOSSVC.INC`, historical SoftPC selection table, pinned Bochs 2.6 decode/exception sources, and the immutable r6 copied exception/build inputs. Record the byte encoding, continuation position, original owner selection, and the compiled #UD-intercept state. |
| Non-goals | No run, build, link, object inspection beyond preserved artifacts, source/configuration/exception change, BOP/SVC handler, NTDOS placement, IVT/BIOS/interrupt repair, guest-byte/state mutation, device enablement, or adapter/Bochs/OpenNT semantic implementation. |
| Evidence | One numbered research record must distinguish source facts, binary-placement facts, historic-owner evidence, current compiled-seam facts, and unsupported causal claims. |
| Verification | Re-read every cited source location; run documentation governance and safe diff checks. |
| Risks and stop conditions | Stop as unresolved if the bytes cannot be tied to a reached source path or if #UD behavior cannot be proven from the copied r6 input. Do not treat a static BOP match as a runtime trace or as authority for a direct handler. |
| Exit criteria | A source-backed statement of the first required host transition and one bounded next design question, with the Bochs/adapter ownership boundary preserved. |

**Result (first missing host transition identified):**
`etc/research/t95-s6-first-prefix-svc-bop-control-transfer-provenance-001.md`
proves that the admitted NTIO prefix reaches `BOP_DOS` / `SVC_DEMLOADDOS`
(`C4 C4 50 11`) before `sysinit`; the preserved image places it at physical
`0xb75`. Historical SoftPC selects `MS_bop_0`, while the r6 copied input leaves
the generic #UD adapter intercept disabled. This is a source-backed missing
startup boundary, not a runtime instruction trace and not authorization for a
direct BOP/DOS handler. **Next gate:** design-only inventory and contract for
one adapter-owned startup transaction using the existing generic default-off
#UD mechanics; it must reject a general BOP/DEM dispatcher before any source
or build admission.

### S6 Existing Startup-Transaction Reuse Design Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Determine whether the existing adapter host-session/service-plane path already expresses the sole required `50:11` startup transaction, and identify the minimum missing same-version integration evidence without adding a second BOP mechanism. |
| Scope | Read-only audit of the adapter host-service catalogue, plane, session, runtime dispatch/take APIs, their unit tests, the registered generic Bochs #UD/write consumer, and retained r12/r6 evidence. Produce one design record mapping the existing interfaces and classifying reuse, missing linkage, and forbidden expansion. |
| Non-goals | No source/configuration/exception-register change; no new ABI, selector, BOP decoder, service table, DOS loader, OpenNT/SoftPC import, build/link/run, device/firmware/IVT/interrupt change, or new guest input. |
| Evidence | The record must identify exact existing functions and their one-use/failure behavior, distinguish r12 synthetic-marker runtime proof from source-built NTIO proof, and name the sole future integration observation without implying it is admitted. |
| Verification | Re-read cited implementation and tests; run documentation governance and safe diff checks. |
| Risks and stop conditions | Stop if reuse would require a second selector parser, more than the existing `50:11` service, raw Bochs/host pointers, or a new Bochs semantic branch. Do not promote a stale UCRT fixture or r6 narrow companion into same-version proof. |
| Exit criteria | A minimal reuse design states whether one existing adapter transaction plus the registered generic #UD/write seam is sufficient, and states the exact preconditions for a separately admitted integration observation. |

**Result (reuse, not replacement):**
`etc/research/t95-s6-startup-transaction-reuse-design-001.md` establishes
that the existing host catalogue, service plane, host session, runtime and
registered generic #UD/write seam already express the sole `50:11` startup
transaction. The host-session test covers exact `C4 C4 50 11`, boundary
binding, one-time take, `DI:0000` destination and `fault_rip + 4` resume. r12
is only synthetic-marker runtime evidence; r6 is only real-NTIO/no-intercept
evidence. **Next gate:** a new admission must freeze a same-version opt-in
object/link closure for exactly those existing components before one fresh
source-built-NTIO integration observation. No new service or boundary API is
needed or admitted.

### S6 Existing Startup-Transaction Same-Version Object-Closure Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Derive the exact existing source/object/archive/link closure needed to enable the already registered generic #UD/write consumer with the existing `50:11` adapter transaction in one fresh companion, without compiling or linking it. |
| Scope | Read only the accepted r6/r12 companion recipes and copied build metadata, current Bochs exception source, existing adapter headers/sources, and original/generated Makefiles. Produce a source-to-object ledger that classifies every member as required, inherited, default-but-unneeded, or rejected. |
| Non-goals | No compiler/linker/archive/tool invocation; no build root; no source/configuration/exception-register/API change; no new object, service, adapter runtime feature, BOP parser, OpenNT/SoftPC import, guest execution, device/firmware change, or r6/r12 reuse. |
| Evidence | The ledger must show why every listed component is reached, prove the opt-in macros needed at the two existing Bochs seams, name the exact expected artifact boundary, and reject broad runtime dependencies that are not needed by the `50:11` path. |
| Verification | Re-read every source/recipe anchor; run documentation governance and safe diff checks. |
| Risks and stop conditions | Stop if the closure needs another Bochs semantic edit, a full product archive, a second adapter path, an unclassified generated configuration input, or an unknown object provider. A future build cannot be admitted from a partial ledger. |
| Exit criteria | One reproducible object/link-closure ledger identifies the sole possible fresh companion composition and the exact preflight predicates for a later single build invocation. |

**Result (current broad runtime rejected for this companion):**
`etc/research/t95-s6-startup-transaction-object-closure-ledger-001.md` shows
that r6's narrow startup-plan endpoint and the current broad #UD runtime are
not a valid minimal pair: `adapter_runtime_v4` retains unrelated read,
observation, multi-write, debug, drive, DPB and COMMAND paths. The required
`50:11` session functions also share `bx_ntvdm_host_session.c` with those
other operation kinds. **Next gate:** before any build, register and implement
an adapter-only source-object separation plus one default-off generic Bochs
#UD/write consumer that calls only an adapter startup bridge. Both must reuse
the existing `50:11` transaction and have focused boundary tests; no new BOP
or service semantics are admitted.

### S6 Narrow Startup-Transaction Bridge and #UD/Write Mechanics Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Materialize the already designed narrow first-startup composition: one adapter-local bridge that owns only immutable startup resources and the existing `50:11` transaction, plus one default-off Bochs #UD/write mechanics block that consumes only its typed result. |
| Scope | Add adapter-local bridge header/source and focused C11 test. It may reuse the existing startup-session, host-service catalogue and host-service-plane contracts, but must not call `adapter_runtime`, `host_session`, or any secondary service. Register `BX-ABI-036` before changing `refs/bochs/cpu/exception.cc`; under one local default-off macro it may capture the existing generic #UD state/window, call the bridge, take one opaque pending write, use the existing ordinary-RAM writer, and apply a checked resume. In the existing execution-plan opt-in, `main.cc` may replace only its narrow startup-session environment endpoint with the bridge's plan endpoint. |
| Non-goals | No new BOP selector/service/catalogue entry; no OpenNT/SoftPC import; no DOS/DEM/COMMAND/filesystem/drive/debug/DPB/read/gather/observation/multi-write behavior; no CPU decode/exception-class/memory/device/firmware/IVT/FPU change; no build/link/run or build-root creation. Existing broad interception remains unchanged and disabled unless its own macro is selected. |
| Evidence | Register exact Bochs exception scope and static negative cases. The bridge test must prove only exact `50:11`, real-mode `DI:0000` preflight, one-time matching take and `fault+4` resume; it must reject all other service/operation paths. A PowerShell boundary test must prove the new Bochs block is default-off, generic and isolated from the broad runtime block. |
| Verification | Run the bridge C11 test, new focused Bochs boundary test, retained default-off/startup-session/#UD tests, documentation governance, and safe diff checks. |
| Risks and stop conditions | Stop if the bridge needs a second catalogue, uses host/Bochs pointers across the ABI, exposes a general service range, needs a special Bochs address/byte/service branch, or changes original default #UD behavior. Do not compile/link/run as part of this admission. |
| Exit criteria | The source boundary is statically proven: the default build path remains adapter-free; the new opt-in has only generic #UD/write/resume mechanics; the adapter bridge contains the sole admitted startup transaction; and broad runtime paths are absent. |

**Progress (adapter endpoint only):**
`bx_ntvdm_startup_transaction_bridge` now owns one process-local immutable
startup session, reuses the existing catalogue and `50:11` service-plane
preflight, and exposes only plan preparation, typed dispatch, and a matching
one-time pending-write take. It does not reference `adapter_runtime`,
`host_session`, read/gather/observation/multi-write or secondary-service
paths; `Test-StartupTransactionBridgeBoundary.ps1` passes. The bridge is not
yet connected to Bochs, and no compile/link/run claim is made. The remaining
admitted work is the registered `BX-ABI-036` default-off mechanics block and
its focused static test.

**Result (source boundary complete; no build claim):** `BX-ABI-036` is now
implemented in `cpu/exception.cc` under
`BX_NTVDM_ENABLE_STARTUP_TRANSACTION=0` by default. Its opt-in block carries
only copied generic #UD state/window, bridge dispatch/take-write, ordinary-RAM
write and checked resume. `main.cc` uses the same bridge for its existing
execution-plan endpoint. `Test-BochsStartupTransactionBoundary.ps1`, retained
default-off/#UD tests and the bridge boundary test pass. No compiler, linker,
build root, executable or guest run was invoked. **Next gate:** freeze the
exact adapter/Bochs object list and a copied-root macro-only opt-in before one
separately admitted same-version build observation.

### S6 Narrow Startup-Transaction Companion Closure Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Freeze the exact source/object/include/macro closure of the implemented narrow startup-transaction companion before any compiler, linker, build-root or guest observation is allowed. |
| Scope | Read only current bridge/Bochs source, r6 companion metadata/shim, original Makefiles and existing static tests. Produce a numbered closure ledger and, if necessary, one source-only preflight test that rejects unexpected sources or macros. |
| Non-goals | No compiler/linker/archive/build-root invocation; no source/configuration/API/exception-register change; no broad runtime, host-session, secondary service, OpenNT/SoftPC/product object, device/firmware action, or guest run. |
| Evidence | List every required source/object and its direct source edge; list every inherited archive; distinguish root-local opt-in macros from source defaults; reject every unneeded adapter source by name. |
| Verification | Re-read cited inputs; run the closure preflight/static boundary checks, documentation governance and safe diff checks. |
| Risks and stop conditions | Stop if any required object has an unknown provider, if a source-only list cannot represent the bridge cleanly, or if the copied-root macro definition would select both broad and narrow paths. No future build may repair such a finding. |
| Exit criteria | A complete no-build closure record names one exact future copied-root target composition and one independent build admission predicate. |

**Result (candidate closure frozen):**
`etc/research/t95-s6-narrow-startup-companion-closure-001.md` freezes the
required bridge/session/transaction objects, rejects `adapter_runtime` and
`host_session`, and requires only copied-root `main.o` and `cpu/exception.o`
to opt into execution-plan plus startup-transaction macros. The old broad
intercept remains undefined. The r6 shim supplies order only and is not reused.
No build was invoked. **Next gate:** a separate fresh-root generator admission
must mechanically materialize and validate this exact list before one build.

### S6 Narrow Startup-Transaction Fresh-Root Generator Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Add and exercise once a refusal-first generator that materializes the frozen narrow companion in a new copied root without compiling or linking. |
| Scope | The generator may copy the immutable r6 base root to one fresh path, replace only copied `main.cc` and `cpu/exception.cc`, copy only the closure-ledger adapter/CLI sources and headers, and emit a root-local shim with exactly the frozen object list and two opt-in macros. |
| Non-goals | No compiler/linker/archive/executable invocation; no existing-root modification, retry, source/config/API change, broad interceptor opt-in, r6 reuse, device/firmware/guest action, or additional source selection. |
| Evidence | Fresh-root collision refusal, input/output hashes, macro/object list, explicit rejected-token scan, and a manifest with no build command. |
| Verification | Static generator review and one generation only; documentation governance and safe diff checks. |
| Risks and stop conditions | Stop if r6 lacks an immutable input, any required header cannot be copied without broad dependencies, or the generated shim contains an unapproved object/macro. Do not repair by widening the list. |
| Exit criteria | One validated fresh root exists solely as a possible future build input; an actual `nmake` remains separately unadmitted. |

**Result (generator-only closure complete, 2026-08-11):**
`tools/build/New-T95S6NarrowStartupTransactionBuildRoot.ps1` created exactly one
fresh root at
`artifacts/build/bochs-2.6-native-startup-transaction-msvc-r1`. It copied the
immutable r6 input, replaced only copied `main.cc` and `cpu/exception.cc`,
removed their copied stale objects plus the stale copied `cpu/libcpu.a`, and
copied only the 16 frozen adapter/CLI translation units (with local headers).
The generated shim contains exactly the frozen 16-object list and exactly two
occurrences each of `BX_NTVDM_ENABLE_EXECUTION_PLAN=1` and
`BX_NTVDM_ENABLE_STARTUP_TRANSACTION=1`; it contains neither the broad
intercept macro nor the rejected runtime, host-session, OpenNT or SoftPC
tokens. The manifest has `buildCommand: null`; its SHA-256 is
`EA6D9B6AB57AB8ACD2499BE5568FA66C783CD26B3DEBB44369FA548DA9E882B2`, and the
shim SHA-256 is `D60ED187AE2EF23616A949233DC17729566A5B4D0A55CE8A4B99C282BE6EE8B8`.
A second invocation against that root refused overwrite before mutation.
Focused bridge, Bochs startup-transaction and default-off #UD boundary tests,
documentation governance, and safe diff checks pass. No `cl`, `lib`, `link`,
`nmake`, executable, guest, or firmware invocation occurred. **Next gate:** a
separate S6 admission is required before one actual fresh-root build attempt.

### S6 Narrow Startup-Transaction Single-Build Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Classify the frozen narrow startup-transaction companion as buildable, externally blocked, or rejected with one MSVC/x86 invocation in its already generated fresh root. |
| Scope | After preflight verifies the recorded r1 manifest/shim hashes, the exact BuildTools x86 environment may invoke only `nmake.exe /f ntdos64-native-startup-transaction.mak ntdos64-native-startup-transaction.exe` from `artifacts/build/bochs-2.6-native-startup-transaction-msvc-r1`. The command may create only that root's declared objects, `cpu/libcpu.a`, executable, map, and one retained log/exit record. |
| Non-goals | No `all`, `bochs.exe`, VS solution, configure/configuration edit, fresh-root regeneration, retry, source/API change, macro/device addition, guest/firmware execution, or service/search claim. The broad interceptor stays undefined. |
| Required evidence | Preflight hashes equal the generator record; exact command/environment and one exit code; complete build log; output/map existence and hashes only if produced; static macro/object rejection still holds. |
| Stop conditions | Any nonzero setup or `nmake` exit, unexpected source/object/macro, stale manifest mismatch, timeout, or missing evidence record ends the package immediately. Do not repair or retry in this root. |
| Exit criteria | One preserved outcome classifies the declared same-version companion build only. A successful link is not a guest-runtime, DOS, filesystem, search, or CLI product claim. |

**Result (r1 link closure rejected, 2026-08-11):** The one admitted BuildTools
x86 invocation reached the final link and exited `2`; r1 is preserved and
will not be retried. `etc/research/t95-s6-narrow-startup-transaction-build-attempt-001.md`
records the exact log and exit hashes. The complete failure is two unresolved
generic ABI helpers: `bx_ntvdm_cpu_delta_v1_initialize` is provided by
`adapter/bx_ntvdm_cpu_delta_abi.obj` to `exception_abi`, and
`bx_ntvdm_cpu_result_v2_resume` is provided by
`adapter/bx_ntvdm_cpu_result_v2.obj` to the narrow Bochs result application.
This rejects the frozen r1 object list only; it is neither a Bochs mechanical
failure nor authority to enable a device, product target, OpenNT/SoftPC object,
or broad adapter service. **Next gate:** separately admit a two-object ledger
and generator correction, retain all rejected-object scans, then use a new
fresh root for at most one new invocation.

### S6 Narrow Startup-Transaction Two-Object Closure Correction Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Correct the r1-proven incomplete object closure with exactly the two direct generic ABI providers required by the selected narrow result records. |
| Scope | Read `bx_ntvdm_cpu_delta_abi.c` and `bx_ntvdm_cpu_result_v2.c` with their direct consumers; add a successor closure ledger; update only the fresh-root generator's declared source/object lists and static count/presence checks. |
| Non-goals | No Bochs/OpenNT/CLI source change, no adapter semantic/API change, no broad runtime/host-session/service source, no device/configuration change, no generator execution, compiler, archive, link, executable or guest action. |
| Required evidence | Direct source provider/consumer table, fixed-width/no-pointer boundary review, frozen list delta of exactly two sources/objects, and retained rejection scan. |
| Verification | Static source/list review, existing bridge and Bochs boundary tests, documentation governance, and safe diff checks. |
| Stop conditions | Stop if either provider requires any further object, introduces a host/guest pointer or service policy, or a generated list selects a rejected object. A new r2 root/build requires a later admission. |
| Exit criteria | A successor ledger and generator name precisely 18 adapter/CLI objects, including only the two evidenced additions, while r1 remains unchanged. |

**Result (two-object correction complete):**
`etc/research/t95-s6-narrow-startup-companion-closure-002.md` records the
direct provider/consumer proof and supersedes closure 001 only for
`bx_ntvdm_cpu_delta_abi.obj` and `bx_ntvdm_cpu_result_v2.obj`.
`New-T95S6NarrowStartupTransactionBuildRoot.ps1` now locks exactly 18 objects
and fails if either provider is absent. Static source/list review plus the
bridge, narrow Bochs, default-off #UD, governance and safe diff checks pass;
no generator/build tool ran. r1 remains unchanged. **Next gate:** a separately
admitted r2 generator run may materialize this corrected list once.

### S6 Narrow Startup-Transaction Corrected Fresh-Root Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Materialize the corrected 18-object closure once in a new r2 copied root, without compiling or linking. |
| Scope | Invoke the refusal-first generator once with the immutable r6 root and the new path `artifacts/build/bochs-2.6-native-startup-transaction-msvc-r2`; inspect its manifest/shim, object count, macros, hashes, and rejected-token scan. |
| Non-goals | No r1 mutation/retry, compiler/archive/link/executable invocation, source/API change, device/configuration addition, broad interceptor, host-service widening, guest or firmware action. |
| Required evidence | Collision refusal, r6 input hashes, exactly 18 source/object entries, both opt-in macros exactly twice, broad intercept absent, rejected tokens absent, and a manifest with no build command. |
| Verification | One generator run, one collision refusal probe, static checks, documentation governance, and safe diff checks. |
| Stop conditions | Stop if any source/hash/list/macro/rejection condition differs. Do not repair by selecting another object or by changing r1/r2 after generation. |
| Exit criteria | r2 is a preserved possible future build input only; a second actual build still needs a separate admission. |

**Result (corrected r2 generator complete):** The generator created exactly one
new root at `artifacts/build/bochs-2.6-native-startup-transaction-msvc-r2`.
Its manifest SHA-256 is
`FE532988788694CE86C4C7102620EE67EDF7CB4620AE3F58D260E39C4D3D54E0` and its
shim SHA-256 is
`C2B8DB745BBB72353BB637066601D1F9DF1C37E50C21F23CA4AF91BE00E16467`.
It has exactly 18 closure objects, no build command, exactly two occurrences
of each opt-in macro, no broad intercept or rejected token, and rejected a
second overwrite attempt before mutation. No build tool ran. **Next gate:** a
separate r2 single-build admission.

### S6 Narrow Startup-Transaction Corrected Single-Build Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Run one and only one corrected 18-object MSVC/x86 companion build in r2 and classify its terminal outcome. |
| Scope | Verify r2's frozen manifest/shim hashes, then invoke only `nmake.exe /f ntdos64-native-startup-transaction.mak ntdos64-native-startup-transaction.exe` from r2 under the recorded BuildTools x86 environment. Retain one log and exit record. |
| Non-goals | No r1 action, retry, `all`, `bochs.exe`, VS solution, configure/configuration/source/API change, device/feature addition, guest/firmware run, or host-service widening. |
| Required evidence | Exact command/environment, preflight hashes, one exit code/log, object/output/map observations and hashes where present, and a classification of every unresolved symbol if the link fails. |
| Verification | Static macro/object scan before invocation; terminal log inspection after it; documentation governance and safe diff checks. |
| Stop conditions | Any nonzero exit or unexpected selection is terminal for r2. Do not repair or retry within r2; any follow-up must be separately admitted in a new root. |
| Exit criteria | r2 produces one preserved build classification only; even a successful link does not establish execution or product-runtime evidence. |

**Result (r2 same-island link available):** The one r2 invocation exited `0`
and produced the 2,500,608-byte
`ntdos64-native-startup-transaction.exe`, its map, and rebuilt CPU archive.
`etc/research/t95-s6-narrow-startup-transaction-build-attempt-002.md` retains
the exact command context, output sizes and hashes. The final shim has none of
the broad interceptor, adapter runtime, host-session, read/gather/observation/
multi-write, OpenNT, or SoftPC tokens. This establishes only a linked
MSVC/x86 Bochs/adapter companion; it is **not** reset, controlled execution,
#UD, NTIO, DOS, guest, filesystem, CLI, or runnable-NTVDM evidence. **Next
gate:** separately derive and admit one bounded native lifecycle
reset-to-controlled-stop fixture before any executable launch.

### S6 r2 Native Reset-to-Controlled-Stop Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Establish the first runtime fact about the linked r2 companion: original native PC reset reaches one finite benchmark stop with no adapter or guest input. |
| Scope | Use the existing refusal-first `Invoke-T95S6NativeResetControlledStopObservation.ps1` once, with r2, `ntdos64-native-startup-transaction.exe`, `pc-reset-floor-v1`, a new evidence root, and its fixed `-benchmark 1` command. The observer clears all adapter/BYOB environment variables. |
| Non-goals | No NTDOS payload, execution plan, #UD transaction, host-service call/write, DOS/filesystem/search/CLI behavior, new fixture source, configuration/build/source change, retry, device enablement, or unbounded execution. |
| Required evidence | New-root collision refusal, binary/ROM/profile hashes, command, cleared environment list, watchdog result, exit code, original benchmark marker, controlled-stop marker, and absence of adapter/guest terms. |
| Verification | One observation only; read its JSON/logs; run documentation governance and safe diff checks. |
| Stop conditions | Any watchdog termination, missing marker, forbidden adapter/guest term, unexpected exit, or evidence-root collision terminates this observation. No rerun in the same root. |
| Exit criteria | A bounded mechanics-only r2 reset/stop result is classified independently from every guest/adapter runtime claim. |

**Result (r2 native reset/stop available):** The single r2 observation is
preserved at
`artifacts/analysis/t95-s6-r2-native-reset-stop-observation-001-20260811-001`.
It records 3,271 ms elapsed, no watchdog termination, native nogui exit `1`,
`Reset(HARDWARE)`, benchmark mode, and controlled stop at tick `10,000,005`.
All adapter/BYOB environment variables were cleared; the command had no guest
or adapter input. `etc/research/t95-s6-r2-native-reset-controlled-stop-001.md`
records hashes and interpretation. This proves only reset → finite budget →
controlled stop, not execution-plan, #UD, NTIO, DOS, host capability, CLI, or
runnable NTVDM behavior. **Next gate:** separately derive a real-NTIO
startup-plan/transaction fixture; do not reuse this no-adapter result as guest
evidence.

### S6 Narrow Transaction Mechanical-Confirmation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Make the already admitted narrow #UD/write/resume path externally observable without adding OpenNT or service semantics to Bochs. |
| Scope | Amend the existing `BX-ABI-036` register before source change. Under its existing default-off macro, add exactly one success-only Bochs log after the already checked ordinary-RAM write and resume result are established; it may emit only generic write address, byte count and resume RIP. Strengthen the focused static boundary test. |
| Non-goals | No selector/service/marker-byte decode or logging, no guest image name, DOS/BOP/DEM/OpenNT/CLI/host-path term, no new callback/ABI/address policy, no lifecycle/decode/device/firmware change, build, root generation, or runtime observation. |
| Required evidence | Exception-register amendment, source diff showing log placement after the generic write checks, default-off preservation, and negative test rejection of semantic terms. |
| Verification | Focused startup-transaction/default-off/generic-#UD tests, documentation governance, and safe diff checks. |
| Stop conditions | Stop if confirmation needs any semantic field, a second Bochs hook, a bridge/API change, or cannot remain default-off. A rebuilt binary and real-NTIO run require later admissions. |
| Exit criteria | A future same-version binary can prove one generic write/resume commitment without making Bochs a BOP/DOS dispatcher. |

**Result (mechanical confirmation source boundary complete):** `BX-ABI-036`
now permits one success-only default-off log after the pre-existing generic
write and resume checks. `cpu/exception.cc` emits only `guest transaction
committed` with generic write address, byte count and resume RIP; it has no
selector, service, instruction bytes, guest image, DOS/BOP/DEM/OpenNT/CLI or
host-path term. The first test correctly rejected the accidental `ntdos64`
label because it contains `DOS`; the final neutral label passes the focused
startup-transaction, default-off #UD, generic #UD, bridge, governance and
safe diff checks. No build, root generation or runtime run occurred. **Next
gate:** one new fresh-root rebuild is separately required before this
confirmation can be observed.

### S6 r3 Mechanical-Confirmation Rebuild Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Rebuild the otherwise identical corrected 18-object companion once so the registered neutral transaction-confirmation log is present in a runnable binary. |
| Scope | Generate one fresh r3 root with the existing refusal-first generator; verify the same 18 objects/rejected scans and current `exception.cc` hash; then invoke the same single named target once under BuildTools x86, retaining logs and hashes. |
| Non-goals | No object-list/source/API expansion beyond the registered neutral log, no r1/r2 retry/mutation, `all`, `bochs.exe`, configure, device change, guest or host-service runtime, or CLI claim. |
| Required evidence | New-root refusal, source/shim/manifest hashes, exactly 18 objects and two macros, one build command/exit/log, output/map hashes, and no rejected token. |
| Verification | Generator static checks, one collision probe, one named target invocation, terminal artifact inspection, documentation governance, and safe diff checks. |
| Stop conditions | Any mismatch or nonzero build is terminal for r3; do not repair/retry in r3. Any NTIO observation is separately admitted. |
| Exit criteria | A linked r3 binary differs from r2 only as documented and is available for a future controlled real-NTIO observation. |

**Result (r3 linked with neutral confirmation):** The one r3 target exited `0`.
The executable SHA-256 is
`ABB7FD816A1FC11E90B8C9EBCED9D12DDAF26BD08658C9C654BC71C5EB9E7972`; its map
SHA-256 is `44839525BCEA38EF4E59E36230E46FEDCF460A1120CCF5009DC50D1936C867BF`.
The retained build-log SHA-256 is
`1EACEB206D941626FAC98DA40F1122902336B2E1E6051C7AF237E21D7074DACE`.
r3 was generated once and rejected overwrite before its one build; r1/r2 were
not changed. **Next gate:** separately admit a dedicated r3 real-NTIO
startup-transaction observer with only immutable BYOB inputs and explicit
generic confirmation predicates.

### S6 r3 Dedicated Real-NTIO Startup-Transaction Observer Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Observe once whether r3 applies the exact source-built NTIO plan and commits its one existing generic write/resume transaction. |
| Scope | Add one refusal-first PowerShell observer under `tools/`. It may copy only the exact source-built NTIO, NTDOS and COMMAND artifacts plus r3 ROM inputs into a new evidence root; write their immutable BYOB profile and the existing PC reset-floor profile; set only `NTDOS64_ADAPTER_PROFILE` and `NTDOS64_ADAPTER_ROOT`; invoke r3 once with `-benchmark 1`; and record plan, generic commit and controlled-stop markers. |
| Non-goals | No Bochs/adapter/CLI/OpenNT source change, no alternate fixture mode, synthetic ROM, BOP/service parsing in the observer, host-drive exposure, DOS/filesystem behavior, r3 rebuild/retry, device/configuration expansion, or unbounded run. |
| Required evidence | Fresh-root refusal, source/component/ROM/profile hashes, exact command and environment names, one exit/log, `requested execution plan applied`, one generic `guest transaction committed` record, benchmark/controlled-stop markers, and absence of a broad-interceptor marker. |
| Verification | Static tool review, one observation and collision refusal, source/hash checks, documentation governance, and safe diff checks. |
| Stop conditions | Missing input/hash/marker, unexpected second generic commit, broad-interceptor marker, watchdog, or unexpected exit ends the observation. Do not retry in that evidence root. |
| Exit criteria | One evidence-backed classification of the first source-built NTIO `#UD → generic write → resume` slice, explicitly short of DOS runtime or CLI product completion. |

**Result (observer prelaunch rejection):** The first evidence root
`artifacts/analysis/t95-s6-r3-real-ntio-startup-transaction-001-20260811-001`
is preserved without retry. This desktop PowerShell supplies a null
`ProcessStartInfo.EnvironmentVariables` collection; the observer failed while
setting its environment, before process creation. The root contains only its
copied/hashes inputs and profile, with no stdout, stderr, log, executable
launch or guest execution. **Next gate:** separately correct only the observer
environment transport to use a scoped process-environment set/restore, then
use a new evidence root once.

### S6 r3 Observer Environment-Transport Correction Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Repair the observer's prelaunch environment transport without changing guest, adapter, Bochs or fixture semantics. |
| Scope | Replace the unsupported `ProcessStartInfo.EnvironmentVariables` writes with temporary process-environment assignments for the same four named variables, restoring their exact prior values in `finally` after process completion or failure. Update static checks. |
| Non-goals | No process launch in the rejected root, no input/profile/command/predicate change, no emulator/adapter/guest source change, no new environment name, no rebuild, or runtime claim. |
| Required evidence | Reproduced null-collection fact, exact four-name set/restore list, and static proof of restoration. |
| Verification | Parser/static boundary checks, governance and safe diff checks only. A new observation remains separately admitted. |
| Stop conditions | Stop if scoped restoration cannot be guaranteed or an extra environment capability is needed. |
| Exit criteria | The observer can carry the same immutable profile/root names through a portable, scoped transport without an executable launch. |

**Result (environment-transport correction complete):** The observer now sets
and restores exactly `NTDOS64_UD_TEST`, `NTDOS64_ADAPTER_BRIDGE`,
`NTDOS64_ADAPTER_PROFILE`, and `NTDOS64_ADAPTER_ROOT` in a `try`/`finally`
scope, avoiding the desktop PowerShell null `ProcessStartInfo` environment
collection. Parser/refusal, scoped-transport, governance and safe diff checks
pass; no executable was launched. **Next gate:** one new-root observation with
the unchanged command/input/predicate set.

### S6 r3 Real-NTIO Startup-Transaction Observation Retry Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Execute the already admitted r3 real-NTIO observation once through the corrected environment transport. |
| Scope | Invoke only the corrected observer with the unchanged r3 build/input set and one fresh evidence root. |
| Non-goals | No retry of the rejected root, rebuild, source/profile/command/predicate expansion, emulator/adapter/guest change, or unbounded execution. |
| Required evidence | Same required evidence as the original observer admission, plus record of scoped environment restoration. |
| Verification | One invocation, collision refusal, retained JSON/log inspection, governance and safe diff checks. |
| Stop conditions | Any terminal predicate failure ends this new root; no retry there. |
| Exit criteria | One classified r3 source-built NTIO startup-transaction observation. |

**Result (002 transport-timeout rejection):** The corrected environment
transport allowed process setup, but the observer synchronously drained stdout
before stderr; the outer 60-second watchdog timed out before it could reach
its internal wait/record path. The preserved 002 root has only copied inputs
and no result/log; a process audit found no remaining r3 child. It is not
retried. **Next gate:** separately replace only the observer's pipe transport
with a file-redirection wrapper and watchdog, preserving its command, inputs,
environment names and predicates.

### S6 r3 Observer Output-Transport Correction Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Eliminate observer pipe deadlock without changing the r3 execution input or meaning. |
| Scope | Replace in-process sequential stdout/stderr reads with one root-local wrapper that redirects both streams to existing evidence files; launch it hidden, wait under the existing watchdog, read its atomic exit record, and retain the same predicate scan. |
| Non-goals | No command/input/profile/environment/predicate change, no guest/adapter/Bochs source change, no build, no new external process beyond the same named r3 binary, and no retry in 002. |
| Required evidence | Static wrapper shape, exact redirections, atomic exit record, process identity/watchdog behavior, and existing restoration proof. |
| Verification | Parser/static check, governance and safe diff checks only. A new evidence root is separately required for execution. |
| Stop conditions | Stop if the wrapper cannot retain an exit code or process identity, or if it changes the named command/environment. |
| Exit criteria | The observer has a bounded non-pipe transport suitable for one future new-root invocation. |

**Result (output-transport correction complete):** The observer now emits one
root-local `cmd` wrapper that redirects the unchanged binary command to
`stdout.log`/`stderr.log`, writes `exitcode.txt`, and exposes PID/watchdog
state. It no longer calls `ReadToEnd`; static parser, wrapper-shape,
governance and safe diff checks pass. No r3 process was launched. **Next
gate:** one fresh-root observation using this unchanged observer.

### S6 r3 Real-NTIO Startup-Transaction File-Transport Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Execute one final r3 source-built NTIO startup-transaction observation with the corrected bounded file transport. |
| Scope | Invoke the same observer once against r3 and one new evidence root. |
| Non-goals | No retry of 001/002, no build/source/profile/command/environment/predicate change, no unbounded process, and no DOS/CLI product claim. |
| Required evidence | Existing observation predicates plus wrapper hash/content, PID, exit record and stdout/stderr hashes. |
| Verification | One invocation, collision refusal, retained evidence inspection, governance and safe diff checks. |
| Stop conditions | Any predicate failure is terminal for the new root. |
| Exit criteria | A conclusive success or failure classification of r3's first real-NTIO startup transaction. |

**Result (003 watchdog-tree rejection):** The file-transport wrapper avoided
pipe deadlock but its 30-second watchdog terminated only the wrapper, not its
r3 child. The outer observer was halted after 60 seconds; a path-verified r3
child PID `18172` remained and was explicitly terminated, after which a
read-only audit found no remaining process. The 003 evidence root is retained
and is not retried. This is not a guest/adapter result. **Next gate:** before
any new real-NTIO process, replace the observer's wrapper start/watchdog with
one mechanism that records and kills the complete process tree, then verify
that transport without using guest input.

### S6 r3 Observer Process-Tree Watchdog Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Bound the existing observer's wrapper and all descendants with an auditable Windows process-tree termination path. |
| Scope | Under the existing timeout branch only, invoke `taskkill /T /F /PID <verified wrapper PID>`, wait for the wrapper, record the command/PID/result, and fail closed if the tree-kill command does not succeed. Keep normal successful completion untouched. |
| Non-goals | No guest input/process launch during this correction, no binary/profile/environment/predicate change, no adapter/Bochs/OpenNT/CLI source change, no arbitrary process selection, and no retry of prior roots. |
| Required evidence | Exact wrapper PID validation, literal tree-kill command construction, timeout-only guard, process-tree result record, and static rejection of broad process targeting. |
| Verification | Static/parser review, governance and safe diff checks. A new real-NTIO observation still needs a separate admission. |
| Stop conditions | Stop if the observer cannot prove its own wrapper PID or if termination would require a broad name/path kill. |
| Exit criteria | A future timeout can terminate only the observer-owned process tree and leave retained terminal evidence. |

**Result (process-tree watchdog correction complete):** The timeout branch now
verifies the started wrapper PID and invokes only `taskkill.exe /T /F /PID
<that PID>`. It records command, PID, attempted/result fields and writes JSON
even when wrapper files are incomplete. Static/parser checks reject broad
image/path targeting; governance and safe diff checks pass. No process was
launched. **Next gate:** one new-root real-NTIO observation under the existing
unchanged command/input/predicate set.

### S6 r3 Real-NTIO Startup-Transaction Tree-Watchdog Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Run one bounded r3 real-NTIO observation with the verified process-tree watchdog. |
| Scope | Invoke only the corrected observer once in a new evidence root. |
| Non-goals | No retry of prior roots, rebuild, source/profile/command/environment/predicate change, device enablement, host capability or DOS/CLI product claim. |
| Required evidence | Existing markers/hashes plus watchdog tree fields and post-run no-child audit. |
| Verification | One invocation, collision refusal, JSON/log/tree audit, governance and safe diff checks. |
| Stop conditions | Any timeout/tree kill/predicate failure is terminal for this root; no retry there. |
| Exit criteria | A conclusive success/failure classification for the first r3 real-NTIO startup transaction under bounded transport. |

**Result (004 process-tree transport rejection):** The wrapper-PID tree-kill
implementation again did not return control within the outer 60-second limit;
after caller termination, a path-verified r3 child PID `39572` remained. It
was explicitly terminated and a follow-up audit confirmed it had exited. The
004 root is retained and not retried. This repeats the host-process-control
failure, not a Bochs/adapter/guest outcome. **Next gate:** do not start another
guest process through this PowerShell-wrapper transport. Derive a separate
process-supervision mechanism whose ownership and descendant termination can
be verified without a guest input first.

### S6 r3 Direct-Process Supervisor Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Replace unreliable wrapper-tree supervision with direct ownership of the sole r3 binary process. |
| Scope | Remove the wrapper from the observer. Start `ntdos64-native-startup-transaction.exe` directly with the unchanged profile/benchmark plus Bochs's own `-log` file in the evidence root; wait on that exact PID and, on timeout, terminate that exact process only. Read markers from the retained Bochs log. |
| Non-goals | No guest process launch in this correction, no profile/input/environment/benchmark/predicate change, no source/build/backend change, no generic process-name/path kill, and no retry of 001--004. |
| Required evidence | Direct binary PID identity, exact `-log` argument, timeout-only exact-PID termination, and static rejection of wrapper/taskkill/stream-pipe use. |
| Verification | Parser/static review, governance and safe diff checks. Any real-NTIO run remains separately admitted. |
| Stop conditions | Stop if direct Bochs logging cannot retain markers or exact-PID supervision cannot be established without a wrapper. |
| Exit criteria | The observer has a single-process bounded transport with no child-tree ownership ambiguity. |

**Result (direct-process supervisor complete):** The observer now starts r3
directly with `-log bochs.log`, owns its single PID, reads markers only from
that Bochs log, and uses timeout-only `$process.Kill()` on that exact PID.
Wrapper, `taskkill`, stream-pipe and process-name/path targeting are absent.
Static/parser, governance and safe diff checks pass; no guest process launched.
**Next gate:** one fresh-root direct-process real-NTIO observation.

### S6 r3 Direct-Process Real-NTIO Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Obtain one conclusive, bounded real-NTIO startup-transaction observation through direct r3 process ownership. |
| Scope | Invoke only the corrected direct-process observer once in a new evidence root with unchanged BYOB/profile/benchmark/predicates. |
| Non-goals | No retry of prior roots, rebuild, code/input/profile/environment/predicate change, device feature, host capability, DOS/filesystem/search/CLI product claim. |
| Required evidence | Component/ROM/profile/binary hashes; direct PID, direct-kill flag if timed out, Bochs log hash, plan/commit/benchmark/stop marker results, no broad-interceptor marker, and collision refusal. |
| Verification | One run only, evidence/root process audit, governance and safe diff checks. |
| Stop conditions | Any timeout/direct kill, absent marker, broad-interceptor marker or unexpected exit is terminal for the new root. |
| Exit criteria | One conclusive success/failure classification of r3 first real-NTIO startup transaction, not a DOS/CLI product claim. |

**Result (first real-NTIO startup transaction proven; post-resume stop):**
The one direct-process r3 observation recorded `requested execution plan
applied` and exactly one narrow generic commit:
`address=8b00 bytes=6cd2 resume=479`, with no broad-interceptor marker. This
proves source-built NTIO plan loading, `#UD → generic write → checked resume`
and the NTDOS write; it does not prove DOS runtime. Afterward it repeatedly
hit `EIP [00010000] > CS.limit [0000ffff]` and a prefetch direct-read panic,
then the watchdog killed its exact PID with no residual child. The retained
evidence and hashes are in
`etc/research/t95-s6-r3-real-ntio-startup-transaction-005.md`. **Next gate:**
read-only CS:EIP/provenance analysis; no benchmark/device/service widening or
repair is admitted.

### S6 Narrow Transaction Fault-State Confirmation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Determine whether the single admitted transaction entered its generic #UD boundary with the expected real-mode CS:EIP, before considering any execution-state repair. |
| Scope | Amend `BX-ABI-036` before source change. Extend only its existing default-off success log with the already copied fault `CS` and `EIP` fields from the generic exception request; update the focused static test. |
| Non-goals | No new event/state capture, guest-memory read, selector/service/instruction-byte/image/DOS/BOP/DEM term, CPU/segment/result mutation, adapter API, lifecycle/device/firmware change, build or run. |
| Required evidence | Register amendment; log fields visibly sourced from existing copied request state; default-off/semantic-negative test; no control-flow/data mutation. |
| Verification | Startup-transaction/default-off/generic-#UD boundary tests, governance and safe diff checks. |
| Stop conditions | Stop if the diagnosis requires a new CPU hook, raw Bochs state beyond the existing request, any semantic marker, or a changed result. |
| Exit criteria | A future fresh binary can distinguish a wrong pre-fault code state from an error introduced after the transaction. |

**Result (fault-state confirmation source boundary complete):** `BX-ABI-036`
now permits its existing default-off post-commit log to include only the
already copied request `CS:EIP`, ahead of the existing generic address, byte
count and resume-RIP fields. The log does not inspect raw CPU state, decode a
selector/service/instruction, alter a result, or change control flow.
`Test-BochsStartupTransactionBoundary.ps1`, retained #UD/default-off tests,
the bridge boundary test, governance verification and safe diff checks pass.
No build or run occurred. **Next gate:** separately admit one fresh copied-root
build that changes only this exception object and then, if that build closes,
one fresh bounded observation.

### S6 r4 Fault-State Confirmation Rebuild Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Link one fresh 18-object companion containing the registered generic fault-state confirmation, without changing its mechanics or closure. |
| Scope | Generate `artifacts/build/bochs-2.6-native-startup-transaction-msvc-r4` once from immutable r6 inputs with the existing refusal-first generator. Verify its manifest, 18-object list, two local macros, rejected-token scan and current `exception.cc` hash; then invoke only `nmake.exe /f ntdos64-native-startup-transaction.mak ntdos64-native-startup-transaction.exe` once under BuildTools x86, retaining its log, exit record and output/map hashes. |
| Non-goals | No r1--r3 mutation or retry; no source/API/object-list/macro/device/configure/VS-solution change; no `all` or `bochs.exe`; no execution, guest/firmware runtime, BOP/service decode, host capability, DOS/filesystem/search/CLI claim. The broad interceptor remains absent. |
| Required evidence | Fresh-root refusal; generator manifest/shim/current-source hashes; exactly 18 declared closure objects; exactly the two narrow opt-ins and no broad macro; one named command/exit/log; output/map hashes only if produced. |
| Verification | Generator checks and one collision probe before the invocation; terminal log and artifact inspection afterward; documentation governance and safe diff checks. |
| Stop conditions | Any generator/preflight mismatch, nonzero named-target exit, unexpected macro/object/token, missing retained record or timeout is terminal for r4. Do not repair, rebuild or execute r4 in this admission. |
| Exit criteria | A linked r4 binary proves only same-island MSVC/x86 closure for the amended mechanical log and is eligible for a separately admitted bounded observation. |

**Result (r4 orchestration rejection before target invocation):** r4 was
generated once and its 18-object/macro/source-hash preflight passed. The
orchestration guard then used an invalid PowerShell `Test-Path` expression
while checking for a pre-existing log or exit record; PowerShell stopped before
creating either record, before loading BuildTools, and before invoking `nmake`.
The fresh root is retained and not rebuilt. This is a host command-shape
failure, not a compiler, linker or guest result. **Next gate:** a separate r5
admission may use only the parenthesized collision guard in a new root; the
source, generator, closure and named target remain unchanged.

### S6 r5 Build-Orchestration Correction Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Execute the already admitted one-target fault-state confirmation build through a corrected host-side collision guard, without changing any Bochs or adapter input. |
| Scope | Generate `artifacts/build/bochs-2.6-native-startup-transaction-msvc-r5` once with the unchanged generator. Before the one named BuildTools/x86 `nmake` target, use only a parenthesized test of the two r5 evidence paths; retain the same manifest/shim/source hashes, 18-object/macro/rejected-token scans, command, log and exit record. |
| Non-goals | No r4 mutation/retry; no source/API/generator/object/macro/device/configure/VS-solution change; no `all`, `bochs.exe`, run, guest/firmware, service/BOP decode, host capability or DOS/CLI claim. |
| Required evidence | New-root refusal; unchanged generator and source/hash preflight; collision guard shape; exactly one named `nmake` invocation and matching process/recorded exit; output/map hashes if produced. |
| Verification | Fresh-root collision probe, static manifest/shim scan before the invocation; terminal log/artifact inspection; documentation governance and safe diff checks. |
| Stop conditions | Any setup/preflight mismatch, nonzero named-target exit, missing/mismatched exit record, unexpected macro/object/token or timeout is terminal for r5. Do not repair, rebuild or execute r5 in this admission. |
| Exit criteria | One classified r5 link result; a successful link is only eligible for a separately admitted bounded observation. |

**Result (r5 build-recording rejection):** r5 was generated once and passed
the unchanged closure/hash preflight. The named target wrote candidate
objects/executable/map, but the one-line `cmd` construction wrote the literal
`%NTDOS64_EXITCODE%` rather than a numeric exit record; the outer observation
then exceeded its 120-second limit. A post-timeout audit found no
`nmake`/`cl`/`link`/`cmd` residual. Because the required exit record is
invalid, no link-success claim is made and r5 is not reused, repaired or
executed. **Next gate:** first create and statically verify a refusal-first
root-local batch observation helper with delayed numeric exit capture; only a
subsequent admission may choose a new build root.

### S6 Build-Record Helper Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Materialize and statically verify one narrow host-side helper that can record a single named r6-style target's actual numeric exit without changing Bochs, adapter, the generator or a build root. |
| Scope | Add one PowerShell helper under `tools/` that refuses an existing log/exit record, writes a root-local batch wrapper with delayed expansion, invokes BuildTools x86 and exactly the predeclared narrow target, records the post-`nmake` numeric exit, and rejects a process/record mismatch. Add one focused static test for wrapper content and prohibited targets. |
| Non-goals | No r5 mutation/retry; no root creation or compiler/linker/executable invocation; no Bochs/adapter/CLI/guest/profile change; no `all`, `bochs.exe`, configure, VS solution, service/BOP decode, host capability or runtime claim. |
| Required evidence | Static helper test proves refusal-first paths, delayed numeric exit capture, exact named target, and absence of `all`/`bochs.exe`/configure or executable launch. |
| Verification | Focused static test, documentation governance and safe diff checks only. |
| Stop conditions | Stop if numeric exit capture requires an unbounded shell process, a target expansion, a retry facility, or a source/build-root change. |
| Exit criteria | A separately admitted fresh root can use the verified helper exactly once and preserve a numeric outcome. |

**Result (build-record helper source boundary complete):**
`Invoke-T95S6NarrowStartupTransactionLink.ps1` is refusal-first and writes a
root-local delayed-expansion batch wrapper. It admits only the exact narrow
target, records `!ERRORLEVEL!` after that target, and rejects a missing or
mismatched numeric record. Its focused static test passes alongside the
startup-transaction/#UD/bridge boundary tests, governance verification and
safe diff checks. No build root was created and no compiler, linker or
executable was invoked by this admission. **Next gate:** a separate fresh-root
r6 admission may invoke this helper exactly once.

### S6 r6 Verified Fault-State Confirmation Rebuild Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Obtain one unambiguous, numeric-exit MSVC/x86 link result for the otherwise unchanged 18-object companion containing the registered fault `CS:EIP` confirmation. |
| Scope | Generate `artifacts/build/bochs-2.6-native-startup-transaction-msvc-r6` once with the unchanged refusal-first generator; verify its manifest, 18-object list, two narrow macros, absent broad macro, rejected-token scan and current `exception.cc` hash. Invoke the verified narrow-link helper once against r6 with its 180-second direct wrapper limit. |
| Non-goals | No r1--r5 mutation/retry or use as runtime evidence; no Bochs/adapter/CLI/guest source or generator change; no object/macro/device/configure/VS-solution expansion; no `all`, `bochs.exe`, execution, firmware/guest run, service/BOP decode, host capability, DOS/filesystem/search or CLI product claim. |
| Required evidence | Fresh-root refusal; manifest/shim/source hashes; 18-object and macro/rejected-token preflight; one exact named target; numeric matching wrapper/process exit; retained log plus output/map hashes only if produced. |
| Verification | Generator/static helper checks and one collision probe before the invocation; terminal record/log/artifact inspection afterward; documentation governance and safe diff checks. |
| Stop conditions | Any preflight mismatch, helper timeout, missing/non-numeric/mismatched record, nonzero target exit, unexpected macro/object/token or residual process is terminal for r6. Do not repair, rebuild or execute r6 in this admission. |
| Exit criteria | One classified r6 link result. A successful link is eligible only for a separately admitted bounded NTIO observation. |

**Result (r6 linked with numeric evidence):** The one r6 named target exited
`0` and the root-local delayed-expansion record matched. The executable SHA-256
is `8AE9A77A68CA34154CB9992EE8E995FF68896963F9A679396CAEF476D4F4AC44`; map
SHA-256 is `728778000C2E4426B8104BAE9A337A2A09A00E291D631EDEEA85423D4A221FD4`.
The retained build-log SHA-256 is
`3B3DC324A96006158B067647ABC57632C9C7DDD4BFCF64A1E2155E198E5CD698`.
The 18-object/macros/rejected-token checks passed and no build process
remained. Evidence is retained in
`etc/research/t95-s6-narrow-startup-transaction-build-attempt-006.md`.
**Next gate:** separately admit one fresh direct-process real-NTIO observation
that requires the new fault `CS:EIP` fields but makes no source/configuration
change.

### S6 r6 Fault-State Observer Parser Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Update the existing direct-process observer so its generic commit predicate requires the already registered fault `CS:EIP` fields from r6's neutral post-commit log. |
| Scope | Change only the observer's generic log regular expression/schema label and add a focused static parser test. It may extract generic hexadecimal CS, EIP, address, byte-count and resume fields from the single existing line. |
| Non-goals | No execution or evidence-root creation; no Bochs/adapter/CLI/guest/profile/configuration change; no selector/service/BOP/DOS semantic decode, guest-memory read, device/firmware expansion, or run of r6. |
| Required evidence | Static test proves the parser requires all five generic fields and rejects the old three-field format or semantic terms. |
| Verification | Focused parser test, governance and safe diff checks only. |
| Stop conditions | Stop if the observer needs any new binary hook, guest input interpretation, or service-level predicate. |
| Exit criteria | A separate observation admission can distinguish the r6 transaction's pre-fault `CS:EIP` from its later post-resume evidence. |

**Result (r6 observer parser complete):** The direct-process observer now
requires the existing neutral commit line's five generic hexadecimal fields:
`CS`, `EIP`, write address, byte count and resume RIP. It no longer accepts the
old three-field form. Its focused parser test, narrow transaction boundary test,
governance verifier and safe diff checks pass. No r6 executable was run.

### S6 r6 Fault-State Real-NTIO Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Observe once whether the r6 source-built NTIO startup transaction enters the narrow generic #UD boundary with a valid fault `CS:EIP`, before considering any execution-state repair. |
| Scope | Invoke only the existing direct-process observer once with r6, unchanged source-built NTIO/NTDOS/COMMAND inputs, r6 ROM inputs, unchanged bounded profile/bochsrc and `-benchmark 1`, in one new evidence root. Require exactly one five-field generic commit, plan/benchmark/controlled-stop markers and no broad-interceptor marker. |
| Non-goals | No r1--r5 reuse, r6 rebuild/retry, Bochs/adapter/CLI/guest/profile/configuration source change, new guest input, service/BOP decode, guest-memory parsing, host-drive/filesystem/search capability, device/firmware expansion, or DOS/CLI product claim. |
| Required evidence | Fresh-root refusal; immutable component/ROM/binary/profile hashes; direct PID and timeout-only direct kill if required; one five-field commit, no broad marker, retained Bochs log and observation JSON. |
| Verification | One invocation only, evidence/hash/log inspection, no-residual-process audit, governance and safe diff checks. |
| Stop conditions | Any missing input/hash/marker, non-single commit, broad marker, timeout/direct kill or unexpected exit ends this root. Do not retry it. |
| Exit criteria | One evidence-backed distinction between wrong fault-time code state and a post-transaction continuation failure; neither outcome proves DOS runtime. |

**Result (fault state observed; post-transaction stop):** The new r6 evidence
root recorded exactly one generic commit with `CS=0070`, `EIP=0475`,
`address=8b00`, `bytes=6cd2`, `resume=479`; the EIP delta is the expected
four bytes and no broad interceptor marker appeared. Only later did the
existing x87 diagnostic report `0000:0293`, followed by its original IRQ13
path and the `0000:10000` prefetch failure. The 30-second direct watchdog
ended the exact process with no residual. This rules out an already-invalid
fault-time code position, but does not yet assign the later transition.
Evidence: `etc/research/t95-s6-r6-real-ntio-fault-state-006.md`. **Next gate:**
read-only provenance tracing across original resume/x87/IRQ13 state; no device
or adapter expansion is admitted.

### S6 Post-Transaction Original-Owner Provenance Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Classify from existing source and r6 logs whether the x87/IRQ13 and `0xa1800` events are original machine-owner transitions, without altering their behavior. |
| Scope | Read-only inspection of the copied r6 log and original `fpu.cc`, PIC, PC-system, CPU and memory source; record ordering, direct owners and unresolved state gaps only. |
| Non-goals | No source/test/build/run change; no new diagnostic, interrupt/mapping/device change, adapter/CLI/guest/host capability, service decode or DOS-runtime claim. |
| Evidence | One source-to-log provenance record with explicit established versus unresolved facts. |
| Verification | Source anchors, log ordering, documentation governance and safe diff checks. |
| Exit criteria | A single next diagnostic boundary is named, or an existing original owner is sufficient to explain the stop. |

### S6 Default-Off IRQ13 Transfer Diagnostic Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Observe whether the original external IRQ13 transfer itself changes `CS:EIP`, without changing original interrupt behavior. |
| Scope | Register `BX-TRACE-037` before source change. In `cpu/event.cc`, add one local default-off macro and two fixed logs only when the already acknowledged original vector equals 13: one immediately before, one immediately after the original `interrupt(vector, BX_EXTERNAL_INTERRUPT, 0, 0)`. |
| Non-goals | No change to vector acknowledgement, PIC/INTR/IVT/RIP/flags/stack/CPU result, adapter ABI, guest-memory access, service/BOP decode, device/mapping/profile/build/run behavior. |
| Evidence | Focused static test proves macro default-off, vector equality gate, pre/post placement and no mutation. |
| Verification | Focused test, retained #UD/x87 tests, governance and safe diff checks. |
| Stop conditions | Stop if a new hook, event state, generic interrupt trace, CPU mutation or adapter/guest term is needed. |
| Exit criteria | A future fresh binary can distinguish state before and after the original IRQ13 transfer. |

**Result (IRQ13 transfer diagnostic source boundary complete):**
`BX-TRACE-037` is now a local default-off `event.cc` diagnostic. Only after the
original acknowledge returns vector 13 does it log local `CS:EIP` immediately
before and after the unchanged original external-interrupt call. It does not
write any CPU/PIC/IVT state or reference adapter/guest semantics. Its focused
test, retained x87 and startup-transaction tests, governance verifier and safe
diff checks pass. No build or run occurred. **Next gate:** a separately
admitted fresh companion may enable only this macro in its copied `event.o`
and observe one unchanged bounded NTIO input.

### S6 r7 IRQ13-Diagnostic Build-Root Generator Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Extend the existing refusal-first copied-root generator so r7 can rebuild only the changed original `event.o` with the registered diagnostic macro. |
| Scope | Add `cpu/event.cc` replacement and stale-object removal, plus one explicit `cpu/event.o` rule carrying only `BX_NTVDM_ENABLE_IRQ13_TRANSFER_DIAGNOSTIC=1`; retain all existing main/exception macros, 18 adapter/CLI objects and rejected scans. |
| Non-goals | No root generation/build/run; no target/object-list/device/configuration/adapter/guest change; no broad interceptor, service or host capability. |
| Verification | Static generator source review, existing focused boundary tests, governance and safe diff checks. |
| Exit criteria | A later r7 admission can generate one new root whose only additional rebuilt Bochs object is `event.o`. |

**Result (r7 generator boundary complete):** Static inspection proves the
generator replaces/removes only the additional original `event.cc`/`event.o`
pair and emits one object-local IRQ13 diagnostic macro. Existing narrow macros,
18-object closure and broad-interceptor exclusion remain unchanged. Focused
IRQ13 and governance checks pass; no root/build/run occurred.

### S6 r7 IRQ13-Diagnostic Link Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Link one fresh r7 companion containing only the registered default-off IRQ13 transfer diagnostic in addition to the proven r6 closure. |
| Scope | Generate `artifacts/build/bochs-2.6-native-startup-transaction-msvc-r7` once, verify its 18-object/macro/rejected-token and three-replaced-source manifest, then invoke the verified named link helper once. |
| Non-goals | No retry/mutation of earlier roots; no source/device/configuration/adapter/guest change; no execution, service/BOP/host capability or DOS/CLI claim. |
| Stop conditions | Any preflight/helper/exit/artifact mismatch is terminal for r7; do not rebuild or execute it here. |
| Exit criteria | A linked r7 binary is eligible only for separately admitted bounded observation. |

**Result (r7 terminal dependency failure):** r7's sole named target exited
`2`: the new explicit `event.o` rule existed, but `cpu/libcpu.a` depended only
on `exception.o`, so `nmake` reached the archive without rebuilding the removed
`event.o`. r7 is retained and not retried or executed.

### S6 r8 Event-Archive Dependency Correction Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Correct the generator's archive prerequisite so the already admitted event diagnostic object is built before the existing CPU archive. |
| Scope | Change only the generated `cpu/libcpu.a` prerequisite list from `cpu/exception.o` to `cpu/exception.o cpu/event.o`; add static verification. |
| Non-goals | No root/build/run or behavior/source/device/adapter/guest change. |
| Exit criteria | A later fresh r8 root can build both declared changed CPU objects before the unchanged archive command. |

### S6 r8 IRQ13-Diagnostic Link Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Obtain one fresh numeric-exit link result after the archive prerequisite correction. |
| Scope | Generate r8 once with the corrected generator and invoke only the existing named link helper once. |
| Non-goals | No source/device/configuration/adapter/guest change or execution. |
| Stop conditions | Any mismatch or nonzero exit is terminal for r8 with no retry. |

### S6 x87 IRQ13-Eligibility Diagnostic Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Distinguish disabled external interrupts from a later IRQ13 transfer failure. |
| Scope | Extend only the registered default-off x87 log with current EFLAGS and IF before its unchanged IRQ13 raise. |
| Non-goals | No state/predicate/IRQ/PIC/IVT/adapter/guest/build/run change. |

### S6 r9 FPU Eligibility-Diagnostic Build-Root Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Carry the registered x87 EFLAGS/IF observation into one fresh root while preserving the original complete FPU archive. |
| Scope | Extend the existing generator to copy `fpu.cc`, remove copied `fpu.o/libfpu.a`, and append only the existing x87 diagnostic opt-in to that root's original FPU Makefile so its original archive rule rebuilds the original member set. |
| Non-goals | No archive member selection, device/configuration/adapter/guest/CLI change, root/build/run in this admission. |
| Exit criteria | A later r9 root can rebuild the original FPU archive with only the diagnostic macro difference. |

### S6 r9 FPU Eligibility-Diagnostic Link Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Generate and link one fresh r9 companion that includes the registered x87 EFLAGS/IF diagnostic. |
| Scope | Generate r9 once with the verified generator, then invoke the one existing named link helper once after its manifest/macro checks. |
| Non-goals | No execution, adapter/guest/device/configuration/semantic change or retry of earlier roots. |
| Stop conditions | Any nonzero/preflight mismatch is terminal for r9. |

**Result (r9 terminal Makefile syntax rejection):** The copied-root helper
appended unsupported NMAKE `+=` syntax; recursive FPU make stopped before FPU
compilation. r9 is retained and not retried. The original copied FPU Makefile
already carries the diagnostic macro, so the append was removed.

### S6 r10 FPU Eligibility-Diagnostic Link Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Link one fresh r10 companion using the original FPU archive rule and the updated copied `fpu.cc`. |
| Scope | Generate r10 once and run only the named helper once; no copied Makefile edit. |
| Non-goals | No run or semantic/device/adapter/guest change. |

### S6 r10 x87 Eligibility Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Observe once the existing x87 record's EFLAGS/IF fields using unchanged NTIO inputs and bounded direct-process supervision. |
| Scope | Invoke only the existing observer once against r10 in a new evidence root; inspect its retained Bochs log for one x87 record with `eflags` and `if`. |
| Non-goals | No source/configuration/input/device/adapter/guest change or retry. |

**Result (IRQ13 is not the first missing capability):** The one r10 root linked
and the one preserved observation emitted the enabled x87 record with
`CR0=60000010`, `EFLAGS=00000493`, and `IF=0`, followed by the unchanged
original `math_abort`/IRQ13 raise.  The earlier IRQ13-transfer instrumentation
does not fire, which is therefore consistent with the original external
interrupt eligibility gate rather than evidence of a missing PIC, IVT `75h`,
or adapter IRQ behavior.  The observation watchdog remains terminal evidence;
it does not prove a guest runtime.  The first next gate is a read-only
OpenNT/NTIO continuation audit from the checked `50:11` resume through the
first subsequent source-defined transition.  No device, interrupt, mapping,
or adapter behavior change is admitted by this result.

### S6 OpenNT Post-`50:11` Continuation Semantics Audit Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Use the pinned OpenNT source, source-built NTIO image/map, and retained r10 log to identify the first source-defined operation after the checked `50:11` resume, and distinguish a missing historical host/guest transition from a Bochs CPU/FPU/IRQ/memory defect. |
| Scope | Read only `msinit.asm`, `sysinit1.asm`, the source-built `NTIO.SYS`/`ntio.map`, retained r10 evidence, and historical SoftPC owner sources. Record relocation arithmetic, instruction purpose, and owner classification. |
| Non-goals | No build/run, source/configuration change, new BOP/service implementation, guest-state injection, IRQ13/IVT/PIC/FPU/VGA change, adapter DOS semantics, or CLI/host-drive expansion. |
| Evidence | One numbered report establishing or rejecting the causal ordering from `50:11` resume to the first unserved operation, with exact source/image/log anchors and a bounded next diagnostic or design decision. |
| Stop conditions | Stop before any implementation if the next operation is not source/image-correlated, requires a broad dispatcher, or would place Bochs/SoftPC/DOS semantics in the adapter. |
| Exit criteria | A source-backed classification only; an implementation remains separately admitted. |

### S6 Unmatched-#UD Position Diagnostic Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Establish whether the first unserved source-defined post-`50:11` marker is the correlated `BOP 12h`, without changing its result or exception handling. |
| Scope | Register `BX-TRACE-039` before changing `refs/bochs/cpu/exception.cc`. Add one default-off macro and one log only after the narrow startup transaction declined a `#UD`, recording CPU id, current CS selector and existing `prev_rip`. Add a focused static boundary test. |
| Non-goals | No marker/service decoding, instruction-window/guest-memory read, adapter call, CPU/flags/RIP/stack/exception disposition change, BOP12 result, IRQ/FPU/PIC/IVT/VGA/device/profile/CLI behavior, build or run. |
| Evidence | Static test proves the macro defaults to zero, the log is gated by `#UD` and occurs only after the narrow interceptor declines, and it has no state or adapter effect. |
| Verification | Focused test plus retained startup-transaction boundary tests, documentation governance, and safe diff checks. |
| Stop conditions | Stop if logging requires byte inspection, a second hook, a guest/adapter term, or any state mutation. A later fresh-root observation is separately admitted. |
| Exit criteria | Default-off diagnostic source boundary only. |

**Result (source boundary complete):** `BX-TRACE-039` is registered and
`exception.cc` now has a default-zero position-only record after the narrow
startup transaction returns false and before the existing broad interceptor
block.  It records only pre-existing CPU id, CS selector and `prev_rip`; it
does not inspect bytes, touch guest memory, invoke the adapter, or alter any
CPU/exception state.  `tests/bx-ntvdm-unmatched-ud-diagnostic-boundary.ps1`,
documentation governance, and safe diff checks pass.  **Next gate:** a
separate fresh-root build/run admission may enable only this macro alongside
the existing narrow startup transaction, then perform one unchanged-input
observation to prove or reject `CS:IP=010d:0516` (physical `0x15e6`).

### S6 Unmatched-#UD Diagnostic Build-Shim Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Make the already registered `BX-TRACE-039` available to exactly one later fresh-root observation without changing the normal startup-transaction recipe. |
| Scope | Add an explicit `EnableUnmatchedUdDiagnostic` switch to the existing narrow-root generator and link helper. When selected, it adds only `BX_NTVDM_ENABLE_UNMATCHED_UD_DIAGNOSTIC=1` to copied `cpu/exception.o`, records the boolean and the exact three-macro set, and requires the same switch at link time. Extend the static helper test. |
| Non-goals | No build-root creation, compiler/linker/archive/executable invocation, Bochs/OpenNT semantic or device change, adapter call, guest input change, BOP12 implementation, or broad exception interceptor. |
| Verification | Focused helper and diagnostic-boundary tests, documentation governance, and safe diff check. |
| Stop conditions | Stop if the macro can appear outside copied `exception.o`, is not represented in the root record, permits a mismatched link invocation, or enables an extra feature. |
| Exit criteria | The later fresh root is mechanically distinguishable from the r10 recipe and has exactly the declared diagnostic delta. |

**Result (build-shim boundary complete):** The generator and linker now bind
the optional third macro to the same explicit switch and recorded boolean;
the ordinary recipe remains two macros. `Test-NarrowStartupTransactionLinkHelper.ps1`,
`Test-Unmatched-Ud-Diagnostic-Boundary.ps1`, documentation governance, and
safe diff checks pass.

### S6 r11 Unmatched-#UD Diagnostic Build Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Produce one fresh MSVC/x86 narrow startup-transaction executable with only `BX-TRACE-039` added to the existing r10 diagnostic closure. |
| Scope | Generate the unique r11 root with `EnableUnmatchedUdDiagnostic`, then invoke the named narrow linker once against that exact root. The fixed 18-object adapter/CLI closure, existing x87 and IRQ13 diagnostic objects, and original archive/link membership remain unchanged. |
| Non-goals | No retry, full/product build, `bochs.exe`, configuration/device/firmware/profile/adapter/OpenNT/CLI change, BOP12 implementation, or executable run in this admission. |
| Verification | Fresh-root refusal, generated record's exact three-macro set, one recorded-zero linker exit, executable/map presence, and documentation governance. |
| Stop conditions | A collision, record mismatch, nonzero link, missing outputs, or any added macro is terminal for r11; do not repair or retry the root. |
| Exit criteria | One linked r11 artifact exists or its terminal link evidence is retained for a subsequent source-free review. |

**Result (r11 link complete):** The unique root
`artifacts/build/bochs-2.6-native-startup-transaction-msvc-r11-unmatched-ud`
was generated once with the recorded exact macro set
`EXECUTION_PLAN`, `STARTUP_TRANSACTION`, and `UNMATCHED_UD_DIAGNOSTIC`.
The named target linked once with recorded exit `0`; the separately named
executable and map are present. No run occurred in this admission.

### S6 r11 Unmatched-#UD Position Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Run the already linked r11 artifact once on the unchanged source-built NTIO/NTDOS/COMMAND evidence profile and determine whether the first post-`50:11` declined transaction reports `CS:IP=010d:0516`. |
| Scope | Invoke the existing direct-process observer once in one new immutable evidence root, with the same profile, ROMs, bochsrc, environment names, benchmark and watchdog as r10. Retain `bochs.log` and `observation.json`, then read only the emitted `BX-TRACE-039` record. |
| Non-goals | No source or tool modification, retry, changed guest input/profile/ROM/configuration, device/firmware/FPU/IRQ/PIC/IVT/VGA mapping change, adapter semantics, BOP12 result, DOS service, or CLI capability. |
| Verification | Root refusal before execution, one r11 binary hash in the observation record, one preserved `50:11` commit, broad-interceptor absence, and position-only log comparison with the source/image-correlated expected address. A watchdog terminal result remains evidence, not a repair authorization. |
| Stop conditions | Collision, missing retained log, mismatched source-built identities, absent 50:11 commit, broad-interceptor marker, or missing/ambiguous unmatched-#UD record stops interpretation; do not rerun or enable a device. |
| Exit criteria | The record either corroborates or rejects BOP12 as the first unserved operation and names no next implementation until owner/design review. |

**Result (BOP12 corroborated):** The sole r11 observation retained
`artifacts/analysis/t95-s6-r11-unmatched-ud-009-20260811-001` and emitted
one accepted `50:11` commit followed by
`ntdos64 unmatched-ud cpu=0 cs=010d rip=00000516`. This exactly matches the
source/image-correlated `goinit` `BOP 12h` at physical `0x15e6`. The later
unmatched markers, x87/IRQ13 record, and `0xa1800` veto are downstream and
cannot justify a Bochs device, mapping, PIC/IVT, FPU, adapter, DOS, or CLI
repair. The watchdog-ended process is retained evidence only. The sole next
gate is design-only: classify the one historical conventional-memory result
and its fixed-width owner boundary before any implementation.

### S6 Complete BOP Catalogue And Passive-Listener Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Replace the one-transition investigative route with one complete, source-derived BOP catalogue and a uniform adapter listener boundary. Every discovered BOP must be identifiable at the generic Bochs `#UD` seam; only an independently source-closed entry may resume execution. |
| Scope | Read all OpenNT MVDM BOP definition sources, including selector macros, the NTVDM-conditioned SoftPC `BIOS[]` table, and BOP-family service include files. Add a reproducible inventory exporter and immutable inventory/report. Design the adapter catalogue record and default-off listener contract. Update current architecture/coding authority to allow this catalogued adapter inspection of copied BOP bytes, replacing the former blanket prohibition on service-ID inspection. |
| Non-goals | No full historical CCPU/SAS/SoftPC linkage; no generic BOP success/failure emulation; no source-defined service implementation beyond the already evidence-backed `50:11` fixture; no automatic `12h` activation; no Bochs selector/service terminology or dispatch; no device/firmware/IRQ/PIC/FPU/VGA change; no host filesystem/drive/CLI capability expansion; no executable build/run in this admission. |
| Required boundary | Bochs captures a bounded generic instruction window and applies only a validated typed resume/delta. The adapter owns the complete static BOP identity catalogue and passive observation. An entry that has no admitted implementation logs and returns pass-through to the original Bochs exception path; it must never advance RIP or synthesize a result. |
| Evidence | Source paths, conditional owner classification, selector/service-range inventory, report hash, adapter/Bochs responsibility matrix, and explicit unsupported/pass-through rule. |
| Verification | Deterministic export from pinned source, source-line and duplicate/conflict checks, focused parser/catalogue tests once authored, documentation governance, and safe diff check. |
| Stop conditions | Stop before runtime wiring if a definition cannot be provenance-classified, the listener needs unbounded guest-memory access, a raw host pointer/callback, a Bochs source selector branch, or an unimplemented entry could resume. Each candidate implementation requires its own source/ABI/failure admission. |
| Exit criteria | One complete inventory and an accepted uniform listener design name the only route from Bochs to adapter and make every unimplemented BOP visible but fail-closed. |

**Exporter attempt (terminal, no inventory):** The first invocation stopped before
creating its output root because the new parser passed base `0` to
`[Convert]::ToInt32`, which PowerShell rejects. No inventory JSON/Markdown,
build, runtime, or source interpretation resulted; that name is not reused.

### S6 BOP Inventory Numeric-Parser Repair Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Repair only the exporter representation of hexadecimal and decimal literal conversion after the pre-output parser failure. |
| Scope | Add one local literal parser that explicitly recognises `0x` hexadecimal or decimal text, and replace the invalid `Convert.ToInt32(..., 0)` use. Re-run only against a new immutable output root. |
| Non-goals | No source-set expansion, source interpretation change, BOP runtime wiring, adapter/Bochs/OpenNT semantic change, build, or execution. |
| Verification | The new output must contain JSON and Markdown; every exported selector must agree with `BOP.INC`; required selector and service-family counts must be nonzero; source hashes are recorded. |
| Stop conditions | Any malformed literal, disagreement, missing output, duplicate key, or nonzero exporter exit terminates that root with no retry. |
| Exit criteria | The exporter produces one reproducible source-definition inventory only. |

**Result (inventory complete):** The distinct immutable root
`artifacts/analysis/opennt-bop-inventory-002-20260811-001` contains the
exported JSON/Markdown. It records 16 top-level selectors with C/MASM parity,
six service families (DOS 74, COMMAND 18, XMS 12, DPMI 25, Redirector 49,
Debugger 16), 45 BIOS constants, and 349 annotated source-table rows across
conditional branches. The detailed provenance and boundary decision are in
`etc/research/opennt-bop-definition-inventory-001.md`. No BOP became enabled
or implemented by this inventory.

**Inventory classification correction required:** Review of the emitted 45
`BIOS_*` definitions found non-BOP storage/model constants such as
`BIOS_VAR_SEGMENT` and `BIOS_KB_BUFFER_SIZE`. The top-level C/MASM parity and
six service-family inventories remain valid, but root `002` must not be used
as the authoritative BIOS-selector inventory. It is retained as parser
evidence only; no runtime/catalogue code has consumed it.

### S6 BIOS BOP-Range Parser Correction Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Restrict only the BIOS-constant exporter to `bios.h`'s explicit BOP-definition section and re-export a precise inventory in a new root. |
| Scope | Replace the generic `BIOS_*` parse with a range parser bounded by the source comments introducing BOP call numbers and BIOS internal storage. It retains private bootstrap/diskette/mouse/date/CPU-return BOP definitions within that section and excludes model/storage/buffer constants. |
| Non-goals | No selector/service-family parser change, adapter catalogue implementation, BOP runtime wiring, Bochs/OpenNT change, build or run. |
| Verification | A new root must retain no `BIOS_VAR_SEGMENT`, `BIOS_KB_BUFFER`, or `BIOS_LEVEL` entry, must retain `BIOS_MEMORY_SIZE=12h`, `BIOS_BOOTSTRAP_1=90h`, and `BIOS_CPU_RETURN=feh`, and preserve C/MASM selector parity. |
| Stop conditions | Missing range sentinels, malformed/duplicate selector, or any excluded data constant in output stops the new root. |
| Exit criteria | The full definition inventory has source-precise BIOS BOP entries only. |

**Parser correction attempt (terminal root 003):** The exporter created
`artifacts/analysis/opennt-bop-inventory-003-20260811-001`, but its required
post-export check found `BIOS_VAR_SEGMENT` still present. The range stop
condition was not evaluated as intended because the PowerShell `-and` and
`-match` expression lacked explicit grouping. The root is retained as failed
parser evidence and is not overwritten or consumed.

### S6 BIOS BOP-Range Predicate Repair Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Make the already selected `bios.h` range predicate unambiguous without changing its sentinels, source set, exported schema, or BOP interpretation. |
| Scope | Add explicit parentheses around the boolean/range-match expressions only, then use one new root to repeat the documented verification predicates. |
| Non-goals | No source range or parser-policy change, runtime/adaptor/Bochs/OpenNT modification, inventory-schema expansion, build, or run. |
| Verification | New root must pass all prior inclusion/exclusion predicates and preserve the existing top-level selector and service-family counts. |
| Stop conditions | Any retained data constant, absent required BOP constant, or mismatched family count terminates the new root. |
| Exit criteria | One precise source-definition inventory supersedes `002`/`003` for all later catalogue work. |

**Predicate repair attempt (terminal root 004):** Parentheses did not change
the result because the stop-marker literal itself said `define` while the
historical comment says `defines`. The new root again retained
`BIOS_VAR_SEGMENT` and is frozen as failed parser evidence; it is not a
re-run candidate.

### S6 BIOS BOP-Range Marker Spelling Repair Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Correct only the one-character source-marker spelling in the existing range predicate. |
| Scope | Change `define` to `defines` in the stop-marker literal, then export once to a new root using the existing input manifest and verification predicates. |
| Non-goals | No parser/control-flow/schema/source-set/adapter/Bochs/OpenNT change, build, or run. |
| Verification | The historical stop comment must match exactly; required BOP entries remain and all previously forbidden storage/model names are absent. |
| Stop conditions | Any missing marker or invalid classification terminates the new root. |
| Exit criteria | A precise BIOS BOP definition slice is available for the adapter catalogue design. |

**Result (authoritative inventory root 005):**
`artifacts/analysis/opennt-bop-inventory-005-20260811-001` passed the
inclusion/exclusion checks: it retains `BIOS_MEMORY_SIZE=12h`,
`BIOS_BOOTSTRAP_1=90h`, and `BIOS_CPU_RETURN=feh`; it excludes the reviewed
storage/model constants. The precise inventory has 16 top-level selectors,
six service families, 41 BIOS BOP constants, and 349 condition-preserving
BIOS table rows. Its JSON SHA-256 is
`893EB8D98A476A4D8A7AE996635B2B2BD24706594DE3666C2E664A1FB97E5C28`.
Root `005` supersedes `002`--`004` for all catalogue work.

### S6 BOP Catalogue And Passive Listener ABI Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Implement the adapter-local, fixed-width BOP identity catalogue and passive observation decision needed to connect a future generic Bochs event without creating a dispatcher. |
| Scope | Add adapter C11 headers/sources and a deterministic unit test. The API may accept only the copied exception event, copied CPU state and existing bounded instruction window; identify `C4 C4 selector` and the fourth byte only for catalogued families; return a typed identity/observation result. It may classify known/unknown, implemented/inactive, and pass-through, but has no guest-memory, host capability, function pointer, callback, Bochs include, or CPU mutation access. |
| Non-goals | No Bochs source change or build/run, no generic runtime connection, no `50:11` migration, no `12h` activation, no service owner call, no guest write/read, no new CLI option, no dynamic table loading, no full source-generated C catalogue claim, and no success result for any unimplemented entry. |
| Required behavior | All copied `C4 C4` selectors are observable. A known service-family selector with fewer than four copied bytes is an explicitly truncated observation, not a guessed service. All unknown selector/family/service values are observable and pass-through. The only possible listener disposition in this admission is pass-through. |
| Verification | C11 positive/negative test covers BIOS `12h`, DOS `50:11`, all six family selector forms, non-BOP, truncated family, unknown selector and unknown service; static source check rejects mutable CPU/flags/RIP, guest memory, host service, Bochs and callback terms. Documentation governance and safe diff checks apply. |
| Stop conditions | Stop if the implementation needs a fixed response value, byte fetch beyond the supplied window, global handler registration, general dispatch, or a Bochs/OpenNT source change. A later Bochs event hook is separately registered and admitted. |
| Exit criteria | The adapter exposes one source-provenance-linked identity record which every future BOP event can consume while all entries remain fail-closed. |

### S6 BOP Catalogue C11 Test Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Compile and run only the newly admitted adapter-local identity classifier and its supporting ABI sources. |
| Scope | Use one new artifact root and one direct MSYS2 UCRT64 C11 command over `cpu_delta`, `cpu_state`, `instruction_window`, `exception`, `bop_catalog`, and the catalogue test source; retain command/output/hash. |
| Non-goals | No CMake product target, Bochs/MSVC target, Bochs source, adapter runtime wiring, BOP implementation, guest profile, or runtime observation. |
| Verification | Compiler exit zero; the executable prints the pass-through classification marker; the retained command names exactly the six admitted sources. |
| Stop conditions | Any extra source, nonzero compile/run, or output mismatch is terminal for the root and does not authorize a repair beyond source review. |
| Exit criteria | The catalogue ABI's pure C11 classification boundary is independently proven. |

**C11 test root (terminal before diagnostics):** The fresh root
`artifacts/build/t95-s6-bop-catalog-c11-001` was created and the exact
six-source command returned exit `1` before producing an executable. The
tool transport retained no compiler diagnostic text. This root is terminal
and is not reused.

### S6 BOP Catalogue Compiler-Diagnostic Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Obtain only the compiler's diagnostic text for the already failed exact six-source C11 command. |
| Scope | Run the same source list once with no executable output and redirect diagnostic text to one new analysis root. |
| Non-goals | No source repair, test-root retry, link/run, CMake, Bochs, adapter runtime wiring, or guest execution. |
| Verification | The retained diagnostic names the compiler failure cause and the command/source list is identical to the terminal test root. |
| Stop conditions | Missing diagnostic or an extra source/tool option ends this diagnostic root; any source repair needs a later admission. |
| Exit criteria | The next source review has concrete compiler evidence. |

**Compiler-diagnostic root (terminal without text):**
`artifacts/analysis/t95-s6-bop-catalog-c11-diagnostic-001` retained the exact
`-fsyntax-only` source list but again returned exit `1` with a zero-byte
diagnostic log. It therefore neither identifies a C source defect nor
authorizes a source repair. The direct Windows process transport is now an
external-toolchain suspect; this root is not reused.

### S6 BOP Catalogue MSYS2-Bash Syntax-Diagnostic Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Distinguish a source diagnostic from the Windows native-process transport by invoking the same no-output C11 syntax command once through the installed MSYS2 Bash environment. |
| Scope | One new analysis root records the exact Bash command, Bash/GCC versions, exit code and combined output. The source list and `-fsyntax-only` scope remain unchanged. |
| Non-goals | No source repair, executable/link/run, CMake, build-root retry, Bochs/adapter runtime change, or guest execution. |
| Verification | Combined output is nonempty or a zero-exit syntax result is explicitly retained; no extra source appears. |
| Stop conditions | A second empty nonzero transport result classifies this as external toolchain/process evidence and stops compiler probing; do not modify C source on that basis. |
| Exit criteria | The C11 test has actionable compiler evidence or a recorded external-blocked conclusion. |

**MSYS2-Bash diagnostic root (terminal tool-path evidence):**
`artifacts/analysis/t95-s6-bop-catalog-msys2-syntax-001` retained the command
attempt and Bash returned `127` with `gcc: command not found`; the installed
Bash PATH lacks the separately confirmed `ucrt64/bin` compiler directory.
No source was compiled or diagnosed. This is tool-path evidence only and the
root is not reused.

### S6 BOP Catalogue Explicit-UCRT64 Bash Syntax Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Execute the same C11 syntax-only source list once with MSYS2 Bash's PATH explicitly prefixed by the already identified `/ucrt64/bin`. |
| Scope | One new analysis root; `PATH=/ucrt64/bin:$PATH`, `gcc --version`, then the unchanged `-fsyntax-only` command. |
| Non-goals | No C source repair, output executable/link/run, CMake/build-root retry, Bochs/adapter runtime wiring, or guest execution. |
| Verification | GCC identifies itself, then returns actionable syntax output or zero; source list remains exact. |
| Stop conditions | Failure before GCC identification is external environment evidence. A compiler-diagnosed source failure is recorded before any repair. |
| Exit criteria | A valid compiler disposition, not an ambiguous Windows/Bash process failure. |

**Result (syntax boundary complete):** The new root
`artifacts/analysis/t95-s6-bop-catalog-ucrt64-syntax-001` explicitly prefixed
`/ucrt64/bin`, identified GCC 16.1.0, and completed the exact six-source C11
syntax-only command with exit `0`. The earlier failures are classified as
process/PATH evidence, not C source evidence.

### S6 BOP Catalogue UCRT64 C11 Test Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Compile and execute one new adapter-only C11 unit-test root using the now proven explicit UCRT64 Bash environment. |
| Scope | One new root; same six source files, `-std=c11 -Wall -Wextra -Werror`, one named test executable, then one execution and executable hash. |
| Non-goals | No CMake, Bochs/MSVC build, Bochs source/runtime hook, guest execution, BOP implementation, or CLI/host capability change. |
| Verification | Compile and run exits zero and emits the identity-only pass-through marker. |
| Stop conditions | Any nonzero compile/run is terminal for the root and requires source review before another root. |
| Exit criteria | The adapter catalogue's passive identity classifier has independent executable evidence. |

**UCRT64 C11 test root (terminal external-temp failure):** The fresh root
`artifacts/build/t95-s6-bop-catalog-ucrt64-c11-001` reached GCC but failed
before output with `Cannot create temporary file in C:\msys64\tmp\:
Permission denied` (exit `127`). No source diagnostic or executable exists;
the root is not reused.

### S6 BOP Catalogue UCRT64 Workspace-Temp Test Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Run the same proven six-source C11 test once with GCC temporary files confined to its new workspace artifact root. |
| Scope | One new root; explicit UCRT64 Bash PATH and `TMPDIR` set to a subdirectory inside that root; identical compiler flags/sources/test executable/run/hash. |
| Non-goals | No source repair, CMake, Bochs/MSVC build, Bochs hook/runtime, guest execution, BOP implementation, or CLI/host change. |
| Verification | GCC can create temporary files under the declared root; compile/run exit zero; marker and SHA-256 are retained. |
| Stop conditions | A nonzero result is terminal evidence; do not change source or retry that root. |
| Exit criteria | Adapter-only executable proof without ambient temporary-directory dependency. |

**Workspace-`TMPDIR` test root (terminal external-temp failure):**
`artifacts/build/t95-s6-bop-catalog-ucrt64-workspace-temp-001` still failed
before source compilation with the same `C:\msys64\tmp` permission error.
GCC did not honor `TMPDIR` alone; no output executable or source diagnostic
exists and the root is frozen.

### S6 BOP Catalogue UCRT64 Full-Temp Environment Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Make one final bounded adapter-test attempt with all three conventional temporary variables (`TMP`, `TEMP`, `TMPDIR`) set to a new workspace-owned directory. |
| Scope | One new root, same PATH/source/flags/executable/run/hash as the prior attempt; only the three temporary-directory variables differ. |
| Non-goals | No source repair, additional compiler probing, CMake, Bochs/runtime/guest work, or BOP implementation. |
| Verification | Compile/run success must show the marker and hash. |
| Stop conditions | A repeated external-temp failure closes executable probing as externally blocked; preserve syntax proof and continue non-build design/wiring work without retry. |
| Exit criteria | Either executable evidence or an explicit external-toolchain limit. |

**Result (adapter catalogue C11 proof):** The final fresh root
`artifacts/build/t95-s6-bop-catalog-ucrt64-full-temp-001`, with explicit
workspace `TMP`, `TEMP`, and `TMPDIR`, compiled and ran the exact six-source
test successfully. It emitted
`bx-ntvdm BOP catalog: identity-only pass-through classification verified`;
the executable SHA-256 is
`A83F2BDFD041741DDC8BCFB47716451E657821425239A765CC0C8B456AC08E7B`.
Earlier C11 roots remain external PATH/temp evidence only. No Bochs hook,
BOP implementation, or guest runtime was enabled by this result.

**Listener total-selector regression:** The same C11 contract test now
exhaustively verifies all 256 `C4 C4 xx` selector values: every value produces
an observable BOP identity and preserves its selector byte, whether or not the
pinned OpenNT/SoftPC definition inventory classifies it. Fresh MSVC C11 root
`artifacts/build/t95-s6-bop-catalog-msvc-c11-001` compiled, linked and ran the
six-source test with `/TC /std:c11 /W4 /WX`; it emitted the same pass-through
marker. This verifies listener totality, not a new dispatch/result capability.

### S6 Default-Off BOP Listener Bochs-Bridge Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Connect every generic Bochs `#UD` BOP-shaped instruction to the adapter's proven identity catalogue, so every BOP invocation can be observed before any individual implementation is selected. |
| Scope | Under registered `BX-ABI-040`, add one default-zero macro and one local Bochs helper. It builds an existing fixed-width exception event, calls only `bx_ntvdm_bop_catalog_v1_observe`, and emits one fixed-width identity log only when `is_bop` is true. The call occurs after existing copied CPU/window capture and before existing startup/broad consumers. |
| Non-goals | No selector/service switch, service/owner name, result/disposition, RIP/register/flags/memory/device/firmware change, guest read, OpenNT include, host capability, callback, generic dispatcher, build/root/run, or migration of `50:11`/`12h`. |
| Required behavior | The helper has no return value used by CPU control flow. A malformed/non-BOP/unknown/truncated record cannot alter exception handling. The existing `50:11` bridge and later broad interceptor retain their order and behavior; the listener observes but never consumes. |
| Verification | Static boundary test proves default-off guard, generic fixed-width inputs, adapter-only call, no selector literal/service term/result mutation, and listener ordering; existing adapter C11 proof, documentation governance, and safe diff checks. |
| Stop conditions | Stop if Bochs needs selector inspection, an adapter result, a guest-memory read, an additional hook, or any state/control-flow mutation. A fresh MSVC root/run is separately admitted after static review. |
| Exit criteria | Default-off source boundary only; runtime activation requires a new root and unmodified guest profile. |

**Result (default-off Bochs listener boundary complete):** `BX-ABI-040` is
registered and `exception.cc` now supplies only a default-zero generic event
notification. It calls the adapter catalogue with the already copied CPU/window
records and logs returned fixed-width identity fields only when the adapter
recognizes `C4 C4`; it neither contains selector/service literals nor uses a
result. The call precedes the existing startup consumer, so `50:11` can be
observed without changing that established transaction. The focused listener
test, unmatched-UD and narrow-link helper tests, documentation governance,
and safe diff checks pass. A separate fresh MSVC closure must add the one
adapter object and macro before any runtime observation.

### S6 r12 BOP Listener Build-Shim Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Extend the narrow startup build generator and named linker so one future r12 root can enable exactly the registered BOP listener and compile its sole adapter catalogue object. |
| Scope | Add an explicit `EnableBopCatalogListener` switch. When selected, it adds `BX_NTVDM_ENABLE_BOP_CATALOG_LISTENER=1` only to copied `cpu/exception.o`, adds only `adapter\bx_ntvdm_bop_catalog_v1.obj` to the frozen closure, records the boolean/exact macro set, and requires the same switch at link time. Extend the helper test. |
| Non-goals | No root/build/link/run in this admission; no broad exception interceptor; no migration of the 50:11 bridge; no BOP service implementation or result; no device/profile/guest/CLI change. |
| Verification | The static helper test must distinguish ordinary, unmatched-UD, and BOP-listener records; source checks ensure the object is present only under the explicit switch and listener macro appears only once in the copied exception compile rule. |
| Stop conditions | Stop if an unrelated object/macro enters, the previous 18-object route changes when disabled, or a link helper accepts a mismatched root record. |
| Exit criteria | A future r12 fresh root has an exact 19-object, four-macro declared closure and cannot be confused with r11. |

### S6 r12 BOP Listener Link Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Link one fresh MSVC/x86 narrow startup executable that observes BOP identity while preserving the r11 startup transaction and diagnostic closure. |
| Scope | Generate the unique r12 root with both `EnableUnmatchedUdDiagnostic` and `EnableBopCatalogListener`; invoke the named narrow link helper once with the same switches. |
| Non-goals | No run in this admission; no full/product build, broad interceptor, device/configuration/firmware/profile/guest/CLI change, BOP result, or service implementation. |
| Verification | Root record has exactly 19 objects and four approved macros; one zero link exit; named executable/map/log/exit record exist. |
| Stop conditions | Collision, record mismatch, extra object/macro, nonzero link, or missing output terminates r12 with no repair/retry. |
| Exit criteria | A single link-built r12 artifact exists for separately admitted unchanged-input observation. |

**Result (r12 link complete):** Fresh root `artifacts/build/bochs-2.6-native-startup-transaction-msvc-r12-bop-listener` linked with exit `0`. Its record declares exactly 19 closure objects, both the unmatched-UD diagnostic and BOP-listener booleans true, and exactly the four approved opt-in macros. The named executable, map, build log, and exit record are present. This is a closure proof only, not a guest-run or BOP-result claim.

### S6 r12 BOP Listener Runtime-Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Observe the source-built NTIO prefix once with the r12 catalogue listener enabled, retaining the exact sequence through the first unimplemented BOP. |
| Scope | Use the existing real-NTIO observer once against the fresh r12 binary and unchanged BYOB inputs, with a 30-second watchdog and a unique evidence root. Correct only the observer predicate so a passive BOP log is counted separately from a retired broad interceptor. |
| Non-goals | No BOP implementation, result/resume change, Bochs/adapter runtime source change, device/firmware/PIC/FPU change, host capability, CLI expansion, or retry. |
| Verification | Retained JSON/log must show the accepted `50:11` commit and reached BOP identities in order; an unimplemented BOP must be followed by original unmatched-UD. |
| Stop conditions | Missing log/JSON, altered input hashes, more than one startup commit, broad interceptor, or claimed resume for an unimplemented BOP terminates the root. |
| Exit criteria | One immutable trace identifies the next source-defined BOP candidate without treating guest progress as runtime success. |

**Result (trace retained; no new BOP enabled):** `artifacts/analysis/t95-s6-r12-bop-listener-010-20260811-001` records the listener before existing `50:11` commit, then BIOS `12h` at `010D:0516`, immediately followed by original unmatched-UD. The watchdog ended the process and controlled stop was not reached, so it is fail-closed routing evidence only. Detailed correlation and the next isolated candidate are in `etc/research/t95-s6-r12-bop-listener-trace-001.md`.

### S6 BOP `12h` Typed-Result Adapter Closure (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Make the first observed but unimplemented BOP available to a future generic Bochs CPU-result consumer without adding BOP semantics to Bochs or enabling a runtime path. |
| Scope | Add an adapter-only bridge from the already source-proven `bx_ntvdm_bios_memory_service_v1_dispatch` result to `bx_ntvdm_cpu_result_v2`. It may select only the existing exact real-mode `12h` service; every other event returns a valid typed pass-through. Add one explicit CMake target and an independent C11 positive/negative test. |
| Non-goals | No `exception.cc` runtime wiring, macro, build-root/link change, Bochs result application, guest run, BOP dispatcher, memory read, BDA emulation, PIC/FPU/device change, CLI or host capability. |
| Verification | Exact `C4 C4 12` must produce only the existing AX low-16 delta `027f` and `RIP+3`; wrong selector and protected mode must remain typed pass-through with no GPR/FLAGS result. |
| Stop conditions | Any extra BOP, pointer/Bochs input, non-typed result, FLAGS/segment/memory mutation, or source behavior difference stops the work before bridge wiring. |
| Exit criteria | A self-contained adapter result can be consumed only by the separately registered generic `BX-ABI-015` path. |

**Result (adapter closure complete; runtime still disabled):**
`artifacts/build/t95-s6-bop12-cpu-result-bridge-ucrt64-002` compiles and runs
the exact eight-source C11 target with exit `0`, marker `bx-ntvdm CPU-result
bridge: exact BOP 12h result verified`, and executable SHA-256
`5A15840DB0EB1AD5F3C88C8D7AA5762F35AA8D94F5B34F565085A32C3748CA1C`.
Root `001` is retained terminal evidence of the incomplete initial write batch
(missing test source), not a code diagnostic. No Bochs path has been enabled.

### S6 r13 Narrow Typed-Result Bridge Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Consume the already closed adapter typed result through registered `BX-ABI-015`, so the reached `12h` boundary may resume without exposing BOP semantics to Bochs. |
| Scope | Add one default-zero `BX_NTVDM_ENABLE_CPU_RESULT_BRIDGE` opt-in. Within the existing narrow startup-transaction #UD path only, build the existing copied event, request an adapter typed result, validate it, apply only its generic low-16 GPR delta and explicitly permitted CF delta, then use its checked resume RIP. The root generator/link helper may include exactly the two result-bridge adapter objects under the same explicit switch. |
| Non-goals | No selector/service/address/value branch or BOP name in Bochs; no broad runtime/interceptor, generic dispatcher, CPU decoder change, memory/device/firmware/PIC/FPU change, host capability, CLI change, OpenNT edit, or full build. |
| Verification | Static boundary checks must prove default-off isolation, no BOP vocabulary in the Bochs bridge block, pass-through on a declined adapter result, generic delta-before-resume order, and exact closure/macro declaration. A fresh root/run is separately admitted only after link closure. |
| Stop conditions | Any bridge result read from guest memory, pointer/object ABI, selector-specific Bochs branch, unsupported FLAGS/segment/memory mutation, extra lifecycle hook, broad interceptor opt-in, or extra adapter object stops before link/run. |
| Exit criteria | One mechanical Bochs result consumer can support this and later admitted typed adapter results without becoming a BOP dispatcher. |

**r13 link result:** Fresh root `artifacts/build/bochs-2.6-native-startup-transaction-msvc-r13-bop12-result` linked with exit `0`. Its immutable record has 21 objects and exactly `EXECUTION_PLAN`, `STARTUP_TRANSACTION`, unmatched-UD diagnostic, BOP-listener, and CPU-result-bridge opt-ins. The executable SHA-256 is `418D355D21364282DDFD10C9EB00A28E28FB127B2F25EF75AE28A51C93444C69`.

### S6 r13 BOP `12h` Runtime Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Run the unchanged source-built NTIO profile once through the r13 result bridge and record whether the first observed `12h` boundary is mechanically resumed. |
| Scope | One unique evidence root, existing observer, existing BYOB inputs, and a 30-second watchdog. Preserve the complete log and hash-pinned observation record. |
| Non-goals | No retry, guest fix, extra BOP implementation, device/firmware/IRQ/FPU/host/CLI change, or claim of boot success. |
| Verification | The trace must retain first `50:11` commit, `12h` listener identity, and then distinguish checked result resume from unmatched #UD by the absence/presence of the original unmatched record at `010D:0516`. |
| Stop conditions | Missing evidence, changed input identity, broad interceptor, duplicate startup commit, or watchdog outcome is retained as evidence and receives no repair in this admission. |
| Exit criteria | One immutable causal trace classifies the consequence of the first typed BOP result. |

**Result (first typed result consumed; no boot-success claim):** The unique
root `artifacts/analysis/t95-s6-r13-bop12-result-011-20260811-001` pins the
r13 executable and unchanged guest/firmware inputs. It records listener
`50:11`, its one existing write commit, then listener `12h`; unlike r12, no
unmatched-UD record occurs at `010D:0516`. This is direct evidence that the
adapter's typed AX/resume result was consumed. The next reached BOP is DOS
family `50:3b` at `8DC8:055C`, which remains listener-only and then follows
the original unmatched-UD path. The watchdog ended the process; the outcome
proves only the `12h` transition, not NTDOS boot, DOS service correctness, or
controlled stop.

### S6 DOS `50:3B` Non-Invasive Debug-Flag Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Rehost the reached OpenNT `SVC_ISDEBUG` call without importing the historical host-debugger probe into the CLI product. |
| Scope | Replace the adapter's host `ProcessDebugPort` probe with the explicit non-invasive CLI policy `AL=0`, preserving the caller's AH and all non-AX state, and resume by four BOP-family bytes. Extend the narrow typed-result bridge only to offer this exact service after `12h`. |
| Non-goals | No debugger attachment/detection, Windows debug API, symbol loading, tracing, arbitrary DOS service dispatch, guest-memory access, Bochs semantic branch, host drive/filesystem, device, CLI option, or OpenNT edit. |
| Verification | Exact real-mode `C4 C4 50 3B` must yield `AL=0`, unchanged AH, `RIP+4`, no FLAGS delta; wrong service/mode must pass through. Source tests reject Windows/debug API terms. |
| Stop conditions | Any host debugger state query, result beyond AX low byte, unbounded service table, selector branch in Bochs, or runtime wiring before adapter proof stops this admission. |
| Exit criteria | The first reached DOS-family service has an explicit, portable, source-anchored non-invasive CLI contract. |

**Result (adapter contract complete):** The exact nine-source UCRT64 C11
fixture in `artifacts/build/t95-s6-dem-isdebug-cli-ucrt64-001` exits `0` and
prints `bx-ntvdm DEM debug service: non-invasive CLI false result verified`.
It proves `AL=0`, preserved AH, four-byte resume, and no result for a wrong
service. The CPU-result bridge now offers this service only after `12h` has
declined. The r13 build root is not altered; a future fresh root adds the
third bridge object before runtime observation.

**r14 link result:** Fresh root `artifacts/build/bochs-2.6-native-startup-transaction-msvc-r14-isdebug-cli` linked with exit `0`, declares exactly 22 objects and the same five explicit opt-ins as r13. Its executable SHA-256 is `279B175FCB6B1ED649DD5207FF3B06A1B73AD1396C807AF1256A377CE49C1C34`.

### S6 r14 `50:3B` Runtime Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Run the unchanged source-built NTIO profile once through the non-invasive `50:3B` contract and preserve the next exact reached boundary. |
| Scope | One unique evidence root using the existing observer and 30-second watchdog against r14. |
| Non-goals | No retry, debugger API, new BOP, device/firmware/host/CLI change, or DOS boot success claim. |
| Verification | Retain first `50:11`, `12h`, and `50:3B` listener sequence; distinguish `50:3B` consumption from its prior unmatched record, then classify only the next boundary. |
| Stop conditions | Missing evidence, changed hashes, broad interceptor, duplicate startup write, or watchdog is terminal evidence only. |
| Exit criteria | One immutable trace proves or rejects the second typed result without widening the bridge. |

**Result (second typed result consumed):**
`artifacts/analysis/t95-s6-r14-isdebug-cli-012-20260811-001` records the
unchanged `50:11 -> 12h -> 50:3b` prefix with no unmatched record for `50:3b`.
It reaches the next DOS-family BOP `50:0f` at `9346:7439`, where the listener
records it and the original unmatched-UD path remains active. The process
reaches Bochs's native controlled stop in 3.908 seconds (exit `1`), without a
watchdog kill or broad interceptor. This proves only the two typed results and
the next source-defined candidate; it is not a DOS boot or host-drive claim.

### S6 DOS `50:0F` Frozen-Inventory Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Rehost reached `SVC_DEMGETDRIVES` from the BYOB profile's immutable drive inventory, without guest-triggered host enumeration. |
| Source contract | `demgset.c:351-419` initializes A/B then counts the contiguous C..Z physical types removable/fixed/CD-ROM/RAM disk; it calls `setAX(nDrives)` and `setCF(0)`. The reached NTDOS caller stores AL in `numio`. |
| Current boundary fact | `bx_ntvdm_dem_drive_service_v1_dispatch` already implements this exact typed result from a supplied 26-byte inventory. The selected r14 profile carries `host_drive_inventory.types` as all zeroes, but the narrow startup session retains only images, not this profile-owned identity. |
| Scope | Extend only the narrow same-island session/CPU-result endpoint to retain the validated profile inventory at install time and offer it to the existing exact service. The session must never enumerate, rescan, query, or expose host paths. |
| Non-goals | No host-drive discovery, CLI option, DOS filesystem, directory/search behavior, guest memory, broad adapter runtime, Bochs semantic branch, device, or OpenNT edit. |
| Verification | All-zero profile inventory yields AX=2 and CF=0 at exact real-mode `50:0F`; nonmatching/malformed state passes through; source tests prove no host enumeration term. |
| Stop conditions | Any guest-triggered rescan, mutable host handle/path, ambient-drive fallback, service-table expansion, or new Bochs behavior stops the admission. |
| Exit criteria | The third reached BOP is resolved solely by an immutable, CLI-admitted profile snapshot. |

**r15 result (ownership boundary discovered):** Fresh root
`artifacts/build/bochs-2.6-native-startup-transaction-msvc-r15-demgetdrives`
links 23 objects (SHA-256 `112543A2C2A25EE98577BF43D4A367CAEEBE6BE7D4C26195D2A9DC6F0B051D46`).
The immutable observation
`artifacts/analysis/t95-s6-r15-demgetdrives-013-20260811-001` again reaches
`50:0f` at `9346:7439`, but it remains unmatched. This is not a drive-service
semantic failure: the startup transaction has installed its own narrow
session, while the new inventory endpoint reads the distinct startup-session
environment instance, which was never installed by this profile. The next
action is a single ownership correction: expose the already installed
transaction session's immutable inventory through its existing bridge, not a
second environment install or host enumeration. The controlled stop remains
native and no watchdog/broad interceptor occurred.

**r17 result (third typed result consumed):** Fresh root
`artifacts/build/bochs-2.6-native-startup-transaction-msvc-r17-demgetdrives-owner`
links the same 23-object closure (SHA-256
`451633D49DF1DB591D135EA38100768EC032B9BD22700B9EA27D9854C54B4446`).
Its immutable observation
`artifacts/analysis/t95-s6-r17-demgetdrives-owner-014-20260811-001` proves
`50:0f` is consumed: the next listener record is `50:1b` at `9346:7484`, with
no unmatched record at the prior `50:0f` location. It reaches native controlled
stop in 3.802 seconds, without watchdog or broad interceptor. This proves only
the frozen-inventory result and names `50:1b` as the next source audit target.

### S6 DOS `50:1B` Narrow DTA-Registration Bridge Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Connect the already tested source-derived DTA registration state to the narrow runtime without importing the broad adapter runtime. |
| Existing proven semantics | `bx_ntvdm_dem_dta_service_v1_dispatch/complete` accepts only `C4 C4 50 1B`, requests exactly two ordinary RAM bytes for `DOSWOWDATA.lpSftAddr`, derives four physical guest locators, and returns `RIP+4`. It carries no host pointer. |
| Scope | Add one pending, same-boundary two-byte ordinary-read state to the startup transaction bridge; add one narrow adapter action/completion endpoint. The existing generic Bochs ordinary-RAM reader may copy only the declared span, then apply the returned typed result. |
| Non-goals | No broad adapter runtime/host session, general guest read API, profile/environment reread, arbitrary ranges, host paths/handles, filesystem/search implementation, BOP branch in Bochs, device/firmware change, or OpenNT edit. |
| Verification | Exact `1B` positive and mismatched/repeated/boundary mismatch negatives; source checks prove one fixed two-byte read and no broad-runtime input; a fresh runtime trace must reach the next BOP. |
| Stop conditions | A second read, pointer crossing, unbounded range, lifecycle hook, non-ordinary mapping, guest-memory parse beyond the two bytes, or need for a new CPU/device feature stops this admission. |
| Exit criteria | The DTA registration becomes the source-closed state input for the already planned paired search capability, without adding an ad-hoc service plane. |

**r20 result (fourth reached service consumed):** The dedicated C11 fixture
`artifacts/build/t95-s6-dta-bridge-c11-ucrt64-001` passes its exact two-byte
read, mismatched service, and repeated-completion cases.  Fresh root
`artifacts/build/bochs-2.6-native-startup-transaction-msvc-r20-dta-registration`
links the 25-object narrow closure, with no broad interceptor, and its
executable SHA-256 is
`CBE97A905559DC753EA034D43854AFA4B94397A50D029C3DFF26543D4B374CD8`.
The immutable observation
`artifacts/analysis/t95-s6-r20-dta-registration-015-20260811-001` reaches
`50:1b` and then `50:32` at `9346:7490`; there is no unmatched record at the
previous `50:1b` location. It reaches the native controlled stop in 3.692
seconds, without a watchdog or broad interceptor. This proves only the DTA
registration transition; `50:32` is the next source-audit candidate.

**Generator-rejection evidence:** retained fresh roots `r18` and `r19` stop
before compilation because the narrow generator's original blanket
`guest_read` rejection incorrectly rejected the now-admitted fixed action
object (and then its source rule). The generator now rejects every other
guest-read object while allowing only `bx_ntvdm_guest_read_action_v1`; r20 is
the first accepted root. No earlier root was overwritten or reused.

### S6 DOS `50:32` Hard-Error-Locator Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Consume reached `SVC_DEMSETHARDERRORINFO` as its original initialization-time locator registration, without implementing the later hard-error workflow. |
| Source contract | `dossvc.h:142` maps `32h`; `demdisp.c:149` dispatches `demSetHardErrorInfo`; `demerror.c:25-39` stores only `GetVDMAddr(DS,DX)` and `GetVDMAddr(DS,BX)`. `msinit.asm:337-343` supplies those two DOSKRNL locations. |
| Scope | Offer the existing exact real-mode typed service through the existing CPU-result bridge and retain its two bounded physical locators in the already installed narrow transaction bridge. |
| Non-goals | No hard-error dialog, retry, device-chain traversal, host error mapping, guest memory read/write, filesystem, broad runtime/host session, Bochs service branch, device/firmware/CLI/OpenNT change. |
| Verification | Existing exact-service positive/mismatched tests remain required; a new bridge result must prove `RIP+4` and both locators, then one fresh trace must reach the next BOP. |
| Stop conditions | Any dereference of either locator, host error behavior, non-real-mode dispatch, generic service table, pointer crossing, or extra Bochs action stops this admission. |
| Exit criteria | The fifth reached BOP records the original locations only and preserves the native fallback for all later hard-error behavior. |

**r21 result (fifth reached service consumed):** Fresh root
`artifacts/build/bochs-2.6-native-startup-transaction-msvc-r21-hard-error-registration`
links the 26-object narrow closure; its executable SHA-256 is
`3914FD055085A0899774F8C0CEBBBB1B66F82BA0AAE9577D1AB53BF61489C2B5`.
The immutable observation
`artifacts/analysis/t95-s6-r21-hard-error-registration-016-20260811-001`
records the unchanged prefix through `50:32`, then the next listener identity
`54:05` at `9346:749d`, with no unmatched record at `50:32`. It reaches native
controlled stop without watchdog or broad interceptor. The result proves only
the two locator registrations; `bop.h` identifies selector `54` as `BOP_CMD`,
and `cmdsvc.h:21` identifies service `05` as `SVC_CMDSETINFO`. It is the next
source-audit candidate, not a redirector or local-drive service.

### S6 CMD `54:05` SCS-Location Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Consume reached `SVC_CMDSETINFO` as its original four-location COMMAND/NTDOS registration without starting a console or rehosting COMMAND. |
| Source contract | `bop.h:29` names `54h` `BOP_CMD`; `cmdsvc.h:21` maps `05h`; `cmddisp.c:21` dispatches `cmdSetInfo`; `msinit.asm:346-352` supplies DS:DX/BX/CX. The source-derived service preserves `RIP+4` and records SCS info, the source-defined `+206` sync flag, DOS-binary flag, and FD-access location. |
| Scope | Offer the existing exact typed service through the existing CPU-result bridge and retain only the four physical guest locators in the installed narrow transaction bridge. |
| Non-goals | No console creation, event thread, COMMAND process, COMSPEC, environment, file access, guest memory read/write, CLI change, broad runtime/host session, Bochs behavior, or OpenNT edit. |
| Verification | Existing exact/mismatched service test, source exclusion checks, one fresh link and one trace reaching the next BOP. |
| Stop conditions | Any locator dereference, host console/event behavior, pointer crossing, general command dispatcher, or added Bochs action stops this admission. |
| Exit criteria | The sixth reached BOP records only its original locations and leaves all later COMMAND behavior unimplemented/pass-through. |

**r22 result (sixth reached service consumed; watchdog retained):** Fresh root
`artifacts/build/bochs-2.6-native-startup-transaction-msvc-r22-cmd-set-info`
links the 27-object narrow closure; executable SHA-256 is
`E58AD62B9927394EF04B8752B2BEA11CF1FA3B6CBBF1F568556EEB5051B93D1A`.
The immutable observation
`artifacts/analysis/t95-s6-r22-cmd-set-info-017-20260811-001` reaches `54:05`
then `50:46` at `9346:7588`, with no unmatched record at `54:05` and no broad
interceptor. After the next unimplemented `50:46` it enters the existing
unmatched-UD loop and the 30-second watchdog terminates it. This is retained
as the correct terminal classification: it proves `CmdSetInfo` consumption and
the next candidate identity, but not a controlled stop or boot success.

### S6 DOS `50:46` Frozen-Inventory DPB-List Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Rehost reached `SVC_DEMGETDPBLIST` using only the startup session's frozen drive inventory, preserving OpenNT's intentionally minimal DPB-list writes. |
| Source contract | `dossvc.h:162`, `demdisp.c:169`, and `demgset.c:940-1003` identify the caller. The original explicitly writes only each physical drive's drive/unit fields and link, advances BP, and leaves other DPB fields unspecified because NTDOS does not maintain them. `msinit.asm:524-532` defines the no-entry case. |
| Scope | Use the existing `bx_ntvdm_dem_dpb_service_v1_prepare`: exact real-mode `50:46`, frozen inventory types, at most two disjoint writes per physical/removable drive, all-ranges preflight, atomic ordinary-RAM copies, exact BP delta and `RIP+4`. |
| Non-goals | No drive discovery/rescan, host path/media/filesystem access, DPB field expansion, guest reads, broad adapter runtime/host session, OpenNT or Bochs service branch, device/firmware/CLI change. |
| Verification | Existing exact DPB service test plus same-boundary/malformed/repeated-take negatives; static boundary proof that Bochs sees only opaque multi-write mechanics; one fresh trace reaches the next BOP. |
| Stop conditions | Any non-frozen host input, partial write, unpreflighted span, guest pointer, write beyond the two source-defined fields per entry, selector-specific Bochs logic, or additional device requirement stops the admission. |
| Exit criteria | `50:46` atomically emits the source-minimal DPB list or fails closed, without claiming a complete DOS disk implementation. |

**r25 result (seventh reached service consumed):** r24 retains the correct
terminal evidence that a zero-write DPB result was rejected by the generic
multi-write preflight. The source-defined zero-entry case is now admitted only
within the narrow DPB endpoint: zero spans, zero payload, valid BP/RIP result,
and no RAM mutation. Fresh root
`artifacts/build/bochs-2.6-native-startup-transaction-msvc-r25-dpb-empty`
links the 30-object closure; executable SHA-256 is
`5C848346B23635484DBE5A72E9ADD49F9AAD3E23FFC0F3AFAB5D252922C270CB`.
The immutable observation
`artifacts/analysis/t95-s6-r25-dpb-empty-019-20260811-001` has no unmatched
record at `50:46` and reaches BIOS selector `15h` at `8DC8:0655`; it then
records COMMAND `54:04`. The watchdog terminal outcome remains retained and
does not alter the BOP-consumption conclusion. No broad interceptor was used.

### S6 BIOS `15h` Extended-Memory Result Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Consume only the reached `BIOS_CASSETTE_IO` BOP when it is NTDOS's source-defined replacement for `INT 15h, AH=88h`, without introducing a general INT 15 or BIOS dispatcher. |
| Source contract | `bios.h:43` assigns selector `15h`; `bios.c:162` routes it to `cassette_io`; `tape_io.c` names this INT 15 owner. `doskrnl/bios/sysinit1.asm:1005-1020` loads `AH=88h`, executes `BOP 15h`, and stores AX as extended-memory KiB. The accepted bare-machine profile fixes `megs: 4`, hence its contiguous post-1MiB capacity is `0C00h` KiB. The historical service's `INT15_EMS_DETERMINE` branch supplies AX and does not synthesize a new interrupt return. |
| Scope | Extend the existing adapter-local BIOS-memory typed-result service with exactly real-mode `C4 C4 15`, pre-state AH=`88h`, AX=`0C00h`, `RIP+3`, and no FLAGS, segment, memory, device, host or Bochs query. |
| Non-goals | No other INT 15 subfunction, cassette, event wait, A20, block move, CMOS/device read, generic BIOS dispatcher, Bochs source/profile change, host capability, CLI change, or OpenNT source change. |
| Verification | Direct C11 contract test covers selector, AH gate, AX value and resume; source boundary checks and one later fresh-root trace must distinguish this consumption from the still-unimplemented `54:04`. |
| Stop conditions | Any reliance on Bochs internals/CMOS, guest-memory access, CF/FLAGS synthesis, value not fixed by the declared profile, or broadened selector/subfunction set stops this admission. |
| Exit criteria | The one reached AH=`88h` call resumes with the profile-derived AX result; all other `15h` calls remain listener-only/pass-through. |

**r26 result (eighth reached operation consumed):** The direct UCRT C11
contract executable printed `bx-ntvdm BIOS memory service: exact BOP 12h and
15h/AH=88h contracts verified`. Fresh root
`artifacts/build/bochs-2.6-native-startup-transaction-msvc-r26-bios15` linked
the unchanged 30-object closure; its executable SHA-256 is
`49875EBEDE3CEF75A3745C34C1A7DBC87F1644D0A589520E2E29662B0A2B3ABC`.
The immutable observation
`artifacts/analysis/t95-s6-r26-bios15-020-20260811-001` has no unmatched
record at `15h`, then reaches DOS `50:0d` at `8DC8:0667` and COMMAND `54:04`.
The watchdog remains downstream fail-closed evidence; no broad interceptor
was used. `etc/research/t95-s6-r26-bios15-trace-001.md` records the source
correlation and makes `SVC_DEMGETBOOTDRIVE` the only next service audit.

### S6 DOS `50:0D` Noninvasive Boot-Drive Fallback Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Consume the reached `SVC_DEMGETBOOTDRIVE` using the original OpenNT fallback result, without reading host registry state or exposing a host drive. |
| Source contract | `dossvc.h:105` and `demdisp.c:112` identify `50:0d` / `demGetBootDrive`. `demgset.c:90-147` initializes `Drive=3`, attempts the NT registry boot-drive value, rejects a non-fixed result, and on every failure reaches `DefaultBootDrive`, which calls only `setAL(Drive)`. `sysinit1.asm:1026-1028` consumes AL as the NTDOS default/boot drive. |
| Scope | Add one adapter-local typed result which accepts exactly real-mode `C4 C4 50 0D`, preserves AH, returns AL=`03h`, resumes `RIP+4`, and leaves FLAGS, segments and memory unchanged. |
| Non-goals | No registry query, host-drive snapshot, drive validation, path, filesystem, CLI option, guest read/write, DOS default-drive mutation, Bochs change, OpenNT edit, or general DEM dispatcher. |
| Verification | Direct C11 test covers exact selector, AL-only preservation, result/RIP and mismatch pass-through; a fresh trace must reach a new next BOP with no unmatched `50:0d`. |
| Stop conditions | Any ambient-host query, a value other than the original fallback, CF/FLAGS change, guest state access, service-family switch or Bochs selector logic stops this admission. |
| Exit criteria | Only the source-defined noninvasive C-drive fallback is consumed; later drive/current-directory semantics remain separately closed. |

**r27 result (ninth reached service consumed):** The direct UCRT C11 contract
executable printed `bx-ntvdm DEM boot-drive service: noninvasive C fallback
verified`. Fresh root
`artifacts/build/bochs-2.6-native-startup-transaction-msvc-r27-boot-drive`
linked the 31-object closure; executable SHA-256 is
`A57580CA0FA09B9105C2B53C0403C57FBB8E96C42958C2E4C9D5759D91178A8B`.
The immutable observation
`artifacts/analysis/t95-s6-r27-boot-drive-021-20260811-001` has no unmatched
record at `50:0d`, then reaches `54:04` at `9346:6774`, where the original
unmatched path remains active. Downstream listener records are not admitted
out of order. `etc/research/t95-s6-r27-boot-drive-trace-001.md` identifies
the required current-directory host-capability closure; no broad interceptor
was used and the watchdog remains terminal evidence only.

### S6 CMD `54:04` Current-Directory Capability Closure (2026-08-11)

**Result (design gate only):** Source/implementation review confirms that the
reached `SVC_CMDGETCURDIR` is the first real CLI-to-host-capability
composition boundary. `cmdmisc.c:517-568` requires drive validation,
per-drive state, a bounded DS:SI output and CF errors; `macro.asm:394-421`
consumes it to synchronize CDS. The existing CLI policy, immutable snapshot,
namespace roots and generic multi-write ABI provide the needed components,
but the r27 startup bridge rightly excludes their host lifecycle. The next
packet must join those components through one small adapter capability session
and a source-shaped transaction; it may not import `adapter_runtime`,
`host_session`, raw host paths or a Bochs selector branch. See
`etc/research/t95-s6-cmd-current-directory-capability-closure-001.md`.

### S6 CLI Namespace-Root And CMD `54:04` Implementation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Materialize the already-closed CLI policy → frozen host snapshot → namespace-root → bounded current-directory transaction path, then consume the reached CMD `54:04` without widening Bochs. |
| Scope | The narrow bridge may capture `NTDOS64_HOST_INCLUDE_DRIVES`/`NTDOS64_HOST_EXCLUDE_DRIVES` exactly once at install, initialize only the existing opaque namespace roots, and retain them session-locally. A new adapter CMD service may accept only real-mode `54:04`, AL/DS:SI plus the copied event/window, derive a root-only DOS string for an available admitted root, prepare one opaque atomic write and source-defined CF/AX result, and bind it to the existing generic multi-write consumer. |
| Non-goals | No `adapter_runtime`, `host_session`, search, file open/read/write, directory enumeration, ambient rescan, `GetCurrentDirectory`, host process environment mutation, host path exposure, current-directory changing service, general CMD dispatcher, Bochs source change or selector branch. |
| Verification | C11 service/session tests must cover include/exclude snapshot policy, absent drive error, exact root bytes, atomic write binding and one-time take. A future fresh-root trace must be launched through CLI-derived child environment and prove no unmatched `54:04`; it must retain the next BOP rather than claim DOS boot. |
| Stop conditions | Any guest-triggered host operation, host handle/path ABI crossing, more than one snapshot capture, unbounded guest string, output partial write, added Bochs semantic token, or import of rejected runtime/session objects stops the admission. |
| Exit criteria | CLI policy determines one immutable namespace-root set; `54:04` returns only source-shaped root/error results through generic mechanics; all later directory semantics remain closed. |

**r28/r29 result (CMD root transition consumed):** r28 is retained first-build
evidence that the existing full namespace enumerator is not yet part of this
MSVC closure; it failed at its NT directory enumeration type surface before
the new service compiled. No retry occurred. r29 instead links only the new
34-object snapshot-root set closure, SHA-256
`C6CE4112794A6EABA493A77711DDFABFD4B987FC200671E60D4ACA5DAFA2B69A`.
The direct C11 test proves exact root/error atomic contracts. Immutable r29
observation `artifacts/analysis/t95-s6-r29-cli-root-set-022-20260811-001`
has no unmatched record at `54:04`, then reaches CMD `54:0c`
`SVC_GETCONFIGSYS`; it is not a CLI-handoff or DOS-boot claim. Details and
the retained r28 rejection are in
`etc/research/t95-s6-r29-cli-root-set-trace-001.md`.

**r30 lifecycle confirmation:** Fresh root
`artifacts/build/bochs-2.6-native-startup-transaction-msvc-r30-cli-root-release`
rebuilds the same closure after adding explicit process-session root release;
the executable SHA-256 is
`1F1F0F4C42346AA29B093B177074E9DE29EA0055C95B45F1BBC2452C9C7ECC68`.
Its immutable observation
`artifacts/analysis/t95-s6-r30-cli-root-release-023-20260811-001` again has
no unmatched `54:04` and reaches `54:0c`; the watchdog is downstream evidence.

### S6 CMD `54:0C` CONFIG.SYS Capability Audit (2026-08-11)

**Result (listener retained; no dispatch admitted):** The first BOP after the
root-only current-directory transition is COMMAND `54:0c`,
`SVC_GETCONFIGSYS`, at `8DC8:0732`.  OpenNT's historical host owner expands a
selected CONFIG.SYS into a temporary file, returns an OEM temporary pathname
through a 64-byte `DS:DX` buffer, and terminates the VDM on failure; NTDOS
then parses that returned file.  The current immutable drive snapshot/root
handles have deliberately none of the required input-selection, bounded file
I/O, temporary-materialization, path-reachability, OEM conversion or cleanup
contract.  Its paired `54:0d` (`SVC_GETAUTOEXECBAT`) is called later by
COMMAND and immediately opened via normal DOS file I/O, so both services also
depend on a prior bounded DOS open/read projection.  A synthetic path or
ambient `C:\\CONFIG.SYS`/system `config.nt` access would violate the CLI
containment boundary.  They therefore remain pass-through/listener-only until
that projection and the paired CONFIG.SYS/AUTOEXEC.BAT capability are
separately admitted.
See `etc/research/t95-s6-cmd-getconfigsys-capability-audit-001.md`.

### S6 DOS Read-Handle Closure Audit (2026-08-11)

**Result (no new BOP dispatch):** The ordinary guest path following a returned
configuration filename is exactly DEM `50:12` `SVC_DEMOPEN`, `50:16`
`SVC_DEMREAD`, and `50:02` `SVC_DEMCLOSE`.  Their historical owners pass a
native handle through the NTDOS SFT and directly map guest pointers; that
representation is forbidden across the current adapter boundary.  A future
capability must instead rehost the complete read-only Open/Read/Close family
with copied bounded path/input, opaque adapter-local handle IDs, atomic guest
writes and source-shaped result/error/lifecycle behavior.  It is neither a
CONFIG BOP shortcut nor a Bochs feature.  All five reached/prerequisite BOPs
remain listener-only pending this coherent family admission.  See
`etc/research/t95-s6-dos-read-handle-closure-audit-001.md`.

### S6 DEM Read Transport Sizing Audit (2026-08-11)

**Result (mechanics design gate):** `DEMREAD` accepts a full unsigned
16-bit `CX` request and returns a possibly short result in `AX`; it is not
limited to the existing 256-byte gather input or 4096-byte structured
multi-write payload.  Those existing transports therefore cannot be reused
as if they implemented a general DOS read.  The coherent next prerequisite is
a separately versioned, generic bounded bulk-result transport covering at
most 65535 bytes, with full preflight and one atomic guest-visible commit.
It must carry no BOP/path/handle semantics.  The existing multi-write v1
limit remains unchanged for small structured results.  See
`etc/research/t95-s6-dem-read-transport-sizing-audit-001.md`.

**Error outcome constraint:** The historical shared `demClientError` path
distinguishes ordinary `AX`/CF errors from disk-class errors that update the
registered INT 24 VHE packet before returning CF. A future read-handle family
must retain that distinction through an explicit outcome record; it may not
flatten failures merely because `50:1B` has already recorded the VHE address.
Pipes and their special broken-pipe EOF path remain excluded. The closure
audit records the exact source range.

**Retry closure constraint:** A hard-error result is not self-contained. Its
three-byte VHE packet is followed by guest `50:33` retry control flow, whose
historical owner restores the saved DEM register frame and re-enters the
failed service. The initial read-only profile must therefore either close a
private retryable-operation record with that BOP, or reject disk-class errors
before presenting them; it may not publish a retryable VHE state with `50:33`
still listener-only.

### S6 DEM Open Input-Admission Audit (2026-08-11)

**Result (host-boundary gate):** `50:12` cannot use the historical native
`HANDLE` path: NTDOS's `SFT_NTHandle` is only a four-byte guest storage field,
so it must hold a session-private adapter token. The first profile can only
consider a bounded copied, explicit-drive regular-file path and a source-made
read-only mode subset. `EXEC_OPEN=3` remains deliberately unadmitted because
the historical owner requests read/write access for it; silently making it
read-only would be new CLI policy and needs reached-caller evidence. Pipes,
devices, EA, relative paths and mutation stay rejected. See
`etc/research/t95-s6-dem-open-mode-and-path-admission-001.md`.

**Error mapping:** The original SoftPC host seam converts native status through
`RtlNtStatusToDosError` before `demClientError` returns `AX`. The future
adapter-native relative-open/read seam must use that same one-way conversion
in a bounded outcome record; raw `NTSTATUS` and an invented per-platform table
are forbidden.

**Read-range closure:** NTDOS normalizes `DS:DX` before `50:16` and cuts `CX`
to prevent a 64 KiB segment crossing. A future bulk result therefore commits
one physical ordinary-RAM range derived from the *post-normalization* snapshot;
it must not reconstruct or wrap the original DOS API pointer.

**Capability-scope correction:** CONFIG.SYS `doconf` calls normal DOS Open,
then seeks to end, seeks to beginning, reads and closes. The actual first
read-only projection is therefore `50:00/12/16/02` (Open/Seek/Read/Close), not
the earlier three-service shorthand. All four stay listener-only pending one
coherent ABI/lifecycle admission.

**Offset ownership constraint:** NTDOS implements absolute seek locally in its
SFT, then signals a subsequent `50:16` to seek with ZF clear. The adapter's
private token record must therefore retain sequential offset and honor the
source ZF rule on every read; a guest SFT update alone is not a host seek.

**Token lifecycle constraint:** `50:02` treats token zero as a source-defined
successful no-op. Every allocated adapter token must therefore be nonzero;
unknown/stale nonzero tokens return ordinary `ERROR_INVALID_HANDLE` with CF,
never a host call or VHE mutation. Close may receive a final absolute offset,
and session teardown owns residual native-handle release.

**Seek ABI:** `50:00` takes signed `CX:DX` and source-defined methods begin,
current and end, returning `DX:AX`. The adapter must implement all three only
for a valid private read-only token; malformed methods stay listener/pass-
through because NTDOS rejects them before its normal caller reaches the BOP.

**Next generic mechanics admission:** `DEMREAD` needs a separately versioned
one-range `0..65535` bulk-result transaction, with a fixed adapter-owned
payload arena and the existing full-span Bochs ordinary-RAM preflight/copy.
It explicitly preserves zero-byte EOF results and does not widen multi-write
v1 or include BOP/DOS/file semantics. Design authority:
`design/ADAPTER-BULK-RESULT.md`.

**Bulk-result ABI v1 (adapter-only):** The pointer-free transaction record
and C11 preflight implementation now exist in
`src/bx-ntvdm-adapter/bx_ntvdm_bulk_result_transaction.[hc]`. Fresh VS x86
source build `artifacts/build/t95-s6-bulk-result-msvc-c11-001` passed its
65535-byte, zero-byte, aperture, payload-mismatch and resume-boundary
regression. It is not queued in a runtime session and has no Bochs consumer,
BOP dispatcher, file capability or host handle.

### S6 Detached Intact-Native Observation Admission (2026-08-11)

| Field | Record |
| --- | --- |
| Objective | Replace only the foreground-host timeout transport with one auditable detached execution of the already defined intact native target. |
| Scope | Add a launcher/monitor pair under `tools/`. The launcher creates one new hash-verified intact root through the existing generator, writes one root-local batch wrapper containing the recorded VS x86 setup and exact named NMAKE target, starts it hidden once, and records PID/command/hash. The monitor is read-only and reports process/exit/log state. |
| Non-goals | No retry, restart, source/config/generated-Makefile/shim edit, configure, `all`, VS solution, `bochs.exe`, target run/reset, adapter/CLI/OpenNT/guest input or runtime claim. The launcher never acts on a previous partial root. |
| Required evidence | New-root identity; wrapper content/hash; exact target command; hidden process PID/start time; complete log and atomic exit-code file; no automatic second invocation; monitor snapshots; artifact/map hashes only after exit. |
| Verification | Static review the generated wrapper; verify collision refusal and start record; monitor to a terminal exit without changing state; then classify build output against the manifest and run documentation governance verification. |
| Risks and stop conditions | Stop if the wrapper cannot record its final exit code, the process identity is lost, the root already exists, or any unexpected target/input appears. A nonzero exit is evidence and must not be retried. |
| Exit criteria | One detached process has a preserved command identity and terminal outcome, allowing native build classification without a foreground timeout. |
| Coordinator acceptance, 2026-08-10 | Accepted after an independent source review of `cpu/exception.cc:1192-1201`, `memory/misc_mem.cc:111-152`, `iodev/devices.cc:103-134,379-390`, `config.h`, and `vs2008/bochs.vcproj`. The MSVC/x86 compiler prerequisite is available, but no minimum build recipe is admissible: triple-fault behavior requires unresolved SIM/CMOS/reset ownership; original memory initialization mixes required allocation with SIM/state registration; original port-space initialization mixes required default arrays with product initialization. The required next action is an owner decision on the triple-fault branch: admit a bounded original-SIM parameter fragment that preserves Bochs behavior, or explicitly authorize a changed triple-fault outcome. Until then, no recipe, fixture build, or S5 native-evidence claim is permitted. |
| Owner decision, 2026-08-10 | The owner authorizes individually registered Bochs intrusions and delegates the implementation choice after registration. S6 selects the smallest original-SIM parameter fragment for the original triple-fault **false/shutdown** path; the hard-reset/CMOS path is a separate deferred closure. Before any source change, the coordinator must register the exact files, behavior, negative case, and removal/review condition in `docs/etc/research/adapter-external-intrusion-exceptions.md`. The fragment must not expose adapter/NTVDM/OpenNT semantics, load a GUI/text configuration front end, enable a CMOS device, or admit a product device suite. |
| Owner-decision correction, 2026-08-10 | Source review proves that the smallest support fragment cannot preserve a hard reset: `false` selects Bochs's original shutdown branch and never reads CMOS; `true` reads CMOS and resets the hardware/device graph. `BX-MACH-023` is therefore registered only for the original `false` parameter path. A real hard-reset/CMOS closure is explicitly deferred as a separate future exception, not silently included. |
| S6 next implementation package, 2026-08-10 | Coordinator accepted the executor's read-only closure audit: `BX_MEM_C` no-SIM memory state and `bx_devices_c` empty port space have disjoint ownership and must be two separately registered intrusions. `BX-MEM-024` and `BX-IO-025` are admitted only as a paired initialization package, with original allocation/default-handler behavior retained and all product configuration, state registration, timers, plugins, CMOS, devices, PCI and adapter/OpenNT semantics excluded. This does not admit a build target or a linkage claim. |
| MSVC closure acceptance, 2026-08-10 | Coordinator independently confirmed that `bx_ntvdm_minimal_sim_initialize`, `BX_MEM_C::init_memory_without_sim`, and `bx_devices_c::init_empty_port_space` have no admitted caller, while the required global `bx_pc_system`, `bx_cpu`, and `bx_mem` are presently defined by product `main.cc`. The historical VS projects are static-library/full-product evidence only. No MSVC recipe is legal yet. The next read-only S6 action is to derive one Bochs-local minimal-machine composition unit; it must own only native storage/lifecycle ordering and expose no adapter, guest, host-service, CLI, BOP or OpenNT term. |
| S6 composition admission, 2026-08-10 | The read-only lifecycle audit is accepted. `BX-MACH-026` is pre-registered for one Bochs-local composition class that defines the selected original globals without linking `main.cc`, calls only the three registered helpers plus original CPU initialize/reset, and has no CPU loop or product startup. It does not admit a configuration header, object/link recipe, native build, firmware, device, host-service or adapter behavior. |
| S6 composition implementation, 2026-08-10 | `BX-MACH-026` now implements the registered private C++ composition owner and two friend declarations only. Its focused static boundary test verifies selected globals, one-shot ordering, reverse owned cleanup, and exclusions. This is source-boundary evidence only: no recipe/configuration change, fixture build, firmware, device, adapter, or guest-runtime claim is admitted. |
| S6 configuration/link gate, 2026-08-10 | The accepted configuration/object audit is frozen in `etc/research/t95-s6-minimal-x86-config-link-closure-contract.md`. It records only source-backed CPU3/x86/no-device choices, marks every non-derived macro family unresolved, and requires a separately admitted MSVC `/Gy` + `/OPT:REF` map/import/undefined-symbol proof. No exact `config.h`, recipe, response file, build root, compiler/linker invocation, or fixture result is legal until that gate closes. |
| Controlled closure-proof admission, 2026-08-10 | The coordinator admits one analysis-only MSVC/x86 closure investigation under a fresh `artifacts/build/` root. It may copy the pinned Bochs input and create a generated configuration only inside that root, compile declared candidate sources with `/Gy`, and link only a non-product analysis fixture with `/OPT:REF`, a map, and verbose unresolved/import diagnostics. It must not modify `refs/bochs`, use the historical VS2008 solution, invoke `configure`, `bochs.exe`, or `all`, enable a rejected feature/device, link adapter/OpenNT/CLI code, or run an executable. Any source intrusion discovered as necessary stops the run pending a prior exception-register entry. The sole deliverable is evidence classifying the exact closure as available, externally blocked, or rejected. |
| S6 analysis attempt 001 rejection, 2026-08-10 | `artifacts/build/t95-s6-msvc-x86-comdat-closure-20260810-001` is preserved unchanged as rejected evidence. Its hand-authored analysis configuration assigned contract-unresolved SMF, A20, idle-hack, exit/frontend and platform families before effective-preprocessor/retained-symbol derivation; the retained `bochs.h:68` `unistd.h` C1083 is consequently non-decisive and establishes no closure classification. The next gate is a narrowly scoped documentation/read-only derivation of every candidate configuration macro as source-forced, platform-probed, or unresolved. No new artifact configuration, compile, link, or recipe is allowed before that gate. |
| S6 per-macro gate, 2026-08-10 | `etc/research/t95-s6-minimal-config-macro-ledger.md` freezes the required per-macro classifications and separates `_WIN32`/`_MSC_VER` compiler facts from Bochs GUI configuration. The next gate is non-building MSVC predefined-macro/header capability observation plus source dependency completion for every platform-probed and retained-symbol-dependent row. |
| S6 MSVC platform probe, 2026-08-10 | `etc/research/t95-s6-msvc-x86-platform-probe.md` records VS2022 x86/MSVC/SDK identity and installed header paths only. It proves neither `WIN32` mapping nor any `BX_HAVE_*` value; the next gate is a recorded non-product predefined-macro/header-declaration probe before any configuration proposal. |
| S6 non-product compiler probe, 2026-08-11 | The standalone x86 probe proves compiler `_MSC_VER=1943`, `_WIN32=1`, and `WIN32=1`, plus asserted ABI widths and selected UCRT declarations. This closes only the compiler-to-`WIN32` mapping question for `bochs.h`; `BX_WITH_WIN32` remains denied and every `BX_HAVE_*` semantic choice remains unselected. Next gate: per-row declaration/semantic matrix for remaining platform macros; retained-symbol rows remain closed. |
| Non-product platform-probe admission, 2026-08-11 | The coordinator admits one standalone MSVC/x86 C/C++ probe below a fresh `artifacts/analysis/` root. It may contain only probe source and captured preprocessor/syntax-only output; it must include no Bochs, adapter, OpenNT, CLI, or guest header, produce no linked executable, and never run. `/EP` and `/Zs` are permitted solely to record `_MSC_VER`/`_WIN32`, the derived `WIN32` mapping question, target ABI widths, and exact UCRT/Windows header declarations. It may not select a `BX_HAVE_*` value from similarly named APIs or change any Bochs configuration. The evidence result may close platform facts only; retained-symbol-dependent Bochs macro rows remain closed. |
| Configuration-shape matrix admission, 2026-08-11 | To close retained-symbol-dependent configuration shape without treating a hypothesis as a product configuration, the coordinator admits one fresh `artifacts/analysis/` root containing copied Bochs input and a finite matrix of generated analysis-only `config.h` variants. Each variant may be tested only with MSVC/x86 `/Zs` against the declared non-product minimal-machine root and candidate translation units; no object, link, executable, CPU loop, or run is permitted. The matrix must vary only explicitly named unresolved families (including SMF linkage and conservative platform/API fallbacks), record every value and hash, and stop on any need to patch `refs/bochs` or enable a denied feature. It may select a syntax-compatible candidate shape for the later `/Gy` closure proof, but never establish COMDAT/link/runtime closure or a Bochs product recipe. |
| S6 configuration-shape matrix 001, 2026-08-11 | `etc/research/t95-s6-msvc-x86-config-shape-matrix-001.md` freezes `artifacts/analysis/t95-s6-msvc-x86-config-shape-20260811-001` as externally blocked and unchanged. The first recorded Community toolchain path did not exist; the later BuildTools `/Zs` command did reach `cl.exe` but stopped at the project-local `WIN32`/`unistd.h` and then generated `instrument.h` header boundaries. No syntax-compatible variant, object, link, executable, or runtime claim exists. The single next gate is coordinator-owned verification of the exact approved MSVC toolchain entry, project mapping, and source-backed instrumentation-header selection before any new matrix root or invocation. |
| S6 second syntax-matrix admission, 2026-08-11 | `etc/research/t95-s6-instrument-header-and-win32-entry-audit.md` independently identifies the original no-op `instrument/stubs/instrument.h` as an unconditional header input when `BX_INSTRUMENTATION=0`, and `/DWIN32` as an MSVC platform definition distinct from the denied `BX_WITH_WIN32=0` GUI choice. The coordinator admits one fresh analysis root to repeat the finite `/Zs` matrix with only those original include/definition inputs and the source-template empty `BX_CPP_AttrRegparmN(n)` fallback. It may not copy/generate/edit instrumentation code, enable instrumentation/GUI/devices, make an object or executable, link/run, or change `refs/bochs`. |
| S6 original stub/Win32 entry audit, 2026-08-11 | `etc/research/t95-s6-instrument-header-and-win32-entry-audit.md` closes the read-only source question: `bochs.h` always needs the original `instrument/stubs/instrument.h`, even with `BX_INSTRUMENTATION=0`; original Makefile/VS metadata supply it via an include directory, while `WIN32` is an explicit `/DWIN32` translation-unit mapping and `BX_WITH_WIN32` remains `0`. The next gate is coordinator admission of one fresh, no-object `/Zs` second matrix using only that original stub include path and the source-template empty `BX_CPP_AttrRegparmN` fallback; it cannot enable instrumentation, GUI/device behavior, or a custom header. |
| S6 configuration-shape matrix 002, 2026-08-11 | `etc/research/t95-s6-msvc-x86-config-shape-matrix-002.md` records that the original stub header and `/DWIN32` let the minimal fixture pass `/Zs`, but the first candidate pass stops at original `config.h.in` extrema/format declaration omissions and then at `cpu/exception.cc`'s pre-existing out-of-tree `bx_ntvdm_cpu_state_abi.h` dependency. `b-smf000` was not invoked; no syntax-compatible SMF shape exists. The next gate is a coordinator choice: source-justify excluding `exception.cc` from the Bochs-local candidate set, or separately admit an exact same-island adapter-header input boundary. No object/link/run or feature enablement occurred. |
| S6 #UD seam boundary repair, 2026-08-11 | **Objective:** restore a Bochs-local default CPU exception compilation path without the pre-existing adapter #UD seam. **Scope:** only compile-time guarding of the existing includes, private helpers, state/types, and invocation in `refs/bochs/cpu/exception.cc`, plus the named static boundary test and exception record. **Non-goals:** no adapter/DOS/BOP behavior, ABI revision, decoder/memory/device change, seam deletion, build recipe, full build, or runtime fixture. **Evidence:** matrix-002 C1083 reaches the unconditionally compiled adapter ABI header; `BX-ABI-027` now records the repair. **Verification:** `Test-BochsUdDefaultOffBoundary.ps1` and the retained #UD seam test pass: local `BX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=0` excludes adapter headers/types/symbols unless explicitly nonzero, while original `LOG_THIS` and its post-`BX_INSTR_EXCEPTION` logging path remain outside the guard. **Risks:** an incomplete guard could leave an adapter type or call in the default path, or a broad macro could silently enable it. **Stop:** stop if guarding requires another source file, configuration-wide default, adapter change, or semantic change. **Exit:** source-boundary evidence is complete, not a build/runtime claim. **Next gate:** a separately admitted fresh syntax matrix may use the default-off `exception.cc` and only original `config.h.in` extrema/format macros; it must not reintroduce adapter input. |
| S6 configuration-shape matrix 003, 2026-08-11 | `etc/research/t95-s6-msvc-x86-config-shape-matrix-003.md` freezes `artifacts/analysis/t95-s6-msvc-x86-config-shape-003-20260811-001` after its sole `a-smf111` MSVC x86 `/Zs` pass. The original `config.h.in:482-497` extrema additions were insufficient: the effective declaration path leaves `FMT_LL` unresolved in SIM/parameter sources and `GET32L`/`GET32H` unresolved in `exception.cc`. `b-smf000` was generated but intentionally uninvoked; neither SMF shape is selected. No further hand-authored macro additions, object/link/run, adapter input, or feature enablement occurred. The sole next gate is coordinator choice of an auditable projection of original MSVC configuration/platform mechanics with explicit denial overlay versus an original generated-configuration source; no new config/matrix/build action is admitted. |
| S6 auditable MSVC projection attempt 001, 2026-08-11 | `etc/research/t95-s6-msvc-config-projection-attempt-001-blocked.md` records two tool-level validation failures before any artifact output: a null `#define` value parser fault in `Project-BochsConfig.ps1`, then an invalid `New-Item -LiteralPath` test-harness invocation. The intended fresh projection root does not exist; no projected config/report, compiler, object/link/run, or syntax result exists. Narrow in-flight corrections and a later collection-shape diagnostic are explicitly unverified, not an accepted repair. **Next gate:** coordinator review of the tool design/error handling only; no automatic tool retry, source/tool edit, analysis-root write, configuration, matrix, or build action is admitted. |
| S6 projection-tool repair admission, 2026-08-11 | **Objective/scope:** repair only `Project-BochsConfig.ps1`'s internal macro-definition collection so each macro key owns an explicit definition-record collection, never a `+=` singleton `PSCustomObject`; extend its focused synthetic test for singleton, duplicate, and empty `#define` behavior. **Non-goals:** no invocation against `refs/bochs/config.h`, no real analysis root/projection, configuration selection, compiler/link/run, or Bochs/source change. **Verification/evidence:** tests must prove one define counts once, duplicate definitions fail closed, empty values parse safely, and failures create no output root. **Stop/next gate:** stop on any non-collection repair or failed test. Only after passing evidence may the coordinator review and separately admit one real projection invocation. |
| S6 projection-tool repair, 2026-08-11 | `etc/research/t95-s6-msvc-config-projection-tool-repair.md` records the accepted narrow representation repair: every parsed macro now owns an explicit `ArrayList` and replacement validation uses its direct count/index, avoiding singleton `PSCustomObject` enumeration. The synthetic-only focused test passes deterministic output, singleton/empty preservation, and fail-closed duplicate/missing/unexpected cases with no artifact projection root. This proves a tool boundary only; it does not validate the pinned projection manifest or invoke Bochs. **Next gate:** coordinator review, then separately admit exactly one real fresh-root projection invocation; any `/Zs` syntax observation remains separately gated. |
| S6 actual projection 002, 2026-08-11 | `etc/research/t95-s6-msvc-config-projection-002.md` records the one admitted invocation against pinned `refs/bochs/config.h`. `artifacts/analysis/t95-s6-msvc-x86-config-projection-002-20260811-001` retains only `config.h` (`A77D063C5960C9825F218253085BA7EEE288E2653E2CC07D568259CDCF0C9A11`) and `projection-report.json`; pinned source hash remains `A1FADC25502FB0A4644917B6F3A62987C125CC7335457F9C59E7CE7E62A45AC8`. Independent review confirms all 46 replacement/addition records and 2 derived assertions exactly once, with no extras. No tree copy, compiler, `/Zs`, link/run, or product selection occurred. **Next gate:** coordinator review and a separate syntax-only admission decision. |
| S6 projected syntax observation 001, 2026-08-11 | `etc/research/t95-s6-msvc-projected-syntax-001.md` freezes one BuildTools x86 `/Zs` projection-baseline observation in `artifacts/analysis/t95-s6-msvc-x86-projected-syntax-001-20260811-001`. The copied root config equals accepted projection `A77D063C5960C9825F218253085BA7EEE288E2653E2CC07D568259CDCF0C9A11`; pinned source config remains `A1FADC25502FB0A4644917B6F3A62987C125CC7335457F9C59E7CE7E62A45AC8`, and original `instrument/stubs` was present. The sole command used `/Zs /DWIN32 /DBX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=0`, produced zero objects, and stopped at `FMT_LL` C3688 in SIM/parameter sources; no SMF variant, retry, header/source input, feature enablement, link, or run occurred. **Next gate:** coordinator review of the preserved original MSVC format-token mechanism; no automatic configuration or syntax action is admitted. |
| S6 format-token projection v2, 2026-08-11 | `etc/research/t95-s6-msvc-format-token-projection-v2.md` records the controlled v2 delta: immutable v1 manifest SHA-256 `6815D8BD419ED9522C98F9DF55D242FB7037F42125C4504EC5766AE732FB9E28` plus one absent `FMT_LL` addition with literal `"I64"`, justified by original SIM/parameter callsites and the MSVC `osdep.h` format-token family. Extended synthetic tests prove v1 preservation and exact v2 addition semantics. The sole actual v2 root `artifacts/analysis/t95-s6-msvc-x86-config-projection-003-20260811-001` retains only config/report; output hash is `C47030F79880C894A346F56E3AB3242924308983E00B5845EDCE849450C6C19D`, input remains `A1FADC25502FB0A4644917B6F3A62987C125CC7335457F9C59E7CE7E62A45AC8`, and independent report review confirms 47 changes plus 2 assertions exactly once. No tree copy or `/Zs`/build/run occurred. **Next gate:** coordinator review before a separately admitted syntax observation. |
| S6 projected syntax observation 002, 2026-08-11 | `etc/research/t95-s6-msvc-projected-syntax-002.md` freezes the one v2 BuildTools x86 `/Zs` observation. Copied root config equals `C47030F79880C894A346F56E3AB3242924308983E00B5845EDCE849450C6C19D`; pinned config remains `A1FADC25502FB0A4644917B6F3A62987C125CC7335457F9C59E7CE7E62A45AC8`; original stubs are present and zero objects exist. Original `osdep.h:59` redefines projected `FMT_LL` (C4005), yet modern MSVC C3688 still rejects the original adjacent-token callsites. This is a source-token boundary, not a new config/feature omission. No retry, variant, source/header/config addition, link, or run occurred. **Next gate:** coordinator review of a precise source-token-boundary exception or evidenced original compatibility mechanism; no automatic action is admitted. |
| S6 FMT_LL lexical compatibility repair admission, 2026-08-11 | **Objective/scope:** `BX-ABI-028` may alter only every original source occurrence where a string literal directly abuts `FMT_LL`, adding lexical whitespace while preserving macro definitions, literal bytes, arguments, and numeric behavior. Read-only full-tree inventory finds 14 occurrences across 7 imported files: `gui/paramtree.cc` (2), `gui/siminterface.cc` (2), `iodev/hdimage/hdimage.cc` (1), `iodev/usb/scsi_device.cc` (3), `iodev/harddrv.cc` (2), `iodev/pit.cc` (3), and `iodev/virt_timer.cc` (1). **Non-goals:** no config/format macro/feature/adapter/DOS change, compile/link/run, or unrelated formatting. **Verification/stop:** a focused static test must reject every direct adjacency and require expected spaced forms; stop if another file or semantic change is needed. |
| S6 FMT_LL lexical compatibility repair, 2026-08-11 | `BX-ABI-028` is recorded in the exception register and `etc/research/t95-s6-fmt-ll-lexical-compatibility-repair.md` records its exact seven-file/14-occurrence scope. The imported source now adds whitespace only at every direct string-literal/`FMT_LL` adjacency; all literal and macro content is preserved. `Test-BochsFmtLlLexicalBoundary.ps1` passes full-tree absence, expected-spaced-form, and unchanged-`osdep.h` definition checks; retained #UD and projection-tool tests also pass. No config projection, `/Zs`, compiler/object/link/run, feature, or adapter/DOS change occurred. **Next gate:** coordinator review before any separately admitted syntax observation. |
| S6 FMT_LL lexical test-coverage correction, 2026-08-11 | Conditional acceptance identified a narrow test defect only: the registered 14 count was not asserted and the scan omitted `*.c`. Scope remains `BX-ABI-028` only. The test may be strengthened to scan all imported `*.c`/`*.cc`/`*.h`, require exactly the registered seven files and per-file 2/2/1/3/2/3/1 spaced-form occurrence counts, reject an unexpected source file or count, retain unchanged macro-definition and zero-unspaced checks. No Bochs/config/manifest/exception-register edit, projection, `/Zs`, compile/link/run is admitted. |
| S6 FMT_LL lexical test-coverage correction result, 2026-08-11 | The strengthened `Test-BochsFmtLlLexicalBoundary.ps1` passes: it scans complete imported `*.c`/`*.cc`/`*.h`, rejects any unspaced adjacency, locks the original five `osdep.h` definitions, derives exactly the seven registered repair files, and asserts 14 occurrences at `2/2/1/3/2/3/1`. Retained #UD default-off/interception, projection-tool, governance, and safe diff checks pass. No Bochs/config/manifest/exception-register or source change beyond the test, and no projection/`/Zs`/compile/link/run occurred. **Next gate:** coordinator re-acceptance of `BX-ABI-028`, then separate syntax-observation admission only. |
| S6 projected syntax observation 003 admission, 2026-08-11 | Coordinator accepts `BX-ABI-028` after independent full C/C++ 7-file/14-count, macro-lock, #UD, projection, and governance verification. Exactly one fresh `artifacts/analysis/t95-s6-msvc-x86-projected-syntax-003-20260811-001` root may copy the pinned repaired `refs/bochs`; its original copied root config must hash `A1FADC25502FB0A4644917B6F3A62987C125CC7335457F9C59E7CE7E62A45AC8` before replacement, then only that config is replaced with v2 `C47030F79880C894A346F56E3AB3242924308983E00B5845EDCE849450C6C19D`. It must retain original stubs and all 14 lexical repairs. One BuildTools x86 `cl /nologo /Zs /DWIN32 /DBX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=0` batch may inspect the prior fixture and declared candidates. **Stop/non-goals:** no retry/flag/SMF/config/source/header change, `/c`/`/Fo`, object/link/run/configure/VS project/`bochs.exe`/`all`; syntax closure requires every declared unit to pass this sole command. |
| S6 projected syntax observation 003, 2026-08-11 | `etc/research/t95-s6-msvc-projected-syntax-003.md` records the sole admitted v2 BuildTools x86 `/Zs` batch. All declared units, including fixture, SIM/parameter and `exception.cc`, pass with exit 0; zero errors and zero objects. The copied original root config was `A1FADC25502FB0A4644917B6F3A62987C125CC7335457F9C59E7CE7E62A45AC8` before only root replacement with v2 `C47030F79880C894A346F56E3AB3242924308983E00B5845EDCE849450C6C19D`; original stub and 14 lexical repairs were verified. The log retains 16 C4005 `FMT_LL` redefinition warnings; no suppression/config change is implied. No link/run/configure/VS project occurred. **Next gate:** coordinator review before any separately admitted object/COMDAT/link closure work. |
| S6 object/link-closure audit admission, 2026-08-11 | Coordinator admits a read-only source-level ledger beginning only from syntax-003's exact fixture and declared candidate translation units. It may identify each candidate's external symbols/types/globals and imported definitions/objects; classify them required for minimal reset-to-bounded-stop observation, default-but-not-required, explicitly rejected, or unresolved; and identify the smallest possible object set without claiming buildability. Historical VS2008/full `bochs.exe`/iodev aggregates are evidence only. **Stop/non-goals:** stop on unproven ownership; do not invoke `cl`/`lib`/`link` or `/c`/`/Fo`/`/Zs`, build/configure/VS project, source/config/exception/device/adapter/OpenNT change, or source copy. |
| S6 object/link-closure audit, 2026-08-11 | `etc/research/t95-s6-msvc-object-link-closure-audit.md` freezes the source-level ledger for syntax-003's fixture plus 15 candidate units. It identifies the composition-owned globals and required SIM/log/memory/empty-port/CPU definitions, rejects `main.cc`, config/GUI/plugin/firmware/full-device/adapter/OpenNT aggregates, and records unresolved retained-object edges. The decisive blocker is `init.cc`'s unconditional `init_FetchDecodeTables` call and `fetchdecode.cc`/`ia_opcodes.h` function-pointer handler closure; additional CMOS/VGA/device/UI/timer references require retention proof. No smallest linkable object set or buildability claim exists. **Next gate:** coordinator review before a controlled `/Gy`/link-retention proof admission; no recipe is admitted. |
| S6 CPU-core compile-object ledger admission, 2026-08-11 | Coordinator rejects immediate `/Gy`/link proof because the CPU handler closure is not stable and project rules require it before a build target. This read-only audit begins at `cpu/fetchdecode.cc`/`cpu/ia_opcodes.h`, using imported CPU build metadata as evidence only, and must derive every native CPU source/object family that provides execute targets or initialization. It classifies each family mandatory, default-but-not-required, feature/config-rejected, or unresolved; reconciles generated/static opcode maps with translation units; and identifies post-core non-CPU edges. **Stop/non-goals:** stop at missing generation provenance; no `cl`/`lib`/`link`, `/Gy`/`/c`/`/Fo`/`/Zs`, config/source/exception change, project/build, device enablement, hand stub, adapterization, instruction-semantic omission, or adapter/OpenNT expansion. **Exit:** auditable ledger only, never a buildability claim. |
| S6 CPU-core compile-object ledger, 2026-08-11 | `etc/research/t95-s6-cpu-core-compile-object-ledger.md` records that checked-in `ia_opcodes.h` is the static handler map, not a generated input, and reconciles it with the imported 32-bit `cpu/Makefile` `OBJS` family. It classifies original scalar/control/state and FPU-off/MMX/SSE compatibility units as mandatory native CPU-family accounting; CPU loop/event as default-not-required; CPU64, CPUID/MSR/APIC, VMX/SVM, and 3DNow families as configuration-rejected; and per-object survival as unresolved. `FPU_ESC` proves FPU-off does not authorize omitting `fpu_emu`. Non-CPU CMOS, port/DMA/timer, memory, logging/SIM edges remain unresolved at object retention. **Next gate:** coordinator review of whether a non-semantic CPU-family object-model closure can be admitted; no build target or `/Gy`/link proof is admitted. |
| S6 CPU-profile authority reconciliation admission, 2026-08-11 | Coordinator admits a read-only authority/provenance reconciliation because the accepted v2 analysis projection is CPU3 whereas the historical `Configure-Bochs26NativeContainer` invocation explicitly used `-CpuLevel 5`. Trace both through current governance/design/queue/evidence, projection manifests, configure scripts, and source configuration; determine their authority/layer, whether either selects the first profile, and whether guest/fixture evidence imposes a level. Classify only resolved, ambiguous, or owner-decision-required. **Stop/non-goals:** do not choose/change CPU level, config/manifest/source/exception/recipe/CPU ledger; do not invoke compiler, linker, configure, project, or build; stop if resolution requires guessed guest runtime behavior. |
| S6 CPU-profile authority reconciliation, 2026-08-11 | `etc/research/t95-s6-cpu-profile-authority-reconciliation.md` resolves CPU3 and CPU5 as different accepted evidence layers: v2 CPU3 is MSVC syntax-analysis-only and the custom mechanics baseline, while CPU5 is an explicitly selected historical stock-ROM/floppy profile because that firmware reaches `cpuid(1)`. The original configuration source confirms these values have materially different APIC/FPU/MSR constraints. Neither record selects the first OpenNT runtime profile; no guest execution evidence imposes a level. **Next gate:** owner decision after a reached first-profile caller states whether stock firmware is in scope. Until then the unified CPU profile is owner-decision-required; no object-model/build/configuration action is admitted. |
| S6 non-CPU retention-edge ledger admission, 2026-08-11 | CPU-profile selection remains pending and is not in scope. Coordinator admits a read-only ledger from the fixed minimal composition/source-level object audit for SIM/parameter/UI/debug, logio, PC system, no-SIM memory, empty port space, triple-fault CMOS, and timer/DMA/VGA edges. It must name members and imported owners, classify direct lifecycle reachability, default-not-required/rejected paths, object-retention uncertainty, and future original-Bochs-core versus forbidden adapter ownership. **Stop/non-goals:** do not infer call paths; do not select CPU profile or change source/config/exception; no compile/link (`/Gy`/`/Zs` included), build/project/configure, device enablement, or adapter/OpenNT behavior. |
| S6 non-CPU retention-edge ledger, 2026-08-11 | `etc/research/t95-s6-non-cpu-retention-edge-ledger.md` identifies direct required original-Bochs owners as minimal SIM/parameter, logging, PC-system construction, no-SIM memory, and empty port space. SIM UI/debug callbacks, log ask/VGA refresh, PC/timer methods, memory debug VGA accesses, full device/plugin initialization, triple-fault CMOS hard reset, and CPU-loop DMA are default-not-required or explicitly rejected; their common-object retention remains unproven. No edge may move to the external adapter. **Next gate:** coordinator review of a per-owner retention-proof plan; CPU profile stays pending and no build target is admitted. |
| S6 retention-proof protocol design admission, 2026-08-11 | CPU-profile owner decision remains pending. Coordinator admits documentation-only design from the accepted CPU/non-CPU ledgers and current rules: name prerequisites, candidate object-family inputs (not a recipe), symbol/map/undefined artifacts, pass/fail rules for dead-stripped denied members versus unresolved required references, stop conditions, and non-runtime limits. It must show CPU3 and CPU5 as mutually exclusive profile substitution points without selecting either. **Stop/non-goals:** no compiler/linker/library/VS/configure command; no source/config/exception change; no device, adapter, or OpenNT semantic change. |
| S6 retention-proof protocol design, 2026-08-11 | `etc/research/t95-s6-retention-proof-protocol.md` freezes the minimal future empirical protocol: a pre-registered single-profile effective configuration; ledger family manifest; `/Gy` object symbols/COMDAT inventories; `/OPT:REF`/`/INCREMENTAL:NO` map, unresolved and import evidence; and explicit pass/fail predicates for required versus denied owners. CPU3 and CPU5 are mutually exclusive substitution points, with CPU5 requiring a new full configuration/ledger review. A passing link is explicitly not executable or runtime evidence. **Next gate:** owner chooses one profile after naming the reached caller; only then may coordinator propose a controlled closure investigation. |
| S6 CPU3 retention-proof admission, 2026-08-11 | Coordinator applies the owner’s earlier minimal/bare-machine/no-stock-firmware intent: CPU3 is selected only for the first bare-machine mechanics profile; CPU5 stock-ROM/floppy remains deferred and does not alter the eventual OpenNT objective. One fresh analysis root may use accepted v2 projected config `C47030F79880C894A346F56E3AB3242924308983E00B5845EDCE849450C6C19D`, copy pinned Bochs input only there, and add one non-product fixture that roots minimal composition lifecycle without calling it. It may compile the protocol-declared CPU3 and non-CPU core source families with `/Gy`, then make one `/OPT:REF`/`/INCREMENTAL:NO` link/map/undefined/import observation. **Stop/non-goals:** no source/config/exception change, `bochs.exe`/`all`/VS project/configure, execution/reset, firmware/device/plugin enablement, adapter/DOS/OpenNT semantics, retry/variant beyond the declared observation; stop at first closure boundary and retain exact evidence. |
| S6 CPU3 retention proof 001, 2026-08-11 | `etc/research/t95-s6-cpu3-retention-proof-001.md` records the selected bare-machine CPU3 empirical attempt. The copied root config hashes to accepted v2 `C47030F79880C894A346F56E3AB3242924308983E00B5845EDCE849450C6C19D`; direct BuildTools x86 `/Gy` compilation produced 72 exact CPU3/minimal-core objects with hash evidence and only known `FMT_LL` C4005 warnings. The one actual `link.exe /OPT:REF /INCREMENTAL:NO` call exited `LNK1146` before object processing because command construction split `/OUT:` and `/MAP:` from their values. No map/unresolved/import/COMDAT/PE result exists, so no closure classification is legal. No executable was created or run; no Bochs/adapter/OpenNT/config/exception source changed. **Next gate:** coordinator review of this one-link command-shape failure; any corrected retention observation requires fresh admission and a prevalidated argument vector. |
| S6 link parameter-vector repair admission, 2026-08-11 | Coordinator accepts the 72-object `/Gy` evidence and rejects `LNK1146` as closure evidence. This bounded package may inspect the failed argument display and add only a deterministic, testable repository tool for a future exact link argument vector or response-file content. It must fail closed unless `/OUT:<one nonempty absolute path>`, `/MAP:<one nonempty absolute path>`, `/OPT:REF`, and `/INCREMENTAL:NO` are intact single tokens; every object path must be nonempty and existing; duplicate/malformed/split values and product/run options must be rejected. **Stop/non-goals:** no `cl`/`link`/project/configure/run; no Bochs/config/adapter/OpenNT/exception change or new analysis root. Record hash/fixture inputs and amend the retention proof record; then governance/diff only. A new link needs independent acceptance and fresh admission. |
| S6 link parameter-vector repair, 2026-08-11 | `tools/build/New-T95S6LinkArgumentVector.ps1` now deterministically creates and validates the future exact vector, retaining `/OUT:<absolute path>` and `/MAP:<absolute path>` as single nonempty tokens with `/OPT:REF` and `/INCREMENTAL:NO`; it rejects split/empty/duplicate/unexpected options and missing/duplicate object paths. `Test-T95S6LinkArgumentVector.ps1` passes a synthetic positive plus malformed split, empty, duplicate, missing-object, duplicate-object, and `/DLL` negative suite without calling `cl.exe` or `link.exe`. `etc/research/t95-s6-cpu3-retention-proof-001.md` records tool/test and retained fixture/object-manifest/failed-display hashes. No vector was generated for the 72-object root and no link retry occurred. **Next gate:** independent acceptance, then a fresh separately admitted single-link observation using this validator. |
| S6 CPU3 link-retention observation 002 admission, 2026-08-11 | Coordinator independently accepts the parameter-vector repair and admits exactly one fresh link-only CPU3 observation. Revalidate and reuse only the frozen 72 objects and `BFF16CBFDF2F3104B4E0732A94FB787FF1504204D5718B6BA4B5A3D91485760F` manifest; no compilation/rebuild. A new analysis root may retain a hash-pinned input manifest plus generated vector, output/map paths, command/logs, and any map/undefined/import evidence. The vector must be produced and validated only by `New-T95S6LinkArgumentVector.ps1`, then passed directly to confirmed x86 `link.exe` in the loaded BuildTools environment. **Stop/non-goals:** one invocation only; no command concatenation, manual options, response file, retry, added object, run, `bochs.exe`/`all`/VS project/configure, or Bochs/adapter/OpenNT/config/device/firmware/plugin change. |
| S6 CPU3 link-retention observation 002 pre-link stop, 2026-08-11 | The fresh root revalidated frozen manifest/object hashes and retained input manifest `2AB84B556A542D61345FE59514EB1439C54E16CED58EB3447A087CE3CFE7DF6F`, but the shell dot-source step was blocked by execution policy before it could load the admitted vector generator. Its retained validation record has null vector/hash/tokens and false required flags (`13A8AEFBFEDBE24AAFCB8B67E166F43869FBFACBF3E36150ECEFC4090C49A88D`). No parameter vector, `link.exe` call, map, executable, undefined/import evidence, compile, or run occurred. Per first-error rule, the root is frozen and no execution-policy workaround/retry was attempted. **Next gate:** coordinator review; any future link observation must be newly admitted and establish approved PowerShell policy before a fresh root is created. |
| S6 PowerShell link-vector entrypoint proof admission, 2026-08-11 | Coordinator accepts the pre-link policy stop as no-link evidence. This bounded package may add one explicit entrypoint intended only for `powershell.exe -ExecutionPolicy Bypass -File <entrypoint>` that dot-sources the accepted vector generator and emits/validates a vector for synthetic existing objects. A focused test may launch exactly that entrypoint with the explicit policy and prove vector output plus fail-closed missing-generator behavior. **Stop/non-goals:** no fresh link root, frozen object-input change, alternative toolchain path, `cl`/`link`/run/configure/project, or Bochs/config/adapter/OpenNT/exception change. Document policy-safe loading mechanics and no-link result; then governance/diff only. |
| S6 PowerShell link-vector entrypoint proof, 2026-08-11 | `Invoke-T95S6LinkVectorEntrypoint.ps1` (`5C993833318843641A1B955D5C2E5E92589F9EDAA704B7F3AA38FB970A90E06B`) is the sole documented `powershell.exe -ExecutionPolicy Bypass -File` entrypoint: inside that process it loads only the accepted generator, confirms its two functions, and emits a validated JSON vector from absolute existing-object inputs. Its focused test (`527870EA69FA57EE8649878A33F4000435C5AB9E58341F2EC2314662234A9802`) passes explicit-policy positive emission and missing-generator fail-closed cases. Dot-sourcing is policy-safe only through this explicit Bypass entrypoint; no CPU3 vector, link, compile, run, or source/object-input change occurred. **Next gate:** a fresh link observation remains separately coordinator-gated. |
| S6 CPU3 link-retention observation 003 admission, 2026-08-11 | Coordinator accepts the explicit-policy entrypoint and admits exactly one fresh CPU3 link-only observation. Revalidate only the frozen 72-object manifest/hashes, create one fresh analysis root, and invoke only `powershell.exe -ExecutionPolicy Bypass -File tools\probe\Invoke-T95S6LinkVectorEntrypoint.ps1` to emit/validate a JSON vector there for fresh absolute output/map paths. Parse the persisted JSON without modifying/reordering tokens, then directly invoke confirmed x86 `link.exe` once with those exact tokens. Retain entrypoint/vector/object/command hashes and any map/verbose/undefined/import/PE evidence. **Stop/non-goals:** no compile/rebuild/retry/alternate shell/response file, source/config/adapter/OpenNT change, `bochs.exe`/project/configure/device/firmware/plugin enablement, or execution. |
| S6 CPU3 link-retention observation 003, 2026-08-11 | The fresh root revalidated all frozen 72 object hashes and used only the explicit-policy entrypoint to write the 79-token vector (`BBEA0874E957568DF2F406A033D2CACB0532A540759584524D3EC44B7D13C928`), then directly passed the persisted original-order tokens once to BuildTools x86 `link.exe` 14.43.34809.0. The sole call reached library search but ended `LNK1120: 72 unresolved externals`; retained `link.log` records 49 `LNK2019` and 34 `LNK2001` reference lines covering required CPU providers and rejected plugin/device/timer/GUI/SIM configuration owners. The map exists but is zero bytes; no PE/import table exists. This is negative object/link closure evidence only: no compile/rebuild/retry/source/config/adapter/OpenNT change or execution occurred. **Next gate:** coordinator review of required-provider versus rejected-retention edges before any newly admitted action. |
| S6 unresolved-symbol ownership classification admission, 2026-08-11 | Coordinator accepts observation 003 as valid negative CPU3 link-closure evidence and admits a read-only classification. From frozen link log/invocation/vector and original source only, enumerate all 72 unique unresolved external identities, their referencing object(s), and original provider source/object family. Give each exactly one status: mandatory native CPU provider, mandatory bare-machine core provider, default-but-not-required retained member, explicitly rejected product owner, or unresolved/ambiguous. Reconcile VMX/SVM/MSR/GETSEC/ADCX-ADOX with fixed CPU3 projection; they may show absent guards or code retention, never feature admission. Produce human and machine-readable records with reproducible searches. **Stop/non-goals:** no compiler/linker, object/source/config/exception/adapter/OpenNT change, added provider, feature/device/firmware/plugin enablement, or execution; stop at any provider that source cannot prove. |
| S6 unresolved-symbol ownership classification, 2026-08-11 | `etc/research/t95-s6-unresolved-symbol-ownership-ledger.md` and frozen machine ledger `16D5C6506FBA2E9289DE8F9D3996C846E09761A04FA5464A72621067E749C5E8` enumerate all 72 unique link identities with referencing objects, provider family/source and exactly one class: 5 mandatory native CPU, 0 mandatory bare-machine core, 31 default-but-not-required retained, 35 explicitly rejected product owners, and 1 unresolved/ambiguous. `GETSEC` is the source-provider stop; VMX/SVM/MSR/ADX appearances are retained static-opcode-map paths under CPU3, not feature admission. No build/toolchain or source/config/adapter/OpenNT/device action occurred. **Next gate:** coordinator review of the unproven GETSEC provider, CPU event/cpu providers and rejected retention before any new admission. |
| S6 rejected-owner retention-root trace admission, 2026-08-11 | Coordinator accepts the ownership ledger and admits a read-only trace for only its 35 explicitly rejected owners. Starting at frozen LNK references and original source definitions, derive each immediate referencing member/object and path toward the admitted fixture/composition root; group distinct root paths as genuine minimal-core demand, member that should dead-strip but does not, or fixture/composition rooting artifact. Name exact source/line and whether a bounded Bochs-local seam could theoretically avoid the root without adapter semantics, or it is a hard incompatibility. **Stop/non-goals:** GETSEC and the 31 default CPU handlers are out of scope except misattribution; no seam/exception choice, compile/link/source/config/adapter/OpenNT change, device/firmware/plugin enablement, or execution. |
| S6 rejected-owner retention-root trace, 2026-08-11 | `etc/research/t95-s6-rejected-owner-retention-root-trace.md` and machine ledger `F0D8E732541FE72863081E1DCD8CDA3E579A369A4BEE4D56ECC66AFAB3DCE336` trace all 35 rejected identities to four static roots: minimal SIM virtual surface (composition artifact), common `devices.obj` full-PC members (should dead-strip but do not), PC-system global lifecycle (composition artifact), and log fatal/`bx_atexit` (should dead-strip; hard incompatibility for current object model). No root proves a genuine minimal-core demand for a product owner. GETSEC/default CPU handlers remain out of scope. No seam, build, or source/config/adapter/OpenNT/device action occurred. **Next gate:** coordinator review before any new bounded Bochs-local retention action. |
| S6 minimal internal retention-isolation design admission, 2026-08-11 | Coordinator accepts the four-root trace and admits design-only examination of original Bochs-internal retention boundaries: full `bx_real_sim_c` vtable from minimal SIM init; `devices.obj` full-PC members; composition-held `bx_pc_system` GUI cleanup; and `logfunctions::fatal -> bx_atexit`. It may enumerate narrowest original-class/method/source-object/helper candidates, expected undefined-symbol effect, intrusive scope/exception need, risk and static verification, but must not choose or implement one. **Stop/non-goals:** no adapter ownership, stub/product-semantic replacement, full product import, build/link/source/config/exception change or execution. |
| S6 minimal internal retention-isolation design, 2026-08-11 | `etc/research/t95-s6-minimal-internal-retention-isolation-design.md` freezes a four-root decision matrix. It identifies only Bochs-local class/source-object/lifecycle split candidates, their source boundaries, expected rejected-undefined effect, exception need, risk and static proof. No existing alternate minimal SIM/PC-system class is evidenced; the empty-port helper is not a retention split. SIM/devices/PC ideas need future exception records; logging remains rejected absent a proven accessor-only original boundary. No candidate, source/config/exception, build/link, adapter/OpenNT or runtime change is selected. **Next gate:** coordinator chooses at most one separately registered Bochs-local boundary, or retains the negative closure. |
| S6 empty-port source-object separation admission, 2026-08-11 | Coordinator selects only the lowest-risk retained-owner candidate: one Bochs-internal source-object split for the already registered empty-port initializer. Before the source patch, `BX-IO-029` must register the exact move of `bx_devices_c::init_empty_port_space` and the original default port read/write handler definitions from `iodev/devices.cc` to one new `iodev` translation unit. **Scope:** byte-for-byte moved definitions only; `cleanup_empty_port_space`, headers, ABI, callers, defaults, lifecycle, configuration, features, adapter/OpenNT code and all other device behavior remain unchanged. **Verification:** a focused static test must prove exactly one definition of each moved member in the new object, zero in `devices.cc`, and zero adapter references; retained FMT/#UD/governance/diff checks must pass. **Stop/non-goals:** stop if relocation needs a declaration/API/semantic change; no compile/link/run, project/configure, device/firmware/plugin enablement, or adapter/DOS/OpenNT semantic change. **Exit:** source-boundary evidence only; no object/link-closure claim. |
| S6 empty-port source-object separation, 2026-08-11 | `BX-IO-029` is registered before the patch and implemented only through `refs/bochs/iodev/minimal_port_space.cc`, which now owns the exact existing definitions of `init_empty_port_space`, `default_read_handler`, and `default_write_handler`; `devices.cc` retains `cleanup_empty_port_space` and all other members. `etc/research/t95-s6-empty-port-source-object-separation.md` records the exact boundary and non-claims. The focused static test, retained FMT/#UD tests, documentation governance and safe diff checks pass. No header/ABI/caller/default/lifecycle/configuration/feature/adapter/OpenNT change or compile/link/run occurred. **Next gate:** coordinator review before a separately admitted object-model/retention observation; this source move alone does not demonstrate dead stripping or link closure. |
| S6 minimal-port compile-only observation admission, 2026-08-11 | Coordinator admits one fresh CPU3 projected-config compile-only boundary observation for `refs/bochs/iodev/minimal_port_space.cc`. **Scope:** copy the pinned Bochs tree/config only under a fresh analysis root; pin source/projected config hashes; invoke exactly one confirmed BuildTools x86 `cl /nologo /c /Gy /MT` on that one source with `/DWIN32 /DBX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=0` and its required original header context. **Stop/non-goals:** no `devices.cc` rebuild, batch/variant/retry, link/executable/run/configure/project, source/config/exception/adapter/OpenNT change, or device/firmware/plugin enablement. **Evidence:** retain command/log and, only if created, object hash plus undefined-symbol dump. **Exit:** compilation/object-boundary evidence only; it cannot establish link, dead-strip, reset, runtime, or product closure. |
| S6 minimal-port compile-only observation 001, 2026-08-11 | `etc/research/t95-s6-minimal-port-compile-observation-001.md` records the one permitted BuildTools x86 `/nologo /c /Gy /MT` compilation of copied `iodev/minimal_port_space.cc` under the accepted CPU3 projected config (`C47030F79880C894A346F56E3AB3242924308983E00B5845EDCE849450C6C19D`). It exits 0, emits only the known `FMT_LL` C4005 warning, and produces object `0E397EF5B0029296142B3B6AB372A3B770DAEB2B7BC6774AD8F0D078C90858A8`. Permitted `dumpbin /symbols` lists five object-local undefined references: original default-handler registration members plus normal CRT allocation/security entries. No `devices.cc` rebuild, batch, retry, link, executable/run, source/config/adapter/OpenNT or feature change occurred. **Next gate:** coordinator review before a separately admitted object-model observation; this does not prove a build recipe, COMDAT removal, link closure or runtime behavior. |
| S6 default-handler registration separation admission, 2026-08-11 | The coordinator accepts the independently reproduced five-import object observation. Three imports are normal MSVC CRT allocation/security support; the sole two non-CRT Bochs imports are the original default-handler registration members at `iodev/devices.cc:717-743`. `BX-IO-030` is registered before any patch and admits only their byte-for-byte move into the existing `iodev/minimal_port_space.cc`, extending the focused static source-boundary test. **Non-goals:** no header/ABI/caller/lifecycle/cleanup/default behavior change; no compile/link/run/build recipe/configure/project; no device, firmware, plugin, adapter, OpenNT or DOS behavior. **Exit:** static source-boundary evidence only; a fresh compile/object observation requires separate admission. |
| S6 five-member minimal-port compile-only observation admission, 2026-08-11 | The coordinator admits exactly one fresh CPU3/MSVC x86 compile-only observation of the post-`BX-IO-030` `iodev/minimal_port_space.cc`. **Scope:** copy the pinned tree beneath a new `artifacts/analysis/` root; replace only its copied `config.h` with the accepted CPU3 projection; pin input hashes; invoke exactly one BuildTools x86 `cl /nologo /c /Gy /MT` with `/DWIN32 /DBX_NTVDM_ENABLE_EXCEPTION_INTERCEPT=0`; if and only if the object exists, capture one `dumpbin /symbols` result. **Expected discriminator:** no non-CRT Bochs import may remain. **Stop/non-goals:** no `devices.cc` or other object compilation, no batch/variant/retry, link/executable/run, configuration/project/configure, source/adapter/OpenNT change, or feature/device/firmware/plugin enablement. **Exit:** object-import evidence only; it cannot prove a link recipe, COMDAT removal, reset, runtime or product closure. |
| S6 five-member minimal-port compile-only observation 002, 2026-08-11 | `etc/research/t95-s6-minimal-port-compile-observation-002.md` records the single permitted fresh BuildTools x86 compile under the same accepted CPU3 projected configuration. It exits `0` with only the known `FMT_LL` warning. Its `dumpbin /symbols` result places both default-handler registration members in local sections and lists only CRT allocation/string/security imports; no Bochs/device/plugin/SIM/GUI/adapter/OpenNT undefined identity remains. This proves the `BX-IO-030` object-import prediction only. No `devices.cc` or other source compiled, and no link/executable/run/configure/project/source/adapter/OpenNT or feature action occurred. **Next gate:** review remaining independent retained roots and CPU static-map closure before any new object model or link admission. |
| S6 CPU3 static-opcode closure decision, 2026-08-11 | `etc/research/t95-s6-cpu3-static-opcode-closure-decision.md` source-proves why CPU3 still retains VMX/SVM/MSR/SMX/ADX handler identities: original `ia_opcodes.h` stores member-function addresses in static decoder metadata and `fetchdecode.cc` retains representative entries outside CPU-level conditional regions. `GETSEC` is source-located as `BX_CPU_C::GETSEC`; no provider object is guessed. The CPU therefore remains an original native Bochs subsystem: a future recipe must derive its complete handler/object closure from original metadata, not delete decoder entries, stub handlers or treat CPU3 as an object-selection license. No source/config/build/link/runtime or feature action occurred. **Next gate:** derive original CPU project/object closure and preserve separate negative SIM/pc-system/logging root conclusions before any new link admission. |
| S6 original CPU-project closure, 2026-08-11 | `etc/research/t95-s6-original-cpu-project-closure.md` read-only parses the pinned `vs2008/cpu.vcproj`: its Debug/Release Win32 static-library project declares 93 CPU `.cc` sources, including 20 named 64-bit/extension-family paths. That original project closure, not the earlier hand-selected partial handler list, is the safe starting upper bound for the Bochs CPU toolchain island; CPU3 remains an execution profile rather than an object-selection permission. The same record preserves the negative decisions for the SIM virtual surface, `pc_system` lifecycle and logging fatal path: no source-backed narrow alternative is proven, so no new intrusion is admitted. **Next gate:** separately admit a CPU-project object inventory under the accepted projection before any retained-owner/link analysis. |
| S6 original CPU-project object inventory admission, 2026-08-11 | The coordinator admits exactly one fresh MSVC x86/CPU3 object-only inventory of the 93 `.cc` paths parsed from pinned `vs2008/cpu.vcproj`. **Scope:** create one fresh analysis root; copy pinned `refs/bochs`; replace only the copied root `config.h` with the accepted CPU3 projection; derive the 93 paths directly from copied `cpu.vcproj`; invoke BuildTools x86 `cl /nologo /c /Gy /MT` exactly once per listed translation unit, in original project order, into that root; capture a manifest, hashes, per-source result and aggregate log. **Stop/non-goals:** no hand-selected file addition/removal, retry, alternate flags/configuration, source/config/adapter/OpenNT change, `devices.cc`, machine/product source, static library/link/executable/run, VS solution/project, configure, firmware/device/plugin feature enablement. A first compile failure is evidence and stops subsequent compilation. **Exit:** a CPU source-to-object availability classification only; it neither selects a final recipe nor proves machine link/reset/runtime closure. |
| S6 original CPU-project object inventory interruption and continuation admission, 2026-08-11 | The original foreground observation was forcibly stopped by the execution host at roughly 64 seconds after exactly 13 successful original CPU objects (`3dnow.cc` through `bit16.cc`); no compiler error and no `cl.exe`/`cmd.exe` process remained, while the next original source had not started. This is external time-bound interruption evidence, not a failed compilation or a source conclusion. The coordinator admits one hidden continuation process in the same immutable analysis root, after validating the existing 13 object names against original project order and their input manifest. It may compile each of only the remaining 80 unattempted original paths once with the same exact flags/configuration, append logs/results, and stop at the first compiler failure. **Non-goals:** no recompilation of existing objects, retry of a failed source, source/config change, link/executable/run/project/configure or feature/device action. The continuation is still one object-only inventory, not a build recipe claim. |
| S6 original CPU-project object inventory 001, 2026-08-11 | `etc/research/t95-s6-cpu3-original-project-object-inventory-001.md` records the accepted CPU3/MSVC x86 object result: all 93 paths from original `cpu.vcproj` produced one object; the initial 13 completed before an external host-time interruption and the validated continuation compiled exactly the remaining 80, all exit zero. No compiler error diagnostic appears; only the known `FMT_LL` warning repeats. VMX/SVM/AVX/XOP/XSAVE and 64-bit source families compile as retained native CPU implementation, not enabled guest features. There is no static library/link/executable/run, device/machine/SIM/adapter/OpenNT source, configuration/project/configure or feature action. **Next gate:** a read-only symbol/retention inventory across this original CPU object island and prior isolated mechanics objects before considering any new link observation. |
| S6 CPU-object symbol/retention inventory admission, 2026-08-11 | The coordinator admits a read-only symbol inventory of the frozen 93 CPU objects from `t95-s6-msvc-x86-cpu3-project-objects-001-20260811-001`, plus the accepted isolated minimal-port object observation as a comparison input. It may run `dumpbin /symbols`, record defined/undefined identities and object references, and correlate them only with original Bochs source definitions and the existing SIM/pc-system/logging retention records. **Stop/non-goals:** no compilation/recompilation, source/config/adapter/OpenNT change, provider addition, library/link/executable/run, project/configure or feature/device action. **Exit:** exact native-CPU versus external-mechanics symbol classes and a decision whether a link observation can be bounded; it cannot itself authorize that link. |
| S6 CPU-object symbol/retention inventory 001, 2026-08-11 | `etc/research/t95-s6-cpu3-symbol-retention-inventory-001.md` records one `dumpbin /symbols` pass over the frozen 93-object CPU island. Its 1,253 internal external definitions eliminate every CPU handler/decode-family reference; 51 identities remain, uniquely classified as CRT (21), original logging (7), SIM/parameter tree (10), minimal-machine globals (4), original memory (3), PC-system (2), and original I/O (2). There is no unresolved CPU feature handler, device plugin, GUI/configuration, adapter or OpenNT identity. CPU is source-built and internally symbol-closed, but the machine is not link-closed: fresh selected SIM/logging/memory/PC/port/machine object evidence is required before any bounded link observation. `devices.cc`, `main.cc`, GUI, configuration, plugins, adapter and OpenNT remain rejected. |
| S6 minimal-mechanics provider object observation admission, 2026-08-11 | The coordinator admits one fresh CPU3/MSVC x86 compile-only observation of exactly ten original/registered Bochs mechanism providers: `bx_ntvdm_minimal_machine.cc`, `bx_ntvdm_minimal_sim.cc`, `memory/memory.cc`, `memory/misc_mem.cc`, `iodev/minimal_port_space.cc`, `iodev/devices.cc`, `pc_system.cc`, `logio.cc`, `gui/siminterface.cc` and `gui/paramtree.cc`, plus no other source. **Purpose:** classify their direct object symbol surfaces against the CPU island; `devices.cc` and `siminterface.cc` are evidence inputs only, not product admissions. **Scope:** one fresh copied tree/projection root, one BuildTools x86 `/nologo /c /Gy /MT` invocation per listed file, input/object hashes, aggregate logs and `dumpbin` only if all objects exist. **Stop/non-goals:** first compiler failure stops; no source/configuration/adapter/OpenNT change, no added provider, static library/link/executable/run, VS project/solution/configure, or feature/device/firmware/plugin enablement. `main.cc` is explicitly excluded; no final object set or link claim follows. |
| S6 minimal-mechanics provider object observation 001 | `etc/research/t95-s6-minimal-mechanics-provider-object-observation-001.md` records that all ten admitted mechanism objects compile under CPU3/MSVC x86 and provide all 28 CPU-island non-CRT requirements. The combined 103-object symbol ledger nevertheless retains 139 unresolved identities. Exact object roots prove `devices.cc` retains device/plugin/timer/GUI ownership, `siminterface.cc` retains real-SIM/configuration/NIC-USB/plugin/GUI lifecycle, `pc_system.cc` retains GUI cleanup, and `logio.cc` retains `bx_atexit`; `main.cc` remains excluded. **Decision:** no bounded link is admissible, and no further one-off helper move is authorized. Any continuation must first design a cohesive Bochs-internal product-shell extraction across those lifecycle roots, without adapter/OpenNT substitution or CPU rewrite. |
| S6 native-container reconciliation, 2026-08-11 | `etc/research/t95-s6-native-container-reconciliation.md` reconciles the 103-object rejection with the existing native-container authority. It rejects a cohesive product-shell extraction as a T95 implementation route because that would still recreate Bochs lifecycle ownership. The minimal boundary is restored as an intact original Bochs 2.6 `nogui` lifecycle with a minimum declarative *enabled profile*, not a minimum source-file set. Existing CPU/object/separation work remains evidence only and no more extraction patch/link is admitted. **Restored next gate:** design one MSVC/x86 native-container recipe from original metadata, declarative optional-plugin denial and native reset/controlled-stop evidence before reconnecting the default-off generic adapter seam. |
| S6 MSVC native-container recipe design | `etc/research/t95-s6-msvc-native-container-recipe-design.md` records the original VS2008 shape: the Win32 `bochs` executable owns eight lifecycle sources and depends on sixteen original projects, including GUI/debugger and optional-device project families. This proves an intact native container must preserve the original build graph while minimum behavior is enforced through a copied CPU3/nogui declarative profile, not hand-selected objects. The first target is a fresh-root MSVC/x86 observation with adapter/CLI excluded and #UD inactive; no VS solution/full build/run is admitted. **Next gate:** source-to-build feature manifest classifying every original dependency as required lifecycle, compiled-but-runtime-disabled, or unavailable, with exact profile/plugin-control inputs and negative predicates. |
| S6 MSVC native-container feature manifest | `etc/research/t95-s6-msvc-native-container-feature-manifest.md` separates historical solution order from actual link membership and CPU3 runtime activation. It classifies original projects into lifecycle owners, compiled-but-runtime-disabled support and utility executables unavailable to the container; CPU3 explicitly denies debugger, instrumentation, FPU, x64, PCI, SMP, network, sound and USB. The only permitted first-profile inputs are original `nogui`, CPU/RAM/approved ROM facts and the existing optional-plugin `plugin_ctrl` denial. **Remaining gate:** derive fresh ordered source-to-object/link membership from original metadata and retained evidence; do not infer it from `.sln` order or invoke a build/link yet. |
| S6 MSVC native-container original link template | `etc/research/t95-s6-msvc-native-container-link-template.md` records the original generated MSVC `Makefile:175-186` executable link ordering: lifecycle objects followed by iodev/optional archives, CPU/CPUDB, memory/gui, disasm/FPU and original platform/link variables. This closes the VS-solution-order ambiguity but remains a template, not a new response file or build action. GCC r4 archives are provenance only and cannot cross into the MSVC island. **Next gate:** generate a fresh-root manifest expanding every original template input with hashes, CRT/toolchain identity, CPU3/nogui/plugin profile and reset predicates before any MSVC build or link admission. |

## Active Packet: M0 T95 S7

| Field | Record |
| --- | --- |
| Identifier mode | `M0 T95 S7`, Ordinary Mode. It is the sole active numeric subtask. |
| Admission and owner approval | The owner approved a contained CLI-first NTVDM path, immutable BYOB identity admission, no ambient host exposure, and a holistic capability rather than per-BOP hacks. Queue order reserves S7 after S6. The source-derived COMMAND placement contract is now complete. |
| Objective | Materialize one coherent, read-only guest-file capability for the first contained profile: a separately declared placement for verified `COMMAND.COM`, plus explicit CONFIG.SYS/AUTOEXEC.BAT materialization and canonical DEM Open/Seek/Read/Close semantics. |
| Scope | Add a new, validated profile/schema revision for explicit canonical guest artifact placement; carry its immutable data through CLI-to-adapter installation; construct one adapter-owned bounded read-only guest namespace with opaque generation tokens; and source-shape the four DEM service outcomes plus paired CONFIG/AUTOEXEC pathname materialization. The future Bochs side may consume only an already-validated generic opaque bulk result. |
| Non-goals | No ambient PIF/SystemRoot/temp/registry behavior; no BYOB-root-as-DOS-root; no host mutation, pipes, devices, UNC/relative paths, arbitrary host reopen, general DOS filesystem, search-family dispatch, redirector, WOW, XMS/DPMI, or new Bochs CPU/device/FPU/PIC/VGA semantics. No direct COMMAND BOP loader or special-case path. |
| Reference baseline | `design/ADAPTER-BULK-RESULT.md`, `design/HOST-NAMESPACE-CAPABILITY.md`, `etc/research/t95-s6-dos-read-handle-closure-audit-001.md`, `t95-s6-command-guest-path-provenance-001.md`, `t95-s6-guest-artifact-placement-contract-001.md`, `t95-s6-cmd-getconfigsys-capability-audit-001.md`, and the BOP state matrix. |
| Files and ABI surface | A versioned BYOB profile/selection extension; adapter-local immutable placement and read-token state; source-derived typed CPU/bulk result records; focused CLI/adapter tests. Bochs retains generic RAM/CPU result consumption only and receives no path, handle, selector, or configuration term. |
| Verification | Profile negative tests cover malformed/conflicting/absent placement and no host-path escape. Adapter tests cover canonical Open/Seek/Read/Close, token generation/staleness, sequential offsets, 64 KiB read bounds, error mapping, config/autoexec pair policy, and cleanup. A later fresh-root runtime trace must reach normal guest file activity without an unimplemented BOP special case. |
| Expected markers | Profile admission rejects missing or invalid placement before guest execution; a read-only namespace test proves identity-backed `\\COMMAND.COM` reachability and denies mutation; service tests prove source-shaped success and bounded failure; Bochs boundary tests prove no DOS/file vocabulary crosses its seam. |
| Completed increment (2026-08-11) | `ntdos64-byob-profile-v2` now requires one explicit `guest_command_placement` plus the closed `guest_boot_files` pair (`\\CONFIG.SYS` / `minimal-comment-v1`, `\\AUTOEXEC.BAT` / `empty-v1`). `bx_ntvdm_readonly_namespace_v1` exposes exactly those three same-drive immutable resources through private generation tokens and bounded adapter-local Open/Seek/Read/Close mechanics. Evidence: `etc/research/t95-s7-contained-boot-file-materialization-001.md`; profile, namespace and adapter-runtime tests rebuilt and passed. No DEM BOP dispatch, guest-memory write, Bochs change, host path or host handle is admitted by this increment. |
| Completed increment (2026-08-11, O/S/R/C local closure) | `bx_ntvdm_dem_readonly_file_service` now source-shapes the adapter-local `50:12/00/16/02` subset from copied state only: bounded explicit `C:\...` path input, private token, signed seek methods, ZF-selected sequential/explicit read and zero-payload EOF. The new `bx-ntvdm-dem-readonly-file-service-test` passes its full local lifecycle. It is deliberately absent from the BOP dispatcher; this does **not** claim a guest RAM commit, production error-family closure or an enabled BOP. |
| Completed increment (2026-08-11, runtime installation) | The adapter runtime now installs a v2 profile's namespace by copying its canonical paths into session-owned storage, and v3's existing generic copied-read mechanism carries a separately classified `DEM_FILE_OPEN` consumer through `50:12` completion. The runtime regression verifies that path. A generic pending bulk-result take wrapper is available for later read completion. Bochs is unchanged; at this point the read selector `50:16` remained unregistered and no real BOP execution was claimed. |
| Completed increment (2026-08-11, no-RAM O/S/C registration) | Adapter runtime v2 now registers the already tested `50:00` seek and `50:02` close subset over the installed private namespace; the runtime regression proves open → seek-from-end → close. `50:16` stays unregistered until the generic bulk-result consumer is demonstrably available in the Bochs mechanics path. No claim is made that Bochs has executed these selectors. |
| Completed increment (2026-08-11, generic bulk consumer seam) | Registered Bochs intrusion `BX-ABI-041` adds only the opaque one-range bulk-result consumer: same-boundary result match, complete ordinary-RAM preflight, one nonempty copy, then existing generic resume. Existing default-off and ordinary-RAM static boundary tests pass. The seam contains no selector/service/path/token/DOS/OpenNT branch. `50:16` remains unregistered pending adapter error and queue/take closure; no Bochs build or guest trace is claimed. |
| Completed increment (2026-08-11, DEMREAD runtime registration) | Adapter runtime v2 now registers `50:16`: a successful bounded read queues one opaque bulk result, while EOF queues the zero-payload form and invalid/stale input returns an ordinary direct CF/AX result without a transaction. Runtime and focused service fixtures cover queue/take payload and stale-token error. Bochs has not been built or traced consuming the result, so no guest execution claim follows. |
| Completed increment (2026-08-11, source-decoded COMMAND boot path) | OpenNT source trace `etc/research/t95-s7-ntio-command-boot-file-bop-trace-001.md` proves reached `54:0C` is `SVC_GETCONFIGSYS`, not a Bochs CPU/device prerequisite: original COMMAND writes a temporary-file pathname to `DS:DX`, after which normal DOS open/read follows. The contained replacement registers only `54:0C/0D`, returning canonical profile-owned `C:\CONFIG.SYS` / `C:\AUTOEXEC.BAT` through one existing generic RAM multi-write. The runtime fixture verifies both pathname transactions; no host temporary file/path, new Bochs intrusion, or guest trace claim is made. |
| Completed increment (2026-08-11, fresh runtime-trace closure audit) | `etc/research/t95-s7-bochs-runtime-trace-closure-audit-001.md` proves historical r30 cannot consume S7 services because it explicitly excludes runtime/session/namespace/guest-read/bulk closure. It freezes the current CMake-derived adapter/CLI source closure and the exclusive macro mode for a new observation: generic default-off exception bridge plus BOP listener on; historical startup-transaction and CPU-result bridge off. It additionally pins the retained source-built NTIO/NTDOS/COMMAND identities and the existing `ntio-v0` startup plan (`0070:0000`, payload `0x700`) as v2-profile provenance. No root was generated, built, linked or run. **Next gate:** admit and create one fresh refusal-first root generator; it must copy/hash only this closure and reject absent or mismatched source-built plan inputs. |
| Completed increment (2026-08-11, unified runtime startup endpoint) | Registered `BX-ABI-042` replaces the obsolete narrow startup-transaction preparation call inside Bochs's existing default-off generic execution-plan consumer with `bx_ntvdm_adapter_runtime_v1_prepare_execution_plan_from_environment`. The opaque plan ABI, RAM preflight/copy sequence and real-mode entry remain unchanged; this makes the proven v2 namespace/BOP runtime and NTIO plan one adapter session rather than two bridge implementations. Execution-plan, default-off #UD and documentation-governance checks pass. No Bochs build or guest trace is claimed. |
| S7 fresh runtime-trace root generation admission (2026-08-11) | The coordinator admits exactly one generator-only invocation of `tools/build/New-T95S7RuntimeTraceBuildRoot.ps1` into the previously absent `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r1`. **Scope:** copy immutable intact r4 provenance; replace only copied current `main.cc` and `cpu/exception.cc`; remove only their copied stale objects and copied CPU archive; copy/hash the 34 adapter and 3 CLI sources frozen by `t95-s7-bochs-runtime-trace-closure-audit-001.md` plus headers; derive one nmake shim with `EXECUTION_PLAN`, generic exception bridge and BOP listener on, and legacy startup/result bridge off. **Stop/non-goals:** no compiler/link/archive/executable invocation, no existing-root overwrite, no source/config/archive mutation, no device/plugin/firmware/OpenNT input, no retry/variant. **Exit:** a manifest-hashed prospective root and one exact future build command only; neither build nor runtime closure follows. |
| S7 runtime-trace root r1 stop and r2 admission (2026-08-11) | r1 generator stopped before any compiler/link/archive/executable action because intact r4 retains only `cpu/libcpu.a`, not the expected loose CPU object island. The partial r1 directory is retained as refusal evidence and is never overwritten. The coordinator admits exactly one corrected generator-only r2 root at `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r2`: it retains and hashes the immutable r4 CPU archive, recompiles only copied `cpu/exception.o` in the later build, and places that object before `cpu/libcpu.a` in the future link line. This avoids archive extraction/repacking and does not change CPU source selection. All other r1 scope/stop rules remain unchanged. |
| S7 r2 runtime-trace build observation admission (2026-08-11) | The coordinator admits exactly one MSVC x86 invocation in the frozen r2 root: initialize BuildTools x86, then run exactly `nmake /f ntdos64-s7-runtime-trace.mak ntdos64-s7-runtime-trace.exe`. Capture stdout/stderr, exit code, map and output identity if produced. **Scope:** only copied `main.o`, copied `cpu/exception.o`, the frozen 34 adapter plus 3 CLI objects, and the named executable link; the immutable copied r4 archives remain inputs. **Stop/non-goals:** first compiler/link failure stops without flag/source/config/archive/device/plugin/firmware changes or retry; no executable run, guest profile, BOP trace or OpenNT action. **Exit:** a source-to-binary classification only. A successful link authorizes a separately admitted controlled trace, not a runtime claim. |
| S7 r2 runtime-trace build observation 001 (2026-08-11) | The first actual MSVC x86 observation reached copied `cpu/exception.cc` after `main.cc` compiled, then stopped before adapter compilation/link/executable creation. MSVC reports the first failure at `exception.cc:343`: a superfluous closing brace after the v4/v3 generic guest-read selection ended `bx_ntvdm_adapter_interceptor` early, so its existing generic result-consumption statements were parsed at file scope. The r2 root and complete diagnostic are retained; no toolchain/config/archive/device/profile retry or runtime action occurred. The coordinator corrects only that unmatched brace in current source; no control-flow branch, result rule, BOP behavior or Bochs semantic changes. A future r3 root/build requires a separate admission. |
| S7 r3 corrected-root generation admission (2026-08-11) | The coordinator admits exactly one generator-only invocation into previously absent `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r3`, using the same r2 frozen closure and command shape with only the current one-brace syntax correction. It must retain r2 untouched, reject collision/missing/hash mismatch, and perform no compiler/link/archive/executable action. |
| S7 r3 corrected build observation admission (2026-08-11) | The coordinator admits exactly one MSVC x86 invocation in frozen r3: `nmake /f ntdos64-s7-runtime-trace.mak ntdos64-s7-runtime-trace.exe`, with the same scope/stop rules as r2. The only source delta is the recorded syntax correction. First compiler/link failure stops; no executable or guest trace is admitted. |
| S7 r3 corrected build observation 001 (2026-08-11) | r3 confirms the one-brace correction: copied `main.cc` and `cpu/exception.cc` compile (only the known narrowing warning remains). The build then stops before adapter compilation because the generated nmake shim emitted a continuation line without a leading `#`, which nmake treated as a command. This is a generator-only comment syntax defect; r3 remains immutable and no source/config/archive/adapter semantics changed. The generator now emits three independent comment lines; a further root/build observation requires a new admission. |
| S7 r4 shim-corrected root/build admission (2026-08-11) | The coordinator admits exactly one generator-only creation of previously absent `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r4`, then exactly one same-command MSVC x86 build observation in that root. The sole delta is the generator comment repair; r3 remains immutable. All prior closure, no-retry, no-executable-run, no-device/plugin/firmware and no-guest-trace constraints remain in force. |
| S7 r4 build observation 001 (2026-08-11) | r4 compiles copied `main.cc`, `cpu/exception.cc`, all 34 adapter and all 3 CLI objects, then reaches the final link. It stops with exactly two closure defects: omitted `bx_ntvdm_bop_catalog_v1.c` provider for the already enabled observation listener, and C++ name mangling of the C gather-read ABI declaration. The catalog provider is added to the frozen closure; `bx_ntvdm_guest_gather_read_action_v1.h` now wraps its C declarations in `extern "C"` when included by Bochs. No Bochs machine semantics, BOP dispatch policy, guest/host behavior, toolchain/config/archive or runtime action changes. A fresh root is required before another build. |
| S7 r5 closure-corrected root/build admission (2026-08-11) | The coordinator admits exactly one generator-only creation of previously absent `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r5`, then exactly one same-command MSVC x86 build observation in that root. The sole deltas are the admitted catalog provider and C linkage wrapper; r4 remains immutable. First build failure stops; no executable, guest or BOP trace action is admitted. |
| S7 r5 build observation 001 (2026-08-11) | The admitted r5 build exits zero: copied Bochs entry/exception objects, 35 adapter objects (including BOP catalog) and 3 CLI objects compile, and `ntdos64-s7-runtime-trace.exe` links at 2,541,568 bytes, SHA-256 `F4416F20F456366CC1AFC83D8B40748BF331D02E336BB9350EE8045329582DC2`; map SHA-256 `D36A9506213641FA21F218A836178B7B5099693C7EAF48AA74695EFD1617110D`. This is source-to-binary closure only, not a Bochs or guest runtime claim. **Next gate:** define a fresh watchdog-bound v2 source-built input observation before any executable invocation. |
| S7 first runtime trace admission (2026-08-11) | The coordinator admits exactly one invocation of `Invoke-T95S7RuntimeTraceObservation.ps1` into absent `artifacts/analysis/t95-s7-runtime-trace-001-20260811-001`, watchdog 15 seconds. Scope: fixed r5 binary, copied source-built NTIO/NTDOS/COMMAND identities, v2 contained placement profile, copied ROMs, child-only adapter environment and Bochs log/JSON evidence. Stop/non-goals: one process only; watchdog termination is evidence; no retry, source/config/device/plugin/firmware change, host capability expansion or runtime-success claim. |
| S7 inventory trace admission (2026-08-11) | The coordinator admits exactly one observation into absent `artifacts/analysis/t95-s7-runtime-trace-002-20260811-001`, watchdog 15 seconds, using the corrected observer with one immutable 26-entry DriveInfo type snapshot in the v2 profile. It tests only whether reached `50:0F` gains its already admitted inventory input; no host path/file access, device expansion, retry or runtime-success claim. |
| S7 inventory trace r2 stop and r3 admission (2026-08-11) | r2 stopped before Bochs launch while PowerShell converted the generated drive letter; its partial evidence directory remains immutable. The coordinator admits exactly one corrected observer invocation into absent `artifacts/analysis/t95-s7-runtime-trace-003-20260811-001`, same r5/input/watchdog contract. The sole delta is explicit numeric-to-char conversion; no retry of r2 process, host capability expansion or runtime-success claim. |
| S7 post-drive source admission (2026-08-11) | Trace 003 reaches `50:0D` and `54:04`. Source audit `etc/research/t95-s7-post-drive-bop-source-audit-002.md` corrects the selector classification: `50:0D` is `DEMGETBOOTDRIVE` (free space is `50:0E`), whose OpenNT fallback is AL=3/C; `54:04` is `CMDGETCURDIR`, which returns `X:\` to DS:SI for a valid drive and CF/AX=0 for an invalid one. The existing adapter units are now registered in the unified runtime only: boot drive uses the typed CPU result, while current-directory uses the existing generic multi-write and a valid-letter mask derived solely from the immutable profile inventory. The focused C11 runtime regression rebuilds and exits zero. No Bochs source/device change, host path I/O, rescan, environment-current-directory persistence, DOS filesystem or `50:0E` admission follows. |
| S7 r6 post-drive root/build admission (2026-08-11) | The coordinator admits exactly one generator-only creation of previously absent `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r6`, then exactly one MSVC x86 `nmake /f ntdos64-s7-runtime-trace.mak ntdos64-s7-runtime-trace.exe` observation in that root. Scope: immutable r4 provenance plus the already admitted current `main.cc`/`cpu/exception.cc`, 37 adapter and 3 CLI source files; the only closure additions are the registered adapter-owned boot-drive/current-directory units. Stop on first generator/build failure; do not retry or change source/config/archive/Bochs devices/firmware/guest inputs. Do not run the executable in this admission. Exit: source-to-binary classification only; a successful link separately authorizes one fresh watchdog trace. |
| S7 r6 runtime trace admission (2026-08-11) | The r6 build links `ntdos64-s7-runtime-trace.exe`, 2,543,104 bytes, SHA-256 `42CC21674491763EE6CDBEA7D6BF128950EC7FF8A7167FE388F5998622F07156`; map SHA-256 `B604A1A79E13D6403B3C7CB4755325D6263A60A61F12304C70ED1341431C9817`. The retained build log records the known `exception.cc` narrowing warning and the stock recursive make rebuilding the selected CPU archive; no archive-member choice or Bochs source changed. The observer now accepts an explicit build-root argument (default remains r5), so it can record binary identity without copying or overwriting artifacts. The coordinator admits one invocation with r6 into previously absent `artifacts/analysis/t95-s7-runtime-trace-004-20260811-001`, watchdog 15 seconds, same source-built payloads/profile/ROMs and child-only environment as trace 003. Stop after this one process; no retry, source/config/device/firmware change, expanded host capability, or runtime-success claim. |
| S7 r6 trace 004 prelaunch stop and 005 admission (2026-08-11) | Trace 004 preserved its copied source-built inputs, generated v2 profile, ROMs and bochsrc, but stopped before `Process.Start`: on this Windows PowerShell/.NET runtime `ProcessStartInfo.Environment` is null. No Bochs log, process, BOP or guest action exists in that evidence root. The observer now writes the same four child-only variables through the non-null `EnvironmentVariables` collection; this is process-launch plumbing only. The coordinator admits exactly one corrected invocation with the same r6/input/watchdog contract into previously absent `artifacts/analysis/t95-s7-runtime-trace-005-20260811-001`; 004 remains untouched and no retry happens in it. |
| S7 r6 trace 005 prelaunch stop and 006 admission (2026-08-11) | 005 retained the same prelaunch inputs but independently proves `EnvironmentVariables` is also null in this host runtime; no Bochs log/process/BOP/guest action exists. A direct `cmd.exe` child probe verifies command-local `set "VAR=value"` works without mutating the observer process environment. The observer now launches `cmd.exe /d /c` and applies the same four variables only in that command before executing the fixed binary. The coordinator admits exactly one r6 watchdog invocation into previously absent `artifacts/analysis/t95-s7-runtime-trace-006-20260811-001`; 004/005 remain immutable. |
| S7 r6 trace 006 environment stop and 007 admission (2026-08-11) | 006 starts r6 Bochs and records native reset/halt, but has zero BOP observations/commits because the nested quoted `cmd set` form did not set the intended adapter variables. It is retained as a launch-only negative result. The observer now uses unquoted `set NAME=value&&` assignments: all admitted r6 paths contain no spaces, and the assignments remain inside the sole `cmd` child. The coordinator admits exactly one corrected r6 watchdog trace into previously absent `artifacts/analysis/t95-s7-runtime-trace-007-20260811-001`; no source, Bochs, profile, device, firmware or host-capability change is included. |
| S7 r6 trace 007 result (2026-08-11) | Trace 007 is valid source-built runtime evidence: r6 SHA `42CC21674491763EE6CDBEA7D6BF128950EC7FF8A7167FE388F5998622F07156` exits 1 without watchdog, with 11 observed BOPs and two RAM commits. Its log proves `50:0D` at `8DC8:0667` resumes with the source-defined AL=3/C fallback and crosses the prior stop. The next unhandled identity is `50:21`, source-defined as `SVC_DEMIOCTL`/`demIOCTL`; `54:04` does not occur in this trace. Evidence and limits: `etc/research/t95-s7-runtime-trace-007-post-boot-drive.md`. No IOCTL/device/host-path admission or runtime-success claim follows. **Next gate:** analyze the reached DEMIOCTL caller/subfunction and failure behavior before implementing any response. |
| S7 DEMIOCTL discriminator admission (2026-08-11) | `demIOCTL` chooses its historical table entry from AL, but `BX-ABI-040` records identity only. Registered `BX-TRACE-043` permits one default-off append-only diagnostic of the already copied EAX/EBX low words on BOP logs. Scope is observation only: no selector/service/register branch, adapter ABI/result, guest-memory access, IOCTL response, device, host capability or CPU state change. Focused listener boundary test plus a fresh root are required before any trace; any implementation remains separately source/ABI/failure-gated. |
| S7 r7 IOCTL-discriminator root/build/trace admission (2026-08-11) | Admit exactly one fresh-root generator invocation into absent `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r7`, then one MSVC x86 `nmake /f ntdos64-s7-runtime-trace.mak ntdos64-s7-runtime-trace.exe` build, then one 15-second trace into absent `artifacts/analysis/t95-s7-runtime-trace-008-20260811-001`. The sole source delta from r6 is `BX-TRACE-043`'s append-only BOP log fields; the 37 adapter/3 CLI closure, profile, ROMs and execution flags remain unchanged. Stop on first generator/build/trace failure; preserve every root; no retry, IOCTL response, device, host path, firmware or feature change. |
| S7 r7 IOCTL-discriminator result (2026-08-11) | r7 trace 008 is valid source-built runtime evidence (SHA `D6250C04C13FE8F827917770471EDA25EED877559AF60B705A03DD232F8C2060`, 11 BOPs, two commits, exit 1): reached `50:21` has `AX=4408`, `BX=F002`. OpenNT source resolves this to the contained C-drive changeable-media query (`demIoctlChangeable`, AL=08, BL=02), whose fixed-drive result is AX=1/CF clear. Evidence: `etc/research/t95-s7-demioctl-4408-trace-audit.md`. **Next gate:** admit/test only this profile-inventory-derived direct CPU result; all other IOCTLs remain pass-through. |
| S7 DEMIOCTL 4408 local closure and r8 admission (2026-08-11) | Adapter-only `bx_ntvdm_dem_ioctl_changeable_service` now accepts exactly real-mode `50:21`, AX=4408 and a profile-admitted drive with Win32 type 2..6; it returns source-defined AX=0 for removable/type 2 or AX=1 otherwise, with CF clear. Invalid/no-root and every other IOCTL pass through. The focused C11 service test and runtime regression pass. Admit exactly one r8 generator into absent `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r8`, one MSVC x86 build, and one 15-second trace into absent `artifacts/analysis/t95-s7-runtime-trace-009-20260811-001`; only the new adapter source enters the r7 closure. No Bochs source, device, host path, profile or feature changes; stop on first failure and preserve roots. |
| S7 r8 DEMIOCTL runtime result and DEMFASTREAD classification (2026-08-11) | r8 trace 009 proves the exact `50:21 AX=4408` response resumes twice, then reaches `50:42 AX=4001`. The command-wrapper watchdog failed to stop its child; the explicitly identified r8 process was stopped and log retained. Source audit proves BP:AX reconstructs the adapter's valid private token and that i386 intentionally selects the fast BOP path, while visible `DEMFASTREAD` is an empty DEM handler that clears CF. It is therefore a missing historical x86 host-composition capability, not a token, guest, Bochs or device defect. `50:42` remains pass-through; no artificial CF fallback, DEMREAD substitution, MIPS-state mutation or fast-I/O implementation is admitted. Evidence: `etc/research/t95-s7-demfastread-host-composition-gap.md`. |
| S7 trace observer process-tree repair (2026-08-11) | The r8 watchdog observation exposed a tool-only containment defect: killing the `cmd.exe` wrapper did not kill its Bochs child. `Invoke-T95S7RuntimeTraceObservation.ps1` now invokes `taskkill /T /F /PID <wrapper>` only after its declared timeout, then waits for the wrapper exit. The script parses under PowerShell and documentation governance passes. This changes neither profile, product process launch, Bochs, adapter, guest state nor any retained trace; the next separately admitted trace may use it to avoid a second manual process cleanup. |
| S7 DEMFASTREAD recovery sweep (2026-08-11) | Exact-symbol and source-owner review across the imported OpenNT MVDM and `softpc.new` trees finds no alternate `DEMFASTREAD` implementation, fast-I/O worker or registration path. `handle.asm` deliberately chooses the fast branch on i386, but `demdisp.c` maps `50:42` to the CF-clearing placeholder. This is high-confidence evidence of an unavailable historical host-composition capability within the imported source set, not proof that no code ever existed elsewhere. The listener/pass-through decision remains unchanged: no `DEMREAD` substitution, synthetic CF failure, MIPS-state edit, filesystem expansion or Bochs change is admitted. Evidence: `etc/research/t95-s7-demfastread-source-sweep-001.md`. |
| S7 full BOP-observation regression (2026-08-11) | Re-ran the retained MSVC/x86 C11 catalogue executable at `artifacts/build/t95-s6-bop-catalog-msvc-c11-002/bx-ntvdm-bop-catalog-v1-test.exe` and the static Bochs listener boundary test `tests/bx-ntvdm-bop-catalog-listener-boundary.ps1`; both pass. The former verifies identity-only pass-through classification, including all 256 selector-byte forms; the latter rejects selector/service/DOS semantics crossing into the Bochs listener. The inventory/catalogue/listener boundary therefore remains full-observation and fail-closed, not an implicit dispatcher. |
| S7 DEMFASTREAD contained-contract admission (2026-08-11) | Source comparison establishes a narrow rehosting route for the reached `50:42`: its prepared `AX:BP`, `CX`, `DS:DX`, `BX:SI`, ZF and normal completion ABI are the same ordinary file-read contract as `50:16`, while only slow read has the unreachable-for-this-profile `HRDSVC` hard-error wrapper. The coordinator admits a distinct adapter endpoint limited to the already installed private read-only namespace and existing generic bulk-result channel. It must match exact `50:42`, preserve the direct success/EOF contract, and pass through all other cases. No synthetic slow fallback, MIPS mutation, fast write, host filesystem, new Bochs intrusion or runtime trace is admitted. Evidence and test plan: `etc/research/t95-s7-demfastread-contained-contract-001.md`. |
| S7 DEMFASTREAD local/runtime closure (2026-08-11) | `bx_ntvdm_dem_fast_read_service_v1_read` now exact-matches only real-mode `50:42`; its dedicated endpoint reuses the existing bounded namespace-to-generic-bulk mechanical helper without becoming a selector alias. Runtime registration queues/takes the same opaque bulk record as the separately admitted `50:16` endpoint, while keeping all precondition failures pass-through or within the existing direct error contract. Fresh MinGW C11 root `artifacts/build/t95-s7-demfastread-contained-c11-002` builds and runs both `bx-ntvdm-dem-readonly-file-service-test` (exact 42, payload/AX/CF, selector-negative) and `bx-ntvdm-adapter-runtime-test` (runtime `42` bulk queue/take) successfully. No Bochs build or guest trace follows from this local closure. |
| S7 r9 DEMFASTREAD root/build/trace admission (2026-08-11) | Admit exactly one generator-only creation of absent `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r9`, then one MSVC x86 `nmake /f ntdos64-s7-runtime-trace.mak ntdos64-s7-runtime-trace.exe` build, and if it links exactly one 15-second watchdog observation into absent `artifacts/analysis/t95-s7-runtime-trace-010-20260811-001`. The sole behavioral delta from r8 is the separately registered adapter `50:42` contained fast-read endpoint; all copied Bochs source, CPU archive/lifecycle, profile, ROMs, device/plugins and execution flags remain unchanged. Stop on the first generator/build/trace failure; retain every root. No retry, fast write, new guest resource, host filesystem, Bochs change or runtime-success claim is admitted. |
| S7 r9 pre-build stop and r10 admission (2026-08-11) | r9 generator succeeds and is retained. Its one build invocation stops before `VsDevCmd.bat`, compiler, nmake, object or executable action: the PowerShell-to-`cmd.exe` wrapper incorrectly passed literal escape characters around the path, so `cmd` rejected the command. No product/build input changed. The coordinator admits exactly one r10 generator root at absent `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r10`, then one MSVC x86 invocation whose single `cmd /d /c` argument is `call "C:\Program Files\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x86 && nmake /f ntdos64-s7-runtime-trace.mak ntdos64-s7-runtime-trace.exe`. The only delta is host-shell argument construction. If it links, the previously admitted single trace target `t95-s7-runtime-trace-010-20260811-001` remains available; stop on first failure, with no r9 rerun. |
| S7 r10 toolchain-path stop and r11 admission (2026-08-11) | r10 generator succeeds and is retained. Its one correctly quoted pre-build invocation stops before `VsDevCmd.bat`, compiler, nmake, object or executable action because the recorded `C:\Program Files\...` toolchain path does not exist. Read-only `vswhere` reports the installed x86-capable BuildTools root as `C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools`; no source or generated input changed. The coordinator admits exactly one r11 generator root at absent `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r11`, then one MSVC x86 build through `call "C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x86 -host_arch=x86 && nmake /f ntdos64-s7-runtime-trace.mak ntdos64-s7-runtime-trace.exe`. The sole delta is verified installed-toolchain path. On link success, use the already reserved one trace root; stop on first failure and never rerun r9/r10. |
| S7 r11 DEMFASTREAD build/trace observation 010 (2026-08-11) | The unique r11 MSVC/x86 root builds and links once (only the recorded `exception.cc` narrowing warning); the produced runtime binary hashes to `C926DBFDECE6066768BCF8353BB81D2F31D8AD841D823D1C1B9C18784FF22173`. Its sole 15-second watchdog observation is retained at `artifacts/analysis/t95-s7-runtime-trace-010-20260811-001`: 19 catalog observations and four commits include the source-built distinct `50:42` bulk read commit followed by an accepted resume and the next `50:02` close. It then observes top-level `5F` (`AX=BEEF`) and `5A` (`AX=0000`) as pass-through before later machine faults. No causation is assigned to those downstream faults, and no device or broad interceptor was enabled. |
| S7 top-level 5A/5F owner-audit admission (2026-08-11) | `etc/research/t95-s7-top-level-5a-5f-owner-audit-001.md` classifies the two reached identities from OpenNT source. It admits only an exact real-mode `5A` source-shaped no-idle-event return: advance by three BOP bytes with no guest-state delta and no host wait/event/VDD/PIF/device mechanism. `5F` remains listener-only because the historical `MS_bop_F` consumes the NTIO vector table and releases ICA interrupts; an independent Bochs-owner mapping is required before any implementation. Scope is one adapter service plus focused exact-window/mode/empty-delta test; no Bochs, OpenNT, guest input, CLI, IRQ/PIC/keyboard, or `5F` source change is authorized. Stop on any need for guest-table decoding, host scheduler state, new device or Bochs intrusion. |
| S7 5A fixture pre-build stop and fresh-root admission (2026-08-11) | Fresh modern root `artifacts/build/t95-s7-wait-if-idle-c11-001` configured and compiled the new service, then its fixture compile stopped because its assertion named nonexistent CPU-delta fields. The ABI itself proves v1 cannot modify those categories; only `gpr16_write_mask` and reserved zero need assertion. The root is retained. The coordinator corrects that test-only assertion and admits exactly one new modern root `artifacts/build/t95-s7-wait-if-idle-c11-002` to configure, build and run only `bx-ntvdm-wait-if-idle-service-test`; no runtime trace, Bochs/OpenNT input, device or capability change is admitted. |
| S7 5A local closure and r12 admission (2026-08-11) | Fresh root `artifacts/build/t95-s7-wait-if-idle-c11-002` configures, builds and runs `bx-ntvdm-wait-if-idle-service-test` successfully. The exact real-mode `C4 C4 5A` result resumes at RIP+3 with an empty v1 delta; protected mode, selector `5F` and RIP overflow are rejected. The runtime trace closure audit now records the authoritative 39-adapter/3-CLI source set, including the existing catalogue, boot-drive, IOCTL and current-directory sources as well as this new service. The coordinator admits exactly one generator-only r12 root at absent `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r12`, one MSVC/x86 build, and only if it links one 15-second trace at absent `artifacts/analysis/t95-s7-runtime-trace-011-20260811-001`. The sole behavior delta from r11 is exact `5A` continuation. `5F` remains listener/pass-through; no Bochs/OpenNT change, device, host scheduler/event/VDD/PIF, IRQ/PIC/keyboard, guest input, filesystem or CLI capability is admitted. Stop on first generator/build/trace failure; preserve all roots. |
| S7 r12 BOP 5A runtime result and next gate (2026-08-11) | r12 links once with only the known `exception.cc` narrowing warning; its binary is `F62A05E339C0FB9F4B2BA8DA387621C877EB93547BF794D1F5AD53AAFAED12A6`. The sole trace 011 records the exact `5A` resume at `0032:350b -> 350e`, `delta=00`, then repeats it until the watchdog (40,025 observed BOPs, four prior commits). Source correlation identifies the call site as KEYB's input/hold waiting loop, not a new service failure. The predecessor `5F` remains pass-through; this result does not attribute the loop to it, but makes its historical keyboard/timer/IVT/ICA initialization the sole next proof obligation. Evidence: `etc/research/t95-s7-bop-5a-runtime-result-011.md`. No further trace or BOP implementation is admitted until a read-only `MS_bop_F` field-to-Bochs-owner mapping classifies native ownership, composition input and unavailable state. |
| S7 5F register-observation admission (2026-08-11) | The read-only owner audit establishes that `MS_bop_F` derives its setup table from guest register state, while trace 011 retained only AX/BX. The coordinator admits registered diagnostic exception `BX-TRACE-044`: append copied CS/DS/SI/DI/CX/EFLAGS values to the existing default-off generic BOP identity log for every observed selector. Scope excludes all selector-specific branches, guest-memory reads, adapter results, CPU/device/memory changes, host services and OpenNT changes. Verify only static listener/default-off boundaries before a later separately admitted fresh trace; no `5F` implementation is authorized. |
| S7 5F owner-map and contained endpoint admission (2026-08-11) | `etc/research/t95-s7-bop-5f-bochs-owner-map-001.md` proves from `spckbd.asm` that the reached call is AX=`BEEF`, `DS:SI`/`DI:CX` handoff to removed SoftPC C-BIOS/monitor machinery, followed by `JC` to the x86 path. Bochs's existing PIC/PIT/8042 owners are initialized before guest execution; their retention is the machine-composition predicate, not an adapter action. The coordinator admits one exact adapter result for real-mode `C4 C4 5F` with AX `BEEF`: RIP+3 and CF=1 only. It must not decode the tables, read guest RAM, call a device, inject an interrupt, add input/host services, implement monitor/WOW hooks, or change Bochs/OpenNT. A focused fixture must reject wrong selector/mode/AX/overflow before any fresh runtime root is admitted. |
| S7 5F focused C11 observation admission (2026-08-11) | Admit exactly one fresh modern root `artifacts/build/t95-s7-spckbd-init-c11-001` to configure, build and execute only `bx-ntvdm-spckbd-init-service-test`. It may compile the fixed-width exception/CPU-result ABI and the one new adapter endpoint; no Bochs compilation, source-built guest trace, device feature, OpenNT input, CLI input, host capability or additional BOP is admitted. Preserve the root on the first configure/build/test failure. |
| S7 5F local closure and r13 admission (2026-08-11) | The unique C11 root `artifacts/build/t95-s7-spckbd-init-c11-001` configures, builds and executes the focused fixture successfully. It proves only exact `5F/BEEF` RIP+3/CF=1 completion and its negative cases. The trace closure now records 40 adapter plus three CLI sources, and the generic listener's registered `BX-TRACE-044` fields. Admit exactly one r13 generator root at absent `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r13`, one MSVC/x86 build, and only if linked one 15-second trace at absent `artifacts/analysis/t95-s7-runtime-trace-012-20260811-001`. The only behavior deltas are exact adapter 5F completion and append-only generic BOP register logging. No device/input/host scheduler/CLI expansion or further BOP endpoint is admitted; preserve every root and stop on the first failure. |
| S7 r13 BOP 5F runtime result and EMS audit gate (2026-08-11) | r13 links once (only known narrowing warning) and trace 012 records source-correlated `5F` inputs `CS=8dc8 DS:SI=073b:4454 DI=041d CX=4 AX=BEEF`, accepted resume to `45a9`, empty GPR delta and CF set. It therefore enters the intended `JC` x86 path. The next observed selector is `66`, inventory owner `emm_init`, followed by repeated selector `06`, inventory owner `illegal_op_int`; both remain generic listener/pass-through. Trace 012 is retained as `etc/research/t95-s7-bop-5f-runtime-result-012.md`. The sole next gate is a source-first EMS caller/owner audit; no EMS page frame/device/memory, `06` recovery, keyboard input, Bochs change, host capability, CLI expansion or further trace is admitted. |
| S7 BOP 66 EMS-unavailable endpoint admission (2026-08-11) | `etc/research/t95-s7-bop-66-ems-unavailable-audit-001.md` traces the reached call to `InitSpcEmm`. OpenNT source defines `BX=0` as the unavailable outcome and its caller declines to install `EMMXXXX0`/INT 67h. Admit one adapter-only v2 endpoint for exact real-mode `C4 C4 66`: set low `BX=0`, resume RIP+3, and make no other delta. A focused fixture must reject wrong selector/mode/window/overflow. No Bochs or OpenNT edit, EMS page frame/allocator/device, PIF/host configuration, guest RAM/IVT operation, FLAGS change, `67/68` endpoint, `06` recovery, or runtime trace is admitted. |
| S7 BOP 66 local closure and r14 admission (2026-08-11) | Fresh root `artifacts/build/t95-s7-emm-unavailable-c11-001` configures, builds and executes `bx-ntvdm-emm-unavailable-service-test` successfully. It proves only exact real-mode `66` continuation with low `BX=0`; wrong selector, v8086 mode and RIP overflow decline. The frozen trace closure now has 41 adapter plus three CLI sources. Admit exactly one generator root at absent `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r14`, then one MSVC/x86 `nmake /f ntdos64-s7-runtime-trace.mak ntdos64-s7-runtime-trace.exe`; only if that links, admit exactly one 15-second trace at absent `artifacts/analysis/t95-s7-runtime-trace-013-20260811-001`. The sole behavior delta is the source-defined unavailable `66` response. Stop on first generator/build/trace failure. No EMS page frame/device/memory, `67/68`, `06` recovery, Bochs/OpenNT input, host/PIF/CLI capability or extra BOP is admitted. |
| S7 r14 BOP 66 runtime result and BOP 06 fault gate (2026-08-11) | r14 links once with only the existing narrowing warning; trace `013` pins binary `746BF963D2D89E55C6340AC2F7B97656B69A2BEA6657F76669FD41482BA5FB6D`. It records exact `66` at `8dc8:79c5`, accepted RIP+3 to `79c8`, and low `BX=0`; no `67/68` is observed. The next repeated identity is `06` at `073b:0740`. `etc/research/t95-s7-bop-06-illegal-instruction-audit-001.md` proves it is the guest INT 6 invalid-instruction vector, whose original handler reads the pre-vector CS:IP from SS:SP. It must remain listener/pass-through: no synthetic success, one-byte skip, EMS connection, host UI, device, guest change or additional runtime trace is admitted. The sole next gate is a bounded generic read-only fault-frame diagnostic adequate to source-correlate the original opcode. |
| S7 BOP 06 generic fault-frame diagnostic admission (2026-08-11) | Register `BX-TRACE-045` before implementation. Extend the existing default-off all-BOP identity listener only: for any real-mode BOP, append copied SS, low SP, and a four-byte ordinary-RAM snapshot at `SS*16+SP` only after full-span validation. It must neither inspect selector/service nor call adapter/runtime code, return a disposition, alter CPU/exception/memory/device/guest/host state, or handle an unreadable/wrapped span. Update the focused listener boundary test to require this fixed read-only shape. Static tests only precede a separately admitted fresh trace; no BOP endpoint, guest source, device, host capability or CLI behavior is admitted. |
| S7 BOP 06 diagnostic static closure and r15 admission (2026-08-11) | The BOP listener boundary, default-off boundary, documentation-governance and diff checks pass after `BX-TRACE-045`; the listener contains only the registered selector-independent real-mode four-byte read. Admit exactly one fresh r15 generator root at absent `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r15`, one MSVC/x86 build, and only if linked one 15-second trace at absent `artifacts/analysis/t95-s7-runtime-trace-014-20260811-001`. The sole change is appended stack-frame diagnostics. Stop on first generator/build/trace failure; no BOP result/endpoint, CPU/guest/device/host/CLI semantic change, source input change or retry is admitted. |
| S7 r15 fault-frame result and code-byte diagnostic admission (2026-08-11) | r15 links with the existing narrowing warning and trace `014` pins `D3FCBC1CE0B8568EB5216DB8B76E50BE8F2D2791C8A4A62AEF8465BF49129C5B`. Its first `06` stack snapshot is `cb78c88d`, proving pre-vector `CS:IP=8DC8:78CB` (physical `0x9554B`); later snapshots are nested exception frames. No source listing/map resolves that physical byte address. Extend already registered `BX-TRACE-045` only with an eight-byte ordinary-RAM snapshot at the stack-derived real-mode CS:IP, with complete validation and no selector/service branch. Admit exactly one static boundary check, then one fresh r16 generator root at absent `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r16`, one MSVC/x86 build, and only if linked one 15-second trace at absent `artifacts/analysis/t95-s7-runtime-trace-015-20260811-001`. No BOP endpoint/result, decoder interpretation, guest/device/host/CLI change or retry is admitted. |
| S7 r16 diagnostic-address stop and r17 admission (2026-08-11) | r16 links and trace `015` retains binary `ECF8C8905F873C3FB2F0DB76C3BD5B9CA78975847FCF2C46D76F51B0F94902BB`. It repeats the verified `06` stack `cb78c88d`, but safely reports `frame-valid=0`: the new diagnostic constructed the 16-bit saved CS with two independently shifted bytes rather than first forming the word then multiplying by 16. This is a contained diagnostic address-arithmetic defect, not a guest/Bochs result change; r16 remains immutable. Correct only that expression. Admit one static check, then exactly one fresh r17 generator root at absent `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r17`, one MSVC/x86 build and, only if linked, one 15-second trace at absent `artifacts/analysis/t95-s7-runtime-trace-016-20260811-001`. No endpoint, CPU/device/guest/host/CLI or decoder semantic change is admitted. |
| S7 r17 BOP C8 discovery gate (2026-08-11) | r17 links with the known narrowing warning and trace `016` pins `F2458007BAA1B3A6E45C797BF6FE4EA1559CF241EEE90248A73858053B751285`. Its first `06` frame is valid and reads `C4 C4 C8 33 C0 8E C0 26`: the upstream fault is BOP `C8`, not an x86 decoder failure. `etc/research/t95-s7-bop-c8-headless-mouse-owner-audit-001.md` maps it to the historical host mouse-install first phase and identifies its guest `mio_table`/IVT contract. `06` remains listener/pass-through. The next gate is a read-only exact-table/IVT dependency audit before any adapter mapping transaction; no host mouse, GUI, Bochs device, IRQ/EOI action, guest-memory write, BOP result or further trace is admitted. |
| S7 BOP C8 exact mapping admission (2026-08-11) | The C8 audit now binds actual r17 `CS:BX=8DC8:77BF` to the source-created `mio_table`. Admit one adapter-only, source-derived transaction: exact real-mode `C4 C4 C8` gathers only four bytes at `CS*16+BX`, then atomically writes those bytes only to IVT INT 33h (`0xCC..0xCF`) and resumes at RIP+3. The immediate guest continuation consumes that original vector before installing its own wrapper. Reject all historical host mouse state, GUI/pointer action, IRQ9/PIC/EOI work, INT 71h, input, device and Bochs change. A focused C11 service fixture must prove exact selector/mode/window/range/overflow rejection before any fresh root or trace is admitted. |
| S7 BOP C8 focused C11 observation admission (2026-08-11) | Admit exactly one new modern root `artifacts/build/t95-s7-mouse-install1-mapping-c11-001` to configure, build and execute only `bx-ntvdm-mouse-install1-mapping-service-test`. It may compile the fixed-width ABI, existing generic gather/multi-write records, and the new adapter endpoint; no Bochs compilation, guest input, runtime trace, host mouse/UI, device, interrupt, PIC/EOI, filesystem or CLI capability is admitted. Preserve the root on the first configure/build/test failure. |
| S7 BOP C8 focused C11 observation 001 and corrected r2 admission (2026-08-11) | Root `artifacts/build/t95-s7-mouse-install1-mapping-c11-001` configured, compiled and linked the focused fixture, then its first execution stopped at the fixture's expected physical table address. Source arithmetic confirms `8DC8:77BF = 0x9543F`; the fixture and evidence had transposed that as `0x9553F`. Preserve r1. Correct only that expected value and admit exactly one fresh `artifacts/build/t95-s7-mouse-install1-mapping-c11-002` configure/build/run with identical source closure and no Bochs/runtime/device/host/CLI expansion. |
| S7 BOP C8 focused C11 result and runtime-closure admission (2026-08-11) | The corrected r2 root compiles, links and executes `bx-ntvdm-mouse-install1-mapping-service-test` successfully. It proves the exact four-byte `CS:BX` gather contract, four-byte INT 33h target, byte-preserving payload and selector/mode/overflow rejection. Admit exactly one fresh modern root `artifacts/build/t95-s7-c8-runtime-closure-c11-001` to configure/build/run only the existing `bx-ntvdm-adapter-runtime-test`, now with 42 adapter sources. This checks the completion-to-opaque-multi-write composition only; it does not build Bochs or run guest/BOP behavior. |
| S7 BOP C8 runtime closure result and r18 admission (2026-08-11) | The 42-source adapter runtime fixture compiles, links and executes in `artifacts/build/t95-s7-c8-runtime-closure-c11-001`; default-off, BOP-listener and documentation-governance checks also pass. Admit exactly one generator-only fresh root `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r18`, then one MSVC/x86 `nmake /f ntdos64-s7-runtime-trace.mak ntdos64-s7-runtime-trace.exe`; only if it links, admit one 15-second trace at absent `artifacts/analysis/t95-s7-runtime-trace-017-20260811-001`. The sole behavior delta is the exact adapter C8 table-to-INT33 transaction. Stop on first generator/build/trace failure and preserve all roots. No Bochs/OpenNT source change, mouse hardware/input/UI, INT71, IRQ/PIC/EOI, host filesystem or CLI expansion is authorized. |
| S7 r18 BOP C8 runtime result and next gate (2026-08-11) | r18 MSVC/x86 links the 42-source closure (binary SHA-256 `DC5B7550FFC5DBB120432C422929C7F505DD72062B9454B39C20EDA05957D9A2`). Its unique trace `017` proves the reached C8 boundary copies exactly one four-byte range and commits exactly one four-byte opaque multi-write. Thus the original guest's INT33 vector handoff is now source-built and observed; no mouse device/input/UI/IRQ/EOI/INT71 claim follows. The later first INT6 frame begins `C4 C4 17 5A`, making selector `17` the sole next source-first owner/caller audit. No further trace, C8 expansion or exception recovery is admitted until that audit classifies its minimal original semantics. |
| S7 BOP 17 printer-unavailable endpoint admission (2026-08-11) | `etc/research/t95-s7-bop-17-printer-unavailable-audit-001.md` maps reached BOP 17 to `printer_io`. In the original source every printer action is under `#ifdef PRINTER`; the device-free build branch is an empty native return. Admit one adapter-only exact real-mode `C4 C4 17` endpoint: RIP+3 and empty CPU delta only. A focused fixture must reject mismatched selector/mode/window/overflow before any fresh runtime root. No printer port/device, host printer/spooler/UI/filesystem, AH-status invention, interrupt, Bochs or OpenNT change is authorized. |
| S7 BOP 17 focused C11 observation admission (2026-08-11) | Admit exactly one fresh modern root `artifacts/build/t95-s7-printer-unavailable-c11-001` to configure, build and execute only `bx-ntvdm-printer-unavailable-service-test`. It may compile the fixed-width exception/result ABI and one adapter endpoint; no Bochs build, guest trace, printer device, port, host output, UI, filesystem or CLI capability is admitted. Preserve the root on first failure. |
| S7 BOP 17 runtime-closure r1 stop | The focused fixture passes. Root `artifacts/build/t95-s7-printer-runtime-closure-c11-001` compiles all prior runtime sources then stops at link because the runtime target omitted `bx_ntvdm_printer_unavailable_service.c`. Preserve r1. The CMake runtime source list is corrected; a new root and test are required before any Bochs build or trace. |
| S7 BOP 17 runtime closure result and r19 admission | Corrected root `artifacts/build/t95-s7-printer-runtime-closure-c11-002` compiles, links and executes the 43-source adapter runtime test. Admit one fresh r19 root `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r19`, one MSVC/x86 build, and if linked one 15-second trace in `artifacts/analysis/t95-s7-runtime-trace-018-20260811-001`. Sole behavior delta: exact BOP 17 empty return. |
| S7 r19 BOP 17 runtime result and next gate | r19 links MSVC/x86 binary SHA-256 `8A2B00259DD5BBD54C3CB009F82E1B800AA01A484C667B53B6B5AD478B411F00`. Trace `018` reaches selector 17 and continues to selector 5E before INT6; therefore the exact no-printer return is consumed and no printer capability claim follows. Selector 5E is the sole next source-first caller/owner audit; no notification, UI, host integration, trace or exception recovery is admitted before it. |
| S7 BOP 5E configuration-notification admission | Source audit proves reached AL=0 is CONFIG.SYS completion. `UMBNotify(0)` is an original no-op; `demDasdInit` is denied raw-disk host initialization. Admit one adapter endpoint for exact real-mode `C4 C4 5E` with AL=0: RIP+3 and no delta. Reject nonzero AL, UMB mapping, raw-disk/DAD tables, probing, host notification, Bochs/OpenNT changes. Focused fixture precedes any trace. |
| Asset needs | Existing admitted NTIO/NTDOS/COMMAND profile inputs and retained OpenNT source only. No additional guest media, firmware, host drive or external source input is admitted. |
| Risks | Turning an identity payload into an implicit host filesystem; modeling CONFIG/AUTOEXEC as bare RAM writes; stale token reuse; leaking Win32 handles/paths across the ABI; and silently using broad legacy runtime dispatch. |
| Stop conditions | Stop if a faithful first profile requires ambient NT/PIF/SystemRoot/temp state, a new Bochs semantic/device change, unbounded guest-string reading, host mutation, raw pointer/handle crossing, or a separate COMMAND/configuration fast path. Record the missing source condition rather than guessing it. |
| Exit criteria | A selected profile explicitly and reproducibly places COMMAND at its canonical guest path; CONFIG and AUTOEXEC are selected/materialized as one contained policy; DEM Open/Seek/Read/Close has a complete bounded source contract and tests; all host resource lifetimes are adapter-owned; no Bochs or guest semantic leakage occurs; and a fresh runtime trace states exactly what has (and has not) executed. |
| Original owner request | Continue a CLI non-invasive NTVDM with Bochs as minimal SoftPC replacement, an adapter as the sole boundary, explicit BOP observation, source-shaped rehosting, and no unprincipled growth. |
| Similar-issue sweep | Review profile admission, runner handoff, startup-session payload ownership, host namespace roots, bulk-result lifecycle, COMMAND/configuration callers, DEM error/retry behavior, BOP listener/dispatch separation and Bochs intrusion registry together. |

## Current Technical Baseline

- Bochs 2.6 is the only approved execution backend candidate, from `O:\repos.external\bochs-2.6-compat\bochs-2.6`; the existing Bochs 3.0 snapshot under `refs/bochs` is not admitted.
- Bochs owns machine mechanics only. OpenNT owns guest DOS/WOW/BOP/DEM/COMMAND semantics. The typed bridge is their sole connection.
- The initial Bochs configuration is deny-by-default: no GUI, debugger, plugins, USB, networking, SMP, APIC, PCI, sound, optional storage controller, or device feature enters the build without a reached first-profile caller and an explicit admission record.
- Historical SoftPC/CCPU/BIOS/DEM sources and fixtures remain behavior evidence, not a runtime dependency.
- `ntvdm64` remains a downstream comparison/handoff target, never a build, link, or runtime dependency.
- The native `nogui` UCRT configuration and r4 build close with two registered
  Bochs exceptions: `BX-UD-001` and `BX-BUILD-002`. The earlier r3 stop at
  `config.cc:3261` remains retained evidence for the latter; r4 `bochs.exe`
  starts and exits through `-help`, but no guest/runtime claim follows.
- `BX-UD-001` is the only approved direct Bochs intrusion: a default-off,
  generic #UD hand-off with fixed-width records and original-path fall-through.
  It is not a BOP or OpenNT semantic extension.
- The r4 native reset fixture proves original reset, triple-fault reset and
  benchmark-bounded stop only. It runs no OpenNT input and is not a DOS boot or
  adapter-runtime claim.
- Adapter exception ABI v1 is authored as a fixed-width C11 header and
  default-disabled layout target; its direct MSYS2 UCRT64 C11 layout test has
  compiled and run successfully. It remains independent from the Bochs r5
  binary and no OpenNT bridge is implied.
- r9 additionally proves a generic `#UD` boundary can copy a validated,
  fixed-width register/selector snapshot to the same-island adapter and retain
  the original exception path by pass-through. It is neither BOP recognition
  nor a guest-memory/OpenNT-runtime claim.
- The adapter-local selector map v1 proves only the owner identity recovered
  from the original `BIOS[50h]` initializer. It rejects every other selector,
  contains no function pointer or BOP decode, and is not connected to Bochs or
  OpenNT execution.
- `BX-ABI-006` adds only a bounded raw instruction-window observation to the
  generic #UD transport. The adapter validates it and remains pass-through;
  no marker recognition, owner call, or OpenNT connection is implemented.
  Its independent C11 tests pass. The attempted r10 native build stopped in
  an unrelated GCC-16/configure incompatibility before adapter compilation.
- Adapter BOP selector probe v1 now admits only the copied `C4 C4 50` boundary
  and returns an owner identity. It remains a fail-closed classifier: it does
  not read a service byte, execute an owner, or alter Bochs/OpenNT state.
- The first-owner closure audit proves the original `MS_bop_0 -> DemDispatch
  -> demLoadDos` fixture is a rejected CCPU/SAS product-composition graph,
  not a Bochs-linkable runtime island. Any source-derived replacement for its
  one NTDOS-load transition now requires separate explicit admission; see
  `etc/research/first-owner-transaction-closure-audit.md`.
- With owner approval for the holistic adapter host-service plane, catalogue
  v1 now admits the complete numeric DEM (`50:00-48`) and COMMAND
  (`54:00-10`) dispatcher ranges through one fail-closed lookup. It executes
  no handler and is documented in `etc/research/adapter-host-service-catalog-v1.md`.
- Shared host transaction ABI v1 now requires every future service mutation to
  preflight a catalogue request, CPU snapshot and complete guest-write range
  in one pointer-free record. The adapter still has no direct memory access;
  its backend commit seam is separately registered; see
  `etc/research/adapter-host-transaction-abi-v1.md`.
- Session resource ABI v1 represents the CLI-admitted NTIO/NTDOS/COMMAND
  identity set without paths, handles or payload pointers. It is documented in
  `etc/research/adapter-session-resource-abi-v1.md` and has a passing C11
  layout/negative test.
- Host-service plane v1 prepares the first source-derived DEM NTDOS-load
  transaction from `DI:0000` only after catalogue/resource/CPU/range
  validation. It writes nothing and leaves all other services unhandled; see
  `etc/research/adapter-host-service-plane-v1.md`.
- Host session v1 now makes that prepared write single-consume: it owns the
  admitted payload only inside the adapter island and returns a resume address
  only with a preflighted pending transaction. Its C11 positive/negative test
  passes; see `etc/research/adapter-host-session-v1.md`.
- The registered Bochs `BX-MEM-007` mechanism supplies only an all-or-nothing
  ordinary-RAM copy primitive. It rejects handlers, ROM/VGA/PCI regions,
  A20 remapping and monitored pages before any byte is copied; it owns no
  session or service semantics and no Bochs r10 executable rebuild is claimed.
- The adapter runtime now owns one profile-validated, identity-checked payload
  session and is connected to the generic #UD/ordinary-RAM bridge. r11 proves
  the bounded C/C++ object closure (`cpu/exception.o` and `memory/misc_mem.o`)
  without linking an executable; see
  `etc/research/adapter-r11-object-closure.md`. CLI environment installation
  and reset-to-stop runtime evidence remain pending.
- r11 machine-assembly audit proves the stock Bochs `devices.cc` path is not a
  bare machine: it unconditionally assembles timer/CMOS/DMA/PIC/PIT/floppy/
  VGA and keyboard infrastructure. The selected direction is therefore an
  adapter-owned composition interface over retained Bochs mechanisms, not a
  configuration claim that this default PC is minimal; see
  `etc/research/bochs-machine-assembly-boundary-r11.md`.
- r12 now supplies the first real runtime evidence: a strict BYOB resource set
  crosses reset, the admitted `50:11` transaction commits into Bochs ordinary
  RAM, and the process reaches its bounded native stop. This proves the first
  adapter transaction only, not NTDOS boot or COMMAND execution; see
  `etc/research/adapter-r12-runtime-fixture.md`.
- The r12 fixture also proves that the first adapter-owned composition
  declaration can use Bochs's existing `plugin_ctrl` gate to keep the six
  optional default plugins out of the reset closure. The stock hard-wired core
  remains in place; this is neither a bare-machine nor a DOS-runtime claim.
  See `design/BOCHS-MACHINE-COMPOSITION.md`.
- The CLI runner now installs the accepted BYOB profile/root into the engine's
  child-only adapter environment in addition to its explicit engine arguments.
  The independent policy test proves both handoffs agree; it does not claim an
  NTDOS boot. See `etc/research/runner-adapter-install-contract.md`.
- Stock `bochs.exe` is not yet a valid `ntdos64-run --engine` value because
  its native command-line/configuration ABI differs from the runner ABI. A
  future thin configuration-materializing shim is deferred until a complete
  first guest-startup descriptor exists, so target arguments cannot be silently
  discarded. See `etc/research/bochs-engine-startup-closure.md`.
- The first guest-startup descriptor now distinguishes source-locked NTIO
  preload/entry facts from unproven Bochs-firmware and reset-state inputs. It
  explicitly keeps the later `50:11` NTDOS transaction out of initial preload
  semantics and leaves BIOS `12h` unhandled. See
  `etc/research/first-guest-startup-descriptor-v0.md`.
- `BX-MEM-010` adds a generic, all-or-nothing ordinary-RAM observation
  primitive. Its r13 object closure and boundary test pass, but no adapter
  observation request is connected yet; it cannot be used as an arbitrary
  guest-memory reader. See `etc/research/adapter-r13-ordinary-ram-observation.md`.
- Observation transaction ABI v1 now binds one non-empty aperture range to a
  copied `#UD` boundary and CPU snapshot, with no output pointer in its
  versioned record. Its default-disabled C11 positive/negative test passes;
  runtime queueing and Bochs bridge wiring remain deliberately absent. See
  `etc/research/adapter-observation-transaction-abi-v1.md`.
- Session pending-operation v1 now unifies write and future observation
  ownership: both are bound to their copied `#UD` boundary, cancel on mismatch
  and have one take only. Adapter unit tests and the r12 write fixture pass;
  observation remains unconnected by design. See
  `etc/research/adapter-pending-operation-lifecycle-v1.md`.
- Machine-profile observation ABI v1 now materializes opaque, prevalidated
  range declarations from optional, profile-owned BYOB metadata. The adapter
  can prepare and single-queue one observation against its copied exception
  boundary; no raw address CLI/environment input, Bochs read, stop fixture or
  guest-runtime claim exists. See
  `etc/research/adapter-machine-profile-observation-abi-v1.md`.
- r14 compiles the profile-observation adapter path together with the admitted
  generic #UD and ordinary-RAM objects using explicit object targets only. Its
  same-version executable link is deliberately deferred until a real passive
  observation trigger/stop contract exists; see
  `etc/research/adapter-r14-profile-observation-object-closure.md`.
- The selected next mechanics gate is a profile-owned neutral `UD2;HLT`
  fixture: it pairs one opaque range with an exact copied #UD window and uses
  the existing native halt/benchmark path instead of inventing adapter stop
  control. Profile parsing, machine-profile matching and adapter-owned output
  queueing now pass; the registered Bochs consumer first had object-only
  evidence, before r15 supplied its separate executable fixture evidence. See
  `design/ADAPTER-PASSIVE-OBSERVATION-TRIGGER.md`.
- `BX-MEM-011` is now the registered generic Bochs consumer for an already
  queued observation transaction. Its r14 `cpu/exception.o` object compiles
  with the trigger runtime; r15 later provides the required same-version
  executable and fixture evidence. See
  `etc/research/adapter-r14-profile-observation-object-closure.md`.
- r15 now has same-version executable and runtime evidence for the neutral
  `UD2;HLT` passive-observation fixture: one profile-owned ordinary-RAM read
  commits, resumes and reaches native bounded stop. This is mechanics evidence
  only, never NTIO/NTDOS/COMMAND runtime evidence; see
  `etc/research/adapter-r15-passive-observation-fixture.md`.
- The next evidence target is deliberately the stock-firmware BDA `0x496`
  probe reached through a neutral boot sector after the original BIOS `INT 19h`
  path. It remains design-only until adapter-owned result digesting and its
  one generic Bochs log call are admitted; see
  `design/STOCK-FIRMWARE-STARTUP-PROBE.md`.
- r16 supplies the first selected-profile firmware input: CPU level 5 and the
  explicit stock-floppy probe reach `0000:7C00`, then observe BDA `0x496` as
  `0x10` through the generic adapter transaction. It proves neither IVT or
  `0x714` values, NTIO entry, DOS boot, nor any new device admission. See
  `etc/research/adapter-r16-stock-firmware-bda-probe.md`.
- Startup firmware snapshot ABI v1 now supplies the separate, fixed-width
  multi-range transaction needed to obtain the remaining pre-entry inputs at
  one neutral boundary. Its C11 test passes; it is not yet parsed, queued or
  consumed by Bochs, so it creates no additional guest-memory capability. See
  `etc/research/adapter-startup-snapshot-abi-v1.md`.
- r17 now supplies same-version custom-target runtime evidence for the generic
  three-range startup snapshot: stock firmware reaches `0000:7C00`, all ranges
  preflight and copy, and the adapter emits one composite digest before bounded
  stop. It is mechanics evidence only; IVT and `0x714` bytes remain unexposed
  and NTIO preload remains unimplemented. See
  `etc/research/adapter-r17-stock-firmware-startup-snapshot.md`.
- r18 now closes the native evidence-export gate: the declared same-version
  custom fixture writes the adapter-owned JSON snapshot atomically, and its
  digest matches the generic Bochs completion log. The selected CPU5 stock
  profile raw IVT/BDA/`0x714` bytes are recorded but not yet source-compatible
  admitted; NTIO preload is still unimplemented. See
  `etc/research/adapter-r18-startup-snapshot-evidence-export.md`.
- The r18 raw inputs are now source-decoded and admitted only through NTIO's
  first `50:11` boundary: all required first-prefix IVT values are saved, the
  BDA selects the documented extended-keyboard branch, and the virtual-
  interrupt word takes the normal x86 FCLI/FSTI route. This is not general
  firmware compatibility. See
  `etc/research/adapter-r18-first-prefix-input-admission.md`.
- r20 extends the same bounded startup snapshot with the source-reached BDA
  conventional-memory word at `0040:0013`. Stock CPU5 firmware reaches the
  neutral `0000:7c00` boundary and records `7f 02` (`0x027f`, 639 KiB) in one
  four-range digest. This is evidence for the later OpenNT BOP `12h` owner,
  not a BIOS emulation or an admitted CPU-result mutation; see
  `etc/research/bop-12-memory-size-contract-r20.md`.
- BOP progress is now assessed from the OpenNT caller and owner, rather than
  from selector order alone.  The reached COMMAND `54:0C` call is proven to
  require a materialized, subsequently reachable CONFIG.SYS pathname and to
  terminate the historical VDM on failure; it is therefore an unavailable
  paired namespace/configuration capability, not a Bochs or CPU defect.  See
  `etc/research/t95-s6-bop-source-control-flow-method-001.md` and
  `etc/research/t95-s6-cmd-getconfigsys-capability-audit-001.md`.
- The adapter's generic bounded bulk-result session lifecycle now copies one
  declared payload into a private fixed arena, cancels on a mismatched copied
  boundary, and preserves a successfully borrowed span until the next queue
  or session teardown.  A direct MSVC/x86 C11 closure compiles and executes
  the full host-session test with exit status zero under
  `artifacts/build/t95-s6-bulk-result-host-session-msvc-c11-001`.  It does
  not connect a BOP or add a Bochs consumer; see
  `design/ADAPTER-BULK-RESULT.md`.
- The complete BOP catalogue C11 test has also been rebuilt and executed in
  the isolated MSVC/x86 root
  `artifacts/build/t95-s6-bop-catalog-msvc-c11-002`.  Besides the pinned
  source-definition classification checks, it verifies all 256 possible
  `C4 C4 <selector>` forms are observable; source absence changes only their
  classification, never listener visibility or pass-through behavior.
- The generic Bochs `#UD` boundary verification now deliberately excludes the
  separately governed, default-off BOP catalogue listener block before it
  checks its semantic-token prohibition.  The listener has its own static
  boundary check, so this prevents an approved passive observer from masking
  a regression in the generic transport or being mistaken for a dispatcher.
  Both checks, the source-inventory coverage check (56 source-defined selector
  values), and documentation governance passed on 2026-08-11.
- COMMAND placement is now explicitly separated from later COMMAND BOPs:
  historical `54:02` mirrors an already established guest COMSPEC into the
  host environment; it neither locates nor loads `COMMAND.COM`.  Current BYOB
  COMMAND identity is not guest placement.  See
  `etc/research/t95-s6-command-guest-path-provenance-001.md`.
- The source-shaped placement prerequisite is now explicit: the first profile
  may expose the already verified COMMAND payload only through a separately
  declared, session-owned canonical guest placement (`\\COMMAND.COM` on the
  declared initial DOS drive).  BYOB root, host drives, `54:02`, and Bochs do
  not supply that placement.  This closes the S7 design input only; it does
  not implement a DOS file projection or resume any additional BOP.  See
  `etc/research/t95-s6-guest-artifact-placement-contract-001.md`.
- BOP evidence is now classified by three separate factsâ€”generic listener
  observation, exact adapter-unit presence, and source-built minimal-route
  evidenceâ€”so historical/broad runtime source cannot be mistaken for a
  connected product capability.  The complete matrix records the small
  admitted startup exceptions and keeps all remaining identities, including
  `54:0C/0D` and the Open/Seek/Read/Close family, listener-only pending their
  coherent S7 file capability.  See
  `etc/research/t95-s6-bop-admission-state-matrix-001.md`.
- The earlier x87/IRQ13/`0xA1800` symptoms are retained, but are no longer a
  current mechanics gate: they followed the then-unserved post-`50:11` BOP
  `12h`, whereas later source-built r29/r30 flow reaches `54:0C`.  No FPU,
  PIC/IVT or VGA feature is admitted from the old trace; the current gate
  remains the coherent configuration/read-only file capability.  See
  `etc/research/t95-s6-post-5011-mechanics-symptom-supersession-001.md`.
- r21 adds the source-owned `50:36 SVC_DEMENTRYDOSAPP` adapter endpoint.  The
  original owner only walks registered VDD create-user callbacks; the contained
  CLI profile declares no VDD registrations, so the empty-list source behavior
  is a precise no-state return.  Its C11 test passes and the same-version MSVC
  runtime records `50:36` at `0032:5a70` followed by an accepted resume to
  `0032:5a74`; binary SHA-256 is
  `A4002F40C5627A17CDE3D39978C289F84B9D641ED7E5B9EB8F292D6A44763395` and
  evidence is `artifacts/analysis/t95-s7-runtime-trace-020-20260811-001`.
  See `etc/research/t95-s7-bop-50-36-vdd-create-user-notify-audit-001.md`.
  The following reached `54:02 SVC_CMDCOMSPEC` remains deliberately unserved:
  OpenNT uses it to mirror an already materialized guest COMSPEC pathname into
   historical host state.  It now makes the declared guest-placement/read-only
   namespace contract the current capability gate, not a Bochs machine defect.
- The later source-built r24 trace reaches the separately admitted contained
  `50:42 SVC_DEMFASTREAD` path and correctly establishes that zero bytes at
  EOF is an ordinary `demRead` success.  Its retained `delta=2f`, however,
  also writes BP/BX/CX/DX.  OpenNT `demhndl.c` defines the complete result as
  AX plus CF only, while `handle.asm` passes the opaque file token in AX:BP.
  The adapter's shared read-result constructor is corrected and its focused
  C11 fixture passes with an exact AX-only write mask.  This is not yet a new
  runtime claim: one separately admitted same-input observation is required
  to classify whether the prior downstream stack fault remains.  See
  `etc/research/t95-s7-demread-register-contract-001.md` and
  `etc/research/t95-s7-command-bootstrap-and-user-frame-source-audit-001.md`.
- **S7 r29 DEM read contract observation admission (2026-08-11).**  Admit one
  fresh root at the absent
  `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r29`, then one MSVC
  x86 `nmake /f ntdos64-s7-runtime-trace.mak ntdos64-s7-runtime-trace.exe`
  invocation, and only if it links one 15-second observation at the absent
  `artifacts/analysis/t95-s7-runtime-trace-025-20260811-001`.  The only
  behavioral delta is the source-required AX/CF-only result mask for the
  already-reached `50:16/50:42` read family.  The observation uses the same
  contained profile, source-built NTIO/NTDOS/COMMAND inputs, ROMs and default
  Bochs machine profile.  Stop on the first generator, build or observation
  failure; preserve every root.  No selector expansion, Bochs semantic/device
  change, host capability/CLI addition, CPU-loop diagnostic, guest-input
  variation or retry is admitted.  It may establish only whether execution
  passes the former `50:42` boundary and what next original BOP or native
  terminal condition is observed.
- **S7 r29 build shell stop (2026-08-11).**  The fresh r29 generator succeeds,
  but its single build wrapper stops while PowerShell parses the Visual Studio
  path's `(x86)` component.  The retained output is
  `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r29/r29-build.stdout.log`;
  no `cmd.exe`, toolchain setup, nmake, compiler, archive, linker or executable
  ran.  This is a host-shell quoting stop only.  r29 is immutable and supplies
  no runtime evidence.  See `etc/research/t95-s7-r29-build-shell-stop-001.md`.
- **S7 r30 host-shell-corrected build/trace admission (2026-08-11).**  Admit
  one generator-only root at absent
  `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r30`, then one
  MSVC/x86 build with the identical r29 nmake command.  The sole delta is that
  PowerShell passes the entire `cmd /d /c` payload as one literal argument, so
  the verified `Program Files (x86)` path reaches `cmd.exe`.  If it links,
  admit the already reserved one 15-second observation at absent
  `artifacts/analysis/t95-s7-runtime-trace-025-20260811-001`; otherwise stop.
  All r29 source/profile/ROM/machine, no-retry, no-expansion and no-CPU-loop
  constraints remain unchanged.  The result may only classify the AX/CF-only
  read correction's continuation; it does not authorize a new BOP endpoint.
- **S7 r30 DEM read runtime result (2026-08-11).**  r30 builds once with only
  the inherited narrowing warning; its binary SHA-256 is
  `DB041AB2F881784DC54844FF06E8FC7D48ED0C967B143A3409A7F2F4D347FB1C`.
  The sole 15-second trace records 39 BOP observations and 10 commits.  Its
  final `50:42` EOF return now has `delta=01` and retains `BP=4E53`; the
  following close has `delta=00`, proving the AX/CF-only correction crosses
  the native Bochs resume seam.  The later `SS=00A7`, `ESP=0003` stack-prefetch
  terminal state remains, with no intervening BOP.  Thus the correction is
  source-required but not the stack-fault root cause; no BOP/device/host/CLI
  expansion is admitted.  Evidence:
  `artifacts/analysis/t95-s7-runtime-trace-025-20260811-001` and
  `etc/research/t95-s7-r30-demread-register-runtime-result-001.md`.
- **S7 post-close source path classification (2026-08-11).**  The original
  `handle.asm` `CloseOK` path transfers to `SYS_RET_OK`, which obtains the
  saved user frame through `Get_User_Stack`; original `LeaveDos` then restores
  live SS:SP from `User_SS:User_SP`, pops the user register frame and performs
  `DOIRET`.  Because r30 has no BOP between the correct close and the native
  stack failure, the open question is an ordinary NTDOS user-frame/state
  writer.  It is explicitly not a new BOP or Bochs feature gate.  The rejected
  r27/r28 CPU-loop instrumentation remains forbidden; a new observation needs
  an original same-configuration facility or independently proven closure.
  See `etc/research/t95-s7-post-close-user-stack-source-path-002.md`.
- **S7 stack-transfer diagnostic admission (2026-08-11).**  The imported
  configuration has `BX_DEBUGGER=0`, `BX_GDBSTUB=0` and
  `BX_INSTRUMENTATION=0`; the original configuration rejects debugger together
  with handler chaining.  `BX-TRACE-048` is therefore admitted as the smallest
  replacement for the rejected CPU-loop logger: default-off, real-mode-only
  log statements inside the original 16-bit `MOV SS` and `MOV SP,r/m16`
  handlers.  It may record only CS:RIP and old/new SS/SP values.  It receives
  no BOP selector, adapter/OpenNT/DOS term, guest-memory range, callback,
  result or state change.  A focused static test must prove those limits before
  any new root, and a later build must link one copied `data_xfer16` object
  before the original CPU archive rather than modifying `config.h`, the CPU
  loop, feature set or device profile.  `BX-TRACE-047` remains rejected.
- **S7 r31 stack-transfer diagnostic build/trace admission (2026-08-11).**
  The focused `BX-TRACE-048` static boundary and build-closure checks pass.
  Admit one generator-only root at absent
  `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r31`, then one MSVC
  x86 `nmake /f ntdos64-s7-runtime-trace.mak ntdos64-s7-runtime-trace.exe`.
  If it links, admit one 15-second observation at absent
  `artifacts/analysis/t95-s7-runtime-trace-026-20260811-001`.  The sole
  behavior delta is default-off-before-build `BX-TRACE-048` logging of
  original real-mode MOV SS / MOV SP memory-source transfers.  Preserve the
  r30 profile, guest/ROM inputs, Bochs feature/device configuration, BOP
  endpoints and adapter sources.  Stop on first generator/build/trace
  failure; no retry, new endpoint, stack repair, CPU-loop/logger expansion or
  host/CLI change is authorized.
- **S7 r31 stack-transfer shim stop and r32 admission (2026-08-11).**  r31
  builds and traces once, but emits no transfer record.  Its map proves the
  source object linked, while its retained build log proves the recursive CPU
  archive had already recreated same-named `data_xfer16.o` before the shim
  target was considered, so the macro-enabled command never ran.  This is a
  build-shim object-name collision, not guest-path evidence.  Preserve r31.
  Admit one fresh r32 root at absent
  `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r32`, identical
  sources/profile/features, but compile the copied file as uniquely named
  `cpu/data_xfer16_trace.obj` and link it before `cpu/libcpu.a`.  One MSVC/x86
  build and, only if linked, one 15-second trace at absent
  `artifacts/analysis/t95-s7-runtime-trace-027-20260811-001` are admitted.
  No source behavior, CPU loop, configuration, device, BOP, adapter, host or
  CLI delta is authorized beyond making the already registered default-off
  diagnostic object actually link.
- **S7 r32 generator preflight stop (2026-08-11).**  r32 stopped before copy,
  compiler, linker or executable action because the generator mistakenly
  required its new output `cpu/data_xfer16_trace.obj` from immutable r4.
  Correct only that input list to require the existing source
  `cpu/data_xfer16.cc`; r32 remains absent.  A later fresh r33 root is required
  before the already admitted unique-object build/trace can proceed.
- **S7 r33 corrected-generator build/trace admission (2026-08-11).**  The
  repaired generator now verifies only existing r4 inputs and has passing
  unique-object closure/static-boundary checks.  Admit exactly one generator
  root at absent `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r33`,
  one MSVC/x86 `nmake /f ntdos64-s7-runtime-trace.mak
  ntdos64-s7-runtime-trace.exe`, and only if it links one 15-second trace at
  absent `artifacts/analysis/t95-s7-runtime-trace-027-20260811-001`.  It has
  the identical r31 source/profile/feature closure; the sole correction is
  unique trace-object naming.  Stop on first failure.  No new behavior,
  endpoint, guest input, CPU loop, Bochs configuration/device, adapter, host
  or CLI change is admitted.
- **S7 r33 generator stale-object stop (2026-08-11).**  r33 copied its
  prospective root but stopped before compilation because the generic stale
  cleanup unconditionally removed the intentionally absent new trace object.
  Preserve that partial root; no build or trace occurred.  Correct only the
  cleanup to remove a stale object when it exists.  A fresh r34 admission is
  required before any further generator/build/trace action.
- **S7 r34 stale-cleanup-corrected build/trace admission (2026-08-11).**
  Admit one generator root at absent
  `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r34`, one identical
  MSVC/x86 build, and only if linked one 15-second trace at absent
  `artifacts/analysis/t95-s7-runtime-trace-027-20260811-001`.  The sole delta
  is conditional deletion of an actually stale trace object; source closure,
  unique-object output, profile, guest/ROM inputs, Bochs feature/device
  selection and all BOP/adapter behavior are unchanged.  Stop on first
  failure; no retry or additional capability is admitted.
- **S7 r34 stack-transfer runtime result (2026-08-11).**  r34 links once and
  trace `027` proves the unique diagnostic object is active while retaining
  r30's 39 BOP observations and 10 commits.  After the correct close, its last
  relevant source-generic record is `MOV SS` at `0032:260A`, `95AB -> 00A7`;
  no later `MOV SP,r/m16` is observed before the unchanged `SS=00A7`,
  `SP=0003` stack-prefetch terminal state.  The remaining issue is therefore
  a source-owned non-MOV-SP stack/control path after that transition, not a
  BOP/read-result/host/device condition.  No further trace or diagnostic is
  admitted until this address is source-mapped.  Evidence:
  `artifacts/analysis/t95-s7-runtime-trace-027-20260811-001` and
  `etc/research/t95-s7-r34-stack-transfer-runtime-result-001.md`.
* **S7 r34 OpenNT dispatcher source correlation.** The last generic stack
  transfer `0032:260A` maps exactly to the `getdseg <ss>` immediately before
  `Redisp=260F` in `msdisp.asm`, not to `LeaveDos`: it saves the caller stack
  then switches to DOS data/`AuxStack`.  The terminal `00A7:0003` is therefore
  already the caller-stack value on dispatcher entry, after the repaired read
  and normal close resumes.  `0000:019D` makes the OpenNT low-memory
  interrupt/stub contract the next source-evidence hypothesis, not yet a
  declared missing machine component.  Evidence:
  `docs/etc/research/t95-s7-r34-opennt-dispatcher-source-correlation-001.md`.
* **S7 real-mode vector diagnostic admission (2026-08-11).** Source mapping
  now satisfies r34's precondition for one further observation.  Registered
  `BX-TRACE-049` may add only a default-off log in original `real_mode_int`
  after its existing IVT reads: vector, pre-delivery CS:IP/SS:SP and the
  already-read target CS:IP.  It adds no IVT/RAM read, vector-specific branch,
  BOP/adapter/OpenNT term or state change.  A focused boundary check and the
  existing copied `exception.o` build seam must pass before one fresh-root
  build/trace admission; no device, low-memory synthesis or BOP endpoint is
  thereby admitted.
* **S7 r35 real-mode-vector build/trace admission (2026-08-11).** Admit one
  generator root at absent `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r35`,
  one MSVC/x86 `nmake /f ntdos64-s7-runtime-trace.mak
  ntdos64-s7-runtime-trace.exe`, and only if it links one 15-second trace at
  absent `artifacts/analysis/t95-s7-runtime-trace-028-20260811-001`.  The
  sole delta from r34 is `BX-TRACE-049` in copied `exception.o`; preserve all
  source-built guest inputs, Bochs configuration/device selection, adapter and
  CLI closure.  Stop on the first failure.  No retry, vector repair,
  low-memory synthesis, endpoint, BOP, host or CLI change is admitted.
* **S7 r36 IRET-TF diagnostic build/trace admission (2026-08-11).** The r35
  result maps INT 1 to source-built NTIO `intret`, whose original operation is
  only `IRET`; `BX-TRACE-050` is registered to identify the already-popped
  TF-bearing frame without changing it.  The focused IRET-TF, prior vector,
  and unique-object closure checks pass.  Admit exactly one generator root at
  absent `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r36`, one
  MSVC/x86 `nmake /f ntdos64-s7-runtime-trace.mak
  ntdos64-s7-runtime-trace.exe`, and only if it links one 15-second trace at
  absent `artifacts/analysis/t95-s7-runtime-trace-029-20260811-001`.  The
  only build delta is a copied, uniquely named `ctrl_xfer16` object linked
  before the intact CPU archive with
  `BX_NTVDM_ENABLE_IRET_TF_DIAGNOSTIC=1`; it logs only original IRET16 popped
  words when TF is set.  Preserve all source-built guest inputs, Bochs
  configuration/device selection, adapter and CLI closure.  Stop on the first
  generator/build/trace failure.  No retry, IRET/INT1 repair, low-memory
  synthesis, endpoint, BOP, host or CLI change is admitted.
* **S7 r36 IRET-TF runtime result (2026-08-11).** The sole r36 build linked
  successfully; binary SHA-256 is
  `A5D8EC3252011EADC4A46B09C5FA820F0F27FEF70E479380EA73E766BF44CF05`.
  Its sole 15-second watchdog observation retains 39 BOP observations and 10
  committed transactions.  The first logged TF-bearing return is from NTIO
  `0070:01EC` to `0000:0043` with saved FLAGS `0346`.  Source-built
  `NTIO.MAP` places `intret` at `0070:018B` and `strategy` at `0070:01EC`;
  original `msbio1.asm` implements `intret` with `FIRET`, whose original
  `vint.h` definition expressly performs native `iret` when the saved frame
  has TF.  Thus the fault precedes/produces the saved TF frame; it is not
  evidence for an IRET/INT1 Bochs repair, adapter BOP change, or new machine
  device.  The unresolved next question is the original producer of the
  `00A7:08F5` frame targeting `0000:0043`.  Evidence:
  `artifacts/analysis/t95-s7-runtime-trace-029-20260811-001` and
  `docs/etc/research/t95-s7-r36-iret-tf-runtime-result-001.md`.
* **S7 r36 INT10 pseudo-ROM source correlation (2026-08-11).** The first TF
  frame is immediately preceded by original guest INT29 `outchr` and INT10:
  `msinit.asm` copies the original `SpcKbd` resident code, `InstSpcKbd` saves
  the prior machine INT10 vector in `host_int10` and replaces IVT 10h with
  guest `int10h_vector`.  Its default `FULLSCREEN` branch far-jumps through
  that saved pointer.  This is guest pseudo-ROM composition, not adapter
  filesystem/BOP behavior or a reason to modify Bochs INT1/IRET.  Existing
  evidence does not expose the saved far-jump target; a later observation may
  only log the already-decoded real-mode far-jump target, without touching
  CPU/IVT/ROM semantics.  Evidence:
  `docs/etc/research/t95-s7-r36-int10-pseudorom-source-correlation-001.md`.
* **S7 r37 real-mode far-jump build/trace admission (2026-08-11).** Source
  correlation identifies the one unresolved mechanical fact: the target saved
  by original `InstSpcKbd` in its `host_int10` far-jump field.  Registered
  `BX-TRACE-051` adds only a default-off real-mode `JMP_Ap` target log after
  the original operand decode and before the original CS:EIP assignment.
  Focused far-jump, prior IRET/vector, unique-object closure and governance
  checks pass.  Admit exactly one generator root at absent
  `artifacts/build/bochs-2.6-native-s7-runtime-trace-msvc-r37`, one MSVC/x86
  `nmake /f ntdos64-s7-runtime-trace.mak ntdos64-s7-runtime-trace.exe`, and
  only if it links one 15-second trace at absent
  `artifacts/analysis/t95-s7-runtime-trace-030-20260811-001`.  The only delta
  is copied unique `ctrl_xfer32_trace.obj` linked before the intact CPU archive
  with `BX_NTVDM_ENABLE_REAL_MODE_FAR_JUMP_DIAGNOSTIC=1`.  Stop on first
  generator/build/trace failure.  No retry, target repair, vector/ROM
  synthesis, BOP/adapter/host/CLI behavior or device change is admitted.
* **S7 r37 real-mode far-jump runtime result (2026-08-11).** The sole build
  links (SHA-256 `A18BD47CBEBAE2AB72267E99B85F4DC0C45B1D9928AABDFE5A64285931F53A16`)
  and the sole 15-second trace retains 39 BOP observations and 10 committed
  transactions.  Its decisive original far jump is `073B:0939 -> 0000:0000`.
  Original `InstSpcKbd` saves IVT 10h to `host_int10` before installing its
  own guest pseudo-ROM vector, so this proves the pre-NTIO machine INT10 IVT
  entry is zero.  The later TF loop is consequently downstream of a missing
  Bochs-owned firmware/reset precondition—not an IRET/INT1, BOP, DEM,
  filesystem or adapter failure.  Do not synthesize this vector in adapter or
  guest code.  The next evidence-only task is native Bochs firmware/reset IVT
  lifecycle classification.  Evidence:
  `artifacts/analysis/t95-s7-runtime-trace-030-20260811-001` and
  `docs/etc/research/t95-s7-r37-far-jump-runtime-result-001.md`.
* **S7 r37 native firmware/IVT lifecycle audit (2026-08-11).** Local Bochs
  source proves that the default-off execution plan runs after machine
  initialization but before the first CPU loop, and its entry helper replaces
  CS:RIP directly with NTIO.  `load_ROM` maps ROM bytes but does not execute
  POST.  The selected Bochs ROM BIOS names `F000:FFF0` as its power-up entry;
  POST's `post_init_ivt` installs nonzero default IVT entries and then invokes
  the VGA option-ROM scan, whose INT10 implementation is the native video
  owner.  The observed zero vector is therefore the expected consequence of
  bypassing the reset-vector firmware path, not a BOP/DEM/adapter/IRET defect.
  The next work is a design decision plus evidence for one native
  firmware-to-NTIO handoff condition.  No IVT synthesis, device expansion,
  guest change, or new trace is admitted by this audit.  Evidence:
  `docs/etc/research/t95-s7-r37-bochs-firmware-ivt-lifecycle-audit-001.md`.
* **S7 native firmware-to-NTIO handoff design (2026-08-11).** The OpenNT
  caller audit corrects the earlier over-broad interpretation: historical
  NTVDM itself calls SoftPC `reset()` and then directly assigns NTIO's entry;
  it does not boot DOS from a disk.  The missing condition is therefore the
  native reset state, not a generic BIOS boot.  A local source audit identifies
  existing Bochs optional-ROM configuration and scan support as the smallest
  native boundary: unmodified Bochs POST reaches a one-purpose neutral marker
  before `INT 19h`, then a new but generic same-#UD-seam plan consumer may
  take an adapter-owned, opaque single-use startup plan.  The current pre-loop
  plan and low-GPR/CF exception result are explicitly insufficient for that
  handoff and are not misrepresented as a solution.  This avoids synthetic
  adapter IVT/BDA state and does not introduce a guest-visible boot disk.
  It remains a design only; a separately admitted packet must define the ROM
  envelope, deferred-plan ABI, precise rejection behavior and a no-media
  runtime proof.  Evidence and rejected alternatives:
  `docs/etc/research/t95-s7-native-firmware-to-ntio-handoff-design-001.md`.
* **S7 deferred startup-plan mechanics admission (2026-08-11).** The ABI
  audit proves that the current pre-loop plan consumer and the current
  exception-result ABI cannot perform a native-POST handoff: the former
  changes CS:RIP too early, while the latter carries only low GPR/CF/RIP
  deltas.  `BX-ABI-052` is registered before code for the sole permitted
  extension: a default-off, one-shot, opaque startup-plan consumer at the
  existing generic `#UD` seam.  It may reuse only current plan preflight,
  ordinary-RAM capture/copy/restore and real-mode CS:RIP mechanics; Bochs must
  remain ignorant of the optional-ROM marker, firmware, address, BOP and
  OpenNT meaning.  Adapter session installation before the CPU loop is
  explicitly non-mutating; only its selected marker may take the plan.  No
  build/run, option-ROM asset, device change, BOP result, host capability or
  CLI behavior is admitted until focused adapter and Bochs boundary tests
  prove these negative cases.
* **S7 deferred startup-plan adapter-contract admission (2026-08-11).** The
  first implementation increment is adapter/profile-only.  It admits one
  optional, closed `machine_startup_handoff` profile object with exact v1
  values `vector=6`, bytes `0f0b`, `CS=C800`, and `IP=0003`, alongside the
  existing enabled `ntio-v0` startup plan.  The adapter may install that
  selected session before a CPU loop and expose one `take` operation only when
  a copied valid real-mode exception event, CPU state and instruction window
  all match; a mismatch or second take is rejected without guest-memory
  access.  The returned data is the existing opaque startup-plan ABI.  Admit
  exactly one fresh modern C11 root at absent
  `artifacts/build/t95-s7-deferred-startup-plan-c11-001` to configure, build
  and execute only `bx-ntvdm-adapter-runtime-test`.  No Bochs source/build,
  option-ROM asset, guest execution, BOP endpoint, firmware/device change,
  host capability or CLI behavior is admitted.  Preserve the root at its first
  failure.
* **S7 deferred startup-plan Bochs-mechanics admission (2026-08-11).** Under
  the already registered `BX-ABI-052` exception, add only the default-off
  generic consumer and pre-loop session preparation. The consumer receives
  the existing copied `#UD` record, asks the adapter for an opaque one-shot
  plan, preflights it, uses existing ordinary-RAM capture/copy/restore and the
  existing real-mode entry setter, then leaves the CPU loop through the normal
  checked resume. Its Bochs block must not name the rendezvous, profile,
  firmware, BOP, OpenNT, DOS, device or host semantics. A focused static
  boundary test and the existing adapter C11 root are required before any
  Bochs object/link/run admission. No ROM asset or new Bochs build is
  authorized by this code/static-test increment.
* **S7 deferred startup-plan adapter/static result (2026-08-11).** Fresh
  `t95-s7-deferred-startup-plan-c11-001` configured, built 50 C11 objects and
  ran `bx-ntvdm-adapter-runtime-test` successfully. It covers profile
  acceptance, a wrong-CS refusal, exact one-shot take, second-take refusal and
  retained direct-plan compatibility. The default-off Bochs consumer/pre-loop
  preparation and its focused static boundary test also pass, as do the
  generic `#UD`, BOP-listener and documentation-governance checks. This is
  not a Bochs compilation or runtime claim: the next separately admitted work
  must establish the copied object/link closure, then create a neutral ROM
  asset and a native-POST runtime observation.
* **S7 deferred startup-plan build-root admission (2026-08-11).** The
  existing fresh-root generator now has an explicit
  `-DeferredStartupPlan` mode: it disables the old pre-loop direct plan and
  enables only the new default-off consumer in `main.o` and `exception.o`,
  while retaining the already audited adapter/CLI source closure. Its focused
  static closure test passes and rejects composition/profile input from the
  generator. Admit one generator-only root at absent
  `artifacts/build/bochs-2.6-native-s7-native-post-handoff-msvc-r38`, followed
  by exactly one MSVC/x86 `nmake /f ntdos64-s7-runtime-trace.mak
  ntdos64-s7-runtime-trace.exe` if generation succeeds. No option ROM, guest
  run, device/firmware change, BOP endpoint, host capability or CLI behavior
  is admitted until that copied object/link result is retained.
* **S7 r38 deferred startup-plan build result (2026-08-11).** The sole fresh
  root generated with `EXECUTION_PLAN=0` and `DEFERRED_STARTUP_PLAN=1`; its
  one MSVC/x86 `nmake` invocation compiled the copied `main.cc` and
  `exception.cc` opt-ins, the retained adapter/CLI closure and linked
  `ntdos64-s7-runtime-trace.exe` successfully. Binary SHA-256:
  `2492EF04B6B4F7AB7790248A8BC619A6548DDFD61724E170AE0F49972D5F7C07`;
  map SHA-256:
  `33210B4E426051599FFDBD7EFC5FD8469422643C211CBAF60134FCDEB090F0B4`.
  The known `exception.cc` narrowing warning remains the only compiler
  warning. This is a build-closure result only: no option ROM was supplied,
  no executable was run and no POST/NTIO behavior is claimed. The next
  admission is a composition-only ROM envelope and profile/bochsrc input
  test, before a separately bounded runtime observation.
* **S7 native-POST handoff composition/run admission (2026-08-11).** The
  bounded observation runner may, only under `-DeferredStartupPlan`, generate
  an adapter-selected 512-byte composition artifact whose bytes are fixed to
  `55 AA 01 0F 0B F4`, zero padding and final checksum `F2` (SHA-256
  `E4CA4AE5190A15ED741D1DFED5A58533A2B6926A40302D839FB714F9FC673FFB`).
  It adds the original Bochs `optromimage1` declaration at `0xC8000` and the
  closed profile handoff declaration; it adds no guest media, host storage,
  device, BIOS service, BOP or payload. The focused composition-input test
  passes. Admit exactly one 15-second observation at absent
  `artifacts/analysis/t95-s7-native-post-handoff-031-20260811-001`, using r38
  and this switch. Preserve the root on any failure. The result may establish
  only native POST reaching the single handoff and the resulting NTIO entry;
  it does not admit a new BOP endpoint, host capability, CLI feature or device.
* **S7 r38 native-POST composition stop and r39 correction (2026-08-11).**
  The sole r38 observation stops before CPU execution with native Bochs panic
  `ROM: address space 0xc8000 already in use`; it records zero BOP observations
  and zero commits. Source plus retained VGA image size prove that the 41,472
  byte VGA ROM owns 2 KiB pages through `CA7FF`, while ROM BIOS scans the next
  aligned general-ROM page. Correct the closed profile/composition rendezvous
  only to `CA80:0003` / `CA8000`; preserve the same 512-byte ROM bytes and all
  owners. Admit one fresh C11 contract root at absent
  `artifacts/build/t95-s7-deferred-startup-plan-ca80-c11-001`, then one fresh
  r39 generator root at absent
  `artifacts/build/bochs-2.6-native-s7-native-post-handoff-msvc-r39`, one
  MSVC/x86 build, and only if linked one 15-second observation at absent
  `artifacts/analysis/t95-s7-native-post-handoff-032-20260811-001`. No retry
  of r38, device/firmware expansion, BOP endpoint, host capability or CLI
  behavior is authorized.
* **S7 r39 native-POST reachability result (2026-08-11).** The corrected
  `CA8000` ROM maps and original POST reaches `CA80:0003`; the native vector
  diagnostic records the expected `#UD` and original BIOS INT6 target. The
  consumer then passed through because source review found it accidentally
  nested inside the disabled legacy `STARTUP_TRANSACTION` preprocessor block.
  This is a bounded placement defect in the new generic consumer, not a
  firmware, ROM, BOP, adapter-plan or NTIO defect. Move that block outside the
  legacy guard and strengthen its static test to reject such nesting; both
  checks pass. r39 is retained and not rerun. A fresh r40 build/observation
  requires separate admission; no additional capability is implied.
* **S7 r40 deferred-consumer placement admission (2026-08-11).** Admit one
  fresh generator root at absent
  `artifacts/build/bochs-2.6-native-s7-native-post-handoff-msvc-r40`, then one
  MSVC/x86 `nmake` link, and only if linked one 15-second observation at absent
  `artifacts/analysis/t95-s7-native-post-handoff-033-20260811-001`. The sole
  source delta from r39 is the already statically verified relocation of the
  default-off consumer outside the disabled legacy guard. Preserve the same
  CA800 ROM, profile, native reset floor, BOP listener and adapter closure.
  Stop on first failure; no retry or additional device/BOP/host/CLI behavior
  is admitted.
* **S7 r40 compile stop and r41 admission (2026-08-11).** r40 stops at the
  copied `exception.cc` compile because this Bochs configuration rejects an
  explicit `this` expression at the consumer callsite. Correct only that call
  to the already single-CPU `BX_CPU(0)` accessor and update the static boundary
  assertion. Admit one fresh r41 generator root at absent
  `artifacts/build/bochs-2.6-native-s7-native-post-handoff-msvc-r41`, one
  MSVC/x86 build, and only if linked one 15-second observation at absent
  `artifacts/analysis/t95-s7-native-post-handoff-034-20260811-001`. Preserve
  all other r40 inputs and prohibitions.
* **S7 r41 native POST-to-guest result (2026-08-11).** The sole admitted r41
  build linked with the default-off generic deferred consumer enabled. Its
  sole 15-second observation completed without watchdog termination and
  records native POST reaching the option-ROM rendezvous, then real guest
  execution: 31 observed `C4 C4` calls and seven bounded adapter transaction
  commits. The first is OpenNT `BOP_DOS:0x11` (`SVC_DEMLOADDOS`); the trace
  later reaches `BOP_DOS:0x36` (`SVC_DEMENTRYDOSAPP`). The runner passes
  native `-benchmark 1`; its one-shot benchmark timer therefore sets
  `kill_bochs_request` at one million simulated ticks, and the text UI maps
  the normal CPU-loop return to exit code 1. This is a deliberate bounded
  observation ending, not a guest/BOP failure. It remains a reachability result, not a claim
  that all invoked DOS services have been implemented. The listener observes
  defined and uncatalogued selectors without dispatching either, while the
  existing bounded transactions retain their own evidence. Preserve r41 and
  classify the observation boundary from source before admitting another runtime iteration;
  do not add a per-BOP handler, device, host capability or CLI behavior.
* **S7 r42 extended BOP-observation admission (2026-08-11).** Source review
  proves r41's exit is the original `-benchmark 1` timer, not a guest stop.
  Admit only a runner parameter that records an otherwise unchanged native
  benchmark duration (default remains one million ticks), plus one fresh
  observation at absent
  `artifacts/analysis/t95-s7-native-post-handoff-035-20260811-001` using the
  existing r41 binary, `-BenchmarkMillions 3` and `-WatchdogSeconds 30`.
  Retain identical ROMs, option ROM, BYOB input identities, profile and
  adapter environment. The only purpose is discovery/classification of newly
  reached `C4 C4` calls. Stop on watchdog or any unexpected non-benchmark
  termination; do not build, alter the guest, add BOP dispatch, enable a
  device, host capability or CLI behavior.
* **S6 r42 result and r43 catalogue-correction admission (2026-08-11).** The
  sole r42 run timed out at its admitted 30-second watchdog (6,901 BOP
  observations, ten existing commits). Its first new edge is
  `BOP_CMD:01` (`SVC_CMDGETNEXTCMD`) at `95AB:03C1`; because no command
  dispatcher exists, native `#UD` follows the guest INT6 vector to
  `073B:0740`, containing `C4 C4 06`. That selector repeats while the real
  mode exception frame consumes stack. OpenNT's complete SoftPC `BIOS[]`
  source table defines selector `06` as `illegal_op_int`; it is not an
  unclassified BOP and must not be implemented as an adapter handler. Admit
  only a source-catalogue correction: make all 256 explicit `BIOS[]` table
  selector values provenance-visible, retain named OpenNT families as
  overlapping provenance, update catalogue/static tests and the evidence
  record. Admit one fresh C11 test root at absent
  `artifacts/build/t95-s6-bop-catalog-r43-c11-001`, build only
  `bx-ntvdm-bop-catalog-v1-test`, and run that test once. No runtime retry,
  guest/Bochs behavior change, BOP dispatch, hardware, host capability or CLI
  change is admitted.
* **S6 r43 interim catalogue result (2026-08-11).** The static source tests now prove
  16 OpenNT top-level, 41 named BIOS and all 256 historical `BIOS[]` selector
  values are classified; the listener boundary and documentation governance
  checks pass. The first PATH lookup did not find CMake and therefore did not
  create a root; subsequent exact-path reconnaissance found the admitted
  `C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe`
  and `C:\msys64\ucrt64\bin\gcc.exe`. Retain the initial failed lookup as
  procedure evidence, correct the limitation, and run the already admitted
  fresh C11 fixture only with those installed tools. The fixture then
  configured with CMake 4.1.0 / Ninja and GCC 16.1.0, built its seven-object
  closed source set, and printed `identity-only pass-through classification
  verified`. r42's source-led
  conclusion stands: `54:01` is the first genuinely required host-facing
  capability; selector `06` is only its historical SoftPC invalid-op path.
* **S7 BOP 54:01 command-delivery contract-audit admission (2026-08-11).**
  The r42 reached trace changes the S7 sequence, not the Bochs boundary:
  `BOP_CMD:01` (`SVC_CMDGETNEXTCMD`) is the first mandatory CLI-to-guest
  delivery edge and precedes further file activity. Admit one source-only
  audit of OpenNT `CMDSVC`/`CMDINFO`, `cmdGetNextCmd`, the `MS_bop_4` calling
  convention, and the immediate COMMAND.COM continuation. It must define the
  copied request fields, bounded response writes, first/repeat/exhaustion
  states, carry/AX outcomes, and which historical PIF/console/environment
  branches are rejected or replaced by CLI policy. No BOP dispatch, CPU
  result, guest-memory write, profile/CLI parser, Bochs/OpenNT change, build
  or runtime trace is admitted by this audit.
* **S7 command-envelope v1 admission (2026-08-11).** Admit one adapter-only
  precursor to `54:01`: a versioned, fixed-size session-owned command envelope
  derived solely from the already validated contained `COMMAND.COM` namespace
  entry. It may contain an ASCII DOS guest path, explicit `COM` extension
  class, drive index, an empty initial tail and a one-shot delivery state. It
  must reject noncanonical/non-ASCII/overlong namespace input and must expose
  no host path, file handle, pointer, BOP/CPU record, guest address or RAM
  write. Add a focused C11 fixture for initialization, valid derivation,
  one-shot take, second-take exhaustion and malformed input; build/run it once
  in fresh root `artifacts/build/t95-s7-cmd-launch-envelope-c11-001`. No profile/CLI argument change, runtime installation,
  BOP dispatch, Bochs/OpenNT change or trace is admitted.
* **S7 command-envelope fixture correction (2026-08-11).** The admitted root
  configured and linked the focused C11 fixture with GCC 16.1.0, but its first
  run exited `1` before printing its success banner. Inspection found a test
  assertion error only: it expected 12 bytes for the ASCII path
  `C:\\COMMAND.COM`, whose correct byte count is 14. Correct that literal
  assertion and rerun only this already-built target in the same admitted
  root. This is not a Bochs build/retry, runtime trace, or scope expansion.
* **S7 command-envelope v1 result (2026-08-11).** The final focused C11
  fixture rebuilt its three-object source set and passed, including first
  derivation, one-shot exhaustion, noncanonical, non-ASCII and non-terminated
  path rejection. Evidence, exact inputs and the first fixture-only failure
  are retained in
  `docs/etc/research/t95-s7-command-envelope-v1-evidence-001.md`. The value
  remains unconnected: no BOP dispatch, CPU result, guest-RAM write, Bochs,
  OpenNT or CLI parser behavior changed. The next S7 gate is a source-led
  copied `CMDINFO` layout and bounded response-plan audit; it must retain the
  no-host-path and generic-result boundary before any `54:01` routing is
  admitted.
* **S7 CMDINFO copied-layout admission (2026-08-11).** Admit a source-led,
  adapter-only C11 codec for the complete 36-byte OpenNT `CMDINFO` v1 record,
  copied from a future checked guest read. The codec may decode fixed little-
  endian fields and classify the exact `CMDGETNEXTCMD` response capacities:
  environment byte capacity, 128-byte UCOMBUF command capacity and 273-byte
  executable-path capacity. It must preserve every record field in its copied
  value, reject a wrong record length, and expose no guest pointer, host
  pointer, CPU record, BOP event, guest address, RAM write or dispatch. Add
  a focused fixture for field offsets, extension values, ordinary capacity,
  environment retry and undersized command/path rejection. Build/run it once
  in fresh root `artifacts/build/t95-s7-cmdinfo-layout-c11-001`. This admission
  does not connect the command envelope, infer a host target mapping, enable
  BOP routing, modify Bochs/OpenNT, or run the guest.
* **S7 CMDINFO fixture correction (2026-08-11).** The admitted root configured
  and linked the three-object fixture with GCC 16.1.0, but its first run exited
  `1` before its banner. Inspection found a fixture-only inverted assertion:
  the codec correctly returns zero when required command or executable bytes
  exceed the source-derived 128/273 maxima, but the fixture treated that
  rejection as failure. Correct only that assertion and rerun this target in
  the same admitted root. No runtime, Bochs or BOP work is admitted.
* **S7 CMDINFO copied-layout result (2026-08-11).** The corrected focused C11
  fixture passed from the admitted root. It proves the 36-byte copied layout,
  all offset decoding, the historical `COM=8` extension value, source-shaped
  environment retry precedence and 128/273 capacity bounds. Full source,
  procedure and first-fixture-failure evidence is retained in
  `docs/etc/research/t95-s7-cmdinfo-copied-layout-evidence-001.md`. It has no
  runtime installation, BOP routing, CPU result, guest-RAM write, Bochs,
  OpenNT or CLI behavior. The next S7 decision is whether existing generic
  gather/result mechanics can carry this copied record without a special
  memory path.
* **S7 `54:01` generic-transport result (2026-08-11).** Existing adapter
  mechanics close the transport question: one already-checked generic gather
  can copy the 36-byte `CMDINFO`, and the existing generic multi-write/result
  transaction can later carry bounded ordinary-RAM output and a fixed CPU
  delta. No BOP-specific memory bridge or Bochs change is needed. The audit is
  recorded in `docs/etc/research/t95-s7-bop-54-01-generic-transport-audit-001.md`.
  Do **not** route `54:01` yet: the present envelope names bootstrap
  `COMMAND.COM`, not the user-selected CLI target, so delivering it would be
  semantically wrong. The next S7 gate is a CLI-owned contained-target
  materialization contract (identity, canonical guest path, normalized tail,
  absence policy and namespace entry); only that coherent capability may
  authorize the generic `54:01` service composition.
* **S7 contained CLI-target contract decision (2026-08-11).** The target
  bridge must not pass a host pathname, inherited handle or GCC object into
  the MSVC Bochs island. Select a profile-pinned target instead: a v3 profile
  will declare one separately identity-checked `TARGET.COM` or `TARGET.EXE`
  component and matching canonical guest placement on the command drive; the
  runner accepts a CLI target only when it is that selected resource, and the
  adapter re-verifies/loads it with its existing BYOB mechanism. The immutable
  normalized tail is bounded to fit OpenNT's 128-byte UCOMBUF. Full boundary,
  rejection and non-goal rationale is in
  `docs/etc/research/t95-s7-cli-contained-target-contract-001.md`. Admit next
  only the v3 profile/parser/selection and runner target-identity fixture;
  do not yet alter the namespace, runtime, BOP routing, Bochs or guest.
* **S7 v3 profile-selection implementation admission (2026-08-11).** Implement
  only the profile half of the contained-target contract: exact
  `ntdos64-byob-profile-v3`/`nt4-en-us-command-smoke-v3` recognition, one
  required and identity-verified `target` component restricted to
  `TARGET.COM` or `TARGET.EXE`, and a `guest_target_placement` that exactly
  names the matching canonical DOS path on the command drive. Expose copied
  target descriptor and placement through `byob_profile_selection`; preserve
  v1/v2 behavior. Extend the existing BYOB profile fixture with v3 acceptance
  and invalid-placement rejection, and build/run only it in fresh root
  `artifacts/build/t95-s7-profile-v3-target-c11-001`. No runner handoff,
  namespace/runtime/BOP/Bochs/OpenNT change or guest execution is admitted.
* **S7 runner target-identity guard admission (2026-08-11).** Admit one
  CLI-only helper that compares the normalized user target path with the
  normalized `BYOB root + selected v3 target file name`. It must accept only
  the exact profile-pinned resource and reject another file or a mismatched
  descriptor. It is a path-selection guard only: byte identity remains the
  receiving adapter's `byob_image_load_exact` responsibility. Add a focused
  C11 fixture and build/run it once in fresh root
  `artifacts/build/t95-s7-target-identity-c11-001`. Do not yet call this
  helper from the runner, pass a path to adapter/Bochs, open target bytes,
  change namespace/runtime/BOP, or run a guest.
* **S7 v3 target profile and identity-guard result (2026-08-11).** Both
  admitted GCC fixtures passed: `byob-profile-test` covers v1/v2 preservation,
  v3 target identity/placement selection and invalid placement rejection;
  `byob-target-selection-test` accepts only the normalized selected target
  path. Evidence and exact roots are in
  `docs/etc/research/t95-s7-profile-v3-target-evidence-001.md`. The runner
  deliberately still does **not** call the guard, because it presently passes
  raw target argv after engine `--`; wiring only the guard would not remove
  that path from the engine boundary. The next cohesive S7 gate is a copied,
  path-free launch declaration (target kind plus bounded normalized DOS tail)
  carried in the child-only environment and required by adapter installation;
  it will then replace the old raw target handoff as one change.
* **S7 path-free launch-declaration admission (2026-08-11).** Admit one
  fixed copied CLI value containing only `TARGET.COM`/`TARGET.EXE` kind and a
  reconstructed ASCII DOS tail of at most 126 bytes. It must round-trip as
  two strict child-environment values (`com`/`exe` and tail), reject unknown
  kind, non-ASCII/control/`=` tail text and overflow, and contain no host
  path, handle, pointer, guest address, BOP or CPU field. Add its focused C11
  fixture and build/run it once in fresh root
  `artifacts/build/t95-s7-launch-declaration-c11-001`. No runner or adapter
  installation, engine argv, namespace, runtime/BOP/Bochs/OpenNT or guest
  change is admitted by this value fixture.
* **S7 path-free runner handoff admission (2026-08-11).** Compose the already
  tested v3 selection guard and launch declaration in the production runner.
  For a DOS engine invocation, require the profile-pinned target, derive
  `com`/`exe` plus tail before creating the child, pass those two values only
  in the child-only adapter environment, and invoke the engine with exactly
  `--byob-profile`, `--byob-root`, and a terminal `--`--no raw target path or
  tail argv. Update the test-only engine probe and runner policy fixture to
  prove the six-argv handoff, profile/root preservation, drive policy, launch
  kind/tail, and target absence from argv. Build/run only runner, probe and
  policy in fresh root `artifacts/build/t95-s7-path-free-runner-c11-001`.
  Do not yet have adapter installation consume the values, add target bytes to
  namespace, route a BOP, change Bochs/OpenNT or execute a guest.
* **S7 path-free runner handoff result (2026-08-11).** The fresh runner/probe
  build and policy fixture passed. The probe proved the engine receives only
  six argv elements ending in `--`, while profile/root, frozen drive policy,
  `com` and `/c smoke` arrive through child-only environment values; the host
  target path and tail argv are absent. Exact procedure and negative-case
  output are recorded in
  `docs/etc/research/t95-s7-path-free-runner-handoff-evidence-001.md`. No
  adapter consumption, target byte loading/namespace entry, runtime BOP route,
  Bochs/OpenNT change or guest execution occurred. Next admit only adapter
  installation's one-time copied declaration validation for v3; it must not
  materialize the target or create a service handler.
* **S7 adapter launch-declaration installation admission (2026-08-11).** For
  a selected v3 target profile only, adapter installation may read the two
  child-environment launch values once, parse them into its private fixed
  copied declaration, and require their target kind to match the profile's
  `TARGET.COM`/`TARGET.EXE`. Missing/malformed/mismatched values must reject
  installation before it becomes live; an empty tail remains valid. Extend
  the existing runtime fixture with v3 missing-declaration rejection and valid
  `com`/`/c smoke` acceptance, build/run only that fixture in fresh root
  `artifacts/build/t95-s7-adapter-launch-install-c11-001`. No target byte load,
  namespace change, BOP service, guest-memory operation, Bochs/OpenNT change
  or guest run is admitted.
* **S7 adapter launch-declaration installation result (2026-08-11).** The
  admitted 51-object GCC runtime fixture passed. It proves a v3 profile fails
  installation when launch values are absent, accepts matching `com` plus
  `/c smoke`, and retains all existing identity-to-transaction checks. The
  adapter copies the declaration only into private state; no target image
  load, namespace entry, BOP/guest-memory/Bochs/OpenNT action or guest run
  occurred. Procedure and Win32 empty-tail handling are retained in
  `docs/etc/research/t95-s7-adapter-launch-install-evidence-001.md`. Next
  admit only the coherent target image re-verification/load plus one canonical
  read-only namespace entry and its file-lifecycle fixtures, before any
  `54:01` response implementation.
* **S7 target namespace-materialization admission (2026-08-11).** Admit one
  coherent adapter capability after successful v3 launch installation: re-load
  the selected target through `byob_image_load_exact`, retain its immutable
  bytes only in adapter-private session state, and append exactly one
  `\\TARGET.COM` or `\\TARGET.EXE` entry to the existing closed read-only
  namespace. Keep the existing three boot entries intact; reject bad target
  image/placement, duplicate/wrong path, wrong drive, or append while open.
  Extend focused namespace lifecycle and runtime-install fixtures; build/run
  them in fresh root `artifacts/build/t95-s7-target-namespace-c11-001`. No
  arbitrary path/directory exposure, host handle, guest-memory operation,
  BOP service, Bochs/OpenNT change or guest run is admitted.
* **S7 target namespace-materialization result (2026-08-11).** The focused
  namespace and runtime-install fixtures passed in fresh GCC root
  `artifacts/build/t95-s7-target-namespace-c11-001`. Once the v3 launch kind
  has matched the selected target, the adapter re-verifies and privately
  retains target bytes, then adds exactly one same-drive canonical
  `\\TARGET.COM`/`\\TARGET.EXE` entry to the closed read-only namespace. The
  lifecycle fixture proves append rejects an open namespace, and proves the
  target's two test bytes are read back exactly after close and one append;
  duplicate append rejects. No host path/handle, guest-memory or BOP action,
  Bochs/OpenNT change, or guest run occurred. Exact environment/toolchain
  condition and procedure are retained in
  `docs/etc/research/t95-s7-target-namespace-materialization-evidence-001.md`.
  Next admit only the source-led `BOP_CMD:01` response contract for the
  already reached `SVC_CMDGETNEXTCMD` path; it must identify request/response
  CPU and guest-memory effects before any handler is written.
* **S7 `54:01` source response-contract admission (2026-08-11).** Source
  audit closes the first contained-command transaction: after the existing
  `CMDSETINFO` registration, gather the 36-byte `CMDINFO` at DS:DX and only
  accept the canonical COMMAND buffers. Stage one generic multi-write plan
  for UCOMBUF, EXECPATH, the precise response fields, SCS_ToSync and the
  registered DOS-binary byte; resume after four BOP bytes with CF clear and
  consume the immutable selected target only after queue acceptance. The
  existing generic 126-byte CLI tail is narrowed to 118 bytes because the
  OpenNT UCOMBUF count must contain six target-basename bytes, optional space,
  tail and CR/LF within 127 bytes. Missing registration, malformed/undersized
  request, no target or a second call must emit no writes and not consume it.
  Add focused codec/service fixtures and integrate only through existing
  v4 gather/multi-write mechanics in a fresh C11 root. Do not modify Bochs,
  dispatch any other BOP, expose host paths/handles, implement environment or
  console policy, or run a native guest in this admission.
* **S7 `54:01` bounded-service result (2026-08-11).** The reached first
  command service is now adapter-owned and wired through the existing v4
  generic gather/multi-write queue. It gathers only copied CMDINFO, requires
  the earlier CMDSETINFO opaque registrations, and stages the bounded
  UCOMBUF/EXECPATH/response/SCS/DOS-binary transaction before CF-clear resume.
  The source-derived 118-byte target-tail cap replaces the generic 126-byte
  declaration cap. Focused service and full runtime lifecycle fixtures passed
  in fresh root `artifacts/build/t95-s7-cmdgetnext-c11-001`; exact procedure,
  writes and non-goals are retained in
  `docs/etc/research/t95-s7-bop-54-01-service-evidence-001.md`. No Bochs or
  OpenNT change occurred and this is not yet a guest runtime claim. Next
  admit one watchdog-bounded native trace of the same selected target to
  verify actual 54:01 commit and classify the next stop before enabling any
  further BOP.
* **S7 r44 v3 command-delivery trace admission (2026-08-11).** Generate one
  fresh deferred-startup native root at absent
  `artifacts/build/bochs-2.6-native-s7-native-post-handoff-msvc-r44`, then
  run its declared x86 nmake target once, then observe it once with a 30-second
  watchdog in absent `artifacts/analysis/t95-s7-native-post-handoff-036-20260811-001`.
  The only closure additions are copied adapter `cmdinfo`/`cmd-get-next` and
  CLI launch-declaration sources. The observer admits only source-built
  OpenNT `QUIT.COM` (three bytes, copied as profile-pinned `TARGET.COM`), v3
  target placement, and an empty `com` launch tail. It tests 54:01's actual
  queue/commit and normal COMMAND open/EXEC/exit continuation. Stop after the
  first generator, build or process failure; no further BOP, device, Bochs,
  OpenNT, host-path, console, environment or target change is admitted.
* **S7 r44 trace 036 pre-guest stop and trace 037 correction admission
  (2026-08-11).** The sole 036 process ran r44 and reset Bochs, but recorded
  zero BOPs/commits because `cmd.exe` treats `set NAME=` as deletion; strict
  v3 installation therefore correctly rejected the absent launch-tail value.
  Preserve 036. The observer's one correction is to supply `QUIT.COM`'s
  nonempty ignored `/q` tail, then run exactly one process in absent
  `artifacts/analysis/t95-s7-native-post-handoff-037-20260811-001` with the
  same r44 binary/profile/ROMs/watchdog. This does not relax v3 missing-value
  rejection. A separate CLI transport packet remains required before an empty
  tail is claimed usable.
* **S7 r44 trace 037 result (2026-08-11).** Valid v3 native evidence proves
  the new `54:01` service rather than merely its fixture: source-built
  `QUIT.COM` as `TARGET.COM` reached `95AB:03C1`, and the adapter committed
  its 12-write, 47-byte CMDGETNEXTCMD transaction. The very next unserved BOP
  is `54:0E` at `95AB:05D9` (`SVC_GETKBDLAYOUT`); it falls through selector
  `06` and exhausts the real-mode exception stack. Therefore 54:01 resume is
  not the failure cause. r44 binary identity, 036 pre-guest empty-tail
  rejection, exact ticks and the new stop classification are retained in
  `docs/etc/research/t95-s7-r44-cmdgetnext-runtime-result-001.md`. Next admit
  only a source audit of 54:0E's fixed EN-US/no-KEYB response contract; do not
  add keyboard hardware, KEYB.COM, host locale, any other BOP or a broad
  dispatcher.
* **S7 `54:0E` fixed-layout admission (2026-08-11).** OpenNT source proves
  COMMAND branches only on DX after `SVC_GETKBDLAYOUT`; DX=0 bypasses KB16.
  Admit a focused adapter-only real-mode `C4 C4 54 0E` service that resumes
  after four bytes and sets only low DX to zero. It must not read/write guest
  memory, consume DS:SI/CX, inspect host locale/registry/SystemRoot, add a
  keyboard device, or load any keyboard artifact. Add focused and runtime
  regression fixtures in fresh C11 root before another native trace.
* **S7 `54:0E` fixed-layout result and r45 trace admission (2026-08-11).**
  The focused service and runtime regression fixtures passed in fresh root
  `artifacts/build/t95-s7-kbdlayout-c11-001`: a valid real-mode
  `C4 C4 54 0E` resumes after four bytes and writes only low DX zero. It has no
  guest-memory range, host-locale/registry/SystemRoot input, keyboard/KEYB
  artifact, Bochs device, or OpenNT source change. The source contract and
  exact fixture result are retained in
  `docs/etc/research/t95-s7-bop-54-0e-service-evidence-001.md`. Admit exactly
  one fresh r45 native closure with that copied adapter source, one declared
  MSVC/x86 build, and one 30-second watchdog trace using the unchanged
  profile-pinned source-built `QUIT.COM` plus `/q` tail. Stop after any
  generator/build/process failure and classify only the next reached BOP;
  no additional BOP, device, host capability, Bochs or OpenNT behavior is
  admitted by this trace.
* **S7 r45 trace 038 result (2026-08-11).** The one admitted r45 build and
  trace prove that `54:0E` resumes: after the earlier `54:01` commit, trace
  038 reaches `54:0E` at `95AB:05D9` and then reaches the next unserved
  `54:04` at `0032:67A1`. `cmdsvc.h` identifies it as
  `SVC_CMDGETCURDIR`/`cmdGetCurrentDir`; later repeated selector `06` is its
  unserved-fall-through loop, not a keyboard failure. The trace has 14 commits
  and is retained in
  `docs/etc/research/t95-s7-r45-keyboard-layout-runtime-result-001.md`.
  Next admit only source/caller audit of `54:04`; no response, trace, device,
  Bochs or host capability is yet authorized.
* **S7 r45 trace 038 correction (2026-08-11).** Raw adjacent log lines show
  that `54:04` was not unserved: the existing current-directory service
  committed one four-byte `C:\\0` transaction and resumed from `67A1` to
  `67A5`. The actual next unserved selector is `FE` at `0D00:0100`; the prior
  `54:04` wording is superseded by
  `docs/etc/research/t95-s7-r45-keyboard-layout-runtime-result-001.md`.
  Next audit only selector `FE`'s OpenNT owner/caller; do not add another
  trace or service on the basis of the superseded classification.
* **S7 `BOP_FE` controlled-stop audit (2026-08-11).** Trace 038 reaches exact
  `C4 C4 FE` at `0D00:0100` only after the normal `BOP_DOS:36` termination
  notification. OpenNT defines FE as `BOP_UNSIMULATE`; the historical CCPU
  calls non-returning `c_cpu_unsimulate`, which exits its CPU invocation into
  the unavailable NTVDM monitor. Therefore FE is neither a CMD/DEM service nor
  an adapter resume point: advancing RIP by three would be false behavior.
  The retained audit
  `docs/etc/research/t95-s7-bop-fe-controlled-stop-audit-001.md` admits only
  the next design question: whether the existing typed CPU-result/Bochs seam
  can carry an explicit controlled-stop disposition without a Bochs semantic
  patch. No stop code, new trace, monitor replacement, BOP handler or device
  is admitted yet.
* **S7 controlled-stop seam design result (2026-08-11).** The active v2
  CPU-result and Bochs exception response accept only pass-through/resume;
  `SIM->quit_sim` is process cleanup rather than a return from the CPU loop.
  The earlier v1 exception ABI's reserved STOP value is evidence of intent but
  not an active transport. `BX-ABI-037` is now registered before any source
  patch: it permits only a default-off generic exception-stop disposition,
  with no Bochs BOP/OpenNT decode and no adapter monitor emulation. Before
  implementation, admit the exact stop record, native CPU-loop termination
  mechanism, CLI exit-status ownership and focused positive/negative tests.
* **S7 controlled-stop native-mechanics result (2026-08-11).** Bochs already
  has a suitable non-`quit_sim` termination path: set its native
  `kill_bochs_request` and `async_event`, then use the existing exception
  longjmp; the CPU loop observes the request and returns to its original main
  loop. This is the only candidate mechanical action for `BX-ABI-037`; it
  needs no monitor, secondary loop, BOP decode, guest-memory operation or
  device change. A later admission must still add the typed v2 stop record,
  make the adapter own exact FE recognition, and define CLI normal exit code.
* **S7 `BOP_FE` controlled-stop implementation result (2026-08-11).** The
  admitted adapter-only exact real-mode `C4 C4 FE` recognizer now returns a
  zero-effect v2 STOP record. The existing v3 fallback transports it without
  a guest read. Registered `BX-ABI-037` adds only a default-off generic Bochs
  STOP response: it sets existing `async_event`/`kill_bochs_request` and uses
  the existing exception longjmp so the original CPU loop returns; it does
  not decode BOP, advance RIP, modify guest state, call `quit_sim`, or add a
  monitor/device. The focused lexical boundary test passed and fresh r47
  MSVC/x86 named target linked successfully. r47 trace 039 is negative
  runtime evidence only: a prior real-mode stack fault reaches an unresolved
  third exception before FE, so no controlled-stop claim is made. Full source,
  toolchain condition and hash are retained in
  `docs/etc/research/t95-s7-bop-fe-controlled-stop-audit-001.md`. Next work
  remains source-led classification of the earlier stack/control-flow failure;
  no additional BOP service is admitted by this result.
* **S7 r48 deferred-POST controlled-stop trace admission (2026-08-11).**
  r47 is retained as a negative direct-entry control: it bypasses native POST
  and therefore reproduces the already source-proven zero-INT10-vector stack
  failure. Admit exactly one fresh deferred root at absent
  `artifacts/build/bochs-2.6-native-s7-native-post-handoff-msvc-r48`, generated
  with `-DeferredStartupPlan`; its only source delta from the last valid
  deferred r45 closure is the already registered generic STOP transport,
  adapter-only exact FE recognition, and copied exception-intercept header.
  Run its one declared MSVC/x86 named target once and, only if linked, one
  30-second/three-million-tick trace at absent
  `artifacts/analysis/t95-s7-native-post-handoff-040-20260811-001`. Preserve
  the r45 POST/option-ROM/BYOB profile and `/q` tail. Stop on first generator,
  build or process failure. This admission tests only whether FE reaches the
  controlled CPU-loop exit; it admits no new BOP response, device, firmware,
  guest, host capability, monitor, CLI exit code, or retry/variant.
* **S7 r48 deferred-POST controlled-stop result (2026-08-11).** The one
  deferred r48 MSVC/x86 named target linked successfully. Its one 30-second
  trace restores the original POST/option-ROM handoff and reaches the exact
  terminal chain `54:01`, `54:0E`, `54:04`, `50:36`, then `FE` at
  `0D00:0100`. The log records both `adapter controlled stop accepted` and
  Bochs's original `cpu loop quit, shutting down simulator`; this proves that
  typed FE STOP does not resume guest bytes and does return through the native
  CPU-loop mechanism. The observer nevertheless had to terminate the
  remaining process at its watchdog and reports exit code 1. Therefore normal
  CLI exit-status/lifetime is explicitly unimplemented, not inferred from CPU
  stop. The binary identity, raw trace and strict distinction are retained in
  `docs/etc/research/t95-s7-bop-fe-controlled-stop-audit-001.md`. Next
  admission, if selected, must define only the Bochs-to-CLI normal-completion
  contract; it must not add a BOP, monitor, guest service, device or host
  capability.
* **S7 r48 process-lifetime correction (2026-08-11).** The watchdog is not a
  failure of the proven CPU-loop stop. After `cpu loop quit`, original
  `main.cc` calls `bx_atexit` and returns; its outer Win32 wrapper then waits
  in an interactive `fgets(stdin)` prompt when `bx_user_quit` is false. The
  hidden observer child supplied neither EOF nor redirected input. This is a
  CLI child-process standard-input/normal-result contract, not a reason to
  patch Bochs, adapter, guest, BOP, firmware or device semantics. The next
  code admission must be a bounded `src/cli` launch-lifecycle design and
  fixture; it may arrange non-interactive child EOF and report normal return,
  but may not abuse `bx_user_quit`, power-off, `quit_sim`, or guest state.
* **S7 CLI child-EOF admission (2026-08-11).** Admit one bounded `src/cli`
  child-process repair. `run_process` may create one inheritable read-only
  `NUL` handle and assign it as the launched engine's standard input, while
  preserving the parent's standard output/error and existing environment,
  cancellation, command-line and exit-code behavior. The handle must be
  closed by the parent on every path after launch setup. Extend the existing
  test-only engine probe to require an immediate zero-byte read from stdin,
  then run the existing CLI policy integration test in a fresh modern GCC
  fixture. No Bochs source/configuration, `bx_user_quit`, `quit_sim`, adapter,
  BOP, guest, device, host namespace or actual Bochs trace is admitted. Stop
  if standard-handle inheritance cannot be expressed without changing child
  semantics beyond closed stdin.
* **S7 CLI child-EOF implementation/verification limit (2026-08-11).**
  `run_process` now provides each child one inheritable read-only `NUL` input
  handle via `STARTF_USESTDHANDLES`, preserves stdout/stderr, and closes the
  parent copy after launch. The existing engine probe now requires an
  immediate zero-byte stdin read. The existing GCC CLI fixture was invoked
  exactly once for its two named targets, but its `gcc.exe` child exited 1
  without diagnostics while compiling both changed and unchanged sources;
  it is the same external GCC-child failure already retained by the C11 STOP
  fixture. No binary or policy-test pass is claimed. Do not substitute the
  MSVC Bochs island for CLI acceptance; re-run this unchanged GCC fixture only
  after its compiler child can execute normally. The source scope remains
  limited to CLI child stdin and its test probe.
* **S7 CLI child-EOF alternate-GCC fixture admission (2026-08-11).** A
  direct executable probe establishes that the installed MSYS UCRT GCC is
  unusable because `cc1.exe` exits `0xC0000135`, while the already installed
  WinLibs MinGW-w64 GCC 16.1.0 (`x86_64-msvcrt-posix-seh`, msvcrt CRT) compiles
  the changed CLI source successfully. Admit one fresh modern-only root at
  absent `artifacts/build/t95-s7-cli-child-eof-winlibs-gcc-001`, configured
  with that exact compiler and the existing Ninja executable. Build only
  `ntdos64-run` and `runner-engine-probe`, then run only the existing
  `ntdos64-run-policy` CTest. This is an independent CLI toolchain island:
  it consumes no Bochs/OpenNT object/header/artifact and creates no cross-CRT
  ABI. Retain compiler identity, CMake command and test result. Stop at the
  first configure/build/test failure; no tool installation, Bochs trace or
  additional source scope is admitted.
* **S7 CLI child-EOF result (2026-08-11).** The fresh WinLibs GCC 16.1.0
  msvcrt fixture configured, compiled `ntdos64-run` and `runner-engine-probe`,
  and passed the single `ntdos64-run-policy` integration test. The probe now
  requires and observes a zero-byte stdin read, while retaining the prior
  path-free profile/root, drive policy and launch-declaration assertions.
  Thus the CLI owns non-interactive engine stdin without a Bochs/adaptor/guest
  change. This does not prove a real Bochs engine launched through the CLI:
  the distinct CLI engine-argument contract remains queued. Exact toolchain,
  commands, result and MSYS-UCRT limitation are retained in
  `docs/etc/research/t95-s7-cli-child-eof-evidence-001.md`.
