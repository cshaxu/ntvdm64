# S13: unselected legacy sources and floppy diagnostic lines

Nine product-unselected C/H files were removed and four obsolete standalone
build inputs retired; the tenth changed source file, `floppy.h`, retains its
selected code and restores four original diagnostic line anchors. Exact
identities and boundaries are in r631–r633. Original guest `enumapis.inc`
remains byte-identical to its recorded hash; no guest was built or changed.

The historical CCPU ABI ledger generator ran successfully with 62 rows using
the pinned original header. The adapted original CallBack16/CCPU fixture was
rebuilt and returned `ORIGINAL_CALLBACK16_ACTIVE_CCPU_BOP_ORDER_OK`. The C-VID
accessor audit completed using the pinned upstream comparison file. The mirror
negative test passed 181 retained guards after three records for already
deleted sources were explicitly marked withdrawn; its initial missing-file
failure was not treated as a pass. Four isolated floppy diagnostic controls
reported the original lines.

Cached x86, x86-extra and x64 builds passed. Eight product images are
byte-identical to the prior release and two differ only in PE timestamps; see
[comparison](s13-r634-build-image-compare.json). Prior Full 17/17 product
evidence is reused under owner instruction; it was not rerun in this batch.
All ten components were published and hash-verified in both destinations.
Remaining file reviews and complete supported-context verification are open.
