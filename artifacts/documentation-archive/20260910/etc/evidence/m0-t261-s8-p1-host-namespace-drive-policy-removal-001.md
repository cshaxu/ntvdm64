# M0 T261 S8 P1 Host Namespace And Drive-Policy Removal

## Question

Can the self-authored `opennt-host` namespace and include/exclude-drive
policy be removed without retaining a live product or test dependency?

## Inputs

- `src/opennt-host/dem/bx_ntvdm_host_namespace.{c,h}`
- `src/opennt-host/dem/bx_ntvdm_host_drive_policy.{c,h}`
- Direct OpenNT DEM bodies in `src/opennt-bop/dem/`
- `tools/build/t260-s8-component-manifest.json`
- Fresh MSVC x64 `/MT` Ninja root `build/t261/s8-r002/`

## Procedure

1. Searched every live `src/`, `tests/`, and manifest input for both namespace
   and drive-policy symbols.
2. Deleted the namespace, its two focused fixtures, and its manifest input:
   it had no product caller.
3. Deleted the include/exclude snapshot provider, engine request fields,
   startup/runtime binding, source-level suppression branches, focused policy
   fixtures, and its manifest input.
4. Restored the original direct host-drive query behavior in reached DEM and
   COMMAND bodies. The historical `bx-vdm` trace fixture was retained without
   modification below `docs/etc/legacy_code/tests/`, because it depends on the
   retired legacy composition and is not a current test input.
5. Generated and built the new formal component graph. Ninja completed all
   360 edges without a compiler or linker diagnostic.

## Observations

- The live-source negative scan finds no namespace or drive-policy symbol,
  CLI mask, or environment-policy variable.
- `t256-s6-engine-geometry-contract-fixture.exe` exits `0` with the reduced
  engine request ABI.
- `t230-s6-demgset-direct-import-fixture.exe` exits `0`, covering the direct
  OpenNT DEM global-setting and host drive query path.
- `t245-s11-config-complete-fixture.exe` exits `2`. This fixture had already
  been recorded as an independent config-complete limitation; P1 neither
  treats it as passing evidence nor attributes it to the policy removal.

## Interpretation

The removed namespace and policy were project-authored product features, not
required OpenNT owner bodies. Their removal makes the active product path
closer to the original process-wide host-drive view: present host drives are
queried directly through the reached OpenNT code and public Win32 calls. The
remaining S8 XMS, VDMREDIR and top-level replacements are independent work.

## Follow-up

S8 continues with the XMS mechanical seam in `adapter-softpc`, then the
original VDMREDIR and `nt_error.c` packages. Do not reintroduce an
include/exclude drive filter or a private namespace provider.
