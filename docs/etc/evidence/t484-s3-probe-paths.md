# T484 S3 P173 native Win31 witness path binding

B0071 wrote three128-byte path fields into copies of independently authored
W31CHECK.EXE/W31DOS.COM using PowerShell byte generation. Keep cached template
assembly and parameterize only test copies. The new win31-probe-paths native
fixture binds boot, ready and DOS-result paths, using the same fixed marker
names and exact field widths. It requires one occurrence of each marker,
complete field bounds, ASCII paths shorter than128 bytes, and validates both
images before writing either. A16MiB input ceiling bounds diagnostic allocation;
these authored DOS/NE witnesses are far smaller. No original guest is modified.

Invoke-Win31ProbeBuild retains its original template/external-controller choice
and builds the binder through tools/build-fixtures.cmd. The matrix calls that
binder instead of maintaining a second byte-patching implementation. Test-only
path arguments are still provided by the runtime case. This is not a general
binary patcher, new product component, or a new guest execution mechanism.

Checked-in verification: test/build/verify-win31-probe-paths.ps1, supplied the
native binder, two real assembled templates and a fresh build-owned output root.
It verifies every non-field byte is unchanged, exact strings/zero padding and
unchanged total sizes. Missing marker, duplicate marker, truncated field and
long path all reject without changing either image. It uses authored copies,
not original media. No new Win31/Win95 boot pass is claimed.

Background PID30876, build/t484-s3-probe-paths/run.ps1, passes these checks and
cached full build in78.1 seconds. All11 raw production images and22 published
hashes remain unchanged, so no redundant publication. Governance results are
closure-result.json beside the run. Main and pending disposition records stay
consistent: B0071 remains open for complete runtime/package preparation;
311/355 disposed,44 remain. Original guest and mirror diff are unchanged.

A read-only scan of the remaining S3 entries also identifies direct compiler
recipes still in native-modal-control, native-vt-fidelity, ntvwm-carrier-game
and wow-dll-entry-boolean, plus dynamic serial-test helper compilation. Treat
these as concrete next producer work; text hits alone are not completion or
proof that other indirect dependencies have gone away.
