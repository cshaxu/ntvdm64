# NTDOS Assembly Compatibility Ledger

## Scope

This records isolated JWasm assembly and linking passes over the complete
object list in `dos/v86/doskrnl/dos/msdos.lnk`. It is evidence for a
compatibility port, not a claim of retail byte equivalence or runtime
compatibility.

For COMMAND specifically, this is also diagnostic history: the later Gate 0
toolchain rebase proves that recovered MASM386 assembles all 34 original
objects without source transforms, while WLink output is rejected for LIDATA
loss. See `OPENNT-G0-TOOLCHAIN-REBASE.md`.

## Open Watcom Candidate Probe

The isolated Open Watcom 1.9 Windows toolchain is now available as a second
assembler/linker candidate. It has assembled the original NTIO `msend.asm` and
`msint13.asm` inputs with zero diagnostics and has produced a self-authored
16-bit MZ link smoke result. This is intentionally narrower than the existing
JWasm/UASM whole-list evidence: it does **not** yet establish that Open Watcom
can replace the selected assembler, `link16`, message generation, relocation,
or stripping path for any guest artifact. The fixed package checksum,
non-system installation, exact probe profiles, and acceptance boundary are
recorded in `docs/OPENWATCOM-16BIT-TOOLCHAIN-PROBE.md`.

The subsequent full NTIO BIOS-list audit reached only 7/12 modules on both a
copy-only original stage and the existing JWasm compatibility stage. Open
Watcom therefore remains a linker/limited-unit candidate, not the selected
complete assembler path; see the same probe record for the failing module set
and machine-readable evidence locations.

## Baseline Closure

`toolchain-probe/stage-opennt-ntdos.ps1` copies only the DOS kernel source
directory, V86 includes/messages and MVDM includes. `MSDOS.CL1` and
`MSDOS.CL2` are present in the source snapshot and are recorded as generated
inputs pending a separate `dosmsg` bootstrap.

The stage applies three already-evidenced, source-level compatibility rules:

- rename the legacy `invoke` macro and all calls in the DOS source/include
  closure, because JWasm reserves `INVOKE`;
- guard the repeated `BUFFINFO` definition and retain its 20-byte assertion;
- rename `SYSVAR.INC`'s otherwise-unused private `Buffinfo` type, which
  collides with `BUFFER.INC` under JWasm's case-insensitive model.

## Result

With those rules and the normal JWasm case-map, 22 of the 38 objects named by
the original link list assemble. The remaining errors fall into four classes:

| Class | Examples | Required next action |
| --- | --- | --- |
| Case-distinct local symbols | `DOS_Rename`/`DOS_RENAME`, `DevName`/`DEVNAME` | Establish a narrowly scoped symbol-renaming pass; `-Cp` alone fails because macro invocations themselves use mixed case. |
| Legacy generated local labels | `??0006` and related errors in long-jump macros | Reproduce the historical macro-local-label convention or substitute a tested compatible macro implementation. |
| Missing generated/configuration symbols | `MSVERSION`, `ERR_TABLE_21`, `COUNTRY_CDPG` | Derive the original message/config generation and assembler defines before source edits are considered. |
| Parser/operand differences | `JNC`, `JE`, `I_NEED` and segment-label forms | Classify each after the preceding macro and symbol layers are resolved; current diagnostics are not independent proof of a line-level defect. |

An experiment with JWasm `-Cp` (`OPTION CASEMAP:NONE`) demonstrated why it is
not accepted: it preserves case-distinct symbols but makes the OpenNT macros
named and invoked with different case spellings unavailable. The default mode
remains the current baseline until a targeted compatibility transformation is
specified and tested.

## Gate

Do not link a partial object set. The acceptance point is a fresh stage where
all 38 objects assemble with named transformations, followed by a separate
link, relocation and runtime-validation record.

## Fresh Per-Object Baseline Audit

`toolchain-probe/audit-opennt-ntdos-assembly.ps1` now creates a fresh-stage,
machine-readable `ntdos-assembly-audit.json` for the original 38-object link
list. The confirmed default-case-map JWasm baseline is 22 successful objects
and 16 failures; the tool captures every invocation's exit status and complete
diagnostics rather than relying on a truncated console log.

