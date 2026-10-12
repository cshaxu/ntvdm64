# T445 S1 — installation and propagation design closure

## Scope, evidence and confidence

Owner authorizes completing S1 design, not implementing S2. Read the current
proposal, working plan, project authorities and source policy; traced the
owners below at baseline 82e81ed38. Findings are static source/API contracts.
No prototype, guest run, title modification or performance result is claimed.
The earlier [visual research](m0-t445-s1-win31-appearance-research.md) remains
evidence; this record resolves its installation choices and allocates remaining
implementation proofs. Source paths below are relative to the repository.

## Source-backed policy map and selected changes for S2

| Boundary / current evidence | Selected implementation responsibility |
| --- | --- |
| src/run16-exe/launch_options.h currently has command and wait only; main.c::launch_vdm feeds original BaseCheckVDM. | Add CLI explicitness and resolve Off/Win16-only/All before classification; attach effective policy to project request metadata, not original guest command text or global environment. Retain command/c and target-tail parsing. |
| src/common/protocol/native_hook.h::native_hook_packet v2 has flags/reserved currently required to be zero by nthook32-dll/context.cpp::nthook_context_read. | Version the copied payload; add a named fixed-width policy field with enum validation in both serializer and reader. Never silently reuse reserved bits while claiming old compatibility. Extend nthook_context and preserve launcher/intercept modes. |
| src/run16-exe/frontend_scope.c::scope_begin reads launcher payload but currently extracts only frontend/execution. | Resolve inherited appearance independently of Console attachments. GUI propagation must work with both handles NULL. Avoid consuming or closing the same payload capabilities twice when composing the launcher context reader. |
| src/common/codec/native_launch.h::run16_native_start and frontend_protocol.h::run16_native_launch_packet feed NTSRV native_commands and ntvwm-exe/execution.c::launch_request. | Carry/version/validate the scalar in the existing request packet and retained native record; copy it into the per-target nthook_context before install/resume. No worker-global policy. |
| src/nthook32-dll/create_process.cpp::finish copies nthook_process_context and clears Console handles for GUI/detached children. | Preserve appearance when clearing those handles, across x86/x64 and NATIVE_HOOK_LAUNCHER redirection. Retain original CreateProcess arguments, suspended-child rollback and caller suspension semantics. |
| src/ntsrv-exe/opennt/include/service_internal.h::OPENNT_BASE_CONNECTION retains task/reservation and request lifetime; base_command.c/base_service.c/base_rpc_client.c own project wire envelopes. | Carry policy in the project Check/Get command envelope and existing admitted request lifetime, keyed by real task/request identity. Do not add fields to original BASE_* guest structures or a second service task registry. Capture before original Check dispatch; retain through Get, failure and task retirement under existing locks. |
| src/mvdm/wow32/wkman.c:449,910,1078 passes VDMInfo.iTask through td.VDMInfoiTaskID to pfnInitTask; wow_user_registration_bridge.c::registered_init_task receives shared_id and task_id. | Resolve direct policy against the exact service request before task/window publication. Keep it in the existing project-owned wow_user_task_lifecycle_thread, with actual guest task identity, and release on that task's normal cleanup. No new process-global current policy. |
| src/mvdm/inc/tdb16.h:62 defines TDB_Parent; wow16/kernel31/task.asm:717 writes it from curTDB; local tasks may have VDMInfoiTaskID == -1. | When there is no direct request, read the existing parent identity through the checked guest-memory binding at task initialization and copy the parent's live policy. Direct request policy, including Off, wins over WOWEXEC's parent identity. Never write new flags into the guest TDB or repurpose WOW compatibility flags. |
| src/mvdm/wow32/wkman.c::WK32LoadModule32 calls CreateProcess at 2163; current WOW project sources have no CreateProcess interception. | A required, previously missing propagation edge. Bind this existing call through a WOW-local CreateProcess-shaped facade that obtains the bound task policy and uses the existing suspended-child installer/launcher machinery. Preserve real returned handles, flags and errors; do not inject nthook into WOW or wrap every native child in run16. |
| src/ntvdm-exe/win32/command_process_compat.c owns DOS-to-host execution; base_rpc_client.c has a delivered-command callback. | Preserve task policy through the existing DOS execution/request boundary and nested run16 relay without giving DOS or NTCON a style renderer. S2 must exercise this route rather than infer it from native Hook success. |

Direct request ID and a guest task handle are different namespaces. Match them
only at original InitTask publication. Parent lookup uses the existing live
task list under its existing synchronization, then copies the scalar; windows
must not borrow a parent task after retirement. A missing known direct policy
is an error, not silently Off. Boot/no-origin tasks default Off. Guest parent
reuse/early-exit cases require an S2 fixture; if current init timing cannot
prove the parent identity, capture at the existing task-creation boundary,
not by guessing the last active task or inventing a durable ancestry registry.

This is a concrete field/owner design, not a claim that these fields already
exist. Shared WOW request reclamation must be checked before retaining the
scalar: the direct caller can return after successful startup while the task
continues. Policy must survive in the task, not depend on a live run16.

## Window installation and message lifetime

Choose one same-thread per-HWND subclass in src/win16-style, using the public
SetWindowSubclass/DefSubclassProc/RemoveWindowSubclass chain, not a replacement
DefWindowProc for the application. Adapters supply enablement; no renderer-side
program classification. Store bounded window state, no global HWND registry.

- WOW normal windows: wow_window_creation_binding.c::creation_hook publishes
  metadata and owner first. Install only after that succeeds; preserve the
  original wow_window_native_proc -> wow_window_dispatch_bound guest callback.
- WOW dialogs: wow_dialog_creation_binding.c::dialog_create_hook is the owner
  boundary; do not replace dialog_return_hook's W32InitDlg or dialog_proc's
  WM_INITDIALOG conversion. Both create scopes already have explicit failure
  cleanup. Unwind style state there even when WM_NCDESTROY never arrives.
