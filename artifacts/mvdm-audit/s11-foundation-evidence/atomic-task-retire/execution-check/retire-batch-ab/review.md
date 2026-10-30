# Retired-window scan lock batching experiment

This isolated candidate groups shared-lock acquisition over consecutive slots
without a pending window; it releases the lock before calling the original
terminal routine. It does not remove retirement or add a persistent cache.
The product source is unchanged. Its longer lock hold changes concurrency
interleavings, and correctness under concurrent create/destroy, task teardown,
reentry, reused handles and failed terminal processing remains unproven.

The first A/B/A comparison had three interactions per package, all passed.
Dialog open times were 3438/2594/2563, 1969/1625/1563, and 2437/1829/1797 ms.
Close times were 2891/2781/2719, 1891/1781/1546, and 1688/1937/2046 ms.
The candidate appeared faster in this ordered run. A reverse B/A/B run also
passed all nine interactions but did not reproduce a consistent benefit:
open 2297/2000/2032, 2734/1797/1735, 2344/1984/2031 ms; close
2172/2125/2265, 2250/1953/1953, 2140/2219/2062 ms. A fixture compilation
overlapped the reverse run, adding noise; it cannot be used as a clean
performance proof. Sealed staged manifests and complete probe logs are retained.

An isolated `window-reclaim` fixture attempt reached its own desktop check and
returned `WOW_PAGE_DOMAIN_DESKTOP_UNAVAILABLE error=5`; it is not a passing
candidate lifecycle test. Since the reverse timings do not confirm a gain and
the lock-hold change lacks complete lifetime/concurrency proof, the candidate
is not adopted. The scan may still merit controlled instrumentation; this
experiment neither proves it is nor excludes it as a bottleneck.

The same selected-object `window-reclaim` fixture was rerun with the authorized
desktop environment and exited zero. It exercised normal and exceptional
borrows, handle reuse and terminal retry, and recorded the page-domain lifecycle
witness. Its output and fixture hash are retained here. This focused result does
not prove every concurrent writer interleaving or establish a performance gain.

A further B/A/B run was performed after confirming no compiler, linker or Ninja
process was running. All nine interactions passed. Open times were
2375/2015/1891, 2547/2016/2016 and 2344/1969/1922 ms; close times were
2172/2110/2109, 2187/2188/2234 and 2312/2093/2219 ms. These differences are
small compared with the first run's apparent gain and within observed run-to-run
variation; the additional result does not establish a robust end-to-end gain.
Exact staged manifests and probe logs are in the `clean-reverse-*` directories.

Source review also shows that the publication lock is not the sole authority
for every entry in `wow_user_object_table`: window publication and menu-tree
publication update entries under their task/domain execution assumptions.
Longer shared-lock coverage alone does not prove those writers are excluded.
The candidate cannot be promoted based on the focused fixture; all writer,
reentry, handle-reuse and ownership assumptions need a complete proof first.
No product change was made.
