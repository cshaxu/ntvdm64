# S12: SCCSID and one MONITOR/PROD branch

The two observed MVDM product command graphs do not select `SCCSID`, and
the project source contains no definition of it. Twenty-nine original
identification-string branches are replaced by registered markers; the
original strings and hashes remain in the companion JSON. One header branch
requiring `MONITOR` and not `PROD` is unselected across its recorded
consumers. Enabled MONITOR paths are retained. Header consumers whose context
inventories did not agree were excluded from this selection.

Independent reconstruction verified all 30 removed bodies and whole files
against baseline `ce133d8e7`. Cached x86/x64 product builds and documentation
governance passed. All ten resulting components matched the previously
published images exactly or differed only by four PE linker timestamp bytes;
runtime tests and republishing were therefore not repeated. These results
apply to the observed product graphs, not to unobserved profiles.
