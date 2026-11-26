# S9 Ordered Core Repair Progress

## Scope and counting

S9 owns the 41 files assigned in files.csv, processed by canonical relative-path
order. The frozen 437-point inventory is classification history, not a byte-
sensitive current diff count. Actual counts include line endings and EOF,
mask only admitted #error replacement bodies, and retain #line differences.
S8 closure is 2c8325846; product baseline is S8 runtime14 / fc9492342.

## P1: dbgdll/brkpt.c

Question: do the two explicit WINAPI additions have individual ABI evidence,
or does the historical whole-TU /Gz failure wrongly justify both?

Fixed XP object: a2f6d7c93aa4f11efc51147566d5961c59a8ef89,
Source/XPSP1/NT/base/mvdm/dbgdll/brkpt.c.
Original SHA256: df0830ac23f1c6332126a1840275e81831f5e2b6d92ffed778627c86833a4b26.
The complete file differs only in CRLF vs LF and WINAPI on xxxDbgBPInt and
xxxDbgTraceInt. Restored original bytes: 1 point/540 added-plus-deleted lines
-> 0/0. The separate frozen IDs HXP-R-00001 and HXP-R-00002 are locally restored.
No exclusions or #line remain to reconcile; original line numbering is exact.

Call-chain evidence: dbg/dbg.c obtains both undecorated PE exports via
GetProcAddress and calls BOOL (WINAPI *)(VOID). Both entries have no stack
arguments. ntvdmd.def exports undecorated names; target.cmake retains the
existing default compiler convention. The historical h001-convention trial
used /Gz for the whole DLL and failed; that does not prove these two zero-
argument additions individually necessary. No whole-TU convention is changed.
Final-link symbol resolution and actual calls are required before delivery.

The existing ntvdm_export_live_vdd.c witness is extended to call both breakpoint
and trace exports and verify exception delivery plus register writeback for
each, restoring its EAX afterward. It still uses the real DLL in the real MEM
worker; no parallel test framework or synthetic product wrapper is added.
The diagnostic restoration script asserts normalized source differs only by
the two qualifiers before writing original bytes.

Local state: 1/41 audited; 40 remain; 1 locally restored, 0 delivered.
Known measured-file reduction: 1 point/540 lines; whole-S9 totals are not yet
measured. Cached full build and dual publication are running in
build/t485-s9/p1; tests, commit and push remain pending.
### P1 final package verification

Runtime1 cached full build and dual publication passed, all 26 image hashes
match. The final link map resolves _xxxDbgBPInt and _xxxDbgTraceInt at the same
code addresses as the prior stdcall zero-argument entries. Existing decorated
imports elsewhere and DLL export names are unchanged.
The updated live fixture was separately incrementally compiled, then loaded
by the real MEM worker. Both calls produced matching SAS-base exception
notifications, and each register writeback was verified. MEM exited normally;
query/allocation/failure/task-termination witnesses also passed. Actual
NESticle rendered its menu and Escape hide/restore passed on the same package.
No original guest binary was modified. This is targeted acceptance, not Full.
Both frozen points are final-verified restored. 1/41 reviewed and verified;
40 unreviewed; byte-sensitive measured scope 1/540 -> 0/0. Commit/push completes
P1 delivery. Next: src/mvdm-host/dbgdll/dbgdll.c.
P1 delivery receipt: 1e15cd498 pushed to origin/main. 1/41 delivered; 40 remain.

## P2: dbgdll/dbgdll.c and coupled original declaration

Entry: raw byte-sensitive 1 point/1356 lines, four frozen points. The file
has CRLF while pinned XP has LF. Three differences are parameterized stdcall
boundaries: DbgDllInitialize (3 arguments; /entry:DbgDllInitialize@12),
xxxDbgNotifyRemoteThreadAddress (2 arguments) and xxxDbgNotifyDebugged
(1 argument), called through WINAPI pointers in dbg.c. These are unlike the
zero-argument P1 functions; restoring default cdecl would violate stack cleanup.
The three qualifiers remain on existing lines so original diagnostic positions
are preserved without new #line statements. The resulting file has 3 points/6
lines relative to XP; production algorithms are otherwise original.

HXP-R-00005's old restore-#error recommendation is wrong: target.cmake selects
/Di386 for this DLL, so #error would stop the supported build. The original
four-line initialization branch is restored instead. Its getIntelRegistersPointer
provider is the existing original stubs.c implementation: PROD assert0 is empty
and it returns zero. px86 has declarations/storage but no reads in current src.
No new pointer emulation, algorithm, callback or condition is introduced.

The original API declaration in softpc.h was confined to the V86-monitor branch.
The same declaration is exposed after v86def.h in the existing common i386
conditional (+1 declaration line); existing #line 33 preserves subsequent
original mapping. This is a coupled header adjustment within the owner's small
mirror-increment allowance. Before-image retained in build/t485-s9/p2.

The initial restoration helper rejected the header's CRLF anchor after restoring
dbgdll.c; the insertion was completed preserving header CRLF before the worker
compile. It was not a product compilation failure. Cached full build/publication
is running. Final link, actual DLL initialization and notification tests are
required before declaring the original initialization safe and delivered.
P2 is local, not delivered; S9 delivered progress remains 1/41.
### P2 final verification

