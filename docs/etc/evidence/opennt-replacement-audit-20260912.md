# Original-owner replacement audit, initial pass

## Question and method

Which selected COMMAND/EDIT host providers replace existing OpenNT behavior?
Read-only source and build-generator inspection on 2026-09-12, against HEAD
`05f3859da` plus retained WIP. Inspected `base_vdm_local.c`,
`base_vdm_client.c`, `console_compat.c`, `demfastio.c`, mirrored `srvvdm.c`,
and external OpenNT `windows/core/ntcon/server/private.c`. Source research
follows the project source policy. No build, guest run or product change.

## Rechecked initial findings

| Functional unit | Current owner | Original owner | Disposition |
| --- | --- | --- | --- |
| Next-command service | adapter BaseSrv `get_next_command` | `base/win32/server/srvvdm.c:BaseSrvGetNextVDMCommand` | Rehosted queue/ownership/wait/capacity policy; recover original service slice before deleting replacement. CSR transport is a separate excluded dependency. |
| PIF response | adapter BaseSrv `fill_pif_info` | `srvvdm.c:BaseSrvFillPifInfo` | Rehosted response policy, not merely transport; compare field and failure order. |
| First-VDM state | adapter BaseSrv `base_vdm_local_is_first` | `srvvdm.c:BaseSrvIsFirstVDM` | Original global policy replaced by session state; explicitly resolve cardinality before migration. |
| Current directories (set/get grouped) | adapter BaseSrv directory storage | `srvvdm.c:BaseSrvSetVDMCurDirs/BaseSrvGetVDMCurDirs` | Recover original ownership and consumption rules through bounded storage binding. |
| DOS task exit | adapter BaseSrv exit | `srvvdm.c:BaseSrvExitDOSTask` | Original teardown policy partly replaced; enumerate unavailable console/CSR lifecycle operations. |
| Fast read | mirror overlay `dos/dem/demfastio.c` | `base/ntos/vdm/x86/rdwr.c:NTFastDOSIO` | New user-mode seek/read/guest-copy route. Compare finite original slice and original carry/fallback path; not proven corrupting writer. |
| Cursor visibility | Win32 adapter `ShowConsoleCursor` | `windows/core/ntcon/server/private.c:SrvShowConsoleCursor` | Confirmed unequal contract: original mouse-pointer display count versus current text-cursor visibility and fixed 0/-1 result. Not proof of prior false clicks. |

The generator `tools/build/New-T310OriginalSoftpcNinja.ps1` explicitly selects
the BaseSrv adapters, `console_compat.c` and the fast-I/O overlay. Mirror
presence alone does not prove an original function enters the product link.
These seven grouped findings are not seven proved removable implementations:
some contain essential unavailable-interface bindings. No safe deletion-line
total has been established.

## Additional scope, not included in the seven

- `base_vdm_client.c:GetNextVDMCommand` also implements capture/wait/retry;
  compare original `base/win32/client/vdm.c`, not just server routines.
- `ReadConsoleInputExW` filters Alt+Enter key-down but not key-up, and its
  NOREMOVE branch bypasses that filter. This is policy beyond API forwarding;
  compare the original input contract before judging intentional asymmetry.
- CPU40 descriptor transitions, BOP/interrupt return, keyboard, mouse,
  text video, memory and environment services remain incomplete audit areas.
  Include mirror-internal special cases and diagnostics, not just adapters.
- Architecture/source policy now places external kernel VDM in `opennt-host`,
  but `docs/design/CODING.md` and `docs/rules/CODING.md` still retain older
  kernel-vdm placement text. Record this governance inconsistency; no source
  move is justified solely by the directory label.

## T405 overlay follow-up: source contracts, not removal approval

Rechecked against the unchanged product-source baseline on 2026-09-12.
This adds bounded findings to the initial pass; it is not whole-project
audit completion. No product code or build selection changed.

| Overlay unit | Original counterpart | Current finding and remaining proof |
| --- | --- | --- |
| `dos/dem/demfastio.c:demFastRead` | OpenNT `base/ntos/vdm/x86/rdwr.c:NTFastDOSIO` | Confirmed duplicated seek/read service. The original uses trap-frame registers, VDM TIB scratch, native file APIs and pending-I/O completion; the overlay uses CPU accessors, a guest-memory lease and synchronous Win32 file APIs. These are substantial implementation substitutions, not a symbol alias. |
| `dos/dem/demfastio.c:demFastWrite` | Same original handler plus NTDOS slow-write fallback | Carry-only refusal, not a second write implementation. Do not count this as duplicated file-write logic or delete the fallback contract. |
| `softpc.new/base/ccpu386/localfm.c` | Original same-named global carrier; selected `ccpusas4.c` SAS owner | Retains Gdp/Cpu/Video and avoids a second Sas definition, with explicit C-VID type selection. Composition difference; no newly implemented execution algorithm found in this body. |
| `softpc.new/base/ccpu386/sas_overwrite_memory.c` | Original `ntstubs.c:sas_overwrite_memory` | Direct call to `c_sas_overwrite_memory`; extracted wrapper, not a replacement invalidation algorithm. Whole-file restoration must avoid alternate global/provider collisions. |
| `softpc.new/base/cvidc/mvdm_cvidc_vector_binding.c` timing accessors | Original `base/cvidc/accessfn.c` CPU-vector accessors and `base/system/qevnt.c` consumers | Autonomous timing state requires priority review. `GetJumpCalibrateVal` and `GetJumpInitialVal` both return one local value initialized to 100; `SetJumpInitialVal` changes it. Separate original accessor slots do not prove these values interchangeable. No equivalent original backing provider has yet been established. |
| Same C-VID file, vector assignments | Original Cpu/Sas/Video vectors and generated binding inputs | Composition repair distinct from the timing policy. Requires generated-slot audit; neither the whole file nor every binding is established as removable. |
| `softpc.new/host/src/mvdm_ica_eoi_bridge.c` | Original `ica_eoi` PIC leaf | Typed forwarding with a LONG temporary, null guard and rotate=0. No PIC arbitration algorithm is duplicated in this wrapper. Call-site contract still needs review. |

The C-VID header and EOI header supply declarations, not separate functional
findings. Together the rows cover the five overlay C bodies and their two
headers, but do not close their transitive/generated dependencies.

### Fast-read contract details needing end-to-end checks

The original handler sets VDM_IDLEACTIVITY, advances EIP past its kernel BOP,
rejects encoded console/pseudo-standard handles, and converts the file offset
as unsigned. The overlay rejects current standard handles and non-disk files,
passes an explicitly zero high word to SetFilePointer, and delegates BOP
dispatch and guest-memory ownership elsewhere. Do not transplant kernel EIP
advancement into the CPU40 callback: it could double-advance the instruction.
The original rejects a successful seek whose low position equals 0xffffffff;
the overlay accepts that position when GetLastError is NO_ERROR.

The overlay can return carry after a successful ReadFile if lease release
fails. Whether a subsequent slow read can then observe an advanced file
position depends on the caller's ZF/seek behavior and lease failure contract;
this is an investigation target, not a demonstrated DOSX corruption.
Likewise the absent local idle-activity assignment is not proof that the
surrounding dispatch omits it. Original pending-status/EOF handling and these
side effects must be compared before proposing a replacement.

### Reviewer limits

Source bodies and generator binding names were re-read separately from their
divergence comments. The comments are provenance hints, not equivalence
proof. Searching the original ccpu386 and host C sources for the three timing
accessor names produced no backing implementation; that bounded negative
search does not prove one is absent from other generated/source packages.
The timing row is therefore unproven autonomous behavior, not yet a confirmed
duplicate original algorithm. The existing seven grouped replacement
findings must not be inflated by counting this candidate as proven.

