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
  compatibility header that maps it to a bounded, session-owned, isolated
  implementation.  Where mapping is required, use the applicable fixed
  mapping-manager instance rather than a shared numeric namespace. Every
  changed line in an imported source file
  must carry `DIVERGENCE:` with the unavailable historical dependency and the
  reason the facade cannot preserve that exact expression.  Do not call a
  self-authored provider “recovered” or “code complete” while the original
  translation unit is available but has not been composed through this form.
- Same spelling and parameters do not prove a safe compatibility replacement.
  A facade must be justified by the reached original call site, ABI layout,
  ordering and failure behavior. Historical guest-pointer APIs may retain
  their exact pointer-returning shape only through the `adapter-softpc`
  facade using the applicable `session`-owned mapping-manager instance: every mapping has a checked address/span,
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
- Keep adopted Bochs upstream files and license notices intact under `src/bochs-core/`.
  Put Bochs-only project code in `src/adapter-bochs/`; place every other project
  source in its declared target owner rather than the transitional legacy tree.
  Any exception is an explicit patch
  with upstream path/revision, rationale and focused test, registered in
  `docs/etc/research/adapter-external-intrusion-exceptions.md` before it is
  applied. Avoid such exceptions; admit one only when an adapter/build wrapper
  cannot solve the problem. Distribution review is deferred until a release is
  considered.
- Place each source file in its named target component: Bochs mechanics in
  `bochs-core`; Bochs-only native assembly in `adapter-bochs`; guest image source in
  `opennt-guest`; independently composable OpenNT host capability in
  `opennt-host`; original SoftPC firmware/ROM/machine-contract inputs in
  `opennt-softpc`; selected original reusable utility packages in
  `opennt-utils`; OpenNT BOP mirrors in `opennt-bop`; unavailable public-Win32
  facades in `adapter-win32`; selector-blind copied-frame BOP ingress and
  typed completion in `adapter-bop`; bounded same-shaped SoftPC/CCPU recovery
  in `adapter-softpc`; dependency-free per-VDM lifecycle/resource/event state
  in `session`; and
  CLI/final composition in `app`. Transitional legacy paths are not a
  precedent for new code. Do not move a file across these owners as an
  incidental feature change; the admitted reorganization package inventories
  it and uses `git mv` whenever ownership is pure.
- The only original-code components are `bochs-core`, `opennt-guest`,
  `opennt-bop`, `opennt-host`, `opennt-softpc`, and `opennt-utils`; every imported-body change is individually
  registered in that component's README and marked `DIVERGENCE:` locally.
  `adapter-bochs`, `adapter-bop`, `adapter-softpc`, and `adapter-win32` are
  mechanical-adaptation components. `app` and `session` are project-authored
  composition components, but `session` remains dependency-free and neutral.
  `adapter-bop` may transfer an opaque machine event only: `bochs-core` must not
  identify a BOP or call an OpenNT route directly.
- **Mirror-component review standard.** Every production file in an
  original-code mirror component has exactly one of the following audited
  classifications:
  1. **Exact original:** its complete content is byte-for-byte the identified
     upstream file. No local exception is needed.
  2. **True original subset:** the retained content is a true subset of one
     identified upstream file. Every deleted logical block is represented at
     the crop boundary by a `DIVERGENCE:` comment and by one README register
     row stating the original purpose, why the block is presently unconnected,
     and its future owner/disposition. A crop may remove only a complete,
     independently unreachable logical block and its dedicated commentary; it
     must not bisect retained control flow or silently alter a retained
     failure path.
  3. **Same-shaped original/subset modification:** the imported file or its
     true subset retains the original function names, parameter ABI, return
     ABI and observable reached semantics. Every deletion and changed
     expression carries a local `DIVERGENCE:` comment and a README register
     row. This class normally exists to replace an unavailable include or
     platform call through the declared same-shaped adapter; it never licenses
     a rewritten algorithm, renamed API or altered failure contract.

  The selected original file or declared true subset is the comparison
  baseline. If more than 50 percent of the retained source, preprocessor or
  data-bearing lines differ from that baseline, the file is not admissible in
  the mirror component, even if its public function signatures remain the
  same. Move the entire differing implementation to the corresponding path
  below a dedicated component-specific `*-overlay/` root (for example
  `src/bochs-core-overlay/cpu/...`) and leave only the smallest declared call
  boundary in the mirror. Record the measured baseline, changed-line count,
  percentage, overlay path and call boundary in the mirror README exception
  register. Do not evade this rule by declaring an artificial subset that
  removes interdependent retained logic.

  A mirror file may not hide new semantics. If an added semantic intrusion
  (for example a `bochs-core` CPU/RAM mechanical interface) requires more than
  three executable lines at its actual insertion point, move that added logic
  out of the mirror into a dedicated component-specific overlay (for example
  `bochs-core-overlay`) and make the mirror call the overlay through the smallest
  declared boundary. The mirror insertion remains a locally marked and
  README-registered exception. An overlay does not exempt its call site from
  the source-first ladder, architecture admission, focused test, or intrusion
  register; it must not become a generic `compat`/`common` container.
