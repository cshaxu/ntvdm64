# M0 T245 S17 — native COMMAND current-directory observation

## Boundary

OpenNT `cmdmisc.c:cmdGetCurrentDir` receives `DS:SI` from guest code and
writes its source-owned current-directory response there.  S17 adds a
default-off bx-vdm post-dispatch observation only: accepted real-mode
`C4 C4 54 04` causes one checked, fixed 64-byte copy at `DS:SI`.  It does not
alter the imported handler, guest RAM, CPU result, file system or Bochs.

## Verification

The formal MSVC x64 `/MT` graph `build/M0-T245-S17/formal-r1` links both the
native target and `t245-s17-command-current-dir-observation-fixture.exe`.
The fixture passed; it checks selector, mode, successful post-dispatch copy,
address translation and a non-`54:04` negative case.

One Direct source-built native run reports:

```text
command-current-dir observed=1 read-failed=0 ds=00a7 esi=000013f3
physical=00001e63
bytes=4f3a5c7265706f732e686f6262795c6e74646f73363400...
O:\repos.hobby\ntdos64
```

This is the actual host process current directory, exactly the Direct-host
value returned through the existing OpenNT-shaped provider.  The sequence
then reaches and resumes `50:12 demOpen`, before later retaining `50:3D`.

## Conclusion

`54:04` is no longer an unobserved suspect and requires no repair.  The next
owner is source-mirrored DEM `50:12 demOpen`: it must be observed at its
actual guest pathname/return boundary before altering current-directory or
file-provider semantics.
