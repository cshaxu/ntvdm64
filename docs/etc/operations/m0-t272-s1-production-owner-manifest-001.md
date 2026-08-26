# M0 T272 S1 production owner and source manifest

## Purpose

This is the initial owner/provenance/build disposition for the clean
thirteen-root source topology. It is a construction manifest, not an import
manifest: no source under `src.old/` is a formal input merely because it is
listed here.

## Snapshot inputs

The following local evidence was observed on 2026-08-25:

- `src.old/app`: 112 files;
- `src.old/session`: 7 files;
- `src.old/bochs-core`: 206 files;
- `src.old/adapter-bochs`: 12 files;
- `src.old/adapter-bop`: 7 files;
- `src.old/adapter-softpc`: 397 files;
- `src.old/adapter-win32`: 49 files;
- `src.old/opennt-guest/dos-v86`: 676 files;
- `src.old/opennt-guest/wow16`: 44 files;
- `build/output/dos`: 28 files;
- `build/output/wow16`: 87 files;
- `O:\repos.external\OpenNT\base\mvdm`: 3520 files;
- `O:\repos.external\OpenNT-4.5\nt\private\mvdm`: 3670 files; and
- `O:\repos.external\bochs-2.6-compat\bochs-2.6`: 672 files.

Counts are discovery evidence only; later source/package manifests carry
path/hash/provenance decisions.

## Root dispositions

| Production root | Owner and input rule | Local reuse source | Formal build/output role | First owner packet |
| --- | --- | --- | --- | --- |
| `bochs-core` | Pinned Bochs 2.6 mirror; exact/subset/same-shaped audit only. | `src.old/bochs-core` is comparison/recovery evidence. | `bochs-core.lib`; only `adapter-bochs` may consume it. | Bochs machine foundation |
| `opennt-mvdm-host` | Canonical non-guest/non-tool OpenNT/OpenNT-4.5 package union. | Existing OpenNT-related roots are reference/disposition evidence only. | One or more package libraries, never a source-component split. | Canonical OpenNT source supply |
| `opennt-platform-abi` | Exact original declarations outside MVDM; no replacement bodies. | Existing copied headers are evidence only. | Header-only or declaration library as required. | Canonical OpenNT source supply |
| `opennt-guest-dos` | Complete existing DOS/V86 carry-forward mirror. | `src.old/opennt-guest/dos-v86`. | Load-only guest bytes; never host compile/link input. | Canonical OpenNT source supply |
| `opennt-guest-wow16` | Complete existing WOW16 carry-forward mirror. | `src.old/opennt-guest/wow16` plus the applicable existing `bin86` material. | Load-only guest bytes; never host compile/link input. | Canonical OpenNT source supply |
| `adapter-bochs` | Bochs-only construction and opaque lifecycle. | `src.old/adapter-bochs` audited file-by-file. | `adapter-bochs.lib`; sole caller of `bochs-core`. | Bochs machine foundation |
| `adapter-bop` | Selector-blind copied frame transport only. | `src.old/adapter-bop` audited file-by-file. | `adapter-bop.lib`. | Historical interface adapter recovery |
| `adapter-softpc` | Same-shaped reached SoftPC/CCPU/SAS interface facade. | `src.old/adapter-softpc` audited file-by-file. | `adapter-softpc.lib`; uses typed `adapter-bochs` operations only. | Historical interface adapter recovery |
| `adapter-win32` | Same-shaped unavailable historical Win32/NTDLL facade. | `src.old/adapter-win32` audited file-by-file. | `adapter-win32.lib`. | Historical interface adapter recovery |
| `adapter-vdm-monitor` | Complete same-shaped VDM monitor interface family. | No prior root; source evidence and existing fragments are reference only. | `adapter-vdm-monitor.lib`. | Historical interface adapter recovery |
| `session` | Neutral per-instance lifecycle, mappings, resources, events and teardown. | `src.old/session` audited file-by-file. | `session.lib`; no provider vocabulary. | M0 T272 |
| `broker` | Project-owned per-user IPC registration/identity/queue/cleanup. | No prior root; any existing broker-like code is reference only. | `broker.lib` and admitted helper executable/process as needed. | M0 T272 contract; historical adapter recovery implementation |
| `app` | CLI and final composition only. | `src.old/app` audited file-by-file. | `app.lib` and `ntvdm.exe`; no provider algorithms. | Bochs shell; dual-architecture product composition |

Their current admission order is authoritative in `docs/QUEUE.md`; no future
numeric T identifier is reserved by this manifest.

## Input exclusions

- `src.old/` is read-only comparison/recovery evidence. A selected file must
  be copied to its destination root and receive an explicit disposition before
  it can appear in any formal manifest or build graph.
- `build/output/dos` and `build/output/wow16` remain prior build output and
  evidence. They are not moved, duplicated into `src/`, or consumed by a host
  compile/link graph.
- Guest objects/libraries and every guest source file are excluded from host
  compile/link graphs; only manifest-selected immutable guest bytes are later
  loaded by app through `adapter-bochs`.
- External OpenNT/OpenNT-4.5 sources are used for the canonical non-guest
  package union. They are not used to re-copy the already complete guest
  mirrors unless a later provenance audit finds a documented defect.

## Reuse decision protocol

Every file proposed from an existing project-owned root receives exactly one
of: `copy`, `reference-only`, `split`, `replace`, or `retire`. A `copy` must
preserve the target component's dependency rules. `split` records the new
owners before code moves. A reused mirror file also satisfies the component
mirror audit standard and its README divergence register.

## S1 verification targets

S1 is complete only when the thirteen root READMEs exist, this manifest has
one disposition for every root, the current queue/status admission agrees
with it, and no source/build manifest contains an `src.old` or `build/output`
path as a formal input.
