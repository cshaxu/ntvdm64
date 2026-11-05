# M0 T474 S5 — LF for project-owned text

The owner requires LF for non-mirror components and original bytes for mirrors.
`.gitattributes` now defaults automatically detected text to LF, with original
mirror and pinned third-party exclusions; `.editorconfig` defaults to LF and
unsets text formatting for original source subtrees. Coding rules record the
boundary. Historical tool text already checked out with CRLF retains that form.

The tracked working-tree audit normalized 641 project-owned UTF-8 text files;
most had already been LF in Git. Of the component source files only
`process-launch/process.c` and `process.h` changed Git bytes, and their diff is
empty when line-ending differences are ignored. Binary media, original guest,
MVDM/non-MVDM mirrors, pinned NXVM/third-party sources and raw audit artifacts
were excluded. An additional 38 historical tool files were restored to their
manifest-identified original CRLF and their SHA-256 hashes match their original
manifests; two corresponding manifest text files were also restored. No guest
or mirror content change is present in the Git diff. Windows CMD executed an
LF-only script successfully.

The Win16-style working tree currently has uncommitted process-launch adoption;
the full `process-hook` (26 files), `process-image` (2) and `process-launch`
(6) trees were verified byte-identical to that working tree. No neighboring
files were modified. Cached x86/x64 production builds and x86 DLL builds passed. The 39-case
attribute test, nine-case command-line/real child launch test, LF-only CMD
smoke test and documentation governance check passed. The ten rebuilt product
images were compared with the previously tested S4 package: all differences
were restricted to PE COFF and IMAGE_DEBUG_DIRECTORY timestamp fields, verified
by parsing their PE sections. No new product binaries were published and no
full product suite was repeated for this line-ending-only change; the S4 full
suite and its published package remain the runtime baseline.
