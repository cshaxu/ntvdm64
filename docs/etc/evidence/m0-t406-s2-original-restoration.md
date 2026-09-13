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

## Group 3 access-boundary investigation

Selected CPU_40_STYLE/CCPU headers route get_byte_addr through c_get_byte_addr
to c_GetPhyAdd, not the excluded older Start_of_M_area arithmetic macro.
The original SAS loads/stores/move loops use per-byte access. The formal-library
test now maps guest pages 0x80/0x81 to reversed external backing pages and
checks four-byte loads, stores and moves across 0x80ffe. All checks pass,
along with prior add/remove, backing restoration and real GDI DIB checks.
No product implementation changed.

The r002 Ninja runner rebuilt original-external-memory-test.exe, intentionally
reusing unchanged dependency-tracked formal x86 libraries. Build log:
r002/cross-page-build.log. Runtime log under the approved runtime logs root:
t406-s2-r002-cross-page-verified.txt, exit 0. The first cross-page.txt run
passed byte checks but failed unmapping two independent map records together;
the final test pairs each unmap with its map. Arbitrary unmap splitting or
coalescing is not proven. Retain the original SAS loops.

nt_emm bulk copies resolve only the initial pointer. Their legal caller spans
and move/exchange paths still require original-caller review and tests;
this SAS test does not cover those APIs. Remaining width cohorts stay open.

### EMS storage width restoration

Restored the original long storage contract as one five-file cohort:
emm_mngr.c, emm.h, host_emm.h, nt_emm.c and x86_emm.c. Paired declarations,
definitions, table, backfill and casts now follow the pinned OpenNT source.
emm.h, host_emm.h and x86_emm.c have no remaining textual diff after newline
normalization. x86_emm.c is not the selected CCPU runtime provider; its source
restoration is not a claim that the MONITOR target was built.
Retained manager stdlib declaration, existing LIM writeback selection and
nt_emm mapping API include. DIV-061 now records only the allocator declaration.
Five source files total +38/-54 lines (net 16 fewer), excluding README/tests.

The r002 dependency-tracked formal rebuild passed 17 steps for product and
memory-test targets; log ems-width-build.log. Test exit 0 includes reversed
SAS pages and real DIB sharing (t406-s2-r002-ems-width-memory.txt). The deployed
product and isolated build-owned EMS package were updated from that link.
EXE: 3,235,328 bytes, SHA-256
3f796f59f0519f227ac950b32d0c48b396d676177f694bbb9a8366cb38962fef.

Integration logs t406-s2-ems-width-{mem,command,edit,write,ems}.txt reside in
the approved runtime logs root. MEM PID 19236 and COMMAND PID 32960 exit 0;
EDIT PID 35768 reaches welcome/editor and is bounded-cleaned after timeout;
WRITE PID 36232 exits 255, not accepted; EMS PID 46212 prints MAP SWITCH
ALIAS UNMAP FREE PASS and exits 0. Q: was removed. Initial observer invocation
with forward-slash log paths was refused with 64 before starting guests;
the recorded runs use its required Windows path syntax.

Review: no allocation or guest policy was rewritten. Wider width cohorts and
EMS move/exchange verification remain open. emm_fncs.c splits expanded spans
at EMM_PAGE_SIZE but leaves conventional spans at full length; therefore its
loop alone does not prove arbitrary conventional mapped-page copies safe.

### Allocation-address width restoration

Restored nt_mem.c's original allocation/free/reallocation address expressions
and exclusiveAllocPages IU32 page arithmetic together. Removed the invented
INTEL_ADDRESS_FROM_HOST/HOST_ADDRESS_FROM_INTEL macros and x64-only range
intermediate/assertion. Source delta +12/-33, net 21 fewer lines. DIV-028/034
registry rows are removed. Remaining nt_mem differences are declarations,
the recorded mapping initialize/release hook, and the correctly typed %p
failure diagnostic; that varargs correction remains valid on x86 too.

Formal product and memory-test targets rebuilt successfully using the same
recorded r002 dependency graph (allocator-width-build.log and
allocator-test-build.log). The test now additionally calls the original
VdmReallocateVirtualMemory(old, new-pointer, size) to grow and shrink normal
memory and confirms retained bytes; all prior cross-page/DIB checks pass.
Runtime log t406-s2-allocator-width-memory.txt, exit 0.

