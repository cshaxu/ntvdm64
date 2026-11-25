# T484 S4 P46 Matrix test preparation

The two matrix callers delegate test file staging to CMD. The existing
guest INI editor strips the copied driver's entries before Win31 Setup
finalization. The shared launcher selector handles the resulting profiles.
The two scheduling callers stage one native batch witness from checked-in
test data. The Win95 test uses its existing PIF utility. No product source
or original guest image is rebuilt for these tests.

The fixture build keys the native CMake cache by the canonical installation
path because the generated guests contain an absolute path. The earlier
parallel matrix captured a Win31 DOS prompt controller whose embedded
paths all pointed to Win95. Two packaged fullscreen controllers also
contained the misplaced Win95 path. The eight-image cache check exercised
two install paths, including a path with spaces, verified embedded paths
and preserved hashes on no-op and cross-path builds. It rejected a
relative path. See build/t484-s4-profile-cache.

The isolated Win31 matrix ran to completion with all 14 cases passing and
normal Windows exit. Coverage includes two Standard startup controls,
six interactive mode/DOSONLY profiles, four fullscreen keyboard cases
and two enhanced DOS window pairs. Both pairs reported VM IDs 2 and 3
and no active mouse callback in either fresh VM. The preparation test
also passed wrong-owner rejection, exact root and PATCH staging,
the DOSONLY roundtrip and byte-exact configuration restoration.
See build/t484-s4-win31-setup-matrix/final and
build/t484-s4-scheduling-matrices/win31-final-evidence.json.

Win95's four normal startup configurations passed with the established
return-code-1 acceptance boundary. A separate same-path comparison
confirmed eight identical files in each of the four configurations.
This comparison exercised the launcher with a non-launching shim.
See build/t484-s4-launch-matrices/win95-retry and the
build/t484-s4-scheduling-matrices/launcher-equivalence.

The final cached build completed after a host restart. It matched all
11 raw host hashes and all 22 file hashes in the two existing publication
locations. No changed host image required replacement. The source check
also matched the earlier tested script hashes and found no changes to
the original guest source or the winnt.zip asset.

The first DOS scheduling combination timed out with both old and new
preparation. The two controllers had the same SHA-256. They recorded
guest heartbeat progress but no SCHED.RUN. The host restart interrupted
the next baseline combination. Only the three unfinished comparisons
were resumed, after the owned test directory had been cleaned.

In the normal post-restart logon environment, the driver-enabled DOS
reentry case passed with both preparation implementations. The same
Win16 controller bytes recorded one Run16 child, advancing heartbeat
during the child and continuing progress afterward. Both used the known
Win95 return-code-1 acceptance boundary. The earlier migrated timeout
did not recur. The driver-disabled native command blocked guest progress
and returned the same host-version receipt with both preparations.

The migrated driver-enabled native command case also passed with the
same host VERSION receipt and nonblocking guest progress. The old and
new preparations had byte-identical SYSTEM.INI, CONFIG.NT, COMMAND.COM,
both PIFs and SCHED.COM. Both original AUTOEXEC.NT files matched, as did
both instrumented candidates. The shared SCHED.CMD deliberately differs
because it derives paths from its location and publishes the completion
receipt atomically. See current-retest, baseline/post-restart and the
individual comparison records under build/t484-s4-scheduling-matrices.

The driver-disabled DOS reentry timeout remains a reported non-pass.
Migration preserves that optional test and keeps interactive editor
acceptance and unexplained runtime boundaries visible. The independent
offline Win95 path restore wrappers are documented tools outside the
required build, staging and publication graph. S6 still owns clean
interpreter-free acceptance for the final consolidated graph.
