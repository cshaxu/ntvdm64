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