Deployed EXE remains 3,235,328 bytes; new SHA-256
f78f42f6a0cd52908391a3ad2fdb36151e82dcd432d444761c7abe3c6781dc8d.
Real integration logs t406-s2-allocator-width-{mem,command,edit,write,ems}.txt:
MEM 47156 and COMMAND 40356 exit 0; EDIT 45600 reaches the editor and times
out under bounded cleanup; WRITE 29444 exits 255 (not acceptance); EMS 36140
prints MAP SWITCH ALIAS UNMAP FREE PASS and exits 0. The isolated package
was refreshed with this exact link; temporary Q: was removed. Wider cohort
review and EMS move/exchange coverage remain unfinished; no T closure.

### Shared host-word restoration

insignia.h IHPE and IUH now use the original unsigned int definitions;
the file matches pinned OpenNT apart from removing one original trailing
space required by diff-check. The formal
generator no longer adds CVIDC_RULE_WORD. The width-only DIV-025/041 rows
are removed. Header delta +1/-10 (net nine fewer lines); generator retains
its existing rule-flags variable without the obsolete macro. No CPU body
changed. SAS capacity declarations remain paired with their current PHY_ADDR
definition; equal storage width does not justify mismatched C declarations.

Regenerated the recorded r002 graph and rebuilt all affected dependencies,
313 steps including product and focused test links (host-word-build.log).
Original memory regression passed including grow/shrink, reversed pages,
and real DIB sharing, log t406-s2-host-word-memory.txt. The five real-program
logs use prefix t406-s2-host-word- in the runtime logs root: MEM/COMMAND
exit 0, EDIT bounded timeout, WRITE exit 255, EMS prints its PASS and exits 0.
These preserve prior bounded acceptance, not full WRITE or GUI acceptance.
Q: was removed after refreshing the isolated EMS package with the new link.

Deployed x86 EXE: 3,235,328 bytes, SHA-256
da235d321b7104ae1224e1fb3fbb80218b7154612cf5f70ab18f781c01813482.
Source review confirms original header semantics and no CVIDC_RULE_WORD use
remains in src/tools/tests. Remaining width/access review is still open.

### Remaining candidate re-screen and C-video review

Re-read the existing paired-diff.csv source mapping, recomputed current
zero-context diffs for its historical text-different rows, and screened added
lines for x64/64-bit, pointer-width typedefs, IHP/IHPE, size_t/ptrdiff_t and
%p. This is a candidate refresh, not full new-file coverage or an updated
all-project removal count. For example, insignia.h still matches the regex
solely because its restored original line names IHP in a comment and differs
by trailing whitespace. A hit is not a remaining width adaptation.

C-video review distinguishes these cases:

- j_c_lang.h adds the existing CrulesRuntimeError void(char *) declaration;
  do not classify a missing original prototype as x64-only behavior.
- evidfunc.h adds four-IUH generated-function prototypes; inspect generated
  definitions and callers before any restoration of original implicit-int or
  differently arity declarations. No rollback is justified by width alone.
- evidgen.h provides callback types rather than IHP transport. This is a
  function/data-pointer contract difference, not just integer width.
- evid_c.h DIV-051 has an incorrect historical rationale: IU32* is a pointer,
  not a 32-bit integer address. Original vglob.c setters/getters use IUH*,
  while original fields use IU32*. In selected insignia.h IUH is unsigned int
  and IU32 unsigned long. The actual question is C pointed-to type agreement,
  not pointer truncation. A grouped original-declaration compile comparison
  remains required before deciding rollback or a necessary minimal fix.

No product source was changed by this read-only review. The DIV-051 rationale
must not be used as proof of unavoidable x64 compatibility. Generated binder
replacement and callback semantics likewise remain separately accountable;
the completed IUH width rollback is not evidence that they are all restored.

### Original CCPU video fields verified

The follow-up restored evid_c.h completely to the pinned original. Formal
rebuild passes 312 steps (original-video-fields-build.log). It emits twelve
unsuppressed C4057 warnings in original vglob.c: the six original IU32* fields
are assigned/returned through IUH* accessors. These are now explicitly known
original type inconsistencies, not truncation or evidence of a missing body.
The selected x86 pointer layout is unchanged; no accessor or CPU algorithm
was rewritten. Removed DIV-051 rather than retaining its false rationale.

evidfunc.h prototypes remain: original sevid001.c defines
S_2127_CGAMarkByte as IUH(IUH,IUH,IUH,IUH), not the historical header's
implicit-int single-argument declaration. This source-defined ABI correction
must not be reverted as if it were mere host-word widening.

Memory test passed with original C-video setup, reversed-page loads/stores,
grow/shrink and real DIB sharing. Five integration logs use prefix
t406-s2-original-video-fields- under runtime logs: MEM 31652 and COMMAND
27592 exit 0; EDIT 24028 reaches editor then bounded timeout; WRITE 43940
exits 255; EMS 16072 prints PASS and exits 0. Q: removed. These bounded
tests do not establish all graphics modes or full WRITE acceptance.

