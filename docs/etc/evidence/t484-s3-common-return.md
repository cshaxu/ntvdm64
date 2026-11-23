# T484 S3 P79 common-return diagnostic producer

B0107 now uses native source projections and the shared two-object diagnostic
link instead of interpreted source rewriting, Ninja command extraction and
CL/link script generation. Original destroy-return and thunk-admission
witnesses remain; the current thunk signature uses opaque public typedefs,
so the old LPFNW32/PVDMFRAME text matcher was obsolete.

Only the thunk diagnostic needs original VDMFRAME/file-write layouts. Its
copy includes precomp.h with original_wcall16's real internal profile; the
lifecycle copy retains its own modern profile. No production source, wrapper
header body or guest is changed. The diagnostic remains nonpublishable.

Initial trials are failures, not passes: adding precomp to both copies first
duplicated wowkrn/wkfileio declarations already included by precomp. Removing
those extra includes compiled but exposed13 missing modern wrapper exports:
original macros must not enter the lifecycle implementation. Restricting the
original profile/header to the thunk copy restores the correct two owners.
The final runner PID9660 builds/links both copies; its
[diagnostic result](../../../artifacts/build-migration/t484/s3-common-return/diagnostic-result.json)
retains the nonpublishable declaration. Raw attempts live in
build/t484-s3-common-return.

Final verification compares each generated body to the prior witness recipe,
with only the documented current signature and header/profile adjustments;
checks229 ordered objects with exactly2 substitutions, cached hash/mtime,
x64 rejection, callback-frame behavior and unchanged production/publication
hashes. Fresh guest common-dialog failure observation is not claimed. This
migration restores a diagnostic producer, not a product behavior repair.

Cached full build PID13368 and final verification PID31600 exit0. Generated
bodies match the documented witness projection;229 ordered objects retain
exactly2 substitutions, cache is stable and callback-frame checks pass.
All11 raw host inputs and22 published hashes remain unchanged; both published
locations are verified without copying diagnostic images. B0107 is complete.
Fixed355 inventory:155 complete,200 remaining (52 pending items including
historical validation/staging,148 awaiting reconciliation).
