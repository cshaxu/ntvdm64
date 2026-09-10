# T236 S1 COMMAND Local Child Source-Recovery Result

## Question

Can the imported OpenNT COMMAND child-execution path use the smallest modern
session-owned host composition while retaining the original `cmdExec32`
ordering and without importing CCPU, CSR/BaseSrv or process-global standard
handle mutation?

## Inputs

- Imported OpenNT `cmdexec.c`, `cmdmisc.c`, and `cmdredir.c` under
  `src/bx-vdm/bop/opennt/command/`.
- Existing T234 session multisz, opaque 32-bit host-handle manager, PIF and
  current-directory seams.
- Formal module manifest
  `tools/build/t225-s7-full-module-manifest.json` and MSVC x64 `/MT` graph.

## Procedure

1. Compared the excluded historical `cmdCreateProcess` worker with the active
   macro replacement. The historical worker depends on CCPU thread state,
   CSR/BaseSrv event services and temporary process-global standard handles.
2. Removed the `cmdExec32` macro route. Imported `cmdExec32` now retains its
   original block, `GetNextVDMCommand`, carry/AL and cleanup sequence.
3. Added one session-local public-Win32 seam for copied command/environment,
   explicit inherited standard handles, Job ownership, wait, exit result and
   fixed-width state. The seam never exports a `HANDLE` to guest state.
4. Replaced unlabelled event/console no-ops with explicit session state, and
   supplied the documented public-current-directory fallback for the NT4
   hidden `=X:` environment form.
5. Compiled changed imported and shim sources with the formal `/W4 /WX /MT`
   flags, then linked/reran the focused fixtures against the source-current
   formal `bx-vdm` archive.

## Observations

- `cmdCreateProcess` remains excluded: carrying the thread-local checked
  guest-copy call through its historical worker would be unsafe and would
  recreate the unavailable CCPU ownership. This is a source-proven
  composition boundary, not a missing BOP implementation.
- `cmdExec32` is now a compiled imported body, not a preprocessor alias. Its
  only documented divergence is the call to
  `bx_ntvdm_command_local_child_execute` in place of the historical worker.
- `t236-s1-command-local-child-fixture` passed Direct `exit 37`, COMSPEC
  `exit 41`, malformed opaque standard-token rejection, and a session-local
  anonymous stdout pipe (`echo T236`).
- The retained T231 lifecycle fixture passed against the source-current
  formal `bx-vdm` archive after its unrelated current-directory assertion was
  transferred to the dedicated T234 directory fixture. That T234 fixture also
  passed against the same archive.
- Fresh formal graph `D:\tmp\ntdos64-M0-T236-S1-formal-r3` includes the new
  fixture. Its Ninja dry run enumerated the complete 82-edge closure: 79
  `bx-vdm` objects, the fixture object, archive and final link.
- The installed Ninja executor again stalled after starting its `cmd.exe`
  child. This is the established executor limitation, not a compile failure:
  the exact formal `/W4 /WX /MT` commands compiled the changed owner/shim
  objects, and the source-current archive linked and ran all three focused
  fixtures above. No successful full Ninja execution is claimed.

## Interpretation And Confidence

High confidence for the declared serialized local child profile: OpenNT
COMMAND retains the service entry/return semantics while the modern adapter
owns only the unavailable host-product mechanics. This does not claim
Redirector protocol, remote/named pipes, WOW, concurrent-session scheduling,
or guest CPU scheduling.

## Follow-up

The exact Ninja execution limitation remains a build-executor follow-up; its
dry-run graph and source-current focused link/run evidence are sufficient for
this S1 closure.  Redirector and WOW proposals remain the owners of their
explicitly excluded protocol/composition branches.
