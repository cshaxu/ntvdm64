# T484 S3 P6: native observation probe production

Eleven old DOS probe producers are replaced by named, opt-in native NASM
targets in tools/build/fixtures/guest-probes.cmake, selected through
tools/build-fixtures.cmd. The37 binary outputs retain their original
filenames within separate profile directories. Four video variants remain
distinct. G7.COM's former byte array is now a small authored assembly source.
No original guest binary or source was rebuilt or modified.

PID15056 actually ran all old producer variants and the new native group:
all37 COM images are byte-identical, including the G7 byte array. No selected
ASM source includes another file. Stable source/tool dependencies are native
Ninja edges. A subsequent build preserved every output hash and mtime.
The sole executable consumer, verify-s44-vdd-shadow.ps1, now calls the CMD
target and reads its output; runtime assertions are unchanged. Its syntax is
checked. The VDD runtime scenario is not rerun or claimed passed here: binary
identity is used under the owner's existing no-repeat-test authorization.
Historical per-invocation JSON sidecars have no executable consumers; the
canonical producer uses persistent target outputs, with identity recorded
by the invoking verification run rather than duplicate production manifests.

Build-FrontendTerminalLibrary.ps1 is also retired: its required frontend
screen binding source no longer exists, there are no executable callers,
and the current product builds the manifest-bound NXVM closure. The pinned
archive, license and historical import record are not deleted or reimported.

Cached full host build PID22660 passed; all11 raw inputs and22 destination
hashes equal the current published manifest. No product image changed or
required republication. Product/mirror source changes are zero.
[Comparison receipts](../../../artifacts/build-migration/t484/s3-guest-probes/result.json)
record all37 hashes; adjacent receipts record build and final checks.
The containing commit delivers this batch. S3 remains active; remaining
test producers, S4 publication and S5 clean acceptance are not complete.
