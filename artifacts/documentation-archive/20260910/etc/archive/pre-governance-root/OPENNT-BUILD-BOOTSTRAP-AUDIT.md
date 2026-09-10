# OpenNT Build Bootstrap Audit

## Checked Revision

Full public OpenNT checkout:

```text
https://github.com/Paolo-Maffei/OpenNT.git
5c5b979ec08c17d3ca2eb70e8aad62d26515d01c
```

The checkout was expanded from `base/mvdm` sparse mode to the entire public
tree. This directory is an input checkout, not an output or distribution
directory for the independent engine.

## Tool Findings

| Tool/service | OpenNT evidence | Status |
| --- | --- | --- |
| NT build orchestrator | `sdktools/build/sources`, target `build` | source present; rebuilt and invoked on the modern x86 host probe |
| MASM | `sdktools/masm/sources`, target `masm386` | source present; rebuilt and used to emit a 16-bit-object smoke-test output |
| linker | `sdktools/vctools/link` | source tree present, primarily 32-bit/link tooling; exact `link16` compatibility unproven |
| resource compiler | `sdktools/vctools/rc` | source tree present; exact `rc16` compatibility unproven |
| `dosmsg` | `dos/v86/doskrnl/bios/dosmsg.cmd` | script present; calls `buildidx` and `nosrvbld` from DOS V86 tools |
| `buildidx`/DOS V86 helpers | `dos/v86/tools` source subtree | sources present, bootstrap order pending |
| `reloc` | required by NTIO/NTDOS/HIMEM makefiles | no corresponding standalone source or bootstrap binary located in this audit |
| `mapsym` | required by NTIO makefile | no corresponding standalone source or bootstrap binary located in this audit |
| `binplace` | `sdktools/imagehlp/binplace.c`; `imagehlp/sources` includes it in `UMAPPL` | source present; modern-host bootstrap and historical behavior validation pending |
| `link16` | required by real-mode target makefiles | no confirmed matching standalone tool or binary; the source-tree linker is not proof of compatibility |
| `rc16` | required by older 16-bit targets | no confirmed matching standalone tool or binary |

## Reproducible Host Bootstrap Evidence

The following probes use no checked-in Microsoft binary. They compile only the
checked revision's public source into `toolchain-probe/build`, outside the
OpenNT input checkout. They are evidence that the public sources are viable
inputs, not a claim that their outputs reproduce a released Windows NT build.

| Probe | Command entry point | Result | Qualification |
| --- | --- | --- | --- |
| `build` | `toolchain-probe/build-opennt-build.cmd` | x86 MSVC produced `opennt-build-probe.exe` (361,984 bytes) and its `-?` command displayed BUILD 4.50.0000 usage | Modern SDK headers produce warnings; it still needs a recreated historical make environment to build a source subtree |
| `masm386` compilation | `toolchain-probe/build-masm386.cmd` | x86 MSVC compiled and linked the full `sdktools/masm/sources` C list into `masm386-probe.exe` (311,808 bytes) | The historical C sources produce many type/CRT warnings, which must be reviewed before any production use |
| `masm386` object emission | `masm386-probe.exe ../../fixtures/masm386-smoke.asm,masm386-smoke.obj,NUL,NUL;` | Assembled a self-owned real-mode smoke input with 0 warnings / 0 severe errors and produced a 115-byte object | This proves only this instruction subset and object emission; it does not prove compatibility with every OpenNT VDM assembly module |
| OMF linker smoke | Open Watcom `wlink.exe` over the MASM386 smoke object | Produced a 32-byte DOS MZ image (`4D 5A` header) | `wlink` is an explicit third-party `link16` compatibility tool, not an OpenNT or Microsoft historical linker; the test has a stack warning and is not a target build |

The first failed host probe was also informative: modern x64 compilation fails
in OpenNT `winnt.h` because the header intentionally requires a target
architecture macro. A real x86 compiler environment, rather than an x64 build
with a forged `_M_IX86`, is required. The successful probes use the installed
MSVC x86 environment.

