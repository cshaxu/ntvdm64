# S13: CCPU shift and original guard diagnostic lines

Ten product-observed mirror files were changed. Two CCPU shift files retain
their selected original bodies and restore diagnostics for rejected alternatives;
their conditional reviews are complete. Eight additional sources retain the
existing SCCSID/SEGMENTATION exclusions, now with original error and following
line mappings; their other condition groups remain pending. Exact source
identities and boundaries are in r635 and r636. Eight existing negative-test
ledger anchors were updated to match the revised physical locations.

Cached x86, x86-extra and x64 builds passed. The mirror negative test passed
181 retained guards; 16 additional isolated MSVC controls checked rejected
branches and original diagnostic/continuation lines. Eight product images are
byte-identical to the previous release and two differ only in PE timestamps;
see [image comparison](s13-r637-build-image-compare.json). Prior Full 17/17 is
reused under the owner's instruction, not rerun. All ten images were published
and hash-verified at both destinations. Guest files were not changed.

The broader mirror and supported-context audit remains open.
