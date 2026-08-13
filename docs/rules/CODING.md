# Coding Rules

- Treat historical code as evidence before editable source. Prefer an adapter,
  compatibility header, wrapper translation unit, or build recipe to a
  behavioral rewrite.
- Keep authored runner code, overlays, host seams, imported material, generated
  files, and fixtures visibly separate.
- Make the smallest evidence-backed change. Preserve calling conventions,
  layout, segment assumptions, dispatch order, and error propagation.
- Each change to historical material records its source path, rationale, and
  focused verification. Unsupported cases fail explicitly.
- Generated binaries, logs, object files, and build trees belong below
  `artifacts/` when retained, never as source or default runtime inputs.
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
