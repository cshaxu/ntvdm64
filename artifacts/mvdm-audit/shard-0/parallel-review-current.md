# S10 shard 0 — DDEML test programs

This read-only pass covers exactly 15 pending physical file roles in shard 0; shared ledger is unchanged. Both original editions, current source and frozen hashes agree; each path has one DOS and two WOW source-catalog rows. The [observer](ddeml-test-identity-observer.py) and [receipt](ddeml-test-identity-observations.json) preserve this identity and the original recipe evidence. The historical ddestrs and client recipes select WIN32 via sources and WIN16 via makefile.dos; ddemo and server have Win16 recipes. These are historical test inputs, not proof of DDEML or user-program runtime behavior. The client WIN32 sources references missing client.rc; actual ddemlcl.rc is in the Win16 recipe. Do not claim that the WIN32 recipe is independently buildable.

| Path | SHA-256 | Proposed file role / boundary |
| --- | --- | --- |
| `wow16/ddeml/tests/ddestrs/cmdln.c` | `6025A70936CA068DC29AF3FA582B3C43AC80EB1B054C73528E9E710F2D508C2B` | excluded candidate (historical test input only): stress command-line object. |
| `wow16/ddeml/tests/ddestrs/ddesorg.ico` | `52399553691C38770FBD97C0FD37136E7D43F2AF91088C08B3C4350CF5F88F7A` | pending: resource has no established consumer; keep pending. |
| `wow16/ddeml/tests/ddestrs/ddestrs.h` | `9E827322A7B8D14F9FCE9E098FADB2688EC8DD199A1626B2336B951FD1B0B982` | excluded candidate (historical test input only): stress declarations used by source and DOS recipe. |
| `wow16/ddeml/tests/ddestrs/globals.c` | `998857E0CC117E58B192B821526CBFFD713C66EEEB36F147E4FAA8A88C9796A8` | excluded candidate (historical test input only): stress global state object. |
| `wow16/ddeml/tests/ddestrs/server.c` | `AE9A7D42428F1E4F75F7A697BF4E8B626F0387DDF1C2C0909209D8C8786C5B77` | excluded candidate (historical test input only): stress server object. |
| `wow16/ddeml/tests/ddestrs/wrapper.h` | `0DAB959CD3BC0EDC45305A4C9DCFC8B2BEE7C12C40B921D3A66DBC044CF9D551` | excluded candidate (historical test input only): stress wrapper declarations. |
| `wow16/ddeml/tests/src/client/dde.c` | `5A43CF63D514AE151F2254E8B17C1DE13F98F979754497B8436DB2DAFED58BD2` | excluded candidate (historical test input only): client DDE object. |
| `wow16/ddeml/tests/src/client/dialog.h` | `D0B07020054BE6C690AB96AA8000060ACD2E713345E12DFECDB74EEBCCF66151` | excluded candidate (historical test input only): client resource/UI declarations. |
| `wow16/ddeml/tests/src/client/infoctrl.c` | `88F0A18F99AC221D8D6E59E8DC18CB3FB79852F98FABFD826DE72A82BD4B7BE5` | excluded candidate (historical test input only): client info control object. |
| `wow16/ddeml/tests/src/client/infoctrl.h` | `36324599A934FD0ADA67D72E0187F590C4F959CA8559F1587BA9E605B3E1FF47` | excluded candidate (historical test input only): client info control declarations. |
| `wow16/ddeml/tests/src/client/mk4dos.cmd` | `C85116E2ECE31AFCDE0D08E1B5D5432725FFC9112736DD44C18B228106A37B13` | excluded candidate (historical test input only): historical nmake wrapper for this test; deletion branch was not executed. |
| `wow16/ddeml/tests/src/ddemo/ddemo.def` | `B5AA923F34DD0712464644AE03A65E82A8FF4A6547B75E23E5E2C633232DE6B2` | excluded candidate (historical test input only): Win16 DDEMO linker definition. |
| `wow16/ddeml/tests/src/ddemo/mk4dos.cmd` | `C85116E2ECE31AFCDE0D08E1B5D5432725FFC9112736DD44C18B228106A37B13` | excluded candidate (historical test input only): historical nmake wrapper for this test; deletion branch was not executed. |
| `wow16/ddeml/tests/src/server/mk4dos.cmd` | `C85116E2ECE31AFCDE0D08E1B5D5432725FFC9112736DD44C18B228106A37B13` | excluded candidate (historical test input only): historical nmake wrapper for this test; deletion branch was not executed. |
| `wow16/ddeml/tests/src/server/server.h` | `5A4E4B5B52498974326E5F91FDDEC18CB210BAE7B62F4EF7E16978765B0253AA` | excluded candidate (historical test input only): server UI/DDE declarations. |

All test-input exclusions require independent coordinator approval before ledger merge. Retain real DDE/DDEML compatibility, callback/lifecycle semantics and regression obligations. Source-byte retention is 100%; executable behavior coverage and provenance of shipped guest binaries are not established. Neither guest nor product images were modified.
