# Device, presentation and redirector provider ownership

## S14 P47

Source baseline 80fcf31d7. Complete bodies read: ntvdm-exe/softpc/
mvdm_softpc_mouse_bridge.c, mvdm_softpc_mouse_guest.c,
mvdm_softpc_text_video.c, mvdm_cvidc_vector_binding.c, mvdm_vm_ingress.c;
ntvdm-exe/win32/console_text.c, console_graphics.c; and
vdmredir-dll/source/mvdm_redirector_guest_copy.c. Supporting header and caller
snippets do not count as additional complete files. Source review only.

| Chain | Implementation destination and constraints |
| --- | --- |
| Mouse queue and scaling | Queue/relative sample scaling is independent mechanics eligible for B after removing D protocol coupling and C IRQ/lock calls. CPU-selected virtual geometry and original interrupt scheduling remain C. Current storage is inside F console_client, obtained through session TLS; outer composition owns its lifetime, but the core resource can use the existing B context without a new registry. Input-ready event is borrowed, not closed here. |
| Mouse delivery | Preserve submitted versus consumed active state, commit producer state only after queue admission, scale a sample once and retain fractional remainders only for matching source/guest dimensions. LEAVE is consumed while active so C restores the cursor background before retiring the route. Cancellation discards pending input without requiring a duplicate frontend ENTER. No polling or new IRQ scheduler. |
| Guest mouse | EmulateCoordinates, selected virtual_screen, original counters and mouse_pointer_route_changed belong C (nt_mouse/mouse_io owners). Saturation is only for the original narrow intermediate; guest coordinate bounds remain original. Producer must not read VM-owned geometry. F INPUT_RECORD decoding/admission must be separated from this C delivery, not moved wholesale into B. |
| Text snapshot | Font-bank indices, interleaved EGA planes, glyph slots, cursor and mode state belong C's original video owners. Snapshot both selected font banks and downloaded data; do not replace with ROM fonts. C may expose a copied snapshot; D wire packing and F frontend publication must consume it from outside. |
| Text route | Current flush/sync mixes C sc state/update_alg with F async publication and frontend preference. Keep software VGA active regardless of presentation surface. Split inward route/snapshot mechanics from outer transport admission; neither hardware fullscreen nor guest mode writes are equivalent. Palette resolution remains original set_the_vlt, with current readiness semantics preserved. |
| Graphics resource | Bitmap/mutex, palette ownership, dirty/update depth, cursor count and overlay storage can become independent B resources after removing publisher/context acquisition. E worker_base publication and F route/endpoint admission remain outer. A B signal callback that still invokes E is not elimination. Capture must not publish midway through a body update; remove overlay before painter writes. |
| Graphics lifetime | Palette ownership transfers before repaint, even if publication fails. Initial missing palette keeps dirty state for later capture. Final publication/drain runs outside the bitmap lock because capture takes it. Only after users are joined may destruction release the bitmap, identity and installed palette. Preserve these lifetimes during resource extraction. |
| C-VID binding | Cpu/Sas/Video vectors, flag setters and qevJumpRestart are C machine wiring. Existing ev_glue.c setup_vga_globals is the calling assembly point. Consolidate into original C carriers with measured mirror cost; do not put C callbacks in B or reintroduce a CPU30 backend. Zero calibration and independent restart state are distinct. Generated binding includes still require slot-by-slot reconciliation before migration. |
| VM ingress | nt_bop.c invokes command_enter for selected services. CPU FAR-call setup belongs C; callback registration is currently tied to the product executor. Request capture, default host handles, launch preparation and executor submission remain F. No request exists before the guest callback establishes VM context; moving the admission chain into B would violate the owner's boundary test. |
| Redirector copy | OEM conversion/allocation are B mechanics; descriptor-to-linear translation remains C. Existing B guest_memory_access owns lease acquire/release. Preserve 4096-byte chunk commit and partial-failure semantics, overflow checks, zero-count rejection and OEM terminator/capacity behavior. The current comment says one lease, but the actual transfer loop can use several; implementation is the evidence. Avoid a B helper that secretly calls C effective-address conversion. |

Concrete callers checked: nt_graph.c flush/sync, nt_event.c queue reset/pending,
console_compat.c capacity reservation, vrnetapi.c name/copy consumers,
nt_bop.c command ingress and ev_glue.c vector binding. console_client.c provides
mouse_current through the current session and activates the E publisher.

Next implementation units after the full inventory: independent mouse/bitmap
resources in B; C guest-mouse/video snapshot and machine vector ownership;
outer route, publication and command orchestration. Exact interfaces, all
consumers, generated vector slots, mirror delta and final runtime tests remain
required. No changes to guest or production sources and no dependencies marked
eliminated. Eight bodies cover twenty open symbol records.
