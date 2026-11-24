# T484 S3 P205: native presentation contract producers

The two existing formal verification gates remain distinct: startup presentation
selection and console font restoration. Their inline C fixture templates move
unchanged into test-owned .c.in files. One CMD/native builder produces current
and regression-control variants; one runner accepts either contract or both.
Invoke-ProductVerification retains both gate names and selects the corresponding
contract. It no longer reaches their generated PowerShell compiler scripts.
The existing presentation-font snapshot target tests a different lifetime owner
and remains unchanged; new targets use presentation-contract-* names.

The initial configure run PID3752 failed on that existing target-name collision,
not on product behavior. Final owned run PID26696 uses run-final.ps1 under
build/t484-s3-presentation-contracts after the first run terminated. The four
actual results are font fixed=0/control=1 and startup fixed=0/control=1, exactly
as required. The controls still remove font restoration or select the wrong
presentation query. They prove the assertions reject those regressions, not
that a product failure is passing. Controlled fixtures are not real guest tests.

Independent old-runner extraction PID22632 verifies all four generated C files
against previous script output after newline/final-separator normalization.
No C assertion, original guest, product or mirror source changed. Full cached
host build and11 raw/22 dual-published identities are verified by the final
runner; unchanged bytes need no redundant publication. Governance, parsing
and diff checks precede commit/push. Final clean/Full acceptance remains S5.

Two direct-command candidates are delivered;13 remain. Frozen316/355 and23
supplemental preparation/staging chains remain separate unchanged counts.
S3 remains active; this does not close S4 or S5.
