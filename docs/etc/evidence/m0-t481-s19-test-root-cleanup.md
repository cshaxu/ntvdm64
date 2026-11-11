# T481 S19 test-root consolidation and root hygiene

## Scope and evidence

Owner requested complete migration of tests into test, repair of references,
root cleanup, cached compilation, verification, publication and pushed delivery.
Baseline is c223177be plus the preserved S18 research/probes. No runtime ABI,
original mirror or original guest implementation is changed.

Inventory: 1,305 files under tests and 341 existing files under test. There
were no same-relative-path collisions. Every migrated file was SHA-256 identical
immediately after movement. After reference repair, 1,209 remain byte-identical
and 96 have source-reference edits. All 1,646 files remain under test; tests is
absent. Task receipts: build/t481-s19/migration-before.json, move-identity.json
and migration-final.json. Git retains movement history.

Source references in CMake, Ninja generators, PowerShell, Python, JavaScript,
test includes and retained Markdown links now name test. Installed-package
fixture directories (for example O:/winnt/tests), generated obj/tests paths,
portable upstream documentation and frozen raw historical evidence retain their
meaning; they are not repository-source roots. Literal-root and remaining-path
scans are recorded under build/t481-s19. No immutable archive was rewritten.

## Cleanup and adjacent repairs

Removed 15 untracked root OBJ files, three temporary TXT logs and the old
extensionless report log (19 files). Tracked root documents and CMakeLists.txt
remain. Three historical build scripts now specify /Fo outputs under their
owned build directory instead of dropping compiler objects at repo root.
The documentation governance gate rejects a recreated tests root and root
compiler/log debris. Coding rules name the same canonical source/output roots.

Historical generator references to uniquely identifiable moved fixtures were
rebound. New-T359S2SasStoreNinja's old configuration-command fixture was already
absent before migration; its retained entrypoint now explicitly reports retirement
instead of generating a broken graph. It is not reported as a passing test.
The authored-profile test accepts .cxx compilation commands as well as .c/.cpp;
actual original OLE thunk sources use .cxx. Its macro checks remain unchanged.

## Verification and delivery

Owned background jobs and stdout/stderr are under build/t481-s19/job.
Cached x86/x64 product builds completed successfully (cached2, cached.exit=0).
The first attempt exposed stale WOW32 generation; invoking the existing CMake
wow32 target regenerated its graph before the successful build.

The initial unittest-discover invocation was invalid: the historical build
probes are standalone argument-taking scripts, not importable unittest suites.
That attempt is retained as runner error, not product failure or a passing test.
Applicable command-line checks subsequently passed: 488 authored/generated
compiler commands have no leaked SoftPC profile flags; actual register byte
layout and unsupported-endian rejection passed. Governance self-test and link
verification passed. A trailing-blank diff-check failure was corrected.

The first Full attempt stopped at its package/cache precondition: staging
embeds a coherent PackageId, so the verification comparison cache must contain
those stamped images. The cache was synchronized from the verified manifest,
then Full was restarted. No failure was suppressed or counted as passed.

Final Full passed all 16 selected gates in 346,153 ms, including Console17,
Window17, RPC, native GUI, negative versions, relaunch, isolation, retirement
and nested-window handoff. The absent external DOS4GW/NESticle fixture remains
an explicit skip. Receipt: build/t481-s19/full/timings.json; Full exit code 0.

The complete 11-image package 7773a9b4-e360-4469-962a-df26bb16faf4 was published
and SHA-256 checked at assets/release and O:/winnt/system32. Publication receipt:
build/t481-s19/publication/result.json (publish.exit=0). No add-on or guest
configuration update is part of this source-layout delivery.

All modified PowerShell and JavaScript files parse successfully. Final governance,
link and diff checks passed after the closure record update (final-checks2.exit=0). S19 P1 is the
reviewed migration, hygiene and evidence delivery; remote synchronization is
verified after pushing that commit.

## Preserved boundaries

S18 side-session probes and design evidence survive this move. They document
an unresolved program identity/DIRECT waiter defect, not a repaired product.
This cleanup does not claim to resolve it. No additional execution of those
research probes is claimed.

Parallel deletions under tools/win-multitask and the new Win95 Setup queue
proposal are outside S19 and are neither reverted nor included in its commit.
The worktree may therefore still contain those separately owned changes.
assets/winnt.zip and installed Win3.x/Win95 configuration are unchanged.
