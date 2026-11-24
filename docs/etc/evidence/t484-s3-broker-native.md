# T484 S3 P139 native original broker fixtures

Reconcile B0215 and B0217. Keep the existing original-classifier and
original-server-lifecycle C fixtures and their formal worker library owners.
Expose both through `tools/build-fixtures.cmd x86 broker-original-fixtures`.
Remove the228-line duplicate JavaScript builders, frozen T412 graph paths,
manual compiler/link commands and stale source/archive-spelling assertions.
No original guest or product source is changed. Historical source hashes and
map reports remain evidence, not current build prerequisites.

The classifier runner already stages suffix inputs and uses repository media.
It passes DOS/WOW formats, self absolute/relative PE, DLL rejection, suffix
policy and missing-file errors. The lifecycle runner now supplies read-only
media copies from the repository in its own build directory, checks hashes,
uses a30-second owned process and records its identity/exit. It does not run
these guest binaries. Original path conversion and command copying remain
under test without depending on the user's installed MEM.EXE.

Two stale fixture assumptions were exposed and corrected:
- PID9640: GET request fixture retained a104-byte prefix /232-byte total.
  Current common/base_srv/base_command.c has a112-byte get_prefix including
  common_dos_delivery. Update both fixed buffers; retain the one-byte-short,
  unchanged-output, all-truncations and generation-negative assertions.
- PID12544: O:/winnt/MEM.EXE is absent. Do not modify the user's installation.
  Resolve the runner-owned short directory and use it in launch/deep-copy/WOW
  assertions. PID22148 passes the full fixture after this correction.

The final run uses the checked-in CMD target and both checked-in runners,
then cached full product build and11 raw/22 published-copy identity checks.
This is native fixture acceptance, not real guest or Full acceptance.
S3/S4/S5 remain open. B0218 and later tools are separate remaining work.

Commands after building the CMD target:

```powershell
test/adapter-basesrv/verify-original-classifier.ps1 -Fixture build/.ninja/msvc-x86-release/worker/original-classifier-test.exe -BuildRoot build/broker-classifier
test/adapter-basesrv/verify-original-lifecycle.ps1 -Fixture build/.ninja/msvc-x86-release/worker/original-server-lifecycle-test.exe -BuildRoot build/broker-lifecycle
```

Raw evidence: build/t484-s3-broker-native.
[Receipt](../../../artifacts/build-migration/t484/s3-broker-native/result.json).
