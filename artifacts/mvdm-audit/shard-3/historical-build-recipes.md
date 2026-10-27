# Historical NT build recipes — shard 3 (corrected configuration-time audit)

The previous statement that all 26 recipes were unused was incorrect: `ninja -t inputs` only exposes the already-generated build graph, and does not show files read earlier by the generator. The corrected `historical-build-recipes.csv` records two separate checks: configuration/graph-generation consumer and generated NTVDM/WOW32/VDMREDIR input closures, alongside original recipe lines and SHA-256. All 26 still match frozen source hashes.

Three original SOURCES files are **partially integrated**, not excluded:

- `softpc.new/base/disks/sources`: `New-T310OriginalSoftpcNinja.ps1` resolves `$disksManifest` and feeds its C member list to `Get-OriginalSources` at line 186.
- `dpmi32/sources`: `$dpmiManifest` is parsed at line 231; the generator adds portable `dpmimemr.c`, `dpmimscr.c` and `i386/dpmi386.c` explicitly. The historical `i386_SOURCES` list is not generally replayed as written.
- `oemuni/sources`: `$oemuniManifest` is parsed at line 236 for its C members. The `UMTEST=toemuni` declaration is not thereby a selected product test.

`Get-OriginalSources` reads the manifest with `Get-Content -Raw` and extracts C names from the `SOURCES` block, stripping comment lines and stopping at architecture/other directives. It does not establish full NT build directives, resource, target, test or architecture semantics. Actual source functions remain independently subject to compilation, linkage and behavior evidence. Their ledger classifications are now `partial` for this limited recipe integration, even though the underlying recipe bytes match OpenNT.

The other 23 entries are excluded **as recipe inputs only** for the examined current worker/WOW generator and three target closures: NTMAKEENV `makefile.def`/`.plt` shims and other historical standalone or architecture-specific recipes. We inspected the current worker generator's named manifest bindings and the worker/WOW build-tool text; nonselection does not exclude their source features or independently run standalone tool recipes. Target closure hashes remain in `historical-i386-binaries-review.json`; this is a scoped build-path finding, not a guarantee about every auxiliary/test generator or future configuration.

`.inc` declarations are outside this recipe batch: `CDEFINE.INC` can alter preprocessor switches and `sim16/*.inc` may define guest assembly ABI. Guest, product source and combined ledger were not touched. No build or runtime tests were performed in this correction.
