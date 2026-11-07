# T479 S8 shared import and S9 display repair

## Owner admission

T479 stays open. S7 delivered-scope audit concluded without claiming unfinished
S6 capabilities. The archived S6 candidate and queue-first successor remain
unchanged; intervening tasks do not invalidate their durable handoff.

## S8: shared library import and wiring (verified and published)

Source: read-only O:/repos.hobby/softpc/src/lib, pinned commit
26d118e5. Inspect corresponding upstream tests and provenance before copying.
Current lib-nxvm components are types, base, kvm-base and kvm-window; audit
all their files and required dependencies. Preserve notices and byte identity;
necessary product binding belongs in local consumers, not divergent shared code.
Reconcile the previously registered local adaptations with upstream behavior.
Import source, not upstream executable snapshots. Build both supported host
configurations and exercise actual consumers before coherent package delivery.

## S9: Win3.1 DOS display transitions (active, owner approved)

Cover all three reported failures as one bounded repair:

- Window to fullscreen: garbled display.
- Fullscreen to window: incorrect characters.
- After returning to window: typed characters invisible while cursor moves.

Pinned upstream references: SoftPC T99 S33 P1 4ecab43b, P3 710ec6e4 and
P4 1ec0d551; use the final accepted state rather than the rejected intermediate
P2 reduction. S33 records CHAIN2 plane-address consistency and text BIOS
aperture selection from the caller BDA. Its source includes cvidc rules,
video addressing/update paths and host nt_cga; snapshot changes require separate
applicability review, not automatic import. Upstream tests are comparative
proof only, not NTVDM integration acceptance.

Trace these changes into the selected NTVDM source, register mirror differences,
retain local selected behavior and use the same fixes with the smallest required
binding. Verify input, repeated transitions, DIR/CLS/VER, repaint and normal
exit on the unchanged guest; retain graphics/input and ordinary DOS/WOW
regressions. No original guest patch or rebuild. Deliver cached build, tests,
all 11 host images, manifests, commit and push under existing rules.

The former queued windowed-DOS text investigation is absorbed here, not duplicated.
The broader guest multitask I/O successor remains separate.

## Initial evidence

Both worktrees were clean at inspection. Local release remains the S7-audited
P4 package; no source import, build, runtime validation or new release has yet
occurred in this admission. SoftPC S33 closure and S34 CURRENT name their own
acceptance and do not establish local success.

## S8 initial byte audit

[Per-file inventory](../evidence/m0-t479-s8-shared-inventory.json) compares all
82 upstream files in the four selected trees against the 52 local files.
Eight are byte-identical, 43 differ only in the previous include-prefix rewrite,
one has substantive differences, and 30 are upstream additions. The previously
local Break identity is now upstream; it must not be retained as a local patch.
The upstream `lib/` include namespace must be provided by the build/layout
rather than rewriting shared source again. Subsequent import and test outcomes are recorded below.

## Import checkpoint and first build

[Imported file identities](../evidence/m0-t479-s8-imported-files.json) verify
82 source files and 33 test/fixture files byte-for-byte. Source resides under
`src/lib-nxvm/lib` to retain upstream includes; product consumers and search
roots are updated. Upstream test bodies remain unmodified; product-owned CMake
registers the Windows-applicable cases and upstream fatal-lifecycle harness.
Linux-only source/tests are retained but not claimed executed on Windows.

Background unit001 PID14444 exited1 before building: cmd parsed forward-slash
batch path as a command switch. Corrected unit002 PID24276 uses the existing
MSVC x86 developer environment and stable ccache/Ninja tree. Its subsequent result is recorded below. The failed launch itself is neither
product failure evidence nor a passing test. Full
build, regression, coherent publication and P delivery remain outstanding.

Unit002 completed exit0: 25/25 x86 shared tests passed in17.09s,
including fatal lifecycle negatives through the upstream harness. This does
not replace product Full or real Win3.1 acceptance.

Unit003 PID19036 completed exit0: 25/25 x64 shared tests passed in11.14s.
The original upstream native window modal/scale and retirement tests are also
imported unchanged under test/integration/lib-nxvm, with their relative fixture.
They verify actual native state and remain distinct from the unit pass.

Integration005 PID26100 completed exit0: the actual AMD64 NTCON linked,
and both upstream native modal/scale and retirement tests passed (6.14s total).
The package Full gate and publication remain pending.

Cached006 PID21492 completed exit0: cached x86 worker/WOW and x64 native
component builds passed; existing shared Win32 tests passed14/14 per architecture.
Governance007/008 failed because Queue rejects numeric task IDs even in link
filenames. Queue now links CURRENT; governance009 PID22560 and diff check
passed. No gate was waived or changed.

Full010 PID19416 failed before product execution because the independent
observer generator lacked the new shared include root. The checked-in generator
and alternate build entrypoints now provide it; observer rebuild succeeded.
Full011 PID27172 is running the unchanged Full cases against fresh11-image
package49af0246-027c-4b11-9ac0-ae94c2a8cfa5.

