# Historical Toolchain Rebase Record

Status: active evidence record, 2026-08-07.

## Source Priority

OpenNT and OpenNT 4.5 are the default historical toolchain inputs. MS-DOS 4.0
is a narrow fallback only where the fixed OpenNT source/tool snapshot lacks a
necessary executable tool or matching 16-bit C runtime closure. A local
substitute is not a formal guest build tool.

## Verified Tool Path

| Role | Actual input | Result |
| --- | --- | --- |
| assembler | OpenNT 4.5 `tools\\opennt-4.5\\MASM.EXE` (MASM 4.00) | Native historical Win32 host tool which produces the required unchanged 16-bit OMF objects. |
| linker | OpenNT 4.5 `tools\\opennt-4.5\\LINK16.EXE` (Microsoft Segmented Executable Linker 5.60.220) | Native historical executable; selected because the fixed OpenNT makefiles name `LINK16`. |
| message generator | OpenNT 4.5 `tools16\\BUILDMSG.EXE` | Executes under DOSBox and generated the GRAPHICS EN-US classes. |
| image relocation | OpenNT 4.5 `tools16\\RELOC.EXE` | Executes under DOSBox; GRAPHICS conversion matches the source-tree reference hash. |
| device-image trim | OpenNT 4.5 `tools16\\STRIPDD.EXE` | Executes under DOSBox and completed the HIMEM packaging path. |

`LINK_60.EXE` is the same recovered LINK16 tool family. Mixed C/assembly guest
targets use OpenNT MASM/LINK16 for assembly/linking. MS-DOS 4.0 `CL.EXE` with
the same package's `BLD\\INC` and model-matched CRT libraries remains the
documented narrow fallback because this OpenNT snapshot does not provide that
standalone C runtime closure. No
non-Microsoft assembler, linker, compiler, or locally rebuilt compatibility
utility is permitted in a formal guest result. The v1 entries below remain
diagnostic history only; the fresh LINK16 records beginning at “LINK16
Correction” are the current source-to-image evidence.

## Rebuilt Evidence

All paths below use the checked-in historical inputs under `tools/`.

### GRAPHICS.COM

The stage at
`artifacts/toolchain-runs/graphics-historical-tools-v1` used original OpenNT
4.5 `BUILDMSG.EXE`, MS-DOS 4.0 MASM 5.10, MS-DOS 4.0 Link 3.65, and OpenNT 4.5
`RELOC.EXE`. Its result is 19,694 bytes with SHA-256:

```text
989B38806EC1E1BDD30DA3DC6B41CC85BDC81DDA74F870FCC7DF399876EB2756
```

That value equals the OpenNT source-tree reference `GRAPHICS.COM` hash.

### HIMEM.SYS

The stage at `artifacts/toolchain-runs/himem-historical-tools-v1` assembled
all six unchanged sources (`himem`, `himem1`, `himem2`, `messages`, `himem4`,
and `himem5`) with MS-DOS 4.0 MASM 5.10. It linked them with MS-DOS 4.0 Link
3.65, then ran OpenNT 4.5 `RELOC.EXE` and `STRIPDD.EXE`. The result is 4,768
bytes with SHA-256:

```text
08AA2C47D835460ED3067FA7D6F8A3B37EDECA524AD102B0588FDD1BF389CE08
```

### LOADFIX.COM

The stage at `artifacts/toolchain-runs/loadfix-historical-tools-v2` assembled
the unchanged source with `tools/msdos400/MASM.EXE`, linked it with
`tools/msdos400/LINK.EXE`, then followed the original `.exc.com` rule: copy
the linker image to `.EXE` and invoke `tools/opennt-4.5/RELOC.EXE` by basename.
The resulting `LOADFIX.COM` is 1,131 bytes with SHA-256:

```text
8E62654A49BD88C784C53F25E9FBA13C641624D6A02214385115F66CCF1ECFDB
```

### EXE2BIN.EXE

The stage at `artifacts/toolchain-runs/exe2bin-historical-tools-v1` generated
the original EN-US control file with `tools/opennt-4.5/BUILDMSG.EXE`, assembled
two unchanged sources with `tools/msdos400/MASM.EXE`, and linked with
`tools/msdos400/LINK.EXE`. The result is 8,411 bytes with SHA-256:

```text
9575A03BE94D91DD60498A1F65D6D9EB0BE8E9B9199FCEB71C26034039579D08
```

### NLSFUNC.EXE

The stage at `artifacts/toolchain-runs/nlsfunc-historical-tools-v1` uses the
same `BUILDMSG`, MASM, and LINK inputs for its unchanged two-module source
closure. The result is 7,039 bytes with SHA-256:

```text
6F4B0C20ECED883168773A95683FA22EFDA38927996F51E171049B351C5F7B17
```

### APPEND.EXE

The stage at `artifacts/toolchain-runs/append-historical-tools-v1` generated
the original EN-US control file with `tools/opennt-4.5/BUILDMSG.EXE`, assembled
the unchanged `append.asm` source with `tools/msdos400/MASM.EXE`, and linked
with `tools/msdos400/LINK.EXE`. The result is 10,761 bytes with SHA-256:

```text
918C04AB25C99BF85B73925B0372CC6FC3F3FE564CAB6BA30ABE0FBAF72BC975
```

### EDLIN.EXE

The stage at `artifacts/toolchain-runs/edlin-historical-tools-v1` generated
the original EN-US control file with `tools/opennt-4.5/BUILDMSG.EXE`, assembled
all five makefile modules with `tools/msdos400/MASM.EXE`, and used the original
`EDLIN.LNK` response file through `tools/msdos400/LINK.EXE`. The result is
12,629 bytes with SHA-256:

```text
80043F076D00F4ADE8946CF76B2CBD01B6EA76275415097977DBCF70E4ED9D04
```

### DEBUG.EXE

The stage at `artifacts/toolchain-runs/debug-historical-tools-v1` generated
the original EN-US control file with `tools/opennt-4.5/BUILDMSG.EXE`, assembled
all eleven makefile modules with `tools/msdos400/MASM.EXE`, and passed the
original `DEBUG.LNK` response file to `tools/msdos400/LINK.EXE`. The result is
20,621 bytes with SHA-256:

```text
A648BB5DF20E18B00D6140C3C7945718532C75142C6CB7A77DE1F8D3265C9511
```

### KB16.COM

The stage at `artifacts/toolchain-runs/kb16-historical-tools-v1` generated
`KEYB.CTL`, assembled all nine makefile modules, linked the original
`KEYB.LNK`, then followed the shared `.exc.com` rule from `dos/v86/make.inc`
using `tools/opennt-4.5/RELOC.EXE`. It produced a 14,710 byte `KB16.COM` with
SHA-256:

```text
9C432EA71E0E11BFB2EDF9166C862826115EC4CDF8439F986A5ED71D9A3AF3B9
```

The source tree's intermediate `KB16.EXC` is the same size but has a different hash
(`8DAB9416340D09AA218FDBF904D52AE841A4FC47E90C9DF3D18BBF1D2B532420`).
This record therefore does not claim byte equivalence to that intermediate
reference, but the final `.COM` packaging itself is directly specified.

### COUNTRY.SYS

The stage at `artifacts/toolchain-runs/country-historical-tools-v1` assembled
the unchanged country source with `tools/msdos400/MASM.EXE`, linked with
`tools/msdos400/LINK.EXE`, and applied `tools/opennt-4.5/RELOC.EXE` as specified
by the original makefile. The result is 27,097 bytes with SHA-256:

```text
9DDF2B738307B5950699513E6B8221E2044BB686487A8C216AF7C6FD3F853B54
```

### ANSI.SYS

The stage at `artifacts/toolchain-runs/ansi-historical-tools-v1` generated
the original EN-US control file, assembled all four makefile modules, linked
`ANSI.EXS`, then followed the shared `.exs.sys` rule from `dos/v86/make.inc`
with `tools/opennt-4.5/RELOC.EXE`. The result is 9,029 bytes with SHA-256:

```text
C929C0893BBA4C6454632D3408EE4F7661B51CF5C2CE20035DCD4283CD623C85
```