## Assembler Compatibility Result

The first genuine target probe was deliberately limited to OpenNT
`dos/v86/doskrnl/bios/mschar.asm`, staged with its `v86/inc`, `mvdm/inc` and
message inputs. It disproved two tempting shortcuts:

- The self-built `masm386` starts and handles a trivial source, but its current
  NT-host reconstruction is not safe for a genuine module. In the staged
  experiment it misparsed makefile-style command arguments and overwrote staged
  source files. No result from that experiment is an NTIO object artifact.
- The installed modern `ml.exe` with `/omf` begins parsing the genuine module,
  then rejects legacy MASM macro/structure semantics (`selectedstatus`, macro
  argument count and structure-field diagnostics), followed by expected
  undefined symbols. It is not a drop-in assembler replacement.

Therefore the historical OpenNT assembler is not accepted as the target
assembler. The staged directory is disposable evidence only. A separately
sourced MASM-5-compatible assembler must have its source, license, hash and
genuine-module compatibility recorded before it is used for a target build.

### JWasm Admission Probe

JWasm source was obtained in the isolated third-party tool area at revision
`7f6f32e78b79565d40bcce496756aadd1ff66900` and rebuilt with its supplied
`Msvc.mak` recipe. It is a third-party compatibility tool under the Sybase
Open Watcom Public License, not an OpenNT component and not a replacement
claimed to be historical MASM.

Its first genuine-module probe used staged OpenNT `mschar.asm` and produced
`mschar-jwasm.obj` (1,843 bytes), with two passes, zero errors and three
legacy `break` macro warnings. The command included `-omf -Zm`, where `-Zm`
enables the MASM 5.1 compatibility behavior needed for the module's forward
symbols.

The probe applied exactly two syntax-only transformations to the staging copy:

| Input spelling | Staged spelling | Reason |
| --- | --- | --- |
| `.selectedstatus` | `selectedstatus` | JWasm does not accept the legacy dot-prefixed equate spelling |
| `es:[bx].media` | `es:[bx+media]` | `media` is an equated offset; this preserves the effective address without relying on the old field spelling |

The copied module's SHA-256 was checked before and after the initial JWasm
probe, proving JWasm itself did not modify the input. The original OpenNT
checkout remains untouched. These transformations now require a named,
deterministic staging rule and per-file audit before broader use; they are not
permission to make unreviewed source edits.

## Missing Build Environment

The full public checkout has component `sources`, makefiles and target
libraries, but no checked `makefile.def` or `projects.inc` build-environment
files. `build.exe -?` runs once `NTROOT`, `BEROOT` and `NTMAKEENV` are set, but
an actual source-tree build cannot yet start because those environment inputs
are absent. Reconstructing a minimal environment must be a named compatibility
port, with each macro and target rule traceable to a component requirement;
it must not be replaced by an undocumented global makefile.

## Consequence

The full OpenNT source checkout is a necessary and now available input for
the requested suite. It is not alone sufficient to reproduce every historical
real-mode artifact. The first reproducible build milestone must be a **tool
bootstrap manifest**, with one row per tool showing:

- source or supplied-binary provenance;
- SHA-256;
- host/guest execution environment;
- supported target architecture;
- exact command line; and
- validation target.

No absent historical tool may be silently replaced with a modern tool under
the same name. Such a substitution must be recorded as a compatibility port,
with byte-level and runtime validation separate from historical reproduction.

## Runtime-Criticality

`mapsym` consumes a linker `.MAP` and emits a debugger `.SYM`; it is useful
build evidence but is not an input to the guest runtime payload. The OpenNT
NTIO/NTDOS makefiles invoke it after linking and before final image conversion.
It may therefore be deferred for a first executable artifact manifest, with
the missing `.SYM` explicitly recorded.