The largest shared failure class is the `LJ` long-conditional-jump macro in
`dosmac.inc`. Its intended semantics are ordinary: use a direct conditional
short branch when a backward local target is in range, otherwise invert the
condition and issue a near `jmp`. The macro's `LOCAL a` label is passed through
the nested condition mnemonic parameter. Both JWasm and UASM render that label
as a `??xxxx` identifier at the nested invocation and reject it. This is an
assembler macro-expansion incompatibility, not a DOS control-flow defect.

The stage driver intentionally remains on the proven default case-map. The
unproven `CASEMAP:NONE` experiment was removed from the baseline rather than
being allowed to obscure these diagnostics.

## Direct-Mnemonic LJ/DLJ Compatibility Result

A minimal JWasm probe established that a local label and a direct conditional
branch both assemble, but the original `LJ` design fails specifically when it
passes the conditional-branch mnemonic as a macro parameter. The stage now
replaces only the named `LJ*` and `DLJ*` wrapper block with direct-mnemonic
equivalents that retain the original short-versus-near selection and DEBUG
behavior. No DOS source translation unit directly invoked the generic `LJ` or
`DLJ` macros.

A fresh `ntdos-stage-v11` audit with ordinary JWasm improved coverage from
22/38 to **27/38** objects. The resolved objects are the previous long-jump
failures (`msproc`, `alloc`, `util`, `handle`, and `exepatch`). The remaining
11 failures divide into: configuration/message symbols in `msdata`; ten
case-distinct procedure/entry symbol collisions across `mscode`, `cpmio`,
`cpmio2`, `fcbio`, `disk`, `dir2`, `dev`, `msctrlc`, and `macro`; and the
separate `I_NEED` segment-label parse failure in `file`.

## UASM Comparative Probe

UASM 2.53 was fetched as public source into the isolated third-party area and
rebuilt with its supplied Visual Studio project, using only command-line
overrides for the installed SDK (`10.0.22621.0`) and toolset (`v143`). Its
legacy makefile was not used because it hard-codes a removed VS2010 path.

On the unmodified source closure it shares the `INVOKE` keyword collision. On
the existing, named `invoke`/`BUFFINFO` staging baseline, it assembled
`origin.asm` successfully (one pre-existing `AsmVar` warning). For the more
representative `mscode.asm`, UASM reduced the observed errors from the JWasm
baseline's 29 to 20, but still failed on case-distinct `CAL_LK`/`ETAB_LK`
procedure symbols, the `??0006`-style macro-local labels, and `IFDEF 0`
semantics. It is admitted as a candidate assembler for further probes, not as
the selected production toolchain.

## Complete NTDOS Candidate Link

`ntdos-stage-v19` is the first fresh isolated stage that assembles all 38
objects in the original `msdos.lnk` order and links them as one image. The
Open Watcom DOS profile needed an explicit `option nocaseexact`: the historical
source intentionally mixes spellings such as the `DosDSeg` definition and
`DosDseg` reference, which original MASM/Link resolved case-insensitively.

The staged `msinit.asm` also receives the same narrow `IF NOT INSTALLED` to
`IF Installed EQ FALSE` compatibility rewrite already used by `macro.asm`.
Without it, JWasm incorrectly retains the `netwinit` call in the installed
configuration although OpenNT provides no definition for that path.

The reproducible sequence is the original object order, Open Watcom link,
`opennt-reloc` at load segment `0000`, then `opennt-stripz`. The latter copies
bytes after the little-endian first-word strip offset, matching the behavior
of the checked OpenNT `stripz.c` source. The verified v19 result is:

| Item | Result |
| --- | --- |
| linked `ntdos.exe` | 36,818 bytes |
| relocated `ntdos.bin` | 36,786 bytes |
| strip offset | 8,784 bytes (`0x2250`) |
| resulting `NTDOS.SYS` | 28,002 bytes |
| SHA-256 | `59C8E33291CAA9F606787D39D4E27BDB9F10AD27A2F4D315B18CBC4082EF2E4D` |
| strip content check | output byte-for-byte equals `ntdos.bin[0x2250..]` |

