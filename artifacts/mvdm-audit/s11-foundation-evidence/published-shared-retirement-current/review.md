# Published baseline: same-worker task retirement

The sealed ten-image baseline in the retained package was rechecked against
all ten `assets/release` image hashes. Its unmodified WINMINE ran in the same
real WOW worker as the unchanged prebuilt common-dialog witness (SHA-256
`406D84F9E4EA04CD269045B354E95C47A342C986E0400A2C4F6A58DC818D3A02`).
The guest checkpoint reports 17 dialogs, 16 explicit destructions, one live
at task exit, and zero errors. The existing runner verified one successful
retirement, resource callback ordering, surviving WINMINE flag/reset/open,
window close and launcher exit. The copied results and binary input hashes
are retained here; the long trace remains under
`build/t473-s11/trace-scalar-shared-retirement-r2`.

The first attempt passed `-VisibleDesktop` with a sibling, which the runner
rejects before launch. A second attempt incorrectly supplied 17 explicit
rounds to the prebuilt 16-round witness; the runner correctly rejected the
456-byte output count after the processes exited. The successful third
attempt used the private desktop and 16 explicit rounds. No guest was rebuilt
or modified, and no product was changed. This strengthens the latest release
identity's shared-task retirement evidence; it does not force the historical
race, diagnose SOL 1400, prove every task lifecycle or substitute for Full.
