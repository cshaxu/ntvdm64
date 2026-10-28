# Shard 3 excluded C/ASM source: current independent recheck

At revision `39f7baff2412b11db792445c8675510b1793e02e` the working `shard-3/file-review.csv` SHA-256 was `1c7d677d1329532b95b9ac6a1eb1bf9839e216de6119f49c2a738bf598fb90c1`. Filtering `review_class=excluded` for `.c`/`.asm` yields exactly 14 physical roles. All six present sources had live SHA-256 equal to their ledger value and at least one original-edition value. Seven CPU30 paths were absent from the current mirror. This is a bounded per-file role audit, not a general compatibility test.

| Physical source | Independent check and scope |
| --- | --- |
| `oemuni/toemuni.c` | Original `oemuni/sources` sets `UMTEST=toemuni`, while its library `SOURCES` are `file.c` and `process.c`. The test-program role may remain excluded; selected OEM conversion remains subject to its own behavior and failure tests. |
| `softpc.new/base/ccpu386/nop.c` | Actual original file defines an empty `NOP()`; selected original `c_main.c:2809-2810` handles opcode 90 inline. Empty helper exclusion does not prove CPU execution/retirement. |
| `softpc.new/base/dasm386/build.c` | Original `dasm386/sources` includes `build.c` in PPC/ALPHA/MIPS sources, and has empty `i386_SOURCES`. The non-i386 tool role is excluded; disassembly/debugger behavior is not thereby accepted. |
| `softpc.new/dat2obj/dat2obj.c` | Original `dat2obj/sources` defines a standalone `PROGRAM` generator, not a selected runtime device. Generated output identity and consumers need independent evidence. |
| `softpc.new/host/src/nt_mess.c` | Its two `hs_err_message` strings exactly appear at `obj.vdm/resource.rc:111-112`; `host_error` in `nt_error.c` calls `host_nls_get_msg`. Original `host/src/sources` selects `nt_error.c` and `nt_nls.c`, not this table. Error-path presentation and other locales remain independently unverified. |
| `v86/monitor/i386/{int,monitor,sas,thread}.c` | These four absent mirror paths belong to CPU30 monitor, which `source-policy.md` explicitly excludes as a runtime/build route. This does not prove CPU40 functionality. |
| `v86/scaffold/i386/{fakeinit,fakevid,softpc}.c` | These three absent mirror paths are the corresponding retired CPU30 scaffold, governed by the same prohibition. |
| `wow32/wthman.c` | The retained original source consists of includes and `MODNAME`, without thunk function definitions. Other ToolHelp entries and original unimplemented class operations remain separate. |

No item in this bounded set warrants a pending change on the evidence above. This conclusion requires the ledger to retain these **physical source-role** qualifiers; interpreting `excluded` as excluding the corresponding product feature would be erroneous. No changes to product, guest or shared ledger were made. No runtime test is claimed.
