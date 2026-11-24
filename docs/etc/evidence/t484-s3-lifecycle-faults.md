# T484 S3 P128 initialization and creation failure producers

The formal native-slot group still calls InitRollback and CreationUnwind.
They cover distinct current failure contracts and are retained, not retired
merely because normal creation passes. Their CMD/native targets reuse actual
queue/lifecycle/creation compiler profiles and existing injection translation
units. Separate archives replace exactly those owners; no duplicate-provider
link override, production hook or new fault algorithm is introduced.

InitRollback proves two allocation failures release their names while retaining
the initiating error, and three failed idle-hook installations leave no live
hooks. CreationUnwind proves native destruction followed by an exception
releases its borrowed scope, restores state and permits retry. Normal creation
retains its unchanged provider. Runtime assertions are unchanged.

PID22436 passes both contracts, cached full host build and11 raw/22 published
image identity checks. Publication is verified unchanged, not repeated.
Seven unreachable old producer branches (35 net lines) are removed after
argument validation confirms these flags cannot select remaining old modes.
The final runner repeats both after cleanup; its recorded result, not the
earlier candidate, controls delivery. Existing compiler/import warnings remain.

[Receipt](../../../artifacts/build-migration/t484/s3-lifecycle-faults/result.json)
records actual process results. Raw evidence: build/t484-s3-lifecycle-faults.
No production/mirror/original guest changes. B0195 remains partial:28 normal
native entries, one consolidated alias, dialog-lifecycle and seven other public
fault switches still require disposition. The two migrated switches do not
increment the original355-file completion count (225 complete/130 open).
S3 and final interpreter-free clean/publication acceptance remain open.
