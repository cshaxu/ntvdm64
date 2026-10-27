# Historical i386 build artifact reconciliation

Frozen reconciliation SHA256 F73FFD45DFF44FCA4ECEFC49EF769BEAB5C085542CD04FFE4D6A7B5F942E3196. 66 pending current-only obj/i386 artifacts; 66 excluded only as retained build inputs. Per-file paths, frozen/actual hashes and results are in `historical-object-batch.csv`.

Package-c scanned 83 files by SHA256. Input membership uses Ninja `-t inputs` and resolved exact source paths for these targets:

- `build/.ninja/msvc-x86-release/worker` / `ntvdm.exe`: graph SHA256 `3706BBEAE9FEF8D67A9BAAC6E5CAFF1A7BDB44077F97DAB0DC1FCB0DC3433F83`; 1014 unique inputs.
- `build/.ninja/msvc-x86-release/worker` / `VDMREDIR.dll`: graph SHA256 `3706BBEAE9FEF8D67A9BAAC6E5CAFF1A7BDB44077F97DAB0DC1FCB0DC3433F83`; 1105 unique inputs.
- `build/.ninja/msvc-x86-release/wow32` / `wow32.dll`: graph SHA256 `03FE1C4AEB2CFB7434FA1471C55A3EF62652254615E630BA5861AE7F8729E278`; 454 unique inputs.
- `build/.ninja/pkg-uuid-x86/worker` / `ntvdm.exe`: graph SHA256 `B1BC7B95C2A9DBC1A9DD8CC88931D86941EE146017C37A5ECCB6843FA80AC470`; 1014 unique inputs.

No artifact is deleted or executed. A negative exact input/hash result says nothing about original source functionality, guest capabilities, provenance, unexamined configurations or future package policy.
