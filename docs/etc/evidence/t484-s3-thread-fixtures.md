# T484 S3 P19: thread producers

The historical heartbeat fixture generator and PowerShell source extractor
are removed. The live thread-boundary runner no longer generates compiler or
linker commands. `tools/build-fixtures.cmd x86|x64 thread-lifecycle` owns three
executable targets and the redirector-entry test DLL, preserving the existing
source/optimization/runtime settings and explicit exported shared binding.
Native build metadata tracks the executable's import library and DLL outputs.

The selected current host_ExitThread body is projected by the native source
selection helper. On both architectures it matches the old unique-function
selector after line-ending/trailing-newline normalization. Existing #error
and #line inside the body remain. No original guest or product source changes.

Final x86 and x64 runs pass thread alert, event-thread alert/join, and all six
session lifecycle marker groups: initialization, rollback, pre-CCPU release,
terminal unbind, terminal intent and shared-owner DLL entry. The CPU release
is an existing test witness, not proof of full CCPU teardown. Final linking
after declaring import-library byproducts changed binary identities; both
architectures were rerun rather than relying on the earlier binaries.

[Results](../../../artifacts/build-migration/t484/s3-thread-fixtures/result.json)
record final identities and marker output. Cached full host build PID10964
passes. All11 raw host images and22 published hashes remain unchanged under
package d8983d2b-3fe1-4d18-8ea9-64613c84346c, so no changed product image
requires republication. This is targeted verification, not a new Full run.
The containing commit delivers P19. Six literal Ninja generators remain;
inline producers, S4 publication and S5 clean acceptance are still pending.
