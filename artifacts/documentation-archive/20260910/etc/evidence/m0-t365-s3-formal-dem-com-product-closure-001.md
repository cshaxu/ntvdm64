# M0 T365 S3 — Formal `.COM` DEM product closure

## Result

The complete source-shaped `.COM` file lifecycle selected by T365/S2 is a
real input of the CPU40/x86 product, not a fixture-only or archive-only
implementation.

The frozen formal graph is `build/M0-T365/S2/x86/build.ninja`.  Its exact
selection is:

```text
demfile.c + demhndl.c
  -> obj/dem/demfile.obj + obj/dem/demhndl.obj
  -> original-mvdm-dem.lib
  -> original-softpc-process.exe

mvdm_host_identity.c + mvdm_guest_location.c
  -> softpc-bindings.lib
  -> original-softpc-process.exe
```

The generated graph contains no alternate app loader or replacement DEM
provider on this path.

## Reproducible verification

In an MSVC x86 developer environment:

```text
ninja -C O:\repos.hobby\ntvdm64\build\M0-T365\S2\x86 -j 8 original-softpc-process.exe
ninja -C O:\repos.hobby\ntvdm64\build\M0-T365\S2\x86 -t query original-softpc-process.exe
ninja -C O:\repos.hobby\ntvdm64\build\M0-T365\S2\x86 -t query original-mvdm-dem.lib
ninja -C O:\repos.hobby\ntvdm64\build\M0-T365\S2\x86 -t query softpc-bindings.lib
```

completed with exit code 0.  The rebuild reported `ninja: no work to do`,
after the successful S2 cold build, and the three queries showed the exact
inputs above.

## Boundary retained

This is a formal input/link closure only.  It makes no claim that a guest
`.COM` was loaded, that `$Exec` completed, or that parent return works at
runtime.  T365/S4 owns one frozen workload observation; any result beyond its
first source-owned terminal remains an input to later work, not an automatic
repair list.