### KEYBOARD.SYS

The stage at `artifacts/toolchain-runs/keyboard-historical-tools-v1` assembled
all 32 modules named by the original makefile, linked the untouched
`KEYBOARD.LNK` response file, and applied the shared `.exs.sys` rule using the
checked-in historical `RELOC.EXE`. The result is 40,665 bytes with SHA-256:

```text
96FCAC606683CF262DBC030713A9263731EE83F19A10207E5DD632E5EE06C2AD
```

### FASTOPEN.EXE and SHARE.EXE

Both one-module utilities were rebuilt in fresh stages using their original
response files and the original `$(exelink)=/E` setting with
`tools/msdos400/MASM.EXE` and `tools/msdos400/LINK.EXE`. Each result is 869
bytes with SHA-256:

```text
BDC67988D79C85CF6F44D4117226EED8DF0ACB4B6556994559A5047BFA8110DA
```

### HIMEM.SYS and REDIR.EXE rebase

The formal reruns now reside at
`artifacts/toolchain-runs/himem-historical-tools-v2` and
`artifacts/toolchain-runs/redir-historical-tools-v4`; both scripts resolve all
assembler, linker, and post-processing tools only beneath `tools/`. Their
outputs are respectively 4,768 bytes (`08AA2C47D835460ED3067FA7D6F8A3B37EDECA524AD102B0588FDD1BF389CE08`)
and 3,338 bytes (`E03D7F71CE39839556488C895205CCD1802E79F7424809BD776D9D22D7695870`).

## Boundary

These are source-to-image reconstruction results only. They do not claim
retail identity, DOS runtime execution, or a working NTVDM host chain. The
staged source is copied before tools run; guest source transforms are empty.

## LINK16 Correction And Kernel Evidence

The older v1 utility entries are assembly/link diagnostics, not final
message-generation proof: their DOS-tool runner did not yet delete a
pre-existing expected output before invocation. They must be rerun in fresh
stages before promotion. This qualification does not apply to the v5 records.

OpenNT makefiles name `LINK16`, not the MS-DOS 4.0 `LINK.EXE`. The checked-in
`tools/opennt-4.5/LINK16.EXE` is Microsoft Segmented Executable Linker 5.60.220
and is now the selected linker for the kernel profiles.

`artifacts/toolchain-runs/ntio-historical-tools-v5` is a fresh unchanged-source
stage: `NOSRVBLD` generated `MSBIO.CL3` through `MSBIO.CL6`, MS-DOS 4.0 MASM
assembled all 12 selected BIOS modules, OpenNT LINK16 linked `MSBIO_4.LNK`, and
the original `RELOC NTIO.EXE NTIO.SYS 70` step produced a 33,792-byte NTIO.SYS
with SHA-256 `CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937`.

`artifacts/toolchain-runs/ntdos-historical-tools-v5` freshly generated all four
`MSDOS.CL*` files and assembled all 38 selected kernel modules. `LINK16 /MAP
@MSDOS.LNK` then exits with code 2 before `NTDOS.EXE` exists:

```text
msdata.obj(msdata.asm) : fatal error L1101: invalid object module
Object file offset: 2458 Record type: a0
```

The stage retains its MAP, log, tool hashes, and empty guest-transform record.
This is a linker compatibility blocker, not grounds for a source overlay.

### Small Link16 consumers

Fresh clean stages `fastopen-historical-tools-v2` and
`share-historical-tools-v2` also validate the selected linker outside the
kernel. Each assembled its unchanged one-module OpenNT source with MS-DOS 4.0
MASM and linked its original response file with OpenNT LINK16 `/E`. Both
produced an 882-byte image with SHA-256
`69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC`.

### Fresh message-bearing utilities

The output-clearing DOS-tool runner was used for fresh `BUILDMSG` stages before
rebuilding `APPEND.EXE` and `EXE2BIN.EXE`. Both use MS-DOS 4.0 MASM and OpenNT
LINK16, with no guest-source transforms. Their resulting hashes are:

| Artifact | Stage | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| APPEND.EXE | `append-historical-tools-v2` | 10,790 | `73D6928246F369AAC5DE742E149196BF4DC3B5801A6EFFEED6D014E52841D0AC` |
| EXE2BIN.EXE | `exe2bin-historical-tools-v2` | 8,424 | `EDF4009A2AB45A30AE3291B0F8C9585DE9A15B6A1262288AE6694D4693CB737E` |

### Additional LINK16 rebuilds

The same clean-stage policy now covers NLSFUNC, EDLIN, and DEBUG. Each command
regenerated its `.CTL` with `BUILDMSG`, assembled unchanged sources with the
MS-DOS 4.0 MASM, and invoked the unmodified response file where the makefile
has one.

| Artifact | Stage | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| NLSFUNC.EXE | `nlsfunc-historical-tools-v2` | 7,052 | `A49E8C6C392F0FD323B52C1FA612420A9FC0258BA59413CCB3D2D19B1F229EE8` |
| EDLIN.EXE | `edlin-historical-tools-v2` | 12,642 | `9D835A8A46406FCB01F4509550CC86EA2755C3084C95C744CBEC79D8D94C0477` |
| DEBUG.EXE | `debug-historical-tools-v2` | 20,634 | `9C230AA1CAFF2FF9D845514017B3E4BBE7B308AD26AD88740967651F7955CD60` |

### Device, keyboard, and redirector rebuilds

`ANSI.SYS`, `KB16.COM`, and `KEYBOARD.SYS` now use OpenNT LINK16 with their
original response files and the shared OpenNT suffix rules. `REDIR.EXE` uses
the exact lines from its source makefile's NMAKE here-document through a
temporary runner response file, which is removed after linking; no generated
link script remains in the stage.

| Artifact | Stage | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| ANSI.SYS | `ansi-historical-tools-v3` | 9,029 | `C929C0893BBA4C6454632D3408EE4F7661B51CF5C2CE20035DCD4283CD623C85` |
| KB16.COM | `kb16-historical-tools-v3` | 14,710 | `9C432EA71E0E11BFB2EDF9166C862826115EC4CDF8439F986A5ED71D9A3AF3B9` |
| KEYBOARD.SYS | `keyboard-historical-tools-v3` | 40,665 | `96FCAC606683CF262DBC030713A9263731EE83F19A10207E5DD632E5EE06C2AD` |
| REDIR.EXE | `redir-historical-tools-v6` | 3,338 | `B6E9FAD30A5423EAD9ECB45C8E28197EA62A39187D36241F9018DB82FACAC3A7` |

### GRAPHICS.COM v2

`graphics-historical-tools-v2` freshly generated `GRAPHICS.CTL`, assembled all
13 original modules with MS-DOS 4.0 MASM, linked unchanged `GRAPHICS.LNK` with
OpenNT LINK16, and applied the common `.exc.com` rule. The resulting 19,694
byte `GRAPHICS.COM` has SHA-256
`989B38806EC1E1BDD30DA3DC6B41CC85BDC81DDA74F870FCC7DF399876EB2756`, equal
to the source-tree reference artifact.

### DOSX Current Boundary

The `dosx-historical-tools-v3` script faithfully begins the x86/486 makefile
closure but MS-DOS 4.0 MASM does not finish its first large `486/dxstrt.asm`
module within 180 seconds under DOSBox, including a `cycles=max` rerun. A
separate 600-second controlled invocation with the full makefile defines and
`/t` also produced no object and no assembler diagnostic before the runner
terminated it. No alternate assembler has been substituted; this is a
host-execution throughput boundary, not an assembly or source-overlay result.

### COMMAND.COM v1

`command-historical-tools-v1` proves that `buildmsg.c` is only a historical
host-side message-tool source, not a guest link dependency. The checked-in
historical BUILDMSG generated `COMMAND.CTL`; MS-DOS 4.0 MASM assembled all 34
objects named by the untouched `COMMAND.LNK`; OpenNT LINK16 linked the image;
and the common `.exc.com` packaging rule produced `COMMAND.COM` at 50,384
bytes, SHA-256 `908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43`.

This supersedes the earlier WLink-only assertion for COMMAND: the recovered
Microsoft LINK16 preserves the required iterated-data representation.

