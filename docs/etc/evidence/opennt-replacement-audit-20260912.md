# Original-owner replacement audit and T405 discussion handoff

## T405 combined static-audit conclusion

The S1-S3 inventory and disposition audit is ready for owner discussion.
This conclusion supersedes the incremental statements below that the combined
report or file-disposition join is still pending; it does not erase their
specific unresolved semantic findings. Historical initial-pass baseline text
below is retained as chronology, not the current T405 input identity.

Current audited product source is the frozen 3d127962c baseline, including
unaccepted 1daff0ace WIP, not an accepted WRITE implementation. All 5043 tracked
inputs are accounted for by the S1 inventory. Direct changes, overlays,
patches, adapters, app/session/broker and declaration carriers have the
dispositions recorded here. Retained compile graphs are selection evidence
only; no new build, guest test or product repair was performed.

The confirmed original-function overlap discussion set is D01-D37 below.
It includes partial overlap, inactive providers and changed selection of an
original algorithm; it is NOT 37 proven bugs, 37 independent replacement
implementations, or an estimate of safe deletions. Unknown source matches
remain explicitly unknown, so no assertion that these are all possible
semantic duplicates is made. The eight unresolved families have named holds
and comparison requirements in their table, rather than invented matches.

Proposed discussion order is Base command policy (D01-D07/D10), DEM/VDD and
fast I/O (D08/D31-D33), Console/input/presentation (D09/D23-D25/D28-D30/D34),
RTL/NetLib (D11-D16/D26), WOW/debugger (D17-D22/D27), then DPMI/XMS
(D35-D37) and unresolved internal-state changes. This is not repair approval.
Original MVDM owners stay in mvdm-host; eligible non-MVDM original bodies
belong in opennt-host; adapters retain only demonstrated missing mechanics.

No whole provider is declared safely deletable solely from name, size or
successful COMMAND/EDIT runs. Pure casts/blank lines, thin original forwarding,
inactive duplicates and diagnostics are footprint candidates subject to
paired ABI/build and affected regression checks during approved restoration.
The requested next step is individual owner discussion. Stop implementation
here; standing commit/push permission does not approve source restoration.

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

#### Overlay-wide proposed disposition and regression boundary

Re-read all five C bodies and both headers against the above original-owner
findings. The following completes the physical overlay list's proposed
dispositions, not approval to implement them:

| Carrier | Proposed disposition | Required proof before delivery |
| --- | --- | --- |
| demfastio.c | Replace duplicated fast-read policy using the finite original NTFastDOSIO slice, or disable only the optimization through original CF fallback after owner choice. Retain explicit fast-write refusal unless that original service is separately recovered. | Ordinary DEM fallback, seek/no-seek, EOF/short read, standard/pseudo handles, failure after read and guest commit; DOSX/COMMAND file workloads. Keep CPU40 BOP advancement outside the imported kernel trap prologue. |
| localfm.c | Retain bounded global/vector-layout composition until an original carrier with the selected single Sas owner composes. Do not introduce a second global just to remove an overlay file. | Exactly one Gdp/Cpu/Sas/Video owner and matching layouts across original CCPU/C-VID consumers; selected x86 link and video regression. |
| sas_overwrite_memory.c | Original wrapper reuse, not autonomous implementation. Prefer an original-path registered source subset if feasible; otherwise retain this exact forwarding body with its collision rationale. | One exported provider, original c_sas_overwrite_memory selected, no alternate state providers; SIM32/EMS memory call regression. |
| mvdm_cvidc_vector_binding.c/.h | Retain necessary vector assembly provisionally; separately resolve unproven timing-state substitution and generator null/overridden slots. Do not classify the whole binder as a duplicate algorithm. | Complete public/private slot signatures and selected targets, initialization ordering, separate calibration/restart semantics, video and timer/PIC workloads. Missing original timing backing remains an explicit unresolved finding. |
| mvdm_ica_eoi_bridge.c/.h | Retain or inline the smallest same-shaped typed boundary after owner review; no PIC arbitration algorithm is replaced. | Original rotate=0, input/output line propagation, x86 int/LONG representation, Redirector/PIC EOI regression. Null guard is extra boundary behavior, not proof of a normal call difference. |

The only current mirror caller found for mvdm_ica_eoi_bridge is
nt_eoi.c:SoftPcEoi; original SoftPcEoi passes the same adapter/line with
rotate=0. The overlay README's plural caller description also names nt_timer,
but that timer uses its own scalar remainder change, not this EOI bridge.
The README register omits demfastio.c/DIV-188 despite that body being selected.
Record both as source-register cleanup items; comments are not authoritative
proof of implementation coverage. These findings add no duplicate D row:
fast read is D08, original wrappers/declarations are not additional algorithms,
and timing remains unproven rather than a fabricated confirmed duplicate.

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

## S2 bounded conclusion and S3 handoff

S2's MVDM changed-family coverage is complete for the frozen source inventory:
the twenty groups in the numerical matrix below have per-file/family reviews
and proposed dispositions in this record. This includes all 174 text-different
MVDM paths (+4379/-1273), all seven overlay source/header carriers and all five
adopted patch carriers. S1 separately accounts for equal, CR-only, relocated,
supplemented and unpaired inputs; they are not omitted from source identity.
The host/src and WOW32 tables complete the last direct-diff disposition joins.

Sequential reviewer reran Join-OpenNtRetainedGraphCoverage.ps1 into
build/M0-T405/S2/coverage-review-001: all 5043 tracked input hashes and membership
matched, with 505 unique selected paths and 508 retained compile edges. Git
diff against the frozen 3d127962c source baseline is empty under src. Runtime
ntvdm32.exe remains SHA-256
27F8D7B5BB074838E9484877954A38DCBCFA0BE64282A4A2D05C776867655107.
These checks prove unchanged inputs and compile membership, not link or guest
execution. No repair or runtime acceptance was performed.

The handoff explicitly retains unresolved semantics: CCPU event/descriptor
changes, DPMI TSS/IDT/profile setup, C-VID timing, physical mapping, callback
failure cleanup and session lifetime. Their disposition is to hold behavioral
changes for original-owner comparison and owner discussion, not to declare
them equivalent or removable. Necessary ABI bindings, diagnostics, inactive
carriers and confirmed overlap remain separate categories.

S3 must consolidate non-MVDM original owners and autonomous providers with
these findings, resolve or explicitly bound outstanding owner matches, and
produce the combined discussion ledger. The 37 D rows below remain a verified
subset until that reconciliation; S2 closure is not the requested final audit
report and does not authorize repairs. Documentation delivery is pending.

### S3 declaration and cross-root count reconciliation

Completed the previously excepted mvdm_softpc_termination.h read. It contains
declarations/comments, not additional inline behavior. Its observer promises
remain subject to the already reviewed implementation and call-site reads;
for example stream_io_update takes a pointer despite its older scalar-only
comment. This closes that header-read exception without adding functionality
to the duplicate count. The earlier nine composition headers and 209-file
ABI provenance reconciliation remain the declaration evidence.

Recomputed inventory root totals with documentation/binary files included:
adapter 112, adapter-opennt-host 1 (README only), app 10, broker 7, guest 2597,
MVDM host 1977, overlay 8, firmware 42, patch 6, tools 48, opennt-abi 210,
opennt-host 20 and session 5: 5043 tracked paths. Source-bearing file counts
exclude README/metadata: overlay 7/245 lines, patch 5/1868, adapter 103/11654,
app 9/1594, broker 6/433, session 4/1065, opennt-abi 199/80463. These source
line totals are physical footprint, not changed or duplicated code totals;
209 ABI source-directory files additionally include non-source carriers.

Paired MVDM changes remain 174 text-different files, +4379/-1273. Paired
opennt-host changes remain seven files, +316/-14011; large subset removals
must not be presented as deleted hacks. Overlay/adapter additions are counted
separately because a no-file baseline is not an original function comparison.
Guest, tools and firmware retain their separate provenance/no-host-link
dispositions and are not silently dropped from the all-root inventory.

### S3 independent presentation completion

Completed presentation_window.c in bounded reads, joining D23 input synthesis
and D34 palette findings. The file implements a separate User32 window and
text rasterizer, not the native Console text path. Its text renderer assumes
two-byte cells, fixed glyph dimensions/font source and a fixed EGA palette;
it ignores blink behavior and does not draw an independent text cursor in the
inspected painting loop. These limitations are not attributed to original
SoftPC rendering or the accepted native Console mouse route.

The window creates its own thread, changes Console input mode, writes synthetic
events to CONIN$ and restores its saved mode on exit. Mouse state accumulates
down/up messages and the window procedure has no capture-loss reset branch.
Closing requests session cancellation; Alt+Enter destroys only this surface.
Startup waits five seconds, but close can then wait indefinitely even if a
window handle was not yet published and no shutdown message was posted.
Record this as a conditional lifecycle risk, not an observed hang.

Proposed disposition: keep this optional presentation boundary distinct from
native Console restoration. Recover proved original input/palette policy
through minimal bindings, and discuss the custom rasterizer/media assumptions
explicitly rather than silently substitute it for the requested Console.
No window was opened, no input injected and no product code changed.
All five app C files and both session C files now have explicit composition
dispositions, completing the implementation-file join with the three broker C
files. Declaration and final cross-root count reconciliation remains separate.

### S3 session lifecycle and storage disposition

Completed session.c's lifecycle, binding, presentation and root sections in
bounded reads, joining the previously read memory wrappers. This is local
composition infrastructure, not an original OpenNT state machine under a new
filename. Its lifecycle replacement remains a distinct source-recovery concern.

Disposal rejects nonzero binding count or an armed escape, then runs void
teardowns in reverse registration order, clears presentation/leases and zeroes
the instance. Teardown failures cannot be propagated through those void slots.
Cancellation/completion change state without themselves performing process-wide
termination. Binding counters are atomic, but that does not prove atomicity of
state transitions, hook lists, borrowed context ownership or presentation data.
Restore original process-lifetime guarantees through a minimal composition
boundary; do not infer thread safety solely from the counters.

The single session escape has no recorded arming-thread identity check on
termination, matching the earlier cross-thread longjmp risk. Presentation
storage allocates/copies buffers and palette values; it is not the original
Console Server object/ownership model and joins D34 and the independent-window
boundary. Root setters and backend validation are app assembly mechanics,
not additional DOS loading algorithms. Diagnostics and unused budget metadata
remain separate footprint candidates. No new confirmed D count is introduced.

### S3 copied guest-memory lease disposition

Read guest_memory_lease.c completely and session.c's begin/end/acquire/release
wrappers. Every nonempty acquisition reads a full bounce buffer, including
WRITE-only requests. Commit writes the full recorded range; end discards all
outstanding buffers without commit. Release frees and clears its record even
when the backing write fails. These are local transaction semantics, not the
original live GetVDMAddr alias contract, and explain why SFT/callback/DEM
restoration must review the shared layer as well as each caller.

Release checks active/epoch fields but not membership of the supplied record
in this context's array. Epoch changes on begin, not on reuse of a slot within
the same context. Thus the API alone does not distinguish an old record pointer
from a later allocation in that same slot. This is a conditional ownership
limitation, not evidence that a current caller retains such a pointer or that
an exploit exists. The session wrappers add active-session checks but no
additional record-membership or per-allocation identity check.

Proposed disposition: retain only required finite guest-memory mechanics and
restore each original caller's field/range/ordering contract. Do not introduce
a generic pointer manager or count this transport as another DOS file system.
Required regressions include overlapping writes, write-only access, failed
commit, teardown with outstanding leases and stale-slot reuse. No guest memory
was read or modified by this audit.

### S3 app entry and machine-shell disposition

Read entry.c and machine_shell.c completely. entry classifies before session
activation; resolved non-DOS/non-Win16 targets go to native process launch,
unresolved commands to host shell launch. Resolution failure is intentionally
not an immediate error. That fallback belongs with D10 and public CLI policy,
not an original DOS loader replacement.

For guest execution the order is package validation, synthetic SoftPC argv,
process environment projection, backend/presentation preparation, session
activation, Base record bind/publish, then original-entry execution. Cleanup
frees argv and restores the process environment before presentation close and
session disposal. A disposal failure replaces the prior result with 72;
presentation-close failure replaces only a zero result. Preserve these as
explicit app lifetime/error-policy differences from original process exit,
and review them with outstanding worker bindings before assuming cleanup
is equivalent. No observed race is claimed from ordering alone.

machine_shell.c validates a SoftPC session and forwards to
mvdm_softpc_execution_run_original_entry; it contains no CPU loop or guest
loader. Its nonzero ips/memory parameters are explicitly unused. They are
obsolete interface-footprint candidates, not working capacity controls or
proof that a second emulator is present. Proposed disposition: retain original
execution ownership, minimize shell ceremony, and preserve the positional CLI
while restoring source-owned launch/exit policies where applicable.

