# S13 per-file audit checkpoint r1052

The owner superseded breadth-first per-depth processing with complete-file
review. The previous `completed-original-witnessed-depth1` state was only an
outer-branch result: 131 entries were reclassified as partial, not claimed as
fully audited. Later source proof completed `vdmredir.h` in its entirety; the
ledger retains its original/current conditional mapping and product evidence
in `s13-r1052-vdmredir-full-file.json`. Eleven previously unselected i386 and
UNALIGNED_VDM_POINTERS arms became #error with original line anchors; selected
arms remain. Supported compiler macro sets and prior branch observations were
checked. No guest file or image was modified or rebuilt.

The ten changed C/H paths in this batch include that file and nine earlier
ordered outer-review files. Their first-level evidence is partial unless their
complete conditional tree has been verified. Source changes to the other nine
only adjust anchors in pre-existing #error bodies. Earlier scanned ambiguities
remain open; their detailed evidence is in the current per-file inventory.

Cached x86 and x64 product builds passed. The coherent ten-image package
compared with the previous release has two identical images and eight with
PE timestamp-only differences, zero substantive byte changes. See
`s13-r1052-build-image-compare.json`. Under the owner's no-repeat rule prior
Full evidence `s13-r682c-full-evidence.json` is reused; no fresh runtime suite
was run. Ten images were published and hash verified at both `assets/release`
and `O:/winnt/system32`.

The current census reports 1,058 existing C/H entries: 168 reviewed identical,
700 reviewed different and 190 not yet complete, of which 130 have outer review
only and 60 have identified unresolved boundaries. The 578 rename-aware net
deleted C/H paths versus baseline c97d4ee2c are separate from the 1,058.
Previous completion classifications other than explicit first-depth states
retain their evidence but still need validation before a final claim. One present
entry is a reviewed product-generator input outside observed graphs.
