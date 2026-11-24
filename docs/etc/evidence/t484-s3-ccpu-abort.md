# T484 S3 P97 actual CCPU abort producer

B0161 retains actual CCPU simulation escape/reentry, private TLS ownership,
selector-error reuse and control restoration across three threads. This is
distinct from the original callback-frame and diagnostic-provider fixtures.
Remove obsolete build.ninja parsing, temporary CL/link recipes, historical
ReferenceRevision source substitution and its two test include overrides.

The existing native worker graph supplies the current descriptor-domain
fixture closure. Its CCPU product object and existing callback lease object
are reused. Private c_xtrn/ntthread inspection remains in two test TUs.
Original duplicate-symbol disposition is retained; no force-unresolved or
new provider aliases. The map confirms c_cpu_simulate from c_main.obj,
ccpu386SimulatePtr from the TLS fixture and call_cpu_function from the
interface fixture. Production and original guest files are unchanged.

Runtime PID21256 exited0. Three CCPU_ABORT_OK markers confirm escape, nesting,
continue and normal FE return; interface markers confirm selector errors13/11
and normal reuse; control markers confirm current descriptors, modes,
continuation, shared CR3 and caller error preservation. These are isolated
actual CPU tests, not guest/full lifecycle acceptance. Historical negative
revision execution is retired, not claimed as newly tested.

Final PID18212 exited0: cached full host build, stable image hash/mtime,
x64 rejection2,11 raw host hashes and22 published-copy hashes pass.
Product bytes unchanged; existing publication was verified.

[Receipts](../../../artifacts/build-migration/t484/s3-ccpu-abort/result.json)
record current P97 identities. The run initially reused the old diagnostic
directory name; only the current PID results are accepted and copied to
build/t484-s3-ccpu-abort. The earlier diagnostic delivery remains separate.
Inventory:194 complete/161 remaining of355. S3 and retrospective review
remain active; S4/S5 remain pending.