### C Guest Targets: SETVER and MEM

The MS-DOS 4.0 tool snapshot includes its original 16-bit `CL.EXE`, headers,
and model-specific CRT libraries. `setver-historical-tools-v3` uses `CL.EXE`
with the makefile's small-model flags, MS-DOS 4.0 MASM for `dos.asm` and
`device.asm`, MS-DOS `SLIBCE.LIB`, and OpenNT LINK16. It produces a complete
11,909-byte `SETVER.EXE` with SHA-256
`AA4F62283C224E3861FC4F5E864DECFE293C64DF4BC5763569AAE74BE9B3F83E`.
The fixed source snapshot lacks the makefile's `usa\\message.h` precursor but
contains the historical target-directory `message.h`; the script verifies and
uses that byte-for-byte rather than synthesizing a replacement.

`mem-historical-tools-v2` confirms the larger mixed-language path: historical
BUILDMSG generated `MEM.CTL`; MS-DOS 4.0 CL compiled all five C modules and
MS-DOS 4.0 MASM compiled the three assembly modules. The original `xmm.asm`
was assembled and OpenNT `LIB16` built `XMM.LIB` in the clean stage, so no
prebuilt XMM binary was consumed. With MS-DOS `SLIBCE.LIB` and OpenNT LINK16,
the resulting `MEM.EXE` is 40,744 bytes with SHA-256
`49C144A341ECC22306B767EC7199004C979EC1D09062D2E2EC5B7055E5508180`.

### NTDOS Linker Boundary Rechecked

`ntdos-historical-tools-v6` rebuilt all 38 NTDOS modules with the complete
makefile assembler profile (`/Mx /t /W1`), then invoked OpenNT LINK16. The
same failure remains at `msdata.obj` offset 2458, record `A0`; it is therefore
not caused by the former omitted `/t` or warning flag. A separate MS-DOS 4.0
LINK 3.65 diagnostic accepted the object list but rejected the same object at
its own parser boundary (`pos: 2318`, record type `4B1E`). Neither result is a
valid image and neither justifies an object/source overlay.

A local candidate audit found that the OpenNT 4.5 Win16-hosted `LINK.EXE` and
the selected `LINK16.EXE` are byte-identical (`910ED26E...8AC1B00`), so it is
not an independent linker experiment. The only distinct OpenNT MASM candidate
is version 4.00; it was deliberately not substituted because the active
reconstruction requirement fixes MS-DOS 4.0 MASM 5.10. No third independent
Microsoft linker compatible with that requirement is present in the permitted
local source/tool inputs.

## Active Build-Surface Cleanup

The active `toolchain-probe` surface was reduced to the local-tool historical
runners, source import, suffix packaging, and explicit product build scripts.
The following obsolete execution paths were removed: generic assembler audits,
the source-built-MASM importer, the DOSBox generic linker runner, all staged
Win32 host ports for BUILDMSG/COMMAND/utility builds, and the `clang-cl` /
Visual Studio support-provider launchers. None is permitted to produce a
current guest artifact.

As a post-cleanup end-to-end check,
`artifacts/toolchain-runs/fastopen-historical-tools-v3` copied the unchanged
source closure, ran only `tools/msdos400/MASM.EXE` under DOSBox and
`tools/opennt-4.5/LINK16.EXE`, and recreated the 882-byte `FASTOPEN.EXE` with
SHA-256 `69DABBDB754B358AC4FE4B22DE04C0E4C93076816F14BB0730CAA9FD223996FC`.
The stage manifest records an empty guest-transform list.

The cleanup also removed the obsolete source-built `masm386-managed` logs and
objects plus the OpenWatcom probe artifact tree. A post-removal scan of active
scripts and artifact paths found no JWasm, UASM, WLink, OpenWatcom, or
source-built-MASM build input. Source copies named `debuasm.asm` are ordinary
historical DEBUG modules and are not UASM artifacts.

## OpenNT Tools16 Baseline, 2026-08-07