## T405 BaseClient and BaseSrv contract follow-up

Read the complete current `base_vdm_client.c` and `base_vdm_local.c`, then
rechecked corresponding original client and mirrored server bodies. These
are selected replacement policies, not merely CSR transport substitutions.
The following refines the five initial server findings and adds two distinct
functional units: client command acquisition and server reentry management.
Thus this ledger now identifies nine grouped replacement findings including
the original fast-read and cursor rows; this remains a lower bound, not the
complete project result. Subcases below are not extra counted units.

| Unit | Source-proven difference | Disposition before repair |
| --- | --- | --- |
| PIF query, `fill_pif_info` versus `srvvdm.c:BaseSrvFillPifInfo` | Original clears selected output strings before capacity checks and always publishes required lengths after them. Local code returns immediately on insufficient capacity, leaving supplied lengths unchanged. Original prefers StartupInfo title, then AppName, and explicitly terminates the copied title; local record only supplies AppName and no Reserved payload. | Confirmed partial replacement and unequal failure contract. Restore original field/capacity policy through a bounded record binding; first test insufficient PIF/title/directory capacity, title precedence and non-consumption. |
| Reentry, `base_vdm_local_dispatch` versus `srvvdm.c:BaseSrvSetReenterCount` | Original decrements and signals an existing VDM wait event on each decrement. Local code rejects decrement at zero and increment at UINT32_MAX, clears a new native-child pending flag on increment, and signals only when count reaches zero and no native launch is pending. | Newly confirmed repeated function with added scheduling policy. Compare nested child begin/cancel/increment/decrement traces before deciding which local lifecycle binding remains necessary. No claim the source's unchecked arithmetic should blindly replace bounds checks. |
| Client command acquisition, `base_vdm_client.c:GetNextVDMCommand` versus original `base/win32/client/vdm.c` | Both wait, set ASKING_FOR_SECOND_TIME, clear exit code and retry. Original owns CSR capture buffers, copies successful fields under exception handling, and zeros all size fields on errors other than STATUS_INVALID_PARAMETER. Local capture is a shallow VDMINFO copy; dispatch writes caller buffers directly and the client copies captured lengths on every error. An early ERROR_NOT_READY can therefore leave original requested lengths visible instead of zeros. | Newly confirmed client-policy replacement separate from the server queue. Preserve original result/error/copy policy; replace only capture/transport dependencies after establishing the local synchronous pointer lifetime contract. |
| Command response, initial next-command finding | Original server/client preserve STARTUP_INFO_RETURNED and startup metadata, plus supplied standard handles. Local normal response sets VDMState=0, StdIn/StdOut/StdErr=NULL and Desktop/Title/Reserved lengths=0. Separate WOW requests are not implemented; an empty shared-WOW query succeeds, while DOS may wait or terminate the session under the local one-shot policy. | Confirmed restricted service model, not full original BaseSrv behavior. Audit actual app producers and consumers before attributing observable failures or replacing the model. |
| First-VDM query | Original `fIsFirstVDM` is server-global; local first_vdm_available belongs to the session record. Query-and-clear syntax is similar but scope is different. | Original policy duplicated with changed cardinality; multiple-worker/session behavior must be an explicit decision, not hidden in an adapter. |
| Current-directory set/get | Original set frees the old value before allocation; local set allocates before replacing, so allocation failure preserves old state. Original server holds its DOS critical section through set/get; local directory operations have no corresponding lock. Successful get consumes and frees the record in both. | Preserve one-shot semantics; review failure behavior and thread reachability. The lock difference alone is not proof of an exercised race. Client zero-length no-op and server mutation must not be confused. |
| Exit | Original client routes DOS and WOW exits to BasepExitVDM and closes a returned wait handle. Local client forwards to a helper which ignores WOW exits and completes only the DOS session. | Partial lifecycle replacement/missing WOW service. Do not count the ignored WOW branch as a working alternate implementation or infer complete WOW cleanup from DOS success. |

Original evidence locations: client `vdm.c` GetNextVDMCommand at line 340,
retry/error handling around 582-650 and ExitVDM at 731; mirrored `srvvdm.c`
BaseSrvIsFirstVDM at 472, Set/GetVDMCurDirs at 486/706,
BaseSrvSetReenterCount at 2529 and BaseSrvFillPifInfo at 2722.
The compared upstream is read-only `O:/repos.external/OpenNT`; local paths
are relative to this repository. No product repair or runtime experiment was
performed for these findings.

Reviewer cross-check: buffer-capacity failure was followed through both local
server and local client, rather than relying on the server comment claiming
the same copy order. Reentry signaling was compared against the actual
original body. The original capture/CSR/security environment is not available
unchanged; original source reuse still needs an explicit interface boundary.
These findings do not prove the reported DOSX/WRITE failure's root cause.

## T405 Console contract follow-up

Read the complete current `win32/source/console_compat.c` and original
Console client/server implementations. The following are additional distinct
functional contracts; the cursor finding was already counted above.

| Current API | Original owner and evidence | Finding |
| --- | --- | --- |
| `WriteConsoleInputVDMW` | `windows/core/ntcon/client/private.c:761` calls WriteConsoleInputInternal with Unicode=TRUE, Append=FALSE; ordinary `client/iostubs.c:425` passes Append=TRUE. `server/directio.c:386` dispatches FALSE to PrependInputBuffer. | Confirmed unequal forwarding: current wrapper uses ordinary WriteConsoleInputW and therefore appends rather than prepends. Reached source consumers include `nt_event.c:ReturnUnusedKeyEvents` and other return-input paths. Returned older events can follow newer queued events. Test existing queue B plus returned events A: original contract is A,B, not B,A. No CPU modification is implicated. |
| `ReadConsoleInputExW` | `server/directio.c` passes NOREMOVE and NOWAIT independently to ReadInputBuffer under the Console lock. `server/input.c:1769` consumes the unreserved Alt+Enter path, excluding Ctrl; key-down controls the display transition but key-up also returns without input delivery. | Current replacement checks queue size separately before a potentially blocking read, so NOWAIT is not atomic against another reader. NOREMOVE goes directly to Peek and bypasses the local filter. Local Alt+Enter filter consumes only key-down, does not exclude Ctrl, and has no reserve-key policy. These are verified contract differences; exercising a NOWAIT race requires a concurrent reader, not assumed from syntax alone. |
| `GetConsoleKeyboardLayoutNameA` | `server/getset.c:1209` first activates Console->hklActive, then queries the layout name. | Current GetKeyboardLayoutNameA queries the caller thread's layout without selecting Console state. Similar output shape does not prove identical layout ownership. Confirmed replacement with different state source; test distinct caller/Console layouts before attributing guest keyboard symptoms. |
| `GetConsoleInputWaitHandle` | `client/stream.c:28` returns dedicated InputWaitHandle. | Current returns STD_INPUT_HANDLE. This is a changed wait endpoint, not original event ownership. Redirection may make stdin differ from the Console input endpoint. Establish product handle setup and the original event initialization before deciding safe reuse; not yet counted as a proved faulty runtime path. |
| `VDMConsoleOperation` | `server/srvvdm.c:23` checks registered VDM process ownership and dispatches window operations. | Current implements only IsIconic, client rectangle and client-to-screen using GetConsoleWindow. Hide, screen-to-client, hidden-state and fullscreen-nopaint operations are rejected. The three leaf User32 operations are reused, not autonomous coordinate algorithms. Registration/security checks and missing operations require separate disposition. |
| `SetConsolePalette` | `server/private.c:406` selects and retains a graphics-buffer palette, manages usage/realization and old-palette lifetime. | Current snapshots RGB entries into session storage and emits an event. That is a restricted alternate presentation path, not restoration of the original palette lifecycle. Inspect the event consumer before concluding what flags/lifetime behavior the complete path implements. |

