# Part 2: DOS common parser and MEM build input distinction

The part-2 owned `dos/v86/inc/_parse.asm` and `dos/v86/inc/parse.asm` both match the two frozen originals per `dos-shared-inputs-observations.json`. They are different from the equally named command-local `dos/v86/cmd/mem/_parse.asm`; filename-only matching must not choose the shared wrapper as MEM's source.

The original MEM makefile at `cmd/mem/makefile:34-38,79-81` explicitly requires local `_parse.asm`, common `$(inc)/parse.asm`, `psdata.inc`, then links `_parse.obj`. The local wrapper at `cmd/mem/_parse.asm:64-89` includes common parse.asm and calls SysParse. The common parser `inc/parse.asm:38-41,230-255` uses shared psdata.inc and declares conditional near/far SysParse entry. The separate `inc/_parse.asm:1-52` is itself a generic C-to-DOS parser wrapper with its own switches and `include parse.asm`; the captured MEM recipe alone does not choose it. Neither assembler input is a host runtime provider.

| Owned path | Bounded classification |
| --- | --- |
| `dos/v86/inc/_parse.asm` | Original generic C/parser adapter; preserve as possible guest source for other commands. MEM's local same-name adapter is the actual shown recipe source. Alternate consumers and profile conditions remain pending. |
| `dos/v86/inc/parse.asm` | Original configurable shared SysParse implementation, an explicit historical MEM source dependency and potentially shared elsewhere. Preserve; switch combinations and final binary provenance are unverified. |

The guest parser owns argument decoding and its original register/flags contract; it is not an excuse to special-case MEM in Run16 or host command dispatch. Historical `m0-t406-s1-physical-mapping-design.md:428-438` records one hash-identified MEM printing totals and exiting 0, observer-active only; that is not a complete argument parsing or numeric-accuracy test. Other old evidence includes both successes and timeouts under different products, and no current execution was performed in this review.

S12 should test immutable, hash-identified MEM with default, valid switches, invalid/quoted input and normal exit, correlating actual parser result and output. Memory sizing follows its own guest/host source owners; do not assume successful parsing proves totals are correct. S19 should rebind current release/guest identities. Keep source-level alternate callers pending; do not rebuild or patch guest.

Replay `rg -n "_parse|parse.asm|SysParse|psdata" src/mvdm/dos/v86/cmd/mem src/mvdm/dos/v86/inc/parse.asm src/mvdm/dos/v86/inc/_parse.asm`, read the actual makefile and part-2 inventory observer. No product, guest or shared ledger changes.
