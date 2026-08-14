# M0 T198 S84: BOP-`5F` Observer-Order Audit

## Question

Why do S81/S83 traces report no passive BOP-`5F` observation even though the
resident SpcKbd `isk_int9` path, which follows that handoff, writes the
observed IVT-10 far pointer?

## Inputs And Procedure

- The S83 bounded source-built run and its `spckbd observed=0` marker.
- `tests/bx-vdm/t198_s23_native_ntio_boundary_bridge.c` ordering.
- `src/bx-vdm/bx_ntvdm_boot_namespace_composition_v1.c` composition dispatch.
- `src/bx-vdm/bx_ntvdm_spckbd_init_service.c` and its focused test.
- Original `spckbd.asm`, `bop.h`, and `softpc.new/host/src/nt_bop.c`.

The audit is static and read-only. It adds no observer, provider, guest write,
machine feature or profile state.

## Observations

The test bridge calls `bx_ntvdm_boot_namespace_composition_v1_handle()` before
its `observed_spckbd` branch. The composition in turn tries
`bx_ntvdm_spckbd_init_service_v1_dispatch()` before common ingress/registry
classification.

That service exactly recognizes real-mode `C4 C4 5F` when `AX=BEEF`, resumes
at `RIP+3`, and sets CF. It returns handled. Consequently the bridge returns
from the composition delegate and cannot reach its later passive recorder.

This is the exact historical guest continuation: `spckbd.asm` issues BOP
`5F`, branches `jc isk_int9`, and `isk_int9` copies IVT-10 into
`host_int10`. The original host selector is `BOP_UNIMPINT` (`bop.h`); the
historical `MS_bop_F` calls `kb_setup_vectors()`. The current narrow service
does not claim to rehost that host routine. It only produces the source-proven
x86 carry continuation; native Bochs still owns keyboard/PIC mechanics.

The generic ingress table separately classifies ordinary selector `5F` as
explicitly unavailable. That generic classification is not reached by the
special BEEF service because its early result has already consumed the event.

## Interpretation

The S81/S83 `spckbd observed=0` marker means **post-delegate invisible**, not
unreached. The pointer bracket, the special service's CF result, and the
original `jc isk_int9` source form one consistent path:

```text
C4 C4 5F, AX=BEEF
  -> bx-vdm special continuation sets CF and resumes
  -> guest jc isk_int9
  -> guest copies IVT 10h to host_int10
  -> later FAR JMP C000:014A
```

No BOP-`5F` observer or provider needs to be added. The remaining substantive
question is semantic completeness: historical `MS_bop_F/kb_setup_vectors()`
also established host display/vector state, while the present continuation
intentionally does not. Whether that host state requires an admitted CLI
policy, an opaque mantle machine image, or a source-derived no-display
profile must be determined before any C000/ROM action.

## Confidence And Follow-up

High confidence: the passive-observer gap is entirely explained by source
order and the current early handled result. S85 must compare the retained
original host routine's observable contracts with the narrow continuation;
it may not broaden BOP-`5F`, import ROM, choose a display mode, or change
Bochs mechanics without a separate admitted result.