### S3 app launch and package policy disposition

Read launch_declaration.c and package_layout.c completely. The CLI grammar,
executable-relative package root and historical buffer-fit admission are app
policy, not original MVDM implementations to copy wholesale into a mirror.
Image classification delegates to D10; one-shot exhaustion and command record
publication join D01-D07 rather than introduce new duplicate counts.

Launch declaration also builds a restricted COMSPEC/PATH environment, uses
code page 437, assigns task/broker identity 1, supplies pure-DOS PIF only for
DOS, and constructs -f/-w/-a in the in-memory SoftPC argv. These explicit
composition choices are not original BaseClient behavior merely because the
record shape matches. Preserve positional product entry while reconciling
original environment, startup metadata and PIF ownership. The generated argv
does not itself change the process GetCommandLine value.

Argument handling is asymmetric: a sole argument is copied as a command-line
string, while multiple argv entries are individually quoted; a quoted path
containing spaces arrives from CRT without its quotes. Later target splitting
therefore needs end-to-end resolution evidence before claiming uniform path
support. This is a static grammar concern, not a reproduced launch failure.
The package validator enforces the original short-path/SHELL capacity and does
not create a shortened path; keep that explicit admission restriction separate
from the resource-lookup algorithm D25. No executable or guest was started.

### S3 broker composition disposition

Re-enumerated composition roots: app has five C files, broker three and session
two, with their paired headers and READMEs separately inventoried. Read all
three broker implementations completely for this join.

- broker.c owns a finite client registry, monotonically assigned IDs and
  equality checks on supplied user keys. It contains no authentication or IPC;
  equality is not proof of an authenticated caller. Keep this project contract
  distinct from original BaseSrv process registration and any future transport.
- wire.c validates fixed fields and a nonzero key. has_native_value always
  returns zero based on record design; it does not inspect integer payloads for
  native values. This is not a runtime security guarantee or an OpenNT provider.
- base_vdm_record.c owns the restricted one-pending-record-per-slot model,
  DOS/WOW owner filtering, capacity and disconnect clearing. It is autonomous
  Base-protocol policy and joins D01-D07, not neutral serialization alone.
  consume compares request_id after locating broker/session but not full record
  contents; the publishing adapter uses request_id 1. Do not infer robust
  stale-request protection from this field without a sequencing contract.

Proposed disposition: restore original Base command policy as an owner package
with the local adapter and record layer considered together; preserve only the
required finite copied-record mechanics. The independent registry/wire fixture
does not prove a resident broker exists and does not authorize implementing
one in this audit. Header shapes were reviewed in the earlier declaration pass.
No registry state, process or product source was changed.

### S3 unresolved findings and counting boundary

The following consolidates the current unresolved findings instead of turning
each new observation into another duplicate count. These remain inside the
audit scope; a hold is a disposition, not proof of equivalence or a repair.

| Family | What is established | What remains unproved / next decision |
| --- | --- | --- |
| C-VID binder timing | Local calibration/restart state and generator slot substitutions exist. | Exact original backing and selected slot contracts; retain provisionally, not as verified original timing. |
| Physical mapping | Local record/alias ordering, exact-span unmap and byte resolution are fully visible. | Original matching implementation and required overlap/deferred-backing contract. Not a confirmed duplicate count. |
| Effective-address cache selection | Original descriptor decoding is reused, with local cache preference. | Original choice rule for equal selectors with distinct caches and required failure contract. |
| DPMI platform setup | Transition-frame overlap D35 and allocator selection D37 are established separately. | Ownership of TSS slots, IDT construction, GDT/LDT sharing and TEB/stack projection. Do not label all of this original kernel reuse. |
| Keyboard normalization | Local synthesis plus partial original paste overlap D24. | Complete modern Console packet contract and surrogate/modifier behavior; original paste policy alone is not a drop-in replacement. |
| WOW private callbacks | D19/D20 source-owner overlap is established. | Helper runtime reachability and full cursor/icon format/error equivalence. Link selection is not execution proof. |
| Session lifetime and memory transactions | Concrete changed failure/cleanup paths are recorded in callbacks, environment, disk, thread and leases. | Which are exercised by selected workloads and which original process-lifetime guarantees must be restored; no wholesale second session framework is prescribed. |
| Native wait/Console capabilities | Explicit unsupported services and observable wrapper differences are recorded. | Selected caller reachability, source-owned policy versus unavailable server mechanics; do not count missing implementation as duplicated implementation. |

The 37 D rows remain the established functional-overlap set, with carrier
files, subcases and shared helpers not counted again. Declared source reuse,
ABI forwarding, diagnostics, unsupported returns and unresolved provenance
are separate from those rows. Before the final combined report, reconcile
app/session/broker composition and declaration inventories against this set;
do not promote the count to an exhaustive total from filename coverage alone.

### S3 remaining named carrier reconciliation

The final filename-presence pass found five carriers whose functions were
already discussed without exact filenames. This check locates indexing gaps,
not proof of semantic completeness. Their explicit dispositions are:

| File | Evidence and disposition |
| --- | --- |
| mvdm_softpc_vdd_configuration.c | Complete read recorded in the small-carrier pass: registry-read binding and missing-key policy; group with nt_msscs. |
| mvdm_softpc_vdd_unavailable.c | Complete read recorded there: ClearInstanceDataMarking controlled failure, not original VDD recovery. |
| wow_callback_frame_lease.c | Complete read in WOW32 pass: finite guest-memory lease wrapper; transaction failure changes are recorded against original CallBack16. |
| wow_user_callback_callconv.c | Complete read in WOW32 pass: four WINAPI thunks into original bodies, not four new USER policies. |
| mvdm_base_vdm_environment.c | Complete reread: calls original BaseCreate/DestroyVDMEnvironment, but installs projection into the current process rather than passing it to a newly created child. Retain original projection owner; review temporary process-wide mutation separately. |

Environment restore ignores SetEnvironmentStringsW failure, then frees saved
blocks and clears its state, leaving no retry information if restoration
fails. This is a source-level lifetime/failure difference, not evidence of
observed environment loss. Recovery should preserve failure reporting and
process ownership, without replacing the original projection algorithm or
conflating it with D11's downstream RTL environment implementation.

All 52 adapter C filenames now have an explicit carrier or family disposition
in this record. This closes that indexing gap only; S3's combined functional
ledger and unresolved-original-owner conclusions remain required before the
requested final audit report. No product code was changed.

### S3 physical-mapping policy disposition

Completed the remaining mvdm_softpc_physical_mapping.c read, including
prepare/set, alias map/unmap, translate/resolve and cancel. Unlike the SAS
forwarders, this file owns an autonomous record/alias policy. Exact original
implementation matching remains unresolved; original names do not make it a
verified recovered kernel-VDM service.

Map updates only an exact destination-base/size match; otherwise it prepends
a record without rejecting partial overlaps. Translation selects the first
matching alias and performs one translation, not recursive alias resolution.
Unmap removes only an exact span and otherwise returns STATUS_NOT_FOUND.
The source range is checked for arithmetic overflow but not for the existence
of backing storage at map time. These are concrete current policies to compare
with original ownership, not claims that overlap or deferred backing is
necessarily invalid. Resolve checks the individual byte against source_size;
prepared page padding is not exposed as valid source bytes by that check.

Disposition: hold for source-owner reconciliation and preserve current
regression baseline; do not classify as a confirmed duplicate or delete it
because the generic x64 identity manager was retired. Required comparisons
include overlapping spans, exact versus partial unmap, alias chains, backing
retirement, mapping publication failure and owner teardown. The earlier
prepare/set transaction and list ownership observations remain applicable.
No mapping was created, changed or exercised during this audit.

### S3 debugger four-file disposition

Read dbg_dispatch.c, dbg_init.c, dbg_state.c and dbg_unavailable.c completely
and rechecked original dbg.c SendVDMEvent/DBGNotifyRemoteThreadAddress bodies.
All four belong to D22, not four additional duplicated-function counts.

| Carrier | Disposition |
| --- | --- |
| dbg_init.c | Rehosts debug-port query and original success/failure policy with native export resolution. Restore original DBGInit ownership with a narrow query binding where necessary. |
| dbg_dispatch.c | Reimplements the ordinary-profile switch, including TOOLHELP metadata and mode-specific AX behavior; attached-debugger paths become unavailable. Restore original dispatch with bounded stack reads rather than maintaining parallel mode constants and reduced branches. |
| dbg_state.c | Separate process-global debug/toolhelp state duplicates original dbg.c ownership. Migrate with the dispatcher; helper names alone do not establish original consumer integration. |
| dbg_unavailable.c | Mixed no-event and controlled-stop substitutes. DBGNotifyRemoteThreadAddress drops state even when not debugged, unlike the original unconditional stores. Retain genuinely unavailable operations only after a precise outgoing-interface audit. |

The original SendVDMEvent uses RaiseException with STATUS_VDM_EVENT and catches
unhandled delivery to return FALSE. This directly contradicts a blanket claim
that every event requires private CSR transport; it does not prove complete
debugger recovery, because context acquisition and consumers remain separate
dependencies. Proposed source-first restoration must retain that failure
direction and original state stores, naming each genuinely unavailable edge.
No debugger attachment, exception delivery or product edit was performed.

### S3 event-thread and guest-memory binding disposition

Read mvdm_softpc_event_thread.c and mvdm_softpc_guest_memory.c completely,
plus nt_remove_event_thread and ConsoleEventThread cleanup. Event handling
remains in the original worker; the adapter duplicates a synchronization
handle, alerts the original handle and waits indefinitely on the duplicate.
The retained handle protects the join from the worker's own CloseHandle, but
the alert still uses the original handle. The caller discards the helper's
failure result. Therefore this is a lifecycle binding with a failure/close
race to review, not proof of guaranteed teardown or a new input dispatcher.
Do not fix it by changing original mouse/button processing.

Guest-memory callbacks perform overflow-safe RAM-size bounds checks and call
original c_sas_loads/c_sas_stores. Allocation, copied lease ownership and
commit semantics belong to session; SAS remains the memory-operation owner.
The callbacks return success after a void SAS call, so bounds validation alone
does not establish equivalent behavior for every ROM/device/mapped span.
Keep memory-type semantics and error propagation in the regression boundary,
without inventing a second memory manager from these forwarding functions.

The physical-mapping implementation remains separately classified as local
record/alias policy with unresolved exact original owner. Reading its record
publication confirms process-global lists and session-pointer ownership, not
an epoch-tagged generic identity manager. This observation does not resolve
its original semantic equivalence or authorize removal. No worker was run,
signaled or killed in this audit.

### S3 descriptor and effective-address carrier disposition

Read mvdm_softpc_descriptor_fields.c and mvdm_softpc_effective_address.c
completely. retrieve_descr_fields delegates descriptor reading/decoding to
original read_descriptor_linear and narrows outputs to the historical helper
types. It is source reuse through a binding, not an independently implemented
descriptor decoder.

The effective-address provider is mixed: it calls original descriptor helpers,
but independently chooses real/v86 calculation or protected-mode cache lookup,
preferring CS, SS, DS, ES, FS, GS in that order before walking GDT/LDT. Equal
selector values do not by themselves prove equal cached bases, so selecting
the first matching cache is a policy requiring original-owner justification.
Its base-plus-offset result does not check segment limit or access rights;
do not advertise it as validation of an entire guest span. Conversely, that
omission is not proof an address-calculation-only historical API required those
checks. The scalar c_effective_addr facade maps failure to zero, which must
not be confused with a distinct success status for guest address zero.

A focused search of original ccpu386 found the cpu4gen declaration/macro,
not a matching C implementation; original host sim32 supplies a different
sim32_effective_addr entry. This bounded negative search does not prove no
original implementation exists elsewhere. Keep cache-selection ownership
unresolved, as already recorded, rather than labeling this entire file either
pure forwarding or a confirmed removable duplicate. Required review includes
same-selector/different-cache states and caller-specific failure handling.
No CPU instruction or product source was changed.

### S3 redirection identity carrier disposition

Read mvdm_redirector_handle.c and mvdm_command_redirection.c completely,
then current cmdredir call/free sites and original cmdredir.c's AX:BX record
decode and BX:CX handle publication. The original x86 path already passes
32-bit native values in guest registers. Current helpers recombine/split those
values and reject zero; they do not maintain an identity table, lease, epoch
or independent pipe implementation. The retire helper is empty and original
free still owns the record. Its mirror comment claiming session-identity
retirement is stale after identity-manager removal.

Proposed disposition is source-shape simplification, with explicit treatment
of added zero rejection and error returns, not introduction of a replacement
manager. Preserve original pipe/resource lifetime and the sole x86 contract.
A nonzero value is not validated as a live pointer or handle by these helpers;
do not describe them as safety validation. mvdm_redirector_handle_from_words
has no direct caller in the inspected MVDM-source search; selected object
membership alone does not establish its runtime use. This thin-carrier set
does not add another confirmed duplicated redirection algorithm.

