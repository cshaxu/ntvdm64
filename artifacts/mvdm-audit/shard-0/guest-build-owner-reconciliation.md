# Immutable guest owner and build-artifact reconciliation

The [712-path census](guest-build-owner-reconciliation.csv) accounts for every
remaining shard-0 frozen immutable-guest/build row at this checkpoint. Each
live file matches its frozen hash. Every path has an original ancestor
makefile or `sources` carrier; 485 have a literal filename mention in the
nearest recipe. Neither proximity nor a filename match proves transitive
compilation or that the historical source produced the shipped image.

The [summary](guest-build-owner-summary.json) retains all recipe hashes and
owner-group counts. It distinguishes 224 sources, 137 declarations, 197 build
intermediates, 29 recipes, 26 binaries/libraries and 99 resources or unresolved
support carriers. Exact byte hashes match eleven rows in the current guest
ZIP and six in package-c/system32. The archive identity is recorded separately
from package identity. Examples include original NTIO.SYS/NTDOS.SYS, WOWEXEC,
and the display image named DISPLAY.EXE in the mirror but VGA.DRV in delivery.
Different names do not imply different bytes, and equal bytes do not establish
source generation or runtime behavior. No original guest is rebuilt.

The display rename has a direct original recipe: driver `makefile:78--82`
builds `display.exe`, copies it to `vga.drv` and binplaces that name. The
current staging script explicitly chooses the original FASTOPEN executable;
the original SHARE and FASTOPEN assembly each documents an NT no-operation
stub. A matching hash across those names is therefore not evidence of a
recovered SHARE file-sharing service. These observations resolve names only,
not compilation lineage or behavior for the rest of either family.

The [221 historical carrier dispositions](guest-build-carrier-disposition.csv)
exclude only retained build artifacts from current host/runtime source roles:
162 OMF object files, 19 linker/assembler text listings, eleven symbol files
and 29 historical build/link recipes. All object bytes parse into complete
OMF record framing beginning with THEADR/LHEADR and ending with MODEND. Three
files contain nonzero checksum sums; exact record offsets are preserved as
observations, not a declaration of corruption or proof that an old linker
would reject them. All 220 such records have a zero stored checksum byte;
the distinction from a nonzero, inconsistent stored checksum is retained.
The review does not change any checksum or object.

These are historical guest link inputs or diagnostic/build outputs, not
standalone guest applications or modern host providers. None matches a whole
deployed or archived runtime file. The selected current host generators use
their own package manifests and do not rebuild guest. Exclusion does not
discard the algorithms represented in OMF code, symbol consumers, guest
resources or corresponding original source files. Their source/capability
reviews remain in scope. In particular `.RES` files are retained pending
rather than lumped together with non-runtime object/listing artifacts.

The other 491 paths remain pending with an explicit owner group and a finite
source-to-deployed-image/evidence question. Source equality alone cannot
prove guest behavior, and missing recipe mentions do not exclude an include,
resource or generated input. Existing COMMAND build evidence is preserved;
this census adds current whole-file archive/package identity and family
coverage without replacing the original narrower source/link evidence.
No original guest, firmware, runtime image or product source changed.
