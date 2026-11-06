# T479 S5 P4 shared command text and frontend startup

This checkpoint retains the approved physical-host COMSPEC interpretation and
shares command/environment preparation between ordinary COMMAND and saved-VM
requests. It does not close S5.

## Implementation and ownership

The existing NTVDM command component now owns OEM-to-ANSI command conversion
and delegates environment conversion to original cmdXformEnvironment. Ordinary
cmdCreateProcess and VM preparation share it. The ordinary wrapper retains its
standard-handle binding, suspended creation, resume, wait, result and reentry
ordering; the VM scheduler retains per-VM identity and wait/wake ownership.
No original guest binary or source is changed or rebuilt.

A separate startup defect lost short DOS descendants' output on frame-only VM
frontends. The original runtime enabled Console stream output before the later
video tick queried frontend capabilities. Trace208 observes rejected writes
with ERROR_NOT_SUPPORTED; the original stream writer ignores that failure.
The authenticated frontend requirement is now queried before original runtime
startup and selects the existing StreamIoSwitchOn option. Native Console
frontends keep their stream optimization. No sleep, new renderer or command-name
special case is introduced.

## Evidence and limits

[Source audit](../operations/m0-t479-s5-cmdexec-reuse-audit.md) and
[hash-bound results](m0-t479-s5-text-preparation.json) retain failed controls,
positive witnesses and implementation identities.

- Shared text composition passes 42 assertions using real RTL conversion.
- Run199 fails; P3 command-source control203 also fails. These are retained.
- Trace205 proves missing output was never committed; trace208 attributes it
  to unsupported stream writes, not a late-frame overwrite.
- Fixed209 and separate210 without the extra trace wrappers pass actual MEM
  output in both Win3.1 VMs, 320 callback-state checks and normal Windows exit.
- Complete cached x86/x64 build211 and all 16 selected Full release212 gates
  pass. External DOS4GW/Nesticle fixtures are explicitly skipped, not passed.
- Package 6c79f45b-88bd-4359-a9c8-15e3a753457e is published at assets/release
  and O:/winnt/system32; all ten images and the hook license match. Diagnostic
  workers are excluded. The independently authored addon remains unchanged.

The local mirror change is 31 additions / 70 deletions in cmdexec.c, registered
under MVDM-HOST-DIV-196. Autonomous production changes, including the new header,
are 57 additions / 6 deletions; test changes add 14 lines. This is relative to
P3, not the upstream mirror, and excludes documentation and release binaries.

Physical keyboard, complete I/O and fonts remain S6. CF=1 same-guest reentry
still lacks a production witness and remains S5; host DOS descendant success
does not replace that proof. Win95 remains deferred. Concurrent queue/proposal
edits are preserved outside this delivery.