Audit012 PID25432 completed exit0: all118 imported source/test/fixture copies
match pinned hashes, all82 source manifest rows have identical local/upstream
hashes, modified PowerShell scripts parse, governance and diff checks pass.

[S8 delivery and exact publication](../evidence/m0-t479-s8-shared-delivery.md)
records Full16/16 and all11 deployed image hashes; S9 is now the sole active packet.

## S9 initial source comparison

SoftPC S33 final change spans14 SoftPC source files (202 additions,230
deletions in that upstream tree); this is not a directly applicable local patch.
The local NT4-derived video.c already has registered `text_regen_base()`
(MVDM-HOST-DIV-545), choosing the V86 caller's mono/color aperture and using it
at multiple BIOS text entry points. It must be compared with both start and end
bounds of SoftPC P4 rather than duplicated or overwritten. Generated CHAIN2
read/write rules and presentation owners must be reviewed as an integrated
addressing contract, including dirty markers and VDD transitions. Upstream
reproduction proves SoftPC behavior only, not this product's root cause.

S9 now has a [14-file hunk map](../evidence/m0-t479-s9-upstream-hunk-map.json).
The reviewed generated write handlers sevid016/sevid018 each match both
upstream hunks exactly; their four plane-address shift corrections are imported
without altering surrounding code or newline format, registered as DIV-889.
This remains a deliberately unpublished partial candidate until the entire
CHAIN2 access/display/dirty closure and tests are integrated.

S9 generated-rule checkpoint: all76 changed statements in upstream sevid000,
sevid001, sevid017, sevid019 and sevid020 are now mapped and imported,
in addition to the four existing sevid016/sevid018 statements. The
[statement map](../evidence/m0-t479-s9-generated-rule-import.json) records
same-rule identities, exact statements and local binding preservation.
This is source-edit evidence only, not compilation or runtime acceptance.
The remaining closure includes display/dirty addressing, non-generated writes,
and review of upstream internal-label dispatch versus local generated calls;
the local tree still contains callable internal labels where upstream uses
same-activation goto. That prerequisite must be resolved against actual local
rule execution before claiming upstream test equivalence. No new publication.

S9 display checkpoint: imported the upstream CHAIN2 text/CGA gather, dirty
comparison and scroll addressing, plus matching legacy write arithmetic.
The local graphics selector previously asserted on CHAIN2 without selecting
new dirty handlers; upstream T88/T99 range conversion and selector4 are now
wired to the existing unchained bitmap owner. All14 upstream S33 files now
have candidate changes; prerequisite dispatch and local extra branches still
require verification. Background compile001 PID27404 completed all51 Ninja
steps and linked ntvdm.exe, including the VdmTib ownership check. Its batch
exit artifact contains `ECHO is off.` rather than a numeric status, so the
runner status must be corrected/rechecked before using this as a passing gate.
No runtime acceptance, full package or publication is claimed.

S9 rule verification checkpoint: compile002 PID3648 exited0 with no work
remaining after compile001's51 steps; the first exit-file defect was cmd
interpreting `0>` as stream redirection, corrected with a separating space.
The actual cvidc-vector-binding fixture now asserts physical odd/even plane
addresses, byte/word reads, both string directions/parities, Copy byte fill
and a following activation. Unknown-marker rejection uses selector5 because
selector4 now deliberately selects CHAIN2 bitmap marking. Rules003/004
failed at the new forward-string assertion due to a fixture direction flag;
matching the upstream forward destination convention corrected the fixture,
not product code. Rules005/006 passed, exit0.
Source review additionally confirmed upstream f1d394ca's48 internal-label
continuations apply: calling crules anew initializes CopyLocalIUH to null
instead of retaining the outer saved-local activation. Those exact goto
repairs are imported with original physical line counts preserved; actual
S-rule calls remain unchanged. Rules007 PID26120 then passed the expanded
actual-provider fixture, exit0. These bounded checks do not qualify all70
table entries or replace Win3.1/Full acceptance. No publication yet.

S9 full-rule matrix checkpoint: the fixture now adapts the pinned upstream
chain2_rule_smoke body (all five write sets/twelve entries, both read modes,
five entries, parity/direction and stable nested-call replay) to actual local
providers. Rules008/009 exposed missing fixture sr_lookup initialization at
mode2; the fixture now uses the original ega_writ table, as ega_prts does.
Rules010/011 then exposed a real CHAIN2 DWORD defect: mode0 offset0 returned
5a5aa580 then0000a580. Both original DWORD rules stored only the low word,
advanced the second CPU word by4, discarded its result and returned uninitialized
high16 bits. Upstream calloc masked the replay symptom; importing it would not
prove correct data. DIV-889 now records the bounded correction: advance by2,
compose both16-bit results in the existing slot and retain scalar latch behavior.
Rules012 PID12468 passed exit0, including five independent four-byte value
checks and the complete upstream matrix. The fixture's link still reports its
existing /FORCE duplicate seam warnings; this is a real-provider unit result,
not standalone product or guest acceptance. Actual transition and Full gates
remain outstanding; release files remain unchanged.

