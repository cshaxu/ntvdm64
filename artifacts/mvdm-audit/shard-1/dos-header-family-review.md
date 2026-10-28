# S10 shard 1: DOS guest C headers and assembler includes

Eleven pending `dos/v86/h` and `dos/v86/inc` files were checked together. Every file matches both original reference trees by full SHA-256. `dos-header-family-evidence.json` records each hash, every bounded textual filename reference with line text, and normalized current x86 worker Ninja VALID dependency objects; replay `PYTHONDONTWRITEBYTECODE=1 python -B artifacts/mvdm-audit/shard-1/dos-header-family-observer.py`. No exact file occurs in that worker dependency log. These are primarily historical 16-bit guest build inputs; host C dependency absence is not proof they never affected a shipped guest binary.

| File | Scope, current evidence and classification boundary |
| --- | --- |
| h/float.h | CRT floating-point constants; scan finds its own heading only. Consumer depends on historical compiler include path; pending external build/profile attribution. |
| h/graph.h | CRT graphics declarations; own heading only. No evidence actual guest graphics library is unused. |
| h/math.h | CRT math declarations; own heading only. Keep alternate build and actual math semantics unproved. |
| h/search.h | CRT search/sort declarations; own heading only. Pending source/toolchain resolution. |
| h/stdarg.h | CRT variadic macros; own heading only. Other same-named headers may resolve on compiler include paths. |
| h/time.h | CRT time declarations; own heading only. Not proof DOS timing capability is missing. |
| inc/chkcpu.inc | Historical CPU-check macro; no exact filename reference in bounded scan. Do not infer runtime CPU emulation absent. |
| inc/macro.def | Historical assembler macro definitions; no exact filename reference in bounded scan. Macro text may be used in other build contexts. |
| inc/mft.inc | Historical assembler definitions; no exact filename reference in bounded scan. Pending original consumer/profile. |
| inc/parse.asm | Shared DOS argument parser source; 32 textual hits including comments, original COMMAND, MEM, ANSI, NLSFUNC, APPEND and BIOS makefiles/assembly. Confirm actual path selection per build rather than counting all textual hits as active; no guest binary provenance or parser semantics established. |
| inc/smdossym.inc | DOS symbolic definitions; no exact filename reference in bounded scan. Pending alternate build/profile attribution. |

Search covers current DOS v86 C/H/INC/ASM/DEF/MAK and files named makefile; basename references may be comments, same-name alternative files, or disabled paths, and generated build inputs/other extensions are not exhaustively searched. Current worker deps include only VALID entries and do not describe an original MASM/CL16 build. No product, guest, build tree or shared ledger modified; no guest execution, parser behavior or lifecycle tested.
