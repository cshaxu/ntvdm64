# S13 per-file checkpoint r1268

The eight remaining files in the current per-file inventory were reviewed.
Seven changed: `ntexapi.h`, `ntregapi.h`, `ntrtl.h`, `usercli.h`,
`ntcftxt.h`, `user.h` and `userk.h`. `ntstubs.c` required no edit: all 73
original groups match by physical source anchor and parent/arm identity.
The original-selected text remains unchanged apart from restoring meaningful
conditions, separating the original UTCHAR and CHARSTOBYTES guards, unifying
the original userk MSDWP frame, and corrected diagnostic anchors. Unimported
original API bodies remain absent. Individual source and guard evidence is
recorded in `s13-r1260` through `s13-r1268` in this directory. No guest
source or binary was modified or rebuilt.

The original `_USERCLI_` and `_USER_` single whole-file guards cannot be
installed unchanged over the admitted cohort-based projections: the negative
controls prove doing so suppresses necessary later inclusion. Their inner
condition frames are retained as projected controls; these two parent-structure
exceptions are registered as OPENNT-HOST-251 and OPENNT-HOST-252. This is not
a claim of exact original parent structure or restored omitted functions.

The final cached x86, x86-extra and x64 product builds
[passed](s13-r1268-full-build-results.json). The
[ten-image comparison](s13-r1268-build-image-compare.json) found one image
identical and nine with only parsed PE/debug timestamp changes; there are no
other differing bytes. The [r1259 Full suite](s13-r1259-full-evidence.json),
including 17/17 Console and 17/17 Window, is therefore reused under the
user-approved rule. Affected x86/x64 preprocessing, rejection, diagnostic-line
and USER multi-cohort tests [passed](s13-r1268-guard-evidence.json). An initial
x64 test used an x64 context for the x86-only NTEXAPI carrier; the test was
corrected to exercise the actual x86 profile and both architectures passed.
All ten candidate images were published and hash-verified at `assets/release`
and `O:/winnt/system32`.

The [census](s13-r1268-counts.json) records 1,056 existing mirror C/H files:
182 reviewed byte-identical to their original candidates, 874 reviewed with
differences, and zero pending in this inventory. Rename-aware net 580 deleted
C/H paths against `c97d4ee2c` are separate, not counted as identical. One
existing inventory file is outside observed build graphs and is a reviewed
product-generator input. Historical single-build observations do not by
themselves prove complete enumeration of every possible supported inclusion
context; the two USER parent projections and omitted declarations remain
explicit limits, not claims of complete original-code equivalence.