Runtime2 cached full build and dual publication passed (26 matching hashes).
The real MEM worker loaded NTVDMD.DLL with restored initialization; both
breakpoint/trace callbacks, register writeback, allocation/failure and termination
checks passed. NESticle rendered its menu and Escape hide/restore passed.
[Final byte-sensitive metrics](s9-p2-mirror-diff.json): dbgdll.c 1/1356 -> 3/6;
softpc.h 1/1629 -> 1/1630. Batch 2/2985 -> 4/1636, +2 points/-1349 lines.
The header has no #error; CRLF differences remain included. Only its coupled
declaration is validated here; its full S12 audit remains outstanding.
HXP-R-00005 is restored; HXP-R-00004/6/7 retain evidenced parameterized stdcall
boundaries. No wrapper, alias, new state or alternate algorithm is added.
S9 is 2/41 reviewed and final-verified; commit/push pending. Across both S9
primary files and the one coupled header, measured baseline 3/3525 -> 4/1636,
+1 point/-1889 lines. This is not the unmeasured whole-S9 census.
Next primary file: dbgdll/excptn.c. Full remains the S13 integrated checkpoint.
P2 delivery receipt: e7dad7b10 pushed to origin/main. S9: 2/41 delivered, 39 remain.

## P3 ordered debugger review (not delivered)

excptn.c: 1 point/681 lines with CRLF. All 20 frozen points have concrete
necessity: 32 register reads/writes must use live CCPU rather than dereference
px86 (the original CCPU provider returns NULL); one parameterized xxxDbgFault
entry is called by dbg.c through WINAPI with ULONG. Original exception frame
low words, high-word merges and dispatch remain unchanged. A context facade
would add synchronization/ownership rather than remove complexity. No product
source change has been made to this file. LF plus inline WINAPI would give
20 points/66 lines (+19 points/-615 lines); owner approval requested before
implementation. All 20 retained points are in pending-decisions.csv.

module.c: 1/415 -> 1/2 locally. The sole adaptation is WINAPI on the two-
argument xxxDbgNotifyNewTask; dbg.c declares and calls the matching pointer.
segment.c: 1/451 -> 1/2 locally. The sole adaptation is WINAPI on the seven-
argument xxxDbgSegmentNotice; matching pointer and actual call are in dbg.c.
Both are restored to original LF, with the qualifier on the original return-
type line. Their bodies are byte-identical to XP after removing that qualifier,
and line counts/positions match original. Existing ABI is unchanged.
Combined local reduction: 0 points/862 lines. No build or delivery claimed;
pure formatting accumulates per owner policy. Residuals stay pending review.

Progress: 5/41 reviewed, 36 unreviewed; 2 delivered, 2 local format restorations,
1 unchanged file awaiting format-point-increase decision. Next dpmi32/data.c.
P2's three parameterized ABI residuals are also now explicitly synchronized
into pending-decisions.csv, alongside their existing repair-ledger records.
### P3 data.c and dpmiint.h review

DPMI data.c: non-error byte-sensitive 5 points/42 lines -> 3 points/38 lines.
Only #line directives change: correct selected ExpSelectorLimit to original
line 131, and code after the inserted CCPU helper to line 157; remove redundant
old-baseline anchors. Removing #line from before/after yields byte-identical
source, unchanged #error. Checked 133 matched retained original nonblank code
and comment lines against logical positions. No algorithm changes.

Cpu40GdtShadowAddress/Cpu40LdtShadowAddress have separate readers/writers in
dpmiselr.c and ownership checks in dpmiint.c. modesw.c consumes the final IDT
accessor, with no fallback to an obsolete construction address. Existing
cpu40_descriptor_domain_fixture.c covers unpublished state, relocation and
architectural limit decoding. These mechanisms remain; no new test pass is
claimed for the local line-only change. HXP-R-00110 remains in pending decisions.

DPMI dpmiint.h: 3 points/6 lines unchanged. The CCPU guard is required for the
software backend's GetRegisterByIndex/SetRegisterByIndex used in DpmiOp0f's
MOV CR0 paths (dpmiint.c). CCPU c_reg.h defines A_EBX=3 and A_ECX=1, proving the
original repeated ECX entry at index 3 wrong for both read and write tables.
All three one-line adaptations remain, with no new wrapper/state/algorithm.
HXP-R-00165/166/167 are recorded as evidence-backed residuals. #error and line
positions are unchanged. No build is warranted solely for this no-edit review.

Progress: 7/41 reviewed, 34 unreviewed; 2 delivered, 3 local format/line files,
1 unchanged approval-pending file (excptn.c), 1 reviewed unchanged header.
Current three local files reduce 2 points/866 lines collectively. The full
S9 byte census remains unmeasured. Next primary file: dpmi32/dpmimemr.c.
### P3 dpmimemr.c review

Byte-sensitive non-error diff: 6 points/27 lines -> 4 points/25 lines.
Removed only redundant #line 261 and 277; all 196 matching nonblank original
lines retain their exact logical positions. No #error, newline or EOF change.
The four allocation-domain adaptations remain evidence-backed residuals:
actual XMS object FLAGS have no i386, so XMSInit reserves its extended range
with VdmAllocateVirtualMemory and initializes ExtMemSA over that reservation.
The generic allocator returns STATUS_NO_MEMORY when exhausted, not the
STATUS_NOT_IMPLEMENTED needed by the original DPMI fallback. Allocation,
free, resize and query must use the same ExtMemSA ownership domain. Restoring
only one operation would mix allocation owners; redirecting shared Vdm APIs
would also affect WOW/EMS and is not an equivalent simplification.
HXP-R-00168 through 00171 are synchronized into pending decisions.
No new runtime pass is claimed. The change is line-only and accumulates.
Progress: 8/41 reviewed, 33 unreviewed; 2 delivered, 4 local format/line files,
1 unchanged approval-pending file, 1 reviewed unchanged header.
Current local batch reduction: 4 points/868 lines. Full S9 census unmeasured.

### P3 dpmimscr.c review

