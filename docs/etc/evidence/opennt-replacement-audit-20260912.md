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

## Full-audit remaining work

For every selected functional unit, record original path/function, current
provider, actual build selection, unavailable outgoing interface, semantic
differences, retain/replace/delete decision and regression workload. Count
functional units separately from files and physical added/deleted lines.
Compare selected upstream against both mirror and overlay; do not label all
adapter lines hacks. This initial pass is not a full COMMAND/EDIT closure
audit and supplies no whole-project total.
