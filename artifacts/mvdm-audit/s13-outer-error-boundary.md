# Excluded conditional nesting cleanup

The owner superseded the previous full-nesting rule: an excluded original
conditional body retains its outer boundary and `#error`; conditional shells
wholly inside that body are unnecessary. The rule is recorded in coding and
source-policy authorities. Selected alternatives remain intact.

The host mirror sweep covered tracked C, C++, header and include files under
`src/mvdm-host` and `src/non-mvdm`. It removed 2,427 physical lines of nested
excluded content in 68 files. Where an existing `#line` immediately follows
the enclosing `#endif`, redundant blank lines were removed; blank lines at
`#else` and `#elif` boundaries remain to preserve other selected alternatives.
Remaining blank lines preserve conditional `#line` and `__LINE__` behavior without
assuming which historical line directives a compilation selects. A repeated
scan using the same conservative matcher, and a broader scan allowing code
before the `#error`, found no further matching nested branches. This is a
scan result, not proof that every possible preprocessor form has been
classified. Original guest sources and images were not changed or rebuilt.
The [file manifest](s13-outer-error-boundary-manifest.json) records before and
after SHA-256 values and removed-line counts; hashes describe the actual
working-tree inputs, including pre-existing candidate changes.

The cached x86 and x64 configurations and the worker, WOW32, x86 auxiliary
and x64 native component builds completed successfully. Documentation
governance, relative links and `git diff --check` passed. The rebuilt NTVDM
`.text` and `.data` sections match the earlier direct-access candidate, but
its `.rdata` and `.rsrc` sections differ; whole-image identity and runtime
equivalence are not established by that comparison.

After the CPU30 vector body was replaced by `#error`, a newly linked package
was staged and passed its own 16-gate Full suite and four Win3.1 combinations.
The external DOS4GW/NESticle fixture was not supplied and its optional case
was skipped. The complete ten-image package was published and verified at
both release destinations.

This source cleanup overlaps an independently unfinished CPU40 direct-access
candidate. That candidate's CCPU bit-operation difference is unresolved;
successful builds or this structural cleanup do not establish its runtime
equivalence or qualify it for release. Build and test outcomes must be recorded
separately before delivery.
