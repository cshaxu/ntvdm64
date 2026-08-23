# M0 T245 S16 — canonical COMMAND bootstrap root

## Cause and correction

S15 established that a relative CLI `--dos-root build\output\dos` was copied
unchanged into the startup root descriptor. The imported original
`cmdconf.c → SHELL= → sysconf.asm` chain therefore wrote the relative token
`BUILD\OUTPUT\DOS\COMMAND.COM` into guest `commnd`, and original
`INT 21h/AX=3D00h` failed before EXEC.

`ntdos64_native_cli.c` now follows the existing absolute-target admission
pattern for both accepted DOS and WOW16 roots. It uses `GetFullPathNameW`
before bundle validation, environment publication and the fixed-width startup
root descriptor. This is CLI argument canonicalization only: it introduces
no DOS drive alias, virtual volume, guest-memory write, BOP provider change or
Bochs change.

## Verification

Formal MSVC x64 `/MT` Ninja graph `build/M0-T245-S16/formal-r1` fully linked
`ntdos64-native.exe`.

One Direct source-built native run used the same **relative** root arguments
and S14 BOP/bootstrap observation. The copied guest bytes now begin:

```text
4f3a5c5245504f532e484f4242595c4e54444f5336345c4255494c445c...
O:\REPOS.HOBBY\NTDOS64\BUILD\OUTPUT\DOS\COMMAND.COM
```

The BOP sequence increases from 28 to 29 entries and, immediately after
accepted `5E`, reaches and resumes original `54:04` before subsequent guest
work. This directly proves that the former `AX=3D00h` relative-command open
failure is gone. The run still later reaches the existing typed `50:3D`
terminal; no command execution or parent-return claim is made.

## Next boundary

The next investigation starts from the newly reached COMMAND `54:04` and the
following original guest file path. Its existing mirror is a separate owner
route; S16 neither changes it nor treats its later terminal as a bootstrap
path defect.