- Native: add thin same-source creation wrappers in nthook for CreateWindowExA/W,
  CreateDialogParamA/W, CreateDialogIndirectParamA/W, DialogBoxParamA/W and
  DialogBoxIndirectParamA/W. Each opens a nested same-thread CBT creation scope;
  it works on later GUI threads without hooking CreateThread or enumerating
  windows. Modal calls are covered before their inner loop, not after return.
  Nested wrappers must not double-install. Do not rely on undocumented internal
  calls between public USER APIs. Private direct syscalls and external creation
  brokers are not guaranteed coverage.
- In CBT, capture the request and install state before creation messages, but
  do not send arbitrary application messages while its initialization is
  incomplete. Final style eligibility is evaluated from completed creation
  data, not an assumed established parent. No USER/GDI window initialization
  occurs in DllMain; only finite interception setup belongs to that boundary.
- Initial caption publication follows successful native creation processing;
  dynamic WM_SETTEXT goes through the retained application chain. S2 tests
  rejection and reentrancy, does not declare every setter successful.
- WM_NCDESTROY detaches the style callback without replacing an application's
  newer subclass; keep state alive through nested callbacks and release once
  after the final active call. Failed creation has explicit scope rollback.
- An application replacing WNDPROC is supported when it preserves the normal
  previous-procedure chain. Do not continually rehook or force appearance when
  an application intentionally bypasses that chain/custom-draws its frame.

Microsoft documents CBT CREATE before WM_NCCREATE/WM_CREATE and notes parent
and final geometry may not yet be established; subclass helpers are same-thread
only. These constrain the design, not proof of our future implementation:
[CBTProc](https://learn.microsoft.com/en-us/windows/win32/winmsg/cbtproc),
[SetWindowSubclass](https://learn.microsoft.com/en-us/windows/win32/api/commctrl/nf-commctrl-setwindowsubclass).
S2 fixtures must verify A/W, dialogs, later threads, nesting, failed creation,
application subclass order, and destruction against the actual adapters.

## Title-marker contract: visible decoration, not API virtualization

Select the simple real-caption marker `(Win16 Style) `, not a new overlay
renderer merely to validate S2. Store the application's accepted source title
separately; format the display from that source, never by prepending to the
already formatted display. Repeated installation or repaint does not append
another prefix. An empty source title still displays the marker.

Do not strip an application-authored matching prefix or infer policy from text.
Exactly one prefix is owned by the library; a source title may coincidentally
contain the same words. A guarded internal title update must not become another
application source update or recurse indefinitely. Restore only library-owned
decoration on an explicit detach of a still-live window; destruction needs no
cosmetic restoration. ANSI conversion uses the window's native string contract.

Important limitation: changing the real caption is observable to GetWindowText,
WM_GETTEXT/length and exact-title lookup; do not promise those still see the
undecorated title. Do not add GetWindowText/FindWindow virtualization to hide this
opt-in diagnostic marker. This narrows the earlier plan's overly broad phrase
"preserve ordinary application title semantics": preserve correct documented
API behavior, not identical returned text after changing a caption. Fixtures
must exercise reads and read-modify-write explicitly. Programs depending on an
exact unmarked caption are a compatibility boundary, not silently passed.
[GetWindowText](https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getwindowtextw).
The marker proves the common enable decision, not the completed Win3.1 style.

## Historical reuse, geometry and appearance policy

The new launcher policy/title witness is an owner-requested product feature,
not a missing original OpenNT implementation. No original source provides these
CLI flags. Keep original task/USER callbacks and HWND behavior in place.
Original NTUSER frame code depends on the NT4 USER/GDI server/desktop internals
and is not Windows 3.1 pixel authority; importing that server or emulating it
through a broad facade violates the stopping boundary. No external runtime
patch is needed. The bounded new library uses public USER/GDI around the
existing windows; historical references establish visuals, not imported code.

The preferred S2 implementation changes project adapters/build bindings only.
The WOW child-creation call needs a narrow per-translation-unit facade binding;
do not globally redefine CreateProcess. If a mirror edit is unavoidable, stop,
name/register its exact minimal seam and review it before implementation. S1
does not promise zero mirror diff without that build proof or authorize guest
changes. Existing original parent fields may be read, not modified for policy.

S3 chooses one layout for both painting and hit tests. It keeps client origin,
usable size and input correct across frame changes, and forwards system commands
and sizing constraints to the original owner. Do not erase all non-client area
by copying the DWM sample. No application DPI-awareness or system-wide colors,
drag settings or shell policy are changed. Scale logical style metrics at the
window's applicable DPI, rebuild local resources on changes; high contrast uses
host accessibility colors rather than forcing a historical palette. Pixel/font
measurement is S3 work, not an S1 runtime acceptance claim. S4 covers real menus,
MDI merged title/menu semantics and native fallback. Standard host minimize and
move behavior remains: this does not recreate Program Manager desktop icons.

## Disposition and closure checks

S1's design is complete with explicit implementation tests assigned to S2/S3/S4.
No implementation test has passed merely because its design is written. The
remaining obligations are in the [working plan](../operations/t445-win31-window-style-plan.md).
No additional owner architecture choice is needed to begin S2 after admission.
Any failed boundary proof still stops that implementation rather than authorizing
a helper, guest modification or process-global fallback.

Independent final review maps ownership, options, inheritance, title contract,
no-NTCON boundary and source placement to the owner requests. Documentation
governance, links and diff checks run as an owned hidden background process
under build/M0-T445/S1/r005 with a 120-second deadline. This documentation-only
closure needs no product build/publication; accepted release 0.0.444 is unchanged.
