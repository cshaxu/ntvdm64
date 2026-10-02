# T424 S2 — Native worker identity migration

## Objective, source and scope

Rename the project Win32 text worker to NTW32 and its project-owned native
record to Win32Record without changing execution or genuine Console names.
Inputs: S1 `3c8e6b7d1`, accepted T423 S40 `f64559086`, the
[S1 inventory](m0-t424-s1-name-referent-audit.md), and the admitted
[ordered plan](../operations/t424-worker-frontend-renaming-plan.md).
All disposable artifacts and raw reports use build/M0-T424/S2/r001.

Fourteen worker files moved with Git to src/ntw32-exe; seventeen worker-named
test/script files and the historical backend evidence path moved with their
consumers. Worker-local function/type/guard/diagnostic names, executable
selection, build/object references, scripts, documentation and links follow
NTW32. NTSRV's project-owned OPENNT_BASE_WIN32RECORD, win32records,
pending_win32record, next_win32record and service_*_win32record preserve the
original fields, list order, request, receipt and completion logic.

Original OpenNT Console citations (including windows/core/ntcon/client),
GetConsoleProcessList, GetCurrentConsoleFontEx, Console APIs and unrelated
identifier substrings remain original. No guest, shared library, original
mirror code, ACL, endpoint, command syntax, scheduling or lifecycle changed.
The only changed file under a mirror root is the project mvdm README.
This is a project-name migration, not historical-source recovery or a claim
of newly implemented native capabilities. Frontend remains NTKVM in S2;
the reserved NTCON frontend name belongs to the later S3, not this worker.

## Audit and ABI

Get-ComponentRenameInventory.ps1 scans all tracked text/path names and all
untracked non-ignored text, including the indexed archive; no archive directory
is excluded. Invoke with WorkerName set to the former worker name,
FrontendName=ntkvm, RecordName set to the former record family,
PackageWorkerName=ntw32, PackageRoot=O:/winnt and a fresh OutputRoot below build/.
The literal pre-migration arguments remain sealed in S1 Git/inventory.

The name-final-02 scan covered 11,513 tracked files and the other session's
one current untracked proposal. It found zero old worker-product and zero
project-record occurrences. Its remaining 2,104 raw hits were 575 unrelated
substrings, 832 original Console-source references, 685 current frontend
references and 12 explicitly reserved frontend references. Per-occurrence
paths, lines and context are retained; later evidence/test additions require
a fresh final scan, not reliance on these intermediate counts.

review-name-only.ps1 compares HEAD source with current source, mapping moved
paths and only the approved case-preserving product/record substitutions plus
the application version. It proved exact normalized-text equivalence for 57
source/test/build files before adding the new identity-only test below. Its
semantic-review.json retains every pair; there are no changed original
mirror/shared-library code files. This review does not exempt documentation
from referent review: the S1 archive row was corrected to preserve its original
Console source citation, and reserved frontend planning names were reviewed
separately instead of blindly replacing them.

APP_VERSION advances once to 0.0.424. APP_PROTOCOL_VERSION remains 28;
service.idl remains version(28.0), UUID
7d3e78bd-0472-4736-8797-e64e966d692e. Only an IDL comment changed; copied
layouts and operations are identical. Fresh MIDL outputs were generated,
not copied from an older graph. The real-service identity test below rejects
the previous T's 0.0.423 application and a mismatched application protocol.

## Reproducible build and tests

VS2022 MSVC14.43.34808, SDK10.0.22621.0, Win32/x86 /MT CCPU40.
Generate New-T310OriginalSoftpcNinja.ps1 with Architecture=x86,
BuildRoot=the above run root, NodeExecutable=O:/.nvm/versions/node/v22.22.1/bin/node.exe.
Run its run-ninja-parallel.cmd product-programs and the focused fixture targets.
The primary build completed 602 steps. New-T404S5Wow32ProviderNinja.ps1,
Architecture=x86, BuildRoot=run-root/wow32 and ParentImportLibrary=run-root/ntvdm.lib,
rebuilt WOW32.DLL from 77 bodies/105 carriers. The startup observer was rebuilt
x86 /MT from tests/observation/console_startup_observer.c with user32/dbghelp;
all objects and outputs stay in the run root. Subsequent fixture-only incremental
builds leave the tested production artifacts unchanged.

