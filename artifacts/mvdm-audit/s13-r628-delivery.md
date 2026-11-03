# S13: unselected mirror alternatives and retained utility

Ten unselected C/H files were removed after product graph and source reference
checks; r625 and r627 identify their original and removed byte hashes and
test-only cases. The C-VID fixture previously selected the removed test-only
`base/inc/j_c_lang.h`. Actual MSVC preprocessing now resolves the product
`base/cvidc/j_c_lang.h`; the fixture was rebuilt and passed its read, write,
mark, negative and concurrent checks. The modified mirror-guard test also
passed its 184 retained guards and additional checks.

The existing tracked `graftabl.com` was copied without modification into
`assets/winnt.zip`; ZIP integrity and all existing entry hashes were checked.
Its provenance and runtime behavior remain unverified, as stated in
[the archive evidence](s13-r625-graftabl-archive.json).

Cached x86, x86-extra and x64 builds passed. Eight of ten product images are
byte-identical to the previous release and two differ only in PE timestamps;
[comparison](s13-r628-build-image-compare.json). The previously passed Full
17/17 is reused per owner direction; it was not rerun here. All ten components
were published and hash-verified at both release destinations.

This does not complete the remaining mirror audit or establish every supported
build context. The current filesystem census and pending files are tracked in
[the census](s13-r624-current-file-census.json).
