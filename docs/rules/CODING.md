# Coding Rules

## Source-first implementation

- Attempt and record recovery in this order: directly composable original
  source; smallest same-shaped adapter/build shim; registered mirror overlay
  or adopted-code intrusion; newly authored behavior as the last resort.
- Audit same-owner current and quarantined project code before authoring a new
  mechanic. Selectively reuse/copy only a per-file reviewed candidate that
  satisfies final component ownership, dependency direction and mirror rules;
  never bulk-copy an old component or revive a rejected semantic path.
- Preserve original names, parameters, calling convention, structures, control
  order and failure semantics. A modern API supplies a dependency; it does not
  authorize bypassing an available original algorithm.
- Recover package edges before individual symbols. An external OpenNT package
  is not imported merely because a header or unresolved symbol mentions it:
  first audit its complete source/build boundary, select only its necessary
  original file/interface slice, record all outbound interfaces, the
  finite adapter/public-API closure, final file disposition, and any current
  project-owned implementation that it would supersede. Do not recurse into
  CSR/CSRSS, NTDLL CSR transport, kernel VDM, full BaseClient/Kernel32,
  Win32k, or USER/GDI server merely to satisfy an import.
- Every project-defined replacement interface records the unavailable
  dependency, rejected earlier rungs, smallest new ABI, failure contract,
  focused test and disposition.

## Placement and dependencies

- Place each production file in one of the architecture roots declared by the
  architecture rules. `mvdm-host`, `opennt-host`, `mvdm-tools` and
  `mvdm-softpc-firmware` preserve selected MVDM runtime (including its
  original support paths), accepted non-MVDM OpenNT host, standalone-tool and firmware-input package
  topology respectively; package libraries do not create ad-hoc new source
  owners.
- `mvdm-softpc-patch` is reserved for individually provenance-registered
  NTVDMx64-derived SoftPC patch bodies. Preserve each original SoftPC interface
  shape and use `adapter-mvdm-host-out/softpc` for an approved mapping-manager
  binding; it is never a generic helper, Bochs consumer or session owner.
- A newly accepted non-MVDM OpenNT package keeps its original relative path
  below `opennt-host`. Its private historical boundary, if one is needed,
  belongs below the correspondingly named `adapter-opennt-host` subfamily;
  neither root may be used as a generic helper collection.
- `mvdm-platform-abi` contains exact original declarations only.
- `session` contains neutral per-instance mechanisms. `broker` contains only
  cross-process registration/queue/notification mechanics. `app` alone wires
  instances and product policy.
- Do not add a generic compatibility/common component. Use the declared owner
  or obtain explicit architecture admission for a source-proven specialist
  interface family.
- Import directly composable MVDM translation units before changing their
  reached function bodies. Every missing external function/interface records
  the original caller and declaration, ABI/layout and failure contract, its
  single adapter owner, the binding change, and current x86 evidence. x64
  compatibility debt is recorded but does not block the recovery path. A missing
  boundary never justifies cropping or rewriting the original algorithm.
- In MVDM source-function BFS, include every original `mvdm-host` definition
  at zero-degree, then retain its transitive resolved call closure while each
  callee's physical original definition remains in selected OpenNT `mvdm`
  source and already has a project `mvdm-*` mirror. Do not classify unreachable
  local MVDM definitions as zero, and never infer membership from a bare symbol
  spelling.

## Original mirrors

- An imported mirror file has exactly one classification: byte-exact original;
  true original subset; or same-shaped minimal original/subset modification.
- Each deleted logical block has a `DIVERGENCE:` crop marker and README row.
  Each changed expression has a local `DIVERGENCE:` explanation and README
  row identifying original purpose, reason and implementation.
- If more than 50 percent of retained source/preprocessor/data lines differ,
  the implementation is not a mirror file. Move it to the matching overlay
  and retain the smallest registered call boundary.
- If added semantics require more than three executable lines at an imported
  insertion point, place the body in the matching private overlay.
- A `*-overlay` is callable only by its matching mirror. It has no standalone
  public ABI and cannot be linked directly by app, adapters, session, broker,
  another mirror or tests.
- Build a mirror and its private overlay as one owner library.  The overlay
  may contain added mechanics but may not duplicate an imported algorithm,
  become the provider selected by an adapter, or acquire a public header.
  Preserve the smallest possible `DIVERGENCE:` hook in the original source;
  the corresponding mirror README registers both the hook and overlay file.
- Keep upstream names and paths after re-rooting. Reference/example/test code
  stays outside production roots.

## Width and mapping

