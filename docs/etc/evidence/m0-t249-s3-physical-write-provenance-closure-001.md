# M0 T249 S3 — physical-write provenance closure

## Result

The first runtime modification of the watched low NTIO aperture is an ordinary
guest CPU write, not a host image copy, BOP result action, or missing Bochs
machine service:

```text
physical-write address=a87 bytes=1 captured=1 sequence=37093
cs=9386 base=93860 eip=7424 ss=8e08 sp=05fa data=ff
```

The existing CS base makes its executed physical position `0x9AC84`.  The
primary original-toolchain `NTDOS.MAP` declares its relocatable `DOSCODE`
range as `0x00000..0x07938`; `EIP=0x7424` is inside that original NTDOS code
range.  Therefore the writer is original guest NTDOS runtime code executing
after relocation.  It is not owned by `bx-core`, `bx-mantle`, `bx-vdm`, a
selector/service provider, or host composition.

This result is deliberately an ownership attribution, not a claim that the
link map has enough symbol detail to name the precise assembly label at
`DOSCODE+0x7424`.

## Mechanical observation

The default-off `BX-TRACE-092` exception records only a caller-selected fixed
physical range, the immediate write bytes, and existing CPU position facts.
It is selector-blind and makes no guest read, write filtering, result change,
or stop request.  The original `writePhysicalPage` slow path is insufficient
for normal guest execution because Bochs' existing TLB fast path writes RAM
directly; the hook therefore also follows the existing generic
`BX_NOTIFY_LIN_MEMORY_ACCESS` notification for writes.  No BOP/DOS/OpenNT
term is introduced in either Bochs source file.

## Verification

The physical-write fixture from the formal MSVC x64 `/MT`, CPU5/P-MMX graph
passes (`exit=0`).  It covers unconfigured/invalid rejection, non-overlap,
first overlap capture, no replacement by a later overlap, reset, copied
position facts (including CS base), and unchanged ordinary RAM writes.

The diagnostic-enabled native graph `build/M0-T249-S3/formal-r1` ran the
source-built DOS/WOW inputs with the one-million instruction budget and
exited `4` only for that declared budget.  It also retained the existing
accepted NTDOS EXEC entry:

```text
ntdos-exec-entry observed=1 read-failed=0 cs=04ea eip=00005a70 pdb=0bf2
terminal-position cs=5859 eip=000078b0
terminal=4 detail=0 lifecycle=5 presentation=5 cancellation=0 budget=1000000
```

The separate default-off formal graph `build/M0-T249-S3/formal-default-r1`
is the normal product graph: it contains no physical-write diagnostic define
and links `bin/ntdos64-native.exe` successfully.  Thus the new source compiles
and links both armed and unarmed, while unarmed code emits no observer call.

## Disposition

The old unknown-writer premise is closed.  `BOP-DEPENDENCY-115` remains
partial only because the original guest has not completed `$Exit/$Abort` and
ordinary parent return.  The next question is the original NTDOS/NTIO runtime
control flow after the now-attributed write and published far target; it is
not a DEM/COMMAND BOP leaf, adapter workaround, or machine-feature request.
