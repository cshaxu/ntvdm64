# OpenNT DOSX / DPMI Build Record

## Scope

This record preserves the pre-rebase x86 DOSX descriptor investigation in the OpenNT tree at revision
`5c5b979ec08c17d3ca2eb70e8aad62d26515d01c`:
`base/mvdm/dpmi/makefile`, `dosxi.lnk`, and its `486` source selection. Its
old WLink result is retained as historical diagnostic evidence only. It is
superseded as current build status by the unchanged-source OpenNT tools16
rebuild recorded in `HISTORICAL-TOOLCHAIN-REBASE-2026-08-07.md`; that rebuild
does not claim that DPMI is loadable or runnable.

## Original Build Shape

The x86 makefile sets these compile-time definitions:

```text
DEBUG=0 DBG=0 WOW WOW_x86 i386 FLATAPIXLAT XMEMNT MD
```

It declares 22 objects, links them in `486/` through `dosxi.lnk`, then uses
historical `LINK16 /EXEPACK /FAR /MAP /CP:1`. The selected modules cover the
DPMI startup path, BOP initialization, INT 31h services, protected/real-mode
interrupt reflection, selector and memory handling, A20 handling, DOSX boot,
and the no-387 NPX exception path.

## Assembly Result

The stage at `toolchain-probe/dosx-stage-v1` copied only OpenNT sources from
`dpmi`, `mvdm/inc`, and `mvdm/wow16/inc`. The OpenNT-source-built MASM probe
accepts the source macro dialect, but crashes when those original defines are
passed on its command line. A stage-only `dosx-build-config.inc` provides the
same values before each of the 22 original sources; it is a tool invocation
workaround, not a behavior change.

All 22 declared modules assembled with zero warning and severe errors:

```text
dxstrt dxmain dxintr dxfunc dxutil dxmmgr dxend dxboot dxmsg dxbug
dxnetbio dxint31 dxdisk dxini dxoem dxemm dxendpm dxfind dxdma dxemm2
ntnpxem dxrom
```

The final per-module result is retained in
`toolchain-probe/dosx-stage-v1/base/mvdm/dpmi/dosx-final-assembly-audit.json`:
22/22 modules have exit code zero. The earlier `dosx-assembly-audit.json`
records the useful first-pass 18/22 diagnostic and is not the final result.

Three source-closure adjustments were necessary and are confined to the
disposable stage:

1. `vdmtib.inc` is declared as a generated input, but its specified source
   `base/inc/vdm.h` is absent from this snapshot. The checked-in
   `mvdm/inc/VDMTIB.INC.old` was copied byte-for-byte to the expected name.
   SHA-256: `675CEEC6E626A6C05A1BAE513D3712A1C216BFFBE483FEC6B86E06B17E8F11D7`.
2. `486/dxint31.asm` includes the generated `dpmi.inc` twice. The historical
   assembler tolerates this; the probe does not. A stage-only include guard
   retains the first expansion exactly and suppresses the redundant second
   one.
3. `ntnpxem.asm` refers to the build-machine path
   `\\nt\\public\\sdk\\inc\\ks386.inc`. The corresponding checked-in OpenNT public
   SDK assembly definition is `public/sdk/inc/ksx86.inc`; it contains all
   `Cs*` i386 CONTEXT offsets used by the module. It was copied unchanged
   (SHA-256 `A209B77DDE4542B0BC40C73333A44B8B70FD20933535991878A84546DB845190`)
   and the stage source references that local file.

## Link Boundary

All objects were emitted into the historical `486/` link directory and passed
to OpenWatcom WLink in the exact `dosxi.lnk` order. WLink accepted the mixed
OMF input and reported exactly one undefined symbol:

```text
PMINTELVECTOR is an undefined reference
```

The makefile-selected `486/dxintr.asm` does not define that table. Its root
directory sibling `dpmi/dxintr.asm` does define `PMIntelVector`, but compiling
that alternate source requires `VDMTIB_Esp`, a member absent from the
checked-in `VDMTIB.INC.old`.

