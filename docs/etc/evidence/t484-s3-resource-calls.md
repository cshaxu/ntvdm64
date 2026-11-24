# T484 S3 P118 resource contracts and migrated-test retention

B0191 keeps three distinct current contracts: original WU32LoadCursor and
WU32LoadBitmap caller ordering/aliasing, and the actual compatibility resource
provider versus native USER. Current C assertions remain unchanged. No suite
caller is claimed; these bounded regressions do not establish guest acceptance.

Remove the independent PowerShell extractor and manual compile/link builder.
Reuse the existing native projection and fixture machinery with a single
wow-resource-calls CMD group. Both projected bodies match the previous selector
ignoring whitespace. Remove three uncalled message/menu stand-ins. First run
PID19012 failed linking the two message symbols; the final graph links their
existing real compatibility provider, without adding substitute implementations.
The menu stand-in was obsolete. No production source or original guest changed.

Retrospectively reviewed the already-migrated targets sharing this build owner:
- GDI alias: stable/reused/retired handles and table exhaustion remain unique.
- Hard-error decoder: sparse source slots, defaults/cancellation and invalid
  descriptors remain unique.
- Hook native binding: module/thread selection, retired/foreign thread and
  handle lifetime remain unique; its API observer is essential to that contract.

These three targets remain justified, not retained merely because migrated.
This is source/coverage review, not a new runtime pass for those three targets.
The retrospective review of all migrations is still incomplete.

Final owned PID14500 exits0: cursor three cases, bitmap two outputs, resource
binding/public modes; cached full host build; stable cached binary hash/mtime;
x64 target rejection2;11 raw product hashes and22 published copies unchanged.
Existing dual publication verified, not republished. Executable assertions are
unchanged; source changes net32 fewer lines before evidence records.

[Receipt](../../../artifacts/build-migration/t484/s3-resource-calls/result.json)
records validation, first failure, projections and markers. Raw evidence:
build/t484-s3-resource-calls. S3 remains active; B0166/B0169 and S4/S5 remain open.
