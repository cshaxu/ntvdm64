# T95 S6 Intact Native Tuple Manifest 001

## Result

`tools/build/New-T95S6IntactNativeTupleManifest.ps1` generated:

`artifacts/analysis/t95-s6-intact-native-tuple-manifest-001-20260811-001/`

The manifest inventories the complete imported generated-Makefile family:
41 `Makefile`/`Makefile.in` inputs, each with relative path, SHA-256 and byte
count.  It also pins the matching imported `config.h`, `config.status`, root
Makefile/template and original executable project.

No configuration projection, configure operation, source copy, nmake,
compiler, linker or executable occurred.  Reusing its output root is refused.

## Runtime profile

The separately hashed profile uses only existing Bochs configuration controls:
`textconfig`, `nogui`, one CPU runtime declaration, 4 MiB RAM, declared
BIOS/VGA ROM paths and one optional-plugin denial declaration.  The denial
includes the previous mismatch witness `gameport` as well as the existing
optional defaults.

The profile does not claim that all compiled support is enabled.  It contains
no adapter/CLI/OpenNT input, host path, BOP/DEM/DOS/WOW behavior or guest
runtime instruction.

## Next gate

A future fresh build root must copy this tuple as a unit and verify every
listed hash before creating the renamed target.  It must not replace
`config.h`, edit a generated Makefile or run configure.  Only then can a
separate dry-run verify that the retained original graph produces the named
container target without `all` or a `bochs.exe` output.