Registration, text invalidation and graphics-buffer creation in this adapter
also replace Console-server facilities with session backing storage and
presentation events. Their transitive session/app implementations still need
comparison before assigning removable original-owner slices. ConsoleMenuControl
and SetConsoleKeyShortcuts explicitly fail, so they are missing facilities,
not functioning duplicated algorithms. Default-off tracing is counted separately.

Reviewer cross-check: the prepend/append difference was verified through both
original client wrappers and the server branch, not inferred from API names.
Alt+Enter key-up suppression was checked in the original input handler, while
the local filter was checked for Ctrl and peek behavior. These findings are
not evidence that the historically fixed mouse false-click defect has returned.
No product code, public entry point or runtime binary was changed.

## T405 CCPU executor diff follow-up

Compared the complete `softpc.new/base/ccpu386/c_main.c` diff against
`O:/repos.external/OpenNT/base/mvdm/softpc.new/base/ccpu386/c_main.c`, then
followed the affected HLT/reset path and the EIP setter. This is a mirror
internal audit: successful COMMAND/EDIT operation does not exempt these changes.

| Local change | Original behavior and verified delta | Proposed discussion item |
| --- | --- | --- |
| DIV-125, UPDATE_INTEL_IP_USE_OP_SIZE, current line 648 | Original USE16 expression masks the sum `(EIP + delta) & WORD_MASK` by C operator precedence. Local expression is `EIP + (delta & WORD_MASK)`. `c_reg.h:135` assigns CCPU_IP without an implicit 16-bit mask. For EIP=65535 and delta=2, original result is 1, local result is 65537; independently evaluated as integer expressions. | Confirmed altered arithmetic, not a harmless x64 cast. Restore original mask scope after owner approval and test 16-bit boundary crossing at actual macro consumers. The expression counterexample is not an executed guest reproduction. |
| DIV-214, RESET in HLT, current line 4087 | Original HLT tests RESET without consuming it, leaving the following external-event block to clear it and call c_cpu_reset. Local HLT uses c_cpu_take_event, which clears it before breaking. The later reset block at 4436 calls the same take helper and therefore sees no bit unless a new RESET arrives. | Confirmed lost-reset path introduced by local atomic conversion. A peek and a consume are not interchangeable. Proposed repair preserves original observation/consumption sites; atomicity itself requires a separate producer-thread analysis. No claim this causes current WRITE failure. |
| DIV-214 hardware event handling | Original acknowledges PIC then clears CPU_HW_INT_MASK; local consumes the bit before acknowledgment. Producers now use InterlockedOr and consumers CAS rather than plain read/modify/write. | Reimplemented synchronization policy inside the executor. Review producers and PIC notification/acknowledgment ordering before retaining or reverting the complete change. The valid motivation to avoid lost updates does not prove every converted site equivalent. |
| DIV-221 signed PIC rejection | Local keeps ica_intack result signed and skips delivery on -1 instead of narrowing immediately to IU16. | Guard changes original behavior; verify original PIC caller contract and synchronization owner. Do not assume the comment's historical-monitor explanation is established evidence. |
| Hardware interrupt hook | Local executor invokes existing `nt_inthk.c:host_hwint_hook` before original do_intrupt fallback; the original host source exposes VdmInstallHardwareIntHandler and invokes the registered handler. | Added composition of an original provider, not an independently invented DPMI dispatch algorithm in this hunk. Still requires full selected-path/return-contract audit. |
| Declaration/setjmp changes and observations | Added original declarations, explicit jmp_buf dereference, BOP FE/decode/retirement diagnostic calls. | Keep ABI corrections and diagnostics separate from execution-policy findings; x64 wording alone does not justify removing declarations. Diagnostic providers and overhead need their own review. |

Reviewer checked that HLT sets quick_mode=FALSE after the wait and that the
subsequent reset handler actually requires a still-set bit. The proof is
conditional on one RESET waking that HLT and no intervening new RESET; it
does not rely on a scheduler race. The IP-mask counterexample follows the
actual SET_EIP definition, not an assumption about register truncation.
No CPU or product code has been edited. These are new mirror-internal
restoration candidates, distinct from the previously counted service groups.

## T405 ccpu386 changed-file coverage

The paired inventory lists 24 non-byte-equal ccpu386 files: 23 text diffs
and c_getset.c with CR-only changes. All 23 complete diffs have now been
read against their original same-path OpenNT sources; c_getset.c was checked
with a CR-normalized diff and produces no hunks. This closes direct diff
reading for this directory, not transitive provider/ABI or runtime verification.

| Files | Classification after direct diff review |
| --- | --- |
| c_main.c | Arithmetic/reset findings above; event synchronization, PIC rejection and original-hook composition need full boundary review. |
| c_seg.c | New CPU_40_STYLE special case for protected-mode selector 0040h writes base=0400h, limit=ffffh and writable/readable DPL3 cache directly. It bypasses original GDT/LDT bounds, descriptor read, type/access/present validation and accessed-bit update. This is a source-internal semantic replacement, not just an API binding. Recover descriptor establishment at its original initialization owner rather than infer permission to bypass generic CPU checks. The comment's DOSX provenance still requires tracing. |
| c_intr.c | Calls local DpmiCpu40RestoreNativeIdt on every protected-mode entry to do_intrupt, and invokes host_swint_hook for privileged-checked protected software interrupts. The first provider lives in modified dpmi32/modesw.c; original INTx already has a software hook. Audit descriptor ownership and possible duplicated hook entry rather than label this an original interrupt body. |
| ccpusas4.c | c_GetPhyAdd adds physical-mapping translation and external resolution before original RAM lookup. This delegates an alternate memory service; its semantic owner lies in the mapping provider and kernel VDM comparison. Added store/direct-access observations are separate. |
| c_xfer.c, call.c, intx.c, iret.c, jmp.c, ret.c, sti.c | Added observation calls; original instruction bodies otherwise unchanged in the diffs. Default-off provider labels do not establish zero overhead or side-effect freedom. |
| c_bsic.h, c_reg.c, c_xcptn.c, ntstubs.c, popf.c, zfrsrvd.c | Declaration/include/explicit callable-type changes, not new instruction algorithms. Verify against selected definitions before reverting for footprint. |
| c_page.c | Added yoda.h selects the PROD check_D macro; compilation selection rather than a new paging algorithm. |
| cpu4gen.h, evid_c.h | Generated declaration and pointer-carrier changes; ABI/layout consistency across consumers must be verified. Not independent execution implementations. |
| fpu.c | stdio declaration and explicit bounded pointer-difference cast in register trace formatting; no FPU arithmetic replacement in the diff. |
| ntthread.c | Diagnostic format and explicit null returns replacing bare returns in pointer-returning failure paths. Not original thread scheduling duplication; null-frame callers still need failure-path review. |
| sascdef.c | Typed null vector-slot cast, not a new SAS invalidation body. Runtime binding is separately covered by the overlay ledger. |

One diagnostic caveat is source-proven in c_seg.c: when selector is zero,
effective_addr and nine sas_hw_at byte reads are evaluated as function
arguments before mvdm_softpc_record_cpu_low_cs_load is called. A runtime
enable check inside that observer cannot prevent those reads. Therefore it
is inaccurate to call this entire caller-side addition default-off work;
whether these reads have guest-visible effects requires SAS address-type
and provider inspection. Do not assert a demonstrated fault from the reads
alone.

Reviewer re-read the original descriptor-validation block following the
0040h special case and verified that it is bypassed by the new else chain.
No descriptor or CPU fix was applied. The inherited full audit scope and
owner approval gate remain unchanged.

## T405 DPMI transition ownership follow-up

