# Proposal — Project code filename identity normalization

## Objective

Normalize project-authored and mechanical-adaptation code filenames so that
component directory ownership, rather than a filename prefix or provisional
implementation-version suffix, identifies the project, machine backend, or
historical subsystem. The product is `ntvdm64`; neither `ntdos64` nor
`ntvdm64` is an admissible project-code filename prefix. Likewise, `bx`,
`bochs`, `_v1`, and `_v2` are not admissible project-code filename markers
outside component directory names that already establish ownership.

## Scope

The task inventories and renames every live project-code source, header,
generated-input manifest, fixture and directly paired build-reference filename
under `app`, `session`, `adapter-bop`, `adapter-softpc`, `adapter-win32`, and
the renamed Bochs assembly component once its final name is admitted. It also
repairs matching include statements, build manifests, fixture target names,
source lists and documentation links. Renames use `git mv`.

The six original-code mirrors—`bochs-core` after its component rename,
`opennt-guest`, `opennt-bop`, `opennt-host`, `opennt-softpc`, and
`opennt-utils`—and all corresponding `*-overlay` roots are out of scope.
Their upstream-relative filenames are source identity, not project branding.

## Naming rule

Outside the mirror roots, filenames describe their local responsibility
without embedded product/backend aliases. Component directory names, target
names required for product packaging, public executable names, and provenance
records are not renamed merely because they contain a product or upstream
name.

## Packets

| S | Scope | Exit criterion |
| --- | --- | --- |
| S1 | Inventory every live in-scope code filename and classify retain/rename, including build and include consumers. | A complete manifest distinguishes code filenames from permitted component directories, product artifacts, provenance and mirror paths. |
| S2 | Rename `app` and `session` code, repair references and run focused formal targets. | No in-scope `app`/`session` filename retains a prohibited prefix; references resolve. |
| S3 | Rename all remaining admitted mechanical-adaptation code and fixture names, repair graph/document references, and remove obsolete paths. | No in-scope production or fixture filename retains a prohibited prefix; no stale include/build reference remains. |
| S4 | Complete cross-root source/build/document scan and formal graph verification. | The manifest is closed, each rename is `git mv`-traceable, focused targets pass, and governance/diff checks pass. |
| S5 | Remove remaining `_v1` / `_v2` implementation-version filename markers, then classify and remove non-semantic project/backend/source-provenance identifier prefixes from in-scope production code and repair exact consumers. | No in-scope project-owned code basename contains a prohibited product/backend/source-provenance marker or standalone `v1`/`v2` filename segment; each remaining DOS/VDM/Bochs/OpenNT lexical identifier has a source-semantic or upstream-interface disposition; direct consumers resolve without an ABI or behavior rewrite. |

## Non-goals

- No component-directory rename, code behavior or ABI change, BOP work, or
  Bochs/OpenNT source-mirror rename.
- No mass rename of historical evidence, external sources, release artifacts,
  or provenance references.
- No change to any original-code mirror or overlay content.
- No blind removal of DOS/VDM/Bochs/OpenNT terms from identifiers that name
  actual guest, machine, register, provider, or preserved upstream-interface
  semantics.

## Admission evidence

This is an unnumbered candidate task. It becomes a numeric T only when the
owner separately admits it into `STATUS.md`.
