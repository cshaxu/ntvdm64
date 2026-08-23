# Coding Rules

- Historical recovery is mandatory source-first engineering, not merely an
  evidence preference. For every recovered OpenNT/guest/host behavior, attempt
  and record this order: (1) directly composable original source; (2) the
  smallest adapter, compatibility seam, wrapper translation unit, or build
  shim which preserves that source contract; (3) a registered intrusion into
  adopted external code; (4) newly authored behavior only if no usable source
  remains. This is a required implementation order: do not recast an available
  original routine as new code merely because a local transcription is easier.
  Do not select a later rung for convenience, a trace hit, a modern API, or a
  temporary capacity limit. Name the owner/source/dependencies and account for
  layout, ordering, and failure behavior at every rejected rung.
- Treat the four-rung source-recovery audit as an implementation gate: record
  original-source reuse, then the smallest contract-preserving adapter/shim,
  then a registered external-code intrusion, and only last new behavior.  A
  modern Win32 API may supply a selected OpenNT owner's host capability, but
  never authorizes bypassing that owner's algorithm or failure contract.
- For rung-2 recovery, prefer an **OpenNT-shaped compatibility facade** over
  rewriting an imported OpenNT body around project-authored APIs.  Keep the
  original call/macro/structure form in the mirror and provide a named
  compatibility header that maps it to bounded, session-owned, internally
  namespaced implementation.  Every changed line in an imported source file
  must carry `DIVERGENCE:` with the unavailable historical dependency and the
  reason the facade cannot preserve that exact expression.  Do not call a
  self-authored provider “recovered” or “code complete” while the original
  translation unit is available but has not been composed through this form.
- Same spelling and parameters do not prove a safe compatibility replacement.
  A facade must be justified by the reached original call site, ABI layout,
  ordering and failure behavior. Historical guest-pointer APIs may retain
  their exact pointer-returning shape only through the one `bx-vdm`
  session-owned mapping manager: every mapping has a checked address/span,
  access rule and active-call epoch; its native pointer never crosses a
  component ABI, enters guest state or asynchronous work. A facade must not
  recreate an unbounded CCPU/SAS, CSR/BaseSrv, NTVDM or emulator product shell.
  Existing non-mirror providers are migration debt: classify them as retain as
  generic mechanics, migrate behind the facade, replace with the mirror, or
  delete when their owner package is next changed.
- A project-defined interface, data layout or algorithm is allowed only as a
  registered recovery exception. Its admission records the failed direct and
  same-shaped-facade attempts, exact unavailable dependency, smallest new ABI,
  source-derived observable behavior, negative/failure contract, focused test
  and planned migrate/retain/delete disposition. Without that record, an
  independently implemented routine must use the original interface shape and
  reproduce the reached original control/data/failure logic as closely as the
  current platform permits.
- Minimize project-defined replacement interfaces. Prefer an existing public
  Win32 API when it preserves the reached OpenNT contract; otherwise keep the
  unavailable Win32/CCPU/SoftPC call's original signature and call shape in a
  facade. A newly shaped interface is permitted only under the registered
  exception above, must expose no more state than the reached call requires,
  and must not be introduced merely to make a local implementation easier.
- Keep authored runner code, overlays, host seams, imported material, generated
  files, and fixtures visibly separate.
- Make the smallest evidence-backed change. Preserve calling conventions,
  layout, segment assumptions, dispatch order, and error propagation.
- Each change to historical material records its source path, rationale, and
  focused verification. Unsupported cases fail explicitly.
- Every disposable build tree, object, archive, generated project file,
  temporary executable and debug log belongs under the ignored repository root
  `build/<task-id>/<run-id>/`. Do not emit compiler or linker products into the
  repository root, `src/`, `tests/`, `tools/`, or `artifacts/`.
- `docs/etc/` is the Git-tracked home for evidence, inventories, manifests,
  hashes, operational records and research conclusions. `artifacts/` is never
  a scratch area and may contain only a report explicitly requested by the
  owner, or an explicitly approved formal deliverable below
  `artifacts/build/<task-id>-<version>/`. The latter must include a manifest
  with task identity, version, source revision, toolchain, architecture, CRT,
  input hashes and the exact disposable build recipe. Do not create an
  `artifacts/evidence`, `artifacts/analysis`, `artifacts/logs`, or
  `artifacts/inputs` tree.
- Before a build starts, create a new `build/<task-id>/<run-id>/` or prove by
  manifest that an existing directory is reusable. Delete the preceding run's
  non-reusable objects, generated files, executables and logs before the next
  build; never accumulate retry roots as retained evidence. After a debug
  problem is resolved, retain only the concise evidence needed to reproduce
  the conclusion in `docs/etc/`, then delete the raw runtime/debug logs from
  `build/`.
- Tools are tracked source and must have one declared responsibility. Place new
  tools below `tools/build/`, `tools/governance/`, `tools/import/`,
  `tools/probe/`, or `tools/historical/`; the directory README states its
  target inputs, outputs, owner and whether it may create a disposable
  `build/` tree. Do not place generated output beside a tool. Existing
  top-level tools are migration debt, not a precedent.
- Keep adopted Bochs upstream files and license notices intact under `src/bx-core/`.
  Put project code in `src/bx-mantle/` or `src/bx-vdm/` by ownership; any exception is an explicit patch
  with upstream path/revision, rationale and focused test, registered in
  `docs/etc/research/adapter-external-intrusion-exceptions.md` before it is
  applied. Avoid such exceptions; admit one only when an adapter/build wrapper
  cannot solve the problem. Distribution review is deferred until a release is
  considered.
- The adapter may translate typed machine events and checked memory ranges
  only. It may additionally identify a BOP selector and catalogued service ID
  from the already copied bounded instruction window, solely to select an
  explicitly source-admitted adapter contract or record an unimplemented
  invocation. It must not use unbounded guest-memory inspection, emulate DOS
  interrupts, own device policy, or turn the catalogue into a generic
  historical dispatcher.
- Keep Bochs feature selection deny-by-default. A configuration addition must
  name its reached OpenNT caller, the Bochs owner, the boundary impact, and a
  negative test; do not enable a feature merely because upstream provides it.
- Do not move historical OpenNT service behavior into Bochs or the adapter.
  Preserve the original owner's calling convention, layout, dispatch order,
  and failure semantics; unsupported cases fail explicitly.
- Do not move Bochs CPU, exception, memory, firmware, device, configuration,
  or other machine semantics into NTVDM/OpenNT code or the adapter. The
  boundary transports typed facts; it does not duplicate either side.
- Build the in-process M0 runtime with MSVC x64 and `/MT` only. A MinGW,
  alternative-CRT, or differently-architected object cannot enter that link;
  communicate only through a separately admitted fixed process ABI, never by
  satisfying linker symbols across CRT boundaries.
