# Pre-Governance Source Policy Record

> **Superseded as current policy on 2026-08-09.** Use
> [etc/operations/policy/source-policy.md](etc/operations/policy/source-policy.md).
> This detailed record remains retained provenance evidence.

## Objective

This project reconstructs a non-invasive, CLI-invocable NT 4-era `ntvdm.exe` by
repairing and composing the OpenNT NT 4.0 SoftPC/CCPU historical path. OpenNT
NT 4.0 MVDM and NTDOS are the primary historical base; OpenNT-4.5, Win2000SRC,
winxpscodes, MinNT, `ntvdm64`, Bochs and PCjs are comparative reference sources.
The project remains independently buildable and testable; no comparison tree
is a runtime dependency, execution gate, or substitute implementation. The
binding reconstruction rules are in `SOFTPC-RECONSTRUCTION-GOVERNANCE.md`.

## Boundary

`D:\home\repos.hobby\ntvdm64` is read-only reference material for this project. Do not create, edit, generate, copy artifacts into, or otherwise write to that directory while working here.

## Source Roles

| Source | Permitted role | Not a substitute for |
| --- | --- | --- |
| OpenNT NT4 | Primary historical code and implementation body | A freely rewritten code base or a replacement DOS runtime |
| OpenNT-4.5 | Comparative source and regression evidence | Provenance for another tree |
| Win2000SRC | Later implementation comparison and ABI clues | NT 4 behavior baseline |
| winxpscodes | Later implementation comparison and ABI clues | NT 4 behavior baseline |
| MinNT | Build/history comparison and missing-interface clues | NT 4 behavior baseline |
| NTVDMx64 | Modern-porting and host-compatibility comparison | OpenNT source, runtime backend, recovered-function implementation, or active SoftPC task input |
| ntvdm64 | Read-only architecture comparison | SoftPC recovery gate, writable dependency, build input, runtime backend, or local implementation source |
| Bochs / PCjs | CPU and PC semantic cross-checks | Copied implementation, runtime backend, recovered-function implementation, or local implementation source |

## Rules For Historical Material

- Record source tree, path, and revision or snapshot identity for every imported file, copied fragment, behavior claim, or ABI conclusion.
- Preserve original source text whenever feasible. Prefer adapters, build shims, generated configuration, and isolated host code over edits to the historical implementation.
- Every unavoidable historical-source change needs a rationale and linked evidence explaining why an adapter was insufficient.
- Do not use Microsoft binaries, proprietary headers, or redistributed build outputs as hidden dependencies. Record required user-provided artifacts as BYOB inputs and validate their identity at the boundary.
- Keep imported or generated content distinguishable from project-authored code. Do not hand-edit generated copies without recording the generator or patch process.
- A comparison source may support an OpenNT reconstruction only after a
  caller-first audit has established the missing historical contract. The local
  implementation must be independently authored in the matching
  `src/opennt/overlay/` ownership path and must state the corroborating source;
  it must not import, translate wholesale, or link the comparison code.
- A modern-host seam is not a recovered SoftPC implementation. It may live in
  `src/opennt/local/compat/host/` only after a reached OpenNT caller proves that a
  retired host dependency blocks the historical data flow. The seam must retain
  that function boundary, have its own bounded fixture, and cannot define DOS,
  BOP, BIOS, CCPU, or device behavior.
- No comparison tree may appear in a CMake source list, include path, link
  library, code generator, or runtime discovery path. A source scan or an
  explicitly disabled provenance audit may read it, but it is not a build input.

## Research Binary Artifacts

An explicit user-approved research artifact may retain a fixed Microsoft binary
only to establish identity or compare a reproduced build. It must live below
`artifacts/`, carry a manifest with source path, role, hash, byte count, and
redistribution status, and remain outside source, default build inputs, normal
runner discovery, and release packaging. It must never be silently substituted
for a source-built result or a BYOB file. The current example is
`artifacts/guest-inputs/nt4-msdos6-recovery-v1`.

## Provenance And Compatibility Evidence

The historical-source gates, toolchain provenance matrix, artifact matrix, and bootstrap audits are evidence records. New experiments link to them when refining or contradicting a prior conclusion. A new source family or toolchain must not become an implicit dependency before it has a documented provenance and compatibility role.

## Source Modification Admission

Before modifying a historical source file, document:

1. Exact file and historical behavior at stake.
2. Host or toolchain incompatibility.
3. Adapter, wrapper, or original-toolchain alternatives considered.
4. Smallest proposed change and observable risk.
5. Validation that distinguishes compatibility repair from semantic drift.
