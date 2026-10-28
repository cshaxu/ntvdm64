# DOS locale and message-input functional family

This extends `dos-message-input-review.md` for all four shard-2 pending inputs in the locale/message family; byte identities and original LOADFIX/BUILDMSG edges for all four are replayable with `dos-message-input-observer.py`.

| File | Evidence and bounded disposition |
| --- | --- |
| `germany.msg` | Both original sources match. Locale data remains **pending** if the ledger requires actual supported German runtime semantics; it may be described as historical parameterized build input without excluding locale functionality. |
| `holland.msg` | Both original sources match. Same bounded conclusion for Dutch; do not conflate it with German or U.S. strings. |
| `loadmsg.msg` | Original LOADFIX assembler and build recipe reference its message labels and input. Suggest **excluded only as historical build input**; LOADFIX error and message behavior remain independently in scope. |
| `usa-ms.idx` | Original BUILDMSG opens a country index and the messages recipe generates an index from message source. Suggest **excluded only as generated build input**, without claiming this retained index was actually produced by a particular build or that locale handling was tested. |

Original `messages/makefile` chooses `COUNTRY=usa-ms` only if COUNTRY is undefined and builds `$(COUNTRY).idx` from `$(COUNTRY).msg`. DOS `make.inc` has the same default but changes `msg` to `..\..\messages\$(COUNTRY)` when COUNTRY is explicitly defined. COMMAND passes `$(msg)\$(COUNTRY)` and `command.skl` to BUILDMSG; DOS kernel message prerequisites use `$(msg)\$(COUNTRY).msg`. Thus explicit locale changes also affect directory/search layout. The recipe does not establish alternate directories exist or are complete.

Captured package `COMMAND.COM` and `NTDOS.SYS` match installed System32 copies; neither identity demonstrates the locale inputs or build settings that produced them. No localized guest behavior, error messages, encoding or source-to-image provenance was executed or proven. No product, guest, build tree or shared ledger changed. Reproduce the first two rows and image identities with `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/shard-2/dos-locale-source-chain-observer.py`; compare its JSON. Reproduce all four original identities and prior build edges with `dos-message-input-observer.py` and its receipt.
