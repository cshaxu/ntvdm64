# DOS v86 header source-role census

The 30 provisional `role-pending` C headers under `dos/v86/h` were checked
individually against the selected x86 worker and WOW32 Ninja dependency logs.
Each current file matches its recorded OpenNT original hash, and none of the
30 exact paths appears in those two recorded dependency sets. The per-file
paths, hashes, previous provisional labels and build-graph identities are in
[the census](dos-v86-header-dependency-census.json). Basename matches such as
`stdio.h` are deliberately not treated as proof that the old header was used.

The source text identifies 27 as old C compiler/library declaration headers,
including the DOS BIOS, console and graphics interfaces. The other three are
`copyrigh.h`, `version.h` and `versionc.h`, containing guest DOS copyright or
version data. The original `dos/v86/make.inc` selects `cl16` and `..\..\h`;
the original MEM makefile names `version.h` and `copyrigh.h`, and `memex.c`
includes `versionc.h`. These observations support a **guest source/build-input
role**, with host-runtime exclusion as the recommendation for each listed
header in the inspected selected builds. They do not prove that the associated
DOS, BIOS, console, graphics, or version behavior is unnecessary: S12 and S13
retain those runtime contracts and their actual guest tests.

The selected dependency databases describe the builds recorded there, not all
possible configurations or every unbuilt target. The evidence therefore does
not authorize changing the frozen mixed-scope classification ledger or making
a full behavioral-coverage claim. Further configuration and shared-ABI checks
remain necessary before converting the provisional source-role recommendation
into a final per-file disposition. No guest image or mirrored source changed.
