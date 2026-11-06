# T478 S36 shell, hook and subclass investigation

Parent 9023619d3; finding35. Final bounded disposition is recorded below; earlier checkpoints preserve their then-current state. No product changes or new
publication. Admission governance passed in background PID36232.

## Existing ownership and reproduced evidence

The original wshell.c retains ShellExecute conversion and RET_WINEXEC callback;
wow_shell_callback.c adapts the modern three-slot provider callback to the
original two-slot signature, saving/restoring synchronous TLS in finally.
The original wuhook.c/wowhooks.c owns hook slots; wkman.c calls
W32FreeOwnedHooks at task exit. Source presence alone does not prove retirement.
Original wsubcls.c retains native-to-guest procedure thunk creation.

Existing shell and hook provider tests ran in owned background supervisor21396:
`verify-wow-shell-callback.ps1 -AdapterProviderBuildRoot
build/.ninja/msvc-x86-release/wow32` with and without `-OuterBoundary`, and
`verify-wow-hook-native-binding.ps1`. Roots are build/t478-s36/shell-adapter,
shell-unwind and hook-binding. All exit zero. The actual modern Shell provider
passes nested callback and inner/outer exception TLS restoration, including a
subsequent call after unwind. Hook binding has errors0, calls24, balanced108/108
handles. That fixture controls its native-call seam; it is not real hook proof.

## Repaired test entry, unchanged product

The earlier audit references verify-win16-hook-probe.ps1, deleted by db91004e5
when project Win16 Style was removed. The independent shell/hook probe sources
remain. The new verify-win16-shell-hook-probe.ps1 recovers only the two original
case assertions from that script's parent, with no Style options or revival.
It uses the current proven physical-stage, fresh package identity, medium token,
original logon environment and bounded background observation path. It assembles
only independent NE tests; it does not compile or modify original guest media.

Background supervisor33172 ran shell then hook on the unchanged S34 package,
with Observer build/t478-s34/observer.exe and LogonHelper
build/t478-s34/logon-env.dll. Fresh BuildRoot values are
build/t478-s36/guest-shell and guest-hook. Both exit zero.

- Shell: phase0/1/2 errors0; success5974, missing-target failure2; child phase3;
  original RET_WINEXEC before/after occurs on the same task/thread. Parent
  continuation and normal launcher exit are asserted.
- Hook: phase0/1/2/4 errors0; created4, queried4, nested1, vetoed1,
  destroyed3, explicitly unhooked1 and freed1. This proves real original CBT
  nesting/veto/recovery and explicit removal, not implicit task-exit cleanup.

[Progress receipt](../../../artifacts/mvdm-audit/shard-0/shell-hook-s36-progress.json)
freezes input/result hashes and transcripts. Existing production-object
window-procedure, native-nested and task-lifecycle tests were subsequently
started serially by supervisor32404, build/t478-s36/lifecycle.ps1; each uses
its checked-in real deadlines. These are supplementary to real Win16 proof.
Current image binding and original task-owned hook cleanup coverage remain open.
No S closure or all-tests-passed claim is made.

## Lifecycle runner repair checkpoint

Supervisor32404 failed at link before execution: its stale archive list omitted
ntvdm_vm_host_create/close/destroy and win32_process_image_machine providers.
The current production NTVDM graph selects ntvdm-vm-frontend,
common-frontend-base, base-rpc-client-core/launch and lib-process-image. The
fixture now links those same providers, rather than adding test stubs. Product
sources and published artifacts remain unchanged.

Supervisor21844 reran in fresh lifecycle2-* directories. All three cases passed:
window-procedure (native subclass forwarding/restoration, transferred CallProc
encoding and independent destination lifetime), native-nested, and task-lifecycle
(native execution scope nesting, queue and quit ordering). These selected-object
fixtures supplement the real Shell/CBT guest cases; they do not establish that a
Win16 task leaving its own hook installed is cleaned up by W32FreeOwnedHooks.

Current image observer attempts PID37984 and4516 stop on the generic COFF binder
assertion for _ThunkMsgFilterHook16@16 in wowhooks.obj. This is an unresolved
observer representation issue, not evidence of a missing production function.
Do not silently skip it or claim all image binding passed. The original three
files reached that point without an assertion, but no complete image receipt
exists yet. Next work: inspect that symbol's body/relocation representation and
complete implicit task-owned hook retirement evidence. S36 stays active.

## Final ownership and verification disposition

