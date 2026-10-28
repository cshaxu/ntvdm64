# Part 8: WOWEXEC guest shell versus test directory name

Seven owned `wow16/test/hello` and `wow16/test/shell` files have individual identities in `guest-family-inventory-observations.json`. Despite its directory named `test/shell`, original `wowexec.c` describes a 16-bit server task receiving launches for 32-bit CreateProcess, and its original `makefile:80-120` builds WOWEXEC.EXE from wowexec/pmdos/wowexfax objects, a definition with KERNEL imports and the resource. Directory name alone cannot exclude the live launch function or prove the exact prebuilt binary was produced from this recipe.

Original selected source chain per `m0-t463-s3-lifecycle-owner-acceptance.md` is BaseSrvCheckWOW → WM_WOWEXECSTARTAPP → registered WOWEXEC window → original StartRequestedApp/LoadModule. `wowexec.c:767-773,1007,1205` contains its shared-worker drain/wait and launch receiver; task state and dispatch remain in original WOW/worker, with NTSRV owning the separately defined service lifecycle. Do not substitute HELLO, a new launcher or a test-only scheduler for it.

| Owned path | Role and bounded recommendation |
| --- | --- |
| `wow16/test/hello/hello.c` | Historical small Win16 window/shell example, not WOWEXEC. Preserve test source; no shipped binary or actual test inferred. |
| `wow16/test/hello/hello.h` | HELLO example declarations; do not conflate with WOWEXEC. |
| `wow16/test/shell/pmdos.asm` | Original low-level DOS helper object in WOWEXEC historical recipe; real call/return coverage is per operation. |
| `wow16/test/shell/wowexec.c` | Original WOW server task/command receiver; source has actual production relevance. Preserve behavior; binary source provenance and all lifecycle corners remain separate. |
| `wow16/test/shell/wowexec.ico` | Original resource asset referenced by WOWEXEC RC; packaged binary resource correspondence unproved. |
| `wow16/test/shell/WOWEXEC.RES` | Original historical resource artifact; exact selected build/output identity not established. |
| `wow16/test/shell/wowexfax.c` | Original WOWEXEC WinFax support object in historical recipe; optional fax functionality/provider and lifecycle require independent proof. |

T463 S3 documents two WRITE rounds in one registered shared WOW worker and two overlapping tasks for its `product-r3` package; it explicitly does not claim all message identity or provider behavior. T428 S2 separately describes original shared/exclusive distinction for its own historical candidate. These evidence records do not prove the current published package or exact source-to-binary provenance. No test was rerun here.

S14 should witness unchanged WOWEXEC registration, repeated command drain, task completion/failure and shared worker reuse, plus separate-WOW last-task behavior; S11/S4-equivalent execution work owns cross-task identity and nested callbacks, and S19 pins guest/release hashes. Optional Fax is separate from basic shell. Do not mark the WOWEXEC source or capability irrelevant because of the parent directory.

Replay: `rg -n "StartRequestedApp|WowWaitForMsgAndEvent|LoadModule" src/mvdm/wow16/test/shell/wowexec.c`, inspect its makefile and cited T463/T428 records; rerun part-8 inventory observer with bytecode disabled. No guest, product or shared ledger changes.
