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

## P2 external-memory group result

Implemented against P1 80a10dac8. The complete VdmAddVirtualMemory and
VdmRemoveVirtualMemory function region now matches the paired OpenNT text
exactly after newline normalization. Original DWORD alignment, page rounding,
allocation, overwrite, remap flag and removal order remain source-owned.
The InitIntelMemory/FreeIntelMemory lifecycle seam is registered as DIV-035.
The unavailable lower setter uses preallocated per-session page slots, not
the deleted publication/prepare/cancel protocol. The source/test search has
no remaining calls to those four retired mapping helpers, including the
former private set wrapper.

Production source/header change is +75/-177 (excluding README): 102 fewer
physical lines. This is net line reduction, not 102 original algorithms
recovered. The adapter still supplies missing lower page bindings; its
55 added lines are not labelled imported source.

### Verification and rejected test paths

Fresh r002-dib-restoration formal x86 build completed 437 steps. The adapter
test compiled /MT /W4 /WX and passed normal plus denied-log cases: invalid
initialization size, duplicate initialization refusal, full two-page lookup,
outside-span miss, original normal-backing reset, EMS replace/unmap and
LastError/errno preservation at the observed mapping operations.

The historical New-T313CcpuLifecycleNinja graph compiled its updated fixture
but could not link nine current dependencies (including host_hwint_hook,
DpmiCpu40RestoreNativeIdt, host_applClose and guest-location services).
It is NOT counted as passing; no stubs were added. Its fixture no longer
preregisters external memory, but that entire historical lifecycle workload
remains unverified in this delivery.

Instead, original_external_memory_test links the same formal production
libraries through the new non-default original-external-memory-test.exe
target. The initial test omitted the original C-VID setup needed by the
overwrite vector and faulted at a null call. It was corrected to call
setup_global_data_ptr/setup_vga_globals, not an overlay entry or replacement.
Private SoftPC fwrite conflicted with test stdio; test reporting now uses
Win32 WriteFile. These were test-composition corrections, not CPU changes.

The final test passes with the real nt_mem/CCPU bodies:

- oversized reservation failure followed by successful allocation;
- raw host pointer plus one byte, no preregistration, original alignment;
- bidirectional external-byte access and page-rounded end address;
- two-page EMS alias to that memory and explicit unmap;
- removal, reallocation at the same address and independent normal backing;
- real CreateDIBSection: SetPixelV/GdiFlush visible through c_GetPhyAdd,
  guest write visible through GetPixel, removal before bitmap deletion.

This proves native DIB storage sharing, not Win16 WRITE/GDI thunk acceptance.
Heap exhaustion during the new initialization allocation was not injected;
its failure cleanup has source review and invalid/duplicate-init tests,
not a claimed forced-OOM execution result.

| Integration | PID | Result |
| --- | --- | --- |
| MEM | 12852 | Exit 0; prior anomalous size output persists. |
| COMMAND | 49604 | Prompt and injected exit, exit 0. |
| EDIT | 51044 | Welcome dismissed with Esc; editor then bounded cleanup. |
| WRITE | 28004 | Same WOW ingress and W32Init FALSE, exit 255. |
| Isolated EMS guest | 19800 | MAP SWITCH ALIAS UNMAP FREE PASS; exit 0. |

Logs are t406-s2-r002-{mem,command,edit,write,ems}.txt,
t406-s2-r002-binding.txt, t406-s2-r002-binding-denied.txt and
t406-s2-r002-original-memory-verified.txt under O:\ntvdm64\logs\.
Prior failed test logs are retained there. Build logs remain in r002.
The unchanged r001 integration observer is intentionally reused; its source
and toolchain are unchanged. Formal libraries are reused only within r002's
dependency-tracked rebuild of the additional test target. Q: was removed.

Published EXE: 3,235,328 bytes, SHA-256
2f4d823e1ef823c7f1253c3fd2b3de09b240359dcd364ad11b79126737389587.
StageProductExecutable verified both build/output/ntvdm32.exe and the runtime
copy. No guest media/default configuration was changed.

## Remaining work after P2

Retained group 2 design rationale: original nt_mem.c initializes/reserves the complete
Intel address space and its allocation metadata before setting memInit.
Its VdmSetPhysRecStructs call cannot report a late metadata allocation failure.
The chosen finite binding therefore allocates translation slots at that
initialization boundary, with failure propagated as failed initialization,
not a late false-success mapping. Slots cover the original reserved address
space, not an invented handle capacity. Normal backing is represented by no
override; restoring the original HostAddress = intelMem + IntelAddress clears
the override. Original add/remove own alignment, reservation, cache invalidation,
flags and free ordering. The missing lower bodies prohibit claiming direct
reuse there; the adapter is the minimal page-translation mechanism justified
by the original PhysicalPageREC.translation comments. No generic identity
namespace or new external registration contract is retained. Implementation
and bounded verification are recorded in P2 above.

S2/T406 remain open. Resolve source-proven remaining access-boundary cases and
width cohorts. P2's native DIB test does not prove Win16 thunk integration;
missing original lower bodies remain an explicit limitation. Full WRITE
behavior is not claimed.