`toolchain-probe/ntdos-stage-v19/ntdos-build-manifest.json` preserves the
object sequence and three deferred historical forward-layout assertions. WLink
reports no stack segment and chooses `0000:0000` as the EXE start address.
That is expected metadata for an image subsequently placed and entered by the
NTIO/VDM path, not evidence that the produced image is independently bootable.
No NTDOS runtime-load, NTIO handoff, DOS service, or retail-equivalence claim
is made by this result.

### Managed-Input Rebuild

On 2026-08-07, the same 38-object sequence was rebuilt in a fresh
`toolchain-probe/ntdos-managed-build-v1` stage whose inputs came only from the
hash-verified `src/opennt` source closure. The stage began without inherited
NTDOS image, map, symbol, or object outputs. JWasm assembled the complete
selected order; Open Watcom linked with `nocaseexact`; then the named
`opennt-reloc` and `opennt-stripz` compatibility tools ran in the same order.

The result is 28,002 bytes with SHA-256
`59C8E33291CAA9F606787D39D4E27BDB9F10AD27A2F4D315B18CBC4082EF2E4D`, equal to
the v19 structural candidate. This is reproducible source-to-image evidence,
not a retail identity claim. The three deferred layout assertions, NTIO
handoff, DEM loading, guest execution, and any DOS compatibility claim remain
unvalidated.

## Self-Authored Text Replacement Probe

The checked OpenNT source does contain `inc/copyrigh.inc`; it is not a missing
input. A fresh `ntdos-stage-v20-self-authored-copyright` clone replaced only
its staged three `DB` text declarations with self-authored text, then
reassembled `mstable.obj` and reused the v19 38-object link, relocation, and
strip sequence. It produced a 27,954-byte headerless candidate (SHA-256
`942AE8F4FF9097F5161938EBF6105503E8FFC52FA709627F38AD9C9B350B7FB7`).

The new text begins at offset 571, the same offset occupied by the original
first text in v19; the original text is absent from v20. The v20 image is 48
bytes smaller and still equals its relocated input after the `0x2250` strip
offset. This proves the include is data-only at this build location. It does
not prove runtime behavior, remove other historical content, or make v20 a
release artifact. Full evidence is in
`docs/SELF-AUTHORED-COPYRIGHT-EXPERIMENT.md`.

## COMMAND.COM Source-Closure Audit

The complete original `command.lnk` list contains 34 assembly objects. An
isolated COMMAND stage now copies the command source, V86 includes, MVDM common
includes, and shared EN-US message inputs, then generates its six COMMAND
message assembly inputs through the staged host compatibility builder.

The ordinary JWasm baseline assembled 3/34 objects. The fresh v18 staged audit
assembles **33/34**. Its named, staging-only rules rename the legacy
`invoke` macro; normalize `DB`/`DW`/`DD (?)` and `DB n(?)` syntax; omit one
non-emitting historical `break` debugger annotation; rename reserved COMMAND
procedure spellings `ECHO` and `GOTO`; normalize message macro parameter and
delimiter spelling; make same-module calls direct where JWasm otherwise creates
an erroneous forward external; and add explicit word widths to three unchanged
16-bit pointer moves. The OpenNT checkout remains unchanged.

The sole JWasm-incompatible object is `tprintf`: JWasm and UASM expand its
historical `MSG_SERVICES` request into calls to `$M_CHECKSTDIN` and
`$M_CHECKSTDOUT` while suppressing the requested definitions. The OpenNT-source
built MASM386 probe assembles the unmodified two-request source with zero
warnings and errors, so the original 34-object link order is retained with 33
JWasm objects plus that one MASM object. Open Watcom WLink then links the full
order and the separately named `opennt-reloc` converter produces a 50,708-byte
`COMMAND.COM` (SHA-256
`8461A5E632769305899C681549D266A2087D9230AD66F501107A46E969BEFEA6`). This is
object/link evidence only: the later stage has a 256-byte zero prefix and is
therefore rejected as a loader-valid `.COM` image by
`OPENNT-STAGED-ARTIFACT-AUDIT.md`. No DOS execution, NTIO handoff, or retail
byte-equivalence claim is made.
