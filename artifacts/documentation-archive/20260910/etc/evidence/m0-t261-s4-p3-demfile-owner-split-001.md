# M0 T261 S4 P3 — DEM file owner split

## Question

Can the reached OpenNT `demfile.c`/`demlabel.c` bodies retain their source
ordering, file error mapping and named-pipe calls while their three distinct
owners are separated: BOP body selection, historical OEM file API shape, and
the deferred Redirector protocol?

## Change

- Moved the project-owned `bx_ntvdm_demfile_invoke` composition source by Git
  rename from `opennt-host/dem/demfile_shim.c` to
  `opennt-bop/dem/opennt_demfile_composition.c`.
- Kept source-visible service selection and scoped CCPU/SAS call invocation in
  `opennt-bop`; it remains the sole owner of interpreting a DEM service value.
- Extracted the same-named OEM calls (`CreateFileOem`, delete, attributes,
  rename, volume information, font removal and debug output) to
  `adapter-win32` as `opennt_dem_file_oem_facade`.  The original generic-read
  `FILE_WRITE_ATTRIBUTES` retry sequence remains intact.
- Changed only the unavailable product include closures in the imported
  `demfile.c` and `demlabel.c`.  `BOP-DIV-039` and `WIN32-DIV-006` record those
  changes beside the corresponding production source.
- Retained `LoadVdmRedir`, `VrConvertLocalNtPipeName` and
  `VrAddOpenNamedPipeInfo` as a lower `opennt-host/redir` capability seam.
  They are not moved into this BOP composition: the shared session record also
  owns `57:xx` ingress, so extracting it whole would create the prohibited
  `opennt-host -> opennt-bop` dependency.  A future Redirector package must
  split its lower capability facade from its BOP route as one unit.
- `Sim32GetVDMPointer` remains an explicit non-success fallback in this BOP
  composition for unreached VDD JFT/SFT helpers.  No raw host pointer is
  exposed to a guest and no second mapping manager is introduced.

## Boundary

`opennt-bop` owns original DEM body selection and copied-state invocation.
`adapter-win32` owns same-shaped public-Win32 OEM file compatibility.
`opennt-host` retains only the independent Redirector host capability and has
no upward call to a BOP dispatcher.

## Verification

- Fresh formal MSVC x64 `/MT` root: `build/M0-T261-S4/r011`.
- The graph completed 462 recorded Ninja edges and linked all seven component
  libraries, declared fixtures and native executable.
- A repeated invocation reported `ninja: no work to do.`
- `t230-s3-demfile-direct-import-fixture.exe` returned zero:
  `seven file services and safe label failure verified`.
- `t245-s21-oem-create-file-fixture.exe` returned zero and verified OEM
  `CreateFileW` conversion/retry behavior.
- `git diff --check` and documentation governance verification passed.

## Result

The direct OpenNT file/provider bodies are still the semantic owner.  The
split removes generic file API and BOP service composition from
`opennt-host`, keeps every directly called historical symbol, and explicitly
defers only the separately owned Redirector and unreached VDD pointer paths.
