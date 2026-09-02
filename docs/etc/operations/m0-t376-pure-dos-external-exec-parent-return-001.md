# M0 T376 — Pure DOS external execution and parent return

## Purpose

Extend the proved CPU40/SoftPC `COMMAND.COM` built-in baseline to original
guest-owned external DOS execution.  The completed result must be one
immutable original `.COM` and one immutable original MZ `.EXE`, each loaded,
run, terminated and returned through original NTDOS `$Exec` and the guest
parent/COMMAND continuation.  This is not an app executable loader, a native
child route or a trace-selected BOP repair.

## Boundary

- Only the formal Win32/x86 CPU40/SoftPC product and executable-relative
  immutable stage are in scope.  CPU30, Bochs, x64 runtime, WOW/Win16,
  Redirector and native-child COMMAND execution are excluded.
- `$Exec`, JFN/SFT, PSP, MCB/arena, environment, MZ relocation, `$Exit` and
  parent restoration remain guest code and media inputs.  They are never
  rewritten in app or an adapter.
- Host file services remain original DEM bodies.  A host `HANDLE` remains a
  session host-resource mapping-manager identity; host access to guest bytes
  uses a bounded guest-memory lease.  No native pointer becomes DOS state.
- A missing source owner may be recovered only as a complete immediate
  original cohort, with its same-shaped adapter binding and source-defined
  failure behavior.  A trace can validate such a cohort but cannot select a
  leaf repair.

## Ordered subtasks

1. **S1 — External-exec owner and selected-media admission.** Reconcile the
   original `.COM` and MZ guest chains, selected immutable test media, current
   DEM host binding and the formal graph.  Revalidate the former T365 ledger
   against the current CPU40 source, identify the first finite missing owner
   cohort and freeze its source contract.  No product code or runtime run.
2. **S2 — Original `.COM` file-lifecycle binding closure.** Recover the
   complete immediate `demOpen`/`demRead`/`demClose` guest-memory and
   host-resource cohort selected by S1, preserving original carry/error
   behavior.  Use focused positive and negative local evidence; do not add a
   loader.
3. **S3 — `.COM` formal integration and one frozen observation.** Link the
   selected original sources, validate the formal product, then run exactly
   one fixed immutable `.COM` workload in the console-owning container.
   Record either original child/parent completion or one earliest exact
   source-owned successor.
4. **S4 — Alternate-root MZ observation disposition.** The first MZ attempt
   must be classified if its newly staged runtime root fails before a source
   marker.  Such a result is a rejected container, not a loader diagnosis and
   cannot be retried under this S.
5. **S5 — Fixed-root immutable-media extension.** Recover the app-owned stage
   update operation that proves every retained asset before adding a missing
   original workload to the established short root.  It must refuse identity
   mismatch and never overwrite a differing executable or media byte.
6. **S6 — MZ seek/relocation and parent-return closure.** Make one new frozen
   observation in the now identity-proven established root.  Prove one
   immutable MZ workload returns to the original parent and COMMAND
   continuation, or record the earliest exact original owner.  A separate
   owner is admitted only under the dependency-first rule.

## Completion standard

T376 closes only when all of the following are identity- and source-proven:

- one original `.COM` executes and returns through its guest parent path;
- one original MZ `.EXE` executes and returns through that same class of
  original guest parent/COMMAND continuation;
- all reached DEM file, mapping and termination seams retain original owner,
  result and cleanup ordering; and
- formal link plus bounded runtime evidence distinguish actual guest execution
  from a host fixture, BOP arrival or timeout.

If a complete required original owner remains unavailable, T376 records the
exact source boundary and all smaller same-owner recovery routes exhausted;
it does not claim program execution.
