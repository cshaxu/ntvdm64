# T477 S2 frontend-instance implementation evidence

## Question and baseline

Implement the approved [S2 design](../operations/m0-t477-s2-frontend-instances-design.md)
without changing original task ownership, direct I/O ordering or native Console
return behavior. Baseline is S1 delivery 6caaf5d78 and S2 admission 49c25a530.
Owner authorizes sequential P1/P2/P3 builds, tests, publication to this worktree
assets/release and O:/winnt/system32, and local commits without push.

## P1 ownership separation

OPENNT_FRONTEND_INSTANCE owns capability/retirement notifications, active I/O
route and frontend admission/close state. Native Console lease, join and return
acknowledgement details are grouped inside that instance. The authenticated
process, worker role, launch reservations and caller's wait-for-return receipt
remain in OPENNT_BASE_CONNECTION. Route and Console-context pointers now refer
to the instance and use its explicit owner when they need a process resource.

P1 stores the existing native instance in one stable connection-owned slot.
It preserves the old RPC and native entry points; it does not implement the
multi-instance registry or claim VM isolation. P2 owns explicit registration,
selection and lifecycle, and P3 owns downstream admission/association.

The source review caught launcher return receipt fields initially grouped with
the frontend and corrected that ownership before final verification. Existing
service fixtures were updated for private field/pointer layout only; their
behavioral assertions remain intact. The first additional x86 fixture build
identified old field references in base_service_reservation_test.c; those were
corrected before the final rerun. Rejected/interrupted build logs remain under
build/t477-s2 with field-review/fixture-review suffixes.

## P1 verification and publication

The coherent ten-image product build passed in
build/t477-s2/p1-verification-gui-path.stdout.log. The final test-only GUI-path
correction was rebuilt in p1-verification.stdout.log; the product sources did
not change. Final header comment restoration preserves ownership invariants
without changing tokens compiled into the images.

- frontend-base channel and service units passed on x86 and x64.
- The actual NTSRV service archive passed all 40 retained cases on each width
  (80 executions), including return ordering, root/worker/request loss,
  management close order, admission authority and completion barriers.
- The GUI projection fixture now selects the existing x86 GUI probe in the
  worker tree; x64 retains its existing suspended NTSRV target. The real PE
  subsystem/machine and production BindNativeTarget checks remain enforced.
- All six frontend integration fixtures passed: Console presentation/video,
  channel lifetime, frame failure, text handoff and scope lifetime.
- The x64 native-service-layout fixture passed with actual exit 0.
- Full product verification passed all 16 selected gates on the same ten-image
  package. See p1-verification/full/timings.json. Existing DOS4GW exclusion and
  WOW startup-only acceptance boundaries remain unchanged.
- Documentation governance with self-tests and relative links passed; diff
  whitespace check passed. Final delivery documents are checked again before
  the local commit.

Publication driver publish-part.ps1 -Part p1 exited 0. Both this worktree
assets/release and O:/winnt/system32 match the candidate images and existing
hook license. No installed project process was running at publication. Guest
and configuration bytes were not copied. p1-publication.json and exact-file
backups retain delivery and rollback evidence. No push.

| Image | Published SHA-256 |
| --- | --- |
| run16.exe | CA47897F8AB74202D6FB3236BC38CA6E3D95B690541B42FF917B4523BDAC6839 |
| ntsrv.exe | 8C7ACFFEA9E62471ECACCCEF3ECFBE2F0A542AAB62728D18209735D0F9DAB6CF |
| ntcon.exe | 915FC591E7EF07FAA5A240E646A0C32A6D0AA148C9CE75A8C6D043F8E142A959 |
| ntvdm.exe | C4E64624E7B42BDBADBD708AEC8861D56A0D6594ED3A4749CB2525E50DB506F9 |
| ntvwm.exe | 583B28EC0AA1E5D89661A9BCD71EFBDE76278F0270445898D090B01AAF2D7897 |
| ntmon.exe | 875C9439373B5BFBBA95A167208BF6F1DFFC449B09B0097788F83BE10D1E1B0D |
| WOW32.DLL | 3DDC4FEC4A380712C6075DADC6C1FF7B23BA71425F7CC797AE5DA43F6684567C |
| VDMREDIR.DLL | 21B428AFC567D14CF84C6EC7C787260335DBD16DAF890F5884DE9004E1F20C1D |
| nthook32.dll | 3000FD443D0836992A5A9422BAAA7F2FF6C46468537AF2F8F811B0CDF8059D16 |
| nthook64.dll | 3770E77F455B48B2839752FAF2C80FA265B7389D173FE8BCAAA14D941FCAEE6D |
| nthook32-LICENSE.txt | F968C2D86BA2A2F483E47431C7CD8D78878B239A47B34CBB792AC7FE5FCD04BE |

P1 is local commit de96479a5. S2 remains active: P2 explicit control identity
and multi-instance lifecycle and P3 downstream/nested association are still
pending. P1 alone is not multi-instance acceptance or S2 closure.

## P2 implementation and local delivery

The owner clarified six role-specific paths: backend I/O and NTSRV control in
worker-base, VM frontend I/O/control through executable-owned adapters, and
native frontend I/O/control through NTCON. The architecture authority records
the distinction, including I/O protocol requests/replies remaining on the direct
pipe. NTVDM adapters are still S3 scope.

