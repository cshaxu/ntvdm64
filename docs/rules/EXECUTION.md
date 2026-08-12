# Execution Rules

## Request Lifecycle

1. Read the task reading set in [docs/README.md](../README.md) and re-read the
   owner request.
2. Keep unapproved candidate packages in [QUEUE.md](../QUEUE.md). A candidate
   becomes a numeric T only after owner approval and admission to the one
   active [STATUS.md](../STATUS.md) packet.
3. Record one bounded S brief in Status, select Ordinary or Coordinated
   Dual-Session Mode, and implement only its admitted scope.
4. Gather the stated evidence and run the packet verification.
5. Review the actual diff and every requirement against the owner request.
6. Make a sequential P delivery: update authorities/evidence, commit, push,
   and record any acceptance limitation.

A material scope expansion, changed boundary, or failed stop condition pauses
work. Revise the S brief and obtain renewed owner approval before proceeding.

## MTSP And Td

The binding hierarchy is M<milestone> T<task> S<subtask> P<part>.

- **M** is a major roadmap outcome with a closure audit.
- **T** is an owner-approved, medium-sized package. T numbers are global,
  unpadded, strictly ascending, and assigned only at Status admission.
- **S** is the smallest bounded implementation/review goal within the active
  T. Exactly one S may be active.
- **P** is one sequential, reviewed delivery of an S: its accepted diff,
  committed revision, and pushed repository state.
- **Td** is standalone documentation/governance work. It may use S and P but
  never allocates a numeric T or displaces the active numeric packet.

The current product package is recorded only by the active packet in
`STATUS.md` (currently M0 T176). T95's legacy r1--r23 labels remain
run/evidence iterations, not T, S, or P identifiers. Their classification and
the Td exception are in
[task-identifier governance](../etc/operations/task-identifier-governance.md).

## Active S Packet

Before source, build, governance, or documentation implementation, Status
must contain these fields for the one active S:

| Field | Required record |
| --- | --- |
| Identifier Mode | Exact M/T/S identifier and execution mode. |
| Admission And Approval | Owner request, admission decision, and scope authority. |
| Objective | Observable result. |
| Non-goals | Explicit exclusions. |
| Reference Baseline | Current facts and prior evidence. |
| Files And ABI Surface | Expected owned files and interfaces. |
| Applicable Rules | Authorities that constrain the work. |
| Verification | Commands, reviews, and acceptance checks. |
| Expected Markers | Evidence that demonstrates the objective. |
| Asset Needs | Required source, fixtures, tools, or declared absence. |
| Reporting Requirements | Results, limitations, and out-of-scope disposition. |
| Stop Conditions | Events that require pausing or re-admission. |
| Exit Criteria | Closure facts required before a P. |
| Original Owner Request | Verbatim or faithfully scoped request. |
| Similar-Issue Sweep | Adjacent surfaces checked for the same problem. |

## Execution Modes

Every numeric T selects one mode:

- **Ordinary Mode:** one implementer owns the brief, evidence, verification,
  diff review, and P formation.
- **Coordinated Dual-Session Mode:** the coordinator owns T/S admission,
  replanning, independent worktree review, and P formation. The executor
  independently reviews the S brief, implements only admitted scope, gathers
  evidence, and raises material evidence, scope, or risk objections before
  continuing. The coordinator resolves those objections in Status before P.

Both modes retain one active S, the same evidence bar, and an independent
review of the final changed files. A coordinator cannot use an executor's run
log as a substitute for this review.

## Evidence, Verification, And Closure

Every investigation records exact inputs, procedure/commands, observed output,
interpretation, confidence, and follow-up in etc/ or an appropriate history/
record. A failed or unavailable check is evidence with a limitation; it is
never a passing result.

For every P or Td closure:

1. Re-read the owner request and map each requirement to changed files and
   evidence.
2. Inspect the actual worktree diff; run git diff --check and focused boundary
   checks.
3. For documentation work, run
   powershell -ExecutionPolicy Bypass -File tools/Verify-DocumentationGovernance.ps1.
4. Update Status, Queue, history, and supporting evidence according to their
   separate roles. Record out-of-scope findings.
5. Commit the reviewed result and push it. If no remote or push is available,
   report the local commit as pending delivery; it is not a fully accepted P
   unless the owner grants and the record states a bounded deferral.

## Third-Party Runtime Backend

Before Bochs code enters an internal research target, the active packet must
record the exact Bochs 2.6 origin, source identity, local patch series, enabled
feature set, build recipe, and boundary tests. Preserve upstream notices with
the source; distribution and license review are deferred until release work.
A source tree labelled only by directory name or newer upstream snapshot is
not an admitted Bochs 2.6 input. The feature manifest is deny-by-default:
every enabled feature identifies its reached OpenNT caller, Bochs owner,
boundary rationale, and negative test.

## Toolchain Islands And ABI Admission

Build ownership follows source ownership; this repository is not one compiler
or CRT graph.

| Island | Required toolchain | Permitted responsibility |
| --- | --- | --- |
| runner/, bx-ntvdm-adapter/, modern tests and tools | MinGW-w64 GCC with the owned CMake/Ninja build | Modern CLI, diagnostics, typed bridge code, and fixtures. |
| bochs/ | MSVC with the documented Bochs 2.6 Win32 recipe | Separately built, minimal guest-machine backend only. |
| OpenNT historical guest/service source and narrow overlays | Matching historical Microsoft toolchain or explicitly evidenced compatible island | Original guest/service ownership; never a direct dependency of the modern runner graph. |

Each island produces and verifies its own artifacts. Cross-island integration
uses only a versioned C ABI of fixed-width copied values, explicit status
codes, and checked guest-memory address/length records. It never passes C++
objects, STL types, exceptions, raw function pointers, host pointers,
CRT-owned memory, FILE*, or implicit Windows-handle ownership. Before a new
bridge or artifact is admitted, the active packet records toolchain,
architecture, CRT, build command, artifact hash, ABI surface, dependency
direction, and positive/negative boundary tests.
