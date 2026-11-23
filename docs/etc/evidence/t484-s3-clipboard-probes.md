# T484 S3 P51: native Clipboard, DC and dialog witnesses

In original inventory order, B0054 now delegates its six Clipboard NE variants,
two native bridge DLLs and retirement EXE to CMD/native targets. B0055/B0056
reuse the existing fixed Win16 helper for DC, dialog and common-dialog probes.
No original guest, product implementation, runtime assertion, private-desktop
boundary or frozen-input validation is changed. Legacy compiler/NASM options
remain accepted but do not select or invoke a compiler.

[Clipboard verification](../../../artifacts/build-migration/t484/s3-clipboard/verification.json)
checks all three lifetime selections with and without DIB. Six binaries and six
listings are byte-identical to old NASM commands. Both bridge DLLs and the
retirement EXE differ only in actual PE timestamp fields, as recorded in
[comparisons](../../../artifacts/build-migration/t484/s3-clipboard/comparison.json).
The retirement EXE rejects missing arguments with64 without opening Clipboard.
Repeated native builds leave outputs unchanged.

[DC/dialog verification](../../../artifacts/build-migration/t484/s3-clipboard/dialog-verification.json)
executes the shared helper and checks three additional binary/listing pairs
bytewise. COMMON_DIALOG_ROUNDS remains2 in both dialog selections. All four
edited PowerShell files parse. Cached full host build,11 raw host identities,
22 published identities and governance are recorded separately. Unchanged
products require no recopy; no fresh guest Clipboard/runtime pass is claimed.
Equivalent witness binaries reuse prior coverage under owner authorization.

All three entries remain pending for shared runtime staging production. This
batch removes their direct NASM/CL production, not the transitive chain. S3,
publication migration and clean interpreter-free acceptance remain open.