This section supersedes every earlier entry in this record that names MS-DOS
4.0 MASM 5.10 as the default assembler, or reports DOSX/NTDOS as an unresolved
assembler/linker boundary. The formal default is the checked-in OpenNT 4.5
tools16 `MASM.EXE` version 4.00, SHA-256
`E54AFAA9565E046CD862C29A015D685DE43311ACCCF796469030E162CED6D00D`.
It is a historical Microsoft Win32 host executable and emits the required
16-bit OMF objects. It runs natively; DOSBox is retained only for historical
MZ tools such as BUILDMSG, NOSRVBLD, RELOC, STRIPDD, and the bounded C fallback.

Fresh clean stages using this assembler and OpenNT LINK16 completed unchanged
source reconstructions for `NTIO.SYS`, `NTDOS.SYS`, `DOSX.EXE`, `COMMAND.COM`,
`HIMEM.SYS`, `ANSI.SYS`, `KEYBOARD.SYS`, `REDIR.EXE`, `MEM.EXE`, `SETVER.EXE`,
`APPEND.EXE`, `DEBUG.EXE`, `EDLIN.EXE`, `EXE2BIN.EXE`, `FASTOPEN.EXE`,
`SHARE.EXE`, `GRAPHICS.COM`, `KB16.COM`, `LOADFIX.COM`, `NLSFUNC.EXE`, and
`COUNTRY.SYS`. All staged manifests record an empty `GuestSourceTransforms`
array. The OpenNT-MASM results include:

| Artifact | Stage | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| NTIO.SYS | `ntio-tools16-opennt-v1` | 33,792 | `CFC8BE16576BB6ACD16BB8FC9B2D9A080F544BBFDD2D2D2DF07ED908B3AB4937` |
| NTDOS.SYS | `ntdos-tools16-historical-v1` | 27,858 | `957662320654AD5251C3A8B228A5DADEC28AA65DDDBCBA38C3658A6E7F93BC84` |
| DOSX.EXE | `dosx-tools16-historical-v6` | 36,152 | `8F21AC29995DADBBD9AC9D8260FD61ED43AC832F4E4BF2F168381524D09916F7` |
| COMMAND.COM | `command-tools16-opennt-v1` | 50,384 | `908A77AC617C2D741F0AA1B73F73973DCF29ADC91F092E5BCB02173C8C732C43` |
| REDIR.EXE | `redir-tools16-opennt-v1` | 3,338 | `B6E9FAD30A5423EAD9ECB45C8E28197EA62A39187D36241F9018DB82FACAC3A7` |
| MEM.EXE | `mem-tools16-opennt-v1` | 40,744 | `49C144A341ECC22306B767EC7199004C979EC1D09062D2E2EC5B7055E5508180` |
| SETVER.EXE | `setver-tools16-opennt-v1` | 11,909 | `AA4F62283C224E3861FC4F5E864DECFE293C64DF4BC5763569AAE74BE9B3F83E` |

`MEM.EXE` and `SETVER.EXE` retain only the explicit MS-DOS 4.0 fallback for
their historical `CL.EXE` plus matching `SLIBCE.LIB` C closure; their assembly
objects, library assembly, and final link use OpenNT MASM/LIB16/LINK16.

### Superseded Artifact Cleanup

After the OpenNT-tools16 stages were verified, all earlier guest MASM 5.10,
JWasm/UASM-era, linker-diagnostic, source-built-MASM, and obsolete DOSX
iteration stages were removed from `artifacts/toolchain-runs`. The retired
`toolchain-probe/build` output tree, which contained source-built MASM and
host-probe objects, was also removed. The historical discussion above remains
as text evidence only; it no longer names an executable build input or a
retained consumable artifact.

## Direct Guest Payload Closure

The direct NT4 DOS guest payload/install set currently resolves to 33 files.
This is distinct from the 75 declared outputs in the full MVDM inventory,
which also includes host, WOW16, and sample targets. The closure is verified
as 22 unchanged-source OpenNT-tools16 builds, seven static companion/data
files already carried by the source snapshot, and four original OpenNT
`binplace` inputs.

