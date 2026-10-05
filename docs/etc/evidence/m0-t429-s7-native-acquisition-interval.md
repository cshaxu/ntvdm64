# T429 S7 native acquisition interval

## Request and source judgment

Owner requests20ms rather than30ms native sampling, deferring event-driven
Console observation. Baseline is Pa47979990 / S6 r006/runtime. The only
production change is the project-owned presentation_loop timeout in
src/ntvwm-exe/main.c. This loop checks admission, frontend input and native
Console capture; shared worker-base publication still caps sends at50Hz.
Original OpenNT/MVDM code, protocols and lifecycle remain unchanged.

Twenty milliseconds is a wait timeout, not a fixed-period deadline: capture,
input RPC and scheduler time add to the loop period. This reduces configured
waiting and increases potential acquisition work; it neither synchronizes
the publisher phase nor proves a measured performance improvement.

## Verification

Only presentation_loop's wait changes30 to20. The wait-set order, membership
lock, input/capture order, original guest code and shared publisher are intact.
Input/acquisition remain coupled; owner asks whether these should be separate,
but that question does not authorize expanding this interval-only delivery.

- Incremental x86 MSVC14.43/SDK22621 /MT CCPU40 build uses the validated
  M0-T427/S2/r001 dependency cache; S7/build2.log compiles only native main
  and links ntvwm.exe. Seven sealed S6 runtime files are reused unchanged;
  S7/r001/runtime is the coherent candidate. build.log retains an initial
  invocation rejected for a nonexistent test target before compilation.
- tests/observation/verify-native-acquisition-interval.ps1 checks the actual
  production20ms literal and unchanged shutdown/release wait-set (source
  contract only), passing in S7/source-check.txt.
- tests/observation/verify-worker-neutral-input.ps1 with the original
  T427/S4/r049 observer and cache runs on private Consoles: all seven cases
  pass in S7/r002, including501 native presentation and689 input-return checks.
  The earlier direct fixture invocation without its private Console has five
  geometry/final-capture assertion failures in S7/native.txt. That fixture
  does not link changed main.c; its proper isolated rerun above passes without
  altering source or assertions. Preserve the failed invocation as evidence.
- Unchanged production-linked S6/r007/publisher-test.exe passes in
  S7/publisher.txt: latest-of200, >=20ms, idle, palette, final drain, resume,
  sticky failure and50 handle-clean teardown cycles. No publisher source change.
- tools/audit/Invoke-ProductVerification.ps1 -Suite Product uses S7/r001/runtime,
  the cache and original T427/S4/r049 observers, original T425/S9/r008/G7.COM,
  and retained S6/r009/product and S5/r028/product WOW baselines. S7/r003/product
  passes in203795ms: WOW67404ms, Console17=63221ms, Window17=67312ms. No case,
  assertion or timeout changes. WOW proof retains frontiers, not gameplay.

- S7/r004/results.json passes all eight existing cases in115061ms aggregate:
  native/nested Console/nested Window real exit23, modern EDIT Ctrl+Q and
  DOS return, cooked outer CMD19, frontend close, unexpected worker loss1067
  with surviving native target, and independent sessions. Entrypoints are
  unchanged verify-broker-io-handoff.ps1, verify-command-native-edit-return.ps1,
  verify-frontend-relaunch.ps1 and verify-ntvwm-management.ps1; the retained
  S6 runner is reused with only its candidate root changed.
- S7/r005/published-manifest.json verifies all eight actual installed files;
  recovery is r005/recovery/system32. Only ntvwm.exe differs from S6, SHA256
  6AF71B8F7BC5136CE4414106A33D2193AC859AC88A5BF91E9052EF6411456F1A.
  The initial smoke wrapper incorrectly tried to create an isolated-package
  cleanup scope for O:/winnt and was refused before any runtime case. Preserve
  smoke.log; use the established published-smoke entry instead, not a weaker
  cleanup check.

- S7/r007/published-smoke.ps1 passes actual deployed Console4/Window4 and
  all eight hashes (smoke2.log); runtime logs have the fresh
  O:/winnt/Logs2/t429-s7-r007 prefix. No test ownership boundary is bypassed.

Final source review finds exactly one production literal changed; shutdown,
release, membership locking, native input conversion, copied acquisition,
shared50Hz cap and final drain are unchanged. No original mirror file changes.
The candidate reuses seven hash-identical S6 files; only native main recompiles.
Source-contract, governance, links and diff checks pass. This reaches bounded
S7 closure, not T429 acceptance. Physical latency, acquisition CPU cost and
RDP behavior are not measured/passed; no speedup is claimed. Input/Console
acquisition remain serial, and a future separation needs its own audit/admission.
All disposable results belong under build/M0-T429/S7. Side-session proposal
hash remains AE9DB32FD31A87EFC07FF895DF66CB3FFDFD17ECBED6B1FBE4391152A2ADF9AB;
it is preserved and excluded from this delivery. Existing baseline limitations
remain as recorded, not reclassified by the20ms change.
