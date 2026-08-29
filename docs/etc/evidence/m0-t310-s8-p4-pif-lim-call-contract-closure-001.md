# M0 T310 S8 P4 PIF/LIM call-contract closure

## Source result

`nt_msscs.c` calls two selected original providers:

- `GetPIFConfigFiles(BOOL, char *)` in `host/src/nt_pif.c`, declared by
  `host/inc/nt_pif.h`;
- `lim_page_frame_init(PLIM_CONFIG_DATA)` in `base/dos/emm_mngr.c`.

The host source omitted `nt_pif.h`; the historical EMS source exposes no
selected public declaration for its page-frame initializer. The host source now
includes the former and publishes the latter's exact original boolean
contract. Both provider bodies, initialization sequence, EMS page-frame
algorithm, and PIF behavior remain original.

## Validation

After cleaning `obj/host/nt_msscs.obj`, the formal Ninja graphs rebuilt
`original-softpc-host-roots.lib` and `original-softpc-dos.lib` on x64 and x86.
Each architecture reached both archive steps without the two reviewed C4013
records or an error. Other existing diagnostics remain visible.

## Boundary

This is direct original-owner binding only. It adds no shim, mapping-manager
operation, guest address conversion, or warning suppression.