### S3 CRT and explicit-unavailable carrier disposition

Read crt_compat.c, wow_private_unavailable.c and
mvdm_redirector_remote_unavailable.c completely. crt_compat forwards sprintf
and sscanf variadic arguments to UCRT with default options and unbounded
capacity; it contains no replacement format parser. Retain a toolchain ABI
binding, with historical format compatibility a separate verification need.
It does not supersede original fprt.c's printf/fprintf owner.

wow_private_unavailable.c is mixed, despite its name: ShowStartGlass is a
deliberate presentation omission, whereas NtRaiseHardError first invokes the
locally implemented recognized-dialog route (D21) and only then falls back to
controlled unavailable termination. Its introductory assertion that hard-error
handling remains wholly unavailable is stale. Neither the filename nor that
comment may exclude its active replacement policy from the audit.

The Redirector file returns explicit failure for RxpTransactSmb,
RxNetUserPasswordSet, GetLanmanSessionKey and eight Xs handlers. It does not
implement another RAP client or report success. Preserve explicit missing
capability separately from duplication; the recorded private transport boundary
does not authorize a replacement protocol or claim those workloads work.
Output buffers are not filled on these paths, so callers must honor failure.
No additional confirmed-duplicate count is assigned to these failure stubs.

### S3 Base command carrier reconciliation

A filename-presence check identified records using family names without exact
carrier names; absence of a filename is not itself absence of review. Read
base_vdm_broker.c completely and completed mvdm_command_guest_state.c in
bounded reads. The combined reread of mvdm_command_native_child.c was truncated;
its earlier native-child analysis remains the evidence, not that incomplete
output. These three names join the existing six-file Base adapter denominator.

base_vdm_broker.c adds bounded record copying, session registration/teardown
and local delivery around broker records. It is not original srvvdm.c and
does not implement a standalone listening process. Delivery publishes into
base_vdm_local before consuming the broker record; consumption failure returns
an error after destination publication. Group this transaction ordering with
D01-D07 rather than count a second independent GetNextVDMCommand algorithm.

mvdm_command_guest_state.c stores original SCS/ToSync/binary/fd-access locations
in TLS, checked against session and memory epochs. Its advance_real_mode accepts
linear addresses through 10FFEF, then narrows linear>>4 to uint16_t. At or above
100000 that canonicalization wraps the segment; the earlier allowed range is
not preserved. This is a source-proven conditional address-conversion issue,
not proof current SCS addresses reach it. scs_field also does not compare its
byte_offset with the stored scs_bytes. Retain as boundary-validation findings
for caller/range review, not evidence of a new DOS state machine or permission
to change CCPU addressing. FD access read/modify/write remains two separate
leases; its failure ordering joins the disk-close findings already recorded.

Proposed disposition: keep original COMMAND/DEM scalar owners; minimize their
finite transport while preserving high-memory representability, field bounds
and transaction failure behavior. No code repair, broker activation or guest
execution occurred.

### S3 wait and thread binding reconciliation

Read complete nt_wait_compat.c, nt_thread_alert_compat.c and
thread_start_compat.c, then original base/win32/client/synch.c's wait return
handling and base/ntos/ob/obwait.c's invalid-handle path. The native wait
returns STATUS_INVALID_HANDLE for that case; the public wrapper converts
failed status to WAIT_FAILED/LastError. The current reverse wrapper maps
every such failure to STATUS_UNSUCCESSFUL, losing the original distinction.
Original public WaitForMultipleObjectsEx also retries STATUS_ALERTED, so
wrapping the public call cannot generally expose native alert semantics.
This is in addition to the previously recorded finite-timeout-to-INFINITE
conversion. Do not claim an exact NtWait contract from identical signatures.

The two inspected nt_timer multiple-wait calls are under MONITOR, outside
the selected profile. They are not proof of active heartbeat exposure to this
wrapper. Preserve the interface finding and establish selected consumers
before treating it as a current runtime failure. A native same-shaped service
binding is the first restoration candidate; importing the kernel object/wait
scheduler is not a proposed solution.

Thread-start thunks preserve original callbacks and add heap context plus
session TLS binding. Creation can succeed while a later bind failure prevents
the original callback from running; source_name and owner are borrowed
pointers. A suspended thread delays their use. Record ownership and startup
failure visibility with session lifetime, not as a new original scheduler.
Likewise NtAlertThread forwards natively when available and only otherwise
queues an APC; its fallback is not a general pending-alert implementation.
Review teardown, delayed start, binding failure and unrelated APC separately.
No threads were created or stopped and no product code changed for this audit.

### S3 adapter implementation denominator

Re-enumerated tracked adapter-mvdm-host-out C inputs: 52 files, partitioned
into basesrv 6, debugger 4, monitor 4, redir 5, softpc 12, vdd 3, win32 14,
wow 4. This is implementation-file coverage, not 52 duplicate algorithms.
The retained graph union selects 48; the four inactive C carriers remain
explicitly included in the canonical-path review below.

The small-carrier cross-check read mvdm_a20.c, mvdm_guest_location.c,
mvdm_softpc_presentation_font.c and both VDD configuration/unavailable files
completely. Their classification complements, rather than duplicates, the
large-provider findings:

- A20 wrappers delegate to original c_sas_* providers. Retain narrow binding;
  they do not independently implement the wrapping mask.
- Guest-location helpers marshal real-mode segment:offset and little-endian
  values through session memory leases. C-string copying refuses to advance
  past offset FFFF; DEM filename and COMMAND COMSPEC callers reach it. This
  is a finite boundary restriction requiring caller-capacity/segment-end
  verification, not proof of equivalent original unchecked-pointer behavior.
- Presentation font snapshot copies a fixed C3990 address through c_sas_loads
  for app/presentation_window.c. It is an independent presentation assumption,
  not original Console font selection or native Console mouse handling.
  Retain as explicit unproved media/font-selection coupling pending review;
  a valid session and fixed buffer size do not prove the current guest font.
- Installable VDD configuration reads the original registry location without
  creating it. Missing key/path becomes NONE; other errors remain errors.
  Group this policy with nt_msscs, not as a new VDD emulator.
- ClearInstanceDataMarking explicitly marks unavailable and requests session
  termination. It is missing capability, not recovered original cleanup and
  not a successful no-op. Its termination mechanics join the lifetime review.

No new confirmed duplicate D row follows from these five files alone. The
remaining family reconciliation must preserve the distinction between original
delegation, marshaling, policy replacement, unsupported behavior and diagnostics.

### S3 opennt-host nineteen-file disposition

Reconciled every non-README opennt-host row in paired-diff-001 with the
revalidated retained graph. Twelve files are byte-equal and seven text-different
(+316/-14011). Most deleted lines are excluded original declarations or
unselected routines, not removed replacement logic or a measure of hack size.
Read the complete six small changed carriers again; BaseClient retained bodies
are covered by the earlier environment comparison in this record.

| Files | Identity and disposition |
| --- | --- |
| basedll.h; base.h, basemsg.h, basevdm.h; basesrv.h, srvinit.c, srvvdm.c, srvvdm.h | Eight byte-equal Base carriers. srvvdm.c/srvinit.c are not selected in the retained compile union; original service presence on disk does not supersede D01-D07's adapter policy. Retain original owner while planning a finite same-shaped binding, not a whole CSR service import. |
| client/vdm.c | +135/-3242 source subset: original environment projection functions, not the full BaseClient process-creation entry. Selected by retained graphs. Keep retained bodies and separately assess installed-environment binding and autonomous classification D10; do not claim CLI launch is original CreateProcess merely because this subset is linked. |
| netlib/allocstr.c, copystr.c, initoem.c, ntstatus.c | Four byte-equal selected original units. D26 identifies overlap in a separate guest-copy helper, not corruption of these files. Preserve original conversion/allocation ownership and audit their outgoing RTL bindings separately. |
| api/apibuff.c | +10/-215; retains NetapipBufferAllocate forwarding to public NetApiBufferAllocate. No second allocator policy in the retained body. Preserve subset and public failure contract; excluded RPC/MIDL routines are not a missing runtime requirement solely because the source has fewer lines. |
| sdk/inc/ntexapi.h, ntpsapi.h | Selected declaration subsets. NtAlertThread has conditional local import decoration; its native-first/APC fallback implementation and heartbeat exit policy are separate audit points. Headers do not implement Ex or process services. |
| sdk/inc/ntrtl.h, nturtl.h | Declaration/macro subsets, with source-facing spelling and compatibility guards. Retain only required ABI after provider reconciliation; D11-D16 and path/heap/time bindings must be classified at their bodies rather than credited to these headers. |
| sdk/inc/ptypes32.h | -2 blank lines around the DEFINITIONS heading, no behavior change; verified with a fresh one-context diff. README's byte-exact claim needs identity wording correction during approved cleanup; no new portability implementation is established. |

The retained graph selects six original implementation files (client vdm,
apibuff and the four NetLib units), not the BaseSrv state machine. README's
older blanket statement that this package is not an enabled build input is
therefore stale for those six files. This is a documentation finding, not
authority to enable srvvdm.c or delete its active replacement. S3 still needs
the all-provider classification and combined owner-discussion conclusion.

### S2 numerical coverage reconciliation

Recomputed all text-different MVDM rows from paired-diff-001/paired-diff.csv,
including the one supplement export change. These are physical changed-file
counts and added/deleted lines, not autonomous-function counts:

| Original owner group | Files | Added | Deleted |
| --- | --- | --- | --- |
| dos/command | 10 | 663 | 130 |
| dos/dem | 12 | 414 | 103 |
| dpmi32 | 13 | 652 | 53 |
| inc | 8 | 64 | 29 |
| softpc.new/obj.vdm | 3 | 17 | 2 |
| softpc.new/base/bios | 3 | 19 | 4 |
| softpc.new/base/ccpu386 | 23 | 469 | 68 |
| softpc.new/base/comms | 1 | 8 | 6 |
| softpc.new/base/cvidc | 8 | 63 | 30 |
| softpc.new/base/dos | 1 | 32 | 24 |
| softpc.new/base/inc | 11 | 86 | 15 |
| softpc.new/base/keymouse | 3 | 127 | 25 |
| softpc.new/base/support | 2 | 44 | 5 |
| softpc.new/base/system | 5 | 63 | 10 |
| softpc.new/base/video | 10 | 178 | 78 |
| softpc.new/host/inc | 7 | 99 | 47 |
| softpc.new/host/src | 34 | 1067 | 382 |
| vdmredir | 6 | 180 | 200 |
| wow32 | 10 | 112 | 26 |
| xms.486 | 4 | 22 | 36 |
| Total | 174 | 4379 | 1273 |

Joining these 174 paths with canonical graph-coverage-002 yields 124 selected
compile inputs, 48 non-direct-translation-unit entries and two unselected C
files: base/ccpu386/ntstubs.c (+14/-1) and host/src/nt_aorc.c (+46/-71).
All remain within semantic review. In particular, ntstubs has an overlay
carrier, so not compiling that original translation unit does not prove its
functionality is unused. The 48 other entries include declarations and build
controls; their category does not mean unused. Supplement export selection is
documented separately above. Byte-equal, CR-only and unpaired source-fragment
categories remain in S1's denominator, not lost or counted as text diffs.

This matrix verifies arithmetic and selection coverage only. The per-owner
semantic dispositions and cross-family restoration requirements must still be
reviewed against the findings below before S2 closes; table completeness is
not proof that every changed behavior has a restoration decision.

### Unselected nt_aorc carrier disposition

Re-read the complete +46/-71 diff of host/src/nt_aorc.c against its selected
OpenNT counterpart. Most edits replace K&R/default-int register setters with
IU8/IU16/IU32/IBOOL/ISM32 prototypes; register assignments and dispatch bodies
are retained. Two added setIP/setCS hooks collect current register values
before dispatch and report low transfers. The report provider returns early
when disabled; argument reads still occur. These are diagnostic/ABI changes,
not a newly authored register emulator.

The retained product graphs omit this file, but
tools/build/Generate-T335S3WowCallbackCcpuNinja.mjs explicitly compiles and
links it into the historical callback fixture. Thus it is not globally dead
code and cannot be deleted just from the product-union result. The current
product generator instead names original accessfn.c as the complete register
bridge; review the selected profile rather than adding both providers.