`reloc` is different: the real-mode makefiles use it to turn a linked `.EXE`
into `NTIO.SYS`, `HIMEM.SYS`, `COUNTRY.SYS`, or the NTDOS image. It is a
runtime-critical bootstrap gap. Any replacement must be a separately named,
tested compatibility tool, never a silent command shim. Its required behavior
must be derived from each OpenNT invocation and verified against MZ headers,
relocation records, load segment arguments, output bytes, and actual guest
startup.

`src/opennt_reloc.c` is the first narrowly scoped compatibility core. Its
`opennt-reloc` CTest proves, using a self-owned synthetic MZ image, header
removal plus relocation-word adjustment for an explicit `70h` load segment.
It makes no byte-equivalence claim. It has now also processed a linked,
OpenNT-source-built NTIO image; the actual historical argument shapes omitted
segment and `0` still need separate target coverage.

## Reproducible NTIO System-Image Evidence

### Managed-Input Closure

The initially successful NTIO experiment was staged from an external checkout,
which was sufficient for a one-off compatibility build but not sufficient for
the adopted-source rule. On 2026-08-07 its complete unmodified source closure
was imported, preserving original paths, under
`src/opennt/base/mvdm/dos/v86/{doskrnl/bios,inc,messages}`. The 123 imported
files are all exact copies of the fixed OpenNT revision and are individually
recorded in `src/opennt/SOURCE-MANIFEST.json`.

`stage-opennt-ntio.ps1` now takes `-ManagedOpenNtRoot`, requires that root's
manifest, and verifies every source closure file against its manifest entry
before it creates a disposable stage. A 2026-08-07 check validated all 2,243
current manifest entries with zero size or hash mismatches, successfully
created a managed NTIO stage (221 staged files), and rejected the unmanifested
external checkout before any stage was created. The source changes required by
JWasm remain named transformations inside the disposable stage; no transformed
file is part of the managed source closure.

The NTDOS experiment had the same original staging weakness. Its complete
58-file `dos/v86/doskrnl/dos` directory is now a managed exact-copy input,
sharing the already managed V86 include and message directories. Its staging
script uses the same root/entry/hash gate. This establishes reproducible input
provenance for the existing NTDOS structural candidate; it does not validate
the deferred NTDOS layout assertions, guest loading, or execution.

A fresh managed-input build has now completed all 38 NTDOS assemblies, link,
zero-segment relocation, and strip operation. Its 28,002-byte output hash is
`59C8E33291CAA9F606787D39D4E27BDB9F10AD27A2F4D315B18CBC4082EF2E4D`, matching
the earlier structural stage. The complete result and remaining runtime limits
are recorded in `NTDOS-ASSEMBLY-COMPATIBILITY-LEDGER.md`.

The same managed-input rule now covers the 51-file COMMAND source directory.
The command staging script verifies that closure and its shared include/message
inputs before running the separately proven BUILDMSG compatibility tool. The
generated class/control files remain stage-only data; the present structural
COMMAND image is still not a runtime validation result.

The upstream command directory also contains a prebuilt `COMMAND.COM` and its
`.exc`/`.map` intermediates. They are deliberately excluded from the managed
source island and its manifest. A fresh stage must therefore obtain no DOS
executable merely by copying its managed source closure.

The same audit removed checked-in NTIO/NTDOS `.SYS`, `.BIN`, `.MAP`, and
`.SYM` build outputs and a HIMEM `.map` from the source island. These are not
source inputs, even when present in the historical checkout. The current
managed root has no binary-like historical build output.

HIMEM's four upstream assembler `.err` logs are likewise excluded. Its stage
now starts from the eight managed text-source/build-descriptor files and
validates them against the source manifest before applying its named UASM
compatibility transformations.

The first complete guest-system artifact pipeline is now recorded in the
independent research workspace:

1. `toolchain-probe/stage-opennt-ntio.ps1` copies only the NTIO source closure
   to a disposable stage and applies eight named JWasm compatibility rewrites.
   They preserve runtime instructions/data or replace assembler-only layout
   assertions with an explicit later-validation obligation; the OpenNT input
   checkout is never modified.
