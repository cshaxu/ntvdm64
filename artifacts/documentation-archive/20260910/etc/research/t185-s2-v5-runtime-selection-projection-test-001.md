# T185 S2 v5 runtime selection projection test 001

The complete repair is one existing-condition extension: v5 now enters the same
command-placement and boot-file projection branch as v2/v3/v4. The v5-specific
branch still owns declared slots, terminal metadata and slot-zero target projection.

The focused MinGW closure rebuilt and passed:

- `byob-profile-test.exe`, including v5 assertions for target, COMMAND,
  CONFIG.SYS and AUTOEXEC.BAT selection fields;
- `bx-ntvdm-adapter-runtime-test.exe --t181-v5-install`, exercising profile
  validation and v5 adapter installation.

No Bochs source, adapter provider, guest process or runtime observation was
involved. T185 S3 may recompose the changed profile object into the existing
T182 derivative only.
