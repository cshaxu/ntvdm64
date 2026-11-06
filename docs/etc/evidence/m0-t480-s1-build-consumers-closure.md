# T480 S1 — actual host build and consumer baseline

## Question and scope

Continue the [initial inventory](m0-t480-s1-brief-audit-plan.md) with this
worktree's actual compiler, include and link evidence. S1 closes the finite
baseline/classification stage, not the later semantic review of every difference.
No host product source, version or release asset changed. No runtime tests or
publication are claimed; nothing was written to O:/winnt or pushed.

Source baseline remains b211fd73b for all 1060 inventoried host C/H files.
The owner subsequently explicitly requested committing the existing 1785 guest
file changes without audit: separate commit 806bf1608. Those changes were not
examined or edited and are not T480 audit findings. Build evidence names that
actual HEAD. Guest audit and guest rebuilding remain excluded.

## Actual build evidence

Both clean worktree-local MSVC configurations completed successfully, with
inherited warnings. Stable caches are build/.ninja/msvc-x86-release and
build/.ninja/msvc-x64-release. No main-worktree output/import library was borrowed.
The CMake x86 configuration's normal native-input prerequisites came from the
committed assets/release images; the separate x64 build then built all six native
images. WOW32 used the newly built NTVDM import library.

| Owned run | PID | UTC start / finish | Result |
| --- | ---: | --- | --- |
| x86 configure, NTVDM, VCDEX, WOW32, VDMREDIR, Hook32 | 37452 | 21:41:42 / 21:48:12, 2026-11-06 | exit 0 |
| x64 configure, NTVWM, NTCON, NTMON, Run16, NTSRV, Hook64 | 27368 | 21:42:09 / 21:46:27, 2026-11-06 | exit 0 |
| Final compiler/include/link collector | 14272 | finished 21:53:55 | exit 0 |
| Six actual-command preprocessing cases | 40768 | finished 21:53:53 | exit 0 |

Build supervisors enforced 3600-second deadlines and owned process trees;
collector deadline was 600 seconds, preprocessing 1200 seconds with 180 seconds
per compiler. Logs, exact command scripts, graph identities, image/object hashes,
dependency records and available MAPs are retained in the receipts below.

| Product | Compile prerequisites | Object/archive-member link inputs |
| --- | ---: | ---: |
| NTVDM | 493 | 493 |
| VCDEX | 494 | 3 |
| VDMREDIR | 536 | 51 |
| Hook32 | 52 | 52 |
| WOW32 | 233 | 231 |
| NTVWM | 63 | 63 |
| NTCON | 66 | 66 |
| NTMON | 15 | 15 |
| Run16 | 58 | 58 |
| NTSRV | 61 | 61 |
| Hook64 | 25 | 25 |

1063 unique compile commands across eight actual graphs were matched to built
objects and VALID Ninja compiler-dependency records. Build prerequisites are not
static-link ownership: VCDEX/VDMREDIR require NTVDM's import-library producer, but
do not thereby contain all its object code. Archive-member input is not proof
that /OPT:REF retains every member/function. The collector records both roles;
function-level minimization still requires symbol/contract review.

Intermediate collector receipts are retained as superseded diagnostics: r1 did
not normalize relative include paths; r2 additionally separated link ownership;
r3 fixed include paths but did not unwrap the outer cmd quote around NTVDM's
link command. Only r4 is authoritative. No source decision used those incomplete
counts. The final collector independently rejects missing objects, invalid deps,
unmatched compile commands and changed census/hunk identities.

## Complete file and hunk classification

| File classification | All files | Differing files |
| --- | ---: | ---: |
| Directly compiled source | 538 | 371 |
| Included input only | 521 | 210 |
| No observed compiler consumer | 1 | 0 |
| Total | 1060 | 581 |

All 581 differing files have an actual product input owner. The sole file without
a compiler consumer is byte-identical softpc.new/base/cvidc/c2cpusad.h; it is
also named as a CMake configuration dependency. It supplies no restoration
candidate. There is no proven whole-file unconnected restoration in this baseline.

| Mutually exclusive lexical category | Blocks | Disposition |
| --- | ---: | --- |
| Diagnostic #line or blank insertion | 3418 | Preserve existing diagnostic policy; not a missing capability. |
| Contains existing #error exclusion | 3680 | Preserve exclusions. A mixed block can also contain connected edits; not all its contents are thereby justified. |
| Whitespace restoration leads | 146 | Review physical context; not 146 automatically safe edits. Some contain #line or line-sensitive blank changes. |
| Substantive change in a product input | 2132 | Complete-owner semantic review in S3; file reachability alone does not justify the edit. |
| Total | 9376 | Stable IDs retained; no unconnected-file substantive category remains. |

These are lexical blocks, not bug counts or completed semantic judgments. Every
block now has a classification and physical source coordinates. The initial
SequenceMatcher used str.splitlines, including form feeds: 468 current-start
coordinates differ from physical LF line numbers. The successor adds physical
coordinates without changing IDs or silently treating old coordinates as compiler
locations. Existing snapshots remain historical evidence.

## Actual header and conditional checks