One concrete declaration hazard needs profile-level reconciliation:
setSS_BASE_LIMIT_AR changed from void to IBOOL without returning a value.
The c_reg.c implementation it calls is VOID, while original cpu4gen.h already
declares that CCPU operation IBOOL and original accessfn.c expects an IBOOL
vector result. This is an original carrier/profile mismatch plus a local
signature change, not evidence that CCPU segment operations are incorrect.
Do not invent a TRUE return or alter segment semantics without identifying
the selected declaration/provider/caller contract. Proposed disposition:
retire unnecessary mirror prototype/diagnostic footprint only together with
the fixture binding decision, retaining original register operations. Any
replacement must check callback fixtures and the product register-vector ABI;
no runtime failure or successful repair is claimed by this static finding.

### Base video ten-file review and disposition

Re-read all ten base/video diffs (+178/-78); the truncated combined output's
egwrtm12 portion was re-read separately in full with zero context. Classify:

| Files | Changed responsibility and proposed disposition |
| --- | --- |
| cga.c, ega_prts.c, ega_read.c, ega_writ.c, vga_prts.c | Primarily original-provider declarations and typed callback signatures, including word-fill IU16 rather than IU8. Retain the selected provider's actual contract; remove only redundant declaration footprint after compilation proof. These hunks do not introduce replacement raster algorithms. |
| ega_vide.c | Two memset4 destination casts change ULONG* to unsigned int*. Resolve the selected function signature; do not label this as new scrolling logic. |
| egawrtm0.c, egwrtm12.c | CPU40-specific conditional selection excludes legacy string movers and zero-initializes legacy handler storage. It changes composition, not the retained byte/word writer bodies. Pointer-width spelling in fill_both_bytes is separately a footprint candidate on x86. Duplicate divergence comments are cleanup debt. |
| gfx_updt.c | Dirty-mark callbacks change boolean-to-void declarations/assignments and remove simple_update's unused FALSE result; excludes legacy vid_handlers in CPU40/C_VID. Verify consumer signatures with gfx_upd.h; original dirty increment and marking policy remain. |
| video.c | Adds a declaration and MVDM_STANDALONE_SAS_VIDEO exclusions to three MONITOR-only direct writes. With MONITOR absent, these added predicates do not change those branch outcomes; do not infer a new text renderer from them. |

Reviewed original selection at ega_write_init and ega_write_routines_update:
CPU_40_STYLE calls SetWritePointers, and Glue_set_vid_wrt_ptrs excludes the
legacy handler dereferences under that profile. Consequently zero-initialized
legacy tables alone are not proof of a selected null callback. The C-VID
provider table itself still needs the separately recorded slot/profile review.
Do not restore dormant ULONG-native-pointer movers solely because x64 was
retired; prove the single selected video provider and avoid duplicate routes.

Required package regression: EGA mode/chain changes, byte/word/fill/move
dispatch, dirty marking and BIOS text output, then native Console COMMAND/EDIT.
Console presentation alternatives remain D09/D23/D28--D30/D34, outside these
original raster bodies. No additional confirmed duplicate algorithm was found
in this ten-file direct-diff pass; no rendering or compile test was run.

### C-VID eight-file direct-diff disposition

Re-read all eight base/cvidc differences (+63/-30). gdpvar.h adds one blank
line; sevid019.c and sevid020.c remove trailing whitespace only. Their
physical numstat rows are not generated-rule algorithm changes. j_c_lang.c
adds stdio and j_c_lang.h declares the original CrulesRuntimeError provider.
evidfunc.h replaces implicit declarations with four-IUH rule signatures;
sevid001.c's SimpleMark and CGAMarkString bodies confirm that actual generated
calling shape. evidgen.h types four callback forms formerly carried as IHP;
cross-definition layout/signature consistency remains a profile requirement.

ev_glue.c has the overlay binder call, stdio/stdlib declarations, wider outer
BIOS-write arguments and host_malloc-to-malloc spelling. Selected host_def.h
already defines host_malloc as malloc, so that last edit is redundant source
footprint under this binding, not a second allocator or a proved allocator
policy change. The binder is the already-accounted overlay family, not another
duplicate counted at its mirror call site.

The BIOS callback width change must not be declared wholly harmless from its
type alone: former IU8/IU16 arguments narrowed before assigning jccc_parm2;
new ULONG arguments assign that global before entering the chain2_evid rule.
Do not infer narrowing from the function-pointer type. Check consumers of the argument carrier
and actual caller values before restoring/narrowing this boundary. This is a
specific unresolved ABI/dataflow check, not a newly found raster algorithm.

Proposed disposition: remove whitespace/redundant spelling footprint in a
coherent approved source cleanup; retain proven generated ABI declarations or
move them to the smallest source-shaped declaration seam after compile proof;
review binder/timing/slot semantics as one CCPU40/C-VID composition package.
No speculative rule-body edits and no additional D count arise from this pass.

### BIOS, device and support twelve-file disposition

Re-read the complete compact diffs for base/bios (3), base/comms (1),
base/dos (1), base/support (2) and base/system (5): +166/-49 in total.

| Files | Disposition and boundary |
| --- | --- |
| bios/reset.c | Two CPU40 continuations no longer add HOST_BOP_IP_FUDGE (-2). This is an instruction-entry contract change, not a new reset algorithm. Preserve the established DOSX direct-target distinction during any cleanup; compare guest continuation producers and CPU40 live-IP semantics, never blindly reintroduce the offset. |
| bios/tape_io.c, bios/virtual.c | Adds original XMS and ClearInstanceDataMarking declarations. The latter provider is the admitted patch boundary; these hunks do not implement A20 or VM teardown again. |
| comms/com.c | Native pointer carrier and byte-pointer type changes at original host serial calls. On x86 these are footprint/type-binding candidates, not replacement UART logic. Restore only with the matching host_com_ioctl/read signatures. |
| dos/emm_mngr.c | Host backing IDs change long to IHP and backfill to ULONG; original handle search/allocation remains. RECOVER_LIM_WRITEBACK selects existing original writeback bodies. Reconcile pointer/host allocator declarations, but do not call the existing EMS manager an autonomous mapping-manager reimplementation. |
| support/main.c | Adds early video-vector binding and original load_sw_cpu_access_functions after CPU initialization. Reuses original providers; initialization order remains a composition obligation with the overlay, not a second CPU entry algorithm. |
| support/xt.c | Excludes alternate CPU-state/table definitions in CPU40. Require one original state owner before undoing the predicate; duplicate storage is not restoration. |
| system/cmosnt.c, system/idetect.c | Whitespace only, and original vdm.h include-path binding respectively. No new hardware or idle algorithm. |
| system/ica.c | Scalar diagnostics around retained EOI, already-high suppression and IRQ selection. The original scan result is still tested with bit 80h. Diagnostics should be separable from production but do not justify replacing original PIC arbitration. |
| system/illegalp.c | Added effective-address and SAS reads execute as observer arguments, even when the observer later declines output. This joins the existing diagnostic-side-effect finding; do not describe all observer work as disabled at the call site. |
| system/rom.c | RECOVER_ROM_RESIDENCY selects the retained original generic ROM loading body. String-length typing/caching is separate footprint. Original resource lookup substitutions remain D25, not a new duplicate loader in rom.c. |

The current generator explicitly adds RECOVER_ROM_RESIDENCY and
RECOVER_LIM_WRITEBACK on their compilation rules. Their presence is selection
evidence, not proof that the complete ROM/EMS boundary is equivalent. Required
regressions cover DOSX continuation targets, ROM residency/protection, EMS
writeback, serial ABI, startup ordering and PIC events. No new confirmed
duplicate algorithm is added by these twelve direct diffs. The diagnostics
and composition constraints remain visible rather than dismissed as harmless.

### Keyboard/mouse three-file disposition

Re-read all keymouse diffs (+127/-25) and the complete kb_setup_vectors body.
keyba.c retains the original scan/admission/8042/EOI decisions, adds scalar
diagnostics and the original XMS declaration, and routes reset through
cpu_interrupt(CPU_HW_RESET,0) instead of mutating the private event bitmap.
That producer change belongs with the CCPU reset-consumption review; it is not
a new keyboard-controller algorithm. Preserve reset wake semantics and do not
reintroduce a second CPU event store just to remove a binding.

keybd_io.c changes guest-address-to-host-pointer construction, plus observers.
The non-MONITOR branch never increments pkio_table; its original entries 12,
22 and 27 therefore remain the same byte offsets after using phy_base and
sizeof(word). MONITOR's table iteration is a separate branch. This excludes
the suspected moving-table-index regression in the selected non-MONITOR path.
Pointer lifetime/range still requires the source-shaped memory-boundary review;
correct index arithmetic does not prove an unbounded alias safe.

mouse_io.c retains button/cursor/call-mask logic, changes host range parameter
types, adds observations and enters the registered user callback at its direct
offset for CPU40. The callback change is already the mouse BOP-entry finding,
not another mouse packet implementation. Reversing it solely to match the old
non-CPU30 predicate would discard the explicitly established live-IP/direct
entry distinction. Keep original mouse policy and put Console input/absolute
coordinate integration at the host boundary.

Proposed package verification: scan make/break/repeat, 8042 reset and IRQ/EOI,
NTIO table entries, mouse move without button transitions, press/release,
callback save/return and EDIT menu/selection behavior. Diagnostics should be
isolated without altering those original branches. No new confirmed duplicate
algorithm is found in these three mirror diffs; host input replacements remain
in their separate ledger entries. No test or product repair was run here.

### SoftPC eighteen-header disposition

Re-read the full zero-context diffs for all eleven base/inc and seven host/inc
text-different headers (+185/-62). They are accounted for individually here:

| Headers | Original/current contract and proposed disposition |
| --- | --- |
| ckmalloc.h, host_def.h declaration additions | stdlib and host_simulate declarations; preserve valid declarations for the selected CRT/provider, not implicit-int behavior. |
| cpu_vid.h, gmi.h, egacpu.h, gfx_upd.h | CPU40/C-VID callback signatures, VGA globals pointer view and marking return types. These are companion changes to the already reviewed video bodies; decide as one layout/signature package, not four new algorithms. |
| emm.h, host_emm.h, host_com.h | Host storage-pointer and ioctl carriers, plus USEBLOCK parentheses. Reconcile x86 provider signatures and expression semantics; no second EMS or UART manager appears here. |
| host.h | memset4 declaration changed to unsigned-int parameters/pointer, paired with video casts. Confirm the implementation's type before reducing footprint. |
| ica.h | Suppresses obsolete interrupt-mask macros under CCPU so they do not replace the original CCPU event enum. Audit with the reset producer/consumer package; an enum collision is not fixed by restoring both definitions. |
| sas.h | Physical length becomes PHY_ADDR and declares the original overwrite wrapper. Match the selected memory-size definition, not an x64-era comment; no memory allocator body added. |
| timeval.h | host_time is declared with long rather than CRT time_t. Match original 32-bit seconds provider and modern CRT use; retiring x64 alone does not prove modern x86 time_t is the old ABI. |
| host_cpu.h | Concrete original register getter/setter signatures and result types. Review alongside nt_cprgs/accessfn/generated declarations; retain the known original operations rather than inventing adapters for every register. |
| host_def.h profile removal | Removes implicit CPU_30_STYLE selection. This is an explicit product-profile boundary, not removable clutter; no revival of the kernel V86 monitor is authorized. |
| insignia.h | IHPE and conditional CVIDC_RULE_WORD IUH use UINT_PTR. Check all selected generated layouts before restoring original spelling; same x86 width does not by itself verify every function-pointer type. |
| nt_event.h | Supplies existing BOOL external declaration; not a new mouse attachment state machine. |
| nt_fdisk.h | Removes durable pFDAccess alias declaration; group with the disk/guest-access replacement and teardown analysis. |
| nt_inthk.h | Uses original intapi.h typed callback contracts instead of local PVOID declarations. The hook implementation and registration ownership, not this header alone, decide functional fidelity. |

No independent duplicated algorithm is added by these header hunks. Several
have effective ABI/profile behavior and are not assumed harmless. Proposed
cleanup must compile all affected original consumers under the one x86 CCPU40
profile and verify slot sizes/signatures, serial/storage carriers and hook
registration. This is source comparison, not that compile/layout proof.

### Host-source disposition: fifteen supporting files

Re-read the complete diffs for these fifteen of host/src's 34 changed files;
the table accounts for each rather than generalizing from adapter names:

| Files | Proposed disposition and verification boundary |
| --- | --- |
| config.c | Format/tagged-value casts are footprint candidates. Removing the conditional memory-sizing branch forces the original i386 calculation rather than RISC GetVDMSize/extension policy; this is retained original logic with changed selection, not a new allocator. Reconcile CPU40 memory profile and PIF/XMS/EMS/DPMI capacity before restoration. |
| fprt.c | Bounded strlen cast and removal of pointer-through-DWORD round trip. Original CRT wrapper logic remains; no second file-output policy introduced by these hunks. |
| nt_com.c | Pairs the native ioctl carrier with base/comms and host_com.h; baud rate still enters a DWORD, worker adapter identity casts are explicit. Preserve original serial ordering and validate the paired ABI. |
| nt_emm.c, x86_emm.c | Original storage allocation/free/reallocation bodies retained with IHP carriers. nt_emm includes the physical-mapping adapter: that service's implementation must be audited separately, not hidden under this type-only description. |
| nt_fulsc.c | Moves original getNtScreenState outside the X86GFX block without changing its body. This is source reuse, not recovery of private fullscreen switching. |
| nt_hosts.c | Enables mouse/extended flags and disables QuickEdit. This is host Console input policy needed for delivered mouse behavior; keep it at the boundary and verify mode restoration, rather than alter original guest mouse logic. |
| nt_inthk.c | Original handler storage/registration gets typed callbacks. Registration behavior is retained; interrupt dispatch composition elsewhere is a separate finding. |
| nt_lpt.c, nt_umb.c | Whitespace-only differences. Do not report a new printer or UMB implementation from these rows. Printer kernel-service availability is separately documented. |
| nt_pif.c | Package-relative CONFIG.NT/AUTOEXEC.NT lookup replaces system-directory defaults while retaining the PIF override branch. Group with D25's resource/package boundary; preserve missing-file and buffer-capacity behavior. |
| nt_sec.c | Error argument becomes literal zero on the null-handle failure branch; retain the actual branch meaning and remove only redundant footprint after review. |
| nt_sound.c, nt_thred.c | Adds original lock and CCPU thread declarations; no sound or scheduling algorithm replacement in these hunks. |
| stubs.c | Diagnostic formatting and whitespace only. Original sas_manage_xms success/no-op behavior predates the local diff; do not label it a new fake implementation. |

The remaining nineteen host/src files include the already recorded CCPU
bridge, BOP, disk/lease, resource, input/presentation and lifecycle findings;
their per-file disposition join remains required. No new D count is introduced
by this supporting set. This review did not compile, execute or alter sources.

### Host disk and memory four-file disposition

Re-read complete nt_fdisk.c, nt_rflop.c, nt_mem.c and sim32.c diffs, then
the changed close bodies and physical-mapping prepare/set/cancel providers.

- nt_fdisk/nt_rflop retain original disk operations but replace PDB/access
  aliases with guest reads and fallible counter updates. After CloseHandle and
  invalidation, failed guest decrement skips fdisk_open_count decrement; the
  floppy path additionally skips owner/auto-lock clearing and density_changed.
  This is a confirmed conditional cleanup-order difference, not an observed
  leak or justification for reverting to unchecked pointers. Opening has an
  explicit rollback, so do not describe opening and closing as symmetric.
- nt_mem retains its original allocation headers/list algorithm with native
  address arithmetic changes. VdmAdd/RemoveVirtualMemory adds prepare/cancel
  and zero-host-address removal behavior. The adapter's set operation can
  activate a prepared record or remove a matching active guest span, and its
  void result cannot report publication failure to the caller. This is a
  physical-binding policy review, not just an x86 pointer cast; the original
  exact backing implementation is still unresolved in the existing ledger.
- sim32 selects the previously reviewed original CCPU descriptor walker via
  a narrow effective-address adapter, rather than reviving CPU30 decoding.
  Its allocation-failure exit now uses session termination and returns NULL
  when termination does not escape. Review with session/thread ownership,
  not as an independent CPU or memory allocator replacement.

Proposed disposition: retain original disk and allocator bodies; restore
source-owned cleanup ordering through a finite guest-access failure contract;
resolve physical-binding owner and transaction semantics before deleting its
adapter; retain the original CCPU walker. Regression needs failed guest counter
publication after host close, reopen/heartbeat counts, PDB access failure,
alignment/size overflow, map/unmap matching, allocation failure and selector
errors. No destructive disk I/O, guest execution or repair was performed.
These four reduce the remaining host/src disposition join from nineteen to
fifteen; they do not add invented confirmed-duplicate counts.

### Host BOP and CCPU bridge three-file disposition

Re-read each complete CR-normalized zero-context diff individually against
the paired OpenNT source recorded in paired-diff-001; combined output had
previously been truncated and was not evidence of review. These three files
account for +229/-70 physical lines, not 229 lines of replacement algorithms.

| File | Original-owner comparison and proposed disposition |
| --- | --- |
| nt_bop.c (+54/-6) | The original DEM, WOW loader, COMMAND and NTIO dispatch remains. Added observation calls and explicit export/ordinal casts do not constitute a second BOP dispatcher. Remove diagnostic footprint from production only with the separate observer contract reviewed; do not infer state neutrality from comments. On the sole x86 profile, evaluate redundant UINT_PTR/FARPROC changes together with declarations rather than claiming every cast is x64 functionality. |
| nt_cprgs.c (+101/-17) | Register-vector storage and initializer assignments retain the original providers. Typed getter/setter slots and the CPU_40_STYLE selection of the existing CCPU branch are composition changes, not an autonomous register machine. Retain the original provider branch; reconcile each declaration against generated provider types before shrinking the diff. Do not restore a CPU30 build profile to make the old conditional select it. |
| nt_cpu.c (+74/-47) | CCPU owns the SAS globals; local duplicate definitions are excluded. host_simulate retains its original execution body and host_unsimulate forwards to original c_cpu_unsimulate. InitNtCpuInfo replaces hard-coded GDP offsets with generated fields and CleanedREC.EIP, with explicit register-cell casts. Keep wrapper/selection mechanics separate from GDP layout changes; generated field width and addressability need paired-profile verification before any offset restoration. |

No new confirmed duplicate point is added: the dispatch/executor owner remains
the original implementation. Required regression boundaries are export lookup
failure and ordinal calls, vector initialization before first register access,
register widths and segment-loader return values, nested simulate/unsimulate,
and GDP field layout. Source inspection does not prove those runtime tests.
Together with the preceding four disk/memory rows, this leaves twelve host/src
files to join to their existing detailed semantic findings. No product source
or build selection changed.

### WOW32 ten-file direct-diff disposition

Read all ten complete CR-normalized diffs against their paired OpenNT paths:
+112/-26 lines. Also read the four USER callback wrappers and the complete
callback-frame lease provider; these are distinct from private USER policy.

| Files | Proposed disposition |
| --- | --- |
| isvwow.h, wgdi31.c, wkman.h, wucomm.c | Calling-convention declarations/definitions; preserve x86 stack ABI, reconcile matching providers before reducing footprint. No replacement algorithm in these hunks. |
| precomp.h, wow32.h | Include/export declaration selection and removal of the native _X86_ register-alias selector for software CPU composition. Audit the supplied private providers separately; a header binding does not prove their behavior. |
| wkman.c | Debugger declaration architecture bracket and WINAPI thread signatures, not a rewritten task scheduler in this file. Scheduler facade findings remain separate. |
| wow32.c | Four WINAPI wrappers delegate directly to W32EmptyClipboard, SetFakeDialogClass, WU32ICBStoreHandle and W32DDEFreeGlobalMem32. Retain ABI mechanics; do not count them as four autonomous USER algorithms. |
| wkfileio.c | Joins D31-D33 SFT/JFT shadow ownership. New fallible commit calls change failure ordering after original mutations; ignored undo-commit failures and close-time publication must be reviewed with the shared provider, not counted as another file subsystem. |
| wcall16.c | Original callback construction and host_simulate remain; lease acquisition/publication adds early returns. The callback-stack flags/address are mutated before the second acquire; failure returns without rollback. The two release calls are joined by short-circuit OR, so a failed first release skips the second. Classify as a changed failure contract requiring transactional cleanup, not a replacement callback executor. |

The lease provider can return failure and clears its view after underlying
release; this does not itself establish that the first release can fail in a
valid runtime callback. The skipped second call is a source-level conditional
finding, not an observed leak. Required regression boundaries include invalid
selector, second-acquire failure, each release failure, nested callback return,
SFT commit/undo failure and callback calling convention. No guest execution,
repair, or proof of complete WOW32 runtime availability is claimed.

### Host-source disposition join completed

The remaining twelve rows are joined below to the detailed findings in this
record. Re-read the complete nt_eoi, nt_error, nt_graph, nt_mouse and nt_reset
diffs for this join; the other seven have complete-diff reviews recorded above
or below. Counts come from paired-diff-001, not estimates of removable code.

| File | Added/deleted | Disposition and original-owner boundary |
| --- | --- | --- |
| nt_aorc.c | 46/71 | Previously reviewed inactive product carrier, selected by a historical fixture. Typed accessors retain original providers; the setSS return-type mismatch is unresolved, not permission to invent a return value. |
| copy_fnc.c | 21/7 | D15: local fill loop overlaps the original RTL fill operation. Resolve count units and forward-pointer movement before original-body restoration. |
| nt_eoi.c | 19/1 | Original EOI/delay decisions remain; pair the int/LONG bridge with its already audited overlay. Diagnostic additions are separate. No second PIC algorithm in these hunks. |
| nt_error.c | 39/20 | Original dialog/thread sequence remains. Calling-convention and callback signatures must remain ABI-correct on x86; TLS and HWND carrier edits are separate footprint candidates. This is not the independent hard-error facade D21. |
| nt_event.c | 146/7 | Autonomous scan-code/modifier normalization plus observers and lifetime hooks. D24 captures the proved partial overlap; the whole normalization policy still requires its original Console owner comparison. Retain original downstream keyboard/controller logic. |
| nt_graph.c | 33/9 | Private graphics-buffer creation becomes session presentation storage; close adds its release. Preserve original raster writers, review buffer ownership separately from palette D34, and isolate stream observers. No restoration of private Console Server is implied. |
| nt_mouse.c | 10/4 | Range-pointer declarations and observation only. Original position/button processing remains; do not count it as autonomous mouse semantics or undo the separately accepted callback/BOP repair. |
| nt_msscs.c | 48/15 | Preserve the explicit no-host-system-file-mutation exception and original software-CPU branch. Registry absence policy requires its documented boundary, not wholesale original installer behavior. |
| nt_reset.c | 43/8 | Original process exit/ExitVDM becomes guarded host cleanup plus session termination. This is a changed lifecycle policy, including suppression of repeated close, not merely an API alias. Resolve one-worker process ownership with the session/thread finding before restoration. |
| nt_rez.c | 40/5 | D25: embedded ROM and resolved retry paths replace original resource routes. Preserve provenance and failure behavior, including the current lack of file fallback after the embedded branch fails. |
| nt_timer.c | 20/2 | Broadened APC heartbeat exit belongs to lifetime restoration review, independently of declaration/diagnostic changes. Test unrelated APC and intended shutdown separately. |
| nt_unix.c | 43/46 | D25 resource lookup plus changed PulseEvent/SetEvent idle behavior. Original surrounding error policy remains; pending-wake persistence is not equivalent to the old condition. |

This completes per-file disposition coverage for all 34 changed host/src
files: 15 support + 4 disk/memory + 3 BOP/CCPU + these 12. It does not close
unresolved owner equivalence, prove runtime reachability, or increase the
confirmed duplicate count by counting each carrier as another function.
The source diff remains audit input only; none of these proposed dispositions
has been implemented.

#### BIOS argument-carrier follow-up

The complete src reference search finds three callback invocation sites in
video.c: word character/attribute assembled from BL/AL, byte AL, and teletype
char ch assigned from AL. Other references are declarations or assignments.
The teletype char can be signed depending on compiler configuration, so a
blanket statement that every ULONG input has no high bits would be wrong.

Followed the actual data carrier instead: j_c_lang.h maps jccc_parm2 to r3;
chain2_evid[4] selects S_2696_Chain2ByteWrite_Copy and
S_2699_Chain2WordWrite_Copy. Their sevid019.c rule entries read r3 as IU8 and
IU16 respectively into local value storage. This supplies the missing
low-byte/word extraction evidence for the selected copy-rule value, including
a sign-extended teletype input. It is not a rendering test or proof about
unrelated vector providers. The earlier suggestion that the call's C type
necessarily narrows the value is withdrawn: cpu_vid.h's legacy EVID write
declarations put the narrow type on the first argument, while these generated
rules consume the register globals. Preserve the separately tracked generated
ABI consistency check rather than inventing another CPU/raster repair.

### Consolidated discussion index: confirmed owner overlap

Stable discussion identifiers below consolidate verified findings in this
document; they are not a claim that whole-project coverage is closed. A row
may cover partial functionality, and is not a promise that its entire current
file can be deleted. Detailed evidence and reachability limits in the named
sections above/below remain controlling. All dispositions await owner approval.

