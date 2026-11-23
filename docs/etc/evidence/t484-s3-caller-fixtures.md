# T484 S3 P4: caller fixture generators

Six historical PowerShell generators are replaced by five native fixture
groups behind tools/build-fixtures.cmd. T280 and T282 compiled the identical
guest-location fixture and now share one target. T291 task frame, T322 XMS,
T324 redirector contract and T335 callback frame retain their source bodies,
assertions, architecture restrictions and explicit LINK library policy.
The three profiles without general /Gy retain it only on the coordination
projection; the other profiles retain their original /Gy setting.
The redirector x86 group also preserves seven compile-only original units.
No product implementation, mirror source or original guest is changed.

Final run PID8284 passed nine executable instances across x86/x64 and the
seven compile-only units. Cached full host build PID33032 passed. All11 raw
host inputs and22 published hashes match the current manifest, so there are
no changed host images to republish. The native fixture graphs contain no
forbidden interpreter commands. These results apply to the final profiles,
not only the earlier candidate run.

[Receipts](../../../artifacts/build-migration/t484/s3-caller-fixtures/result.json)
record each executable and its hash. The containing commit is this bounded
batch's delivery identity; S3 remains active.

The Python helpers remain pending, not retired: full reference scanning also
found three JavaScript generator users (DEM lifecycle, XMS startup and
redirector workstation query) plus the COMMAND capture CMake entry. This
corrects the earlier six-generator-only scope statement; those four consumers
must migrate before removal. Other S3 producers, S4 publication and S5 clean
acceptance are unfinished.
