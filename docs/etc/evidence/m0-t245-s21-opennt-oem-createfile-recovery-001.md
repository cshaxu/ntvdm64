# M0 T245 S21 — OpenNT OEM CreateFile recovery

## Source decision

S20 showed that permanent `COMMAND.COM` reaches imported `demfile.c:demOpen`
but returns `ERROR_PATH_NOT_FOUND`. The prior modern shim independently used
OEM-to-ANSI conversion and `CreateFileA`, while the directly relevant OpenNT
owner is `base/mvdm/oemuni/file.c:CreateFileOem`.

S21 recovers that source structure in `demfile_shim.c`:

1. OEM input is converted to Unicode;
2. the Unicode path is given to `CreateFileW`;
3. a `GENERIC_READ` open first includes `FILE_WRITE_ATTRIBUTES`;
4. that special read request retries without the extra right on failure.

The only named divergence is replacing NT4-private
`NtCurrentTeb()->StaticUnicodeString` / `RtlOemStringToUnicodeString` storage
with call-local public `MultiByteToWideChar(CP_OEMCP)` storage. The imported
`demfile.c` algorithm, error path and handle publication remain unchanged.

## Verification

Fresh formal Ninja `build/M0-T245-S21/formal-r1` passes both:

```text
T230 S3 direct OpenNT demfile/demlabel import: seven file services ... verified
T245 S21 OpenNT-shaped OEM CreateFileW conversion/retry seam verified
```

The one bounded native run still returns `AX=3`, but S21's default-off
host-call record supplies the decisive difference:

```text
guest source path: O:\REPOS.HOBBY\NTDOS64\BUILD\OUTPUT\DOS\COMMAND.COM
CreateFileW path:  O:\REPOS.HOB\NTDOS64\BUILD\OUTPUT\DOS\COMMAND.COM
first-error=3 retry-error=3 access=GENERIC_READ share=7 creation=OPEN_EXISTING
```

Public `GetShortPathNameW` returns the unchanged long source root on this
volume, so no usable 8.3 host alias exists for the CLI to publish directly.

## Conclusion

The OEM CreateFile compatibility seam is locally source-shaped and closed;
it was not the residual runtime defect. The guest DOS pathname canonicalizer
truncates the dot-containing host component before the host service sees it.
The next owner is CLI/startup admission: it must supply the original `SHELL=`
consumer a real, session-scoped 8.3-representable host alias, or reject an
unrepresentable root explicitly. DEM, BOP routing and Bochs must not repair or
guess that path.
