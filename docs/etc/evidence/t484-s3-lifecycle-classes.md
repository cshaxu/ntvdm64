# T484 S3 P123 lifecycle contracts and migrated-producer cleanup

Continue B0195 using the P122 shared native graph. Eight additional public cases
now use the same CMD entry: original-hook-cleanup, task-order, thunk-scope,
start-notification, menu-cleanup, class-client, class-query and class-lifetime.
Retain their distinct contracts: hook removal/foreign preservation, original
scheduler eligibility/order, nested thunk lifetime/unwind, startup identity and
retry, menu disposal, class registration conversion, query/commit rollback, and
shared class/window lifetime. These are native bounded tests, not guest acceptance.

The two class/menu parent profiles reuse the existing storage fixture. Original
hook/thunk callbacks retain the production /Gz convention; task-order retains
strict incompatible-pointer diagnostics. No new fake provider or product path
is introduced. Already migrated cases are also cleaned: remove46 net lines of
unreachable old producer branches, including the native-nested source extractor.
Runtime assertions remain; no valid case was retired merely for lacking a caller.

Initial attempt PID26592 passed hook/order but thunk exited c0000005. Existing
exception observer PID19060 reproduced the read at address20, fixture RVA88f6.
The linked map identifies wow_window_words_allocate: the fixture called attach
without binding the existing window access provider. Add its existing fixture
header and bind call, as other current fixtures do. This selects actual original
layout functions, not a success stub. Production code is unchanged. Counter-review
checked the provider declarations, original calling convention and retained
nested exception assertions; no assertion is weakened.

PID30220 then passes all eight, cached full host build and11 raw/22 published
hash comparisons. After obsolete producer removal, PID3616 passes all17 native
public entries (including P121/P122 regressions), hash/mtime cache identity and
x64 rejection2. Existing dual publication remains byte-identical; not republished.
No mirror, original guest or installed package changes.

[Receipt](../../../artifacts/build-migration/t484/s3-lifecycle-classes/result.json)
records final identities. Raw evidence is build/t484-s3-lifecycle-classes,
including attempt1, thunk-debug, cases and final-cases. B0195 remains partial:
17/30 ordinary public cases migrated,13 plus fault variants remain. Overall
completed dispositions remain225/355,130 open. B0166/B0169 and retrospective
review are still open, as are S4/S5; this batch does not close S3.
