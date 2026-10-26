# M0 T473 S1 — whole-closure baseline

## Method and reproducibility

`tools/audit/Export-MvdmWholeClosureInventory.mjs` compares every file below the selected OpenNT `base/mvdm`, OpenNT-4.5 `nt/private/mvdm`, and current `src/mvdm` roots. It hashes bytes, rejects symbolic links, permits output only below `build/`, and deliberately does not select an upstream conflict or claim build/link/runtime reachability.

Two runs (`build/t473-s1/mvdm-union-node-r1` and `...-r2`) produced byte-equal CSV inventories, SHA-256 `E61F17A52FE5E318FFF2BC6E207D1FB5F7B1F79DF98C2EC1E62E113B9D452BCF`.

## Source-union facts

| Classification | Paths |
| --- | ---: |
| Both upstream trees, byte-identical | 3,513 |
| Both upstream trees, different bytes | 6 |
| OpenNT only | 1 |
| OpenNT-4.5 only | 151 |
| Current-only | 1,013 |
| **Union total** | **4,684** |

The six edition conflicts are all Win16 build/declaration inputs under `wow16/`; current bytes select the OpenNT copy. S1 records that fact but does not yet accept the edition choice. Their role is load/build evidence, not a host-runtime provider.

Current identity is 3,271 exact OpenNT/identical files, 125 exact OpenNT-4.5/identical files, six exact OpenNT conflict selections, 203 mirror-different files, 66 not-currently-mirrored paths, and 1,013 current-only paths.

### Safe non-runtime exclusions, not repair candidates

All 66 not-currently-mirrored paths have a known category: 33 `tools16` compiler/build products and 33 `v86` CPU30 monitor/scaffold files. The latter is the prohibited retired kernel-VDM executor; the former is not a product host link input. S1 therefore does not treat either group as a missing runtime library.

Of the 1,013 current-only paths, 836 are retained historical `.obj` files. The remainder is overwhelmingly build/media/tool input; only seven source-like files require provenance review (`softpc.new/obj.vdm/obj/i386/ntvdm.def`, `wow32/obj/i386/wow32.def`, and five WOW16 assembly inputs). They are not silently classified as autonomous code.

## Actual current composition, not inferred reachability

The current x86 worker Ninja graph directly compiles 367 `src/mvdm` edges: 297 SoftPC, 27 DOS, 16 DPMI32, 16 Redirector, seven XMS, two OEMUNI, one SubAlloc and one debugger edge. The separate WOW32 graph directly compiles 78 original WOW32 edges. This is a selection baseline, not proof that every archive member is linked, loaded, reached, or behaviorally complete.

## Finite S1 review worklist

The 203 mirror differences contain 202 source-like files. 55 have no inline `DIVERGENCE(...)` marker; 16 also have no direct current `src/mvdm/README.md` path/leaf reference:

```text
dos/command/cmd.c; dos/v86/cmd/command/{init,rdata,stub}.asm;
dpmi32/{dpmiint.h,dpmimscr.c,vxd.c,xlathlp.h};
softpc.new/base/ccpu386/{c_debug.h,iret.c,ret.c};
softpc.new/host/inc/insignia.h;
wow32/{winsockp.h,wkglobal.c,wklocal.c,wow32.def}
```

Some are line-ending-only or declaration/build-carrier differences; others are known functional adaptations such as system-root projection, CPU40 stack selection, FastBOP, registry binding, and Win16 Winsock task cleanup. None is assumed justified merely because it currently builds. The remaining S1 work is to map these 16 and the other 187 differences to a source owner, registered divergence or a removal candidate, then to reconcile manifest selection, archive/link participation, initialization, teardown and runtime evidence by complete owner package.