2. `toolchain-probe/assemble-ntio-bios.cmd` assembles all twelve objects named
   by the original `msbio_4.lnk` order using source-built JWasm in OMF/MASM-5
   compatibility mode.
3. `toolchain-probe/link-opennt-ntio.ps1` uses the Open Watcom DOS linker
   compatibility path, preserves the original object order, then calls the
   separately named `opennt-reloc` tool at load segment `0070`.

An entirely fresh `ntio-stage-v6` completed all twelve assemblies, linked an
MZ intermediate, and produced a 33,760-byte headerless `NTIO.SYS` whose first
bytes are `E9 6D 03 00 00 EA FD 02 70 00`. The generated
`ntio-build-manifest.json` records source order, paths and load segment.

This proves a modern compatibility-build path for this source revision. It
does **not** establish equivalence to a retail file, successful boot, the
historical entry/stack contract, or the deferred `off_d0` layout assertion.
The linker reports no explicit stack and defaults its start address to
`0000:0000`; that matches the absence of explicit stack/entry directives in
the original link description but needs guest-startup validation.

On 2026-08-07, a fresh `toolchain-probe/ntio-managed-build-v1` stage used only
the manifest-verified managed NTIO closure. It assembled the original twelve
objects, linked them in the recorded order, and used `opennt-reloc` at segment
`0070`. Its output is 33,760 bytes with SHA-256
`9BCD709E962DBD897DA062C5287B02403B1470C78763B245197866870E03B43F`,
byte-identical to the earlier structural candidate and beginning `E9 6D 03`.
This is reproducible source-to-image evidence only: it neither proves the
deferred `off_d0` layout assertion nor admits NTIO guest execution.

The fresh link driver now emits an explicit WLink map. Its managed-input map
records `off_d0` at `0049:00D0`, proving the original assembly requirement
`offset off_d0 == 0x00D0` that the JWasm stage had deferred. The image hash is
unchanged after map generation. This closes that one post-link layout check;
it does not establish the higher-memory copy, IVT/BDA state, reset ordering,
or any guest execution behavior.

## Reproducible HIMEM System-Image Evidence

### Direct Historical-Source Toolchain Check

The staged HIMEM compatibility pipeline above remains a named compatibility
port. It must not be confused with direct assembly of the tracked original
source input. On 2026-08-07 both UASM 2.53 and the installed VS 2022 `ml.exe`
were invoked directly over the unchanged six-module
`dos/v86/dev/himem` source set. UASM accepts only `himem1.asm` and
`messages.asm`; its other failures are old segment/symbol-type propagation.
Modern `ml.exe` rejects `.286` in the shared include and then reports the
resulting legacy structure-field and symbol failures for every module.

The original makefile identifies the full historical contract: `masm`,
`link16`, `reloc`, and `stripdd`. The OpenNT source tree contains `stripdd.c`,
but the reviewed snapshots do not provide a matching `reloc` implementation
or an admitted 16-bit MASM/linker toolchain. These direct-tool results are
kept under `toolchain-probe/build/himem*`; no original source file was edited.
Consequently a future historical-reproduction route needs a separately
proven MASM-5-compatible assembler plus linker/postprocessor inputs. It must
not retrofit modern assembler syntax into `src/opennt` or promote the staged
compatibility image to historical equivalence.

`dos/v86/dev/himem` is the second guest-side source target with a complete
compatibility-build pipeline. A fresh stage copies the HIMEM source closure
and MVDM includes, then makes only these named assembler-compatibility changes:

- old `[register].field` syntax becomes `[register+field]`;
- byte-sized `cLock` increment/decrement operands become explicit; and
- two `EXTRN` names are case-normalized to their own source-defined `MoveIt`
  and `Interrupt` symbols, which historical MASM case mapping joined.

UASM assembled all six objects (`himem`, `himem1`, `himem2`, `messages`,
`himem4`, `himem5`). Open Watcom's DOS OMF linker produced `HIMEM.EXE`, and
`opennt-reloc` produced the zero-load-segment `HIMEM.BIN` (45,744 bytes).