| ID | Functional unit and current carrier | Original owner | Discussion disposition |
| --- | --- | --- | --- |
| D01 | BaseSrv next-command policy, base_vdm_local.c | srvvdm.c:BaseSrvGetNextVDMCommand | Restore original queue/response policy; bind transport separately. |
| D02 | PIF response, same | srvvdm.c:BaseSrvFillPifInfo | Restore capacity, title and output-field policy. |
| D03 | First-VDM query, same | srvvdm.c:BaseSrvIsFirstVDM | Resolve global versus per-session cardinality explicitly. |
| D04 | Current-directory set/get, same | srvvdm.c:BaseSrvSetVDMCurDirs/BaseSrvGetVDMCurDirs | Preserve consume-on-get and define locking/failure ownership. |
| D05 | DOS task exit, same | srvvdm.c:BaseSrvExitDOSTask | Recover original lifecycle slice; ignored WOW exit is missing capability. |
| D06 | Reentry accounting, same | srvvdm.c:BaseSrvSetReenterCount | Separate original accounting from local native-child scheduling. |
| D07 | Client wait/retry/capture, base_vdm_client.c | BaseClient vdm.c:GetNextVDMCommand | Restore client result/copy semantics with bounded transport. |
| D08 | Fast read, demfastio.c overlay | kernel vdm/x86/rdwr.c:NTFastDOSIO | Compare finite original service versus original slow fallback. |
| D09 | Mouse-pointer visibility, console_compat.c | ntcon/server/private.c:SrvShowConsoleCursor | Current text-cursor substitution is not equivalent. |
| D10 | Executable classification, mvdm_image_classification.c | BaseClient vdm.c:GetBinaryTypeW | Restore format classification, keep CLI search policy separate. |
| D11 | Mutable environment blocks, opennt_support_rtl.c | ntos/rtl/environ.c | Restore original validation and block ownership. |
| D12 | Status translation, same | ntos/rtl/error.c and tables | Restore complete mapping or verified native binding. |
| D13 | Extended integer division, same | rtl/x86/largeint.asm | Remove signed/zero-divisor contract substitution. |
| D14 | Extended integer multiplication, same | rtl/x86/largeint.asm | Preserve original overflow semantics; verify compiled binding. |
| D15 | ULONG fill, same plus copy_fnc.c local loop | rtl/x86/movemem.asm | Resolve length units/tail contract without blind rollback. |
| D16 | USER multibyte conversion, wow_public_user_facade.c | ntuser/rtl/chartran.c:MBToWCSEx | Restore ACP partial-conversion and allocation policy. |
| D17 | Network font tracking, same | ntgdi/client/font.c | Restore original path/list policy through public allocation/GDI bindings. |
| D18 | WOW glyph outline, same | ntgdi/client/dcquery.c, gre/ttgdi.cxx | Recover ignore-rotation compatibility, not public-A substitution alone. |
| D19 | WOW cooperative scheduling, wow_private_user_compat.c | ntuser/kernel/taskman.c | Linked helper reachability unproven; avoid treating it as recovered scheduler. |
| D20 | Cursor/icon DIB construction, same | ntuser/client/clres.c | Restore conversion/size/share policy without importing the USER server wholesale. |
| D21 | VDM hard-error dialog, wow_hard_error_dialog.c | ntuser/server/harderr.c:DisplayVDMHardError | Restore decoding/default/cancel policy; isolate desktop/CSR mechanics. |
| D22 | Debugger startup/dispatch/state, dbg_*.c | mvdm/dbg/dbg.c | Recover original finite policy; establish unavailable outgoing calls. |
| D23 | Independent-window input records, presentation_window.c | ntcon/server/input.c | Preserve event-time modifiers/buttons; not native Console input. |
| D24 | Character-to-key sequence, nt_event.c | ntcon/server/clipbrd.c:DoStringPaste | Partial overlap only; exclude clipboard-specific policy from packet adapter. |
| D25 | Resource lookup/read, nt_unix.c/nt_rez.c and firmware adapter | original same-named MVDM resource owners | Move packaging boundary out of original algorithm where feasible. |
| D26 | Redirector OEM copy, mvdm_redirector_guest_copy.c | netlib/copystr.c:NetpCopyWStrToStr | Original already mirrored; preserve byte-capacity contract. |
| D27 | Inactive WOW callback stack carrier, mvdm_wow_task_frame.c | WOW CallBack16 TD stack branch | Inactive duplicate; removal must verify no non-fixture consumers. |
| D28 | Return queued input, console_compat.c:WriteConsoleInputVDMW | ntcon/client/private.c and server/directio.c | Restore prepend ordering; public WriteConsoleInputW appends instead. |
| D29 | Input read flags and reserved-key filtering, console_compat.c:ReadConsoleInputExW | ntcon/server/directio.c and input.c | Separate atomic NOWAIT/NOREMOVE behavior from Alt+Enter policy; preserve paired key events and Ctrl exclusion. |
| D30 | Console keyboard-layout query, console_compat.c:GetConsoleKeyboardLayoutNameA | ntcon/server/getset.c | Caller-thread layout is not necessarily Console layout; resolve state ownership before replacement. |
| D31 | DOS handle allocation, mvdm_vdd_sft_shadow.c and demfile.c wrappers | mvdm/dos/dem/demfile.c:GetFreeSftEntry/VDDAllocateDosHandle | Restore original selection/reservation and optional-output contract through bounded guest access. |
| D32 | NT handle association, same | demfile.c:VDDAssociateNtHandle | Current shadow-only update changes exported API visibility; include modified WOW32 commit callers in restoration. |
| D33 | DOS handle retrieval/release, same | demfile.c:VDDRetrieveNtHandle/VDDReleaseDosHandle | Restore field-scoped ownership; full snapshot writeback and partial commit are not original live-alias semantics. |
| D34 | Graphics palette state, console_compat.c plus session/app presentation | ntcon/server/private.c:SrvSetConsolePalette | Partial replacement: RGB snapshot rendering does not retain original per-buffer palette ownership, usage or realization policy. Private desktop/system-palette operations remain a named boundary, not an import mandate. |
| D35 | Protected/real transition frame interpretation, dpmi32/modesw.c | dpmi32/i386/dpmi386.c:switch_to_protected_mode/switch_to_real_mode | Source-derived repeated frame logic; retain original owner/layout while binding active CCPU state separately. Autonomous TSS/IDT construction is not established as this same original implementation. |
| D36 | XMS block/memory movement, mvdm_xms_memory.c and xms.c | xmsMoveBlock plus xmsmemr.c/i386/xmsmem86.c move callbacks | Replace duplicated chunk-copy policy through source-shaped guest access; distinguish original copy and overlap-safe move contracts. |
| D37 | DPMI allocator selection, dpmimemr.c | Original Vdm allocation call and SA fallback branches in the same file | Restore or explicitly justify changed provider-selection order. Original SA allocator remains reused; do not count a new allocation algorithm. |

These 37 discussion rows are a verified subset, not the requested final total.
D28--D33 consolidate the Console and DEM/VDD follow-ups above; optional output
and commit failure are subcases, not extra duplicate algorithms. Console wait
endpoints, registration facilities and internal memory/control changes
still need disposition reconciliation. Public User32 leaf forwarding, explicitly
unsupported operations and a changed wait handle are not automatically an
independent reimplementation of the Console server. Pure diagnostics,
unresolved-owner C-VID timing/physical mapping, same-body source carriers and
necessary ABI declarations stay separate rather than inflating this table.

### MVDM restoration-package boundaries for discussion

These proposals consolidate existing direct-source findings; they authorize
neither repairs nor new imports. Overlapping entries refer to the same D IDs,
not extra duplicate counts.

| Original owner package | Proposed source-first disposition | Required regression and unresolved boundary |
| --- | --- | --- |
| DEM/VDD | Restore original SFT/JFT selection, association and release policy (D31--D33) in DEM. Bind bounded guest memory underneath, and remove shadow-specific WOW caller changes together. | Allocation visibility, NULL optional outputs, retrieve/associate/release, full-JFT overwrite avoidance, partial commit failure, COMMAND/EDIT file operations and WOW file calls. A raw alias is not automatically safe merely because upstream used it. |
| COMMAND native execution | Keep original cmdCreateProcess/task execution body. Isolate asynchronous input lifetime and native thread ABI, preserving original command terminators, environment, standard streams and failure order. | Native-child launch/wait/failure, BOP 54:08 reentry, environment/tail limits, three standard streams and parent/child return. Broker architecture is not this repair's prerequisite. |
| DPMI transitions and allocation | Recover original transition frame policy (D35) with active CPU state binding; reconcile allocator selection (D37). Do not bless hard-coded TSS slots, inferred IDT, shared GDT/LDT backing, or guest TEB/PM-stack projections as original reuse. | Mode-entry/return, descriptors, interrupt stacks, allocation/free/reallocate, DOSX and WOW bootstrap. Kernel/guest initialization owner for the autonomous projections remains unresolved and must be discussed separately. |
| XMS | Recover original move/copy policy (D36), choosing the correct original backend callback contract rather than keeping unconditional zeroing by default. | Overlap versus non-overlap, partial failures, commit/decommit meaning and memory retention, XMS consumers; original CCPU overwrite no-op is not a missing cache implementation. |
| CCPU execution | Restore demonstrated mask-scope and reset-observation semantics after approval; keep atomic producer requirements separate from consumer ordering. Move selector-specific establishment to its proper owner rather than bypass generic checks in c_seg.c. | USE16 wrap, HLT/RESET consumption, PIC producer/acknowledgment ordering, descriptor validation and callbacks. These are mirror semantic changes, not proof that the original CCPU is broken. |
| Redirector | Retain original network queue and service bodies; isolate encoding/copy changes (including D26) and replace or prove asynchronous buffer/teardown policy. | Actual completion before freeing I/O buffers, cancellation, guest copy publication, worker self-exit, native thread ABI, named pipes and OEM names. Original empty teardown cannot be restored without accounting for current session lifetime. |

These rows intentionally preserve unresolved provenance as unresolved. They
cannot be used as approval-ready kernel import lists or claims that every
affected original translation unit has already been composed. Diagnostics,
declaration casts and inactive fixtures remain distinct review categories.

### Console palette consumer reconciliation

Re-read console_compat.c:SetConsolePalette/console_video_event, session.c
palette set/snapshot, app/presentation_window.c:presentation_event and
presentation_paint, and original ntcon/server/private.c:SrvSetConsolePalette.
The app event consumer only schedules repaint for palette events; it does not
consume event flags or select/realize the supplied palette. Painting rebuilds
an RGBQUAD table and calls SetDIBitsToDevice with DIB_RGB_COLORS. Thus the
earlier unresolved question about downstream usage/lifetime policy is answered:
this path retains copied RGB values, not original palette policy. The original
owns per-screen-buffer hPalette/dwUsage, deletes a replaced palette, and performs
desktop-sensitive selection and realization. Modern RGB presentation may need
a different binding, but is not proof those original semantics were recovered.

The event also carries palette_handle despite comments claiming the app never
receives HPALETTE. The current app ignores that field; distinguish a stale or
overbroad contract from demonstrated dereference or cross-process transport.
The palette setter writes the session array without acquiring the graphics
mutex used by snapshot readers. This is a synchronization gap if calls overlap,
not evidence of an observed race in this read-only audit.

Do not add a spurious orientation regression: the adapter accepts either sign
of DIB height and the app paints top-down, but the reached original nt_graph.c
graphicsResize caller itself supplies negative height and one plane. General
positive-height support remains unproved; that does not show this caller paints
upside down. No product or runtime changes were made for these findings.

### Retained graph denominator reconciliation

#### Unpaired printer carrier resolved

The seven-path unpaired inventory is a manifest-coverage classification, not
seven unknown algorithms. Read the complete kernel-vdm/v86/monitor/i386/
monitor_printer.c and matched its three BOOLEAN function bodies to original
OpenNT/base/mvdm/v86/monitor/i386/monitor.c. Upstream SHA-256 is
80E97A535AE7760089B2EE9FAD8F6AE8202936240F7BC47C17227611AA0268B6,
matching the carrier comment. Extraction from each BOOLEAN signature through
its column-zero closing brace, normalizing CR only, yielded equal bodies:
MonitorInitializePrinterInfo 1465 characters, MonitorEnablePrinterDirectAccess
952, MonitorPrinterWriteData 334. The original intervening
MonitorVddConnectPrinter function is not part of this carrier.

Disposition: retained original MVDM fragments, not duplicated autonomous
algorithms and not three new D rows. The new filename and kernel-vdm prefix
do not preserve the original relative file shape; propose source-path/fragment
registration reconciliation, not migration to non-MVDM opennt-host merely
because the directory says kernel. The current build generator compiles the
carrier and includes kernel-vdm-printer.lib in the process link rule. This
establishes selection, not runtime printer acceptance.

Read the included monitor_context.h and local NtVdmControl provider too:
VdmTib is process-global, not a thread-local replacement. Direct-I/O close is
rejected by the local provider (only VdmQueryDir is admitted there); the
original caller ignores that result before clearing printer bookkeeping.
The commented-out direct-I/O open call is already in upstream and must not
be blamed on this import. Missing kernel service behavior is separate from
the exactness of these retained bodies. No printer test or repair occurred.

