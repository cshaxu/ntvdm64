# S10 provisional recovery worklist

This is a dependency-ordered handoff from the current evidence, not the final
S10 scope or permission to bypass its remaining source reconciliation. Entries
separate demonstrated behavioral differences from source-level risks. Stages
follow the approved T473 stage plan; a stage must establish actual owner,
original behavior, lifecycle and regression before implementing a change.

| Stage | Cohort and evidence | Established result / remaining work |
| --- | --- | --- |
| S11 | [CPU far-transfer diagnostic](shard-3/cpu-far-diagnostic-audit.md); [BOP/interrupt diagnostic inventory](shard-1/diagnostic-guest-access-inventory.md); [illegal-op diagnostic](shard-0/illegal-op-diagnostic-audit.md) | Controlled real CCPU fixture shows exception ordering changes with logging disabled; other fixtures establish extra guest reads. Recover original ordering without removing the underlying handlers; prove normal, fault, disabled/enabled and nested paths. Actual guest fault incidence remains unproved. |
| S11 | [Signed memory bit indexes](ccpu-header-macro-review.md#demonstrated-signed-memory-bit-index-difference) | Native x86 hardware and both real CCPU packages disagree for BTC word memory with index FFFF: hardware selects -2 and carry one; CCPU selects +8190 and carry zero. Recover the complete original bit-string addressing group: signed register versus immediate indexes, both widths and address sizes, wrap, permissions, carry and fault ordering. This is a reproduced host execution defect; no repair is claimed. |
| S11 | [x87 operand spans](ccpu-header-macro-review.md#x87-source-inconsistency-runtime-result-inconclusive) | Actual expanded prechecks use 94/106 while transfers use 108/94. Establish the original protected-mode exception route before testing both operand sizes, exact/short limits, read/write and failure-before-commit. Exploratory DPMI startup and raw-PM GP controls are inconclusive; do not attribute their FF exit to x87. |
| S11 | [SAS vector](shard-3/sas4gen-vector-audit.md) | Product vector contains breakpoints and field-layout differences, but reachability and consumers are unproved; trace indirect/generated users before a repair or exclusion. |
| S11 | [486 profile gates](shard-3/cpu486-profile-boundary.md) | Current SPC386 objects omit six guarded 486 definitions and the decoder retains Int6 paths. Reconcile CPU_486 declarations with SPC486 bodies, original activation provenance and affected AC/CD/NW, TLB/control/reset state as one finite recovery group; no capability is excluded merely by current nonselection. |
| S11 | [DLL-entry BOOLEAN return](dll-entry-boolean-return.md) | Actual selected bridge declares original one-byte BOOLEAN as int. Controlled valid-return witness propagates non-return EAX high bits, while a one-declaration test candidate normalizes correctly. Restore the real return type at the project generator; verify attach failure, notifications/cleanup and actual WOW lifecycle. Real original heap-failure manifestation is unproved; no private loader import or mirror/guest change is needed. |
| S11 | [Original export-source/profile recovery](export-source-recovery-review.md) | The generator consumes a supplemental generated declaration with one project-added export, while bypassing the byte-exact primary template. Actual i386/CPU40 metadata declares 140 names, only 138 published; query/reallocation names are absent. Recover canonical template generation and exact project-extension ownership, preserve existing APIs and verify both original memory service signatures/providers/lifecycles with S12 before claiming their surface complete. Restore the modified historical declaration rather than accepting an unregistered mirror addition. Name/source retention is not behavior coverage. |
| S12 | [Kernel directory reset](external-kernel-vdm-boundary.md) | Original fast index/name reset has no observed product callback registration; verify actual selection and filename fallback under directory mutation before deciding whether a provider is needed. |
| S12 | [XMS block request](shard-0/xms-block-zero-boundary.md) | Original zero-size allocation and equal-size reallocation contain local-value candidates; guest source handles the latter before dispatch. Establish actual immutable guest path and contract before correcting host behavior. |
| S12 | [Suballocation failure](shard-2/suballoc-xms-callback-failure-review.md) | Controlled original-source fixture shows partial commit and failed decommit bookkeeping; selected native callbacks can fail. Establish actual XMS callback effects and guest-visible recovery before designing rollback. |
| S12 | [Disk close](shard-3/disk-close-failure-audit.md) | Actual-object failure seam demonstrates partial retirement after guest counter update failure; real reachability is unproved. Establish guest counter and resource ownership, retry/teardown and both floppy/fdisk outcomes. |
| S12 | [COMMAND exit](shard-1/command-exit-owner-audit.md) | Original host dispatch owner is identified; the current cached map and released executable differ in identity. Verify actual guest dispatch, nested exit, broker receipt and abnormal handoff. |
| S12/S13/S17 | [DOS command inputs](dos-command-input-review.md) | Historical build-input exclusions do not exclude COMMAND/utility execution, memory reporting, keyboard/codepage, graphics/printing or external host-service behavior. Verify the immutable guest/provider boundaries and image provenance at their existing owners. Legacy message bindings are retired only after tracing their original replacements. |
| S12 | [ForceDOS](shard-3/forcedos-original-owner-audit.md); [PIF](shard-3/pif-config-lifecycle-audit.md) | ForceDOS consumer rejects an encoded original flag; nested guest execution still needs proof. PIF state retention is a source-level question, not an established defect. Trace task and worker owners before choosing changes. |
| S11/S12/S13/S17 | [BIOS integrations](bios-native-role-review.md) | Preserve the original 256-entry dispatch and virtualized callbacks; new real guest proof covers 768 control errors, 64 PROD illegal no-ops and four invalid EMS returns per package only. S12 owns valid 30-slot EMS/handle/page/map/access-key/reset and failure contracts; S13 owns actual CMOS, ROM-error and slave-IRQ lifecycles. The D11 slave-port finding still requires real PIC/guest proof. |
| S13 | [BIOS memory and RTC](shard-3/bios-memory-rtc-audit.md) | Historical INT 12h baseline and CMOS flag tests do not cover changed BDA, BOP 70 alarm, countdown and EOI; verify those reached original paths. |
| S13 | [Original scan-table exact bound](input-native-role-review.md#proven-one-past-key-table-read) | Actual nt_keycd.obj reads one-past normal[133]/enhanced[84] under an isolated data sentinel and returns it. Change both `>` guards to `>=` with a registered minimal mirror exception; preserve all mapping/ABI and translation flow. Verify strict rejection, all normal/enhanced scans, BIOS conversion and actual DOS/WOW input/reset/retirement. Real queued occurrence or guest crash is not established. |
| S13 | [ROM and CPU register owners](shard-1/rom-residency-xt-audit.md) | Required embedded ROM failures return zero and rom_init ignores the result; deployed failure reachability and outer handling are unproved. Verify required resources, protection, startup failure and register/parity ownership. |
| S13 | [Codepage transition](shard-1/standalone-tool-chain-audit.md) | Independent console carrier codepages were observed; establish original console authority and test both native and guest output before modifying propagation. |
| S13 | [Original NT quick events](quick-event-owner-review.md) | Selected `qevnt.obj` passes 37 controlled queue/identity/failure/reentrant callback assertions and the original 64-bit declaration rejects a wrong-width build. Preserve NT owner selection and intentional unused-tick boundary; verify real calibration, late-event behavior, ICA concurrency, device/guest reset and retirement and published-image equivalence before declaring `qevnt.c`/`host_qev.h` full. Do not repair or revive generic `quick_ev.c` as if it were the selected provider. |
| S13 | [IOS and CCPU port owners](ios-owner-review.md) | Three actual cached objects pass 70 controlled routing/registration/string/disconnect/wrap assertions. Preserve original NT word-split dwords, whole-port disconnect and callback snapshots; S11 must verify real decoder/permission/fault paths, S13 device concurrency/reset/retirement and S17 applicable VDD callback retirement. These six files remain partial; direct dword array availability does not authorize SFELLOW or prove the NT CPU uses it. |
| S14 | [Generic thunk](shard-0/wow-generic-thunk.md) | Product-object mismatch failure and real guest differential exist; candidate original assembly needs an EBX-safe ABI. Complete convention/exception/lifecycle proof and coherent package tests before publication. |
| S14 | [WOW current directory](shard-2/wow-directory-sync-review.md) | Original host cwd, CDS and task state ordering are identified; existing stubbed caller tests do not prove real guest task-switch, long-path or failure behavior. |
| S14 | [SYSTEM.INI profile](shard-2/wow-kernel-profile-module-review.md); [language thunks](shard-1/wow-language-thunk-owner-audit.md) | Trace shadow failure and write/teardown semantics; separately verify real guest ANSI, DBCS, selector and writeback behavior. These are unverified boundaries, not demonstrated failures. |
| S14 | [DDEML](shard-2/ddeml-owner-boundary.md); [WINMM callback](external-winmm-callback-boundary.md) | Historical private dependencies are identified but actual guest/provider behavior and recoverability remain pending. Trace callback identity, queue delivery and task teardown; never infer absence from an unselected provider alone. |
| S15 | [EDIT tab-stop return](shard-2/wow-edit-tabstop-unthunk-review.md) | Two gated real Win16 observations show word-break state changes 0 to 1 after a successful tab-stop message; selected object injection confirms the original fallthrough. Recover correct semantics and verify callbacks, errors and lifecycle. |
| S15 | [Clipboard formats](shard-2/wow-clipboard-format-failure-review.md); caret finding in [cross-shard findings](findings.md) | T460 covers text and DIB, not every format; fault/ownership paths require format-specific tests. Existing caret evidence is inconsistent and needs an actual guest witness. |
| S15 | [Selected metafile versus dual-purpose reference source](wow16-extension-source-role-review.md) | Preserve the original GDI local-bits / WOW32 play-enumeration split. The selected `wgmeta.c` and public-host witness do not prove original guest conversion, mutable enumeration callbacks, allocation failure and nested retirement; verify those at their real per-ordinal owners before declaring the file full. Do not classify `gdi16.h` as guest-only or restore an alternative renderer merely because old bodies are retained. |
| S16 | [WOWFAX](shard-1/wowfax-trace.md); multimedia source-selection findings in [cross-shard findings](findings.md) | Object compilation does not prove reverse MCI, aux/joystick, fax installation or runtime behavior. Establish actual guest/provider route and its dependency boundary. |
| S17 | [DLC completion event](shard-1/vdmredir-dlc-completion-event-audit.md) | Original source checks the wrong event after the second creation; provider availability and actual failure path remain unproved. Test cleanup, worker thread and response on injected creation failure before correction. |
| S17 | [BDE extension](shard-0/bde-debugger-extension-audit.md) | Original standalone debugger-extension command chain is identified; product scope and whether its context/symbol dependencies are recoverable remain to be decided. Do not equate it with all debugger functionality. |
| S17 | Registry identifiers and selected-versus-linked reconciliation in [cross-shard findings](findings.md) | Resolve misleading provenance and owner records without treating documentation collisions as runtime defects; unresolved unselected and external paths remain in the exhaustive audit. |
| S18 | [Unused C-VID test-provider scaffold](cvidc-inert-scaffold-review.md) | Current production and actual binding-fixture inputs exclude the inert object, whose query has no consumers. Reconfirm against the implementation-time graph, then remove only the unused project generator/rule declarations and exercise the real fixture; this is not a production success-stub recovery or an original-provider removal. |

The [retired-host checkpoint](retired-host-role-review.md) excludes four
superseded physical files, not their selected NT capabilities. S11/S13 retain
nt_reset once-only cleanup, error/return codes and NTSRV-coordinated retirement;
S12 retains actual DEM file/handle/path and configuration behavior; S13 retains
device/profile/frontend propagation. Source cleanup order and 73 linked DEM
slots do not close these obligations or accept unimplemented/fast-I/O slots.
Do not restore old no-op HFX file stubs or a dead application-local kill flag.

The [memory checkpoint](memory-native-role-review.md) leaves S13 original
UMB initialization/failure, public hook and UMB-list identity/retirement,
nesting, actual guest faults and SAS/XMS/reset lifecycle. Direct helpers accept
six invalid diagnostic ranges, while public entry guards reject those inputs
for a valid registered hook; establish reachable inputs/coherence before
deciding on a patch. This is not a demonstrated product defect. Alternative
HOST_SAS and NT4 hardware-mapping files do not exclude selected CPU40 memory
capabilities or authorize treating guest numeric addresses as host pointers.

The [planar checkpoint](planar-native-role-review.md) leaves S13 actual
CGA text/high-colour and EGA low-resolution/alternative modes; real caller
dimension/address preconditions and source/LUT/DIB lifetimes; mask concurrency,
mode changes, transport failures and retirement. Original unsupported mono/
user-font bodies require an original-profile capability disposition, not a
success stub. The 26,114 controlled pixel cases do not close those contracts.

The [input/NLS checkpoint](input-native-role-review.md) leaves S13 real LED/
BIOS/Console synchronization, input nesting/reset/retirement, OEM/DBCS and
Alt-numpad conversion and failure, and missing initialization resources/
noncontinuable NLS failure, locales and actual error/dialog lifetimes.
The two generic error-table physical files are superseded by original NT
resource/error owners, not exclusions of localization or error capability.

The [video-entry checkpoint](video-native-role-review.md) leaves S11/S13
valid BIOS/table dispatch and stream ownership; adapter reset/retirement;
all original geometry, wrap/split/font/update/paint modes; VGA palette/pixel/
scroll/display-information and state buffer/failure contracts. CPU40 uses
original EVID instead of legacy a2cpu ga_glue; address units, direction,
RAM/video source identity, faults and all generated operations remain at
those partial owners. EGA_DUMP tracing still needs a diagnostic-profile and
file/callback lifetime disposition. Invalid-entry passes prove none of these.

The [disk-provider checkpoint](disk-native-role-review.md) leaves S12/S13
the original BIOS/controller group: configured geometry and real read/write/
verify/format, DMA/byte-word boundaries, command/result phases, interrupt and
reset ordering, media/provider changes, failure and retirement. Keep the
separate proven host-counter close transition. SLAVEPC serial/FDC transport
and non-PROD diagnostic selection need explicit original-profile dispositions;
all three remain pending rather than treating guarded code as lost capability.

The [system-provider checkpoint](system-native-role-review.md) leaves S13 a
finite device group: DMA byte/word transfers, masks, auto-init, decrement/page
wrap, LIM and reset/failure; RTC alarm/calendar/format, register-C concurrency
and real IRQ8/BOP70; all PIT modes/gates/latches and time discontinuities;
exact nineteen-strobe blank/refresh/reset and retirement; and unexpected
master/slave/cascade interrupts including D11. Generic CMOS and ica.old are
physical alternative/revision exclusions only; their selected NT capabilities
remain in this group. The full dummy leaf does not close these lifecycles.

S18 removes only providers actually displaced by verified recoveries. S19
reconciles every applicable path and runs the complete product and capability
gates. Neither this list nor prior per-package acceptance converts pending
rows to full/excluded, and the 42,963 external rows still require relevance
review. Preserve original guest bytes and the owner-approved host boundaries.
