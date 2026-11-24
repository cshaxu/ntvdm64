# T484 S3 P107 native module alias contract

B0176 retains the actual wole2 alias contract. The fixture covers full,
truncated, single-byte and unavailable name output,32-slot oldest-creation
reuse, and explicit retirement. Thread signaling alone intentionally does
not retire the alias. The fixture and product source are unchanged.
Its independent purpose and historical result are recorded in the
[alias/WinOldAp audit](m0-t452-s3-alias-winoldap-audit.md).

The old runner parsed a Ninja compile command, substituted source/object
paths and manually recompiled the owner and fixture. Remove that producer
and its separate toolchain/provider parameters. The CMD/native target links
the existing product original_wole2 object; only the fixture is compiled,
inheriting that owner's actual flags. No extracted algorithm or new mocks.

Owned supervisor PID14892 exits0. The real fixture reports
WOW_MODULE_ALIAS_BOUNDS_AND_LIFETIME_OK. Compiler-command audit verifies
both source profiles use CCPU40/CCPU/MT/O2/Gz; the map attributes all five
alias API definitions to wole2.c.obj. This is current native owner coverage,
not guest/runtime acceptance or proof of all possible OS thread-ID reuse.

Full cached build passes. A second native target build preserves executable
hash and modification time; x64 selection is rejected with exit2.
Eleven raw host inputs and22 published copies match the current manifest.
Existing publication verified, not republished; no product, mirror, guest
or asset change.

[Receipt](../../../artifacts/build-migration/t484/s3-module-alias-native/result.json)
includes process identity, provider commands and cache hash.
Raw evidence: build/t484-s3-module-alias-native.
Inventory206 complete/149 remaining of355. S3 and migrated-entry retention
review continue; B0166 Idle and B0169 DLL-provider reconciliation remain
open. S4/S5 final publication/clean acceptance are not complete.
