# M0 T183 finite-sequence CLI shim/bundle composition plan

## Packet purpose

Recompose the existing CLI-to-Bochs shim and fixed native bundle around T182's
linked engine. This restores the established process boundary; it does not add
Bochs, adapter, BOP, DOS, or host-capability semantics.

| S | Scope | Initial disposition |
| --- | --- | --- |
| S1 | Inventory current shim/runner source, T182 engine, exact bundle inputs, manifest and policy closure. | Complete: current source and four bundle inputs form a finite closure. |
| S2 | Define one current shim/bundle build and assembly recipe, or reject it with the missing dependency. | Active in Status. |
| S3 | Build and policy-validate the shim/bundle without starting a guest process. | Pending S2. |

## Invariants

- The shim accepts BYOB metadata from the runner alone; raw Bochs accepts only
  its fixed native `-q -f bochsrc` invocation.
- Bundle membership remains exactly engine, BIOS, VGA BIOS and option ROM,
  each content-addressed by the fixed-order manifest.
- No Bochs object/archive build, adapter source change, device enablement or
  new configuration directive is admitted.
- A guest observation is a separate future admission after this closure.
