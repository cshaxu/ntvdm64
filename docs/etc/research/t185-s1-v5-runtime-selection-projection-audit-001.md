# T185 S1 v5 runtime selection projection audit 001

T184's `54:0C` loop exposed a complete, narrow projection omission.

`byob_profile.c` validates v5 command placement, boot files, declared slots and metadata.
Its selection branch copies command placement and boot-file paths only for v2/v3/v4.
The v5 branch copies slot/metadata state and, after T183, slot-zero target placement.

| Selection field | Existing consumer | V5 before T185 |
| --- | --- | --- |
| target placement | runner DOS-engine gate | projected |
| command placement | adapter boot namespace/runtime | omitted |
| CONFIG/AUTOEXEC fields | COMMAND `54:0C/0D` boot-file provider | omitted |
| declared slots and metadata | v5 COMMAND sequence | projected |

No consumer requires a new provider or schema field. S2 must include v5 in the existing
command/boot projection branch and extend the existing v5 profile test to assert all fields.