softpc.h has 148 object consumers across NTVDM, WOW32 and VDMREDIR. cfpu_def.h
has two NTVDM consumers. They are not unused because they have no standalone
compile command. Macro definitions require consumer inspection, not absence in
ordinary /E output.

Six original-command /E captures cover keybd_io.c, fpu.c, egawrtm0.c,
egwrtm12.c, dpmiint.c and wow32.c. Observations:

- keybd_io.c's allegedly inactive sas_loadw call actually emits
  `*&KbdInt09Off = c_sas_w_at(0x09*4);`. The historical absent-text observation
  was not exclusion proof; macro expansion explains the changed spelling.
- fpu.c emits the cfpu_def.h `_statusfp()` exception reads. Restoring the old
  control-word reads merely because they are header macros is unjustified.
- dpmiint.c emits the CPU40 stack-info allocation/publication; the current
  NTVDM MAP contains `_Cpu40PmStackInfoAddress` as COMMON storage.
- The video translation units emit their CPU40 table carriers. They are selected
  source, not proven dead code; smaller table/selector boundaries require S3.
- WOW32's actual compile command uses /Gz. softpc.h's explicit __cdecl declarations
  bridge that consumer to the NTVDM exports; the NTVDM MAP identifies `_getAX`
  in original-softpc-cvidc:accessfn.obj. No blanket calling-convention restoration.
- `_dta_location`, `_current_pdb_location`, `_Cpu` and `_Video` are present as
  COMMON symbols in the actual NTVDM MAP. Missing object attribution does not
  establish that demdata.c/localfm.c are unconnected.

Already deleted vdmtib.h/wkglobal.c/wklocal.c/nt_aorc.c remain excluded from
new work. Existing SafeLoadLibrary/i386, PIG, BIGEND and historical diagnostic
#error exclusions are not restored. VCDEX's WINAPI entry ABI and the recovered
apisrv.c subset are not redundant local implementations to remove.

## Confirmed small reductions and subsequent work

The currently confirmed restoration opportunities are formatting, not missing
original functionality. These exact edits can restore original spelling while
keeping current #line directives and executable tokens unchanged:

| Candidate | Current physical location | Exact permissible reduction |
| --- | --- | --- |
| F01, D-537ee29591f5d83b | dos/command/cmdenv.c:267 | Restore original leading spaces on `if (cchRemain < cchString)`. |
| F02, D-edffe1e3661cced8 | dos/command/cmdenv.c:322 | Restore original leading spaces on `setBX(...)`. |
| F03, D-d367fef12c817ced | dos/command/cmdenv.c:387 | Remove added indentation before the original `DWORD Length` declaration. |
| F04, D-8ac690a9f8d2c352 | softpc.new/base/system/qevnt.c:155 | Restore leading spaces inside the existing lock-contract comment; retain the preceding #line and condition. |

These four were inspected in their complete immediate context: no string
continuation, macro continuation, added behavior or calling-convention change.
The other whitespace leads remain leads until similarly checked, particularly
when changing physical line count can invalidate diagnostic correspondence.

No nontrivial functional restoration/removal is yet proved safe by S1. This is a
negative finding, not authority to delete connected adapters. S2 should process
the finite formatting leads and independently identify any remaining inactive
sub-blocks, preserving existing exclusions. It must not invent a whole-file
restoration task. S3 reviews the 2132 substantive blocks and mixed exclusion
blocks by original owner: DOS/COMMAND/XMS/DPMI, CPU/devices/video, WOW32/USER,
Redirector/network, non-MVDM service/declarations and VCDEX. Existing repairs and
modern-host ABI needs must be retained unless concrete counterevidence exists.

## Reproduction and retained evidence

- [Build summary and run/hash manifest](../../../artifacts/mvdm-audit/t480-s1-build-summary.json).
- [All file consumers](../../../artifacts/mvdm-audit/t480-s1-consumers.csv).
- [All block classifications and physical coordinates](../../../artifacts/mvdm-audit/t480-s1-hunk-classification.json).
- [Full command/object/consumer evidence, gzip JSON](../../../artifacts/mvdm-audit/t480-s1-build-evidence.json.gz).
- [Raw build/preprocessor/graph/MAP receipts](../../../artifacts/mvdm-audit/t480-s1-build-receipts.zip).
- [Collector](../../../tools/audit/collect-host-mirror-build-evidence.py) and
  [focused preprocessor capture](../../../tools/audit/capture-host-mirror-preprocessing.py).

Reproduce the normal CMake commands from the recorded build-x86.cmd/build-x64.cmd,
then run the collector with a fresh build/t480-s1 output directory. Run the
preprocessor tool against its evidence.json and another fresh output directory.
Both tools reject out-of-scope output locations; source reads are limited to the
host census. The zipped intermediate records are not additional acceptance runs.

S1 acceptance is source/build/classification evidence only. Functional equivalence,
runtime tests, final semantic minimization and release belong to later admitted
implementation stages. The source tree remains unchanged by this audit.

Delivery checks: final documentation governance PID4516 exited0 after restoring
the required Current headings omitted in the first two checks. The retained
100 receipt hashes, all1060 file records, 9376 unique block IDs and1063 commands
were checked; all581 differing files have product link-input owners. Scoped
git diff --check passed, with no host src or assets modifications.
