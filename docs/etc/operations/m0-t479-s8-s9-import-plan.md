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
