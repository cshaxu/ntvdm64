# T198 S50: current finite-composition membership

## Result

The clean NTIO recipe no longer obtains its `bx-vdm` compile membership from
the historical S25 command script.  It consumes the tracked manifest
`tools/t198-s50-bx-vdm-composition-manifest.json`, which names the current
finite-composition source set explicitly.

The manifest distinguishes three facts that must not be conflated:

1. the source-built fixture's actual root is
   `bx_ntvdm_boot_namespace_composition_v1`;
2. compilation membership does not activate a BOP route; and
3. the DPB provider, `adapter_runtime`, startup transaction bridge and host
   namespace are detached from that finite root.

## Witness

`t198-s50-current-members-r3` rebuilt the complete CPU5 mantle/core and every
manifest member under MSVC x64 `/MT`, linked the finite NTIO fixture, and
reproduced S49's controlled `50:46` frontier:

```text
observed-drive-resume=1 observed-dta-resume=1
observed-hard-error-resume=1 observed-first-generic-ud=0
observed-stop=1 next=50:46 terminal=1:9346:758a
```

No object from `bx_ntvdm_adapter_runtime.c`,
`bx_ntvdm_startup_transaction_bridge.c`,
`bx_ntvdm_host_namespace.c`, or the detached DPB provider is linked.

An earlier all-directory experiment is retained as negative evidence:
`adapter_runtime.c` fails the current `/W4 /WX` compile owing to historical
host-shell warnings, and `host_namespace.c` requires Windows host namespace
types.  Neither is a valid dependency of the non-invasive finite composition.

## Next admission

`50:46` has an existing source-derived DPB producer.  The next task may add a
small, explicit multi-write composition seam to
`boot_namespace_composition_v1`: it must obtain the same immutable drive
snapshot already owned by the gset plane, prepare the existing DPB
transaction, execute only its checked ordinary-RAM writes through the mantle,
and return its existing typed result.  It must not import either detached
runtime, host namespace, a device, or DOS semantics into Bochs.
