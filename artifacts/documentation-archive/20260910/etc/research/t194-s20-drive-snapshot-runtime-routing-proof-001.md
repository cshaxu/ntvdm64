# T194 S20 drive-snapshot runtime routing proof 001

Date: 2026-08-12  
Packet: M0 T194 S20  
Disposition: focused runtime routing proof complete; no Bochs or guest process
started.

## Fixture

`bx-ntvdm-adapter-runtime-test.exe --t194-drive-snapshot-routing` creates a
self-contained v6 one-slot profile and a temporary four-file root. The profile
deliberately contains a static `host_drive_inventory` that marks C as fixed.
Before adapter installation, the fixture sets the CLI child policy
`NTDOS64_HOST_EXCLUDE_DRIVES=A,...,Z` and the valid one-slot launch plan.

The session capture therefore produces an immutable empty admitted snapshot,
while the profile-static value would have produced a three-drive result if it
were consulted.

## Assertion and result

The fixture invokes only the typed `C4 C4 50 0F` runtime boundary and passed
with these assertions:

- result disposition is `RESUME`;
- resume RIP is the fault RIP plus four bytes (`0x7c04`);
- AX is exactly `2`, the original A/B baseline for an empty captured snapshot;
- CF is the only EFLAGS write and is clear.

This proves the normal CLI route uses the installed snapshot rather than the
conflicting profile-static inventory. The test opens no host path, starts no
engine or guest code, and its only host observation remains the existing
single install-time drive capture.

The separate broad default runtime fixture still exits `1` without a diagnostic
marker. It is not part of this focused proof and remains an out-of-scope
legacy-fixture limitation.

## Follow-up

T194 S21 may build a new adapter-only derivative from the current source,
retaining the S8 r2 Bochs inputs byte-identically. Runtime observation is a
separate later S.
