# T484 S3 P22: native WOW control producers

The CMD wow-controls group replaces New-T447S3CallBoundaryNinja.ps1 with
explicit native CMake targets: 158 compilation units, 46 links and 35 retained
groups. All 158 objects and 46 linked outputs were built. The five existing
source projections match the former generator after LF normalization only.
Projection inputs trigger reconfiguration; missing boundaries fail configure.

The globals fixture now projects the existing clglobal.c globals before its
registration implementation. Full-file linking pulled unrelated registration
providers into bounded controls tests; function sectioning did not resolve
that dependency. Production still compiles its complete current source.

Two expired fixture assumptions were corrected without changing production:
the keyboard lifecycle fixture no longer duplicates the restored _GetDlgItem,
and maintains its real child link across destruction. The get/set fixture now
binds the current window-access provider, shared with the existing message
fixture. Before correction SetWindowLong reported ERROR_INVALID_STATE and the
test later dereferenced a null edit descriptor. Original assertions remain.

Final entry: test/observation/verify-wow-control-boundaries.ps1, default cohort.
All 38 runtime cases returned zero. Additional research outputs were built,
not claimed as runtime passes. The initial failed cohort is not acceptance.
See the [result](../../../artifacts/build-migration/t484/s3-wow-controls/result.json),
[projection comparison](../../../artifacts/build-migration/t484/s3-wow-controls/projection-comparison.json)
and [inventory](../../../artifacts/build-migration/t484/s3-wow-controls/migration-inventory.json).

Cached full host build PID1072 returned zero. Separate identity checks verified
11 raw inputs and 22 published files unchanged; no product recopy is needed.
No production, mirror or original guest bytes changed. This is targeted
acceptance, not a new Full product regression. The containing commit delivers
P22. S3 still has other producers to reconcile; S4/S5 remain pending.
