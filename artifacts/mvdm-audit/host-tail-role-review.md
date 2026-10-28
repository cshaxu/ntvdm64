# T473 S10: seven remaining NT host source roles

This checkpoint classifies physical source roles; it does not claim full COM,
frontend, floppy or debugger behavior. The [observer](host-tail-image-observer.py)
joins each unchanged file to the frozen DOS16 catalog and both WOW32 audit
editions, then checks the current build graph, object dependencies, map and
linked machine-code bytes. Its [record](host-tail-image-observations.json)
binds the exact image and source hashes. Later releases supersede that image;
they must not be rolled back merely to reproduce this historical record.

| Source | Decision and evidence | Remaining boundary |
| --- | --- | --- |
| `fprt.c` | Partial. The two linked, three-byte production implementations are original no-op diagnostic printing bodies; the non-PROD pipe and prompt code is not active. | Establish every production caller's diagnostic expectations; do not present a disabled debug channel as restored product output. |
| `nt_det.c` | Partial. Both original console-registration bodies are linked and both relocation targets resolve to the project-owned `RegisterConsoleVDM` adapter. | Validate registration identity, failed initialization, teardown, reentry and modern frontend handoff; machine-code binding alone is not a behavior proof. |
| `nt_ntfun.c` | Partial. Eleven original COM status, line-control and native IOCTL bodies are linked. | Real external serial peer, CTS/XOFF, immediate and pending failures, cancellation and handle lifecycle remain open; the existing mock substitutes these bodies. |
| `nt_wcom.c` | Partial. Three original UART entrypoints are linked; the original WOW32 `GetCommHandle` and `GetCommShadowMSR` exports are present. | Callback installation, actual guest UART paths, cross-thread lifetime, error and teardown behavior still require proof. Export existence is not proof the function pointers are connected. |
| `nt_hunt.c` | Excluded debug profile. It is `HUNTER`-conditional, omitted from the selected NT recipe and build; its trapping UI is not the release debugger contract. | Normal guest debugging and error handling remain separate obligations. |
| `nt_vflop.c` | Excluded historical alternative. It implements an unselected OS/2-era fixed `c:\softpc\pctool.A` virtual floppy; the selected NT recipe instead includes the original GFI and real-floppy provider. | This does not establish image-backed floppy capability or waive selected disk/media and error tests. |
| `nt_yoda.c` | Excluded debug profile. YODA is disabled; its selected production object contains only an empty stub and the production startup object has no call to it. | This does not exclude guest debug/exception semantics owned elsewhere. |

All seven files are byte-identical to their primary original. Accordingly no
mirror-diff restoration percentage is claimed. The four selected roles remain
partial because their operational and lifecycle evidence is incomplete; the
three exclusions apply only to the identified historical profile, not related
product capabilities. No runtime image or guest was changed by this audit.
