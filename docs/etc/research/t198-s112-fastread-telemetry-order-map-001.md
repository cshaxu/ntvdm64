# M0 T198 S112: Fast-Read Telemetry Order Map

## Result

S111's final `next=50:42` is **decline telemetry**, not proof that the
source-derived fast-read provider accepted that event.  The bridge invokes the
composition first; only when it returns false does it record selector/service,
set `GENERIC_UD_STOP`, and print the final `next` fields.

The independently observed BOP `5F` still proves the S106 no-op intercept is
no longer blocking the stream initialization path.  It does not identify which
of potentially multiple fast-read calls the provider accepted.

## Current-Code Order

| Order | Source | Meaning |
| --- | --- | --- |
| 1 | `t198_s23_native_ntio_boundary_bridge.c:151-164` | Copy BOP `5F` event fields before delegation; no mutation. |
| 2 | `...:165-202` | Call `bx_ntvdm_boot_namespace_composition_v1_handle`; a true return immediately leaves the bridge without fallback telemetry. |
| 3 | `...:250-260` | The first fast-read recorder runs only after composition declined. |
| 4 | `...:278-286` | `observed_selector/service` and controlled STOP are written only on that same declined path. |
| 5 | `bx_ntvdm_boot_namespace_plane_v1.c:81` | A valid namespace `50:42` can dispatch the source-derived provider, prepare a checked bulk write and return a typed result. |

## Disposition

S113 may add a default-off test-only post-commit marker owned by `bx-vdm`.
It must be written after the typed provider result and its mechanical action
have been accepted, and it must not read guest data, alter the outcome or
broaden the adapter ABI. A future single-run task can then distinguish an
accepted fast-read from the retained declined fallback event.
