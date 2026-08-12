# Git Baseline Scope — 2026-08-12

## Purpose

The committed repository stopped at T125 while subsequent project-authored
source, tests, tools, imported research source and governance records were
created in the working tree. This record defines a single honest recovery
baseline. It does not recast the intervening work as individually committed
historical P deliveries.

## Included

- current authored and imported source below `src/`;
- tests below `tests/`;
- tools below `tools/`;
- governance, design, history and research records below `docs/`;
- repository-control and top-level build/project documents, including
  `CMakeLists.txt`, `AGENTS.md`, `README.md`, `CONTRIBUTING.md`, `TODO.md`,
  `QUEUE.md`, `ROADMAP.md` and `RESEARCH-PROJECT.md`; and
- `artifacts/README.md`, which documents the generated-artifact boundary.

## Excluded

- every generated runtime/build/log/probe/evidence payload below `artifacts/`;
- compiler/linker products (`.obj`, `.exe`, `.pdb`, `.ilk`, `.map`, `.lib`,
  `.a`); and
- local scratch output and probe environments.

## Consequence

The next task delivery starts from this versioned source baseline. Any future
runtime claim must continue to cite retained artifact hashes and paths; the
absence of generated payloads from Git is not evidence that those runs did not
occur.