Non-error byte-sensitive diff remains 1 point/2 lines. Actual object FLAGS
select both _X86_ and CCPU. Original fast-BOP hardware entry consumes TEB
VdmContext and GetFastBopEntryAddress; there is no selected implementation
of that hardware entry. Original emulator branch returns zero BX/DX/ES to
the guest. The single guard adjustment preserves that original contract.
Dpmi dispatch slot 3 and initialization are the actual consumers. No source,
#error, newline, EOF or logical-line change. HXP-R-00172 is pending decision.
Progress: 9/41 reviewed, 32 unreviewed, 2 delivered; local reduction stays
4 points/868 lines. Next int21map.c; its 1/6004 raw diff includes CRLF and
excluded bodies and is not yet the non-error census.

### P3 int21map.c entry measurements (review in progress)

Pinned original hash verified. Framework comparison matches all conditional
directives; 10 branches consist only of excluded bodies. Removing those
original bodies and current error markers gives 1 point/5965 lines, including
CRLF/LF and EOF. LF-only candidate gives 20 points/51 lines; it is not applied
and does not include any further line-map correction. Measurement is saved
in build/t485-s9/p3/int21map-metrics.json. Raw 1/6004 is superseded by this
non-error count for reporting. No source change and no completed-file credit.
Existing S38 evidence includes the actual AH59 ES:DI loss, poisoned EDI
return check, INT21 offset/length and EXEC output witnesses. Review continues
against current callers and source; historical tests are not final-P3 passes.

### P3 int21map.c review conclusion; candidate not applied

The functional residuals retain original API translation ownership:
- AH59 uses original segment conversion with DI and width-specific zero-extension;
  S38 recorded real ES:DI being lost and tested poisoned high EDI after repair.
- AX6300 uses existing ReturnDSSI in the non-DBCS profile; S38 unchanged pointer
  probes failed before and passed after this dispatch correction.
- AH06/0C06 nonblocking input retains DOS ZF after the existing CF-only IRET;
  S38 captured FLAGS 3246 becoming 3202 before repair.
- DS/ES unchanged selectors need their protected caches reloaded: SetV86Exec
  loads pseudo descriptors, whereas DpmiSwitchToProtectedMode restores CS/SS
  only. The guard preserves translated output selectors rather than replacing
  them with saved inputs.
- EXEC AL1 copies the successful eight-byte SS:SP/CS:IP result back to its owner;
  the before/after probe recorded A5 bytes versus returned stack/entry values.
- IOCTLMap2Bytes needs the client DX/EDX offset; the original uses DS:0.
- IOCTL metadata pairs need 25/2-byte mapping; format 42 needs 5/7 bytes instead
  of uninitialized Length. Existing controlled device-boundary tests cover
  these without actual disk writes.
- Track I/O's 32-bit selector is at original ParameterBlock+13, outside the
  13-byte bounce copy; reading it from BufferedPBlock is out of bounds.

Candidate preserves all functional tokens and all ten #error branches, restores
LF and reconciles #line. 2152 matching nonblank lines were mapped to XP positions.
Current non-error 1/5965 -> candidate 22/52 (+21 points/-5913 lines). Owner approval
requested before applying; file remains unchanged. This is byte-sensitive,
including newline/EOF, not lexical-zero accounting. Pending records synchronized.
S9: 10/41 reviewed, 31 unreviewed; 2 delivered; four local format/line files,
two unchanged files awaiting format approval, two reviewed unchanged files.
Local reduction remains 4 points/868 lines. Next dpmi32/precomp.h.

### P3 precomp.h review

Non-error byte-sensitive diff stays 1 point/10 lines, no source edit. CCPU
getMODE must use live PE and !VM rather than host architecture or passive TEB
context. The empty local-context macro avoids introducing the monitor-only
VDM_TIB context into CCPU. Three shared declarations bind actual definitions
in data.c/dpmiselr.c to dpmi32.c initialization, selector updates, dpmiint.c
ownership checks and modesw.c IDT publication. They are private DPMI contracts,
not duplicate algorithm providers. Keeping them here avoids repeating externs
in consumers or moving C layout knowledge to B. HXP-R-00222 remains pending.
The pm_entry fixture cited by old audit uses its own getMODE macro; it proves
mode-entry ordering only, not this header's VM test. No new test pass claimed.
Progress: 11/41 reviewed, 30 unreviewed, 2 delivered. Four local format/line
files, two unchanged approval-pending files, three reviewed unchanged files.
Cumulative measured changed files (six primary plus coupled softpc.h):
16 points/4460 lines -> 13 points/1703 lines, net -3 points/-2757 lines.
This includes delivered P1/P2 and local P3, not the complete S9 population.
Local P3 alone remains -4 points/-868 lines. Next base/bios/slave_bs.c.

### P3 BIOS slave and tape review

slave_bs.c non-error byte-sensitive 5/6 -> 3/4. Removed two #line directives
inside excluded branches, retaining post-branch anchors; 105 matching original
nonblank code/comment lines checked. EOI correction remains: ICA1_PORT_0 owns
OCW2 acknowledgment, while ICA1_PORT_1 updates IMR. Existing slave_bios_eoi_test
uses actual INTACK/BIOS/PIC and checks ISR/masks/callback counts. Not rerun yet.

tape_io.c non-error byte-sensitive 1/636 -> 1/635 (19 excluded branches,
including nested directives removed with their owning excluded body). Removed
unnecessary xms.h; actual selected xms.486/xms.h declares no descriptor helper,
and the old cited base/inc/xms.h does not exist. Original local A20 declarations
remain. Exact product FLAGS compile the no-header candidate successfully; only
existing file-scope-function and unused-local warnings. No runtime claim.
HXP-R-00500 is locally restored, not delivered. HXP-R-00501 remains necessary:
third output is limit (not base); actual compat provider writes ULONG and BIOS
source_limit/target_limit are sys_addr. Keep matching prototype. Current CRLF
and redundant line anchors remain for a separately measured formatting candidate;
no blanket byte-restoration claim. Original guest and all exclusions unchanged.
Source review: 13/41, 28 unreviewed; delivered 2. Local changes in six files.
Local cumulative reduction 6 points/871 lines. Across delivered P1/P2 and these
six local files plus coupled softpc.h: net reduction 5 points/2760 lines.
Formatting approval remains unresolved, so full S9 is not delivered.

