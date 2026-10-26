# M0 T473 S1 — whole-closure baseline

## Method and reproducibility

`tools/audit/Export-MvdmWholeClosureInventory.mjs` compares every file below the selected OpenNT `base/mvdm`, OpenNT-4.5 `nt/private/mvdm`, and current `src/mvdm` roots. It hashes bytes, rejects symbolic links, permits output only below `build/`, and deliberately does not select an upstream conflict or claim runtime reachability.

Runs `r20` and `r21` of the original identity inventory were byte-identical, SHA-256 `265C7323F903E6275BEC6A4B078E19AA753D3E57C7AD17AA66FDA98C51A39A11`. The corrected product-selection inventory is `build/t473-s1/mvdm-union-node-r24`, SHA-256 `F582D45B0C6FE500B6E2E906B1842B92FDCA730FF964E8D83BB7CBF8611B3D13`. The normalizer changes only CRLF-to-LF and trailing horizontal whitespace; it never selects an edition, changes source, or claims runtime reachability.

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

Current identity is 3,271 exact OpenNT/identical files, 125 exact OpenNT-4.5/identical files, six exact OpenNT conflict selections, eight formatting-only OpenNT selections, 195 semantic mirror differences, 66 not-currently-mirrored paths, and 1,013 current-only paths.

### Safe non-runtime exclusions, not repair candidates

All 66 not-currently-mirrored paths have a known category: 33 `tools16` compiler/build products and 33 `v86` CPU30 monitor/scaffold files. The latter is the prohibited retired kernel-VDM executor; the former is not a product host link input. S1 therefore does not treat either group as a missing runtime library.

Of the 1,013 current-only paths, 836 are retained historical `.obj` files. The remainder is overwhelmingly build/media/tool input; only seven source-like files require provenance review (`softpc.new/obj.vdm/obj/i386/ntvdm.def`, `wow32/obj/i386/wow32.def`, and five WOW16 assembly inputs). They are not silently classified as autonomous code.

## Actual current composition, not inferred reachability

The canonical x86 Ninja graphs contain 368 unique `src/mvdm` paths in the worker graph and 78 in the WOW32 graph. This full-graph selection includes fixture edges. The corrected product selection is 350 current MVDM source paths in `ntvdm.exe`, 16 in the separate `VDMREDIR.dll` boundary, and 78 in `wow32.dll`. `ntvdm.exe` selection is checked against its explicit original-library `/wholearchive` contract; `VDMREDIR.dll` and WOW32 are direct product object selections. The two graph-only inputs are the debugger fixture `dbg/dbg.c` and the `VDMREDIR` module definition, neither of which is silently counted as a worker source provider.

The reproducible path ledger now assigns all 4,684 rows an S1 disposition:
444 selected product sources; 2,616 immutable guest/media/build inputs; 306
retained build artifacts; 119 tooling/build carriers; 33 unmirrored tool
inputs; 33 unmirrored retired CPU30-monitor inputs; two retained retired-CPU30
paths; and 1,131 unselected host-package rows explicitly queued for S2
disposition. The latter category is a finite worklist, not a statement that
those providers are optional or complete.

## Finite S1 review worklist

The 195 semantic mirror differences contain 194 source-like files. After removing three needless local deltas (an unused Registry include and two formatting-only adjustments), seven remaining source-like deltas have neither an inline `DIVERGENCE` marker nor a direct current `src/mvdm/README.md` leaf reference:

```text
dos/command/cmd.c;
dpmi32/{dpmiint.h,dpmimscr.c};
softpc.new/base/ccpu386/{iret.c,ret.c};
wow32/{winsockp.h,wow32.def}
```

The eight formatting-only paths are not semantic repair candidates. The remaining differences include declaration/build carriers and functional adaptations such as system-root projection, CPU40 stack selection, FastBOP, registry binding, and Win16 Winsock task cleanup. None is assumed justified merely because it currently builds. The remaining S1 work is to map these seven and the other 188 semantic differences to a source owner, registered divergence or a removal candidate, then to reconcile manifest selection, archive/link participation, initialization, teardown and runtime evidence by complete owner package.