No product gap was demonstrated for finding35. Existing original Shell/hook/
subclass paths are retained; no duplicate replacement implementation was added.
The new tests restore reproducible coverage rather than adding a product feature.
[Final receipt](../../../artifacts/mvdm-audit/shard-0/shell-hook-s36-results.json)
freezes the passing results, inputs and all ten unchanged release identities.

The COFF failure was an observer limitation: ThunkMsgFilterHook16 contains
IMAGE_REL_I386_SECREL (0Bh) four-byte TLS relocations in addition to DIR32 and
REL32. The S36 observer now masks those actual linker-resolved words and still
compares every other byte of each complete function section. Its final PID34208
run passes 84 bodies: wshell22, wsubcls2, wuhook5, wowhooks53 and wkman2.
The latter two are W32DestroyTask and the registered renamed original
WowOriginalFreeTask, whose COFF includes the call to W32FreeOwnedHooks.
The failed expanded run assumed the pre-adaptation W32FreeTask symbol name;
source and current map established the actual owner, without dropping the check.
Source hashes show wshell/wuhook identical to both originals; wsubcls/wowhooks/
wkman retain existing registered adaptations. No mirror diff was added by S36.
Static binding does not prove every hook type or every Shell association.

### Actual original hook cleanup

`verify-wow-task-lifecycle.ps1 -Case original-hook-cleanup` now selects
wow_original_hook_cleanup_fixture.c, linked against the current original
wowhooks.obj owner. It allocates original table entries for two task IDs and
installs real native thread CBT hooks. Calling original W32FreeOwnedHooks for
one ID removes its actual USER hook (the old handle rejects another unhook),
clears only its slots, preserves the other task's callbacks, tolerates an empty
repeat according to original behavior, and reuses a released slot. A NULL hook
entry proves the original rule that table state clears even if native unhook
fails. This is a reproduced original rule, not a new retry/failure guarantee.
Final supervisor37892, fresh original-hook-cleanup3, exit0 and:
`WOW_ORIGINAL_HOOK_CLEANUP errors=0 removed=1 foreign_preserved=1 failed_unhook_cleared=1 reusable=1`.

Retained test-construction failures: PID39728 compiled the fixture with cdecl
while original declarations require the selected /Gz default, producing five
unresolved original function names. PID39184 corrected that ABI and exited0 but
lacked its required marker: the link map resolves printf to the original fprt
host sink, so stdout was empty. The final fixture uses the established stderr
reporting path; no product or semantic expectation was weakened.

### Real task exit without explicit unhook

The independent hook probe has a HOOK_LEAVE_INSTALLED variant that jumps over
its explicit UnhookWindowsHookEx/FreeProcInstance calls, retaining the original
import relocation sites. Supervisor35312 runs that variant through the same
real private-desktop package runner: all four guest phases report errors0,
nesting/veto/recovery counters remain4/4/1/1/3, unhooked/freed remain0, and the
launcher exits normally. Initial PID29088 could not assemble because removing
the call sites broke the hand-authored NE relocation list; no guest executed
in that failed attempt. The corrected variant preserves unreachable call sites.

The guest test proves exit with a live hook, not direct per-slot observation
inside the guest process. Slot removal/isolation/reuse is separately demonstrated
through the selected original body and real USER hooks; current source and image
connect that body to original task retirement. This distinction is intentional.
No arbitrary forced-termination or failed-native-unhook recovery is claimed.

### Scope and package

Together with the previously passing real Shell child/parent, CBT nested
creation/veto/explicit unhook, actual shell provider nested/exception TLS scope,
and production-object native subclass/CallProc/task lifecycle cases, these
results satisfy the bounded finding35 audit. Known original failure behavior
remains original; no unsupported tolerance is introduced. General DDE/multimedia
belongs to S37 and is not inferred from the shell/hook tests.

Only independent tests, observation tooling and evidence changed. The exact S34
package 03d9792b-753e-4407-8a52-883ad9bc83e3 and its Full16/16 remain current;
all ten assets/release hashes equal that tested package. No redundant product
rebuild, Full rerun or republication is needed for test-only changes. No O:/winnt
write or push. S37 is next; broader T478 inventory reconciliation remains open.

Final governance PID33076 passed, as did git diff --check. The default (explicit-unhook) probe reassembled byte-for-byte equal to the previously passing guest-hook/CBTPROB.EXE, so the new exit variant does not change that established input.
