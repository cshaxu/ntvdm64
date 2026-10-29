# S10: uncommitted Win95 launch tool review

Scope: the working-tree changes under `tools/win95-launch` and their changed or
new `tests/component-integration/win95_launch_*` fixtures. Source and Git
history inspection only: no guest image, installed tree, product or fixture was
executed or modified in this review. These changes are independent of the MVDM
C/H reconciliation. The worktree is preserved; this report is not permission
to discard it or a claim that the new tool is ready for release.

## Intent and migration

The change replaces the PowerShell launch/undo implementation with CMD apply
and unapply entry points. It prepares a private launch profile, patches VMM32,
replaces the active `[boot] mouse.drv` in place, stores adjacent backups, and
uses a text journal and content hashes for restoration. Three new CMD fixture
tests cover existing-PATCH refusal, failure before a supported VMM image is
prepared, and one synthetic unapply success. The previous PowerShell scripts
and two fixture tests are deleted; the retained
`tests/component-integration/win95_start_menu_template_test.ps1:80,100` still
calls the now-deleted `Prepare-StartMenu.ps1`. The deleted helpers also provided
explicit compatibility, Start Menu preparation/install/restore and paging
operations; the new two entry points do not replace those functions. The
original documentation of those functions is historical evidence, not proof
that their removal has been accepted.

The new fixtures are not referenced by the inspected test/tool/CMake text
searches; their automatic execution by another gate has not been established.
They do not cover successful real-image preparation, changed-file refusal,
multi-step undo failures, unusual paths, pre-existing reparse points, or journal
write failure. The old PowerShell integration fixture explicitly exercised
journal-write failure and preservation of user-changed menu files; it cannot be
considered replaced by the three narrower CMD tests.

## Findings before commit

1. **Broken existing test and removed tool capability.** The still-present
   Start Menu template fixture calls a deleted producer. Either retain/migrate
   the existing functionality and its tests, or obtain an explicit scope
   disposition and update every dependent caller; do not silently drop them.
2. **Path and journal writes are not robust.** `APPLY.CMD:70,79,90,99` uses
   unquoted redirect paths; a selected Windows tree containing spaces can
   cause the wrong output path or a redirection failure. These writes are not
   checked before the script announces success, including the journal, after
   VMM32 and the mouse driver have been replaced. Generated lines at 76–85 also
   interpolate path text as CMD syntax. Paths containing metacharacters require
   explicit handling or an early safe refusal. This is a source-level finding;
   an end-to-end reproduction was not run.
3. **Selected-tree containment is only lexical.** `APPLY.CMD:124–129` may
   construct `TARGET\SYSTEM\..\...` without canonicalizing the resulting
   driver path; the subsequent string-replacement prefix check is not a
   containment or reparse-point check. `UNAPPLY.CMD:26–28` similarly trusts the
   journal path's textual prefix. Resolve the real target and reject escape and
   reparse-point traversal before any replacement or restore, with fixtures.
4. **Undo depends on the current release bytes.** `UNAPPLY.CMD:33–34` compares
   the installed files with today's `assets/release` components, in addition
   to journal hashes. A later legitimate release update can make a previously
   installed tree impossible to unapply even when its installed files still
   match the journal. The journal is the per-install identity authority; its
   integrity, versions and recovery behavior need an explicit design and test.
5. **Partial restore is not recoverable by rerun.** `UNAPPLY.CMD:42–47` changes
   files sequentially; an error after the first successful restore leaves a
   journal whose preflight still requires both installed files and both backups.
   Subsequent unapply refuses, despite saying remaining files are preserved.
   Define and test failure recovery without claiming atomic success.

Recommendation: retain the worktree and do not commit this migration as a
finished tool yet. First reconcile the removed helper functionality and test
entry points, then fix the bounded path/journal/restore findings and run the
non-guest fixtures against actual entry points, including space-containing
paths, failure injection and changed-file preservation. A full installed-Win95
validation remains separate and must respect the repository's guest/media
policy. This audit did not determine who authored the uncommitted changes or
whether another session is actively working on them.
