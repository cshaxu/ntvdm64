# DEM search and FCB I/O: original owner and lease failure boundary

Both selected sources differ from OpenNT and remain partial. The independent
text-retention metric is 425/432 original code lines for `demfcb.c` (98.38%)
and 1509/1518 for `demsrch.c` (99.41%); it does not measure behavior. Original
FCB operations, DOS search ordering and per-PSP search lists remain the owner.
DIV-005 replaces retained DTA aliases with short guest leases; DIV-360/362
add search-buffer reclamation/restart behavior. These changes must be judged
separately, not treated as wholesale replacement of the original DEM.

The current `demFindFirst` acquires a writable DTA bounce buffer, executes
`demFileFindFirst` and then commits/releases the buffer. `SearchFile` can
create and attach an `FFindEntry` to a per-PSP list, and the successful DTA
receives its find ID and pointer. `guest_memory_lease_release` can return
failure if the session or epoch is invalid, or a guest write fails; the
wrapper reports `ERROR_INVALID_ADDRESS` after a failed commit. The shown
failure branch does not explicitly retire a just-created find entry.
`demTerminatePDB` later frees the PSP list. This source trace identifies a
conditional ownership question: whether a failed initial commit can leave an
unreachable search entry retained until PSP termination. It does not establish
that the failure is reachable during a normal active call, that an entry was
actually retained in a run, or that PSP cleanup fails. A first result without
a retained lookahead entry must not be mistaken for this condition.

`demFindNext` similarly advances the original search before guest DTA commit;
a commit failure returns an error after that advancement. FCB read executes
host `ReadFile` before committing a write lease, while FCB write uses a
read-only lease and retains original disk-full handling. Error and teardown
semantics must be compared to the original guest-visible contract, not fixed
by blindly rewinding file or directory state. Legacy synchronous pathname and
FCB search-buffer `GetVDMAddr` uses remain and need their own bounded-lifetime
audit; the DTA lease does not establish that every guest pointer in either
file is safe.

For the later DOS service group, inject commit failure after a successful
find-first with a known retained entry and count list/handle retirement;
separately test find-next advancement, FCB I/O failure and task termination,
including retry and guest error registers. Compare original behavior and
existing host adapter ownership before selecting any recovery. Existing
successful directory/FCB observations do not test this failure boundary.
No guest, runtime code or tests were changed by this source review.