| Check | Entrypoint and arguments | Result / raw evidence |
| --- | --- | --- |
| Next command ownership | ntw32-next-command-test.exe | Pass ownership, failure disposal and completion. |
| Shared text frame | ntw32-text-frame-test.exe text-frame-01.txt | 133 checks, zero failures, production receiver; not a real channel test. |
| Execution lifecycle | ntw32-execution-lifetime-test.exe execution-01.txt | 586 checks, zero failures, 12 completed/16 cancelled, target survival, zero remaining handles. |
| Console handoff | console-channel-lifetime-test.exe --private-desktop-full channel-01.txt | Pass actual/logical 80x30 to 25/28, repeated/shrink/grow/cells/cursor, invalid request and cleanup. |
| Frontend scope | frontend-scope-lifetime-test.exe | Pass role/join, invalid association, authenticated retirement and cleanup. |
| Client result/failure | frontend-request-client-test.exe | Actual target 37, resume, rejection/version/contradictory/EOF/partial reply and final presentation pass. |
| Bootstrap identity/failure | frontend-bootstrap-test.exe --startup-rejections | Pass version/application 1306, status 5, short 109, unregistered 5023, no local handle leak. |
| Real service mixed identity | worker-identity-version-test.exe run-root/ntsrv.exe | identity-negative-01.txt: previous-T APP identity and wrong APP protocol rejected by real authenticated RPC. |
| Management RPC | monitor-rpc-test.exe --empty | Pass current snapshot, wrong version rejection and absent-worker rejection. |
| Actual modern EDIT return | verify-command-native-edit-return.ps1, Observer=run-root/observer.exe, PackageRoot=Z:/, ReportPath=run-root/edit-return-01.txt | EDIT menu/status, Ctrl+Q, exact native return, DOS MEM and COMMAND completion 1 pass. |
| Product regression | Verify-CommandExitStatus.ps1, Observer=run-root/observer.exe, PackageRoot=Z:/, ProcessPackageRoot=run-root/runtime, LogRoot=run-root, GuestFixturePath=build/M0-T423/S38/candidate/G7.COM, OrdinaryFrontend | t424-s2-console17-summary.json and t424-s2-window17-summary.json: each 17/17; actual text/interaction and exit assertions. Window uses MVDM_OBSERVER_WINDOW_INPUT=1; both use private desktop. |
| Independent sessions | verify-ntw32-management.ps1 with Observer/MonitorRpc from run root, PackageRoot=Z:/, ProcessPackageRoot=run-root/runtime, LogRoot=run-root, LogPrefix=t424-s2-two-sessions, TwoSessions | Selected acknowledged close ends its worker/CMD; independent session echoes ISOLATED-SESSION-OK and returns 23. |
| Root failure | Same management script, LogPrefix=t424-s2-frontend-loss, FrontendLoss | Pass associated Console close and non-success direct completion. |
| WOW retained frontier | observe-wow-frontiers.ps1, Observer=run-root/observer.exe, WindowObserver=build/M0-T423/S9/publication-window-reader.exe, Prefix=t424-s2-wow-frontier, PackageRoot=Z:/, ProcessPackageRoot=run-root/runtime, LogRoot=run-root, PostExitObservationMs=5000 | WINMINE guest main window retained; SOL/WRITE retain original out-of-memory dialogs, not usability passes. |

The new identity-only fixture is tests/component-integration/worker_identity_version_test.c.
Compile with cl /c /MT /W4, /I run-root/obj/basesrv /I src, output object
under the run root; link its object with run-root/obj/run16/stub.obj,
run-root/broker-transport.lib, rpcrt4/kernel32/advapi32 into
run-root/worker-identity-version-test.exe. It refuses a pre-existing broker,
starts/cleans only its own service, and tests current identity acceptance
followed by old application and wrong protocol rejection. Its first compile
used the wrong Disconnect signature and failed; the corrected four-argument
fixture then built and passed. No product code was changed for this test.

