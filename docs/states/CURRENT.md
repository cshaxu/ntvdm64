# Project Status

## Current Work

**No active M/T/S packet.** T481 is closed by owner approval.

See the [T481 closure and retained boundaries](../history/m0-t481-closure.md).
S21 consolidates driver handling into win95-launch's dynamic WIN.CMD route,
removes standalone win-multitask and its obsolete callers, and retains local
restoration of old installation backups. Win31-setup and Win101-setup are
unchanged. The Win95 Setup proposal is queued, not admitted.

No subsequent task is active. See [Queue](QUEUE.md) for candidates.

## Current Technical Baseline

Published package: 7773a9b4-e360-4469-962a-df26bb16faf4, all 11 images
hash-verified in assets/release and O:/winnt/system32. S20 cached x86/x64
builds and focused tests passed. S21 repeated cached builds, focused launcher
tests and all 16 selected Full gates successfully. External
DOS4GW/NESticle is a missing-fixture skip. S20 P1 is 843cae6b4.
This closure changes Win95 tooling, tests and documentation; runtime, addons, original guest,
winnt.zip and installed Windows configurations are unchanged.

The confirmed DOS program identity/wrong-waiter defect and approved Win95
exit-code boundary are retained in [TODO](TODO.md), not declared repaired.

## Recent M0 Closures

- [T481](../history/m0-t481-closure.md): guest multitask delivery, launch profiles, mouse repairs and test cleanup; explicit retained boundaries.
- [T480](../history/m0-t480-closure.md): adjacent mirror audit closure.
- [T479](../history/m0-t479-closure.md): guest multitask transferred to T481.

## Recent Governance

See the [execution rules](../rules/EXECUTION.md) for current governance.
