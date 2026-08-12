# T95 S6 MSVC Native-Container Manifest 001

## Result

`tools/New-T95S6NativeContainerManifest.ps1` produced the first deterministic
input manifest at:

`artifacts/analysis/t95-s6-msvc-native-container-manifest-001-20260811-001/`

Its two outputs are `native-container-manifest.json` and the prospective
`native-container-profile.bochsrc`.  This is configuration evidence only: it
did not copy a source tree, project a `config.h`, compile, archive, link, or
execute Bochs.

## Frozen inputs

The manifest records SHA-256 identities for the imported `src/bochs/config.h`,
the existing CPU3 projection used by the S6 compile-only observation, the
original `Makefile`, `vs2008/bochs.vcproj`, `vs2008/bochs.sln`, and the two
original reset-fixture ROM inputs.  It records BuildTools MSVC Win32/x86 with
the observed `cl` 19.43 / `link` 14.43.34809 identity and `/MT` CRT model.

It preserves the original generated-Makefile link-template order; it does not
mistake the VS solution's project order for a reduced link vector.  The
prospective profile limits itself to original `textconfig`/`nogui`, CPU3,
4 MiB RAM, the two mapped ROM names and the original optional-plugin denial
gate (`unmapped`, `biosdev`, `speaker`, `extfpuirq`, `parallel`, `serial`).

## Boundaries and checks

The manifest explicitly excludes adapter, CLI and OpenNT objects, all guest
and service semantics, the default-off `#UD` interception seam, hand-picked
Bochs core-object assembly, the VS2008 solution, UCRT artifacts, `all`, and
`bochs.exe`.  Its expected native evidence is narrowly reset plus controlled
stop; no DOS, BOP, DEM, WOW, search, CLI or guest-startup result follows from
this record.

The generator refuses an existing analysis root.  The first generation and a
same-root collision check both passed on 2026-08-11.  Static inspection
confirmed manifest schema `ntdos64.t95-s6.native-container-manifest.v1`,
CPU3, `/MT`, fourteen original link-template entries, six explicit exclusions,
and the required `display_library: nogui` profile line.

## Next admission condition

This manifest closes only the input-freezing prerequisite from
`t95-s6-msvc-native-container-recipe-design.md`.  Before any first fresh-root
MSVC container observation, the coordinator must separately admit an exact
source-copy/projection procedure and a non-product target that consumes this
manifest.  A successful build alone will still not establish adapter
integration or a guest runtime.
