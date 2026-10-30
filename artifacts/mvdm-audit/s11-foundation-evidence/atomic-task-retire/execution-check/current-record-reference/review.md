# Current-record incoming-reference scan

ADAPTER-WOW-107 replaces the project-owned incoming-reference scan over all
65,535 handle slots with the existing live borrow-record list and the separate
desktop backing. `page_domain_prepare_window` inserts the record before any
`page_domain_commit_window` publication; ordinary release retains the record
until withdrawal. The only normal slot publication and removal sites are
`page_domain_commit_window` and `page_domain_withdraw_window`; domain teardown
also clears slots. The existing `window_borrow_is_current` test rejects
retired, prepared and displaced generations. The same four incoming edges
and self-exclusion remain checked. No new index or retained authority is added.

The x86 worker was rebuilt and the real-CCPU graph-snapshot and window-reclaim
fixtures exited zero. They exercise atomic link changes, retirement, reuse,
retained readers, cleanup failure and teardown. Logs and current source and
worker hashes are retained here. These bounded fixtures do not exhaustively
prove corruption or arbitrary concurrent behavior; the guest execution and
native-callback exclusion of this page-domain operation is still required.

The graph fixture was extended with a narrower desktop-only incoming-edge
case: after other windows are retired, the desktop alone points to the parent.
Retiring that parent must fail without changing the desktop edge; after
removing the edge, the fixture continues its ordinary lifecycle assertions.
The actual selected-CCPU graph fixture passed with
`WOW_DESKTOP_INCOMING_EDGE_CCPU guarded=1`; output, exit code and fixture
source hash are retained as `desktop-edge*`. This specifically verifies the
desktop path that is outside the ordinary record list.

The scan now scales with retained window records instead of the fixed handle
table size; each retained record still incurs its current-generation check.
No end-to-end latency improvement is claimed before a sealed-package A/B
test. This host adapter change does not modify the original mirror or guest.

A subsequently sealed ten-image candidate including this worker and the
Alt+Enter changes passed the 59-file immutable guest check and three SOL
Options open/close/parent-recovery rounds; the launcher exited zero. The
package manifest SHA-256 is
`360D80B7BF50EB93B75FCBD1E59BAE7362D05117CC7649DBAAF966B1F467062E`,
and the probe, observer and manifest are in `sol/`. Open times were
4.672/2.406/1.703 seconds; closes were 1.906/1.875/1.938 seconds. The first
outlier and unpaired measurements do not establish a performance improvement
or exclude a regression. Full product validation remains open.

A subsequent ordered A/B/A comparison ran three SOL interactions per
package. The nine non-NTVDM unsealed inputs matched exactly; NTVDM inputs
differed. A1 opens were 2531/1734/1844 ms and closes 2203/2141/2031 ms;
B opens were 2562/1750/1734 ms and closes 2203/2063/2094 ms; A2 opens were
2609/1859/1859 ms and closes 2359/2266/2047 ms. All dialogs recovered and
all launchers exited zero. The `ab/` directory retains the runner, per-run
reports and sealed manifests. This short non-randomized sample shows no
clear end-to-end improvement or significant regression; it does not exclude
rare outliers or establish that the changed scan is a dominant latency source.
The bounded reason for keeping the candidate is its reduced asymptotic scan
over existing records, not a claim that SOL is now responsive.