#### Canonical-path and inactive-adapter cross-check

Comparing tracked adapter C paths with retained graph inputs requires resolving
dot segments. r014 selects monitor/source/../mvdm_vdm_tib.c. A literal path
comparison incorrectly listed it as unselected; GetFullPath relative to the
repository root removes that false negative. Canonical union remains 505,
so the numerical total alone would not reveal the membership mistake.

The resulting unselected adapter C set has exactly four members for these
two graphs: monitor/mvdm_wow_task_frame.c, monitor/source/host_idle.c,
win32/source/mvdm_redirector_thread.c and win32/source/
wow32_public_api_adapters.c. These are still in audit scope; unselected does
not mean deletable without checking other supported targets. Task-frame
fixture generators explicitly compile the first; the public USER duplicate
and idle stub retain their previously recorded dispositions.

Read the complete redirector-thread stub and its header. It discards all
thread inputs, zeros an optional thread id and returns NULL with
ERROR_CALL_NOT_IMPLEMENTED. The only include found under src/tools/tests is
its own translation unit; no current caller there selects its CreateThread
macro. Classify it as an inactive refusal scaffold, not an original threading
algorithm or proof that active Redirector workers cannot start. Proposed
cleanup may remove this obsolete source/header pair after supported build
entrypoints are checked. No files were deleted or product selections changed.

Re-extracted unique src inputs from cc/cc_* and rc rules in retained r014
and r015 Ninja graphs: 505 total, comprising adapter 48, app 5, broker 3,
MVDM 435, overlay 5, patch 1, opennt-host 6 and session 2. The six opennt-host
inputs are client/vdm.c and NetAPI apibuff.c, allocstr.c, copystr.c, initoem.c,
ntstatus.c. srvvdm.c is absent from these compile-input sets. This confirms
the BaseSrv original-owner/selected-replacement distinction without counting
the entire mirrored server as restored. Graph presence is not successful
linkage, export use or runtime reachability; these are retained WIP graphs,
not proof that the frozen accepted executable uses all 505 inputs.

Coverage closure still requires joining the per-family findings to every
inventoried provider and a consolidated disposition ledger. ABI directory
pairing is complete; patch/header follow-ups above remove specific prior
read gaps. They do not close CURRENT/S1 automatically or authorize S4/S5.
Guest/firmware/tools retain their separately labelled load-only/independent
scope from the all-root inventory; no silent expansion into guest rebuilding
or exclusion of overlay bodies is made.

### C-VID binding generator semantic reconciliation

Re-read GenerateCvidcCpuBinding.mjs and the overlay vector-binding C body.
The generator derives slot names from original metadata, but provider choices
are locally authored. Duplicate object keys deliberately or accidentally
overwrite earlier choices: ClearHwInt, InitIOS and six port-registration
entries end as zero, as do private InitNanoCpu/PrepareBlocksToCompile.
Other explicitly zeroed slots include status/control accessors. Thus the
generated-file label does not mean original provider selection was recovered.
Null slots still require per-profile reachability evidence; do not infer a
current null-call crash solely from their presence.

The overlay also supplies GetJumpCalibrateVal and GetJumpInitialVal from the
same mutable value initially 100. Original accessfn.c forwards those as
separate queries; qevnt.c accumulates CalibCount from the former at line 542.
This is a locally authored timing approximation, not merely vector wiring.
An available original calibration-provider body has not been established, so
classify it as unproven replacement semantics rather than a confirmed
deletable duplicate. Actual video binding remains Video = C_Video and the
SAS overwrite slot uses the separately audited CPU40 provider. No generator
was run, no build selection changed, and no timing acceptance was claimed.

### Session termination thread-ownership check

Checked session.c::session_arm_termination_escape/session_terminate_current,
the two setjmp sites in mvdm_softpc_execution.c, and all three original-worker
thunks in thread_start_compat.c. The session has one shared jmp_buf; worker
thunks bind the same owner into their TLS. terminate_current checks active
state and armed status but does not check that the calling thread established
that setjmp. Therefore a bound secondary thread reaching this endpoint can
attempt a cross-thread longjmp rather than the original process-wide exit.
This is an ownership hole in the autonomous lifetime replacement; a concrete
normal-workload path into that hole is not proven by the interface inspection.
Do not claim the error-dialog worker or Console callback does so without
checking its particular binding and branch. Unbound threads instead fail the
session check and return, another reason this is not an ExitProcess-equivalent
API. Group with the existing lifecycle restoration item; no new count for
each thunk and no runtime failure attribution.

### WOW cursor/icon original-owner follow-up

Read current wow_private_user_create_legacy_dib_cursor_icon and its public
wrapper, original clres.c::WowServerLoadCreateCursorIcon, and the complete
ConvertDIBIcon body. Original WOW selects LR_SHARED and default system
dimensions, adds LR_MONOCHROME for cursors, passes module/name identity into
conversion and scales cursor hotspots to the chosen dimensions. The current
legacy helper creates at resource dimensions with the original unscaled
hotspot, using its own DIB parsing/GDI assembly; its successful resource path
does not retain original shared-resource identity. Its parser only accepts a
40-byte BITMAPINFOHEADER, one plane, uncompressed positive-height data and
1--8 bits per pixel; icon fallback is distinct from the cursor failure path.

This confirms substantial duplicate conversion/assembly policy rather than
a pure public-API rename. Restoring policy does not imply importing private
CreateIcoCur object management wholesale. Original ConvertDIBBitmap's complete
format coverage is not proven by this follow-up, and the previously recorded
unproven callback reachability remains: a linked helper is not necessarily an
active current WOW path. No cursor, icon, window or guest was created.

### WOW MBToWCSEx original contract follow-up

Read original ntuser/rtl/chartran.c::MBToWCSEx completely and the full
ntos/rtl/nlsxlat.c::RtlMultiByteToUnicodeN body, alongside the current facade.
Original ACP conversion deliberately stops at output capacity and returns the
actual converted count: the RTL routine caps its loop and returns SUCCESS.
Current facade routes ACP through MultiByteToWideChar with the full input
length instead, losing that source-visible partial-conversion policy. Keep
this as a concrete difference within the already counted conversion family,
not another duplicate count. Non-ACP user-mode original conversion already
uses MultiByteToWideChar and should not be described as newly invented.

Original allocation failure assigns NULL to the caller output before return;
the replacement allocates into a temporary and leaves the prior output
unchanged on allocation failure. Conversely, freeing an allocated buffer after
conversion failure without clearing the published pointer occurs in both
versions: do not attribute that behavior solely to the adapter. Restoration
can retain original branching and allocation ownership through a bounded
binding; no build or runtime equivalence has yet been demonstrated.

### Mirror keyboard normalization and control-event boundary

Original-owner follow-up: fully read OpenNT
windows/core/ntcon/server/clipbrd.c::DoStringPaste and input.c::GetControlKeyState.
DoStringPaste already converts Unicode characters to console key sequences
using VkKeyScan, modifier transitions and paired key records. It handles AltGr
with enhanced right-Alt/control state and unmappable characters with an OEM
numeric-keypad sequence. Current nt_rdp_normalize_key has a different modern
packet entry but independently implements the overlapping character-to-key
algorithm; its all-left modifier synthesis and dropping unmappable input do
not preserve those policies. Classify this portion as confirmed functional
overlap with an original owner, not an exact duplicate of the whole paste
operation. Clipboard chunking, CR/LF filtering and Console output-code-page
ownership are not automatically appropriate for the modern packet adapter.
No wholesale replacement is proposed without separating those boundaries.

GetControlKeyState explicitly reads left/right modifier and toggle states,
supporting the previously recorded independent-window input gap. It is not
proof that polling current key state after a queued event preserves the event's
original state; a restoration must retain the event-time contract. No runtime
test or keyboard repair was performed.

Re-read the complete nt_event.c diff. DIV-211 adds four static functions that
map missing scan codes from virtual keys or UTF-16, synthesize modifier/key
make-break sequences, and change raw reads from five records to one with an
eight-record expansion buffer. Existing scan-bearing records pass through.
The packet synthesis zeroes control state except ENHANCED_KEY and forces
repeat count one; surrogate input and characters unmappable by VkKeyScanExW
are dropped. pending_high_surrogate is stored/reset but never combined into
a codepoint. This is an autonomous keyboard-normalization policy inside the
mirror, not just an API declaration. Original KeyMsgToKeyCode and keyba.c
still own subsequent translation/controller behavior. Do not count the new
normalizer as replacement of the whole original keyboard or as proof of the
historical mouse false-click cause. Its corresponding original Console input
owner remains a required comparison before a restoration decision.

Separately, CntrlHandler replaces ExitProcess with session termination and
returns FALSE. This joins the existing lifetime audit, not the keyboard
normalizer count. Added scalar mouse/key observers and event-thread join
bindings are likewise separate from input semantics. All findings are static;
no Console input, guest run or product edit was performed.

### Resource, media and idle mirror reconciliation

Re-read all paired hunks for nt_rez.c, nt_unix.c and nt_msscs.c. In nt_rez.c,
host_read_resource now returns the embedded-ROM helper result immediately for
ROMS_REZ_ID, bypassing the original file route even when the helper fails or
the name is not one of its three recognized ROM names. The helper implements
FindResource/LoadResource/copy locally inside the mirror. This is a packaging
boundary replacement of the original resource-read route, not an additional
CPU implementation; group it with resource lookup. The write retry also uses
the resolved path instead of the original bare filename, a separate path
policy change that must not be hidden under declaration-only edits.

nt_unix.c replaces the original GetSystemDirectory/FindFirstFile policy with
the firmware adapter while retaining the failure switch. Its HostIdleNoActivity
also changes a NowWaiting-conditional PulseEvent into an IdleEvent-nonnull
SetEvent. A pending wake can therefore survive until a later wait; this is an
intentional semantic change, not proof that the old and new event contracts
are identical. Record it with worker-idle behavior, without asserting a fault.

nt_msscs.c's suppression of real-host IO.SYS/MSDOS.SYS marker creation is an
explicit standalone non-intrusion policy, not a missing guest DOS algorithm
to restore blindly. Missing installable-VDD registry state becomes NONE;
other failures retain the error route. CPU_40_STYLE selects the original
software-CPU virtual-interrupt bit branch instead of using host i386 as the
machine selector. Separate that CCPU profile binding from the autonomous
resource and idle policies. No product edits or runtime acceptance claims.

### Mirror-local copy and heartbeat semantic changes

Recompared copy_fnc.c and nt_timer.c against the exact upstream paths in the
paired-diff inventory, reading their complete changed hunks and current
surrounding branches. copy_fnc.c::fwd_word_fill changes more than pointer
width: the selected #else branch now fills a DWORD count with its own loop
and advances l_addr4, replacing the original RtlFillMemoryUlong call and
byte-pointer advance. Group this with the RTL fill owner discussion. It is
not justified merely by the nearby x64 address-alignment comment. The original
call's count/unit usage itself requires interpretation; restoring it blindly
is not an approved repair. No runtime correctness claim for either branch.

nt_timer.c broadens heartbeat termination from STATUS_ALERTED to also include
any STATUS_USER_APC. Re-read nt_thread_alert_compat.c: it prefers native
NtAlertThread and uses QueueUserAPC only when that export is unavailable.
Consequently the mirror treats APC completion as terminal even on its native
alert path; it does not identify whether the APC was this fallback's request.
This is a changed lifecycle decision inside the mirror, not an ABI cast or
proof of an observed unwanted exit. Retain as a restoration-review point with
the native-alert binding and session teardown family. The tv_usec temporary
conversion is separately bounded by the positive divisor and is not the same
kind of semantic change. No product edits or timer executions.

### SoftPC patch carrier follow-up

Completed a subsequent full sas4gen.h read and full no-index comparison with
OpenNT/base/mvdm/softpc.new/base/cvidc/sas4gen.h. The current x86 carrier
retains that original file except for the sas_touch, VirtualiseInstruction
and IsPageInstanceData types/vector fields/dispatch declarations added by the
retained NTVDMx64 sas4gen.patch. The macro bodies select original c_* or Sas
vector calls; they do not implement another memory-access algorithm. This
provides a concrete original carrier counterpart despite the missing original
host/inc/x86/prod path. It does not justify deleting added vector slots:
sasCdef.c's composition and its selected provider ABI still require them.
The CCPU no-check scalar aliases and empty overwrite/instance-data declaration
branches are present in the original counterpart, not newly authored behavior.
The earlier partial-read limitation below is superseded for sas4gen.h only.

Read callconv.patch completely and verified its SHA-256 equals the retained
NTVDMx64 minnt patch (1D3FA424A9C3337AE54E1BD5C8490BBC8FDA2F8E0A1D39E2275D7FB412823D78).
It is calling-convention/cast evidence, not a second worker algorithm; do not
assume every supplied hunk is applied merely because the patch is present.
Read PigReg_c.h completely: CPU/NPX state record declarations, no function body.