Read the full modesw.c diff and the original i386/dpmi386.c transition
bodies, then followed the shadow-table publication in dpmi32.c/dpmiselr.c.
This is not yet full diff coverage of those latter files.

| Functional unit | Original counterpart / local implementation | Audit disposition |
| --- | --- | --- |
| Protected/real transition frames | Original i386/dpmi386.c:switch_to_protected_mode and switch_to_real_mode use the same stack offsets as the new CPU40 bodies in modesw.c. Local code captures the frame before changing PE and applies segment setters afterwards. | Source-derived duplicate transition bodies. Frame interpretation has direct original evidence; passive CONTEXT versus active CCPU cache ordering requires a binding, not necessarily deletion. Original fixed VDM state RI/RM bit updates are omitted and must receive an explicit owner disposition. |
| Native task carrier | cpu40_install_native_task_carrier creates two TSS images, sets backlink and port bitmap, writes busy-TSS descriptors into slots 01f0h/01f8h, and sets TR. | Autonomous platform-state construction absent from the compared original DPMI bodies. Kernel/processor initialization provenance and guest ownership of those slots are unproved. Original task-state logic, if reused, belongs to its genuine source owner; a familiar x86 layout is not proof of original implementation. |
| IDT reconstruction/forced restore | dpmi32.c infers IDT address as published AX segment minus 2048 bytes. modesw.c latches the first nonzero address and sets IDTR to 256 entries; c_intr.c invokes restore on protected interrupt dispatch. | Autonomous reconstruction with a guest-layout assumption. It may override later IDTR state, so compare DOSX publication, accelerated context restore and original interrupt ownership. The latch is a file-static ULONG with no reset assignment in this file, despite its comment calling it session-local; session lifetime elsewhere remains to be checked. |
| GDT/LDT shadow | dpmi32.c installs Cpu40LdtShadowAddress as LDT; modesw.c installs that same address as GDT and writes the two TSS slots there. dpmiselr.c publishes descriptor updates into this shadow. | Distinct guest GDT/LDT selectors alias the same table backing in the local model. This is a substantial table-ownership decision, not merely copying an original descriptor. Slot collisions and original table roles require proof before restoring/removing it. |
| FastWOW TEB projection | dpmi32.c allocates a page, writes Self at 18h and a TD-prefix pointer at c0h targeting offset 100h. | Autonomous guest projection of original TEB/WOW state. The offsets alone do not establish complete original thread/TD lifecycle. Follow FastWOW and original WOW32 owner before calling it an adequate closure. |
| Generic DPMI mode helpers | Local DpmiSwitchToRealMode refreshes six caches instead of the original non-i386 SS-limit adjustment. DpmiSwitchToProtectedMode additionally loads DOSX CS and SS whereas original explicitly leaves segment setup to its caller. | Changed integration semantics; inspect every caller's saved-frame ownership. Do not describe these as byte-equivalent original implementations. |

The copied transition frames and the new TSS/IDT/TEB constructions are
separate review items. No original kernel body has yet been identified as an
exact replacement for the complete local construction; that is unresolved
provenance, not authorization to retain a made-up implementation. Likewise
the current evidence does not prove these mechanisms caused DOSX failure.
Reviewer checked actual table-base assignments and frame offsets rather than
accepting comments about what native NT allegedly inherited. No repair or
additional OpenNT import was performed.

## T405 dpmi32 changed-file coverage

All 13 text-different dpmi32 files in the paired inventory have now had their
complete diffs read. This is direct-hunk coverage, not full closure of the
called kernel/adapter providers. modesw.c findings are above; remaining files:

| Files / unit | Source-level classification and follow-up |
| --- | --- |
| dpmiint.c hardware interrupt guard | New SEGMENT_IS_PRESENT check precedes even the real-mode reflection branch. dpmi32p.h defines this macro against mutable Ldt, whereas CPU tables and DpmiEmulateInstruction use Cpu40LdtShadowAddress. Therefore the new guard's present decision can disagree with the executed descriptor image. Test publication followed by source-table reuse; do not claim that scenario has already been observed. |
| dpmiint.c PM stack publication | Allocates and zeros a guest VDM_DPMIINFO projection and publishes its address instead of original VdmTib.PmStackInfo. This is another autonomous state carrier. DPMI's LockedPMStackCount and related host state remain separate; shared field synchronization and DOSX writes must be traced before asserting lifecycle equivalence. |
| dpmiint.c instruction emulation | Adds null-alias rejection and selects shadow-table Default_Big instead of original Ldt. Original emulation body remains; the table-selection policy differs from the new IRQ guard in the same file. |
| dpmiint.c / dpmiint.h hook selection | Broadens original non-i386 fault/software/hardware-hook composition to CPU_40_STYLE. This reuses original handlers, not a newly invented handler algorithm. Additional fault-frame/registration observations are diagnostics. |
| dpmimemr.c | CPU40 allocation/free/reallocation directly invoke existing SAAllocate/SAFree/SAReallocate, bypassing original Vdm* calls and STATUS_NOT_IMPLEMENTED fallback decision. Query still calls VdmQueryFreeVirtualMemory before overwriting results with SAQueryFree. Confirmed duplicated provider-selection policy, but not a rewritten allocation algorithm. Compare the actual Vdm allocator owner before choosing restoration. |
| dpmi32.c / dpmiselr.c | Shadow allocation/publication, IDT inference and TEB projection described above; remaining deltas are observations and native-pointer carriers. Publication writes copied descriptors to the shadow in addition to original FlatAddress bookkeeping. |
| data.c / dpmi32p.h / dpmidata.h | Storage/declarations for the new guest projections, native-width host addresses, CPU40 transition dispatch and conditional LDT declaration. Not separate counted functional duplicates. |
| buffer.c / int21map.c / xlathlp.h | Pointer-width/range-conversion changes, with original DOS/DPMI behavior bodies retained. Audit selected x86 type equivalence and compilation before deciding footprint-only restoration. |
| xmem.c | Only an added diagnostic header in the paired diff; no new XMS algorithm in this file. |

Reviewer distinguished the original SA fallback body from the added early
selection branches, and checked SEGMENT_IS_PRESENT's actual macro rather
than inferring it used the current CPU shadow. No product changes or guest
execution were performed. None of these bounded conclusions constitutes the
requested all-MVDM/non-MVDM audit completion.

## T405 DEM/VDD SFT ownership follow-up

Read the full demfile.c diff, complete adapter vdd/mvdm_vdd_sft_shadow.c
and original allocation/association/release bodies. This exposes another
confirmed original-owner replacement family, not just an overlay concern.

Original demfile.c GetFreeSftEntry and VDD handle bodies are retained under
`#if 0`; active exported wrappers invoke the adapter's independent SFT/JFT
search, allocation, retrieval and mutation lifecycle. Original source owner
is `base/mvdm/dos/dem/demfile.c`, so restoring this logic should keep that
MVDM owner rather than relocate it to opennt-host.

| Functional point | Original versus replacement |
| --- | --- |
| SFT/JFT allocation | Original finds the free JFT/SFT and immediately writes guest alias state. Replacement copies tables, repeats free-entry search, and returns an uncommitted host shadow. It imposes a 256-group traversal cap. This duplicates original selection policy and changes reservation visibility. |
| Handle association | Original writes the returned live SFT fields immediately. The adapter repeats the same mode/attribute/flags/devptr/NT-handle assignments but changes only a shadow. Exported VDDAssociateNtHandle does not commit it. Existing WOW32 callers now explicitly commit, but an unchanged external VDD API caller has no equivalent original commit operation. |
| Retrieve/release | Original returns live SFT/JFT aliases and release changes one JFT entry/refcount. Replacement creates snapshots and release writes back the complete SFT and complete captured JFT, then frees the shadow. Concurrent or intervening unrelated JFT mutations can be overwritten; actual overlap must be tested before claiming an exercised race. |
| Optional allocation output | Original allocation permits ppSFT=NULL and still allocates. Active wrapper rejects it with ERROR_INVALID_ADDRESS. Confirmed changed public API behavior, not a necessary consequence of typed pointers. |
| Commit failure | Replacement writes SFT first and JFT second; failure of the latter leaves a partially published update. Registry locking protects the shadow list, not a transaction spanning these guest writes. Failure recovery must be compared with original direct ownership. |