The local OpenNT object database retains an unreferenced historical
`public/internal/base/inc/vdmtib.inc` (blob
`1ef6ae88b801348aec2220218309832c248e656e`, reachable from local dangling
commit `69a14b6a16a918dee360f471854cceed4b28914e`). It is source-backed and was
examined without fetching external material, but it uses newer `Vp*` offset
names and also does not define `VDMTIB_Esp`. It therefore cannot legally make
the root alternative ABI-compatible. An alternate object or a hand-copied
table remains inadmissible: either would silently choose a VDM ABI without a
matching descriptor.

### NT4.5 Cross-Check

The local NT4.5 source tree was examined strictly as a reference, not as an
input to the fixed NT4 build. Its `dpmi/486/dxintr.asm` is byte-identical to
the NT4 selected source (SHA-256
`23B221750FD0E7525E508C7388DF482FF3016694FDFC53801F320FCD18E6D4C0`), and
its root `dpmi/dxintr.asm` is likewise byte-identical to the NT4 root sibling
(SHA-256 `EEE4DB25227BDD18C553E0AFE6078F0C5101F12FF50C948D266413A77FA1D333`).
The x86 makefile explicitly builds only `486/dxintr.asm` as part of the
22-object `OBJS486` set. The root sibling is not a dropped 23rd object.

The root sibling does define `PMIntelVector`, but its WOW path reads
`VDMTIB_Esp`. Both the NT4 `VDMTIB.INC.old` source used by the stage and the
corresponding NT4.5 `mvdm/inc/VDMTIB.INC` define the same
`VdmPmStackInfo` members through `VDMTIB_DosxFaultIretD` and contain no
`VDMTIB_Esp` member. NT4.5 therefore corroborates the mismatch rather than
closing it. It is inadmissible to mix that root source, invent the missing
field, or add a hand-written `PMIntelVector` merely to satisfy WLink: each
would choose an unproven VDM/monitor ABI while claiming a fixed NT4 artifact.

The supported conclusion is narrower but firmer: the checked source is enough
to assemble the selected DOSX objects and identify the missing descriptor
contract, but not enough to reconstruct the final x86 link closure.

### Reproducible Link Recheck

On 2026-08-07, the existing `dosx-stage-v1` was rechecked with the local
Open Watcom linker. `recheck-opennt-dosx.ps1` parses the historical
LINK16-format `dosxi.lnk` as a 22-object list and emits WLink's required
explicit `file` list in the same order. It does not modify or supplement that
list. The recheck exits nonzero as expected and records exactly:

```text
Error! E2028: PMINTELVECTOR is an undefined reference
```

This proves the recorded unresolved symbol is current and descriptor-faithful,
not a stale log or an artefact of feeding LINK16 syntax directly to WLink.

The first WLink pass consequently establishes that the object format is
accepted, but does **not** produce a DOSX executable. Historical EXEPACK has
also not been reproduced; it must not be bypassed when claiming a final
`DOSX.EXE` result.

## Research Consequence

The DPMI code makes the required external machine contract concrete: it
initializes through `BOP_DPMI/InitDosx`, assumes protected-mode transitions and
interrupt reflection, accesses VDM state through `VDMTIB`, depends on A20 and
memory services, and uses DPMI INT 31h. The source build itself is therefore
evidence for the need for a non-product-specific monitor/mantle service
surface; it is not evidence that a current `nvtdm` implementation provides it.

## Required Unblock

Resolve the `PMIntelVector` descriptor mismatch with the exact matching x86
interrupt-provider selection or a source-complete build descriptor. The
recovered local VDM include is useful provenance evidence but does not satisfy
the root `dxintr.asm` ABI. Only after that contract is closed should linking be
repeated with a linker/packer equivalent to historical LINK16 plus EXEPACK.
