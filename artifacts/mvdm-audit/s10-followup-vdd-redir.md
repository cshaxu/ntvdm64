# S10 follow-up: sample VDDs and redirector diagnostics

Scope is the 13 previously pending host-candidate C/H rows under `vdd/samples`
and `vdmredir` in `host-ch-scope/inventory.csv`. This is a physical source-role
assessment, not permission to remove source or a claim that a whole capability
is absent. No product, guest, build graph, shared ledger or test was changed.

## Provenance and composition

All 13 current files were freshly SHA-256 checked against the current, OpenNT
and OpenNT-4.5 hashes recorded in `frozen-file-reconciliation.csv`: each
matches all three. That frozen join has one DOS catalog row and two WOW catalog
rows for each; those catalog appearances do not themselves establish selection
or execution. These are source-retention results, not behavioral recovery rates.
The frozen reconciliation marks the ten sample files not directly compiled and
not product selected. A read-only search of the existing Ninja graphs and CMake
source definitions found no sample-VDD compile input. The two diagnostic C files
are worker-graph and VDMREDIR link inputs, not uncompiled sources. The named
original `sources` recipes below establish independent DLL ownership; they do
not establish that current releases package those DLLs.

## Per-file physical dispositions

`Excluded` below means excluded only from the observed selected runtime role;
source must remain available as upstream material. `Partial` means a live
source/header role exists or cannot be fully assessed. None is declared full.

| Path beneath `src/mvdm/` | Disposition and bounded evidence |
| --- | --- |
| `vdd/samples/adlibvdd/vdd.c` | Excluded sample DLL body; original `adlibvdd/sources` names its own target, no selected graph input. Actual AdLib behavior/provider not established by this exclusion. |
| `vdd/samples/adlibvdd/vdd.h` | Excluded private interface of that unselected independent sample; not a general VDD API exclusion. |
| `vdd/samples/mscdex/vdd/mscdex.h` | Excluded private interface of unselected VCDEX sample. CD-ROM/MSCDEX service capability remains open. |
| `vdd/samples/mscdex/vdd/vcdex.c` | Excluded sample DLL body; `mscdex/vdd/sources` selects `vcdex` DYNLINK and source implements CD-ROM APIs. No conclusion that a different CD-ROM provider is equivalent. |
| `vdd/samples/mscdex/vdd/vcdex.h` | Excluded private VCDEX interface together with that sample; not the product's CD-ROM interface. |
| `vdd/samples/sample1/fax32/fax32.c` | Excluded separate sample FAX32 VDD body, original `sample1/fax32/sources` target `fax32`; no selected graph edge. |
| `vdd/samples/sample1/fax32/fax32.h` | Excluded sample-private header, not the VDD registration/user-hook contract. |
| `vdd/samples/sample2/vdd/vdd.c` | Excluded separate demonstration VDD body, original `sample2/vdd/sources` target `vdd`; no selected graph edge. |
| `vdd/samples/sample2/vdd/vdd.h` | Excluded sample-private header, not the actual shared VDD ABI. |
| `vdd/samples/vsbd/vdd.c` | Excluded sample DLL body; `vsbd/sources` names DYNLINK target `vsbd`, with NTVDM and WinMM dependencies. No proof of sound-device equivalence. |
| `vdmredir/vrdebug.c` | Excluded only as executable debug behavior in the observed x86 release. Selected graph compiles and links its object; `cross-369/redir-debug-provider-disposition.md` and its object-symbol observations prove the `DBG` body is inactive there. Other profiles remain unproven. |
| `vdmredir/vrdlcdbg.c` | Same narrowly bounded x86-release disposition: linked object exists, but observed `DBG`-guarded diagnostic implementation contributes no normal code/data symbols. Not a DLC functionality exclusion. |
| `vdmredir/vrdlcdbg.h` | Partial: original diagnostic declarations and CCB constants; included by selected `vrdlc5c.c`, `vrdlcbuf.c`, `vrdlcpst.c` and diagnostic source. Header compile dependency is not proof every constant or diagnostic call is active; preserve and review actual expansion/consumption. |

## Provider, boundaries and handoff

The sample DLLs are separate original components, not replacements for the
selected host's generic VDD machinery. `softpc.new/host/src/nt_vdd.c` owns the
current selected VDD adapter/port/DMA mechanism; `nt_msscs.c` contains the
original installable-VDD registration/loading path. Their executable reachability,
DLL registration, user-hook lifecycle and exact current release policy are not
proved for any named sample by a recipe or a source match. The two proved
generic adapter-uninstall and DMA channel boundary defects remain assigned to
S13 under `vdd-native-role-review.md`; sample exclusion must not hide them.
S13 should also retain device-specific sound/CD-ROM ownership and real VDD
loading/teardown questions until actual selected providers and guest contracts
are traced and verified. In particular no claim is made that an independent
sample is required in the shipped product, or that another provider supplies
the same observable behavior.

The selected VDMREDIR service paths have their own dispatch and DLC/NetBIOS/
pipe providers; excluding diagnostic executable bodies does not exclude those
providers or prove their contracts. `vrdlcdbg.h`'s consumer and active macro
surface and any debug-profile behavior belong to the remaining bounded
redirector package investigation, S17, together with source/link and runtime
proof; it should not be declared full from includes alone. Preserve unknowns
for failure, concurrency and retirement checks. No sample-specific behavioral
restoration, functional test, guest execution or debug-profile verification was
performed in this follow-up.

The coordinator can apply the above dispositions to the shared inventory only
after reconciling its current revision and disjoint shard ownership. Existing
catalog provenance remains unchanged; no executable mirror diff is added or
removed, and 100% identical source bytes is not a quantified behavior result.
