# OpenNT COMMAND.COM Layout Recovery Card

Status: Gate 0 bootstrap-payload blocker, 2026-08-07. This record contains
format observations only. It neither distributes a guest artifact nor admits a
staged image to the runtime.

**Supersession.** The v20 JWasm-derived image described below is retained only
as format history. It is not current source-to-image evidence: the canonical
stage now uses pristine sources and recovered MASM386 for all 34 objects, then
rejects OpenWatcom WLink because it cannot preserve original LIDATA
relocations. See `OPENNT-G0-TOOLCHAIN-REBASE.md`.

## Question

Why does the complete 34-object COMMAND build (`command-stage-v18`) produce a
non-loadable `.COM` while the earlier structural stage starts with executable
bytes?

## Fixed Source Evidence

The OpenNT makefile at
`base/mvdm/dos/v86/cmd/command/makefile` links `command.exc`, then invokes:

```text
reloc command.exc command.com
```

No output-type switch is passed to that historical tool. The command link
input has the original 34-object order in `command.lnk`.

The current v18 stage preserves that object order: 33 objects are assembled
with the recorded compatibility assembler treatment and `tprintf` is built by
the source-built MASM386 probe. Its WLink output has this MZ header state:

| field | v18 value |
| --- | ---: |
| image bytes | 50,740 |
| `e_cparhdr` | `0x0002` (32 bytes) |
| `e_cs:e_ip` | `0000:0100` |
| relocation count | 0 |

The first executable byte sequence begins at physical offset `0x120`:

```text
header bytes (0x20) + entry offset (0x100) = 0x120
```

The current locally owned `opennt-reloc` helper only removes `e_cparhdr * 16`
bytes and applies MZ relocation words. Consequently its v18 output begins
with the 256 zero bytes between the MZ module base and `IP=0100`; the staged
file is not a valid COM loader candidate.

The earlier v16 stage is not a provenance substitute, but it is useful format
evidence. Its source MZ header has `e_cparhdr = 0x20` (512 bytes) and the same
`e_cs:e_ip = 0000:0100`. Its structural `COMMAND.COM` is exactly the source
`.exc` suffix beginning at `0x300` (`0x200 + 0x100`) and therefore starts with
the observed `E9` entry byte. It cannot establish retail identity or validate
the v18 object layout.

## Cross-Check Against System Images

The selected staged source images use `e_cs:e_ip = 0000:0000`:

| input | header bytes | entry |
| --- | ---: | --- |
| `ntio.exe` | 224 | `0000:0000` |
| `ntdos.exe` | 32 | `0000:0000` |
| `himem.exe` | 48 | `0000:0000` |
| `country.exe` | 32 | `0000:0000` |

Thus they do not prove the command-entry rule, and a global unreviewed change
to header stripping would be unsound.

## Compatibility Decision And Probe

The original OpenNT `reloc` executable remains unavailable in the fixed source
inputs. The current research helper therefore uses the documented OpenWatcom
`exe2bin` compatibility condition, rather than an output-name heuristic:

```text
CS:IP = 0000:0100 and relocation count = 0
  -> remove the MZ header and the following 0x100 bytes
otherwise
  -> retain the ordinary MZ-to-raw conversion behavior
```

This is not a claim that the helper is the historical `reloc` tool. It is an
explicit, independently documented compatibility rule with the same format
preconditions observed in the complete COMMAND link. It rejects a `0000:0100`
input that has relocation entries; it leaves the known `0000:0000` system
image cases unchanged.

`OPENWATCOM-TOOLCHAIN-ADMISSION.md` records the separately acquired,
checksum-verified OpenWatcom research archive that may later cross-check this
behavior after a local, non-system extraction.

That cross-check is complete. The locally extracted OpenWatcom 1.9
`binnt/exe2bin.exe` converted the same v18 `command.exc` to a second disposable
output with the exact same result as `opennt-reloc`:

| property | value |
| --- | --- |
| byte count | 50,452 |
| OpenWatcom SHA-256 | `4F70F0F406524DD2F7E9D5B59CF60ABC8198E08913581C4365FFE03F49DFBAB8` |
| helper SHA-256 | `4F70F0F406524DD2F7E9D5B59CF60ABC8198E08913581C4365FFE03F49DFBAB8` |
| byte-for-byte equality | yes |

This validates the helper's limited conversion behavior against the selected
OpenWatcom tool. It does not prove byte identity with the unavailable
historical OpenNT `reloc` executable or validate COMMAND execution.

