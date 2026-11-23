# T484 S3 P73 native WOW host attach failure producers

B0100 now delegates AttachFailure and MissingInit source projection, selected
WOW object compilation and linking to native CMD targets. Marker bodies move
unchanged from the old runner into explicit test include templates. Projection
boundaries require unique matches; the original product source and guest are
never written. MissingInit changes only the selected W32Init export to the
existing W32InitS31Hidden alias. The output and failure-mode runner contract is
preserved, with owned background process and unchanged normal provider checks.

The one-object provider composition is shared with P72 callback-abort. It uses
the real original object profile, product object order, import libraries and
export definition; it does not scrape Ninja command strings or fork the product
source manifest. All three diagnostics remain optional, outside publication.

Actual builds succeed for both modes and callback-abort. Independently running
the exact source-projection portion of the pre-change runner produces equal
AttachFailure46093-character and MissingInit46168-character source after LF
normalization and optional final LF. This compares whole source, not just
insertion markers. It does not claim the DLLs are byte-identical to the old
historical compiler graph or that guest failure scenarios were rerun.

Raw evidence: build/t484-s3-host-attach. Concise receipts:
artifacts/build-migration/t484/s3-host-attach. Source/ABI projection, object and
export inspection, unit tests and guest runtime claims remain separate.

Final checks pass for all three diagnostics:229 objects each, exactly one
replacement, stable image hashes/timestamps, expected W32Init versus hidden
initializer exports, x64 rejection and callback-frame fixture. Cached full
build passed;11 host input and22 publication hashes are unchanged, no recopy.
The first final check failed before dumpbin due to a verification-only CMD
path spelling; the corrected check passed and the failure receipt is retained.
No guest fault scenario is claimed passed. Fixed355 inventory:150 completed,
205 remaining (50 pending producer chains,155 awaiting reconciliation).
