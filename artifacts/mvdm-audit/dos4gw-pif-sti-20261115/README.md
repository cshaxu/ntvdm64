# P103 DOS4GW regression evidence

The side-session bisect, debugger logs and `candidate-dpmiint.c` are frozen
research inputs, not build or release sources. In particular, the candidate's
original trailing whitespace is retained rather than rewriting the measured
source. The staged whitespace check excludes only that frozen C snapshot;
production source, tests and documentation pass the normal check.

[Production repair and verification](main-production-candidate.json) records
the final four-file adaptation, original-relative difference counts, source
hashes, selected preprocessing, Full gates, Win3.1 profiles and publication.
[Baseline](main-baseline-contract.json) distinguishes the observed failures
from the [intermediate completion candidate](main-completion-diff.json).
Neither intermediate candidate is a final acceptance result.

Published-package images: [menu](production-nesticle-before.png),
[Escape hidden](production-nesticle-hidden.png),
[Escape restored](production-nesticle-restored.png).
No guest executables or ROMs are distributed here.
