# T221 S3 P34: Virtual mutation foundation

The selector-blind Virtual mutation backend now owns private directory create,
file delete and empty-directory removal. It accepts only the private store,
drive and DOS-relative name; it never accepts a host namespace or handle.

The x64 `/MT` `hostIo=false` probe passed in `build/M0-T221-S3/108`, covering
create, empty-directory detection, nonempty refusal, file tombstone and final
directory removal. Rename is deliberately not yet bound: directory relocation
must be completed as one shared transaction before any namespace BOP adopts
this backend.
