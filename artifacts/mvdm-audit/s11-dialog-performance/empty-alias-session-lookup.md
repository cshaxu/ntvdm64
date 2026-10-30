# Empty-alias session lookup investigation

**Correction:** the SOL A/B/A candidate worker link emitted LNK4014/LNK4006
because the archive member removal used its basename rather than its stored
`obj/adapter-softpc/` path. The old mapping object won; these product-package
timings did not test the candidate. The standalone microbenchmark linked the
candidate mapping object directly and remains valid within its stated scope.
No production change was made.

The current project-owned `mvdm_softpc_physical_mapping_translate` initializes
`owner` using `session_thread_current()` before checking whether `aliases` is
empty. Its ADAPTER-WOW-083 comment describes avoiding a session fence in that
case; the existing implementation avoids `session_valid`, but not the TLS
accessor itself. The accessor reads thread-local binding state and does not
mutate it. Moving the accessor below the empty-list and null-output checks is
a candidate that preserves output initialization and the existing active-alias
owner checks; this is an adapter change, not a change to original CPU logic.

The SOL PC sample includes `session_thread_current`, but gives neither a stack
nor a call count. It therefore does not establish that this particular caller
is hot, that the alias list is empty during the observed interaction, or that
the change would improve end-to-end performance. The existing
`physical_alias_empty_fixture.c` covers empty, active, multi-owner, remap and
teardown behavior and includes a microbenchmark; its contract alone cannot
establish SOL improvement. No candidate has been applied to product source.
An owned isolated A/B measurement and the fixture should precede any decision.

An isolated candidate compiled with the x86 production compiler and linked to
the same fixture and session library passed the full existing fixture. Five
alternating baseline/candidate runs measured respectively 5.013/3.017,
6.167/4.893, 5.051/2.977, 5.520/3.186 and 5.020/2.723 ns per empty-alias
call, with 2,000,000 calls and zero invalid results in each run. The candidate
source, binaries and paired records are under `build/t473-s11/alias-candidate`;
the original object and baseline are under `build/t473-s11/alias-baseline-r2`.
This demonstrates a microbenchmark improvement, not an end-to-end SOL gain.
The candidate remains outside product source until that distinction is tested.

The candidate was then compiled with the actual x86 worker flags, linked into an
isolated ten-image package, and compared with the current package in three
ordered SOL runs per package (A/B/A). All nine dialogs opened and closed. Open
times were 2422/1906/1828 ms, 3703/2562/2422 ms, and 2703/2078/2078 ms;
close times were 2093/2110/2391 ms, 2906/2828/2907 ms, and 2359/2281/2297 ms.
As corrected above, this sample used the original worker object and establishes
neither candidate gain nor regression. The candidate is not adopted; product
source is intact.
The independent staged manifests, probe output, build hashes and microbenchmark
are retained in `../s11-foundation-evidence/atomic-task-retire/execution-check/sol-empty-alias-ab/`.
The first A/B runner used Windows PowerShell 5 and failed before executing a
case because its JSON array was not enumerated like PowerShell 7; the recorded
successful comparison used PowerShell 7. This is a test-harness compatibility
issue, not a product result.
The manifest helper now assigns the parsed JSON before wrapping it as an array,
including the fresh-identity path. Actual current ten-image hash validation
passed in both Windows PowerShell 5 and PowerShell 7. The fresh-identity path
was subsequently rerun under PowerShell 5 and also passed.

The candidate was rebuilt with the exact archive member path and verified to
have one mapping object and no LNK4006/LNK4014 warnings. Nine other component
inputs matched the integrated baseline's sealed images. All nine SOL A/B/A
interactions passed. Open times were 2453/1625/2610, 3578/1672/1875, and
2547/1985/1969 ms; close times were 1812/2406/2781, 2219/1890/1859, and
2156/2265/2140 ms. This small ordered comparison shows no consistent
end-to-end gain and does not establish a regression; the code remains outside
product source. [Build and probe receipts](../s11-foundation-evidence/atomic-task-retire/execution-check/alias-fixed-ab/)
include the link log, candidate and staged manifests.

Following the owner's criterion permitting a measured time improvement in
project-owned compatibility code without a large net code increase, the exact
fixture-tested candidate was applied to the working tree. On recheck the
working source and fixture candidate both had SHA-256
`EE2F29D94720391DBCE97F9CB1A81E2D5255E36E848FDF6E0695A4F562C0E6CA`.
The existing candidate fixture again passed all empty, null, owner, bounds,
multi-owner, remap, teardown and physical-retention checks, reporting 2.614 ns
per empty lookup for 2,000,000 calls on that run. This is a linked candidate
fixture, not a fresh coherent product build or evidence of an end-to-end SOL
speedup. The original CPU code is unchanged and broader delivery gates remain.

The actual cached x86 worker graph was then built from the working source:
Ninja compiled the mapping source, rebuilt `softpc-bindings.lib`, relinked
`ntvdm.exe`, and passed the VdmTib ownership check. The immediate second
build reported `ninja: no work to do`. The resulting worker SHA-256 is
`E9046FE061570A57E867F949819F290BF7F019720A2B017D88492D3D407856CF`.
This is a worker build, not a newly sealed ten-component product package or
the product regression suite; previous package results do not certify it.

An isolated package was staged by replacing only NTVDM in the previously
tested `retire-capacity-integrated` package with the newly built worker;
the staging tool verified the selected host images. Its product manifest
SHA-256 is `FD0C73D8BEF82B8B2E17937441BB0731CF8539E1C36B8A4798DA1CD9DE9D4493`.
The guest manifest check passed 59 immutable files (configuration excluded).
This is an isolated candidate with the other nine host images inherited from
the earlier package; it is not a release or proof that every current source
change is represented. Its product integration tests remain outstanding.

The staged package then passed one SOL Options open/close/parent-recovery
probe and its launcher and observer exited zero. Opening took 3109 ms and
closing 2750 ms, with game-thread CPU accounting near those wall times. This
confirms a basic integration path for this package, not a speedup or a Full
pass. Reports and package manifest are retained in
`../s11-foundation-evidence/atomic-task-retire/execution-check/alias-worktree-sol/`.

The same staged manifest subsequently completed the existing Console-17 and
Window-17 exit-status matrices. Both summaries contain 17 cases and zero
actual/expected mismatches; the owned runner exited zero after stage cleanup.
The manifest and summaries are retained in
`../s11-foundation-evidence/atomic-task-retire/execution-check/alias-worktree-matrix/`.
These test results do not cover all Control, WOW application, Full or
performance requirements and are not a release qualification.

The same ten image hashes subsequently passed all 12 Control gates in an
isolated PowerShell 7 run; runtime manifest and timing records are in
`../s11-foundation-evidence/atomic-task-retire/execution-check/alias-worktree-control/`.
An initial preparation attempt lacked four test helpers, and a second attempt
used Windows PowerShell 5, where `ProcessStartInfo.ArgumentList` is null;
neither executed the full suite. The successful run copied the existing
helpers and used the prior PowerShell 7 path. These results do not establish
Full acceptance or solve the multi-window performance issue.

With the same sealed manifest, one Notepad Find and one Write Find dialog
opened, closed, returned to the parent and allowed the launcher to exit zero.
Notepad opening/close recovery took 1218/438 ms; Write took 1484/1094 ms.
Reports are in `../s11-foundation-evidence/atomic-task-retire/execution-check/alias-worktree-multidialog/`.
These single-run witnesses neither establish steady performance nor cover
the remaining WOW applications or the intermittent 1400.
