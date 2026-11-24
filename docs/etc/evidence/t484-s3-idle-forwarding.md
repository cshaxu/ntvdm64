# T484 S3 P183 private native idle forwarding

## Reconciliation of B0166

P101 migrated five creation fixtures but its native idle witness failed at
SwitchDesktop, before any next-hook notification. Production has two distinct
contracts: WK32ForegroundIdleHook forwards through real USER CallNextHookEx
after leaving its borrowed guest scope; wow_user_idle_hook_dispatch is a direct
taskman call and must not enter that chain. Automatic foreground-idle event
generation is a Windows desktop prerequisite, not a build producer contract.

Replace the activation/timer/window-dependent witness with a deterministic
WH_GETMESSAGE trigger on a test-owned nonactive desktop. Both thread hooks are
owned by the fixture. The first enters the actual production
WK32ForegroundIdleHook with the real queue payload, and production
CallNextHookEx reaches the next real USER hook. Assert exact next-call counts,
registered/unregistered dispatch, closed borrow scopes, propagated return47,
and no next-call on direct taskman dispatch. Also install/remove the actual
WH_FOREGROUNDIDLE hook to check registration lifecycle. No SwitchDesktop,
foreground window, user input, RDP unlock, product change or guest modification.

This is explicitly NOT evidence that Windows automatically generated
WH_FOREGROUNDIDLE; that old broader claim is withdrawn, not reported passed.
The output states trigger=private-queue and foreground_notification=not-exercised.
Mocked task/borrow scopes still do not prove production scheduler ownership.
The test's purpose is bounded registration/dispatch/USER-chain forwarding.

## Verification

PID17060 fails to link because the new PeekMessage still resolves through the
WOW private macro. Undefine PeekMessageA like the other native API selection.
PID12300 then fails the chain because the trial passes a null message pointer
into a WH_GETMESSAGE chain. Keep its failure logs; use the real queue arguments
unchanged and assert their arrival at the provider. This fixes the test, not
production. No failed attempt is counted as passing.

PID27444 in build/t484-s3-idle-chain-verified passes all five cases: hook
registration/rollback, menu/sizing, timer30, idle scope5 plus two real USER
forwardings, and dialog cleanup/resource ownership. LogicOnly remains explicit
and is not used for this run. Final cached full/publication identity and
governance receipts are in the same directory.

B0166 is reconciled as a delivered native producer with this bounded scope;
frozen census316/355 disposed,39 remain. Two supplementary matrix caller
chains outside that frozen census still require configuration/staging. S3 and
the whole T remain active, and S4/S5 obligations are unchanged.

PID27444 completes in24.4seconds; cached full/publication PID26340 passes
in63.4seconds. Initial governance PID17304 passes document validation but
fails diff-check on an extra EOF blank line in the moved historical index;
remove that blank and rerun the gate. A global rescan also finds18
integration scripts with direct NASM commands, beyond the two supplementary
matrices already handled. These are candidate live/obsolete producer edges
requiring reconciliation, so the frozen355 count cannot establish S3 closure.

Remaining direct-NASM scan (not yet individual necessity decisions):

- `test/integration/support/wow-guest-probe.ps1`
- `test/integration/ntvdm64-386/build-interrupt-observer.ps1`
- `test/integration/multitask/verify-win31-empty-profile.ps1`
- `test/integration/multitask/verify-win95-guest-only.ps1`
- `test/integration/multitask/verify-win31-progman-dde.ps1`
- `test/integration/verify-dem-fcb-guest.ps1`
- `test/integration/verify-dem-pressure.ps1`
- `test/integration/verify-dlc-provider-boundary.ps1`
- `test/integration/verify-dpmi-debug-context.ps1`
- `test/integration/verify-dpmi-int21-state.ps1`
- `test/integration/verify-forcedos.ps1`
- `test/integration/verify-memory-transition-guest.ps1`
- `test/integration/verify-mouse-state-checksum.ps1`
- `test/integration/verify-redirector-remote.ps1`
- `test/integration/verify-vga-cursor-disable.ps1`
- `test/integration/verify-video7-mode-flags.ps1`
- `test/integration/verify-wow-generic-call.ps1`
- `test/integration/verify-xms-block-guest.ps1`