Deployed x86 EXE: 3,235,328 bytes, SHA-256
dc9d467d46133137c5e756138c585ee2167c47f40727c113acae030d84e6b1ce.
This completes the current tested restoration batch, not all remaining width
or EMS move/exchange work. Changes are eligible for a local review checkpoint;
remote delivery remains constrained by the recorded push rejection.

### Delivery and real EMS move/exchange follow-up

Owner explicitly approved permanent main pushes for this personal project.
Push to the existing origin https://github.com/cshaxu/ntvdm64.git succeeded:
80a10dac8..78d492964 on main, including ec7b4bd1d and 78d492964. This resolves
the preceding pending-delivery limitation for those two commits.

Extended the existing guest-only EMS probe using original emm_fncs.c's
INT 67h/57h request layout. With both allocated logical pages unmapped, it
moves four conventional bytes to logical page 0 offset 3ffe, reads across
the 16 KiB boundary back to conventional storage, exchanges new bytes and
reads back again. Assertions check both exchanged old values and new EMS
contents. Existing mapping/switch/alias/unmap/free checks remain.

NASM built r002/emsprobe-move.com from the updated test source. The existing
isolated build-owned package received this probe; its product EXE is unchanged.
Runtime PID 46412 printed EMS MAP SWITCH ALIAS UNMAP MOVE EXCHANGE FREE PASS
and exited 0 (t406-s2-ems-move-exchange.txt in runtime logs). Q: removed.
No host provider, CPU, product source, default EMS profile or user media changed.
This proves original expanded-page splitting in the reached request path;
arbitrary conventional addresses crossing noncontiguous mapped windows remain
unverified. The T and remaining width audit are still open.

### DOS host-pointer arithmetic cohort

Restored source-proven host-only arithmetic in demsrch.c (sentinel,
pathname/device offsets, directory-buffer traversal/alignment), cmdmisc.c
(MULTI_SZ per-drive scan), and demfile.c (FOT prefix length). Removed
DIV-003/091/104. Source delta +19/-38, net 19 fewer lines. DTA/PDB leases,
SFT shadows, guest handle casts, asynchronous command state and observation
hooks are unchanged; these are not implicitly accepted or restored here.

Formal product and memory target rebuild passed (r002/dos-pointer-build.log),
memory test exit 0. Added a dir case to the existing observer, using ordinary
ntvdm32.exe command.com /c dir. The first observer compiler command wrongly
passed user32.lib as C input; it failed and produced no executable. Subsequent
shell attempts therefore did not run guests and their stale LASTEXITCODE
prints are not results. Corrected with /link user32.lib, compiled /MT /W4 /WX,
then reran with terminating shell errors and explicit observer exit checks.

Actual logs t406-s2-dos-pointer-{dir,mem,command,edit,write,ems}.txt are under
runtime logs. DIR enumerates actual package filenames; MEM/COMMAND complete;
EDIT reaches its editor under bounded cleanup; WRITE remains unaccepted;
EMS prints MAP SWITCH ALIAS UNMAP MOVE EXCHANGE FREE PASS. No FOT font-removal
scenario or every search-buffer failure branch was exercised. Runtime scope
is bounded and does not imply all DOS service correctness. Q: was removed.

Deployed x86 EXE: 3,235,328 bytes, SHA-256
94f29e71f6a47cc21638bf1df1138dc94741cb3ec4ca49b4cf197e47d0a3d010.
Review retained the original algorithms and failure branches, not a new
pointer manager. Wider width and mapping-edge audit remains active.

### Host fill-loop source recovery, pending group integration

copy_fnc.c already contains a directly usable DWORD fill loop under #if 0.
The local implementation had instead authored a count-down loop in its other
branch, alongside width changes. Restored original types/alignment and chose
the original loop by #if 1, without rewriting its body. Removed stdint and
the now-unused RtlFillMemoryUlong declaration; DIV-081 retired, DIV-064 narrowed
to source-branch selection. The original NTDLL branch is preserved inactive,
not claimed tested or repaired. Its count contract is not part of this proof.

Before restoration, the formal-library test exercised four start alignments
and 0..32 word lengths (132 cases), checking guard bytes and hashing every
output byte. Baseline digest 91153575. Restored source yields the same digest
and passes the guard checks plus existing SAS/allocator/DIB tests, exit 0.
Logs t406-s2-fill-baseline.txt and t406-s2-fill-original.txt are under runtime
logs. Build logs fill-baseline-build.log and fill-original-build.log are in
r002, using its intentional dependency-tracked formal library reuse.

