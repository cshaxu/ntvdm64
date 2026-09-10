# Core Delta Handoff Record

Status: M37 governance handoff, 2026-08-08. This is a research handoff for a
separately governed core repository. It is not an implementation request,
source change, or dependency declaration.

## Capability A: Configured Decode-Point Transition

**Purpose:** permit a composition layer to observe and, under strict core
validation, handle a registered guest byte pattern at a defined pre-execution
decode point.

**Existing foundation:** the current undefined-instruction transition already
uses copied state, finite dispositions, core-validated instruction consumption,
and restricted GPR/FLAGS output. It is intentionally limited to real-mode
`#UD` and is retained unchanged.

**Neutral invariants:**

- registration accepts a bounded byte pattern only during configuration and is
  frozen with the machine topology;
- the core identifies the physical fetch location and supplies copied bytes;
- registration conflicts and prefix ambiguity are rejected deterministically;
- a consumer receives copied state, including the segment/address context
  needed to request normal core translation, never an executor or RAM pointer;
- possible results are only unhandled, handled-resume, stop, and fault;
- core alone validates consumed bytes and atomically applies permitted
  GPR/FLAGS changes;
- no result may write CPU mode, control registers, segment cache, descriptor
  tables, mappings, topology, IRQ state, or a host object.

**Not in scope:** instruction emulation framework, arbitrary decoder callback,
firmware portal, operating-system ABI, profile command, or product policy.

**Consumer gate:** no independent consumer is currently evidenced. The
historical external adapter is one candidate, not sufficient proof for core
promotion. A second product or an existing core-owned scenario must state the
same contract without external protocol vocabulary.

## Capability B: Atomic Ordinary-RAM Patch Transaction

**Purpose:** publish one or more already-translated copied byte spans to
ordinary guest RAM at a stopped or paused boundary, with all-or-none semantics.

**Existing foundation:** checked physical `read`, `write`, and `query`, frozen
provider routing, and initial-entry preload validation already exist. The entry
plan is initial-state composition, not a general runtime transaction.

**Neutral invariants:**

- input spans are copied bytes and checked physical addresses; no caller-owned
  backing pointer or provider callback is exposed;
- all spans are validated through the normal frozen mapping route before any
  mutation; each must be writable ordinary RAM;
- overflow, inaccessible ranges, provider/ROM routes, and conflicting aliases
  fail before mutation; identical overlap bytes have deterministic meaning;
- commit publishes all normalized spans or none, including an injected
  publication failure case;
- transaction handles are scoped to one stopped/paused boundary and become
  invalid on resume, reset, stop, fault, destruction, or context change;
- core knows neither why bytes are written nor any path, file, protocol,
  command, or device identity.

**Not in scope:** DMA, RAM remapping, mutable provider registration, generic
snapshot/restore, host filesystem I/O, or a product-specific memory editor.

**Consumer gate:** two observed uses occur within one external adapter family.
They establish correctness pressure but not a second product consumer. An
independent core, VM, or NXVDM consumer is required before promotion unless a
core owner explicitly establishes broader generic reuse.

## Handoff Decision

Both capabilities are valid neutral candidates, but both remain **research
requests pending consumer evidence**. The adapter project will not implement
them locally, approximate them through serial writes, or add an execution
backend. It will continue to preserve historical source, artifacts, and host
boundary evidence until the core side has an independently admitted path.

## Acceptance Evidence For Any Future Core Task

1. A consumer-neutral API and lifecycle contract with no external adapter
   identifiers.
2. Direct tests for registration freeze/conflict, illegal patch rejection,
   result atomicity, stale-boundary rejection, and no CPU-mode change.
3. A second consumer using each capability without importing external adapter
   headers or profile policy.
4. Regression proof that retained VM startup and normal instruction execution
   do not change when no capability is registered.
5. A boundary audit proving core exposes no RAM pointer, path, host handle,
   protocol selector, or product session policy.