- A `*-overlay` is called only by the corresponding native mirror, never by a
  different component or test. It has no standalone public include/link ABI.
  `app` and `adapter-softpc` are the only production callers of the declared,
  selector-blind, fixed-width mechanical facade of `adapter-bochs`.
  `adapter-softpc` must not include `bochs-core` or access a Bochs type, object
  or global. `adapter-bochs` is the only production caller of `bochs-core`.
- Keep imported OpenNT mirrors recognizable. Preserve source names, interfaces,
  data structures and ordering, and annotate each required edited expression
  with `DIVERGENCE:`. A replacement of an unavailable Win32, CCPU or SoftPC
  dependency must first be offered through an equivalently shaped
  `adapter-win32`, `adapter-softpc`, or `opennt-host` facade. A self-authored
  alternate interface remains a registered last-resort exception.
- A production OpenNT component may retain an original package only when that
  package is admitted for an actually composed/recovered provider or is a
  declared guest/firmware/tool input. Do not import dormant VDD, debugger,
  WOW, FAX or similar host product packages merely as a convenient local
  source archive; the pinned external OpenNT tree remains their source record
  until their owner-package task begins.
- `adapter-softpc` may implement a reached historical SoftPC/CCPU interface
  only through the same name, parameters, calling convention and observable
  mechanical result, backed by bounded Bochs operations. It may transport a
  copied bounded instruction window to `opennt-bop`, but does not interpret a
  BOP selector or service ID. `opennt-bop` performs that source-owned
  interpretation. Neither adapter may use unbounded guest-memory inspection,
  emulate DOS interrupts, own device policy, or turn a catalogue into a generic
  historical dispatcher.
- `session` has no dependency on a product component and no source-owned
  service vocabulary. It may provide neutral lifecycle, token/resource,
  capability-registration, completion/event and teardown contracts only.
  `app` alone creates and wires an instance. Do not add generic
  `adapter-common`, `adapter-host`, or `compat` code: a later specialist
  adapter requires an owner-package source/ABI audit and an explicit admission.
- Keep Bochs feature selection deny-by-default. A configuration addition must
  name its reached OpenNT caller, the Bochs owner, the boundary impact, and a
  negative test; do not enable a feature merely because upstream provides it.
- Do not move historical OpenNT service behavior into Bochs or either adapter.
  Keep it in the matching `opennt-bop` or `opennt-host` owner; preserve the
  original calling convention, layout, dispatch order and failure semantics;
  unsupported cases fail explicitly.
- Do not move Bochs CPU, exception, memory, firmware, device, configuration,
  or other machine semantics into NTVDM/OpenNT code or the adapter. The
  boundary transports typed facts; it does not duplicate either side.
- Build the in-process M0 runtime with MSVC x64 and `/MT` only. A MinGW,
  alternative-CRT, or differently-architected object cannot enter that link;
  communicate only through a separately admitted fixed process ABI, never by
  satisfying linker symbols across CRT boundaries.