This verifies bounded output equivalence, not every data value or graphics
mode. No product deployment yet; current runtime identity remains the prior
DOS-pointer batch. Config tagged values and platform selection remain under
review; complete group integration before publishing this working batch.

### Config/fill group verified

Restored config_inquire's original tagged-pointer return expressions and the
complete original memory-size conditional bodies. The sole platform seam
recognizes _M_IX86 alongside historical i386 locally, so selected x86 behavior
does not change and no global CPU macro is introduced. DIV-117 retired;
DIV-056 narrowed. Retained %p varargs correction and bounded observations.
Config delta +26/-20 restores previously deleted original lines; copy_fnc
delta +9/-22 removes the replacement loop and width scaffolding. Combined
source delta +35/-42 (net seven fewer lines), not a count of algorithms.

Formal config-fill-build.log succeeds. Memory regression includes the 132
fill cases and prior SAS/allocator/DIB tests, exit 0. Deployed x86 EXE:
3,235,328 bytes, SHA-256
6fd33cecbf1e40c8eef907306d2e4cef0bf1cae4bcebd9b7ee51da08ff126bb4.

Actual integration logs t406-s2-config-fill-{dir,mem,command,edit,write,ems}.txt
are under runtime logs: DIR 37136, MEM 41192, COMMAND 23636 exit 0; EDIT
40404 reaches editor then bounded timeout; WRITE 44640 exits 255; EMS 23176
passes mapping and cross-page move/exchange, exit 0. Refreshed the isolated
package product from this link and removed Q:. No all-graphics or full WRITE
claim. This closes this restoration group; the remaining audit stays open.

### Host support width cohort

Restored original ROM string-count expressions, nt_reset fixed option length,
nt_fdisk private buffer address/alignment, nt_sec null-handle diagnostic and
fprt CRT buffer/length casts. fprt.c and nt_sec.c now match original text.
The five files total +13/-32 (net 19 fewer lines). Retired DIV-069/070/124/145.
Kept ROM residency selection, PDB/SCS leases, session teardown, CCPU accessor
prototypes and observers. The inspected nt_cprgs/nt_aorc callback changes
are a generated-provider contract family, not pure pointer width; not reverted.

Formal host-support-build.log and host-support-test-build.log pass. Added
six pure-memory get_aligned_disk_buffer cases (masks 0,1,3,15,511,4095),
checking address range/alignment and writable first/last sector bytes, then
freeing the test-owned allocation. No disk handle was opened or disk I/O
performed. Existing 132 fill, SAS cross-page, allocator and DIB tests pass,
log t406-s2-host-support-memory.txt, exit 0.

Integration prefix t406-s2-host-support- under runtime logs: DIR 31812,
MEM 22572, COMMAND 34720 exit 0; EDIT 45088 reaches editor then bounded
timeout; WRITE 52160 exits 255; EMS 20012 passes map/move/exchange and exits 0.
Isolated package refreshed and Q: removed. No raw-disk or fullscreen regen
acceptance is claimed. Deployed EXE 3,235,328 bytes, SHA-256
f3b84cd6fe5326a8f0b5ee0abe8e3c4ac90a41bbbb00438ab6c9ab685d8c3afa.
This group is verified; the wider audit remains active.

### COM residuals and redirector string widths

Restored four remaining com.c ioctl pointer casts and nt_com.c's forward
declaration/thread argument casts to original long/DWORD forms. This corrects
the earlier P1 cohort's incomplete caller sweep. Kept the UTINY read buffer
contract and correctly typed baud diagnostic. Restored original DWORD name
lengths and pointer arithmetic in vrmslot.c/vrnmpipe.c, including retirement
of a size_t/MAXDWORD guard (not representable as a successful larger x86
allocation). Async request, guest leases and worker termination are untouched.
Four source files +14/-20, net six fewer lines; DIV-168 removed.

Formal product/memory builds pass, and VDMREDIR.dll was explicitly built
successfully (com-redir-dll-build.log), because its sources are not proved by
the EXE link. DLL remains a build artifact; no DLL deployment or live remote
pipe/serial/mailslot acceptance is claimed. Product EXE was deployed, SHA-256
8f6e90a7425575007b967ea0fc28d28c924c671b4381dcbd6cbd0371b88457ec,
3,235,328 bytes. Six integration logs use t406-s2-com-redir-: DIR/MEM/COMMAND
exit 0, EDIT bounded timeout, WRITE 255, EMS map/move/exchange PASS; Q: removed.