Tape formatting candidate is now fully measured: 1/635 -> 16/17, +15 points
/-618 lines, 19 exclusions unchanged, 255 matching nonblank lines reconciled.
Candidate only in build/t485-s9/p3/tape-format-candidate.c; approval requested.
HXP-R-00500 is in repair-ledger as local compile-verified, pending delivery.

### P3 virtual.c NIDDB review

Non-error byte-sensitive 13 points/20 lines -> 3 points/9 lines. Removed
redundant line anchors and one extra blank; 681 matching nonblank original
lines mapped correctly. Existing behavior tokens and two exclusions unchanged.
The original calloc instance table is not published until all children exist;
its allocation-failure cleanup needs the added table free after freeing children.
The existing no-retry fixture counts live allocations at every failure ordinal.
The VM byte must be range/live-table checked before vrecs indexing and before
same-byte early return; the existing noreturn compat failure provider is used.
No fallback VM, second registry or allocator is introduced. Three residuals
remain pending; HXP-R-00503 formatting restoration is local, not delivered.
No runtime test rerun or acceptance claim. S9: 14/41 reviewed, 27 remaining,
2 delivered; seven local files. Local reduction 16 points/882 lines; combined
with delivered P1/P2, net reduction 15 points/2771 lines in measured changed
files. Next softpc.new/base/ccpu386/c_bsic.c.

### P3 c_bsic.c review

Non-error 9 points/18 lines -> 1 point/10 lines. Removed redundant #line;
all matching original nonblank lines retain physical/logical positions because
each excluded original body is one line replaced by one error marker. Original
file is LF; only appended helper had ten CRLF lines, now restored to LF too.
The ten-line helper remains: F binds its scalar callback in mvdm_dpmi_binding.c;
B uses the three scalar fields without importing CPU_DESCR. It calls original
read_descriptor_linear, not a second decoder. HXP-R-00506 stays pending.
No behavioral token change; no new runtime pass. S9 15/41 reviewed, 26 remain,
2 delivered. Eight local files; local reduction 24 points/890 lines. Including
delivered P1/P2, measured net reduction 23 points/2779 lines. Not full-S9 census.

### P3 c_bsic.h and c_debug pair review

c_bsic.h 1/2 unchanged. A real PUSHF object compile with the fixed original
header produces C4013: c_getEFLAGS undefined, assumed extern int. That owner
does not include external cpu4gen.h; retaining the one owner-local prototype
avoids adding a broader CPU external API header. Log: pushf-original-header.log.
No runtime defect is inferred solely from the warning.

c_debug.c 1/778 unchanged, LF-only candidate 3/6 (+2/-772), not applied.
Three necessary events: initial pending state, original data match, original
instruction match. TSS match is in c_tsksw.c; c_main.c consumes/reset/cancels
and exposes it with debug register save/restore. DR6 remains architectural
sticky status. S38 actual CPU witnesses verify retained DR6 without spurious
repeat delivery, exception cancellation and RF suppress-once behavior.
c_debug.h LF restored locally: 1/92 -> 1/2; only its pending-state declaration
remains. No behavior or #line-number change. Existing test evidence is not a
new final-package pass. All residuals are synchronized into pending decisions.

Progress: 18/41 reviewed, 23 unreviewed; 2 delivered. Nine local changed files;
three unchanged files awaiting formatting approval (excptn.c, int21map.c,
c_debug.c), four other reviewed unchanged files. tape_io.c has a local include
restoration and a separate unapplied formatting candidate. Local reduction 24 points/980 lines; combined
with P1/P2 net reduction 23 points/2869 lines among measured changed files.
Next c_main.c. No S9 closure or whole-population byte census is claimed.

### P3 c_main.c review

Current non-error byte-sensitive 1 point/9173 lines is unchanged. The full
LF/#line-only candidate is 81/213 (+80/-8960), not applied. Both original and
current have a terminal LF; original has 4940 LF lines, current 4839 CRLF
lines. Candidate verifies 3793 matching nonblank original logical positions;
75 excluded bodies remain. Final selected-path compilation is still required.

All 37 frozen functional records have source-reviewed dispositions in pending.
Removing new declarations produces nine C4013 diagnostics with the existing
product flags, including d_mem/limit_check/address_add from original operand
macros, host_timer_event, ica_intack and load_pseudo_descr. This is declaration
contract evidence, not an observed runtime crash. Diagnostic PID 18464 has
finished; log is build/t485-s9/p3/c_main-original-declarations.log. This was a
negative source-copy compile, not the final cumulative build or acceptance.

The original heartbeat is a separate host thread. Atomic observe/raise/consume
keeps the original event bits, and consuming before callback avoids erasing a
new notification. HLT observes RESET without consuming it; the normal boundary
consumes it. The existing auto-reset event closes the check-to-wait race and is
owned by worker startup/teardown. PIC actually returns -1; the stale-PIC fixture
uses the actual provider. Valid IRQs have one acknowledgement and the existing
DPMI hook chooses DOSX or original guest dispatch.

DPMI successful non-STI emulation marks the current existing simulation frame;
ntthread consumes that mark into completion longjmp code 2. This reuses original
TF/debug/PIC completion, while STI retains its instruction delay. DR6 remains
sticky status, separate from one pending event; reset/cancel and restored debug
context use actual CPU witnesses in ccpu_halt_reset_test.c. Narrow capture/apply
providers are registered by mvdm_dpmi_binding.c, not an alternate CPU. Finally
cleanup abandons only its matching simulation frame; the callback-abort fixture
covers inner and outer SEH exits and frame reuse. These source/test inspections
do not claim a fresh final-package runtime pass or every trap variant covered.

