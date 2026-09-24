# T422 S2 reproducible run evidence protocol

## Scope and authority

Owner requested this protocol on 2026-09-24 after the E26/E27 investigation.
It implements the active S2 reporting requirements under
[Execution Rules](../../rules/EXECUTION.md); it does not admit another task,
change guest media, or redefine functional acceptance. The
[S2 ledger](../evidence/m0-t422-s2-user-client-lifecycle-ledger.md) remains the
capability/evidence record. This is the operational procedure, not a second
status document or a claim that capture automation already exists.

## Identity and storage

- Allocate a never-reused `run_id`, for example
  `t422-s2-20260924T183012123Z-<random8>-wndproc-roundtrip`. Timestamp is UTC;
  reject an existing destination rather than merging with it. Every retry,
  A/B branch and debugger run gets its own ID and parent/comparison IDs.
- Builds, frozen build-input snapshots, generated graphs, link maps, symbols,
  build logs and sealed output sets belong below `build/M0-T422/S2/<run_id>/`.
  A mutable incremental build directory is a cache, never a historical ID.
- Runtime manifests, observations, stdout/stderr, traces, screenshots and
  exception logs belong below `O:/winnt/logs/<run_id>/`. Test executables and
  fixtures belong below `O:/winnt/tests/`; no test output at the package root.
- A retained runnable package may use `O:/winnt/builds/<run_id>/`. This path
  is an evidence location, never a hard-coded production discovery path.
- Tracked ledger entries retain the manifest digest, concise exact excerpts,
  artifact/source identities, conclusions and replay recipe. Do not import
  raw logs or build trees into docs, or create a document inventory.
- Never redirect a new run to a previously cited log. For a harness with fixed
  filenames, first preserve and hash the old files, give the new invocation
  exclusive ownership of that log namespace, and seal its files before any
  subsequent invocation. Otherwise label it incomplete, not reproducible.

## Required record before execution

Create `manifest.json` in the run's log directory with these required groups.
Use explicit `not-applicable` with a reason instead of silently omitting a
field. Unknown required identity means evidence is incomplete.

| Group | Required contents |
| --- | --- |
| Question | Run ID, UTC creation time, owner, ledger row IDs, narrowly stated question, expected positive/negative observations, timeout, baseline run ID and intentional differences. |
| Source | Git HEAD, branch, dirty status, retained binary diff against HEAD including staged changes, exact untracked build/test input bytes, deleted-file list, input path/size/SHA-256 manifest and snapshot location. HEAD or a diff alone is insufficient for a dirty tree. |
| Build | Exact generator/build commands and working directories; compiler/linker/Ninja/SDK identities, x86 CCPU40 and CRT flags, generated graph/input hashes, link/import-library pairing, build exit results and logs. A reused artifact references its sealed build record. |
| Package | Each deployed EXE/DLL path, size, SHA-256, PE machine, matching MAP/PDB where available; include run16, basesrv, ntvdm, WOW32, VDMREDIR and dtmgr if deployed. Validate nonzero MZ/PE/x86 before launch. Record deployed hashes, not just build-output hashes. |
| Inputs | Guest executable and dependency/media manifest hashes; exact package/profile files including SYSTEM.INI and NTVDM.REG as applicable; before/test/after hashes for temporary profile changes. Missing-media and no-driver branches are distinct runs. |
| Conditions | Exact command/arguments, cwd, package root, guest path, host OS build, locale/code page, Console/Terminal/RDP context when relevant, debugger attachment, enabled trace flags, environment profile and all behavior-affecting overrides. A reduced environment is diagnostic, not the ordinary product profile. |
| Observer | Test script/source and observer executable hashes, commands, action sequence, predicates and timeout policy. Distinguish an observer error from a product failure. |

Do not collect or commit credentials, tokens or unrelated personal environment
values. Record the relevant environment allowlist and explicit unset variables;
note redactions or omitted dependencies. A secret-dependent run without a safe
replay recipe cannot be called fully reproducible. No full environment dump
belongs in a tracked ledger.

Build from frozen inputs, or ensure the build owner excludes concurrent edits
to all selected inputs during capture/build. Record before/after input hashes;
an uncontrolled concurrent build is incomplete evidence. Do not reset, stash,
or overwrite the main worktree to manufacture a clean source snapshot.

## Observation and sealing

1. Verify the baseline and input manifests before deployment. Do not replace a
   running worker's files or mistake an old broker/provider for this run.
