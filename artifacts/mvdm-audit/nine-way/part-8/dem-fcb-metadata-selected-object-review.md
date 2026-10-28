# DEM FCB metadata selected-object high-size probe

The x86 probe links the selected object `build/.ninja/msvc-x86-release/worker/obj/dem/demfcb.obj` (SHA-256 `40550EA098C987667A0114709625B57D814AD25EB14E4286B5D644188ADC872D`) and calls its `demGetMiscInfo` directly. The only project seam supplied is a no-op `demClientError`; Win32 file functions are real. A temporary file is marked sparse before its logical length is increased to 4 GiB, and opened with delete-on-close. The fixture closes the handle itself after checking it.

Built with the local i686 MinGW GCC and `-Wl,--gc-sections` against the object and `dem-fcb-metadata-selected-object-fixture.c`; linker reported `Warning: corrupt .drectve at end of def file` but returned success. The resulting x86 process returned 0 and printed:

```text
highsize result=0 handle_live=1
```

This establishes that the selected object returns failure on a real high-size file while the passed file handle remains open. It does **not** establish that a guest opened such a file through `demFCBCommon`, that any error response is correct, or that the selected object matches an installed release. The sparse operation is checked and the probe exits without growing a file when unsupported. Other metadata failures and normal controls need separate coverage. The original/current source comparison and caller cleanup concern remain in `dem-fcb-open-review.md`.