Progress: 19/41 source-reviewed, 22 remain; 2 delivered. Nine local changed
files; four unchanged files await formatting approval. c_main has no actual
diff reduction yet. Local reduction remains 24 points/980 lines; including
P1/P2, net reduction 23 points/2869 lines among measured changed files only.
Next c_oprnd.h. No full-S9 current byte census or delivery completion is claimed.

### P3 approved format restoration and c_oprnd.h

Owner permanently approves original newline/format restoration even when a
whole-file newline hunk splits into the smaller real residual differences.
No behavioral increase is admitted by this exception. Applied excptn.c 1/681
->20/66, int21map.c 1/5965->30/60, tape_io.c 1/636->23/24,
c_debug.c 1/778->3/6 and c_main.c 1/9173->137/269. Earlier candidate counts
are superseded: explicit anchors after conditional branches are retained so
an inactive branch cannot skip the line restoration. Error markers and all
non-line-control tokens are unchanged. These are local changes, not delivery.

c_oprnd.h: 24/42->16/34, three excluded PIG bodies unchanged. Removed eight
redundant line directives; 176 selected-path matching noncontinuation lines
retain original locations. Macro continuations are kept intact. The original
D_E5e limit sizes 94/106 disagree with its own actual F_E5e/P_E5e 108/94-byte
transfers. Actual x87 fixtures cover exact/short segment spans, discontiguous
pages and page protection, with no changes allowed before a failed store.
The shared tail probe forces multi-page translation rather than treating a
word-validated physical address as a contiguous large operand. Bit-string
register indices are signed; the retained calculation and original address_add
handle negative displacement and 16-bit wrapping. The existing CCPU_BIT_CASE
matrix computes its independent oracle using int64 division and includes
BT/BTS/BTR/BTC and 16/32-bit addresses. Nine residual records stay pending;
no new runtime pass is claimed.

Current local batch, recomputed from HEAD and fixed XP bytes: 14 files,
70 points/18346 lines ->246 points/551 lines; +176 points/-17795 lines.
Including disjoint delivered P1/P2 files/header: +177 points/-19684 lines.
Formatting explains the approved point increase; no source-line count is used.
20/41 source-reviewed, 21 remain; two delivered, fourteen local changed,
four reviewed unchanged. No formatting approval is outstanding. Next c_tsksw.c.

### P3 c_tsksw.c review

HXP-R-00559/00560: non-error 1/1171 ->5/7, three excluded bodies unchanged.
Original LF restored; removed #line inside the excluded PIG/#if0 bodies and
one redundant anchor. 448 matching selected-path source lines retain original
logical positions. One c_debug.h include and one pending assignment remain.
Original TSS T bit still sets DR6.BT; pending marks this event for the common
consumer without turning sticky DR6 into a repeated event. No second event
owner is added. Existing debug cancellation tests are relevant shared-state
evidence, not an independent task-switch runtime witness. No new test pass.
21/41 source-reviewed, 20 remain, two delivered; fifteen local changed files.
Local net +180 points/-18959 lines; with delivered disjoint P1/P2, +181/-20848.
Next c_xcptn.c. Final cumulative build/publication/tests/commit/push pending.

### P3 c_xcptn.c review and batch boundary

Non-error 29/41 ->12/23, seven excluded branches unchanged. Restored the extra
blank line (HXP-R-00562), removed 17 redundant #line directives; 437 matching
selected-path noncontinuation lines retain original logical positions.
Five host-hook conditionals retain braces: original syntax only conditions the
first assignment, then unconditionally calls the nonreturning continuation,
skipping guest dispatch even when the hook returns FALSE. The actual host hook
explicitly returns FALSE when unhandled. This is direct control-flow evidence.
The trace guard is also necessary: selected PROD trace.c defines trace_file
with zero initialization, and selected trace_init has no initializer. The
CCPU_SHOW_EXCEPTIONS environment option can otherwise enable fprintf(NULL).
This is the precise provider evidence, rather than the older vague claim that
logging was merely early. Eleven functional residual IDs remain pending.

22/41 reviewed, 19 remain; two delivered, sixteen local changed files and four
reviewed unchanged. P3 batch non-error 100/19558 ->263/581, net +163/-18977.
Including delivered disjoint P1/P2, cumulative +164/-20866. Original newline
restoration explains approved point growth. Complete current-file work is now
ready for the cumulative cached full build, dual publication and targeted
final-package verification. Next ordered file after P3 delivery is call.c.

P3 build attempt 1 (PID 5668) terminated at CMake configuration: the IRQ
fixture projection bound its end to the removed literal #line 4421. No product
compile failure was observed. Updated SourceProjection to find a unique end
after its unique start, and changed this fixture to its actual #else boundary.
No assertion or production control flow was removed. Attempt 2 PID 5564 uses
the same cached build root after attempt 1 terminated. Its final result is
pending; logs and run identities are under build/t485-s9/p3.

P3 cached full build attempt 2 passed; runtime3 staged from runtime2 and current
build outputs, all thirteen images published to assets/release and
O:/WINNT/SYSTEM32 with 26 SHA256 checks. Test attempt 1 PID 10576 stopped before
fixture compilation because the temporary orchestrator passed a slash path to
cmd.exe; this is a runner error, not a product failure. The corrected existing
CMD runner is active as PID 24588. No product rebuild or republish was repeated.

### P3 final verification

Final package runtime3: cached full x64/x86 product build and dual thirteen-
image publication passed; 26 hashes checked again after tests. Four targeted
native fixtures pass: notification atomicity, extracted event consumers,
actual CCPU HLT/reset/debug context and debug binding lifecycle. The actual
CCPU run also verifies TSS T-bit single delivery, updating the earlier source-
review coverage limitation. Extracted-consumer tests remain labelled as such,
not full CPU/PIC acceptance.

