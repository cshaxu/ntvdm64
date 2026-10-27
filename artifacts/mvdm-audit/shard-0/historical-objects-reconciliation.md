# Retained historical build products

The 75 paths in `historical-objects-reconciliation.csv` are current-only
historical `obj/i386` objects, libraries, resources, debugger files or sample
DLL outputs. Each file hash matches the frozen row. The selected four Ninja
input closures contain no exact source-tree path, and no file in package-c
matches any of their hashes. This supports excluding these retained files as
build inputs, not excluding their source files, original feature or guest
capability. Unknown provenance remains recorded; bytes are retained.

Checked current Ninja build graphs (graph identity is not runtime behavior):

- `build/.ninja/msvc-x86-release/worker` / `ntvdm.exe`: `3706BBEAE9FEF8D67A9BAAC6E5CAFF1A7BDB44077F97DAB0DC1FCB0DC3433F83`
- `build/.ninja/msvc-x86-release/worker` / `VDMREDIR.dll`: `3706BBEAE9FEF8D67A9BAAC6E5CAFF1A7BDB44077F97DAB0DC1FCB0DC3433F83`
- `build/.ninja/msvc-x86-release/wow32` / `wow32.dll`: `03FE1C4AEB2CFB7434FA1471C55A3EF62652254615E630BA5861AE7F8729E278`
- `build/.ninja/pkg-uuid-x86/worker` / `ntvdm.exe`: `B1BC7B95C2A9DBC1A9DD8CC88931D86941EE146017C37A5ECCB6843FA80AC470`

This comparison does not cover other build configurations or future
packaging. Firmware ROMs, generated source and guest binaries are not in this
cohort. No executable was run, modified, deleted or linked for this audit.
