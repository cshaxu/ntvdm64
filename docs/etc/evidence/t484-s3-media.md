# T484 S3 P66 media-owner comparison producer

B0075 now delegates current/upstream DEM body and VOLINFO extraction and
compilation to CMD/native targets. The same fixture, /MT /W4 profile and
mocked volume-query boundary remain. The runner retains the current-success
marker and original-failure-at-line42 assertion, source hashes and owned
process deadlines. Its obsolete generated compiler script is removed.

The upstream input is the same read-only O:/repos.external/OpenNT/base/mvdm
tree recorded by T483 R0098, with demgset.c SHA256
19C5999308A479CC9C0FCB30D23E8301D9A835408417AA965B85523DFB8A1653.
This is an opt-in test input, not a product dependency or imported source.
The CMD entry requires both source files; ordinary fixture requests clear the
optional source root, preventing persistence as an implicit external dependency.
No original guest is rebuilt or changed.

Actual current-owner tests pass; the original reproduces the expected label
overwrite failure at line42. Independent projection comparison first detected
the native generator's final LF. Both body texts matched all original content
(1399 current /1347 upstream characters) plus exactly that LF. The follow-up
comparison allows only CRLF-to-LF normalization and one final LF in the body
and type files; it does not normalize tokens or arbitrary whitespace.
The failed comparison receipt is retained, not described as a passing run.

Raw evidence is under build/t484-s3-media; concise receipts are under
artifacts/build-migration/t484/s3-media. These are bounded owner tests, not
a fresh DOS integration run. Cached host build, publication identity, cache
and governance verification complete the delivery.

Final checks passed: cached full host build, stable media executable hashes
and timestamps on no-op rebuild, rejection of missing original input and x64,
and clearing the external dependency on an ordinary fixture request. All11
host input hashes and22 published hashes are unchanged; no recopy is needed.
Governance passes. Inventory:355 entries,138 completed dispositions and217
remaining (54 pending producer chains,163 awaiting reconciliation).