- Use `uint16_t`, `uint32_t` and `uint64_t` for fixed ABI fields. Use
  `uintptr_t`, `size_t`, `HANDLE` and platform-native structures only inside
  process-local implementation.
- Never cast a native pointer or HANDLE into a 32-bit MVDM field. Register
  opaque native identity in the appropriate session mapping-manager instance
  and expose only its surrogate32.
- Use the same mapping path for every active x86 cross-boundary identity. Do
  not identity-map native x86 resources. x64-specific adaptation remains
  deferred, but that deferral never permits native identity pass-through.
- The single manager implementation is instantiated separately for
  `guest_memory`, `host_resource` and `completion_callback`. A typed wrapper
  must select the instance; a bare numeric token is insufficient.
- Allocation tests candidate IDs from zero, skips original ABI sentinels,
  advances monotonically, maintains forward/reverse lookup and retains stale
  tombstones. Do not reuse an ID during the owning session.
- Tokenize identity only. Validate numeric sizes, offsets and address arithmetic
  in a wider temporary type before narrowing; reproduce original overflow and
  failure behavior.
- A guest-memory native pointer exists only under a synchronous checked lease.
  It cannot be serialized, stored in a durable record, passed to broker IPC or
  retained by a worker.
- Translate pointer/HANDLE-bearing OpenNT structures to native process-local forms
  inside the owning adapter, call the public/native API, then translate results
  back.

## Session, threading and broker

- Project-owned stateful APIs take an explicit session or use a bounded
  thread binding established at entry. Do not create a process-global current
  machine, current session, mapping table or resource registry.
- Every worker that calls imported MVDM code binds the owning monitor/session
  context and unbinds it on exit. An asynchronous record contains stable
  session-owned IDs, not a stack/guest/native pointer.
- Imported MVDM process globals may remain for the admitted one-session-per-
  process profile. Do not rewrite them for speculative reentrancy. A future
  in-process multi-session change first inventories and classifies every
  reached global and asynchronous owner.
- Broker IPC uses copied versioned records, explicit sizes, per-user access
  control, stable broker IDs, leases and deterministic disconnect cleanup.
  Never enumerate/control unrelated processes as a discovery mechanism.
- Local surrogate IDs and native resources are process-local and never appear
  in broker messages.

## Guest inputs

- Guest source, resources, build files, `.obj`, `.lib`, `.exe`, `.com`, `.sys`,
  `.bin`, `.dll`, `.fon`, `.sym` and related products remain in their selected
  guest mirror paths and manifests.
- Guest objects and libraries never enter a host compile/link graph. App loads
  only manifest-selected immutable bytes.

## Naming

- Production names describe current semantics. Do not use obsolete project or
  component names, `v1`, `v2`, task-part labels or `legacy` for a current ABI.
- Preserve an upstream spelling when it is part of imported source identity;
  register any unavoidable conflict. `legacy` is acceptable only for archived
  evidence, an immutable external name or prose describing a historical API.

## Build and evidence hygiene
- The current recovery build is MSVC Win32/x86 `/MT` and every reached SoftPC
  behavior must compile and run under the selected original `CPU_40_STYLE` /
  CCPU40 configuration. `CPU_30_STYLE` is an NT4 kernel-VDM V86-monitor
  contract, not a CCPU execution profile; it is retired and prohibited from
  every project-owned production or fixture compilation, linking, runtime and
  acceptance input. Its remaining upstream-mirror conditionals and historical
  evidence never authorize a CPU30 build row.
  x64 compatibility is recorded as deferred debt and is not a current gate.
  Do not link objects of different architecture or CRT into one process.
- For a functional fixture that reaches machine execution, guest state or an
  imported MVDM-host behavior, record the selected SoftPC CCPU40 row.
  An unavailable result is passing only when the fixture asserts that exact
  source-shaped unavailable outcome; it may not be silently omitted.
- Generate the Ninja graph from audited owner/package manifests and dependency
  files. A configuration hash change invalidates the relevant graph/objects.
- Put all disposable compiler, linker, generated and debug output under
  `build/<task-id>/<run-id>/`. Do not emit it into the repository root, `src/`,
  `tests/`, `tools/` or `artifacts/`.
- Put concise tracked evidence, inventories and manifests in `docs/etc/`.
  `artifacts/` is reserved for owner-requested reports and formally manifested
  deliverables.
- Every source/import change records provenance, exact owner, build-selection
  state, focused positive/negative verification and unsupported behavior.
- Preserve unrelated worktree changes. Use `git mv` for an admitted pure move
  and never treat `src.old/` as an implementation input.
