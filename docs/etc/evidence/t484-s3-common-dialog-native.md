# T484 S3 P98 common dialog producer and recorder review

B0163 retains seven typed native-call boundaries and the independent native
Find/Replace reference. Replace per-run source rewriting/compiler commands
with the shared native source projection and two CMD-selected fixtures.
The projection preserves the entire current implementation macro and its
seven instantiations; the final normalized body comparison passes.
No product, mirror or original guest change.

Retrospective review also covers already-migrated B0162: the existing
integration measurement calls this recorder test, whose nesting, overflow,
read-only and invalid-state/identity contracts remain independently useful.
Remove its unused CompilerDriver parameter; do not count it completed twice.

Runtime PID23376 exited0: seven leaves/42 boundary scenarios, two real native
Find/Replace modes with hooks and cleanup, and recorder controls all pass.
Recorder includes nine invalid states, three invalid identities and85/85
handles. These isolated tests do not prove guest dialog acceptance.

Final PID28336 exited0: full cached host build, four unchanged native
image/projection identities, x64 rejection2,11 raw host hashes and22
published-copy hashes. Product bytes unchanged; existing publication verified.

[Receipts](../../../artifacts/build-migration/t484/s3-common-dialog-native/result.json)
retain actual results; raw logs remain build/t484-s3-common-dialog-native.
Inventory195 complete/160 remaining of355. S3 and retrospective review
remain open; S4/S5 have not completed.