Source search confirms modified wow32/wkfileio.c adds explicit commit calls
after allocation/association and release-related writes. Thus this replacement
has propagated into another original mirror component; merely deleting the
adapter would not restore its callers safely. A coherent restoration must
cover DEM, its memory binding and all affected WOW/VDD call sites together.
Runtime export reachability for third-party VDDs remains to be verified.

Separately demfile.c copies open-path input into a bounded MAX_PATH buffer
instead of using the original guest alias; create/attribute paths still use
aliases. Treat this as memory-lifetime/error-policy adaptation, not a second
CreateFile algorithm. The rest of this diff contains diagnostics, handle casts
and original pathname-offset arithmetic with explicit types.

Reviewer verified original optional outputs and immediate field writes against
the actual original bodies, and inspected both commit writes and registry lock
scope. No product changes were made; no corruption or WRITE root cause is
claimed from static replacement evidence alone.

## T405 DEM read and lease boundary follow-up

Read the complete demhndl.c diff and both adapter guest-memory forwarding
bodies, then the complete session/guest_memory_lease.c implementation.
Original owner remains demhndl.c:demRead; host ReadFile/named-pipe/seek policy
is retained, but original alias plus Sim32Flush/Free is replaced by a copied
lease and new failure exits. The same lease mechanism backs the fast-read
overlay already recorded above; do not double-count it as another file reader.

- Acquire always invokes the read provider for nonzero length, even when
  access is WRITE only. Thus an ordinary short read preserves the snapshotted
  tail; a claim that an uninitialized tail is blindly committed is disproved
  by the actual implementation.
- Commit writes the complete requested byte_count, not dwBytesRead. A
  concurrent or nested change to the untouched tail can be overwritten by
  its old snapshot. This requires an overlapping writer; no such occurrence
  is established here. Direct aliases do not add this snapshot rewrite.
- Successful I/O followed by failed commit reports an invalid-address error
  after the host file pointer has advanced. Guest result and host I/O effects
  can diverge. This is a new failure boundary, not proof of DOSX corruption.
- demStoreExtendedError repeats original field assignments through another
  lease. It adds allocation/acquisition/commit failure inside named-pipe error
  handling. Exact GetLastError preservation must be checked through called
  providers, not assumed from the wrapper comment.
- demWrite retains its original alias path; its direct diff adds diagnostics
  and the signed SetFilePointer high-word type. The read/write memory models
  are therefore not uniformly replaced.

The session lease is autonomous generic snapshot/lifetime infrastructure,
while c_sas_loads/c_sas_stores are original providers. This separation is
important for restoration: keeping original SAS access does not establish
equivalence of the new transaction semantics around it. Review original
SIM32 mapping/flush and alias lifetime before deciding how much can be removed.
Reviewer followed allocation, initial copy and complete write-back, rather
than deriving behavior from access flags alone. No product repair performed.

## T405 DEM changed-file coverage

All 12 text-different dos/dem files in the paired inventory have now had
their complete diffs read. demfile.c and demhndl.c findings are above.
The remaining ten classify as follows; this does not close downstream adapters.

| Files | Direct-diff result |
| --- | --- |
| dem.c | Initial system directory comes from the session media-root binding instead of GetSystemDirectory. Product-environment substitution, not a second DOS initialization algorithm. Verify root-provider policy separately. |
| dem.h / demdata.c | Replace durable DTA/current-PDB/extended-error/SFT pointers with numeric guest locations. These support the already-recorded lease/shadow replacements, not four new independent algorithms. |
| demgset.c | Registration records original DS:offset locations and reads DOSWOWDATA through a lease; failed SFT discovery clears the location. DOS drive-list logic is otherwise retained, with DWORD far-link spelling. |
| demdasd.h / dosdef.h | Fixed-width guest-field spelling and handle/search-identity casts. In selected x86 these are footprint/ABI candidates, not proof of a newly implemented file system. |
| demdisp.c | Replaces two unimplemented dispatch slots with fast-read overlay and carry-only fast-write refusal; already counted in the fast-I/O family. Do not count dispatch and provider as separate duplicated services. |
| demmisc.c | Original disk reset still calls HostFloppyReset/HostFdiskReset, but writing the guest access word now goes through the command-state adapter and failure calls TerminateVDM. Added failure policy beyond a pointer substitution; verify registration/lifetime and whether failure can occur. |
| demfcb.c | Original FCB file I/O retained around a DTA snapshot lease. Failed reads commit the lease; ordinary demRead's failed path discards it. This is an explicit inconsistency in local partial-result handling, requiring original flush semantics and named/regular-file failure tests, not an assumed bug. Handle casts are separate. |
| demsrch.c | Original search/list/enumeration algorithms remain. FindFirst/Next now copy and commit DTA after advancing host search state; failed commit can leave host search progressed but guest DTA stale. Current-PDB lookup adds lease-failure exits. Most other hunks retain byte-offset arithmetic at native pointer width. |

Reviewer checked that demsrch.c's native list pointer is still narrowed into
the original 32-bit guest identity field and compared against a host-list
lookup. It is not a newly introduced token-table implementation merely because
the declaration became ULONG. In an x86-only product, claims of x64 pointer
safety in historical comments do not establish continued necessity.

Confirmed restoration families remain source-owned SFT/JFT policy and the
fast-I/O replacement; the lease-based DTA/error/input changes additionally
require coherent original memory-boundary review. Neither text-diff coverage
nor absence of a newly spotted algorithm proves complete semantic equivalence.
No production file was modified.

## T405 COMMAND native-child follow-up

Read complete cmdexec.c diff and targeted native-child capture/activation
helpers plus command_process_compat.h. Original cmdCreateProcess still owns
conversion, process creation, wait, exit and reentry flow, but macros redirect
CreateThread, SetStdHandle and CreateProcess to adapters. Familiar source
spelling is therefore not proof the original endpoints execute.

- DIV-196 copies command/environment/standard handles into session state
  before the detached worker. This is a new ownership mechanism around the
  original worker, not a new worker body. Yet copy_guest_command duplicates
  parsing policy: it accepts NUL as well as CR, while original cmdExec scans
  specifically for CR within 124 bytes and otherwise returns BAD_FORMAT.
  Capture failure now returns INVALID_ADDRESS for multiple failure causes.
- The helper preserves the original CR-to-NUL guest write; it is not a
  read-only snapshot. Environment capture and activation lifetime remain
  transitive audit work. activate returns the stored command/environment
  after checking non-null inputs rather than using their contents.
- DIV-197 calls local BaseSrv child-begin/cancel around thread creation.
  This is the pending/reentry policy already counted in the BaseSrv family,
  not a second independent scheduler finding.
- DIV-149 substitutes child-local STARTUPINFO standard-handle state for the
  original process-global SetStdHandle sequence. Compare complete adapter
  implementation and inherited-handle behavior before judging equivalence.
- cmdCheckBinary retains original classification and command.com /z rewrite,
  but rewrites SCS fields through a snapshot with explicit commit. Registers
  are updated before final commit succeeds; failure consistency must be
  examined with the caller, not inferred from preserved copy order.
- cmdReturnExitCode resolves redirection state through the adapter instead
  of casting the original BX:CX process address. Follow identity lifetime
  and final copy ownership together with redirection implementation.

