# OpenNT MVDM Declared-Output Triage

Status: G0.4 first-pass reconciliation complete, 2026-08-07. This is a classification
of the 75 build-descriptor declarations in
`opennt-mvdm-artifact-inventory.json`. A declaration is not evidence of a
retail binary, a runnable component, or a default build target.

## Counting Rule

The inventory has 194 descriptors and 75 declared-output rows. Those rows
reduce to 72 distinct declaration strings because `dest=ntio.sys` appears in
two historical makefiles and `TARGETNAME=ntvdm` appears in three product
compositions. The duplicate rows are retained here because their ownership is
different.

The current state vocabulary is deliberately narrow:

- `source-built`: the exact direct DOS payload is recorded in the controlled
  historical guest closure.
- `blocked`: source exists but the declaration has no independently verified
  final artifact under the fixed profile. The stated reason is part of the
  result, not a request to manufacture a replacement.
- `sample/development-only`: not a runtime input for the selected NT4 DOS
  profile.

## Direct DOS Declarations: 20 Rows

| Declaration rows | Classification | State | Evidence or exact blocker |
| --- | --- | --- | --- |
| `dest=ansi.sys`, `dest=append.exe`, `dest=command.com`, `dest=debug.exe`, `dest=edlin.exe`, `dest=exe2bin.exe`, `dest=fastopen.exe`, `dest=himem.sys`, `dest=kb16.com`, `dest=keyboard.sys`, `dest=loadfix.com`, `dest=mem.exe`, `dest=nlsfunc.exe`, `dest=ntdos.sys`, `dest=setver.exe`, `dest=share.exe`, `target=$(OBJPATH)\redir.exe` | NT4 DOS payload | `source-built` | Per-artifact stages in `NT4-GUEST-ARTIFACT-CLOSURE.md`. |
| `dest=ntio.sys` from `dos/v86/doskrnl/bios/makefile`; `dest=ntio.sys` from `softpc.new/bios/makefile` | NT4 DOS payload, two historical build owners | `source-built` | One artifact, two retained declarations; the controlled NTIO stage records the selected source/build path. |
| `dest=.` from `dpmi/makefile` | NT4 DOS payload, `DOSX.EXE` | `source-built` | The makefile's `all` target resolves the placeholder to `486\dosx.exe`; `dosx-tools16-historical-v6` records its unchanged-source MASM/LINK16 build and current output hash. Runtime/DPMI support remains out of profile. |

## Indirect Direct-Guest Install Rules: 14 Artifacts

These artifacts are outside the explicit 75-row declaration form, but their
original makefiles resolve their install ownership. Together with the 19
unique source-built artifacts represented by the declarations above, they
reconcile to the 33-file direct DOS payload closure.

| Owner and original rule | Artifacts | State |
| --- | --- | --- |
| `dos/v86/dev/country/makefile`: `all: country.sys`, then `binplace country.sys` | `COUNTRY.SYS` | `source-built` |
| `dos/v86/cmd/graphics/makefile`: `dest1=graphics.com`, `dest2=graphics.pro` | `GRAPHICS.COM`; `GRAPHICS.PRO` | `source-built`; `static companion` |
| `v86/util/makefile`: `all: quit.com` | `QUIT.COM` | `source-built` |
| `dos/v86/cmd/backup/makefile`: `binplace backup.exe` | `BACKUP.EXE` | `external reference` |
| `dos/v86/cmd/qbasic/makefile`: nine explicit `binplace` rules | `QBASIC.EXE`, `MSHERC.COM`, `EDIT.COM`; `QBASIC.HLP`, `EDIT.HLP`, `GORILLA.BAS`, `NIBBLES.BAS`, `MONEY.BAS`, `REMLINE.BAS` | three `external reference`; six `static companion` |

`external reference` is intentional: the fixed tree provides only binplace
rules for those four binaries. No stage or report may relabel them as
source-built until a complete provenance-matching source/build rule exists.

## Historical VDM Host/Support: 29 Rows

All rows below are `VDM host/support` and `blocked`: they are source/object
closure candidates, not independently verified final artifacts. Their common
blocker is that a historical library or product `TARGETNAME` does not define a
standalone installed file; final composition remains gated by the unrecovered
x86 direct-monitor generated ABI and, for product shells, excluded legacy NT
integration.

```text
TARGETNAME=bde          TARGETNAME=bios         TARGETNAME=ccpu386
TARGETNAME=command      TARGETNAME=comms        TARGETNAME=Cvidc
TARGETNAME=dbg          TARGETNAME=debug        TARGETNAME=dem
TARGETNAME=disks        TARGETNAME=dos          TARGETNAME=dpmi32
TARGETNAME=keymouse     TARGETNAME=monitor      TARGETNAME=oemuni
TARGETNAME=resource     TARGETNAME=sim32        TARGETNAME=src
TARGETNAME=suballoc     TARGETNAME=support      TARGETNAME=system
TARGETNAME=unix         TARGETNAME=vdmdebug     TARGETNAME=vdmredir
TARGETNAME=video        TARGETNAME=xms486
TARGETNAME=ntvdm  (ieuvddex/sources)
TARGETNAME=ntvdm  (softpc.new/obj.vdm/sources)
TARGETNAME=ntvdm  (v86/scaffold/sources)
```

This state does not discard the compiled original object evidence. In
particular, the R5 closure compiled selected CCPU, BIOS, BOP, DEM, and provider
groups, but it did not produce a verified historical host executable and must
not be represented as one.

The three `ntvdm` declarations are separately resolved in
`OPENNT-NTVDM-PRODUCT-COMPOSITION-TRIAGE.md`; they are not alternate names for
one source target or one executable artifact.

## Build-Time Tools: 8 Rows

The following are `sample/development-only` for the research payload. They may
be historical build inputs, but are not NT4 DOS guest or host runtime files:

```text
TARGETNAME=buildidx     TARGETNAME=convert      TARGETNAME=dasm386
TARGETNAME=dat2obj      TARGETNAME=getsize      TARGETNAME=rename
TARGETNAME=stripdd      TARGETNAME=stripz
```

## Samples And Tests: 9 Rows

The following are `sample/development-only`; no build attempt is admitted by
the selected profile:

```text
TARGETNAME=adlibvdd     TARGETNAME=ddemlcl      TARGETNAME=ddemlsv
TARGETNAME=ddemo        TARGETNAME=ddestrs      TARGETNAME=fax32
TARGETNAME=vcdex        TARGETNAME=vdd          TARGETNAME=vsbd
```

## Utilities, Debuggers, And WOW: 9 Rows

The first six rows are `VDM host/support` and `blocked`: they are separate
historical product utilities or debugger surfaces with no fixed-profile
installation/runtime admission. The final three rows are `WOW16 dependency`
and `blocked`: they are not DOS payloads and require a separately selected
WOW runtime profile.

```text
TARGETNAME=forcedos     TARGETNAME=graftabl     TARGETNAME=pifedit
TARGETNAME=vdmdbg       TARGETNAME=vdmexts      TARGETNAME=win

TARGETNAME=wow32        TARGETNAME=wowfax       TARGETNAME=wowfaxui
```

## Consequence

G0.4 is closed as a first-pass inventory and provenance exercise. It does not
authorize a broad attempt to build the 29 blocked host targets. Their next
use is evidence extraction: split a specific historical dependency into an
owned-monitor requirement, an optional host capability, or an out-of-profile
historical product only when a bounded execution trace needs that distinction.
