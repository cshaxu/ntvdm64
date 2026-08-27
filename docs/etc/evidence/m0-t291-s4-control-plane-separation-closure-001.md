# M0 T291 S4 — Control-plane separation closure

## Scope

S4 separated the three reached historical layers before any further WOW
provider recovery: BaseSrv-shaped `GetNextVDMCommand`, kernel-VDM-shaped
`NtVdmControl`, and synchronous SoftPC/monitor `host_simulate` re-entry.

## Completed facts

- `VdmQueryDir` uses a distinct typed monitor-control capability and preserves
  unavailable results for every other control service.
- `GetNextVDMCommand(VDMINFO)` uses an independent typed command-provider
  capability; the generic `session` operation table was removed. The app
  source is explicitly one immediate one-session producer, not a BaseSrv/CSR
  replacement.
- Every selected direct `host_simulate` caller has an owner-specific
  postcondition disposition in the [ledger](../operations/m0-t291-s4-host-simulate-postcondition-ledger-001.md).
- The current WOW task projection is explicitly one-thread/non-fast callback
  foundation only; it does not impersonate the original TD/TEB product shell.

## Verification

- formal MSVC `/MT` x86 and x64 command-provider fixture: `PASS: local VDM
  command transport`;
- formal MSVC `/MT` x86 and x64 `VdmQueryDir` fixture: `PASS: VdmQueryDir
  adapter contract`;
- formal MSVC `/MT` x86 and x64 task-frame and stack-simulation fixtures:
  successful build and zero-exit execution; and
- negative source scan: no remaining `session_dispatch_control`, numeric
  session route registration, or former command-operation wrapper in live
  source/test/build inputs.

## Limitation and handoff

No BaseSrv/CSR clone, WOWEXEC wake-up path, fast WOW or second executor was
introduced. [S5](../operations/m0-t291-s5-source-shaped-command-broker-and-monitor-reentry-plan-001.md)
is active for source-shaped command-broker and monitor re-entry recovery.
