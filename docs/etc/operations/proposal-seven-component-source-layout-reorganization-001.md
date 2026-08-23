# Seven-component source-layout reorganization proposal

## Admitted task scope

M0 T260 is a single bounded repository reorganization task: **eight-component
source-layout reorganization and build closure**. Its components are not
separate T tasks because their headers, library names, link order and source
ownership must change as one auditable dependency graph. The task changes
ownership, build inputs and documentation only; it does not use a move as
authorization to change a BOP provider, guest behavior, machine feature, or
compatibility semantic.

| Target component | Contract |
| --- | --- |
| `src/bx-core` | Adopted Bochs core machine. Upstream layout is retained and every change is exceptional; `README.md` is its local-intrusion register. |
| `src/bx-mantle` | Cropped, Bochs-only native assembly/lifecycle layer. It contains no OpenNT, DOS, VDM, WOW or Win32 semantics. |
| `src/opennt-guest` | OpenNT DOS/WOW16 source and guest images. Original prebuilt artifacts are packaged by default; original-source builds are fallback/reproducibility paths. `README.md` records every divergence, expected to be none. |
| `src/opennt-host` | Independently composable original OpenNT host-capability components and necessary source-derived replacements for unavailable historical host facilities. `README.md` records every changed import and new component. |
| `src/opennt-bop` | Minimal-change mirrors of OpenNT BOP providers. Original names, interfaces, structures, ordering and failure behavior stay visible. `README.md` records every imported-source exception; each necessary dependency redirection is annotated `DIVERGENCE:` and targets `opennt-host`, `adapter-win32`, or `adapter-softpc`. |
| `src/adapter-softpc` | Bochs-backed recovery of reached historical SoftPC/CCPU interfaces with the same name, parameters, ABI and observable mechanical semantics. It does not own BOP meaning. |
| `src/adapter-win32` | Same-shaped modern public-Win32 facades for unavailable historical Win32 APIs. It does not own BOP policy. |
| `src/app` | `ntvdm64` executable entry, CLI, and explicit final loading/composition of all other components. |

## Target dependency graph

```text
app -> opennt-guest                         (guest image input)
app -> opennt-bop -> opennt-host
opennt-bop -> adapter-win32
opennt-bop -> adapter-softpc -> bx-mantle -> bx-core
app -> adapter-softpc                       (session assembly/run only)
opennt-host -> adapter-win32                (declared Win32 facade only)
opennt-host -> adapter-softpc               (declared SoftPC/CCPU facade only)
```

No reverse edge is permitted. In particular, `bx-core` and `bx-mantle` do not
know DOS, VDM, BOP, OpenNT, or host policy; adapters do not become BOP/service
providers; and guest images are not linked into host providers.

## Current-to-target migration map

This map is the required starting disposition, not permission for a blind
directory move. S1 must expand every row to a file-level owner/provenance/build
manifest before S3 changes product paths.

| Current location or kind | Target owner | Required disposition |
| --- | --- | --- |
| `src/bx-core/` | `src/bx-core/` | Retain in place. Create its `README.md` local-intrusion register and transfer the existing Bochs exception evidence into it. |
| `src/bx-mantle/` | `src/bx-mantle/` | Retain only pure Bochs assembly in place. Every file or statement with OpenNT, DOS, VDM, WOW or Win32 semantics is assigned to its target owner before S3 closes. |
| `src/cli/` | `src/app/` | `git mv` the CLI, BYOB/image selection, startup lifecycle, cancellation, and native executable entry. |
| `src/bx-vdm/bop/opennt/{dem,command,xms,dpmi,softpc}` | `src/opennt-bop/…` | `git mv` each original mirror while retaining its owner-family layout, original interface shape, and existing `DIVERGENCE:` annotations. |
| `src/bx-vdm/bop/*_generic_ud_bridge*`, BOP runtime/composition/ingress routes, and historical selector/service maps | `src/opennt-bop/` | Move as BOP interpretation/routing. They may request a bounded `adapter-softpc` operation but are not adapter mechanics. |
| `src/bx-vdm/bop/shim/` file/path/process/environment/PIF/redirector/XMS/DEM session capability code and `src/opennt/local/compat/host/` | `src/opennt-host/` | Classify by original host-capability owner and move there when it provides a host service rather than an unavailable-API facade or Bochs conversion. |
| Same-named unavailable Win32/NTDLL facade code, including RTL and message/dialog facades | `src/adapter-win32/` | Move by API family. Preserve historical spelling and observable semantics; this component owns no BOP/provider policy. |
| Root `src/bx-vdm/` CPU-frame, exception, checked guest-memory, instruction-window, typed-result, and machine-engine contracts | `src/adapter-softpc/` | `git mv` as bounded same-shaped SoftPC/CCPU-to-Bochs mechanics. It must remain selector/service blind. |
| Shared mapping-manager implementation and its session-local `guest_memory`, `host_handle`, and `session_data` instances | `src/adapter-softpc/` | Retain one implementation and three session instances. The manager maps opaque IDs only; Win32 handle semantics/lifetime remains in `adapter-win32` or `opennt-host`. |
| Root `src/bx-vdm/` host-drive/namespace policy and final startup/session/image plan | `src/opennt-host/` or `src/app/` | Host path/drive capability is `opennt-host`; final component assembly, guest-image loading and startup selection is `app`. S1 assigns each file, not its name. |
| Live DOS/WOW source and guest artifact/build inputs below `src/opennt/base/mvdm/{dos,wow16}` | `src/opennt-guest/` | Move only source and artifact inputs belonging to the guest. Original prebuilt artifacts remain default packaging inputs. |
| Live OpenNT headers below `src/opennt/base/mvdm/inc`, SoftPC includes, and public/internal include trees | `opennt-bop/include`, `opennt-host/include`, or `opennt-guest/include` | Split by original consumer/owner; host ABI declarations are published by `opennt-host` to its declared consumers. |
| Unreached full OpenNT source not named by the product manifest | `refs/opennt/` evidence input | Remove it from product source/build roots while retaining immutable original-source provenance. It is not a second runtime provider. |
| `tests/bx-vdm/`, `tests/opennt/`, `tests/runner/`, and old machine-composition test paths | `tests/adapter-softpc`, `tests/opennt-bop`, `tests/opennt-host`, `tests/opennt-guest`, or `tests/app` | Move each test with the library/component under test. Retain historical task names only in fixture filenames and evidence. |
| `tools/build/*manifest*` and formal Ninja source lists | `tools/build/` | Keep tools in place; replace legacy paths, include roots, module names, fixture library lists, and final link order with target-component entries. |