Real MEM/VDD: NTVDMD loaded, break events matched SAS, XP debug writeback and
register round trip passed, name/ordinal allocation/reallocation/free and failed
reallocation preservation passed, matching task termination and MEM exit passed.
Real DOS4GW/NESticle: 640x400 menu pixels matched, Escape hid the menu, second
Escape restored the same menu mask. No ROM gameplay claim. All evidence uses
runtime3. Scripts/results are under build/t485-s9/p3; original guest unchanged.
Documentation governance and relative links pass. S13 still owns Full.

22/41 reviewed and covered by final verification; 19 remain. P3 is verified,
with commit/push pending. Remaining functional adaptations retain owner-review
status; this delivery does not approve their permanent retention. The measured
P3 source batch remains 100/19558 ->263/581 (+163/-18977); no mirror change was
needed for the test-projection repair. Next ordered file is call.c.

P3 delivery receipt: f424289aa is committed and pushed to origin/main.
All 22 reviewed S9 files are final-verified and delivered through this batch;
19 remain. Runtime3 and both published locations retain matching hashes.
Functional residual owner decisions remain open; S9 and T485 remain active.

## P4: call.c

Entry non-error 6 points/652 lines, six excluded bodies; fixed XP is all LF,
current source had 341 CRLF and 15 LF lines. HXP-R-00573 is the one functional
change: after load_SS_cache, target SS.B selects SP/ESP independently of gate
operand width. The actual CCPU stack-transition fixture independently combines
both widths and observes stack/register/memory results and fault boundaries.
Restore LF and remove 16 redundant line directives; 218 matching selected-path
lines retain original positions. Result 7/8 (+1/-644), six exclusions unchanged.
No new behavior, no new runtime pass. Residual stays in pending decisions.
23/41 source-reviewed, 18 remain; 22 delivered. P4 one local file; including
prior deliveries, measured cumulative +165 points/-21510 lines, not whole-S9
census. Next ccpusas4.c; pure-format changes accumulate per owner rule.

## P4 ccpusas4.c complete source review; delivery pending

- Frozen HXP-R-00574--00586: all thirteen have concrete provider, ABI or
  correctness evidence in pending-decisions.csv; none is counted restored.
- Whole-file non-error byte-sensitive diff: 42 points/400 lines ->42/387.
  Ten excluded branch bodies remain untouched. 1486 matching selected original
  lines retain their XP logical line numbers; closing #line 1960 remains.
- Unified the duplicate read/write chunk loop, preserving const input, one
  captured owner across callbacks, zero-length behavior, access flags,
  read-before-write lease preparation, partial earlier-chunk commits and
  immediate failure return. Reused existing owner checks in public/location
  wrappers. No implementation moved to B or hidden behind a new provider.
- Original LF restored throughout; functional residuals remain pending owner
  review. Final-code cached build, dual publication and targeted tests pending.
- 24/41 source-reviewed, 22 delivered, 17 unreviewed. P4 two-file cumulative
  batch: 48/1052 ->49/395 (+1 point/-657 lines). Including disjoint P1-P3 files:
  +165 points/-21523 lines; this is not the whole-S9 census.

### P4 final-code verification and delivery

Cached full build and runtime4 staging passed. Thirteen host images published
in assets/release and O:/WINNT/SYSTEM32; all 26 manifest hashes matched.
winnt.zip and original guest inputs were not modified. The owner could test
immediately after publication, before targeted tests completed.

Current-source targeted results: CCPU stack 36/36, actual CCPU/SAS BIOS access
123/123, guest-location exit 0, physical mapping and alias ownership pass;
lease resource 44 checks and coordination boundary 75 checks passed, together
with the existing session lease fixture. The coordination test covers captured
owner across a context switch, partial earlier-chunk commits, inactive owner,
zero length, overflow, wrong-owner and exhausted-slot failure behavior.
Real MEM loaded the actual NTVDMD exports and passed breakpoint/trace/register
and allocation lifecycle checks. Real DOS4GW/NESticle displayed its menu and
passed Escape hide/restore. ROM gameplay and Full are not claimed; Full remains
at S13 according to the admitted repair plan. Evidence: build/t485-s9/p4.

24/41 files final-verified; 17 remain. 142/437 frozen register points reviewed,
295 unreviewed. No P4 functional register point is claimed completely restored;
all fourteen remain residual-owner-review. Two-file byte-sensitive non-error
result is 48/1052 ->49/395 (+1/-657); cumulative measured change across disjoint
P1-P4 files is +165/-21523. This remains distinct from a whole-S9 census.
Next relative-path file: softpc.new/base/ccpu386/fpu.c. Commit/push pending.

P4 delivery receipt: f57a2ce66 is committed and pushed to origin/main.
All 24 reviewed S9 files are delivered; 17 remain. Documentation governance
and relative-link checks passed. Post-test dual publication hashes remain
26/26 matching. S9 and T485 remain active; next file is fpu.c.

## P5: ordered remaining CPU files

| File | Non-error points/lines before | After | Evidence and local result |
| --- | --- | --- | --- |
| fpu.c | 21/26 | 4/7 | M64I must round before mantissa conversion; remove 19 redundant line controls; 5311 matching selected original lines verified; six exclusions unchanged. |
| iret.c | 1/653 | 2/3 | Target SS.B chooses SP/ESP; LF restored and two redundant anchors removed; 223 original lines checked; one exclusion unchanged. |
| localfm.c | 1/21 | 1/3 | Real product flags: original fails C2079 for Sas/Video incomplete types; current succeeds. Keep two canonical includes and line 5 anchor; restore LF. |
| ntthread.c | 2/540 | 5/36 | Preserve original TLS frame ownership with abnormal-unwind cleanup and per-frame completed-instruction reason; restore LF; 203 original lines checked. |
| ntthread.h | 1/21 | 1/3 | Original prefix now byte-exact; preserve three owner declarations/constants used by reviewed chain. |
| ret.c | 1/534 | 2/3 | Target SS.B chooses SP/ESP; restore LF and remove three redundant anchors; 186 original lines checked; one exclusion unchanged. |

