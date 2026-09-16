# T413 S1 — CCPU/C-VID original-source restoration audit

## Result and scope

Baseline is `5e93bff07`, with unchanged T412/S11 runtime products. This is a
source/build audit and an original-code research fixture, not a published
crash fix. Every changed file in the two audited directories has a disposition
below; runtime repair acceptance remains the implementation gate, not an
unclassified audit item.

The primary defect is selection of context-changing generated video accessors
for host-side access, while the original tree already supplies direct C field
accessors. Restore that original implementation under the correct GDP profile.
Do not rewrite the CCPU instruction executor or add a second renderer.

## Reproducible inputs and counts

- Original comparison: `O:\repos.external\OpenNT\base\mvdm\softpc.new`.
  Mirror: src/mvdm-host/softpc.new. Compare complete base/ccpu386 and base/cvidc,
  not a hand-picked modified-file subset. These counts do not include every
  SoftPC host/video/adapter file outside those directories.
- Run `node tools/audit/AuditCvidAccessorProfile.mjs` with an executable Git
  available (AUDIT_GIT can name its absolute path). Outputs remain under
  build/M0-T413/S1/accessor-audit. The script uses git --no-index numstat with
  CR-at-EOL normalization; blank lines and other whitespace changes still count.
- It emits audit.json, both complete directory diffs, a generated all-accessor
  fixture and the exact x86 build/preprocessing commands. Run build-fixture.cmd,
  preprocess-consumers.cmd, then the script with --collect.
- Existing final build map/dependency log: build/M0-T412/S11/cell-grid.
  Its Ninja dependencies establish actually selected headers, not guessed
  include order. Candidate research patches are not selected product inputs.

| Cohort | Changed files | Added lines | Removed original lines | Diff distance (+ plus -) |
| --- | ---: | ---: | ---: | ---: |
| CCPU base/ccpu386 | 22 | 390 | 62 | 452 |
| C-VID base/cvidc | 7 | 33 | 21 | 54 |
| Total, non-overlapping | 29 | 423 | 83 | 506 |

The old eight-file C-VID count included evidgen.h line-ending-only changes;
its normalized text now has zero diff. It is not an eighth semantic change.
Seven C-VID files still count because two contain trailing-whitespace removal
and one contains an extra blank line. Do not restore trailing whitespace merely
to advertise a smaller number.

Overlay footprint (whole-file lines, not original-mirror distance): localfm.c
20; sas_overwrite_memory.c 16; mvdm_cvidc_vector_binding.c 80; its header 9.
Total 125. GenerateCvidcCpuBinding.mjs is a separate 39-line build generator;
it is neither a mirror diff nor a runtime overlay and must not be double-counted.

## A — Original accessors are present and directly composable

Original base/cpu/src/evid/vglob.c has 451 lines and 38 getter/setter pairs
(76 functions). The mirror, pinned original and ccpu386 copy have identical
SHA-256 E695A9807285703119CFFB0C3BC8A3AA6433EDE43EFA499F21A6E736F94395D8.
The file explicitly says its C interface replaces the EDL/C context switch
for a single field access. It excludes latches, which remain EvPtrs-owned.

Current build compiles ccpu386/vglob.c into original-ccpu386.lib, but the final
product map contains none of its direct accessors. Base/video is compiled with
C-VID's full evidgen.h, whose macros call Video.Get/SetVideo*. The local binder
copies C_Video; original vglfunc.c fills that table with SINIT011/012/013
context-changing functions. Having the original C file in an archive is not
equivalent to using it.

The shared cpu/src/evid body can compile unchanged with the C-VID include
profile. The research fixture actually compiles and links it using MSVC x86
/MT and tests every pair against the independent numeric offsets extracted
from the original generated setters. All 38 pass field offset, value readback,
full 64-KiB guard comparison (only the intended field may change), and r0-r31
sentinel preservation. Initial fixture linkage needed the matching UCRT legacy
stdio import; the source body itself needed no edits. This is direct original
source reuse, not a newly authored replacement accessor algorithm.

Recovery ladder: unchanged original C body is usable (rung 1); finite product
selection/table binding is still required (rung 2). No mirror intrusion or new
field-access algorithm is justified. Generated pointer/signed signatures must
be bridged without incompatible function-pointer calls; do not manufacture
wrappers for already identical types. The abandoned 38-wrapper research patch
is not prescribed as the final implementation.

## B — Exact failure route and expected repair effect

The original host owns a separate ConsoleEventThread. Its
WINDOW_BUFFER_SIZE_EVENT branch at nt_event.c:606 calls nt_mark_screen_refresh;
nt_graph.c:818 calls screen_refresh_required; gfx_updt.c:3585 selects
setVideodirty_total(1000000L) for CPU40. Current macro binding enters
SINIT013::S_0417_CiSetVideodirty_total, which writes shared r21=j_Gdp, r1,
r20 and other J-code temporaries. The CPU thread's generated video write also
uses those global registers. They are globals in j_c_lang.c, not per-thread
or saved automatically by a C call. The generated setter does not preserve
the video's r21 scratch contract.

