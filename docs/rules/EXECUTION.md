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

### Historical Recovery Audit Gate

Before admitting or changing a recovered OpenNT, guest, or host-service
behavior, the active S evidence must classify the proposed implementation
against this mandatory order: (1) directly composable original OpenNT source;
(2) that source composed through the smallest adapter, compatibility seam, or
build shim; (3) a registered exceptional intrusion into adopted external
code; (4) newly authored behavior only when no usable historical source
remains.  The record names the original owner and source path, preserves or
explicitly accounts for layout, ordering, and failure behavior, and explains
why every earlier applicable level cannot compose.  A CLI product requirement,
the Bochs-for-SoftPC substitution, approved host-mutation modes, or the
unpack-and-run host non-intrusion rule may constrain a decision only when the
owner has explicitly recorded that exception.  Convenience, a trace hit, or a
temporary capacity limit is never a reason to skip a level.

The audit must record the disposition of every rung, including whether the
original translation unit was actually attempted or is excluded by a named
unavailable historical dependency.  It must identify the smallest retained
source fragment or compatibility seam before proposing a rehost, and must
record why a registered external-code intrusion or newly authored behavior is
unavoidable.  A passing fixture and the availability of a convenient modern
Win32 API validate a selected seam; neither substitutes for this source-first
analysis.

### Source-Recovery Audit Criterion

Every recovered behavior must carry a reviewable four-rung ledger: **original
source reuse**, **smallest adapter/shim retaining that source contract**,
**registered external-code intrusion**, then **newly authored behavior**.  The
first usable rung is mandatory.  The ledger must cite the original owner and
paths; state whether its translation unit was practically composable; identify
the exact historical ABI, ordering, data layout, and failure rule retained; and
name the concrete dependency that prevents every earlier rejected rung.  The
only permitted policy constraints are an explicitly approved CLI requirement,
Bochs-for-SoftPC substitution, one of the approved host-mutation modes, or
the unpack-and-run rule that forbids changing the host merely to install this
product.  A supported Win32 API is normally a capability behind the selected
OpenNT owner—it is not authority to replace that owner with a new adapter
policy.

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
| CLI, bx-vdm, bx-mantle, bx-core and in-process modern fixtures | MSVC x64 with `/MT` | One modern runtime ABI/CRT: CLI, diagnostics, typed bridge code, provider composition and minimal guest-machine backend. |
| PowerShell tools and retained cross-toolchain evidence | Their recorded host/toolchain | Inspection, historical evidence and non-runtime probes only; they never supply an object to the MSVC x64 process. |
| OpenNT historical guest/service source and narrow overlays | Matching historical Microsoft toolchain or explicitly evidenced compatible island | Original guest/service ownership; never a direct dependency of the modern runner graph. |

Each island produces and verifies its own artifacts. Cross-island integration
uses only a versioned C ABI of fixed-width copied values, explicit status
codes, and checked guest-memory address/length records. It never passes C++
objects, STL types, exceptions, raw function pointers, host pointers,
CRT-owned memory, FILE*, or implicit Windows-handle ownership. Before a new
bridge or artifact is admitted, the active packet records toolchain,
architecture, CRT, build command, artifact hash, ABI surface, dependency
direction, and positive/negative boundary tests.
