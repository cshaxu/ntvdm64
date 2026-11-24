# T484 S3 P189: FORCEDOS and memory transition producers

Both focused regressions retain their existing case selection, guest assertions
and cleanup. FORCEDOS uses two immutable authored templates: ordinary DOS stub,
and the nested parent stub; the child reuses the ordinary template.

The memory-transition producer includes interrupt16, interrupt32, code32,
task-cleanup, forced XMS relocation, and both DPMI cleanup children. EMS call
return reuses the already-declared CPU integration output. Its PIF builder and
normal-user environment reader also reuse existing native targets, removing
inline CL and external C# compilation. The unused LogonHelper argument is
removed; no current repository caller passes it to this runner. Historical
commands remain historical evidence, not current invocation instructions.

Source hashes and diagnostic macro metadata remain in the runtime report.
The native environment query uses the existing checked reader; no environment
values are written to evidence. No original guest or product runtime changed.

## Verification

`build/t484-s3-transition-probes/run.ps1`, PID2332, exit0, proves:

- Ten real authored outputs exactly match previous source/define commands.
- Repeated CMD builds preserve their output modification times.
- The native logon reader successfully returns45 entries including SystemRoot;
  only the count is recorded.
- Both caller scripts parse, with no remaining NASM/Add-Type/inline CL invocation.

After removal of the unused parameter, final script parsing is also covered by
the closure gate. `images.json` records the compared image hashes. The old
commands are comparison oracles only. No new guest runtime acceptance is
claimed, and shared staging/configuration remains unfinished.

The cached full build and11 raw/22 published hash checks are recorded in
`delivery-result.json`; unchanged products retain their existing publication.
Final governance/parser checks are recorded by `closure-result.json`.

The fixed census remains316/355 disposed,39 pending. Of the18 additional NASM
scripts,16 producer sites are handled and two remain. Seventeen supplemental
caller chains still need shared runtime preparation reconciliation. S3 is open.