2. Record launcher/broker/worker PID plus creation time, executable path/hash,
   module load base and provider identity. PID alone is not durable identity.
3. Save raw observations and action timestamps while running, including the
   first exception and address/RVA, not just a later error-handler crash.
   Trace loss, truncation, unavailable streams and timeout are explicit facts.
4. For windows, record HWND, owning PID/TID, class, title, visibility, hung
   state and observation time; revalidate before sending an action. Distinguish
   WOWExec/Fax helper windows, application windows and host ghost/error windows.
   Preserve a screenshot/content observation for a visible-UI milestone.
5. For callbacks, distinguish the encoded WNDPROC, decoded guest selector:offset
   and native pointer. Record message, entry, return success/result and relevant
   frame/descriptor evidence. A tag is not a guest selector.
6. Record close/destroy/task retirement/worker and launcher completion
   independently, with exit codes, residual processes and test-forced kills.
   Restore permitted temporary configuration and verify original hashes.
7. Write `result.json` with per-check verdicts, UTC end time, observation links,
   actual cleanup and limitations. Hash all retained files into
   `checksums.sha256`, excluding that checksum file itself. Mark the run sealed
   only after manifest, logs, artifacts and result agree. Never edit sealed
   evidence; corrections use a new note/run referencing the original digest.
8. Before another test reuses mutable build/deployment/log locations, preserve
   the sealed source and artifact set for any first deeper milestone, last
   reproducible baseline, causal A/B pair or unique failure. Preserve symbols
   with binaries. Retention requires actual bytes, not just paths and hashes.

An interrupted run gets `INCOMPLETE` and retains what exists; a later retry
cannot overwrite it. A normal exit code, a BOP hit, a non-null slot or an HWND
alone is not a functional pass.

## Verdicts, causality and report template

Each check uses `PASS`, `FAIL`, `NOT-RUN`, `INCOMPLETE`, or `NOT-APPLICABLE`
with a reason. Separately label the evidence kind: source, build, mock/fixture,
diagnostic guest, or ordinary-product guest. Passing a fixture does not pass
its guest row. An overall lifecycle pass requires every mandatory predicate;
window existence and task termination cannot substitute for guest interaction
or successful destruction callbacks.

Use this compact entry in the existing S2 ledger, not a Markdown file per run:

```text
Run ID / UTC:
Question / capability IDs:
Baseline run / controlled change:
Source snapshot / artifact manifest SHA-256 / evidence locations:
Command / profile / observer:
Facts: exact log excerpts with file and line/event references:
Per-check verdicts: creation / callback-return / content / interaction /
  destruction / task-cleanup / worker-cleanup / DOS-regression:
Interpretation and confidence (separate from facts):
Counterevidence / untested alternatives / replay limitations:
Next action / supersedes which interpretation (not which raw evidence):
```

A causal regression/fix claim names both run IDs and compares source,
artifacts, guest media, configuration, environment and observer. Prefer one
semantic change and repeat the decisive result; multiple changed variables
must remain declared confounders. Source/disassembly can prove an invalid
transformation without a complete historical replay, but cannot prove that
every old observation had that one cause. Preserve both sides before changing
more code. Failed causal controls are retained, not erased by a passing retry.

At each new conclusion update the ledger's leading closure register as well
as appending chronology. Mark withdrawn interpretations explicitly and link
the correction; do not leave an obsolete explanation as the current blocker.
Reports must name the exact passing capability, not simply say "runs deeper".

## Historical transition and retention

E21--E26 record real partial observations, not whole-application acceptance.
E27's duplicate-encoding interpretation was corrected by E41. Existing `rN`
directories and fixed-name logs are historical candidates, not immutable run
identities. Missing old source/config/log bytes are labelled incomplete; never
backfill guessed values or silently assign today's hashes to an old run.

The next S2 verification invocation starts the new run-ID procedure. An
already-running invocation may finish undisturbed but must be marked incomplete
where its evidence cannot meet this protocol. Adoption does not claim previous
runs have been retroactively sealed. Capture may be manual until harnesses
support it; missing capture prevents acceptance, not honest diagnostic work.

Retain sealed milestone, comparison and failure evidence through T422 owner
acceptance and until an explicit retention decision. Hash-deduplicated storage
is allowed only with verified existing bytes and stable manifest references.
No automatic build cleanup may delete the sole copy of a retained baseline.
Local hashes detect changes but do not protect against disk loss; loss must be
reported, and an external backup is a separate operation, not claimed here.