Reviewer checked std_handles helper zeroes all three outputs even on failure;
the local worker's subsequent accesses must not be reported as uninitialized
array reads on that path. This is negative evidence alongside the confirmed
parser-contract change. No product code was modified and cmdexec.c direct
diff coverage does not close the command adapter family.

## T405 command process adapter follow-up

Read complete win32/source/command_process_compat.c. Its implementation is
not limited to standard-handle forwarding:

- A local parser recognizes an exact environment COMSPEC path followed by
  /c and extracts the tail. Quoted executable names and whitespace are parsed
  autonomously. Every recognized tail is relaunched as the current product
  executable, not handed to the original shell process.
- This is an app/shell launch-routing policy embedded in the Win32 adapter.
  Compare original COMMAND-to-shell and BaseClient process classification
  as distinct owners; no direct replacement body has yet been established
  for the complete local routing policy.
- The static simple_shell_tail helper checks metacharacters but has no call
  site in this translation unit. It therefore does not restrict actual
  interception of pipes, redirection or compound commands. App-entry fallback
  must be examined before claiming those commands work or recurse.
- On the intercepted path, application_name is not passed onward: launch
  constructs a new command line and calls CreateProcessA with NULL application.
  The observed original cmdCreateProcess call uses NULL, so this is a broader
  adapter-contract limitation, not proof of a currently reached failure.
- TLS handle overrides force STARTF_USESTDHANDLES for all three streams if
  any one is overridden. The flags are never cleared in this file; restoring
  a handle value also marks it overridden. Compare per-worker thread lifetime
  and original process-stream inheritance before deciding equivalence.

Reviewer confirmed the apparent simple-tail gate is unused and that standard
handle restoration does not reset override flags. These source facts do not
prove a product launch loop. The routing and stream policies should be
discussed separately from the justified cdecl thread ABI bridge. No repair.

## T405 XMS direct diff and replacement coverage

Read all four xms.486 text diffs and the complete adapter
softpc/mvdm_xms_memory.c. xmsa20.c changes only a blank line; xms.h changes
parameter names/order in a same-typed declaration, not a move algorithm.
xms.c adds a backend selection condition; actual selected macro composition
must be checked before interpreting its historical comment.

The xmsblock.c original xmsMoveBlock body is replaced by an adapter call.
The adapter independently parses the same 12-byte destination/source/word-count
frame, implements chunked copying and adds cancellation on failure. This is
a confirmed original-function replacement, not merely address translation.
Original xmsMoveBlock explicitly distinguishes its RtlCopyMemory operation
from overlap-safe moving; the new helper chooses backward chunks for overlap
and is shared with xmsMoveMemory. Do not claim the original specified a
particular result for overlapping input, but do not call these two original
contracts interchangeable either. The original non-i386 explicit
sas_overwrite_memory notification is absent at this call site; whether the
selected SAS write provider already satisfies it needs separate verification.

The same adapter implements xmsCommitBlock/xmsDecommitBlock by zeroing guest
ranges, with decommit calling commit, and xmsMoveMemory using the chunked
copier. These callbacks require comparison with both original backend
implementations; reserve/commit/decommit and zeroing are not equivalent merely
because subsequent reads may see zeros. Partial chunk failure can leave a
partially changed destination before cancellation is requested.

Reviewer checked actual copy direction and shared callers, distinguishing
the original block-copy entry from the allocator's move callback. No original
overlap guarantee is invented and no observed XMS failure is claimed. No
product changes; downstream callback equivalence remains open.

## T405 XMS callback original-owner verification

Read complete original xmsmemr.c and i386/xmsmem86.c. Original x86 commit
and decommit call NtAllocateVirtualMemory(MEM_COMMIT) and
NtFreeVirtualMemory(MEM_DECOMMIT), respectively. Original RISC callbacks
delegate to sas_manage_xms with operation 1/2. Both original move callbacks
use RtlMoveMemory; the RISC one additionally notifies sas_overwrite_memory.

Followed the selected CPU40 providers: host/src/stubs.c:sas_manage_xms
prints a diagnostic and returns TRUE without allocation/zeroing; the original
CCPU c_sas_overwrite_memory body explicitly does nothing. Therefore:

- The missing explicit overwrite notification is not evidence of a CCPU
  compiled-cache defect. Preserve the API boundary if restoring original
  source, but do not invent a missing functional cache operation.
- The adapter's unconditional range-zeroing is not equivalent to the RISC
  CPU40 stub and cannot be justified simply as restoring that source body.
  Whether allocator clients require stronger guarantees needs a separate
  contract decision. Native x86 commit/decommit uses a different backend.
- Current main generator names the adapter implementation. The
  MVDM_XMS_SESSION_BACKEND macro is explicitly defined in the older bounded
  New-T287OriginalXmsStaticNinja script; its presence must not be inferred in
  the main build merely from the new xms.c conditional.

Reviewer distinguishes original callback implementation from the semantic
requirements of the complete allocator. These checks narrow the earlier
unknowns without asserting a safe whole-file deletion. No runtime or source
repair was performed.

## T405 Redirector direct diff coverage

Read complete diffs for all six text-different vdmredir files. Original
network bodies are largely retained; replacements are concentrated at guest
copy and asynchronous lifecycle boundaries, not wholesale network stubs.

| File | Classification |
| --- | --- |
| vrmslot.c | Removes comments/DBG diagnostics and changes string-length types. Original bitmap allocation loop remains, including its per-word Handle16 reset; do not attribute that inherited logic to the local diff. No new mailslot algorithm found. |
| vrdlcpst.c | Adds thread ABI adapter header; inspect macro expansion/worker ownership separately. |
| vrinit.c | Replaces null-structure field-address expression with FIELD_OFFSET, not a new initialization algorithm. |
| vrremote.c | Replaces direct wide-name strcpy with WideCharToMultiByte(CP_OEMCP), adding conversion failure handling. Deliberate encoding behavior change, not transport forwarding. Compare original NetAPI character contract before deciding restoration. |
| vrnetapi.c | Reimplements CD-name output preparation into three local buffers then adapter composite guest writes, replacing conditional direct destination writes. Computer/user-name APIs similarly use copy adapters and new failure results. Original NetAPI queries remain. Distinguish conversion policy from guest destination ownership; composite partial failure remains to inspect. |
| vrnmpipe.c | Original request queue, worker and ICA flow remain, but request capture/completion now use adapter snapshots. Adds worker-stop/join and request cancellation/free to originally empty VrTerminateNamedPipes, plus release calls throughout. This is autonomous lifecycle policy, not an original implemented cleanup body. |

In added VrTerminateNamedPipes, after worker join the loop calls CancelIoEx,
closes the request event and frees its adapter/request storage without a local
completion wait. Joining the application completion worker is not itself
evidence of OS I/O completion. The adapter's actual buffer ownership and
cancellation completion contract must be checked before judging safety;
static reading has not established an observed use-after-free.

Worker stop initialization also follows CreateThread, and termination invoked
on the worker thread skips joining it. These are review questions requiring
call-site/thread-state evidence, not proof of a race merely from ordering.
The original empty cleanup may have relied on process termination; restoring
it blindly would not satisfy a session teardown contract.

Reviewer kept these new lifecycle mechanisms distinct from original queue
algorithms and excluded removed DBG lines from autonomous-code counts.
All six direct diffs are covered; async/copy adapter semantic closure remains
open. No production change or network execution was performed.

## T405 async pipe adapter ownership verification

Read complete redir/mvdm_redirector_async.c. It allocates a staging buffer
used by the original overlapped I/O submission, retains numeric destination
locations and frees that same staging allocation in async_release. This
confirms that the prior cancellation/free concern involves actual I/O buffer
ownership, not merely diagnostic storage. Outstanding-I/O completion still
needs proof; cancellation request alone is not recorded as completion here.

