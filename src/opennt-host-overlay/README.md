# opennt-host-overlay

Private implementation and declaration fragments for registered OpenNT-host
mirror divergences.  It has no public ABI: only `opennt-host` may include or
call it.  It preserves the source-first mirror rule by isolating unavailable
NT4 product-composition bindings from otherwise original OpenNT bodies.

| ID | Original purpose | Private divergence | Mirror boundary |
| --- | --- | --- | --- |
| `HOST-DIV-012`, `HOST-DIV-014` | `nt_error.c` uses product TLS/resources/dialog plumbing. | Same-shaped adapter-win32 macro bindings are private; no host error algorithm is replaced. | `softpc.new/host/src/nt_error.c` includes `nt_error_bindings.h`. |
| `HOST-DIV-017` | `VrInitialized` is a VDMREDIR DLL export. | Static-provider link-name binding is private. | `vdmredir/vrinit.c` includes `vrinit_bindings.h`. |
| `HOST-DIV-024`, `HOST-DIV-025` | `vrnmpipe.c` obtains DLL and product manifests from NT4 product headers. | The admitted static-DLL selector and pure reached manifests live privately; no pipe algorithm is reimplemented. | `vdmredir/vrnmpipe.c` includes `vrnmpipe_product_seams.h`. |
