# M0 T318 S2 P27 — Retire unselected BOP ingress subset

## Finding

`mvdm-host-overlay/softpc.new/host/src/nt_bop_dem_command.c` was an earlier
hand-maintained subset for `MS_bop_0` and `MS_bop_4`. It was not a member of
the selected CPU40 Ninja graph: the formal graph selects original
`mvdm-host/softpc.new/host/src/nt_bop.c` as `obj/host/nt_bop.obj`, and has no
rule or library input for the subset.

The selected original translation unit contains the full established ingress:

- `MS_bop_0` reads the DOS service byte, calls `DemDispatch`, advances IP and
  invokes the original idle notification condition;
- `MS_bop_4` reads the COMMAND service byte, calls `CmdDispatch`, then
  advances IP.

## Disposition

The unselected subset, its isolated fixture/generator, and its stale
`MVDM-HOST-DIV-017` registry entry are removed. This is a removal of duplicate,
non-production reconstruction code; it does not replace, alter, or newly
enable the selected original BOP path.

The P26 observation gate consequently refers only to the original
`nt_bop.c` ingress. Any later passive reach observation must attach to that
single selected implementation or remain external to the product. It must not
restore a second DEM/COMMAND ingress subset.
