# opennt-host-overlay

Private implementation and declaration fragments for registered OpenNT-host
mirror divergences.  It has no public ABI: only `opennt-host` may include or
call it.  It preserves the source-first mirror rule by isolating unavailable
NT4 product-composition bindings from otherwise original OpenNT bodies.

| ID | Original purpose | Private divergence | Mirror boundary |
| --- | --- | --- | --- |
| `HOST-DIV-012`, `HOST-DIV-013`, `HOST-DIV-014`, `HOST-DIV-026`, `HOST-DIV-027` | `nt_error.c` uses product TLS/resources/dialog plumbing and includes unadmitted product-shell paths. | The admitted original true subset and same-shaped adapter-win32 bindings are private because the subset exceeds the mirror threshold. | `softpc.new/host/src/nt_error.c` includes `nt_error.c` and invokes only its same-signature entry. |
| `HOST-DIV-017`, `HOST-DIV-022` | `VrInitialized` is a VDMREDIR DLL export within a broader product initializer. | The reached original subset and static-provider binding are private because the subset exceeds the mirror threshold. | `vdmredir/vrinit.c` includes `vrinit.c` and invokes only its same-signature provider boundary. |
| `HOST-DIV-015`, `HOST-DIV-016`, `HOST-DIV-020`, `HOST-DIV-023`, `HOST-DIV-024`, `HOST-DIV-025`, `HOST-DIV-028` | `vrnmpipe.c` combines synchronous routines with unadmitted async/VDD product machinery. | The admitted synchronous original subset and pure manifests are private because the subset exceeds the mirror threshold; public wrappers keep original names/parameters/results. | `vdmredir/vrnmpipe.c` includes `vrnmpipe.c` and invokes only same-signature entries. |
| `HOST-DIV-030` | `nt_event.c` needs a new bounded observation record for the reached non-graphical event transitions. | The state record has no original NT4 equivalent and exceeds the mirror-change allowance. | Keep it private; `nt_event.c` exposes only narrow declared state accessors and the original host entry names. |
