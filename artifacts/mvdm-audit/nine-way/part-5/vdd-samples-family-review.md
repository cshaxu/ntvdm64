# Part 5: VDD interface documentation and historical sample family

All 31 owned keys are enumerated below. Twenty-eight match both original editions; the three current-only sample binaries have no corresponding original byte identity. The observer records hashes and up to 20 literal reference leads per source (comments/conditionals and same-name ambiguity are not resolved) and checks exact ZIP byte matches for sample binaries. Historical separate samples are distinct from the project's actual VDD API, guest dispatch and hardware behavior.

| Owned key | Captured evidence | Bounded status |
| --- | --- | --- |
| `vdd/samples/adlibvdd/adlibvdd.def` | 0 literal references; example `no literal sample-source reference found` | Historical sample declaration/resource/definition; selected profile and runtime behavior remain open. |
| `vdd/samples/adlibvdd/vdd.c` | 6 literal references; example `vdd/samples/adlibvdd/sources:20` | Historical sample role; actual sample build, selection and behavior remain pending. |
| `vdd/samples/adlibvdd/vdd.h` | 4 literal references; example `vdd/samples/adlibvdd/vdd.c:27` | Historical sample declaration/resource/definition; selected profile and runtime behavior remain open. |
| `vdd/samples/hpscan/hpscan16.asm` | 1 literal references; example `vdd/samples/hpscan/hpscan16.inc:3` | Historical sample role; actual sample build, selection and behavior remain pending. |
| `vdd/samples/hpscan/hpscan16.exe` | 1 literal references; example `vdd/samples/hpscan/makefile:44` | Current-only sample binary; provenance and selected consumer pending; exact archive matches: []. |
| `vdd/samples/hpscan/hpscan16.inc` | 1 literal references; example `vdd/samples/hpscan/hpscan16.asm:9` | Historical sample declaration/resource/definition; selected profile and runtime behavior remain open. |
| `vdd/samples/hpscan/HPSCAN16.SYS` | 4 literal references; example `vdd/samples/hpscan/makefile:30` | Current-only sample binary; provenance and selected consumer pending; exact archive matches: []. |
| `vdd/samples/mscdex/readme.txt` | 0 literal references; example `no literal sample-source reference found` | Historical sample documentation; do not exclude the functionality described. |
| `vdd/samples/mscdex/tsr/messages.inc` | 5 literal references; example `vdd/samples/mscdex/tsr/makefile:41` | Historical sample declaration/resource/definition; selected profile and runtime behavior remain open. |
| `vdd/samples/mscdex/tsr/messages/usa/messages.inc` | 5 literal references; example `vdd/samples/mscdex/tsr/makefile:41` | Historical sample declaration/resource/definition; selected profile and runtime behavior remain open. |
| `vdd/samples/mscdex/tsr/mscdexnt.asm` | 1 literal references; example `vdd/samples/mscdex/tsr/makefile:46` | Historical sample role; actual sample build, selection and behavior remain pending. |
| `vdd/samples/mscdex/tsr/mscdexnt.exe` | 3 literal references; example `vdd/samples/mscdex/tsr/makefile:31` | Current-only sample binary; provenance and selected consumer pending; exact archive matches: ['WINNT/SYSTEM32/mscdexnt.exe']. |
| `vdd/samples/mscdex/tsr/mscdexnt.inc` | 2 literal references; example `vdd/samples/mscdex/tsr/makefile:46` | Historical sample declaration/resource/definition; selected profile and runtime behavior remain open. |
| `vdd/samples/mscdex/vdd/mscdex.h` | 1 literal references; example `vdd/samples/mscdex/vdd/vcdex.c:48` | Historical sample declaration/resource/definition; selected profile and runtime behavior remain open. |
| `vdd/samples/mscdex/vdd/vcdex.c` | 1 literal references; example `vdd/samples/mscdex/vdd/sources:19` | Historical sample role; actual sample build, selection and behavior remain pending. |
| `vdd/samples/mscdex/vdd/vcdex.def` | 0 literal references; example `no literal sample-source reference found` | Historical sample declaration/resource/definition; selected profile and runtime behavior remain open. |
| `vdd/samples/mscdex/vdd/vcdex.h` | 1 literal references; example `vdd/samples/mscdex/vdd/vcdex.c:52` | Historical sample declaration/resource/definition; selected profile and runtime behavior remain open. |
| `vdd/samples/mscdex/vdd/vcdex.rc` | 1 literal references; example `vdd/samples/mscdex/vdd/sources:19` | Historical sample declaration/resource/definition; selected profile and runtime behavior remain open. |
| `vdd/samples/readme` | 0 literal references; example `no literal sample-source reference found` | Historical sample documentation; do not exclude the functionality described. |
| `vdd/samples/sample1/fax16/fax16.asm` | 0 literal references; example `no literal sample-source reference found` | Historical sample role; actual sample build, selection and behavior remain pending. |
| `vdd/samples/sample1/fax32/fax32.c` | 1 literal references; example `vdd/samples/sample1/fax32/sources:15` | Historical sample role; actual sample build, selection and behavior remain pending. |
| `vdd/samples/sample1/fax32/fax32.def` | 0 literal references; example `no literal sample-source reference found` | Historical sample declaration/resource/definition; selected profile and runtime behavior remain open. |
| `vdd/samples/sample1/fax32/fax32.h` | 1 literal references; example `vdd/samples/sample1/fax32/fax32.c:10` | Historical sample declaration/resource/definition; selected profile and runtime behavior remain open. |
| `vdd/samples/sample2/16bits/16bits.asm` | 0 literal references; example `no literal sample-source reference found` | Historical sample role; actual sample build, selection and behavior remain pending. |
| `vdd/samples/sample2/16bits/16bits.inc` | 2 literal references; example `vdd/samples/sample2/16bits/16bits.asm:4` | Historical sample declaration/resource/definition; selected profile and runtime behavior remain open. |
| `vdd/samples/sample2/vdd/vdd.c` | 5 literal references; example `vdd/samples/adlibvdd/sources:20` | Historical sample role; actual sample build, selection and behavior remain pending. |
| `vdd/samples/sample2/vdd/vdd.def` | 0 literal references; example `no literal sample-source reference found` | Historical sample declaration/resource/definition; selected profile and runtime behavior remain open. |
| `vdd/samples/sample2/vdd/vdd.h` | 4 literal references; example `vdd/samples/adlibvdd/vdd.c:27` | Historical sample declaration/resource/definition; selected profile and runtime behavior remain open. |
| `vdd/samples/vsbd/vdd.c` | 5 literal references; example `vdd/samples/adlibvdd/sources:20` | Historical sample role; actual sample build, selection and behavior remain pending. |
| `vdd/samples/vsbd/vsbd.def` | 0 literal references; example `no literal sample-source reference found` | Historical sample declaration/resource/definition; selected profile and runtime behavior remain open. |
| `vdd/vddserv.doc` | 0 literal references; example `no literal sample-source reference found` | Historical VDD service documentation; actual product VDD callback/ownership semantics remain separate. |

The ZIP byte scan applies to these named sample binaries only; no equivalent producer, device behavior, external application, or different configuration is ruled out. Literal references must be checked in context before marking any source selected. No guest/build/product/shared-ledger changes or test execution. Replay `PYTHONDONTWRITEBYTECODE=1 python artifacts/mvdm-audit/nine-way/part-5/vdd-samples-family-observer.py` and compare JSON.
