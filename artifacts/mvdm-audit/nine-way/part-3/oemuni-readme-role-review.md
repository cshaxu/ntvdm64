# Part 3: OEMUNI private header and mirror README

`oemuni/oem.h` defines the private `InitOemString` and `BaseSetLastNTError` macros. `oemuni/file.c:12` and `oemuni/process.c:12` include it; those two files are selected by the `New-T310OriginalSoftpcNinja.ps1` OEMUNI archive generator, which includes that archive in the worker link graph. Filename resolution and final linked-image identity were not independently checked here. `Generate-T420S17OemUniNinja.mjs` also creates separate OEMUNI fixtures; their existence is not proof that they were run in this review. Recommendation: retain this private original interface; verify actual build dependencies, conversion/error behavior and lifecycle under its existing feature owner.

`README.md` is project-maintained MVDM divergence and evidence documentation, not an original upstream source file or runtime executable. Its current content includes Video7 and BOP divergence entries; their historical status text is not updated by this audit. It belongs to the mirror's documentation/provenance owner and should not be classified as original source implementation, code diff or execution evidence merely by residing under `src/mvdm`.

Neither item establishes runtime behavior or an overall functionality percentage. This review runs no code and changes no product source.