The retained real failure has r1/Gdp=012025a8, r21=012025a8, ring pointer
017a65e4, ring bounds 005a3fe0..005a4040 and state 6666. State 6666 is the
setter's final marker. Subtracting Gdp from the corrupted ring pointer yields
005a403c: the video ring increment used Gdp instead of its constant 4. The
next write at ring-pointer minus 4 faults. This arithmetic, original setter
body and independent event-thread route identify the shared-scratch defect;
it is not evidence of incorrect emulated x86 decoding.

Restoring direct C accessors removes this host-accessor route into shared
J-code scratch state and its context save/restore/allocation overhead. The
fixture proves the replacement body does not mutate that state. It does not
prove all possible video concurrency is solved or that the real 80x5 workload
has already passed with a repaired product. S2/S3 must verify that, rather
than calling a source proof a completed integration fix. No new global lock
or thread-local clone of the entire video engine is the selected design.

## C — GDP and vector layout decisions

1. ccpu386/vglob.c quotes its local gdpvar.h: VGAGlobals offset 1535. The
   selected C-VID engine uses 1280; its dirty_total field is at 1340. Binding
   the currently compiled CCPU object would write the wrong location. Use the
   identical shared original body without that local header, under the proven
   C-VID profile. The research fixture validates all field offsets, not just 1340.
2. C-VID VideoVector is 81 pointer slots: 39 getters, 39 setters, three pointer
   selectors. CCPU's other original header has five slots. C_Video initializes
   its 78 accessor slots; the trailing three default to null. Under C_VID,
   original egacpu.h deliberately calls setRead/Write/MarkPointers directly;
   those null slots are not proof of a missing reached implementation.
3. The actual Video storage in localfm overlay uses the full C-VID definition.
   The nine short-header translation units are sasCdef, mouse_io, trace,
   nt_ega, nt_vga, nt_graph, nt_mouse, nt_fulsc and nt_det. Preprocessing using
   their product flags finds zero evaluated Video/C_Video field accesses in
   all nine. c_getset's relevant interface is PIG-only and absent in this build.
   Therefore the current crash must not be attributed to an observed short-
   table wrong-slot call. Keep complete consumer guards in the restoration;
   do not product-wide change all header precedence without a reached need.
4. Preserve the original two latch providers and three direct selector
   functions. They are not supplied by vglob.c and must not be silently filled
   with invented field operations. Tests must include their reached contract
   and prevent short-layout consumers from acquiring incompatible slots.

## D — Complete changed-file disposition

Numbers are current +added/-removed original lines, not removal promises.

| Files | Current diff | Decision |
| --- | --- | --- |
| c_xfer.c; call.c; intx.c; iret.c; jmp.c; ret.c; sti.c | +8, +19, +7, +8, +9, +19, +5; all -0 | Each difference is observation-only. Restore these seven complete original files as a separate diagnostic-retirement group after preserving required evidence; no instruction algorithm rewrite is involved. 75-line mirror-distance reduction. |
| c_main.c | +163/-38 | Mixed declarations, setjmp ABI, host interrupt integration, atomic events and diagnostics. Do not revert the file. Retain event transport (T411 evidence) and actual DPMI hooks. The separate IP-mask regression below is real and requires its own complete instruction-boundary scope before implementation. Diagnostics here are additional candidates but not included in the exact seven-file deletion target. |
| c_intr.c | +14/-0 | Keep source-shaped protected interrupt hook; remaining observations can retire with their complete observer family, not wholesale rollback. |
| ccpusas4.c | +38/-1 | Retain EMS/external-memory bindings and declarations. Store/direct-RAM observers are optional instrumentation, not proof that mapping can disappear. Do not delete the whole hunk. |
| c_seg.c | +20/-0 | Early selector 0040h compatibility belongs to DOSX/DPMI integration, not video accessors. Do not call it an x64 remnant or remove it based on this fault. Its code condition is selector equality, not an explicit one-time latch; the comment alone does not prove later lifecycle limitation. |
| cpu4gen.h | +19/-13 | Return-type declarations align selected original c_reg.c bodies (ISM32 versus generated IBOOL/IUH). Retain ABI correction; not a replacement CPU algorithm. |
| c_bsic.h; c_page.c; c_reg.c; popf.c | +2, +4, +4, +7; all -0 | Original-owner prototypes/stdarg/debug-header composition. Retain necessary declarations; removing comments is not functional recovery. |
| c_xcptn.c | +1/-1 | Retain selected yoda/host_getenv declaration compatibility. |
| fpu.c | +7/-1 | stdio declaration remains; pointer-difference IU32 cast/comment is a six-distance x86 cleanup candidate. Assign to existing x87 task, not this accessor repair. |
| ntthread.c | +11/-3 | Typed-null failure returns remain. sizeof format/comment is a five-distance cleanup candidate, not the C-VID fix; do not restore bare pointer returns. |
| ntstubs.c | +11/-1 | Not selected by formal graph. Full original restoration would remove 12 diff lines without changing runtime; do not link the whole file to replace one facade because it defines conflicting Cpu/Gdp/Video and other state. |
| sascdef.c | +5/-1 | Typed-null Sas_overwrite_memory slot preserves declared function type; retain. Actual binding is separately supplied from the original CCPU owner. |
| zfrsrvd.c | +9/-3 | Keep source-owner declarations and typed entry; comment/spacing is not a runtime fix. FPU algorithm/layout belongs to the separate x87 package. |
| ev_glue.c | +13/-2 | Retain original setup with minimal product binder, standard declarations and BIOS callback ABI alignment. Do not replace original video algorithm. |
| evidfunc.h | +13/-17 | Retain correct signatures for the 13 generated mark providers, replacing old implicit/mismatched and duplicate declarations. |
| gdpvar.h | +1/-0 | Only extra blank line; removable formatting, not a missing GDP implementation. |
| j_c_lang.c; j_c_lang.h | +2/-0 each | stdio and CrulesRuntimeError declarations; retain. The shared registers themselves are original, not an authored replacement to delete. |
| sevid019.c; sevid020.c | +1/-1 each | Only trailing-whitespace removal; original algorithms are already intact. Do not restore whitespace or patch the ring write to mask corruption. |