An added direct VrConvertLocalNtPipeName fixture was invalid: that symbol in
the EXE is a common-data function-pointer cell, not the DLL function body.
The map locates it at preferred 007d8368, exactly the relocated fault address.
Binding session did not fix that invalid call. Removed the invalid test, kept
failure logs memory-verified/memory-bound, and added /map to the test link for
future attribution. Final t406-s2-com-redir-memory-final.txt exits 0 with all
prior memory/fill/DIB tests. No replacement function or test stub was added.
Direct DLL path-normalization execution remains unverified, explicitly not
covered by these regressions. The wider T remains open.

## Conventional EMS window span: reproduced failure

The preceding logical-page MOVE/EXCHANGE PASS does not cover conventional
addresses crossing independently mapped EMS windows. Original
softpc.new/base/bios/emm_fncs.c:2141 explicitly assumes Intel memory is
contiguous; its loop splits expanded operands at 16 KiB, not conventional
operands. host_copy_con_to_con in host/src/nt_emm.c uses EM_moves, which
resolves each starting address once before MoveMemory.

Extended the existing guest probe without changing production code: map
logical page 1 into window 0 and page 0 into window 1, write 5b6c/7d8e to
the last/first words, then request an INT 67h/57h four-byte conventional
copy from frame:3ffe to ordinary guest RAM. This reverse mapping avoids
accidentally passing through contiguous logical backing.