Additional source-proven differences from original direct aliases:

- Completion writes error and byte-count words first, then validates and
  copies read payload. A failed payload acquisition leaves completion words
  published without the data. Original OS I/O writes the direct buffer before
  the completion routine publishes those words. ANR/ICA delivery is still
  controlled by the retained mirror and is suppressed on helper failure.
- Preparation copies a fixed 24-byte request even for older non-Type2 forms;
  original code accessed the optional semaphore only for Type2. Verify guest
  packet readable extent before treating the fixed capture as equivalent.
- Write payload is snapshotted before submission; read payload copies only
  the actual byte_count on completion, unlike ordinary DEM's full-request
  lease commit. Do not carry DEM's tail-write risk over to this helper.
- Worker stop state is a process-global static atomic, not per-request or
  per-session. It is reset by worker_begin after CreateThread in the caller.
  Reinitialization and session teardown need a combined state audit.

Reviewer traced the returned staging pointer into ReadFile/WriteFile and
verified release frees it. The documented risk is not a claim of a runtime
crash already reproduced. No source repair or live network operation.

## T405 common MVDM header coverage

Read all eight text-different inc headers against original same-path files:
apistruc.h, dossvc.h, intapi.h, mvdm.h, softpc.h, suballoc.h, vdmtib.h and
vrnmpipe.h. Direct-diff classification:

- apistruc/dossvc and guest records in vrnmpipe replace pointer spellings
  with four-byte integers. These are layout/representation changes, not
  independent duplicated algorithms; selected x86 layout tests are needed
  before footprint restoration.
- vrnmpipe's host request also changes flat pointer fields into numeric far
  locations and adds PrivateAsyncState. This is part of the already-counted
  asynchronous replacement, not a guest record field or a separate service.
- intapi types callback pointers explicitly; softpc adds WINAPI to host
  thread entry declarations and widens IntelBase. Cross-definition ABI
  consistency must be verified, not presumed unnecessary after x64 removal.
- mvdm unconditionally selects the original i386 fetch/store macros instead
  of conditional RISC forms. suballoc unconditionally selects 4096-byte
  commitment instead of the non-i386 65536-byte branch. These are composition
  choices with behavioral consequences, not simple include cleanup. CPU40
  build intentionally lacking i386 makes this distinction material.
- vdmtib replaces an absolute source-tree include with the mirrored ABI
  include. No new VDM_TIB declaration or lifecycle implementation in this hunk.

Reviewer separated guest layout from host request layout and identified
allocation granularity as policy rather than a mere type alias. Original
allocator call sites and actual compiler definitions remain part of the
transitive audit. No product edits; eight-header diff reading is not overall
ABI verification or full-task completion.

## Full audit still pending

### BaseClient retained environment bodies

Compared all three current opennt-host/base/win32/client/vdm.c environment
functions with original OpenNT base/win32/client/vdm.c lines 3022--3286.
BaseCreateVDMEnvironment, BaseDestroyVDMEnvironment and BaseGetEnvNameType_U
retain the inspected original branches, path-shortening policy, arithmetic,
conversion order and cleanup. The large deletion count is mostly removal of
other functions and comments, not a newly authored replacement environment
algorithm. Local header/type/tag and BaseSetLastNTError bindings are separate
composition differences. The downstream RTL replacements previously recorded
still change the composed behavior; original caller-body retention does not
prove whole-call-chain equivalence. No build or runtime acceptance claimed.

### Consolidated additional replacement candidates

This table consolidates source comparisons performed after the initial seven
rows. It is a discussion index, not a final total or repair authorization.
Paths below are relative to src unless explicitly identified as upstream.
Product-source files remained unchanged during these comparisons.

| Functional family | Current provider | Original OpenNT owner | Classification and evidence limit |
| --- | --- | --- | --- |
| Image classification | adapter-mvdm-host-out/basesrv/source/mvdm_image_classification.c | base/win32/client/vdm.c:GetBinaryTypeW, plus image-section validation | Own MZ/NE/PE probe precedes the system query: all NE becomes Win16, two-byte PE becomes native. Original distinguishes OS/2 and Win16 and rejects DLL images. Restore classification contract, retaining separately justified CLI search policy. |
| RTL environment blocks | win32/source/opennt_support_rtl.c | base/ntos/rtl/environ.c | Reimplemented mutation, allocation and validation. See detailed comparison below. |
| RTL error translation | same | base/ntos/rtl/error.c and translation tables | Three special cases replace table-driven mapping and special status handling. Original owner restoration candidate. |
| USER string conversion | win32/source/wow_public_user_facade.c:MBToWCSEx | windows/core/ntuser/rtl/chartran.c | Rehost of original conversion/allocation contract; original ACP RTL branch is bypassed. Buffer-short behavior needs focused comparison. |
| Network font tracking | same:AddFontResourceTracking/RemoveFontResourceTracking/UnloadNetworkFonts | windows/core/ntgdi/client/font.c | Duplicate task/path/count list, with changed path classification; detailed comparison below. |
| WOW glyph outline | same:GetGlyphOutlineWow | windows/core/ntgdi/client/dcquery.c and gre/ttgdi.cxx | Lost bIgnoreRotation compatibility policy; detailed comparison below. |
| WOW cooperative scheduling | wow/wow_private_user_compat.c | windows/core/ntuser/kernel/taskman.c | Autonomous 16-slot thread/event scheduler, not original priority/event-count scheduling. r015 links the object, but imports UserRegisterWowHandlers from system USER32 and no current callback assignment to these helpers was found. Classify as linked, unproven reachable, not a demonstrated live scheduler. Cleanup returns success without removing its slots/events. |
| WOW cursor/icon construction | same:legacy DIB cursor/icon helper | windows/core/ntuser/client/clres.c cursor/icon conversion route | Autonomous DIB interpretation and GDI construction; original counterpart located but full format/error comparison remains open. Same callback-binding limitation as above. |
| VDM hard-error dialog | win32/source/wow_hard_error_dialog.c | windows/core/ntuser/server/harderr.c:DisplayVDMHardError | Recreates button decoding/default policy and modal presentation. Original allows no explicit default and takes the last default marker; replacement rejects both cases, hardcodes English labels and omits original CancelId handling. Private desktop/CSR orchestration remains a separate boundary. |
| Debugger startup/dispatch/state | debugger/source/dbg_*.c | base/mvdm/dbg/dbg.c | Rehosted ordinary-profile dispatch and state. Original DBGNotifyRemoteThreadAddress saves state unconditionally; replacement discards it when not debugged. Original SendVDMEvent uses RaiseException, so generic claims that all event delivery requires private CSR are insufficient. Exact outgoing closure remains to be established. |
| App window input conversion | app/presentation_window.c | windows/core/ntcon/server/input.c:GetControlKeyState/ConvertMouseButtonState | Independent window synthesizes Console records; loses modifier-side, toggle and enhanced-key information, accumulates mouse buttons instead of using current message state. This is not the native Console input path. |
| Worker lifecycle | softpc/mvdm_softpc_execution.c, session/session.c and mirror nt_reset.c | base/mvdm/softpc.new/host/src/nt_reset.c and original entry | Original CPU entry retained; ExitVDM/ExitProcess lifetime replaced with explicit teardown and session longjmp. Group with existing thread/termination changes, not a second emulator. |
| Resource lookup | softpc/mvdm_softpc_firmware.c and mirror nt_unix.c | base/mvdm/softpc.new/host/src/nt_unix.c:host_find_file | Rewrites path/existence lookup. Package-relative root is product policy; preserving original lookup/error owner remains a separate decision. |
| Redirector OEM guest copy | redir/mvdm_redirector_guest_copy.c | ds/netapi/netlib/copystr.c:NetpCopyWStrToStr | Original already mirrored; adapter converts independently. VrGetUserName checks character count while replacement copies OEM byte count, requiring DBCS-capacity proof. Group conversion logic with RTL without double-counting shared helper code. |