Frontend instance registration/control now has explicit keys and a separate
common/base_srv/frontend_client.h/.c facade. The process core supplies a borrowed
authenticated connection view; frontend operations do not reconnect, replace
the process binding or set a mutable current-instance selector. The frontend
I/O-release acknowledgement moved from common/rpc/worker_control to
frontend_control, together with its unit assertions. Worker-base source checks
and negative controls reject frontend composition and frontend-client use.

Protocol 66 carries explicit frontend selectors. The ACF allows these controls
to progress while the same host waits in backend command/I/O calls; service
locking and joined rundown remain the concurrency authority. NTCON retains its
key in service/channel adapter contexts and reports frontend faults through the
instance-scoped close operation. No NTVDM frontend adapter or worker-base
frontend dependency has been added.

The added real-service in-process fixture covers two registered instances,
wrong owner/generation, independent request and release state, scoped close,
host rundown and repeated retirement without handle growth. Its admission and
selected-route setup are private test boundaries; it is not evidence of real
RPC peer authentication or full downstream launch admission. Those distinctions
must remain explicit in the final acceptance record.

Initial protocol generation required updating interface symbols and async ACF
entries. Direct-RPC test callers also needed new arguments. After frontend
facade extraction, the complete product build passed in
p2-verification-rpc-runner-review.stdout.log. Its test driver then stopped at a
PowerShell Select-String argument-order error after the worker-control unit
itself passed. The corrected final driver rebuilt the changed tests and ran
all acceptance below; product source remained unchanged.

### P2 acceptance

- Both x86 and x64 complete component builds passed. Application 0.0.477 is
  unchanged; protocol 66 is selected consistently by IDL, ACF and interface
  symbols. No compatibility wire fallback was retained.
- Common RPC role units passed: worker control 126 checks, frontend control
  90 checks, both zero failures and zero handle delta. The moved frontend
  acknowledgement remains covered by its new owner.
- frontend-base channel/service units passed on x86 and x64.
- All 41 real-service cases passed on each width, including the new two-instance
  case and 128 repeated retirements per process without handle growth.
- All six existing frontend integration fixtures passed again.
- The added actual RPC fixture passed against the real NTSRV. The broker creates
  the test carrier from its normal worker reservation; the carrier registers
  two endpoints, rejects duplicate/stale identity, closes/unregisters the first,
  keeps the sibling and backend controls live, and completes a real direct
  target with result 37. Existing injected final-I/O error 29 is intentional
  and consumed exactly once. The fixture carrier exits 0. Evidence:
  p2-verification/instance-rpc-frontend-instances.txt and console witness.
  Only this build-owned fixture package substitutes its carrier; release and
  installed images remain the actual ten product components.
- verify-frontend-link-ownership.ps1 passed including new role negative controls.
  NTCON's x64 link map resolves OpenNtBaseClientFrontendRequest and
  OpenNtBaseClientRegisterFrontendRoot from
  base-rpc-client-core:frontend_rpc_client.obj. Worker-base has no frontend-client
  or frontend-base include/call edge.
- Full verification passed all 16 selected gates in 388643 ms.
  Exact timings and package identity are in p2-verification/full/timings.json;
  final driver exited 0. The established DOS4GW exclusion and WOW startup-only
  acceptance boundaries remain unchanged.

Publication driver exited 0. The ten product images and hook license match at
both worktree assets/release and O:/winnt/system32. No installed project process
was running. Guest/configuration files were not copied. p2-publication.json and
its exact-file backups retain verification and rollback evidence. Governance
and diff checks complete the local commit; no push is authorized.

| Image | P2 published SHA-256 |
| --- | --- |
| run16.exe | 2F8C0DD98E84AB4778B1292996754C876A4A00683A3DF69DE612D4F7DFF049D0 |
| ntsrv.exe | BE6A7D740AD9D9A86CC7EBA51EE07F64E4906BBA554B80DECB213955372ACD0F |
| ntcon.exe | C173B331B02975DCFDBC9FD3EC76599EEB3B8B29EEF87A209B57D8D3C1D15DFD |
| ntvdm.exe | 362F3337E603649B60878CEBD31E27D7D3283AA9A5CC008EA145A28B1D271F4E |
| ntvwm.exe | A5802FB0D8E1784DB80F672B05EAC3894D34D5A9F7721247818466E0C5EDD144 |
| ntmon.exe | 362601E358208E84F20EE7811716C1DC9B96E3DF8573AC6E7E046BEE1516FDF9 |
| WOW32.DLL | 549E84C3AD65DBA07028D51F6F0CCE6E25B09AE45E6C7AC271E86CE6FEAD3DB4 |
| VDMREDIR.DLL | ABE1FA4BD16F66CF4C14841EDC2787AD0636A7C2299489C3E5E40E9379405DFC |
| nthook32.dll | C89501FE73C99DD1D87618330BF8EAC54527988352AD212F2EF12E2BB06B73CB |
| nthook64.dll | 67BECCA4A878E5EBF40BFACC6008BF1935C2B60BD74C22068DE34F32E27E33DA |
| nthook32-LICENSE.txt | F968C2D86BA2A2F483E47431C7CD8D78878B239A47B34CBB792AC7FE5FCD04BE |

### Remaining S2 boundary

P2 is committed with this record. Native NTCON uses the explicit-instance path;
multiple embedded endpoints can register and use their isolated control state.
This does not yet claim that existing launch/capability selectors can admit
independent downstream task chains for them: P3 must replace the remaining
process-root association assumptions and prove nested cleanup/upstream isolation.
Actual NTVDM VM adapters remain S3. S2 is active, not closed.
