# T479 S2 shared-library import audit

Owner admits the import, then authorizes making imported tests generic so both
projects can use the same tests instead of maintaining parallel variants.
Local admission baseline is c43cec416. Read-only upstream is
O:/repos.hobby/win16style, clean commit
`d27956b4e5e62d6a81edbd511f2ba4bbd5290026`.
No sibling write or runtime dependency is part of this delivery.

## Source intersection and feasibility

| Component | Identical source files | Changed | Upstream additions | Decision |
| --- | ---: | ---: | ---: | --- |
| handle | 2 | 0 | 0 | Keep byte-identical provider; import shared tests. |
| process-image | 2 | 0 | 4 | Import complete source, including handle validation and bounded remote-image helpers. Existing machine API unchanged. |
| process-launch | 6 | 0 | 0 | Keep byte-identical provider; consolidate tests. |
| process-hook | 20 | 6 | 6 | Import fixes and additive batch/propagation/unwind APIs unchanged; retain NTSRV/product policy in existing consumers. |

Counts include retained licenses and MinHook inputs. Source-tree replacement
is +342/-3 lines, net +339 before removal of downstream compatibility glue;
30 files already match, six change and ten are added. No source-only local
file in these four components needs preservation. The apparent upstream file
component is an empty untracked directory: local file identity/native-path
providers remain required and are not part of the intersection. Other local
components and upstream-only styling/global-management components are excluded.

The helper now rejects odd-sized packets. Our native_hook_packet has a
compile-time 64-byte size check, followed by WCHAR path lengths; current
installer packets are even-sized. Staging also compares the requested DLL
name on repeated installation, fixing false idempotence for equal payloads.
Startup hooks now support MSVC __assume directly, so the downstream forced
include compatibility header and its build references should be removed.
The public late-patch capacity moves from .c to .h without changing its value.

New propagation is NOT a replacement for NTHook's product CreateProcess
classification/Run16/NTSRV routing. Importing an API does not require making
it the product policy owner. Its bounded failure wait and batch retry API
must not introduce new lifecycle waits into existing product callers.

## Test audit and authorized consolidation

Upstream has 14 files across the four test components, while the union also
contains 17 local-only files. These counts are file inventory, not test counts.
Three concrete obstacles prevent blind directory replacement:

1. process-launch/process.c includes run16s-exe/process_lifecycle.h and tests
   executable-private lifecycle policy, rather than the shared provider.
   Replace that test with a generic process.h contract test; do not import
   run16s policy or build a fake compatibility header.
2. Portable CMake and the GCC helper assume tests/ rather than test/ and the
   documented build path is outside this project's stable Ninja cache root.
   Resolve tests from their own file location; keep compiler/build selection
   in the owning project's runner. Generic sources must work under either root.
3. Local machine/permissions/cross-width and late-guard-race coverage is not
   wholly present upstream. Merge effective assertions into the shared suite
   and retain useful generic fixtures, rather than deleting coverage to obtain
   directory equality. Remove superseded duplicate tests and obsolete shims.

Source files remain exact upstream bytes. Test generalizations are explicitly
authorized downstream changes intended for reverse import; do not claim both
repositories already match until the sibling adopts them. Exact source hashes
and the final shared test inventory must be retained with delivery evidence.

## Verification and handoff

Audit all actual callers/build registrations, compile shared tests on supported
x86/x64 compilers, exercise added failure/unwind/batch cases and existing
cross-width fixtures. Then cached product build, required regression and
coherent ten-image publication precede S2 closure. Source audit alone does not
prove binary compatibility. No guest or original mirror changes are required.

This side stage is S2/shared-library-import. Under the owner's mixed-task
numbering rule, planned S2/guest-bridge remains a distinct future packet;
S3--S5 guest obligations are neither renumbered nor dropped.
