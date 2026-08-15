# T210 S3 — host-context family regression and native observation

## Question

Does one real CLI-admitted host context supply the complete reached DEM and
COMMAND context family, and what does the existing native runtime observe
after that source-built family is available?

## Inputs and procedure

The new x64 `/MT` fixture
`tools/Invoke-T210S3HostContextFamilyProbe.ps1` captures the current process
drive once as an explicit admitted root, creates one rooted namespace and one
shared session context, then performs:

1. DEM `50:18` set current directory;
2. DEM `50:1A` set default drive with matching `DL`;
3. DEM `50:13` query/reconcile with CDS repair output;
4. projection to COMMAND `54:04` current-directory response;
5. COMMAND `54:02` COMSPEC bootstrap; and
6. COMMAND `54:0F` environment publication.

The exact r1 invocation passed at
`artifacts/build/t210-s3-host-context-family-r1`. It uses ordinary user-mode
host CWD integration, restores the test process CWD, and neither executes a
guest nor imports a Bochs object.

After the package regressions passed, the pre-existing native observer was
run once through `Invoke-T95S7RuntimeTraceObservation.ps1` with a fresh
`artifacts/evidence/t210-s3-native-observation-r1` root. Its process record
was interrupted by the host's `taskkill` access-denied response before the
script could write `observation.json`; the retained Bochs log is nevertheless
direct evidence. It reports `requested execution plan rejected before CPU
loop`, then `quit_sim ... exit code 1`. There are no adapter BOP observations
and no transaction commits.

## Interpretation

The source-built package passes its full reached context family. The native
non-observation predates CPU execution and therefore neither validates nor
contradicts any individual DEM/COMMAND BOP. It is an existing engine/startup
admission limitation, outside T210's host-context owner boundary. The result
does not authorize a trace-driven endpoint repair.

## T210 completion

T210 closes the shared host-context owner package:

- One session provider owns selected drive, CWD, projected environment and
  common profile enforcement.
- DEM `13/18/1A` and COMMAND `02/04/0F` consume that one state.
- Direct uses public user-mode Win32 CWD operations; readonly, overlay and
  virtual remain common-profile modes.
- The duplicate DEM CWD store and COMMAND synthetic-root workaround are
  removed from the active route.

It does not claim native CPU execution, full COMMAND recovery, or general BOP
completion.
