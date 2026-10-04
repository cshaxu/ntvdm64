# T428 S2 shared GUI worker residency

Owner approved the T428 audit and original-semantic alignment on 2026-10-04.
This bounded repair removes the added ten-second retirement of admitted
GUI-only native workers. It does not complete T428's parent restoration,
management shutdown, exclusive-session or association consolidation.

## Source and recovery decision

Original shared WOW retains WOWEXEC and accepts later tasks; separate WOW
uses `WowSetExitOnLastApp(TRUE)` in
`src/mvdm/wow16/test/shell/wowexec.c`. Original DOS shared/exclusive and PIF
`CloseOnExit` decisions remain in `src/mvdm/dos/command/cmdmisc.c`.
These original translation units remain composed in their existing backend;
they cannot execute native Win32 targets and are not moved into worker-base.
The smallest counterpart here is deletion of the project-added native idle
policy in NTSRV. No imported source change, replacement scheduler or new
worker state is needed; external intrusion and new lifecycle policy are
rejected. Exclusive native counterpart work remains in the indexed plan.

Deleted `service_unbound_native_deadline()`, its deadline selection/firing,
the `unbound_native_deadline` watch field and registration initialization.
Frontend workerless grace, root-loss shutdown, empty-broker grace and launch
timeouts are unchanged. GUI target startup/completion and worker lifetime
remain distinct.

## Verification

All new evidence is under `build/M0-T428/S2`. The validated incremental
MSVC14.43/SDK22621 Win32 x86 `/MT` CCPU40 cache is
`build/M0-T427/S2/r001`; sealed runtime baseline is
`build/M0-T427/S5/r001/runtime`. Only NTSRV changed in the eight-file product.
APP0.0.427, RPC38 and I/O25 are unchanged; no wire change.

| Contract | Entry and retained evidence | Actual result |
| --- | --- | --- |
| x86 affected closure | `r001/build.cmd`, `r001/build-elevated.log`; ntsrv.exe and basesrv-service-reservation-test.exe | Compile/link passed. Existing C4201/C4457 warnings remain. |
| Residency policy and adjacent service behavior | `tests/observation/verify-service-fixtures.ps1`, r002; `--shared-worker-residency` | 22 cases passed, 3255ms. No shutdown/deadline at explicit times before, at and after old expiry; active/idle and native/WOW comparison retained. |
| Coherent full package | `tools/audit/Invoke-ProductVerification.ps1 -Suite Product`, r003 runtime, r004 reports/manifest/timings | Console17, Window17 and three independent retained WOW frontiers passed; 221675ms total. WOW observation is not gameplay acceptance. |
| Actual GUI routing and residency | `tests/observation/verify-native-gui-routing.ps1 -PackageRoot build/M0-T428/S2/r003/runtime -LaunchRoot Z:\ -FullDeadlines`, r009 | Five cases passed: startup0, actual wait37, GUI-to-text, target survives launcher/release and worker survives beyond11s, text-GUI-text19 with output marker. Both controlled GUI exits37. |
| Recoverable publication | r007/publish.ps1, recovery and published manifests | Tested eight files copied to O:/winnt/system32; hashes matched. Guest media/configuration unchanged. |
| Published smoke | r010/published-smoke.ps1; O:/winnt/Logs2/t428-s2-published-console and window reports | COMMAND, MEM, EDIT and native-zero passed in both modes; all eight published hashes remained identical. |

GUI scripts now accept the actual system32 layout and authenticated
test-only physical/short alias cleanup scope. `subst Z:` is removed in
`finally`; no other drive alias is used. Failure cleanup does not prove
product idle retirement. Long-path/premature flat-cache launch in r005
failed before target startup; r006 reached actual GUI and exposed the
separate selection limitation below. Neither failed run is counted passed.

## Remaining limitations

Two independent GUI launches did not select the same resident native worker
in r006. S2 proves retention, not reuse. The original retention assertion
remains tested; a newly attempted same-PID reuse assertion is not claimed
passed. Selection/root association consolidation belongs to the planned
stage, and must preserve session isolation. Shared DOS/WOW execution,
exclusive close-on-exit and native management-close symmetry are not changed
or claimed solved by this repair.

Physical RDP clipping/focus remains owner-waived/unobserved. No new host
scrollback guarantee is introduced. S1 documentation delivery is combined
with this reviewed P; side-session performance planning is excluded.
