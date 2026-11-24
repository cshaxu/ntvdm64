# T484 S3 closure and S4 handoff

## Bounded conclusion

S3 closes addon and test-binary production migration, not the complete T484
build/release objective. Its delivered changes are indexed in
[t484-s3-early-deliveries.md](t484-s3-early-deliveries.md) and P214-P217 records
linked from Current. S4 now owns configuration, isolated package preparation,
manifest generation, release composition and their callers. S5 still requires
clean interpreter-free execution, cache/failure cases and final acceptance.

P217 at 0cb40e7e2 is the final S3 implementation. Its final run built all14
addons and the complete host set; addon raw/release hashes and11 host raw/22
published hashes matched. No new product bytes required copying. The native
mouse callback and real selected DPMI tests passed P216; current Win31 CMD
apply/repeat/reject/restore and exact adaptation checks passed P217. Individual
producer behavior, cache and source-selection checks remain in their delivery
records. This closure is a source/call-chain reconciliation, not another Full
runtime pass or proof of a clean interpreter-free environment.

## Final call-chain reconciliation

The frozen355 inventory retains323 disposed and32 unresolved entries. They
remain unresolved at T scope; there is no artificial increase in completion
for assigning them to S4. Additional24 preparation chains overlap the inventory
and are not24 additional unique compiler sites.

Direct compiler searches of interpreted test/tool files found only false
positives (clipboard-epoch delegates to CMD; monitor close calls are runtime).
CMD build entrypoints and native CMake descriptions had no executable forbidden
interpreter reference; /nodefaultlib is a linker option, not Node. These text
searches support the reviewed chains; they are not standalone clean-build proof.

The two stale outstanding producer descriptions were traced explicitly:

- WOW callback: build-fixtures.cmd wow-callback-cost; unit callback-cost-unit;
  nested mine latency uses wow-latency-probes and logon-environment CMD targets.
- WOW page: build-fixtures.cmd wow-page-cost; nested original EMS uses
  cpu-guest-probes and logon-environment; nested mine latency as above.
- Shared SOL/WRITE, Win31 matrix and VM identity helpers only delegate assembly
  or compilation to CMD/native. Remaining writes are runtime configuration,
  stage population, test stimuli or evidence, to be distinguished in S4.
- Stage-Compat95Release.ps1 checks inputs/evidence and publishes a driver and
  JSON manifest. It has no compiler invocation; it is S4 publication work.

Do not automatically convert assertion/evidence runners or test stimuli to
CMD. Only required build/generation/configuration/package actions are in scope.
Clear-BuildOutput is the explicit safe cleanup owner; S4 must classify its
actual necessity rather than remove protections merely to remove PowerShell.

## Frozen unresolved files handed to S4

| File | Remaining responsibility |
| --- | --- |
| `test/integration/ntvdm64-386/verify-win31-editors.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/ntvdm64-386/verify-win31-identity.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/ntvdm64-386/verify-win95-editors.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/ntvdm64-386/verify-win95-identity.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-broker-io-handoff.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-dos-identity-pair.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-native-guest-output.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-pic-reset-eoi.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-s45-keyboard-shadow.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-s45-vcd-serial-discovery.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-win16-ddeml-probe.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-win16-print-probe.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-wow-callback-cost.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-wow-clipboard-transfer.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-wow-external-native.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-wow-graph-cost.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-wow-idle-workers.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-wow-mine-gameplay.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-wow-mine-latency.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-wow-multi-dialog.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-wow-observer-cost.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-wow-page-cost.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-wow-sol-gameplay.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-wow-sol-options.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/verify-wow-write-lifecycle.ps1` | Runtime configuration/package preparation; native production retained. |
| `test/integration/win31_runtime_matrix_test.ps1` | Runtime configuration/package preparation; native production retained. |
| `tools/audit/Invoke-ProductVerification.ps1` | Package preparation versus standalone assertion runner separation. |
| `tools/build/Clear-BuildOutput.ps1` | Cleanup-owner scope and interpreter-free build prerequisite classification. |
| `tools/build/Stage-AddonUtilityRelease.ps1` | Publication, source/hash checks and manifest composition. |
| `tools/build/Stage-Compat95Release.ps1` | Publication, source/hash checks and manifest composition. |
| `tools/build/Stage-MouseDriverRelease.ps1` | Publication, source/hash checks and manifest composition. |
| `tools/build/Stage-System32ProductPackage.ps1` | Publication, source/hash checks and manifest composition. |

## Supplemental preparation chains

- `test/integration/multitask/verify-win31-empty-profile.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/multitask/verify-win31-progman-dde.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/multitask/verify-win31-setup-modes.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/multitask/verify-win95-guest-only.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/multitask/verify-win95-launch-matrix.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/support/wow-guest-probe.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/verify-command-native-edit-return.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/verify-dem-fcb-guest.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/verify-dem-pressure.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/verify-dlc-provider-boundary.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/verify-dpmi-debug-context.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/verify-dpmi-int21-state.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/verify-forcedos.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/verify-memory-transition-guest.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/verify-mouse-state-checksum.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/verify-redirector-remote.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/verify-vga-cursor-disable.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/verify-video7-mode-flags.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/verify-wow-edit-tabstop-guest.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/verify-wow-generic-call.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/verify-wow-logon-gameplay.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `test/integration/verify-xms-block-guest.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `tools/audit/Verify-T420S37OemGuest.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.
- `tools/audit/Verify-T420S38DpmiFatal.ps1`: native producer delivered; remaining preparation tracked in dispositions.json.

## Retained failures and boundaries

P193 native output loss across guest return remains a product observation, not
fixed or waived. P215 inherited developer-environment EDIT startup fails on
both original and modified runners; native Userenv case passes. No new root
cause is claimed. These do not turn failed runtime cases into passes and must
remain visible in final T acceptance. Original media, O:/w31, winnt.zip and
product runtime code were unchanged by final retirement batches.

S4 priority is the shared physical runtime/package owner, then host/addon
publication and configuration callers. Preserve identity checks, nonzero error
propagation and cleanup ownership. Avoid a second packaging graph or embedding
an arbitrary scripting interpreter in a native tool.
