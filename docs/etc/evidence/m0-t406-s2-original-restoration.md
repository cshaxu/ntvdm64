# M0 T406 S2 grouped original restoration

## Approval and boundaries

Owner: “批准修复为opennt原始实现  并测试通过”; then
“你把可以恢复的原始代码分组，每次恢复一组便跑一次编译测试 不要划分太细 要有效率”.
One agent implements and separately reviews. S1 evidence is the prerequisite;
this record does not convert its unresolved lower mapping bodies into originals.

## P1 width cohort

Baseline a03a6b4e3. Original comparison is
O:\repos.external\OpenNT\base\mvdm\, repository revision
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c. Only existing paired expressions are
restored, not whole mixed-behavior translation units.

Four-rung disposition: original expressions directly compose in Win32/x86,
so they are restored in mvdm-host. No new facade, overlay or autonomous
implementation is needed for this cohort. Necessary existing callback ABI,
CCPU descriptor/IDT/TEB adaptations and observations are unchanged.

| Cohort | Original-relative files | Restored contract |
| --- | --- | --- |
| DPMI host addresses | dpmi32/{data.c,buffer.c,dpmi32.c,dpmi32p.h,dpmidata.h,dpmiint.c,dpmiselr.c,int21map.c,xlathlp.h}, inc/softpc.h | ULONG IntelBase/FlatAddress, descriptor mapping values, buffer/DTA/IVT and 16:16 conversions, paired declarations and consumers. |
| COM private ioctl | softpc.new/base/{inc/host_com.h,comms/com.c}, softpc.new/host/src/nt_com.c | Original LONG/long third argument, including synchronous output pointer. |
| Direct-access warning TLS | softpc.new/host/src/nt_error.c | Original DWORD category mask and original shift/TLS expressions; WINAPI thread and public callback fixes retained. |

Source diff against the prior product is +33/-75 across 14 C/header files:
42 fewer physical source lines including comments, not 42 deleted algorithms.
Three obsolete README entries are removed separately. DIV-140 was duplicated
in that register: the unrelated host_getenv declaration entry is retained.
Remaining x64-pattern families are not silently declared cleared by this P.

Review found no LPT width counterpart: nt_lpt.c differs from OpenNT only by
one space in the checked expression; host_lpt.h, printer.c and printer_.c
have no textual diff. Overlay width-pattern screening found only original-
supported IHP carriers, not a reason to delete C-VID binding behavior.

## Group verification

Build root build/M0-T406/S2/r001-original-restoration was newly generated with
New-T310OriginalSoftpcNinja.ps1, Architecture x86, Node 22.22.1. The formal
original-softpc-process.exe target completed all 437 steps; build.log retains
compiler/linker output. Existing warnings remain; this is not a warning-free
build claim. The integration observer compiled with /TC /MT /W4 /WX.

The original-width declarations and definitions compile/link in the selected
CCPU40 graph. Source review compares restored expressions to the paired
OpenNT source; it does not infer source recovery from tests alone.

| Workload | PID | Observed result |
| --- | --- | --- |
| MEM | 40560 | Exit 0; prior anomalous largest-program-size value persists, not repaired or accepted as correct. |
| COMMAND | 44296 | Banner/prompt, injected exit, exit 0. |
| EDIT | 30200 | Welcome and editor after paired Esc; bounded Job timeout/cleanup, not normal exit or mouse acceptance. |
| WRITE | 41784 | DPMI 53:0f then WOW ingress, W32Init FALSE, exit 255; same frontier as baseline, not WRITE success. |
| Isolated EMS guest | 44612 | 2 MiB profile; MAP SWITCH ALIAS UNMAP FREE PASS and exit 0. Actual map/unmap/alias-hit markers present. |

Reports t406-s2-r001-{mem,command,edit,write,ems}.txt and PID-correlated
physical-mapping logs are under O:\ntvdm64\logs\. The isolated EMS package
uses copied media/defaults and a changed copied PIF only; temporary Q: was
removed after execution. No real serial device test or exhaustive invalid
guest BOP category test was performed; these results are not full device or
DPMI service acceptance.

StageProductExecutable.mjs verified/published the x86 executable to
build/output/ntvdm32.exe and O:\ntvdm64\ntvdm32.exe:
3,236,864 bytes, SHA-256
6cddfd9e917e892a04df877d8b6d69c3e561cf31d933d8a4980325deb6a77947.
The prior tested EXE is retained as prior-ntvdm32.exe in the run root.

## Remaining work

S2/T406 remain open. Restore the external-memory add/remove protocol as a
complete group with original-shaped callers, prove alignment/backing/failure
semantics and preserve EMS; resolve source-proven access-boundary cases and
remaining width cohorts. No missing mapping body, DIB integration result or
full WRITE behavior is claimed by this P.