## E — Removal accounting and retained boundaries

- Core accessor restoration: **76 original functions reselected, 0 current
  mirror-diff lines inherently removed**. The 451-line body is already exact;
  linking it is not 451 lines of deleted diff. Minimal binding may add lines.
- Concrete complete-file cleanup group: **75 added diagnostic lines across
  seven files**, reducing current combined distance 506 to 431 (14.82%) and
  changed files 29 to 22, before any necessary new binding. This is a measured
  restoration target, not an already delivered deletion or a crash cure.
- Separate unused ntstubs cleanup: **12 diff lines** (+11/-1), zero runtime
  effect. Combined with the seven-file group: 87, leaving distance 419.
- Two width-era cleanup candidates total **11 diff lines**, not 11 runtime
  instructions. They are not counted as current-task committed savings; x87
  ownership and diagnostic format compatibility must be respected.
- No entire overlay is currently proved removable. localfm's 20-line carrier
  avoids duplicate Sas; the 16-line SAS facade retains an original ntstubs
  fragment without its conflicting globals; the 89-line binder/header and
  39-line generator assemble missing product metadata and preserve timing
  bindings. No second original ready-made full binder was found in the audited
  original tree. Retain minimal composition, do not rename it recovered logic.
- Audit delivery changes **zero product lines**. All proposed savings are
  separated from actual source changes; total eventual net delta is measured
  after the selected binding and cleanup are implemented, not guessed here.

## F — Additional definite semantic divergence (not the C-VID crash)

c_main.c's UPDATE_INTEL_IP_USE_OP_SIZE originally computes
`(EIP + delta) & WORD_MASK` for USE16. Current parentheses compute
`EIP + (delta & WORD_MASK)`; SET_EIP is a direct CCPU_IP assignment, so it does
not repair the missing final mask. Example EIP=0xfffe, delta=4 gives original
0x0002 versus current 0x10002. The macro has nine source call sites, including
conditional branches. This is a concrete original-semantic divergence,
not an x64 requirement and not proof of the observed C-VID crash.

Restoring the whole instruction-delta spelling cohort would remove +5/-4
(nine diff-distance lines), but crosses the admitted instruction-algorithm
non-goal. Record it for explicit complete instruction-boundary admission and
test taken/untaken transfer, operand sizes, page crossing, stack return-address
and exception boundaries. Do not slip a one-line instruction patch into the
video-accessor implementation. The audit finding itself is conclusive; its
repair and runtime acceptance are not claimed.

## Implementation decision

First compose the 76 original C accessors at the existing finite binding,
prove the complete 81-slot/consumer profile including latch/selector rules,
and remove the unselected wrong-layout object from product composition. No
original code needs downloading or inventing. Then execute the declared real
short-window stress and prior Console/command matrix. Retire the seven pure
diagnostic files as one reviewable group with regression tests, not seven tiny
tasks. Report any new necessary binding lines against the 75-line cleanup
target honestly. Keep the independent x87 and instruction-delta findings out
of the C-VID root-cause claim.
