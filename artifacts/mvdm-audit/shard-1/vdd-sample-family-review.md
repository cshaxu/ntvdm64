# S10 shard 1: historical VDD sample family

Eight pending files from five original sample components were checked together. `vdd-sample-family-evidence.json` contains exact source/reference hashes, current four-host-graph hashes and normalized VALID dependency matches, and ZIP/system32 basename observations; replay `PYTHONDONTWRITEBYTECODE=1 python -B artifacts/mvdm-audit/shard-1/vdd-sample-family-observer.py`. All eight have zero exact VALID deps in those four current graphs; none has a same-basename ZIP member or system32 file. The basename checks for generic `vdd.c` are not a comprehensive census of VDD binaries and say nothing about external deployment.

| Pending path | Source identity, role, and boundary |
| --- | --- |
| adlibvdd/vdd.c | Matches both primary original trees; sample Ad Lib VDD talks to an external synth device. Prior `vdd-audio-samples-review.md` covers sample linkage and limits. Not proof all Ad Lib or audio support is absent. |
| hpscan/HPSCAN16.SYS | Absent from both primary reference paths but matches third `opennt-src-2` tree. Original hpscan makefile builds a DOS driver via assembler/linker/exe2bin; hpscan16.asm describes passing HPSCAN requests to HPSCAN32.DLL. Binary equality does not prove source build lineage or scanner/device availability. |
| mscdex/tsr/messages.inc | Matches both original trees; `mscdexnt.asm:233` includes it, and TSR makefile copies the COUNTRY-selected message file before building MSCDEXNT.EXE. Current retained file equals `messages/usa/messages.inc`; no runtime MSCDEX or locale test. |
| sample1/fax32/fax32.h | Matches both references; fax32.c includes it and declares VDD hooks and lifecycle. Sample source calls VDDInstallUserHook, but no sample fax device or callback behavior was verified. This is distinct from product WOW32 fax. |
| sample2/16bits/16bits.asm | Matches both references; sample DOS companion performs VDD demonstration requests. See prior `vdd-sample2-owner-review.md`; no sample run. |
| sample2/16bits/16bits.inc | Matches both references; same sample companion declarations and build linkage. Not proof of production DMA/memory behavior. |
| sample2/vdd/vdd.def | Matches both references; exports VDDInitialize for sample host DLL. Definition alone is not a loaded DLL or runtime binding. |
| vsbd/vdd.c | Matches both references; separate historical sound-blaster sample VDD, not the Ad Lib file. Prior audio-samples report gives bounded selection evidence; no device run. |

The original recipes refer to guest compilers and optional device providers; current host VALID dependency absence does not exclude alternate profiles, runtime-loaded external VDDs or corresponding production functionality. No guest, sample, provider, product, shared ledger or build tree was modified or executed.
