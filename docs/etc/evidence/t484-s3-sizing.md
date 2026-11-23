# T484 S3 P76 window sizing diagnostic producer

B0102's previous runner temporarily rewrote two product files and rebuilt the
normal DLL before restoring the source. Its old F paths no longer exist; both
implementations now live in compat. The observation points remain present:
two DispatchClientMessage sites and one trace_creation site.

The native window-sizing target projects those exact current sources into its
diagnostic build directory, adds the unchanged sizing witness header, and
uses the real two owner profiles. The shared diagnostic link helper accepts
paired source/owner lists while retaining single-owner callers unchanged.
Exactly two objects are replaced at their original positions. Production
source and normal WOW32 are never rewritten. The runner keeps fresh output,
nonpublishable manifest, bounded process ownership and failure propagation.

Validation receipts are retained in
[the evidence bundle](../../../artifacts/build-migration/t484/s3-sizing/product-build-result.json).
The source projections are compared to the prior recipe after LF normalization;
object count/order, cache hash/mtime, x64 rejection, four existing diagnostic
targets, callback-frame fixture and11 raw/22 published hashes are checked.
No new WINMINE guest sizing runtime pass is claimed. The historical observation
remains in [T478 S44](m0-t478-s44-window-sizing.md); this batch changes its
producer, not the original sizing behavior or acceptance conclusion.

Cached full build PID3600 exited0; final verification PID7324 exited0.
The final229-object closure replaces exactly2 objects, cache is stable, and
the frame fixture passes. Published images are unchanged and their hashes
are verified at both destinations; no diagnostic DLL is published.
B0102 is complete. Fixed355 inventory:151 complete,204 remaining (51 pending
producer items including B0101's historical verification,153 reconciliations).
