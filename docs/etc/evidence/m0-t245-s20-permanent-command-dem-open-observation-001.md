# M0 T245 S20 — permanent-command DEM-open observation

## Question

Does the permanent source-built `COMMAND.COM` open reach the imported OpenNT
`demfile.c:demOpen` provider after the earlier temporary CONFIG open, and what
does its source-shaped result say?

## Boundary

S18's default-off observation retained only its first accepted `50:12`, which
is the temporary SCS configuration input. S20 keeps the same bounded
adapter-only record but stores the latest accepted `50:12` plus a count. It
does not alter guest RAM, DEM dispatch, the provider outcome, Bochs, or the
machine ABI.

## Verification

The fresh formal Ninja `build/M0-T245-S20/formal-r3` focused fixture passes.
It proves two accepted opens retain the second pathname/count and that a
non-`50:12` selector remains unobserved. One bounded Direct source-built
native run from formal-r2 reports:

```text
dem-open observed=1 count=4 ... mode=0 ... gpr-mask=00000001
ax=0003 bp=0000 flags-mask=00000001 flags=00000001
path=O:\REPOS.HOBBY\NTDOS64\BUILD\OUTPUT\DOS\COMMAND.COM
```

`file.asm:OpenCall` invokes `SVC_DEMOPEN`, and original `demfile.c:demOpen`
uses `demClientError` on the failure path. Thus `AX=3` is the source-shaped
`ERROR_PATH_NOT_FOUND`, not an EXEC, PSP, or cmdExec32 result. A public
read/write `CreateFileA` check of that exact host pathname and sharing request
succeeds outside the guest process.

## Conclusion

The permanent command does reach the imported DEM provider; the failure is
inside its current OEM host-file compatibility seam. It is not a missing file,
a missing BOP, or Bochs behavior. The next source-first owner is OpenNT
`base/mvdm/oemuni/file.c:CreateFileOem`: its original OEM-to-Unicode and
`CreateFileW` structure must replace the present simplified
OEM-to-ANSI/`CreateFileA` shim through the smallest modern compatibility
helpers.