OpenNT includes the source for the required final tool at
`dos/v86/tools/src/stripdd/stripdd.c`. The separately named
`opennt-stripdd` compatibility tool implements that documented tail format:
it removes the paragraph-rounded ORG zero hole described by the 16-byte `ZZZ`
tail embedded in `himem.asm`. It produced a 4,784-byte candidate `HIMEM.SYS`.
This is source-build evidence, not byte-equivalence or runtime-load evidence.

The earlier candidate is now reproduced by `himem-stage-v5-managed`, which
starts only after `stage-opennt-himem.ps1` verifies every copied HIMEM and
shared `base/mvdm/inc` input against `src/opennt/SOURCE-MANIFEST.json`.
UASM emits the six recorded objects, WLink links their original makefile order,
and the named relocation/`stripdd` helpers produce a 4,784-byte `HIMEM.SYS`
with SHA-256 `2CDB2687600E76C79ECDC48F1C9666C694810C6EFDDAD7AA422E826F49937B07`.
It is byte-identical to the earlier structural candidate. The UASM implicit
BYTE and WLink no-stack/no-entry warnings remain recorded in the stage output;
neither they nor byte identity establish XMS installation or runtime behavior.

## Reproducible SHARE Utility Evidence

`dos/v86/cmd/share` is a one-source, one-object DOS utility target with no
message-generator or library dependency. Its original source explicitly says
that on NT it is a stub which does nothing, then executes `INT 21h/4Ch` with
success status. The managed `src/opennt` island carries its three original
text inputs, and `share-managed-input-audit-v1` verifies all three against
`SOURCE-MANIFEST.json` before building. UASM assembled `share.asm` with zero
warnings; the Open Watcom DOS linker produced a 295-byte MZ image with zero
relocations and entry `0000:0100`, SHA-256
`AA317B288A07962DD9EA75DEBB21A20452E5B38791EEEB9619BD2B1D6F994BBD`.
WLink retains the historical no-stack-segment `W1014` warning. This is a
managed source-to-EXE structural build record, pending a runtime trace and
without a claim of retail byte identity.

## Reproducible LOADFIX Utility Evidence

`dos/v86/cmd/loadfix` closes using its single assembly source and the checked
OpenNT `dos/v86/messages/loadmsg.msg` input. UASM assembled the source and the
DOS OMF linker emitted `loadfix.exc`. Its MZ header reports zero relocation
records, a 32-byte header and entry IP `0100h`. The historical V86 `make.inc`
defines the `.exc.com` suffix rule as `reloc $* $*.com`, not as `stripz` or a
plain header removal. The checked source tree contains no corresponding
historical `reloc` source input: its V86 tools source directory contains
`buildidx`, `getsize`, `h2inc`, `stripdd`, and `stripz` only.
An exact local source-name cross-check across the available OpenNT, NT4.5,
NT5/2000, and XP trees found only `sdktools/mip2coff/reloc.c`, a MIPS COFF
tool, not a DOS `.exc.com` converter. It does not close this input gap.

The current self-authored `opennt-reloc` helper removes the MZ header and
applies relocation words, but intentionally has no `.COM` entry-layout rule.
Applied to this image it leaves the paragraph generated by `ORG 100h`: the
1,387-byte output begins with exactly 256 zero bytes, so a COM loader would
execute padding rather than `start`. It is therefore rejected as a loadable
candidate. The original assembly/link evidence remains useful, but producing
a valid `LOADFIX.COM` requires either recovery of the historical `reloc`
contract/source or a separately admitted, source-derived COM-conversion
contract with its own fixtures. `stripz` is inadmissible here because its input
format requires a first-word strip length; LOADFIX's first word is zero.

## Reproducible FASTOPEN Utility Evidence

