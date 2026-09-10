# T211 S5 — COMMAND lifecycle source, ABI and provider map

## Question

How should OpenNT COMMAND lifecycle services `54:00`, `54:03` and `54:0B`
be composed for the CLI profile without replacing the engine, terminating the
host process, or recreating the missing NT command broker?

## Original ownership and current disposition

| Service | Original source contract | Current provider result |
| --- | --- | --- |
| `54:00` | `cmdExitVDM` in `cmdexit.c` calls private `TerminateVDM()` | typed controlled stop only; no `ExitProcess`, engine result or CLI exit claim |
| `54:03` | `cmdSaveWorld` in `cmdmisc.c` has its implementation behind `CHECK_IT_LATER` | original active no-op: resume with no register or host-state change |
| `54:0B` | `cmdReturnExitCode` in `cmdexec.c` accepts `DX`, `AL`, `BX:CX`; it updates state then calls `GetNextVDMCommand` to choose re-entry or final shell return | copied declared-plan return only; re-entry sets CF, final return records low `DX` byte in the existing package-local terminal record |

`GetNextVDMCommand`, `VDMINFO`, BaseSrv/CSR state, event-thread scheduling,
redirector completion and standard-handle restoration are an unavailable
historical composition, not missing ordinary Win32 APIs.  A normal user-mode
Win32 process does not expose a compatible replacement.  The no-delivered-slot
case therefore keeps the prior source-derived no-reentry answer and never
creates a terminal record.

## Ownership repair

Before S5, the two groups were split incorrectly:

- a detached CLI profile helper handled `00` and `03`;
- the launch/execution helper handled `0B` beside `06/07/08/0A`;
- the legacy adapter runtime had a separate `0B` gate.

S5 creates `bx_ntvdm_command_lifecycle_provider_v1`, embeds it in the single
COMMAND package session and moves all three lifecycle services there.  The
launch/execution helper is now explicitly limited to its own `06/07/08/0A`
component.  The detached profile helper and runtime `0B` route are deleted.

This does not move lifecycle semantics to Bochs: all selector recognition and
source-derived behavior remain in `src/bx-vdm`, while bx-core/bx-mantle see
only typed stop/resume results.

## Verification

1. `Invoke-T198S16BootNamespaceVdmX64Probe.ps1 -Fixture command-lifecycle`
   at `artifacts/build/t211-s5-command-lifecycle-r2` source-built, linked and
   ran under MSVC x64 `/MT` (`passed: true`).  It covers all three identities,
   exact stop/no-op outcomes, the no-broker result, final declared-plan copied
   `DX` low-byte record, non-membership and protected-mode refusal.
2. The same probe with `-Fixture boot-namespace` at
   `artifacts/build/t211-s5-command-lifecycle-r3` passed under MSVC x64 `/MT`.
   It verifies the complete bx-vdm composition after the provider migration.

## Limitations and follow-up

The provider does not prove that the current guest path reaches `54:0B`, nor
does it publish a DOS exit code through engine or CLI.  T203/T206/T207 remain
the controlling negative evidence for that missing post-return normal-terminal
sequence.  S5 remains open pending its final provider/failure review and
whole COMMAND package closure; native trace is an integration observer only.

## Final package regression

`tests/bx-vdm/bx_ntvdm_command_package_family_v1_test.c` is the final
T211 ownership invariant.  It iterates every callable COMMAND slot
`54:00..10`, performs the normal ingress/registry/COMMAND-plane/facade
classification and asserts that exactly one of the bootstrap, launch/execution,
console/keyboard or lifecycle providers owns it.  The special `54:10`
bootstrap member is additionally asserted to remain package-deferred.

The fresh MSVC x64 `/MT` run at
`artifacts/build/t211-s5-command-package-r1` passed.  This is an ownership and
route regression; service semantics remain covered by the focused S2, S3, S4
and S5 fixtures.  Together they close T211's COMMAND owner package without
claiming an engine/CLI normal completion or using native trace as an
implementation selector.
