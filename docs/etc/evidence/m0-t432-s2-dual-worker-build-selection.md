# M0 T432 S2 bounded build/selection conclusion

## Outcome and delivery boundary

S2 implements dual native-worker builds, verified native machine selection,
registration/reservation and management projection. This is a bounded
engineering conclusion, **not a delivered production P or complete dual-width
runtime capability**. Matching Hook64 is the declared S3 prerequisite. The
AMD64 installer currently refuses Hook32 injection; it does not silently run
an unhooked direct target. Direct64 execution, text I/O/handoff, retained full
product/WOW gates and coherent publication remain required in S3–S5.

The owner-accepted T431 package at O:/winnt/system32 is unchanged. S2 source is
uncommitted; no production P, push or publication is claimed. Other-session
proposal/TODO edits are preserved. This dependency disposition uses the
bounded-conclusion/deferred-P rule in EXECUTION and S2's admitted exit criteria.

## Source and ownership audit

- `common/image_classification` extracts project native metadata querying from
  Run16; it retains SEC_IMAGE/NtQuerySection ordering behind the finite Base
  classifier facade. Host-native SIZE_T declarations compose both widths;
  original OpenNT/MVDM bodies are not rewritten or relocated.
- NTSRV uses the existing reservation, connection, worker-watch and direct
  record owners. Requested machine is restricted to I386/AMD64; prepared
  process, connecting worker and bound target are independently checked by
  actual Windows process machine. Reuse filters machine before selection.
- Native backend duplicate registration is now keyed by root and machine:
  NTVWM32 and NTVWM64 may share a logical frontend, but duplicate same-width
  binding remains rejected. NTCON gains no worker-width policy.
- NTMON reads service WIN64 kind3, rather than guessing from image names.
  Existing DOS0/Win16 1/Win32 2 categories remain unchanged.
- APP0.0.428/RPC39/I/O25 is the candidate identity. RPC major and application
  protocol advance together; the accepted baseline remains427/RPC38/I/O25.
- The formal x86 product graph delegates both production worker names to
  `New-NativeWorkerNinja.ps1`, a single source family with isolated x86/x64
  objects and MIDL stubs. Both native profiles pin MSVC14.43 /MT and use the
  existing common/worker-base mechanisms, not original guest execution bodies.
  Parent output copies were verified hash-identical to profile outputs.
- The package builder preflights the whole source set. Dual mode requires
  both workers and both hooks, checks architecture and EXE/DLL type, retains
  baseline guest/configuration and removes only obsolete ntvwm.exe within the
  checked fresh build tree. Legacy mode remains for retained baseline fixtures.

## Reproducible evidence

Reusable formal cache: `build/M0-T427/S2/r001`; S2 run evidence:
`build/M0-T432/S2/r001`. All locally generated products remain under build.

| Entrypoint / arguments | Actual result and scope |
| --- | --- |
| Formal `run-ninja-parallel.cmd ntvwm32.exe ntvwm64.exe` | Both native profile compile/link and final copy pass; separate architecture objects/stubs/maps. Not Hook64 proof. |
| `native-machine-test.exe <native32> <native64>` at both widths |111 checks each; reciprocal actual image/process metadata and steady-state handle delta0. |
| `run16-image-classification-test.exe <GUI32> <native32> <command.com>` |227 checks, zero failures/delta; retained original-shaped DOS/GUI/CUI classification. |
| `nthook-install-test.exe` |93 assertions; retained Hook32 actual CMD/nested/A-W/resources/context/negative paths. |
| `ntvwm-execution-lifetime-test.exe <report>` |1077 checks, zero failures/remaining handles; retained startup/cancel/target-survival rules. |
| `worker-identity-version-test.exe <real-NTSRV> <T431-run16> --native-workers <GUI32-fixture>` | Real RPC rejects old application, wrong protocol and old RPC client; service creates I386/AMD64 workers, projects READY/WIN32/WIN64, new connection reuses same PID, selected-width change rejected. Actual32 GUI target executes twice with loaded Hook32, real process exit0 and service receipt. No text I/O or direct64 claim. |
| `basesrv-service-reservation-test.exe --native-width-root <native64>` | Production registration accepts actual32/64 process identities at one root and rejects another32. Trusted seeded watch/admission fixture, not RPC authentication proof. |
| Same service fixture `--native-worker`, `--management-gui`, `--management-gui64 <native64>` | Reservation/root-loss/rundown and actual target projection/close pass; wrong-machine target refused before record mutation. |
| `verify-system32-package-staging.ps1` with accepted baseline/cache/WOW32, fresh S2/r002-staging and NativeWorker32/64/Hook32 inputs | Legacy staging/overlay/conflict/mixed-layout positives and negatives pass. Incomplete dual set, swapped worker width and EXE-as-Hook64 are refused before creating output. Final eleven-image positive awaits real Hook64. |

Raw reports include rpc-native-direct32.log, native-width-root.log,
native-worker-formal.log, management-gui-formal.log,
management-gui64-formal.log and r002-staging/results.json. Earlier focused
reports remain enumerated in CURRENT. Exact fixture-owned process handles
and creation times bound cleanup; forced fixture cleanup does not prove
ordinary retirement. The first failed reuse attempt incorrectly retried the
same unconsumed creator reservation; the passing case uses a fresh authenticated
connection, as a new launcher does. No production assertion was weakened.

## Open gates inherited by S3–S5

Matching Hook64/context-only x86 Run16 and actual64 direct execution; four
installation directions and true-child semantics; real native32/64 text I/O,
DOS/WOW parent return, same-root cross-width handoff and isolation; complete
retained runtime gates and eleven-image publication. No unexecuted gate is a
pass. S2 does not supply a deployable substitute for the accepted baseline.