The large gdpvar.h output was truncated and is not claimed as a full manual
read. A whole-file line-pattern check instead classified every nonblank line:
fixed-offset typed GDP dereference macros, guards, CurrentUniverse setter,
TraceVector expression/size, GDP_SIZE/CHECKSUM, Gdp declaration and GDP_PTR
alias. The setter and four-byte TraceVector displacement are explicitly in
the retained upstream gdpvar.patch. There is no independent control-flow
algorithm in this carrier; correct offset/layout composition still needs
separate evidence. sas4gen.h was only partially read in this follow-up and
is not marked complete. Patch README statements about mapping-manager leases
and dual-host support are historical, not proof of current product selection.

### App, broker and session header coverage

Fully read all nine headers in src/app, src/broker and src/session. They
contain no inline execution bodies. The broker 16-client and 16-record
capacities, record v3 command_owner, session eight-lease/eight-teardown limits,
and single termination_escape are explicit autonomous composition contracts;
they are not original OpenNT ABI simply because their records copy VDMINFO
fields. Group them with the previously audited broker queue, memory leases
and worker lifetime, not additional function counts. The wire user_key field
alone does not establish authentication. App headers preserve a separate
target application and WOW bootstrap kernel and do not resolve the previously
recorded internal-argv versus OS-command-line split. These declarations were
checked against the existing implementation-family findings; no product edit.

### Native I/O facade classification refinement

Fully read the remaining win32/include ntioapi.h, wow_user_declarations.h,
presentation_surface.h and wow_hard_error_dialog.h; reread the complete
win32/source/ntioapi_facade.c. The latter resolves ten native NTDLL APIs,
forwarding three directly and translating IO_STATUS_BLOCK through stack
temporaries in seven. It does not reimplement directory enumeration,
filesystem control or device control algorithms. Classify it as a native
binding/marshalling layer with an asynchronous-lifetime risk, not ten
duplicated kernel algorithms.

The header justifies the private status layout by x64 pointer width. With the
current x86-only target, reassess that conversion before retaining it. The
wrapper returns without waiting and copies native_status even when completion
may be pending; an asynchronous completion could outlive the temporary. But
demsrch.c's directory open explicitly uses FILE_SYNCHRONOUS_IO_NONALERT at
line 1075, so do not present its normal directory query as demonstrated use
of this failure. All other caller classes still require their own lifetime
proof. This narrows the earlier generic suspicion without erasing the wrapper
contract issue. No native API was invoked and no product behavior changed.

### Remaining provider-family header pass

Read all headers in adapter softpc/include (except termination and the
separately read symbol-compat header), vdd/include and redir/include; then
read all basesrv/include, debugger/include, wow/include and monitor/include
headers not covered by prior passes. These contain records, constants,
prototypes and the following already-accounted dispatch binding, rather than
additional inline service algorithms: redir/vdmredir.h replaces
HANDLE_FROM_WORDS with mvdm_redirector_handle_from_words. Its comment still
describes a session mapping instance; actual provider behavior, not this
comment, determines whether the retired mapping-manager model remains.

The SFT/JFT header explicitly requires synchronous commit/discard of returned
host shadows. The native-child header exposes capture/activate/finish rather
than replacing the original cmdCreateProcess worker. BaseSrv headers expose
the separate command-owner and DOS record state machine already counted in
the BaseSrv replacement family. The inactive WOW task-frame header is a
second TD-shaped carrier, not evidence of a second active task scheduler.
No new confirmed duplicate functional unit is added merely for these
declarations. Their implementation and caller findings remain controlling.

error_abi.h locally repeats error-table types/enumerators and the physical
mapping header declares original-shaped EMS services; declaration duplication
is distinct from algorithm duplication. This header pass does not resolve
the previously explicit physical-mapping implementation-owner uncertainty.

### Additional adapter-header dispatch coverage

Fully read the following twelve headers: win32/include/winbasep.h,
winconp.h, conapi.h, vdmapi.h, command_process_compat.h,
thread_start_compat.h, nt_thread_alert_compat.h, mvdm_redirector_thread.h,
mvdm_base_vdm_environment.h, wow32_provider_private.h, winuserp.h, and
monitor/include/monitor_context.h (all under adapter-mvdm-host-out).

Dispatch reconciliation: command_process_compat.h replaces SetStdHandle and
the generic CreateProcess spelling with the previously audited child-local
provider; thread_start_compat.h replaces CreateThread/ExitThread with named
cdecl/session thunks; mvdm_redirector_thread.h redirects CreateThread to its
separate provider. These macros change the effective owner even though the
mirror call text is retained. They belong to the already recorded command,
worker-lifetime and Redirector families, not three new duplicate counts.

monitor_context.h maps pNtVDMState to process-global host storage, not the
original guest low-memory address; this is the binding point for the existing
monitor-state finding. Its temporary _X86_ declaration gate is not evidence
of selecting the kernel monitor implementation. winconp.h suppresses the
original import name for the ANSI keyboard-layout query before declaring the
local provider; conapi.h exposes the previously audited Console replacements.

The other listed files primarily carry declarations, constants, empty include
compatibility or imports. wow32_provider_private.h's MBToWCS macro routes to
the already audited MBToWCSEx conversion; it is not a second converter. The
nt_thread_alert_compat.h comment describes APC-only behavior, but the previously
read implementation prefers native NtAlertThread: record stale explanation,
not an additional runtime implementation. No new confirmed duplicate family
was discovered in these headers. Header reads do not replace call-site and
selected-build verification for their providers.

### ABI WOW subset reconciliation

The supplementary pairing check is now reconciled for all 209 files under
opennt-abi/source: 194 same-path byte-equal files; four relocated/cohort
byte-equal files; seven terminal-blank-line-only differences; one guarded
vdmapi.h; three WOW subset carriers read below. This exhausts this directory's
file denominator, not the complete project or transitive runtime ABI proof.
Full diffs verified the seven additions are blank lines at EOF, not logic.
The relocated apiworke.h equals OpenNT/ds/netapi/rpcxlate/apiworke.h. dlcio.h,
pmvdm.h and jpeg1/x.h equal their respective OpenNT-4.5/nt/private paths.
These four were checked by SHA-256 after locating actual reference files;
do not call them unproven autonomous implementations merely because the
OpenNT same-path lookup failed.

The canonical and two cohort manifests were located in
artifacts/documentation-archive/20260910/etc/operations/ledgers. The README
problem is a stale current-path link, not evidence that the manifests were
lost. No source import or dependency selection was changed by locating them.

Rechecked every file under opennt-abi/source against its same relative path
in the current external OpenNT checkout. This supplementary check is not a
replacement for edition-aware provenance: four private-path files have no
same-path counterpart, and the three named WOW subsets intentionally have
different filenames. Eight same-path files differ by hash. vdmapi.h contains
an added include guard/comment and closing-declaration indentation, with no
new provider body. Seven other pairs each have a one-line addition by numstat;
that count alone is not a semantic classification.

Fully read ntpsapi_wow32.h, ntrtl_wow32.h and nturtl_wow32.h. The first two
contain selected types/declarations and a current-thread pseudo-handle macro,
not independently implemented thread or conversion services. The last contains
RtlAssociatePerThreadCurdir: comparison with original nturtl.h lines 511--515
shows the same four field assignments. Its effective storage owner changes
through the previously recorded NtCurrentTeb binding, not a new association
algorithm. Do not count it as a second environment implementation.

The ABI README still describes byte-identical declarations and links an
operations manifest that is absent at that current path. Its summary is not
sufficient evidence for present coverage or provenance. This is a documentation
reconciliation item, not authority to remove these files or import replacements.

### RTL arithmetic duplicate and declaration carriers

Follow-up comparison completed the original x86 RtlExtendedIntegerMultiply
body (largeint.asm 484--529) and RtlFillMemoryUlong body (movemem.asm
508--563). Both adjacent adapter routines duplicate available original owners.
Multiply agrees on representable signed products; original assembly retains
the low 64 bits on overflow, whereas the replacement's signed C multiplication
has no portable overflow guarantee. This is a source-language contract risk,
not a demonstrated mismatch in the current compiler output. Mirror nt_timer.c
uses it at line 889 with multiplier 1000.

Fill has a definite length-contract difference: original shifts byte length
right by two and ignores the last zero to three bytes; replacement copies
those trailing bytes too. Current copy_fnc.c passes count shifted left by two,
so this caller does not expose that difference. WOW wkmem.c passes cbSize and
asserts DWORD divisibility before calling; that assertion expresses the
intended input, not proof of every release caller. Do not claim this difference
caused a current WOW failure. These two functional units join the division
unit as original-owner restoration candidates, not three independently
proven runtime regressions.

Also fully read adapter nt.h, vint.h, mvdm_crt_redirect.h,
mvdm_softpc_symbol_compat.h and wow_user_callback_callconv.h. The latter
is declarations only; CRT/shutdown headers rename existing owners. nt.h
redirects NtCurrentTeb to synthetic TLS and NtWaitForMultipleObjects to the
previously audited wait adapter; vint.h substitutes monitor_context storage
for fixed-low-address VDM state. These are binding points of existing audit
families, not new independent duplicate-service counts. The nt.h fallback
InsertTailList retains the original cached ListHead form, unlike the separate
ntrtl.h InsertHeadList rewrite discussed below.

Read the complete current opennt-host ntexapi.h, ntpsapi.h, nturtl.h and
ntrtl.h subsets. Most content is declarations, not service implementations.
The ntrtl.h list macros retain ordinary list-link operations but are locally
rewritten: InsertHeadList repeatedly evaluates ListHead where the original
caches it. This matters for side-effecting arguments; no such active caller
is established here. Count these macros separately from declaration-only
deletions and from the downstream RTL service providers.

Confirmed another duplicate functional unit in
`src/adapter-mvdm-host-out/win32/source/opennt_support_rtl.c:392`:
RtlExtendedLargeIntegerDivide replaces original OpenNT
`base/ntos/rtl/x86/largeint.asm:235`. Read the complete original routine.
Original divides an unsigned 64-bit dividend by an unsigned 32-bit divisor;
the replacement casts the dividend and divisor to signed LONGLONG. Original
normal (non-BLDR_KERNEL_RUNTIME) divide-by-zero raises
STATUS_INTEGER_DIVIDE_BY_ZERO; the replacement returns zero quotient and
remainder. These are source-proven contract differences, not merely different
instruction selection. For dividend FFFFFFFFFFFFFFFFh and divisor 2, unsigned
quotient is 7FFFFFFFFFFFFFFFh, while replacement signed division yields zero.

Mirror nt_timer.c calls this API at lines 349, 366, 368 and 525. That establishes
call sites, not proof that ordinary timer values exercise the differing input
range. The adapter justification still cites x64 import-library availability;
that is not sufficient justification for the present x86 product. Discuss
restoring the original owner or a verified same-contract native binding rather
than retaining an independently authored arithmetic policy. No repair or
runtime failure attribution is made. RtlExtendedIntegerMultiply and
RtlFillMemoryUlong are adjacent independently implemented routines; their
complete original-contract comparison is not claimed by this row.

Sequential reviewer checked signedness, the explicit zero-divisor branches,
the original build conditional, and mirror call sites. No product files changed.

### Diagnostic-provider coverage reconciliation

Completed the remaining bounded reads of
`src/adapter-mvdm-host-out/softpc/mvdm_softpc_termination.c`, including lines
1050 through EOF, and rechecked its report writers and environment capture.
This is autonomous instrumentation, not another confirmed replacement of an
OpenNT functional owner. Count it separately from duplicate services.

The report writers call environment/file APIs without saving and restoring
Win32 last-error state. Thus non-fatal logging is not evidence of transparent
error-state preservation. Whether a caller subsequently consumes that state
still requires a call-site proof; no observed product failure is attributed
to this finding. Several IRQ/keyboard observers increment static counters
before sink checks, whereas the low-fault and command-environment observers
explicitly gate their guest reads. Do not describe every observer as having
identical default-off behavior.

Environment capture removes twelve named selectors from the host process;
restore_child_report_paths restores only five captured paths. This is an
explicit environment/lifecycle policy, not a read-only observer. The command
and config observers use selected-media offsets (including COMMAND 203Ch,
0592h, 011Ch/0120h/0124h and NTIO 03D8h/3466h); those reads are not general
DOS interfaces. In the newly inspected tail, guest leases are READ leases
and release uses commit=0: no guest-memory write was found there. This does
not establish transparency of all transitive helper calls.

Sequential review distinguished source-visible host state changes from
unproved runtime effects and did not add these observations to the confirmed
OpenNT-duplicate count. Product source, build and runtime were unchanged.

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
