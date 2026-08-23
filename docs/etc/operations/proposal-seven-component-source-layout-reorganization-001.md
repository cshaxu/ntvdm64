# Seven-component source-layout reorganization proposal

## Decision requested

Admit a bounded repository reorganization task which moves the transitional
source tree into the following target components. The task changes ownership,
build inputs and documentation only; it does not use the move as authorization
to change a BOP provider, guest behavior, machine feature, or compatibility
semantic.

| Target component | Contract |
| --- | --- |
| `src/bx-core` | Adopted Bochs core machine. Upstream layout is retained and changes are exceptional; `README.md` is its local-intrusion register. |
| `src/bx-mantle` | Cropped, Bochs-only native assembly/lifecycle layer. It contains no DOS, VDM, OpenNT or host capability semantics. |
| `src/opennt-guest` | OpenNT DOS/WOW16 source and guest images. Original prebuilt artifacts are packaged by default; original-source builds are fallback/reproducibility paths. `README.md` records any divergence, expected to be none. |
| `src/opennt-host` | Independently composable original OpenNT host-capability components and necessary source-derived replacements for unavailable historical host facilities. `README.md` records changed imports and newly authored components. |
| `src/opennt-bop` | Minimal-change mirrors of OpenNT BOP providers. Original names, interfaces, structures, ordering and failure behavior stay visible. A necessary dependency redirection is annotated `DIVERGENCE:` and targets `opennt-host`, `adapter-win32`, or `adapter-bx`. |
| `src/adapter-win32` | Same-shaped modern public-Win32 facades for unavailable historical Win32 APIs. It does not own BOP policy. |
| `src/adapter-bx` | Bounded typed conversion between Bochs and VDM-facing OpenNT-shaped interfaces. It does not identify a BOP selector or own DOS/OpenNT policy. |
| `src/app` | `ntvdm64` executable entry, CLI, and explicit final loading/composition of all other components. |

## Target dependency graph

```text
app -> opennt-guest                         (guest image input)
app -> opennt-bop -> opennt-host
opennt-bop -> adapter-win32
opennt-bop -> adapter-bx -> bx-mantle -> bx-core
opennt-host -> adapter-win32                (declared facade only)
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
| `src/bx-mantle/` | `src/bx-mantle/` | Retain in place. Audit only for foreign DOS/VDM/OpenNT meaning; pure Bochs assembly remains without an external-source exception register. |
| `src/cli/` | `src/app/` | `git mv` the CLI, BYOB/image selection, startup lifecycle, cancellation, and native executable entry. |
| `src/bx-vdm/bop/opennt/{dem,command,xms,dpmi,softpc}` | `src/opennt-bop/…` | `git mv` each original mirror while retaining its owner-family layout, original interface shape, and existing `DIVERGENCE:` annotations. |
| `src/bx-vdm/bop/*_generic_ud_bridge*`, BOP runtime/composition/ingress routes, and historical selector/service maps | `src/opennt-bop/` | Move as BOP interpretation/routing. They may request a bounded `adapter-bx` operation but are not adapter mechanics. |
| `src/bx-vdm/bop/shim/` file/path/process/environment/PIF/redirector/XMS/DEM session capability code and `src/opennt/local/compat/host/` | `src/opennt-host/` | Classify by original host-capability owner and move there when it provides a host service rather than an unavailable-API facade or Bochs conversion. |
| Same-named unavailable Win32/NTDLL facade code, including RTL and message/dialog facades | `src/adapter-win32/` | Move by API family. Preserve historical spelling and observable semantics; this component owns no BOP/provider policy. |
| Root `src/bx-vdm/` CPU-frame, exception, checked guest-memory, instruction-window, typed-result, and machine-engine contracts | `src/adapter-bx/` | `git mv` as bounded Bochs/VDM mechanics. It must remain selector/service blind. |
| Shared mapping-manager implementation and its session-local `guest_memory`, `host_handle`, and `session_data` instances | `src/adapter-bx/` | Retain one implementation and three session instances. The manager maps opaque IDs only; Win32 handle semantics/lifetime remains in `adapter-win32` or `opennt-host`. |
| Root `src/bx-vdm/` host-drive/namespace policy and final startup/session/image plan | `src/opennt-host/` or `src/app/` | Host path/drive capability is `opennt-host`; final component assembly, guest-image loading and startup selection is `app`. S1 assigns each file, not its name. |
| Live DOS/WOW source and guest artifact/build inputs below `src/opennt/base/mvdm/{dos,wow16}` | `src/opennt-guest/` | Move only source and artifact inputs belonging to the guest. Original prebuilt artifacts remain default packaging inputs. |
| Live OpenNT headers below `src/opennt/base/mvdm/inc`, SoftPC includes, and public/internal include trees | `opennt-bop/include`, `opennt-host/include`, or `opennt-guest/include` | Split by original consumer/owner; host ABI declarations are published by `opennt-host` to its declared consumers. |
| Unreached full OpenNT source not named by the product manifest | `refs/opennt/` evidence input | Remove it from product source/build roots while retaining immutable original-source provenance. It is not a second runtime provider. |
| `tests/bx-vdm/`, `tests/opennt/`, `tests/runner/`, and old machine-composition test paths | `tests/adapter-bx`, `tests/opennt-bop`, `tests/opennt-host`, `tests/opennt-guest`, or `tests/app` | Move each test with the library/component under test. Retain historical task names only in fixture filenames and evidence. |
| `tools/build/*manifest*` and formal Ninja source lists | `tools/build/` | Keep tools in place; replace legacy paths, include roots, module names, fixture library lists, and final link order with target-component entries. |

`src/bx-vdm/bop/shim/` is explicitly a mixed historical directory. Its name is
not an ownership category and it may not survive as a target product path:
each file is classified as `opennt-host`, `adapter-win32`, `adapter-bx`, or
`opennt-bop` before it moves. Likewise, a file is not moved to `adapter-bx`
merely because it presently mentions a guest pointer; BOP/provider semantics
remain in the OpenNT owner and only the bounded mechanical conversion moves.

## Migration method

1. Produce a file-level owner/provenance/build-input manifest for every live
   source, header, test, generated input and build-list entry below the current
   transitional `src/cli`, `src/bx-vdm`, and `src/opennt` paths.
2. Create the target component README/register roots before moving source.
   Their exception records must link source identity, reason, replacement seam
   and focused verification.
3. Move pure-owner files with `git mv`; update include paths, manifest entries
   and Ninja module lists in the same change. Do not copy imported sources as a
   substitute for preserving history.
4. For intertwined files, split only after their individual statements have
   been classified by owner. Record every necessary source edit; imported
   OpenNT mirror expressions receive `DIVERGENCE:` comments.
5. Rebuild each static-library/module closure and then the composed fixture or
   CLI target. A build repair must not silently introduce a semantic shim.
6. Retire transitional production paths only after every live caller and build
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