`dos/v86/cmd/fastopen` has a single assembly object and depends only on the
checked OpenNT `fastopen.inc` source closure. The managed `src/opennt` island
now carries the original text-only `fastopen.asm`, `fastopen.lnk`, and
`makefile`, while the source manifest also verifies every copied V86 include
input. A fresh `fastopen-managed-input-audit-v1` stage verified 79 such inputs
before assembling `fastopen.asm` with UASM OMF/MASM-5 compatibility and linking
it through the Open Watcom DOS linker. The resulting candidate `FASTOPEN.EXE`
is a 295-byte MZ image with zero relocation entries, a 32-byte header, and
entry `0000:0100`, SHA-256
`AA317B288A07962DD9EA75DEBB21A20452E5B38791EEEB9619BD2B1D6F994BBD`.
The original link description provides no stack segment, so WLink reports
`W1014`; that historical metadata warning is retained. The generated manifest
records the one-object link order. Its present stage passes the artifact audit's
MZ length, zero-relocation, header-size, and entry-point checks, so it is a
structural stage image. It is not a claim of retail equivalence or runnable
cache-service compatibility.

## Reproducible SETVER Mixed-Language Evidence

`dos/v86/cmd/setver` is a full-set boundary because its original build combines
`setver.c` and `parse.c` with `dos.asm` and `device.asm`. A fresh isolated
`setver-stage-v8` copied the command source, its checked EN-US
`usa-ms/message.h`, and the V86 include closure. OpenWatcom C16 compiled the
two C sources in small-model DOS mode with zero warnings/errors; JWasm
assembled the two original assembly modules with one layout-equivalent `DD (?)`
to `DD ?` staging normalization.

The historical sources assume Microsoft C ABI decoration for four assembly
procedures. The stage preserves their original labels and adds their missing
public declarations; a stage-local forced header uses four OpenWatcom
`#pragma aux` declarations to request the matching Microsoft-C external names,
caller-stack parameter convention, and return registers. This is an explicit
toolchain bridge, not a source rewrite or runtime stub.

Open Watcom WLink linked the original order `device`, `setver`, `parse`,
`dos` into `SETVER.EXE` (11,586 bytes, SHA-256
`B72C071A0A494BE9E8D6F6958799B83C8956169DA6D76F94A73944B2CCF3ECC9`).
`setver-stage-v8/setver-build-manifest.json` records the evidence. It remains
a compatibility-build artifact pending DOS runtime validation; it is not
claimed retail-identical.

## Reproducible APPEND Message-Driven Utility Evidence

`dos/v86/cmd/append` is the first non-COMMAND validation of the staged host
`BUILDMSG` compatibility tool. The managed `src/opennt` island now carries its
six original command inputs, including the text-only MASM macro source named
`sysmac.lib`. A fresh `append-managed-input-audit-v1` stage verified those
inputs plus every copied V86 include, MVDM include, and message input against
the source manifest: 196 source inputs in total. The self-built host tool then
processed the original `append.skl` and checked EN-US `usa-ms.msg`, generating
`APPEND.CTL` plus `APPEND.CLA`, `APPEND.CLB`, `APPEND.CL1`, and `APPEND.CL2`.
This proves the retained OpenNT message parsing/output logic is usable beyond
the COMMAND skeleton; it does not claim historical `nosrvbld` equivalence.

JWasm then assembled the original `append.asm` with all four generated message
classes and zero errors (one retained legacy `break` macro argument warning in
`pdb.inc`). The Open Watcom DOS linker produced `APPEND.EXE`, 12,034 bytes,
two relocations, entry `0000:1B4B`, SHA-256
`9797ED5E592F4F3E8A93B60902DEE7A33C33DD1D71EC59012D8059E0C07F65BF`.
The stage manifest retains the original single-object link order. The image
passes the MZ declared-length structural check. This closes the
message-generation prerequisite for this utility family, while runtime
execution and retail-byte equivalence remain unproven.

## Reproducible QUIT BOP Utility Evidence