`src/bx-vdm/bop/shim/` is explicitly a mixed historical directory. Its name is
not an ownership category and it may not survive as a target product path:
each file is classified as `opennt-host`, `adapter-win32`, `adapter-softpc`, or
`opennt-bop` before it moves. Likewise, a file is not moved to `adapter-softpc`
merely because it presently mentions a guest pointer; BOP/provider semantics
remain in the OpenNT owner and only the bounded mechanical conversion moves.

## Migration method

### S1 — Complete owner/provenance/build manifest

Produce a file-level owner/provenance/build-input manifest for every live
   source, header, test, generated input and build-list entry below the current
   transitional `src/cli`, `src/bx-vdm`, and `src/opennt` paths.

### S2 — Target roots, registers and library boundaries

Create the target component README/register roots and declared public include
and static-library boundaries before moving source.

### S3 — Stable in-place Bochs components

Confirm `bx-core` ownership in place and purify `bx-mantle`: migrate its
required register evidence and remove every OpenNT, DOS, VDM, WOW or Win32
semantic from mantle into the appropriate target component.

### S4 — `app` and `opennt-guest`

Move `src/cli` to `src/app` and move live DOS/WOW source/artifact inputs to
`src/opennt-guest`, then repair only their independent build-path references.

### S5 — `opennt-bop`

Move the original DEM/COMMAND/XMS/DPMI/SoftPC mirrors and BOP ingress/route
logic out of `bx-vdm` without changing provider semantics.

### S6 — `opennt-host` and `adapter-win32`

Classify every remaining mixed legacy `shim` file: host capability moves to
`opennt-host`; unavailable API same-shape facade moves to `adapter-win32`.

### S7 — `adapter-softpc`

Move CPU frames, checked RAM, typed results, engine contracts and the one
mapping-manager implementation with its three session instances to
`adapter-softpc`.

### S8 — Tests, Ninja and legacy-path exit

Move fixtures by tested owner; replace every formal Ninja module/include/link
input; verify no live product input uses `src/bx-vdm`, `src/cli`, or generic
`src/opennt`.

Across S2--S8, create the target component README/register roots before moving source.
   Their exception records must link source identity, reason, replacement seam
   and focused verification.
Move pure-owner files with `git mv`; update include paths, manifest entries
   and Ninja module lists in the same change. Do not copy imported sources as a
   substitute for preserving history.
For intertwined files, split only after their individual statements have
   been classified by owner. Record every necessary source edit; imported
   OpenNT mirror expressions receive `DIVERGENCE:` comments.
Rebuild each static-library/module closure and then the composed fixture or
   CLI target. A build repair must not silently introduce a semantic shim.
Retire transitional production paths only after every live caller and build
   manifest points at the target component. Historical evidence may remain
   under its indexed evidence path, never as a second product provider.

## Non-goals

- No new BOP implementation, selector routing, guest DOS/WOW behavior, or
  Win32 capability is admitted by this task.
- No Bochs feature is enabled and no Bochs CPU/device semantic change is
  justified by a directory move.
- No guest artifact is rebuilt merely to validate a source relocation.
- No compatibility facade is invented where a move or include-path repair is
  sufficient.

## Exit evidence

- A complete manifest assigns every live source/build input one target owner,
  provenance class and migration disposition.
- The target README registers exist and contain all inherited exception data.
- Pure-owner moves retain Git history through `git mv`; each split or retained
  transitional file has a recorded reason.
- The formal Ninja graph emits component static libraries and its input lists
  contain no undeclared legacy production path.
- Focused module builds and one composed link pass; `git diff --check` and the
  documentation-governance verifier pass.
- `docs/design/ARCHITECTURE.md`, `docs/design/CODING.md`, and the architecture
  and coding rules match the resulting ownership graph.

## Admission prerequisites

The active packet must cite the live BOP tracker and current build manifest,
preserve unrelated working-tree changes, and follow the source policy in
[`source-policy.md`](policy/source-policy.md). Any claimed source exception
requires its original source identity and an explicit owner disposition before
it is moved or edited.