The win32/source and softpc/ etc. provider paths in this table are under
adapter-mvdm-host-out unless explicitly prefixed app/session. The separate
physical mapping and effective-address cache-selection algorithms remain
source-owner-unresolved: matching original declarations/callers are not proof
of an available matching implementation. Do not classify them as confirmed
duplicates or safe deletions on this evidence alone.

Mechanical waiting also needs a distinct semantic finding: nt_wait_compat.c
maps sufficiently large finite timeouts to MAXDWORD, the Win32 INFINITE
sentinel. NtAlertThread instead prefers the native NTDLL entry and only falls
back to APC; do not label its native path an autonomous alert implementation.

Duplicate source carriers and inactive helpers are tracked separately below.
No family count in this table supersedes the all-root inventory or establishes
full transitive closure. Reviewer checked the classification against observed
source calls/build rules, separated missing capability from duplicate behavior,
and retained uncertain reachability rather than assigning speculative faults.

### Mirror build-carrier direct-diff reconciliation

Re-read paired-diff-001/paired-diff.csv grouped by parent path and compared
the remaining obj.vdm/sources and obj.vdm/obj/i386/ntvdm.def rows using
git diff --no-index --ignore-cr-at-eol against each recorded source.
The former removes monitor.lib with a profile explanation; it adds no runtime
algorithm. The latter adds the Sim32FlushVDMPointer export to the supplemented
definition file. Its concrete CPU40 no-op provider was separately read in
wow_sim32_pointer_compat.c and must not be counted as a second pointer manager.
These two carrier hunks do not prove every generated build selection correct.
The semantic audit still needs its all-root coverage reconciliation and final
per-functional-unit disposition; direct hunk reading is not that closure.

### WOW glyph-outline compatibility loss

Current wow_public_user_facade.c::GetGlyphOutlineWow directly calls
GetGlyphOutlineA; wgdi31.c:303 is its mirror caller. Original OpenNT
windows/core/ntgdi/client/dcquery.c::GetGlyphOutlineWow passes TRUE for
bIgnoreRotation to GetGlyphOutlineInternalA; the public A form passes FALSE.
Tracing that flag to gre/ttgdi.cxx proves it suppresses LOGFONT escapement and
orientation when recomputing the font transform for Win3.1 compatibility.
The original comment identifies rotated-text printing as a consumer. Thus
the replacement loses a specific source-visible compatibility policy; its
comment claiming equivalence is not sufficient evidence. No rotated-text
runtime test or WRITE-causality claim was made. Recovery must locate a bounded
way to retain this policy without importing the full GDI server.

Also compared wow32_public_api_adapters.c against wow_public_user_facade.c:
only the two include lines differ, with identical function bodies. The r015
graph selects the latter. Count functionality once and the former as a
duplicate source carrier, not a second active GDI implementation.

### Additional inactive monitor replacements

Read monitor/source/vdm_control.c, monitor/source/host_idle.c and
monitor/mvdm_wow_task_frame.c completely. The task-frame helper duplicates
CallBack16's selection of vpStack/vpCBStack, word alignment and initialized
flag in a separate TLS record. Current wcall16.c retains the original TD-based
branch and does not call that helper. Searches of src/tests/tools found task
frame binding and begin-callback consumers only in fixtures. Neither retained
r014/r015 graph contains its compile rule. Count it as an inactive duplicate,
not as evidence of two simultaneously executing WOW stack owners.

The empty HostIdleNoActivity is likewise not selected by those graphs; the
selected original nt_unix.c retains its own implementation. NtVdmControl is
selected in r014, but its VdmQueryDir callback binding has only a test caller
in the searched source set. Without that binding its code explicitly returns
STATUS_NOT_IMPLEMENTED. This is an unavailable capability, not a recovered
directory-query algorithm. Original kernel-owner dependency tracing remains
necessary before proposing restoration. No source edit or runtime execution.

### Additional WOW font-tracking comparison

Read current `wow_public_user_facade.c` and original OpenNT
`windows/core/ntgdi/client/font.c` functions AddFontResourceTracking,
RemoveFontResourceTracking, RemoveFontResourceEntry, UnloadNetworkFonts and
bFileIsOnTheHardDrive. Current wgfont.c calls the add/remove providers and
wkman.c calls unload with CURRENTPTD as owner identity. The retained r015
WOW32 graph selects the facade. This is a duplicated task-owned font list and
load-count algorithm, not only a public GDI binding.

Original code stores Unicode paths; the replacement stores MAX_PATH ANSI
paths and adds a critical section. Original treats every UNC path as remote
and classifies drive types other than removable/fixed/CDROM/RAMDISK as
non-local. Replacement tracks only paths for which GetDriveTypeA reports
DRIVE_REMOTE, including an extra UNC-root query. Classification is therefore
not identical for unknown/unavailable drives or shares.

Both implementations load the font before allocating its tracking node and
return the successful load result if tracking allocation fails. Both discard
the tracking entry after the requested number of unload attempts regardless
of individual removal failure. These are retained behaviors, not newly
introduced defects; do not attribute them uniquely to the replacement.

Executor comparison and sequential reviewer used full function reads and rg
of mirror callers. No font was installed or removed. Restore the original
list/policy through bounded allocation, path and public GDI bindings if the
owner approves; composability has not yet been demonstrated by a build.

### Additional RTL environment comparison

Read the complete current `opennt_support_rtl.c` and compared the validation,
deletion and return paths of original OpenNT `base/ntos/rtl/environ.c`
`RtlSetEnvironmentVariable` (starting at line 458). This is a duplicated
environment-block algorithm, not only a heap/API binding. Original allocation
uses virtual-memory services; current allocation uses the process heap.

Source-proven differences: the original rejects an equals sign after the first
name character, supports an omitted Environment argument through the process
PEB, and retains STATUS_VARIABLE_NOT_FOUND for an absent-name deletion. The
replacement does not perform that equals-sign validation, rejects a NULL
Environment argument, and returns success after rebuilding even when deletion
found no matching name. These differences are not claims that current normal
COMMAND input exercises those cases. Current cmdenv.c calls the replacement
with an explicit NewEnv pointer.

The replacement also allocates just one WCHAR when deletion leaves no entries,
but its own environment-size scanner subsequently reads two WCHARs to identify
the empty block. That is an internal allocation/reader contract mismatch;
runtime reachability and a focused fixture remain unverified. Do not infer
WRITE's failure from this static finding.

Separately compared original `base/ntos/rtl/error.c` with the current
RtlNtStatusToDosError replacement. Original table lookup, customer/HRESULT and
OS/2 cases, unknown-code result and TEB LastStatusValue update are not retained
by the replacement's three special cases and ERROR_INVALID_PARAMETER default.
Restoration discussion must consider the original table-driven owner and a
same-shaped native binding before accepting this reduced algorithm.

Procedure: Get-Content for current provider and bounded original function
ranges; rg for current cmdenv callers. Sequential reviewer checked the concrete
return and allocation paths. No product modification, build or runtime test.
These are additional verified points, not completion of all-root coverage.

For every selected functional unit, record original path/function, current
provider, actual build selection, unavailable outgoing interface, semantic
differences, retain/replace/delete decision and regression workload. Count
functional units separately from files and physical added/deleted lines.
Compare selected upstream against both mirror and overlay; do not label all
adapter lines hacks. This initial pass is not a full COMMAND/EDIT closure
audit and supplies no whole-project total.