`v86/util/quit.asm` builds as a single-object V86 utility when UASM is given
the original makefile's 16-bit large-model equivalent (`-ml`), in addition to
OMF/MASM-5 compatibility options. The DOS linker emitted an MZ intermediate
without a 32-bit-object warning; zero-segment header removal produced the
three-byte `QUIT.COM` image `C4 C4 FE`. Its only operation is
`BOP_UNSIMULATE`; it is valid only when the host VDM monitor recognizes that
BOP and must not be presented as a standalone DOS utility.

## Keyboard Device Compatibility-Assembly Boundary

The research workspace now has an isolated source-closure stage for
`dos/v86/dev/keyboard`, its 32-object original link order, an UASM assembly
driver, and an Open Watcom link/header-removal driver. The intended artifact is
the headerless DOS character device `KEYBOARD.SYS`, whose historical checked-in
reference begins `FF "KEYB    "`.

A fresh staged build proves two terminal objects, `KDFEOF` and `KDFNOW`, assemble
without changes. The remaining keyboard layouts share a stricter blocker in
the historical `KEYBMAC.INC` macro dialect: it defines a macro named `OPTION`
and uses macro nesting/conditional forms that UASM rejects after the naming
collision is removed. A direct JWasm check of the unmodified staged source
fails at the same macro definition and cascades through the same nested
conditionals. The stage records the attempted limited rename of that macro and
its `EXIT_IF_FOUND` call sites, but no `KEYBOARD.SYS` is emitted and the result
is not an artifact claim.

This is a toolchain-compatibility gap, not evidence that keyboard support is
optional. Resolving it requires either a verified MASM-compatible assembler
that accepts this dialect, or a separately tested source-to-source macro
compatibility pass with byte/layout comparison against a legally supplied
reference. The OpenNT checkout remains untouched.

## Message Bootstrap Boundary

The OpenNT `dos/v86/tools/src/buildidx/buildidx.c` source was built in an
isolated host stage and successfully verified the checked EN-US `usa-ms.msg`
against its supplied `usa-ms.idx` (`Message file and index file are not
changed.`). This establishes the index half of the message pipeline.

The historical `dosmsg.cmd` then invokes `nosrvbld <skeleton> <message-file>`
to produce `.CTL`/message assembly outputs. No `nosrvbld` source or executable
is present in the complete OpenNT checkout. The stage-local `BUILDMSG` host
compatibility port is now proven against two distinct skeletons, `COMMAND` and
`APPEND`, so it is an admitted substitute for those validated utility stages.
It is not yet a blanket replacement for every skeleton; ANSI, EXE2BIN and
other message-driven targets still require their own fresh-stage evidence. The
historical `buildidx.c` also assumes an existing IDX file before its comparison
path; cold-start index rebuilding needs a small, separately tested
compatibility port rather than reliance on that unchecked legacy path.

For corroboration only, a separate clone of Microsoft's public MS-DOS 4.0
reference tree was inspected. It uses the same `nosrvbld` invocation contract
but contains `NOSRVBLD.EXE` rather than source. That binary is not used as an
input, executed, copied, or distributed by this research project. The result
raises confidence that the tool is a historical external bootstrap dependency;
it does not close the source-build gap.

## COUNTRY.SYS Managed Source Closure

The earlier isolated `COUNTRY.SYS` stage was incomplete: it copied only
`dos/v86/dev/country`, while `country.asm` also unconditionally includes the
shared `dos/v86/inc/copyrigh.inc`. That source file does exist in the OpenNT
checkout; it is not a missing-input boundary.

The managed `src/opennt` island now carries the original text-only country
source closure and records every imported file as a hash-verified
`upstream-copy`. `stage-opennt-country.ps1` verifies that each local country
input and the shared include is uniquely manifest-listed and hash-identical
before copying them to its disposable stage. It does not accept generated
`COUNTRY.SYS`, map files, or an untracked replacement include. This restores
source provenance; it does not establish retail identity or runtime loading.

## COMMAND Message Builder Compatibility Result