`QUIT.COM` was recovered from the byte-identical OpenNT `v86/util` source
closure and built with OpenNT MASM 4.00, LINK16, and RELOC. Its 3-byte output
has SHA-256 `06A37DFF559DF7325DE8B003F4DF53C188F733E0CA312AAD961C34DAE48D7B83`.
It remains a V86 termination-BOP utility, not a normal runtime component.

`BACKUP.EXE`, `QBASIC.EXE`, `MSHERC.COM`, and `EDIT.COM` have no source-build
rule in the OpenNT NT4 tree: their makefiles only use `binplace`. They are
therefore staged unchanged from the fixed OpenNT 4.5 input tree in
`nt4-prebuilt-guest-inputs-v1`, where each source/staged hash is equal and
`SourceBuilt` is explicitly false. This is a provenance result, not a claim
that copying a historical binary is compilation.

## WOW16 Device-Driver Addendum

Two historical WOW16 driver artifacts have also been rebuilt in clean,
separate stages.  They are not counted as direct DOS install payloads, but they
are relevant guest-adjacent device compatibility inputs.

| Artifact | Stage | Tool path | SHA-256 |
| --- | --- | --- | --- |
| `MOUSE.DRV` | `wow16-mouse-tools16-opennt-v1` | OpenNT MASM 4.00, LINK16, RC16 | `2FA5611BB18EDD9C0F35E0A6B479B9D9DB17549AED811AB692BD79C3E899F890` |
| `VGA.DRV` | `wow16-display-tools16-opennt-v1` | OpenNT MASM 4.00, LINK16, historical EXE2BIN, RC16/RCPP | `943C2E3499E64157D11C81C85DB7586E13AF623044880876B390FBFCFAC3CFB6` |
| `KEYBOARD.DRV` | `wow16-keyboard-tools16-opennt-v1` | OpenNT MASM 4.00, LINK16, RC16/RCPP | `C34D7D8D71453DD7567C6DB28958A0D4D11E4B198CFC436F4D430D93A5A3584C` |
| `COMM.DRV` | `wow16-comm-tools16-opennt-v1` | OpenNT MASM 4.00, LINK16, RC16/RCPP | `EE589C26791B66742F7B5E183A44C56A13053DC584B0DCA3FCC20CD87BDCA69E` |
| `SOUND.DRV` | `wow16-sound-tools16-opennt-v1` | OpenNT MASM 4.00, LINK16, RC16/RCPP | `D5932CBD4E0155A07D4D0F64C0F8AFCDC3AEFE90E06E7265C6651B10C2F03801` |

`VGA.DRV` is constructed from `config.asm` through historical LINK16 and the
already rebuilt historical `EXE2BIN.EXE`, then from `display.asm` through
LINK16 and RC16 resource binding.  No staged `CONFIG.BIN`, `DISPLAY.RES`, or
`VGA.DRV` is used as an input.  The fixed OpenNT snapshot lacks `wownt16.h`, so
the stage records an explicit byte-identical historical-header fallback from
the local OpenNT 4.5 SDK tree.  RC16's companion RCPP is located through the
same fixed tools16 directory, exactly as the original makefile's PATH contract
requires.

The present verified artifact count is therefore 38: 27 source-built artifacts
(the 22 direct DOS builds plus these five drivers), seven static companion/data
files, and four transparently staged prebuilt inputs.  The wider MVDM inventory
contains 75 declared outputs and remains incomplete, but all five declared
WOW16 device-driver directories (`comm`, `display`, `keyboard`, `mouse`, and
`sound`) now have source/tool/input closure and rebuild records.

## Final Legacy-Stage Removal

After an explicit reference audit, the remaining generated directories under
`toolchain-probe` were removed. These were obsolete assembler/C-tool stages,
CCPU backend probes, a host-stack inventory, and an upstream source copy; none
was referenced by active scripts or by the current guest artifact stages. The
directory now retains only fixed research inputs (`downloads`, `fixtures`,
`reference`, `third_party`, and `tools`) plus active reproducibility scripts.
A post-removal scan of all active `.ps1`, `.cmd`, and `.bat` files found no
JWasm, UASM, WLink, OpenWatcom, or `masm386-managed` selection.
