# Proposal — Dual-width native workers and launch hooks

## Superseding owner approval — single worker

The current scope is one x86 ntvwm.exe plus nthook32/64.dll, not dual workers.
S6 rebuilds from accepted12160c657. Superseded candidate code is archived
only and must not enter main. Actual target machine chooses its Hook DLL
and task display (NTSRV actual64 metadata, NTMON WIN64), not worker selection.
NTCON and existing lifecycle/handoff paths remain width-neutral. Final
delivery has ten images, with only nthook64.dll AMD64. The current plan's
S6 section owns verification and publication. The remainder below is the
rejected earlier proposal, retained for provenance, not implementation scope.

## Owner direction and reference

Owner accepts the delivered x86 Hook baseline and separates this package from
that completed work. Select NTVWM32/64 and nthook32/64; run16, NTCON, NTMON,
NTSRV, NTVDM, WOW32 and VDMREDIR remain x86. General component x64 migration
is a separate queued package afterward. Reuse existing Hook/search/installation
and retained ABI/source research; do not start a second implementation family.

## Architecture and scope

- One native-worker source family, two architecture-correct binaries, each
  owning its hidden Console; one Hook source/contract family and matching DLLs.
- NTSRV chooses/reuses the worker matching the selected direct target machine,
  using existing registration/association/completion/lifecycle authority.
- NTCON retains the same width-neutral text renderer and one authorized pipe;
  DOS/native32/native64 use the existing final-state/input-return/release/seed/
  acknowledge/resume handoff. No second frontend or handoff policy.
- Ordinary native children retain Windows identity, handles, parent waits,
  exit codes and Console inheritance. Bitness alone creates no new direct task.
- Shared application search and recognition remain one contract for Run16 and
  both Hooks; audit original same-machine restriction and native-width section
  ABI through minimal original-shaped bindings, not a parallel resolver/parser.
- Cross-width installation is a suspended actual-child transaction with
  caller-requested suspension, exact success, authority and rollback. A finite
  transient installer helper may be used if necessary after review; no resident
  helper or target replacement/lifecycle ownership.
- Hook64 delivers context to x86 run16 for DOS/Win16; run16 never intercepts
  itself. Resource duplication alone is not proof of remote DLL installation.

## Deliverables and acceptance

The detailed sequence and gates belong to the
[working plan](../etc/operations/t432-dual-width-native-workers-hooks-plan.md).
Prove actual32→32,32→64,64→64,64→32 propagation, x86 launcher native-width
selection and both context directions; real CMD/DOS/native nested return,
GUI propagation, suspended/no-inherit/HANDLE_LIST, malformed/missing DLL,
partial-install rollback, independent roots and worker/broker failure.

All production deliveries retain established regression and independent WOW
frontiers, source/input identity and coherent recoverable publication. Final
package has eleven images at product-relative system32: run16, ntsrv, ntcon,
ntvdm, ntmon, WOW32, VDMREDIR, ntvwm32/64 and nthook32/64. This replaces
ntvwm.exe by32/64 variants and adds nthook64.dll to the accepted nine-image set.
The explicit manifest governs every deployment. No x86 object enters
an x64 link; all intermediate/generated/fixture outputs remain below build/.
Compiler success alone cannot prove propagation or T completion.

No guest mutation, x64 MVDM/service/WOW provider, Job observation, new task
registry/scheduler, system mutation or unrelated frontend repair is selected.
Preserve the accepted baseline until a replacement passes its retained gates;
final owner acceptance is required before T closure.