S9 package verification checkpoint: cached013 completed exit0; delivery013
PID25984 continues its serial real-package verification through Full014.
No Full pass or publication is claimed before its receipt is inspected.
The ev_glue.c HEAD uses LF (819 lines); an accidental CRLF rewrite expanded
its apparent diff. Restoring LF leaves exactly43 added lines, no deleted
lines and identical normalized source content. The next transition-stage
runner reuses New-PhysicalRuntimeStage and the successful logged-on probe
path; it preserves SYSTEM.INI and both installed authored addon binaries.

S9 Full014 failed at Console-17 EDIT after the preceding sixteen command
cases matched their expected results. The retained candidate menu loses
text and the observer times out. Paired edit016 uses the same checked-in
runner and owned physical stages: published S8 passes; candidate fails
again. This is a candidate regression, not an environment waiver. Full
later groups remain unrun and publication is withheld. Evidence is in
build/t479-s9-display/{r014-verification/full,edit016.json,edit016-baseline,
edit016-candidate}. A new independent RAM word-move oracle initially failed
because it supplied ascending RAM bytes to the generated descending-source
contract. Rules018 corrects that fixture convention; no product change was
made on the basis of the invalid Rules017 assertion.

S9 host-profile closure: SoftPC vga_mode.c excludes NTVDM&&!MONITOR,
selecting ega_text_update; this product selects jazz_text_update instead.
Its retained4-byte cell stride missed the new CHAIN2 layout. Correcting
its pointer/strides/cursor restored EDIT exit in edit020, which then exposed
alternating-character loss in MEM after Console synchronization. The reached
nt_fulsc.c copyConsoleToRegen likewise retained4-byte cells; its increment
is now7 after writing the character, yielding8-byte cells. edit021 passes
the existing EDIT/MEM/EXIT assertions, with no weakened marker or deadline.
The temporary force-full diagnostic019 was rejected with1306 due package
identity before meaningful rendering; it proves nothing about dirty rectangles.
Its source was restored byte-for-byte before the two production corrections.
Rules018 passes its corrected independent descending-RAM source oracle.
Full022/023 will qualify this corrected candidate; the previously failed
Full014 and paired edit016 records remain negative evidence.

Cached022 completes exit0. Full023 remains active under delivery022 PID21096;
no completion is inferred from earlier focused passes. The display import
ledger now preserves initial unmatched hunks separately from reviewed applied
resolutions and hashes the reviewed sources. Local byte destinations retain
their existing implicit narrowing; no upstream-only cast or newline churn
was added. The prepared stage024/control024 runners reuse the successful
Win3.1 probe and record SYSTEM.INI, WIN.INI and both addon hashes unchanged;
they have not yet been launched while Full owns the real-package test slot.


S9 convergence checkpoint: inspected new upstream SoftPC8c27a3b6. Its
independent mode1/odd-address oracle reveals the local missing S_2100
physical-group offsets4/6; rules026 fails at101 with00300010 vs40302010.
Two exact upstream constant changes to8/10 retain diagnostic line counts.
Rules029 passes the existing suite plus all twelve new physical-value cases.
Rules028 did not execute because the guarded source edit rejected a nonunique
match before writing; the subsequent attempted runner did not exist. It is
not product evidence. The guarded match was refined to the actual r23 address
expression, with one match for each constant.

Full023 completed16/16. Actual r023 Win3.1 probes showed intact fullscreen
text, return-to-window text, echo characters visible before Enter, echo output,
DIR/VER/CLS and DOS EXIT. Windows normal exit was requested by F10/F/X and
confirmed through the owned window; all three recorded Run16/NTCON/NTVDM PIDs
then disappeared without forced termination. The root observer had already
expired its600-second observation budget (wait258/exit259), so this run does
not prove the final Run16 exit code. Final-candidate verification must retain
a live observer through normal retirement. No release has been changed.

Cached030 and Full031 run serially under delivery030 PID3844 against the
new candidate. Shared source remains unchanged; host-specific jazz_text_update
and copyConsoleToRegen stay separate because SoftPC does not select these
NTVDM Console paths. Their eight-byte cell addressing follows the same CHAIN2
layout; deleting them would reintroduce the paired EDIT/MEM regression.


Final test review found an upstream fixture boundary issue: offsetfffd plus
six seeded bytes exceeds this fixture's256KiB planes. Change the final offset
tofff9, keeping an odd near-end address with every physical access in bounds.
No production change; rules029 is superseded by a fresh fixture run after this
correction. Do not import an out-of-bounds test just to preserve textual identity.

[Final S9 delivery ledger](../evidence/m0-t479-s9-delivery.md) records source
alignment, negative controls and qualification status.


S9 P1 qualification completes: Full03116/16, bounded rules033, final
Win3.1 instance n766 normal exit receipt wait0/exit0 and reviewed transition
frames. Publication040 verifies all11 images in both destinations. See the
final delivery ledger for runner corrections, retained failures, source counts
and exact publication identities. Owner acceptance remains pending; T stays open.
