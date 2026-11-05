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

P1 is committed with this record. S2 remains active: P2 explicit control identity
and multi-instance lifecycle and P3 downstream/nested association are still
pending. P1 alone is not multi-instance acceptance or S2 closure.
