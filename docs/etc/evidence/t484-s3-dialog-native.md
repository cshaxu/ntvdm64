# T484 S3 P166 DC and dialog native chains

The two retained integration runners already call Invoke-Win16ProbeBuild,
which delegates to build-fixtures.cmd x86 win16-probes. The CMake source list
owns DCPROBE, DLGPROBE and CDLPROBE assembly; it preserves the two common-dialog
rounds. No dynamic compiler remains in these callers. They cover distinct
production contracts and are retained, not duplicated or mechanically rewritten.

Actual normal-user-environment run: supervisor26544, child6716,
build/t484-s3-dialog-native. Reused the existing native logon environment reader;
no environment values recorded. The immutable guest package uses current 11
published host images. DC report verifies bitmask63 with no errors. Ordinary
dialog report ends at phase7 with errors0, initialized6/terminated6, modal2,
failed1, killed1, leaving1 and expected focus/default/cancel/edit paths. Common
dialog report ends errors0, created3/closed2/hooks3/leaving1, successful destroy,
IsWindow0. Both launcher and guest retire normally. The test asserts all
intermediate records, not just these final counters.

The same run passed cached full and verified11 raw build input hashes and22
published hashes unchanged. No production or original guest source/binary
changed and no new publication. This is focused runtime acceptance of existing
CMD migrations, not a new Full regression. Evidence includes result.json,
full-build.log, guest reports, observer records and exact inputs. The package
copy was removed after termination; scoped short stages were cleaned by the
existing helper. closure-result.json records the documentation gate.