Built with `nasm -f bin tests/observation/ems_mapping_probe.asm -o
build/M0-T406/S2/r002-dib-restoration/emsprobe-window.com`;
probe SHA-256 bd1916921a4cccf7c7b3c1c5d77fb609cfc89029d8f7627e34f8dbddab05cd0c.
The isolated Q: package EXE hash is the published
8f6e90a7425575007b967ea0fc28d28c924c671b4381dcbd6cbd0371b88457ec.
Observer invocation: `mapping-workload-observer.exe ems
O:\ntvdm64\logs\t406-s2-ems-window.txt Q:\`.

Result: PID 38344 exits c0000005, not PASS. Fault address 00756c87,
image base 00700000, read target 03468000; recorded stack includes source
03467ffe and length 4. Q: is removed and the test job is closed. The
normal runtime EXE was not changed. This is concrete missing integration
coverage, consistent with a host copy running past a translated page; it
does not implicate CCPU instruction semantics. Preserve this failing test
while completing original-source/binding analysis. No mapping closure or
production repair is claimed by this evidence delivery.

### Contiguous-alias feasibility boundary

The current nt_mem.c reserves private memory with VirtualAlloc and commits
the low-memory area separately. VdmMapDosMemory currently records numeric
aliases only; c_GetPhyAdd translates the first address, not a following
native-pointer span. Thus the ordinary host address reservation being
contiguous does not establish contiguous access through EMS aliases.

Microsoft's [MapViewOfFile3 contract](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-mapviewoffile3)
permits page-aligned section views replacing exact placeholders, without the
usual 64 KiB offset/base restriction. This is a candidate mechanism for real
EMS aliases, not proof of a drop-in repair: the current private allocation
would need a section-backed allocation/lifetime binding, preserving commit,
free, resize and failure behavior. The API requires Windows 10 version 1803
on clients. No product platform requirement is changed by this investigation.

External DIB pointers are a separate boundary: original wdib.c passes
CreateDIBSection's returned bits to VdmAddVirtualMemory. MapViewOfFile3 takes
a section handle and offset, not an arbitrary existing private pointer.
Consequently an EMS section design cannot by itself remove the external-DIB
translation mechanism. Full replacement would need separately proved DIB
storage ownership; do not count it as automatically solved or replace live
sharing with a one-time copy. Next step is a bounded mechanism experiment
and source-contract comparison before any allocation redesign.

Source follow-up: fault RVA 00056c87 falls inside the formal EXE map's
memcpy/memmove range (preferred 00456780 through the next symbol at
00456d00). This attributes the failing instruction to CRT memory copying,
not a CCPU opcode. Both pinned OpenNT editions retain the same one-address
EM_loads/EM_stores/EM_moves macros in nt_emm.c. The original
emm_mngr.c::copy_exchange_data dispatches conventional-to-conventional
requests directly to host_copy_con_to_con; mapped expanded operands can also
be converted to that conventional route.

Original ccpusas4.c offers bytewise c_sas_loads/stores and
c_sas_move_bytes_forward. However, the forward loop does not reproduce
overlap-safe MoveMemory, and c_sas_move_bytes_backward calls c_sas_not_used.
Consequently, replacing EM_moves with that forward loop would be an
unproved contract change, not a complete original-source repair. The next
design comparison must cover restoration of contiguous host aliases versus
a smallest source-shaped access binding, including overlap and all copy /
exchange callers. No change to those original algorithms is made here.

### Native placeholder experiment

Added tests/observation/placeholder_alias_probe.c, a standalone Win32
mechanism test, not a new production provider. It uses the documented
[VirtualAlloc2](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc2),
MapViewOfFile3 and
[UnmapViewOfFile2](https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-unmapviewoffile2)
contracts; it imports no additional historical source and selects no CPU.

Build: existing msvc-x86.cmd wrapper, cl /TC /MT /W4 /WX, onecore.lib,
all OBJ/EXE output under build/M0-T406/S2/r002-dib-restoration.
Build log placeholder-alias-build.log has no warning or error.
EXE SHA-256 f2cc9e43b2d41acea6f34de904caf23f4c7239758985f83cec6d8a3d7f6dbf1a.
Runtime log O:\ntvdm64\logs\t406-s2-placeholder-alias.txt, exit 0:
`PASS x86 reverse-span duplicate-alias refusal restore cleanup`.

Two 4 KiB adjacent placeholders map section offsets in reverse order;
memmove reads the expected four boundary bytes. Replacing an occupied view
is refused without changing existing bytes. Unmapping preserves a reserved
placeholder; remapping both views to the same offset proves bidirectional
shared writes against a third canonical view. Restoring the original offset
recovers its preserved bytes. All views, reservations and section handles
are released with checked results.

Reviewer disposition: this proves availability of native page-alias mechanics
on this host under x86, not original VdmMapDosMemory implementation recovery
or guest acceptance. No production source, formal EXE or deployed EXE changes.
The committed guest window regression still fails on the product baseline.
Before integrating, audit original nt_mem commit/decommit/resize/free and
external backing lifetime; do not substitute this experiment for that closure.

Lifecycle follow-up: nt_mem.c uses private-memory decommit in
VdmDeCommitVirtualMemory, VdmFreeVirtualMemory and in-place shrink; separate
header-table commit/decommit must remain private. A moved allocation copies
bytes and then calls the original free path. Therefore globally substituting
a section for intelMem cannot preserve these callers unchanged.

Extended the native experiment to attempt VirtualFree(MEM_DECOMMIT) on its
canonical section view. Same x86 /MT /W4 /WX build succeeds; log
O:\ntvdm64\logs\t406-s2-placeholder-lifetime.txt records
`section-decommit-rejected error=87`, then the previous PASS. VirtualQuery
still reports MEM_COMMIT and the sentinel survives. Updated probe EXE hash:
f0330fdf5ef31e901d7b623cb54cf37295d53db32bc6f4345a0e4fbd2a62b86e.
This negative result is consistent with the documented
[section commitment restriction](https://learn.microsoft.com/en-us/windows/win32/api/winbase/nf-winbase-createfilemappinga):
SEC_RESERVE permits later commitment but not VirtualFree decommit afterward.
Neither replacing decommit with a success no-op nor retaining all committed
storage indefinitely would restore the original lifecycle.

Retain the native-alias route as feasible but not yet composable: it needs a
bounded backing-granularity and reclamation design, or a different proven
same-shaped binding. Production code is unchanged. This finding rules out
the simple single-section replacement; it does not prove that every native
alias design is impossible.

## Remaining COMMAND length cohort

While the proposed EMS access binding awaits owner confirmation, continued
the approved width-only sweep. Paired with pinned OpenNT dos/command:
restored cmdexec AppNameLen/CommandTailLen ULONG declaration, cmdmisc ULONG
cb and original Console title argument, and cmdkeyb INT iSize/iSaveSize.
Retained LPBYTE/LPDWORD corrections, byte overflow rejection, guest leases
and lifecycle code. Retired DIV-115; narrowed DIV-112/114. Three C files:
+6/-14, net 8 lines removed; README accounting is separate.

Intentionally reused the dependency-tracked r002 graph/toolchain.
command-length-build.log shows three changed units compiled and both formal
EXE and original-external-memory-test relinked. Build exits 0; cmdexec C4703
and LNK4070 remain warnings. Focused t406-s2-command-length-memory.txt exits
0 with allocation, reversed SAS pages and native DIB sharing passing.
Staged x86 EXE: 3,235,328 bytes, SHA-256
2fba77c21709104319017fb239b5b020597ea654daf6927f670faa754f013ce6.

Runtime logs under O:\ntvdm64\logs\ use t406-s2-command-length- prefix:
COMMAND/MEM/DIR exit 0; EDIT reaches editor then bounded cleanup; WRITE
exits 255. Extended EMS window workload still exits c0000005, as on the
preceding baseline: failed, not reclassified as acceptance. Q: removed.
Reviewer checked type restoration separately from retained functional
divergences. No full runtime recovery or T closure is claimed.

## Host ordinal and dialog-local carrier cohort

Restored original ULONG ordinal comparisons in nt_bop.c without changing
their threshold/branch. In nt_error.c restored original PDWORD callback
body, thread-ID casts and local int result. Retained signed EnumWindows
callback return, full dialog callback signatures, LPARAM transport and
WINAPI worker correction. Updated DIV-008/066. Two C files +11/-18, net 7
removed; source-paired ABI reasoning, not blanket removal of pointer types.

Formal x86 r002 build compiled both units and linked product/focused test.
host-carrier-build.log retains existing warnings. Focused runtime log
t406-s2-host-carrier-memory.txt passes. Integration logs with that prefix:
COMMAND/MEM/DIR exit 0; EDIT reaches editor then bounded cleanup; WRITE
exits 255. These do not accept VDD ordinal registration or every dialog action.

Product .text differs from the previous cohort, so that comparison was not
treated as equivalence proof. After comment-only wording cleanup, rebuilt
(host-carrier-final-build.log); .text matches the just-tested build hash:
4606cf91696f9d5112b52ad77bb8506e513da787843441a86979419834ffcbf1.
Final staged EXE: 3,235,328 bytes, SHA-256
14dd3417c8387350d6a01d1585f21fec15be66b30b730e8eb50f8ea125d504eb.
Final COMMAND repeat exits 0 (t406-s2-host-carrier-final-command.txt).
Known EMS failure and pending access-binding approval remain unchanged.

## Retained declaration audit

- timeval.h / DIV-032: original nt_unix.c defines long host_time(long *).
  NTVDM cmos.c::rtc_init uses long; the alternative time_t caller is in the
  non-NTVDM branch. Installed SDK 10.0.22621.0 ucrt/corecrt.h:642-645 selects
  __time64_t unless _USE_32BIT_TIME_T is set; current build.ninja has no such
  definition. Restoring the header's time_t declaration alone would mismatch
  the selected original body even on x86. Retain long; correct the registry's
  obsolete x64-only rationale. This does not establish all CMOS behavior.
- evidfunc.h / DIV-121: original sevid001.c:526-736 defines thirteen
  S_2126..S_2138 rules as IUH-returning four-IUH-argument functions. The
  original header instead uses implicit return types and zero/one/two
  arguments, with four duplicate declarations. Current declarations match
  those original generated bodies. Retain this arity/type repair; it is not
  a width-only rollback candidate or an autonomous algorithm.
- sas.h / DIV-092: direct sas_overwrite_memory declaration is distinct from
  the selected SAS vector; retain pending whole-carrier composition review.
  DIV-055: original ccpusas4.c:135 defines Length_of_M_area as PHY_ADDR;
  base_def.h defines PHY_ADDR as IU32, whereas restored insignia.h defines
  IHPE as unsigned int. The current header matches the original provider's
  named type. Retain the matching declaration instead of restoring IHPE on
  only the header side; equal byte widths do not ensure equal C types.

This is source/declaration classification only, no product-source change,
new runtime test or complete remaining-width count. Existing function and
overlay bodies keep their separate unresolved owners and evidence.

### Current bounded width re-screen

At 9301f97fc, re-ran original/current diff for historical `text-different`
rows in build/M0-T405/S1/paired-diff-001/paired-diff.csv (1,989 total rows).
Command uses git diff --no-index --ignore-cr-at-eol -U0, examines added
non-header lines for x64, 64-bit, uintptr_t/intptr_t, Windows pointer-sized
integer types, IHPE/IHP, size_t/ptrdiff_t and %p. Result: 41 files, 72
matching added lines. This is a screening count, not remaining deletable
code or complete new-file/overlay coverage; it includes comments and proved
necessary declarations. Historical row classification is not regenerated
here, so this does not establish a whole-repository completion claim.

Next reviewed cohort is DEM guest-handle serialization and search-record
casts: dem.h GETHANDLE, demfcb.c two casts, demfile.c four casts, demsrch.c
six pointer-carrier casts. Examine only removable width intermediates;
guest identity policy and lifetime changes remain separately owned. Other
screened families still include generated callback declarations, diagnostics,
packed guest layouts, CCPU/event bindings and selected-platform macros.
All require explicit dispositions before declaring the width sweep complete.

## DEM carrier-expression restoration

Restored thirteen original expressions across dem.h, demfcb.c, demfile.c
and demsrch.c: GETHANDLE reconstruction, three pairs of handle-register
outputs, and six search-record pointer serialization/comparison expressions.
Only redundant width casts are removed; the original field values, lookup,
comparison and release order remain. Retired DIV-103; demhndl.c's remaining
diffs concern leases, observation and SetFilePointer's LONG argument, not
this retired conversion. Four source files +13/-16, net 3 lines removed.

Formal dependency-tracked r002 rebuild and both links pass
(dem-carrier-build.log). Focused original memory/DIB regression exits 0.
Integration logs O:\ntvdm64\logs\t406-s2-dem-carrier-{command,mem,dir,edit,write}.txt:
COMMAND/MEM/DIR exit 0, EDIT reaches editor then bounded cleanup, WRITE exits
255. FCB-specific operations were not independently exercised; compile and
source-expression review must not be presented as full FCB acceptance.
Existing EMS cross-window failure remains unresolved, not retested or fixed
by this width-only cohort. No new identity provider or CPU behavior is added.

Staged x86 EXE is 3,235,328 bytes, SHA-256
a9f26bfd4512bbc0d329555196f6c2a93790dc1a7479e0c0f037706e51490171.
Reviewer inspected the complete four-file expression diff against pinned
OpenNT; governance and whitespace checks are required before delivery.

## Original guest packet declarations

Restored x86 pointer-typed fields in dosdef.h, apistruc.h, dossvc.h and the
two guest request structs in vrnmpipe.h. Original decoding remains unchanged;
the pointer typedef does not authorize native dereference of a far address.
The first three headers now match pinned OpenNT after newline normalization.
vrnmpipe.h retains only DIV-167's async completion/lifetime changes (two
numeric locations and PrivateAsyncState), not reverted as width residue.
Retired HOST-DIV-001 and SUPPORT-DIV-002/003/004. HOST-DIV-004's separate
guest error lease remains registered. Header delta +13/-29, net 16 removed.

Added original_guest_layout_probe.c: 14 x86 compile-time assertions cover
pointer width, DTA/FCB sizes and offsets, nine-byte DEMEXTERR, eight-byte
mailslot request and both 24-byte pipe requests. /TC /std:c11 /MT /W4 /WX
with original header roots compiles and exits 0. First attempt omitted
/std:c11 and failed at _Static_assert; preserved guest-layout-probe-build.log
is not passing evidence. Corrected log: guest-layout-probe-final-build.log.

r002 formal EXE, focused test and actual VDMREDIR.dll build/link pass
(guest-layout-build.log). DLL is build-tested only, not separately deployed
or accepted for live pipe/mailslot operations. Original memory/DIB tests pass.
Runtime logs O:\ntvdm64\logs\t406-s2-guest-layout-*: COMMAND/MEM/DIR exit 0;
EDIT reaches editor then bounded cleanup; WRITE exits 255. EMS failure is
unchanged and not retested by this header cohort. No full FCB/pipe acceptance.

Staged EXE: 3,235,328 bytes, SHA-256
12d66e1afd94c970316d38c4f08290f5ce4157c8f3cbd148110d65aab7b51096.
Reviewer verified original-paired header diffs, retained async boundary,
layout assertions and actual DLL build selection. T406 remains open.

## Original platform branch restoration

Restored both original branches in inc/mvdm.h and inc/suballoc.h, replacing
unconditional x86 selection with the original guard extended by _M_IX86.
The sole MSVC x86 build still selects the same direct field macros and
4096-byte commitment. No i386 macro is added globally and no RISC or CPU30
build is selected. Both original branch bodies are retained verbatim; only
the guard and its divergence comment differ. Header delta +13/-9 (net +4)
because previously cropped original source is recovered, not new behavior.

Dependency-tracked r002 EXE, focused test and VDMREDIR.dll build/link pass
(platform-guards-build.log); existing warnings remain. Focused original
memory/DIB tests pass. Runtime logs t406-s2-platform-guards-* under the
designated logs root: COMMAND exits 0, EDIT reaches editor then bounded
cleanup, WRITE exits 255. MEM/DIR and live pipe operations were not rerun
for this branch-equivalent group. EMS cross-window failure remains open.

Final deployed x86 EXE: 3,235,328 bytes, SHA-256
196bb5823edcacdf5886347712b8ceedc60fa0338c099f9e36a6ec9e76c2f52f.
Reviewer checked original branches and unchanged selected condition rather
than treating restoration of an inactive branch as runtime acceptance.