OpenNT supplies `cmd/command/buildmsg.c`, the checked EN-US generated
`usa-ms/bldmsg.h`, `command.skl`, and the shared `usa-ms.idx`/`usa-ms.msg`
inputs. The source's parsing and output logic was retained in an isolated
staged file. Only its DOS PSP/memory diagnostic, file-handle wrappers, and the
two absolute seeks were mapped to checked host CRT operations; the upstream
checkout is not edited. `toolchain-probe/buildmsg_host_compat.h` and
`stage-opennt-buildmsg.ps1` make that boundary explicit.

The v8 staged host builder compiled with MSVC and successfully processed the
EN-US inputs. Its host-port staging additionally normalizes all three parsed
`CurrentMessageNumber` values to 16 bits. This preserves the original 16-bit C
behavior for skeleton entries such as `:use -1`, which must render `FFFFH`, not
the host's 32-bit `FFFFFFFFH`, in generated assembler symbols. It emitted
`COMMAND.CTL` (19 bytes) and the five required message include files:
`COMMAND.CL1` (2,000 bytes), `COMMAND.CL2` (1,281 bytes), `COMMAND.CL3`
(1,200 bytes), `COMMAND.CL4` (1,197 bytes), and `COMMAND.CLF` (49,194 bytes).

A fresh v18 stage assembled 33 of 34 original objects with JWasm. The remaining
`TPRINTF` nested-`MSG_SERVICES` conditional expansion is assembled from its
unmodified source by the OpenNT-source-built MASM386 probe (zero warnings and
errors). WLink links the exact 34-object original order, and `opennt-reloc`
produces `COMMAND.COM`, 50,708 bytes (SHA-256
`8461A5E632769305899C681549D266A2087D9230AD66F501107A46E969BEFEA6`). This is
a compatibility build record, not a reproduction of historical `BUILDMSG.EXE`,
a DOS runtime execution result, or a retail-byte-equivalence claim.

That v18/v20 line is now superseded as a COMMAND source-to-image result. The
canonical pristine stage starts from manifest-verified `src/opennt`, assembles
all 34 original objects with a disposable MASM386 rebuilt from the managed
`sdktools/masm` closure, and rejects WLink because it drops `rdata.obj` LIDATA
relocations. `OPENNT-BUILDMSG-HOST-ABI-RECOVERY.md` still records the host-only
generator ABI correction needed to create valid fresh EN-US message inputs.
See `OPENNT-G0-TOOLCHAIN-REBASE.md`.

## BUILDMSG Host-Port Memory Validation

The staged v11 host builder was also compiled with LLVM AddressSanitizer and
run against a fresh EDLIN EN-US skeleton stage. That run completed both passes
and generated `EDLIN.CLA`, `EDLIN.CLB`, `EDLIN.CLC`, `EDLIN.CL1`, and
`EDLIN.CL2` without an AddressSanitizer finding. This test found and required
three host-only corrections: bound the directive-token scan from `%s` to
`%31s` for `RecordTypeText[32]`; do not uppercase the string-literal class
arguments synthesized by `EndRecord` in place; and enlarge message-derived
`sprintf` scratch storage from 128 bytes to the documented parser-line bound.
The upstream OpenNT checkout remains unchanged. This validates memory safety
for the exercised builder path, not retail BUILDMSG byte identity.

## Next Concrete Checks

1. Create a fresh-stage post-link verifier for NTIO segment placement,
   `off_d0`, and any other deferred assembler-only layout contract.
2. Validate the linked NTDOS candidate's deferred layout assertions, then
   establish an NTIO-to-NTDOS runtime-handoff probe before describing it as a
   loadable runtime.
3. Drive every declared output in the complete `base/mvdm` artifact inventory
   through a source-closure/toolchain/link/runtime state machine, including
   device, keyboard/mouse, command, editor, redirector, locale and monitor
   families. Named examples must never become an implicit scope cap.
4. Inventory all uses of `reloc`, `mapsym`, `link16`, `rc16`, and `binplace`,
   including expected argument forms.
5. Keep the isolated build workspace; every generated tool and artifact must
   receive a manifest row before it can become an input to another stage.