No functional tokens changed. No second state machine, new callback or outward
provider introduced. The original M64I converter shifts the mantissa without
rounding; original neighboring integer-store paths call npx_rint first. Existing
real CCPU FIST64 tests cover all four guest rounding modes. For ntthread, the
DPMI emulator marks only the active simulation frame after successful emulation
(except STI's distinct existing timing); c_main resumes its existing instruction
completion boundary. AbnormalTermination abandons only the matching frame.
Existing real CCPU callback-abort tests cover nested escape, LastError, normal
FE and reuse on multiple threads. Residual frozen IDs 00587--00596 remain
pending owner review; none is claimed completely restored.

Local batch: six files, 27/1795 ->15/55 (-12 points/-1740 lines).
30/41 source-reviewed, 24 delivered, 11 unreviewed; 152/437 frozen register
points reviewed, 285 unreviewed. Disjoint measured P1-P5 cumulative change is
+153 points/-23263 lines, not a whole-S9 census. Cached build, dual publication
and targeted verification pending. Next file after delivery: base/dos/emm_mngr.c.

### P5 final verification

Cached full build and dual runtime5 publication passed (13 images, 26 hashes).
Six files have identical functional bytes after LF/#line normalization.
Final-code targeted checks: actual FIST64 16/16; actual stack 36/36; DPMI
completion cases 177-180 all passed, including one TF trap at completed EIP
0x1003. Real CCPU abort fixture passed nested/escaped/normal-FE reuse on three
threads, original selector/interface and control restoration checks. Real MEM
loaded NTVDMD and completed; real NESticle rendered its menu and passed Escape
hide/restore. Evidence build/t485-s9/p5; Full remains S13, not claimed here.
All 30 reviewed files are now final-verified, 11 remain; commit/push pending.

The [unified S9 census](s9-p5-census.csv) now uses one start commit fc9492342.
Forty files are measured: 334 points/30128 lines ->487 points/6864 lines.
The remaining timer.c has three unmatched exclusion anchors and is explicitly
unmeasured, not zero; its entire current bytes equal fc9492342, proving net
change zero without claiming its absolute non-error counts. Therefore the
41-file net change since that start is +153 points/-23264 lines. This supersedes
the -23263 accumulated mixed-batch line delta above (one-line discrepancy).
Absolute full-population totals still await timer.c's ordered review. This
measurement is not additional source audit credit. Current batch remains
27/1795 ->15/55 (-12/-1740), with all ten residual register points retained
for owner review. Next ordered file is base/dos/emm_mngr.c.

P5 receipt: 810623ad1 is committed and pushed to origin/main. All 30 reviewed
files are delivered, 11 remain; documentation governance and relative links
passed. Post-test publication hashes are 26/26. S9/T485 remain active.

## P6: emm_mngr.c

Entry has 43 frozen points (00976--01018), not 43 current diff blocks. Initial
projector said 99/371 by wrongly masking the original LIM writeback body while
counting its selected CCPU copy as new. Corrected projection excludes the 23
truly removed bodies; the additional non-NTVDM writeback diagnostic is retained
as a profile guard, not a removed original body. Recomputed before=109/197.

Restore all 42 whitespace/comment items and the exact XP LIM function body at
its original location. Replace the artificial excluded-original/elif-copy
framework by !defined(NTVDM)||defined(CCPU), preserving its non-NTVDM #error
inside the selected scope. All 24 error directives are retained byte-for-byte.
Original software patch_pages/LIM_b_write/LIM_w_write/LIM_str_write remain;
SAS callers require this provider under CCPU. No copied host memory algorithm,
new state or outward dependency is introduced. Final non-error=25/27,
net -84 points/-170 lines. 1250 matching selected original lines have correct
XP logical line numbers. The old line 1883 at the start of the restored body
was two lines late; the body actually begins at original line 1881.

Actual product compiler flags preprocess both versions successfully; all
163154 C tokens match, including expanded values/string literals. The 42 format
points are locally restored; 01018 is minimized and remains pending owner
review. Cached full build, dual publication and actual guest EMS/return/NESticle
verification remain pending. 31/41 files reviewed, 30 delivered, 10 unreviewed;
195/437 register points reviewed, 242 unreviewed. Next ordered file after this
whole-file delivery is base/inc/ica.h.

### P6 final verification and corrected census

Cached full build and dual publication passed for runtime6, all 26 image hashes
matched. Final-package original guest INT67 Mapping passed switch, alias,
unmap, move, exchange, window and free checks; CallReturn passed its stack/map
marker. Both independent COM fixtures exited normally. Real DOS4GW/NESticle
rendered its menu and passed Escape hide/restore. No original guest binary or
winnt.zip changed; Full remains at S13. Evidence build/t485-s9/p6.

The [refreshed census](s9-p6-census.csv) corrects the LIM projection in both
fc9492342 and current views: 40 measured files 344/29954 ->413/6520; unchanged
timer.c remains explicitly unmapped, so the full 41-file NET is +69 points and
-23434 lines without claiming an absolute count for timer.c. This supersedes
P5's absolute subset census; historical snapshots are not silently rewritten.
P6 itself is 109/197 ->25/27 (-84/-170). 31/41 source-reviewed and verified,
10 unreviewed; 195/437 frozen points reviewed, 242 unreviewed. This batch's
42 format points are restored-final-verified, adding to six earlier S9
restorations; 01018 remains a qualified residual owner decision. Commit/push
pending. Next ordered file is base/inc/ica.h.

P6 delivery receipt: 496479cdf is committed and pushed to origin/main.
All 31 reviewed files are delivered; 10 remain. Forty-eight S9 register points
are restored and verified; remaining functional decisions stay pending.
Documentation governance/links passed, post-test publication hashes 26/26.
Next ordered file is base/inc/ica.h; S9 and T485 remain active.

## P7: base/inc/ica.h

Whole-file non-error byte-sensitive diff 20 points/23 lines ->9/10, net -11/-13.
Restore original whitespace (01079,01081) and both deleted CPU hardware macros.
Retain the original macro unit under its existing CPU_30_STYLE test extended
with !CCPU. Original cpuint_c.h declares CPU_HW_RESET=0 and CPU_HW_INT=2;
ICA's old CPU_HW_INT macro=0 must not override that enum in the selected CCPU
owner/nt_cpu.c caller. CPU_SW_INT has no production use outside this header.
This is an existing-condition extension, not another nested conditional path.
01080 therefore remains a qualified residual decision after minimization.

All eight #error directives remain, with original excluded bodies untouched.
Both HOST_CPU_MOUSE_INT choices retain correct logical lines for 57 matching
original lines each. Entire original macro unit and EOF Ctrl-Z are retained.
32/41 reviewed, 31 delivered, nine unreviewed; 198/437 frozen points reviewed,
239 unreviewed. P7 two format points locally restored; earlier 48 delivered.
Cumulative net +58/-23447 relative to unified fc9492342. Cached build, dual
publication and actual CPU/PIC/keyboard targeted tests pending. Next mouse_io.h.

### P7 final verification

Runtime7 cached full build and dual publication passed. Post-test 26 image
hashes are verified against the runtime manifest. CPU notification (4096 rounds),
event consumers, HLT/reset and actual keyboard/PIC (90 assertions) passed.
The keyboard fixture initially failed fast during reset because it omitted
ntvdm_dpmi_initialize; it now uses that existing product initialization, without
replacing the PIC or reset algorithms. Real runtime7 NESticle menu and Escape
hide/restore passed (dos4gw/nesticle.txt.graphics-result.json). A redundant
second invocation was rejected because the evidence directory already existed;
it is not counted as another pass. Original guest and winnt.zip are unchanged.

32/41 reviewed and final-verified, 31 delivered; P7 commit/push pending.
Nine files remain; 198/437 frozen points reviewed, 239 remain. Restored and
verified S9 points: 50. Residual 01080 stays pending owner decision.
Whole-file P7 diff 20/23 ->9/10, reduction 11 points/13 lines. Since fc9492342,
net +58 points/-23447 lines; 40 measured files now 402/6507, while unchanged
timer.c still awaits its conditional mapping and is not included in that total.
Owner permanently approves original-format/newline/EOF restoration even when
it reveals additional hunk boundaries; both points and lines remain reported.
Next file is base/inc/mouse_io.h. Evidence: build/t485-s9/p7.

P7 delivery receipt: 3afcbb772 is committed and pushed to origin/main.
All 32 reviewed files are delivered; nine remain. Fifty restored S9 register
points are final-verified and delivered. Governance and links passed; publication
hashes remain 26/26. Next ordered file is base/inc/mouse_io.h.

## P8: mouse_io.h, timeval.h, support/main.c

Whole-file review in canonical order: mouse_io.h 19/23 ->10/14 (-9/-9),
timeval.h 7/8 ->3/4 (-4/-4), main.c 1/620 ->33/43 (+32/-577).
Counts include original LF/EOF and #line, excluding only actual #error bodies.
Batch 27/651 ->46/61 (+19/-590). Functional source bytes are unchanged after
line-anchor/newline/trailing-comment-whitespace normalization. All 8/2/29
exclusions remain; selected original logical-line checks are 631/35/112.

Mouse saved painter/address remains on the existing VM background record.
mouse_io.c records it on display and consumes it through cursor_undisplay;
current video mode/page cannot identify old saved pixels. Existing production
body tests cover graphics/page transition, single consumption and restorer
selection. XP nt_unix.c defines long host_time(long *); the single declaration
adaptation matches that provider rather than modern 64-bit time_t.

The main split preserves initialization before outer lifecycle activation and
CPU execution inside its exception boundary. The outer main owns argc across
both calls. Search finds no later production pargc reader: do not claim one.
Its stored pointer nevertheless remains valid with the split signature; no
new storage or execution path was added. Functional points remain pending.

35/41 files reviewed, 32 delivered, six unreviewed; 208/437 frozen points reviewed,
229 remain. Two new format restorations await verification; 50 earlier delivered.
S9 net +77/-24037 against fc9492342; 40 measured files now 421/5917. Unchanged
timer.c remains outside that absolute total pending its conditional mapping.
P8 cached build/publication/targeted validation and delivery pending. Next is
base/support/terminat.c. Evidence: build/t485-s9/p8.

### P8 final verification

Runtime8 cached full build, dual publication and post-test hashes 26/26 passed.
Actual production mouse cursor bodies passed saved-position/page/graphics-mode
transition and single-consumption controls (memory/geometry mocked). Final-package
MEM/NTVDMD live export test and actual NESticle menu plus Escape hide/restore
passed. Original guest and winnt.zip unchanged. 35/41 files final-verified;
52 S9 restored register points verified. P8 commit/push pending; functional
residual decisions remain listed. Evidence build/t485-s9/p8.

P8 delivery receipt: abe34fd2a committed and pushed to origin/main.
35/41 reviewed files are delivered, six remain; 208/437 points reviewed.
52 restored register points delivered. Governance and links passed.
Next ordered file: base/support/terminat.c; S9 and T485 remain active.
