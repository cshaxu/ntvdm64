# T484 S3 P144 native cache and boundary audits

## Scope and disposition

B0223 retains the two-pass generated-input stability contract but delegates
configuration/build entirely to tools/build.cmd. It no longer maintains its
own worker configure command or assumes all products share one graph. Each
pass verifies ROM RC/provider DEF and all11 raw images keep both their hashes
and modification timestamps, and checks no work remains in all eight graphs.

B0224 remains a read-only compiler-header audit. Its old WOW object path no
longer existed. Locate the source's compiler object within the actual product
closure, excluding the diagnostic original-frame variant of the same source;
require current VALID dependency records. Shared-header and three forbidden
header controls are retained. It uses the shared native graph reader.

B0225 is retained unchanged as a read-only standalone audit. Its subprocesses
are Ninja tools commands/deps/inputs, not compiler or generator invocations.
It checks A/B compile arguments, current headers and actual prerequisites.
Standalone assertion languages are outside the CMD producer migration.

## Actual verification

Raw evidence: build/t484-s3-native-cache-audits. PID29856 completed both CMD
build passes with no image/input byte or timestamp changes and eight no-op
graphs. That run then failed on the obsolete B0224 object path, not on the
build. PID10148 exposed two source variants before product-closure filtering.
Final repaired boundary run PID26156 exits0: both memory consumers/three
negative controls and all eight library graphs pass. A/B object instances:
worker112, WOW79, frontend34, service28, launcher25, monitor2, native worker33,
hook13; total326 (not unique source files).

The final run also checks11 raw host input hashes and22 published hashes
against the existing release manifest. No changed image requires republication.
No original guest, production source or native build graph was changed.
These are build/static audits, not a new Full runtime pass.

[Receipt](../../../artifacts/build-migration/t484/s3-native-cache-audits/result.json).
S3 remains active; S4 publication and S5 final clean/interpreter-free acceptance
remain outstanding. B0227 is next; B0226 was retired in P137.
