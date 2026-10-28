# DDEML module version and historical stress test family

These three remaining shard-2 entries match both original editions; the companion observer verifies identity, original source/test build relationships and archive-name snapshot. Earlier `guest-ddeml-test-build-review.md` covers the six-object stress-test build and separate client/server samples. This audit did not execute the stress script or guest; the script has an unbounded loop and starts multiple processes.

| Pending file | Evidence and restricted recommendation |
| --- | --- |
| `wow16/ddeml/tests/ddestrs/ddestrs.txt` | Documents DDESTRS test modes including asynchronous exchanges, multiple clients/servers and timers. Candidate **historical test documentation role**; no assertion those scenarios passed. |
| `wow16/ddeml/tests/ddestrs/runddes.bat` | Explicitly starts/invokes DDESTRS.EXE repeatedly and loops; the test makefile builds that standalone executable and links DDEML. Candidate **historical test runner role**; not a product execution dependency, nor a validated test harness for current product. |
| `wow16/ddeml/verddeml.h` | `ddeml.c` includes it, and original module makefile compiles ddeml.obj into DDEML.DLL; it defines three version words. Candidate **historical guest build declaration** in that original configuration; actual shipped module content, initialization and version query not proved. |

The captured ZIP contains no entry named DDESTRS.EXE or DDEML.DLL. This is a filename-only observation, not proof that another guest file, host implementation or installation cannot provide DDE services, or that DDE is broken. Preserve actual DDE message, memory, cross-task and lifecycle obligations separately from historical test/build-file classifications.

Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/shard-2/ddeml-test-version-observer.py` and compare JSON. No product, guest, shared ledger or build tree changed.
