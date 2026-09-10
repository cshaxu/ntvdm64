# M0 T245 S18 — native DEM open input/result observation

## Source boundary

Original `demfile.c:demOpen` accepts the guest full pathname at `DS:SI`, open
mode at `BL`, and writes its source-owned success/failure result as the typed
CPU delta. S18 adds only a default-off bx-vdm observation: after accepted
real-mode `50:12`, it copies `MAX_PATH+1` input bytes through existing checked
RAM and copies the already produced generic outcome.

## Verification

Formal MSVC x64 `/MT` graph `build/M0-T245-S18/formal-r1` links the native
target and `t245-s18-dem-open-observation-fixture.exe`. The focused fixture
passes selector/mode/path/outcome and negative-selector cases.

One Direct source-built native run reports:

```text
dem-open observed=1 read-failed=0 ds=00a7 esi=000003d2 mode=0
physical=00000e42 disposition=1 gpr-mask=0000002f
flags-mask=00000001 flags=00000000
path=C:\Users\neko\AppData\Local\Temp\SCS8545.TMP
```

The cleared typed carry result proves this `demOpen` succeeds. The opened
file is the original COMMAND temporary configuration file, not `COMMAND.COM`.

## Conclusion

S18 removes DEM `50:12` from the current failure hypothesis. The bootstrap
has advanced through temporary CONFIG consumption; the next correct work is a
fresh, bounded post-S16 guest interrupt/error classification, not a demOpen
or current-directory rewrite.