`opennt-reloc-test` now proves ordinary MZ relocation/header removal, the
valid `0000:0100` no-relocation conversion, and rejection of a relocation
bearing COM-shaped input. On 2026-08-07 it passed after the change.

A disposable conversion of the exact v18 `command.exc` produced:

| property | value |
| --- | --- |
| stage | `toolchain-probe/command-layout-probe-v1` |
| output size | 50,452 bytes |
| SHA-256 | `4F70F0F406524DD2F7E9D5B59CF60ABC8198E08913581C4365FFE03F49DFBAB8` |
| first bytes | `E9 9D 15 00 B7 15 00 00` |
| first nonzero byte | offset `0` |

The candidate is **structural format evidence only**. It was converted from a
previous v18 disposable link; it does not replace a fresh full source-to-image
rebuild, command execution trace, or retail-identity evidence.

The v18 manifest predates the managed-source gate and names the legacy
`upstream/opennt` checkout. It is therefore retained only as a link/layout
observation. The current `stage-opennt-command.ps1` accepts a root only when
it contains `src/opennt/SOURCE-MANIFEST.json`, verifies every COMMAND, shared
include, and message input against that manifest, and applies compatibility
rewrites only after copying into a disposable stage. A fresh candidate must
use that managed-root path.

On 2026-08-07, a fresh managed stage was created at
`toolchain-probe/command-stage-v19` from `src/opennt`; its host-side BUILDMSG
step regenerated `COMMAND.CTL`, `COMMAND.CL1` through `COMMAND.CL4`, and
`COMMAND.CLF`. This establishes the fresh input and message-generation half
of the stage, not an assembled or linked guest image. The OpenWatcom 1.9
WASM probe is recorded separately in
`OPENWATCOM-TOOLCHAIN-ADMISSION.md`; it currently fails on the historical
MASM dialect after a shallow-path control removes an unrelated output-path
failure.

## Remaining Fresh-Stage Recovery Requirement

The next COMMAND build experiment must establish one of the following from
historical tool or format evidence:

1. a LINK16-compatible header-layout option that makes the complete v18
   `.exc` retain the historical header/entry placement; or
2. the original `reloc` tool's documented/raw behavior for a nonzero MZ entry
   offset, including relocation-address treatment after any entry-prefix
   removal; or
3. a fresh complete command stage that repeats the exact 34-object v18 build
   and then uses the documented compatibility condition above.

The experiment must use a fresh disposable stage, retain the original object
order, record the exact linker version/options, and validate all of:

- a nonzero COM byte at offset zero;
- MZ source entry correspondence rather than a blind byte strip;
- relocation safety (including a synthetic nonzero-relocation fixture if the
  command link itself still has zero relocation entries); and
- unchanged handling for the known `CS:IP=0:0` system-image path.

## Managed v20 Completion

All four requirements above are now met by `command-stage-v20`. It was created
from manifest-verified `src/opennt` after its BUILDMSG host-stage correction
restored proper EN-US record boundaries. JWasm assembled 33 objects; the sole
remaining incompatible `tprintf.asm` was assembled unmodified by MASM386
rebuilt from the managed `sdktools/masm` source closure. OpenWatcom WLink then
linked the exact 34-object order, and the tested converter generated:

| property | value |
| --- | --- |
| MZ `command.exc` | 50,740 bytes, SHA-256 `590FD040F50B2BE9D1CE686BBE9E9422912BC7598DBD075101C49B296A4E1A73` |
| `COMMAND.COM` | 50,452 bytes, SHA-256 `4F70F0F406524DD2F7E9D5B59CF60ABC8198E08913581C4365FFE03F49DFBAB8` |
| initial COM bytes | `E9 9D 15 00 B7 15 00 00` |
| conversion rule test | `opennt-reloc-test` passed ordinary MZ, COM-shaped, and rejected-relocation fixtures |

The output equals the independently cross-checked structural conversion hash.
It closes the managed source-to-image requirement, not the later NTIO handoff,
COMMAND initialization, DOS execution, or retail-identity requirements.

## Prohibitions

Do not:

- accept v16 as the complete managed rebuild;
- add an unconditional `0x100` strip to `opennt-reloc`;
- edit OpenNT command source merely to force a different entry layout; or
- treat an entry-byte check as command-runtime validation.

Until one recovery decision is proven, the complete COMMAND build remains
`object/link evidence`, not a structural stage image.