## Additional observation and non-passes

verify-continuous-worker-chain.ps1 was additionally run with BuildRoot=run-root,
PackageRoot=Z:/ and LogPrefix=t424-s2-continuous after building frontend-chain-cui
and frontend-chain-input. The real DDWWDDWW chain returned 0 and recorded all
16 ordered ENTER/RETURN events and input/output witnesses under one frontend.
Its eight DOS-stage management samples show the same DOS PID 30256 and native
PID 26800, DOS depths 1/2/3/4 on entry and return, and the same parent task IDs.
These are positive captured reuse observations, not a claim that the old
validator passed: it stops at an obsolete EPOCH requirement. The production
DTO now emits RECORD lines with kind 0/2 instead of its old 1/4 assumption.
The whole script is a non-pass and was not weakened or used as an all-green
gate. The published baseline's same source has that obsolete assertion.
The required product matrices, actual return, isolation and lifecycle gates
above are separate passing tests.

The obsolete nominal-root --execution/public-startup RPC fixtures documented
in S32 were not substituted for actual frontend runtime; they are not new
passing evidence. Unsupported synthetic Window Ctrl+Q and physical desktop/RDP
observations remain unclaimed, as in S40. No full SOL/WRITE acceptance is claimed.

## Publication and disposition

Eight tested x86 files were published together to O:/winnt. The recovery set,
original exact basenames, prepublication and published manifests are under
run-root/published-recovery and *publication-hashes.json. The obsolete worker
EXE was removed only after a recoverable backup and eight-of-eight hash check;
the intermediate frontend is still ntkvm.exe. SYSTEM.INI, NTVDM.REG and guest
media were not overwritten. Z: staging mapping was removed after testing.

| Published file | SHA-256 |
| --- | --- |
| run16.exe | B0B66BC3BA67B65F44436AA53700A276CE4FF6605A32DC5C4D6B2B0A444EBF7A |
| ntsrv.exe | 8FD2C38EDD4758AA12A793D244C2010E59760A9BC524ED07137FEFF595CAA164 |
| ntvdm.exe | 50AC0FF4A2018023D1E777F542806BBC463B2EA31FDB4BF82F93BCB955C0596D |
| ntw32.exe | F59C29ACCE6CF23A7EE5CA59C9F261630D503F27BAAC53421F4057581165A492 |
| ntkvm.exe | 8950683266549DE72451207DD1DCA0C0CFBB426819060BB197CD3A4F11F74783 |
| ntmon.exe | 1DD84B369BE0B632196331F8BCF512CE04C9A603199F3697825F312B57962F90 |
| wow32.dll | BEB688FE832921D788DB297787B2D1062D7FDD095AF1A304824F31DE7F19D0C1 |
| VDMREDIR.dll | 13BC631926C741DC31E1A259A85DA6482BD6B8093DE1716B08D3FF35226C4DC9 |

Other-session native launch-hook/trace proposals and Queue reordering are
preserved and included under the owner's standing documentation approval;
they do not admit or implement those packages. S2 closes only after its final
publication smoke, referent/governance/diff review and committed/pushed P.
T424 remains open and S3 frontend migration is not performed here.

Postpublication verify-command-native-edit-return.ps1 uses PackageRoot=O:/winnt
and ReportPath=run-root/edit-return-published-02.txt; it passes the real EDIT
screen/Ctrl+Q, native echo, DOS MEM and launcher completion assertions. This
existing fixture requires reports below repository build/; an attempted Logs2
report was rejected before execution and is not counted as a runtime test.
The final reviewed scan also covers this evidence and the new identity fixture.
name-final-03 covers 11,513 tracked and three untracked current files: 2,111
raw hits, 576 unrelated substrings, 833 original-source citations, 689 frontend
references and 13 reserved frontend references; zero old worker/record hits.
Final name-only review again passes all 57 existing source/test/build pairs;
the only new code is the separately reviewed identity-test fixture. Governance,
relative-link verification and git diff --check pass. Eight published hashes
were rechecked against the manifest after the postpublication smoke pass.
