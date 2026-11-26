# S9 P11: final headers and XMS

Fixed source: winxpscodes a2f6d7c93aa4f11efc51147566d5961c59a8ef89,
paths and SHA256 verified against files.csv. Original guest is unchanged.
Owner permanently approves original newline/EOF restoration even where it
reveals multiple actual differences formerly joined by whole-file CRLF.

| File | Before points/lines | After points/lines | Change | Exclusions |
| --- | ---: | ---: | ---: | ---: |
| softpc.new/host/inc/cfpu_def.h | 1/379 | 16/46 | +15/-333 | 2 |
| softpc.new/host/inc/host_cpu.h | 1/661 | 11/15 | +10/-646 | 8 |
| xms.486/xmsblock.c | 1/519 | 7/38 | +6/-481 | 1 |

Counts exclude only matched error bodies, include newline/EOF and #line,
and compare with fixed XP bytes. Source total line counts are not a reduction
metric. Batch +31 points/-1460 lines; complete S9 baseline 684/32045 becomes
640/1457, net -44/-30588. See [41-file census](s9-p11-census.csv).

## cfpu_def.h

The fourteen frozen points 03144-03157 describe x86 floating/integer layout
and exception-state adaptations. Original high/sign-first declarations do not
describe little-endian MSVC bitfield and word layout. Preserve low word first,
mantissa low bits, exponent and sign ordering for FP32/64/80 and FPU_I64.
The status-query macros inspect _statusfp, not the _controlfp exception-mask
configuration. Clear uses _clear87 plus masked exceptions; original unmasking
would expose native FPU exceptions during emulation. Existing x87 layout and
actual CCPU FIST64 fixtures cover these distinct contracts.

Restore LF and remove redundant/inactive #line directives. All 82 selected
content/logical-line pairs remain identical. No functional token is changed.
See [metrics](s9-p11-cfpu_def-metrics.json). All fourteen adaptations remain
in pending decisions; source review does not count them as restored.

## host_cpu.h

Frozen points 03158/03159 retain the x86 low/high byte ordering. Point 03160
omits the original host_simulate macro: actual nt_cpu.c defines that function
as the lifecycle wrapper around cpu_simulate. Restoring the macro with the
real product compile flags fails at nt_cpu.c:86 with C2054 on
host_simulate_func (exit 2), rather than binding the required function.
The temporary control header was restored in finally; it was not published.
Evidence: build/t485-s9/p11/nt-cpu-original-macro.log and exit file.

Restore original LF and retained declarations. Correct the stale #line after
the reordered byte fields: original pad is line 131, not 134. Selected
original-line checks cover 143/175 matching lines for the two CCPU selections.
Keep all eight exclusions. See [metrics](s9-p11-host_cpu-metrics.json).
All three functional deviations remain pending decisions.

## xmsblock.c

All six frozen points 04545-04550 retain necessary caller/provider boundaries.
The zero-size base is not justified merely by its existing test: original
himem4.asm AEMGotHandle checks AX immediately after XMS_ALLOCBLOCK and treats
zero as out of memory; LockExtMemory returns the stored base. Returning zero
cannot provide the supported successful zero-size handle to this caller.

Descriptor access uses existing C SAS copy_from; memory preflight uses
ccpusas4.c range_valid, including translation/page/range checks. The original
GetVDMAddr/host-pointer copy cannot replace the selected SAS owner for paged
or device-backed memory. Copy stays c_sas_move_bytes_forward, preserving the
original forward-copy and zero-length semantics. Failure follows the existing
ERROR_INVALID_ADDRESS/TerminateVDM path, not an outer-layer session callback.
No new memory algorithm, ownership table or fallback is introduced.

Restore LF and remove the inactive #line inside the excluded i386 arm.
All non-#line bytes are unchanged after newline normalization; 201 original
selected-line mappings checked. See [metrics](s9-p11-xmsblock-metrics.json).
XMS allocation/move fixtures exercise the actual provider with bounded seams;
they do not establish full CPU execution or product shutdown on invalid input.

## Verification and delivery

All 41 S9 files and 437 frozen points are source-reviewed. Prior delivery
covers 38 files and 253 fully restored frozen points. P11 adds no fully
restored frozen functional point: its 23 remaining adaptations stay in
pending decisions. Format cleanup is not double-counted as a functional fix.
Cached full build, runtime11 dual publication, x87/FIST64/XMS targeted tests,
final-package MEM/NTVDMD and NESticle are pending. Full remains assigned to S13.
Build/test scripts and logs are under build/t485-s9/p11.

### Final-package verification

Runtime11 cached full build and dual thirteen-image publication passed;
26 destination hashes rechecked after testing. x87 layout passed; real CCPU
FIST64 passed 16 cases. XMS actual-provider allocation passed 138 assertions;
move passed 8 assertions including descriptor/range rejection and termination
seam. Final-package MEM completed successfully with live NTVDMD export,
debugger register/writeback and lifecycle witnesses. Actual DOS4GW/NESticle
rendered its menu and passed Escape hide/restore. No original guest or winnt.zip
was changed. The three final source token streams match their pre-P11 versions
apart from line directives/format. All 41 census hashes match current sources.
Build/publish runner PID 12432; targeted runner PID 10224, both completed;
all declared PASS artifacts inspected. Source review and targeted verification
are complete; commit/push pending. Full is not claimed and remains S13.

Delivery receipt: e9bc7d0b3 committed and pushed to origin/main; documentation governance and relative links passed after indexing this record.
