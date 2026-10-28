# SoftPC host GFI alternative providers: selected-profile audit

This review covers three `softpc.new/base/disks/` C files. It does not modify the shared ledger, compile guest code, or establish any real-media runtime result.

| Source | Original behavior | Observed selected x86 PROD profile | Disposition |
| --- | --- | --- | --- |
| `gfi_ibm.c` | Under `SLAVEPC`, constructs requests to an external IBM PC floppy controller over a remote-procedure link. | `obj/disks/gfi_ibm.obj` is built and included in `original-softpc-disks.lib`, but contains no executable section or ordinary C function symbol in the existing object. | Pending original capability decision and dependency analysis. |
| `gfi_sflp.c` | Under `SLAVEPC`, models commands, DMA, interrupt status and activation for the external slave floppy. | `obj/disks/gfi_sflp.obj` likewise exists in the archive without executable body in this profile. | Pending with `gfi_ibm.c`; no actual external-controller transfer is proved. |
| `gfi_test.c` | Under `#ifndef PROD`, provides the original tracing/success pseudo-floppy test provider. | `obj/disks/gfi_test.obj` exists in the archive without executable body under `/DPROD`. | Pending scope decision for the original diagnostic profile; not a replacement for real floppy tests. |

Original `disks/sources` lists all three files. `build/.ninja/msvc-x86-release/worker/build.ninja` compiles each and lists each in the archive; the generic compiler flags include `/DPROD` and no `/DSLAVEPC`. In `gfi.c:155-158` the diagnostic call is guarded by `#ifndef PROD`; its slave-server validation, change and activation branches at lines 294–345 are guarded by `#ifdef SLAVEPC`. The selected route reaches `host_gfi_rdiskette_*` instead, whose existing owner review is `../nine-way/part-4/gfi-real-floppy-provider-review.md`. These facts establish configuration and source selection, not an end-to-end disk operation.

The three existing object SHA-256 values were respectively `62801bb5d352576758c85c5b7b1c68d48ddd14ed5934a65cb4b031690c21e642`, `f86874b51f2bc711484aa6bcded581e1f59156f4af3b22403380dfb5ecf9547b`, and `3fd49df37c61938f2a8c3ff33cc0e591e98a9fd4f25dec1c157828f3b4fb58f9`. `objdump -t` shows only compiler feature metadata, `.drectve`, `.debug$S` and `.chks64`; an archive listing is not equivalent to a linked provider or observed behavior.

Keep the three source rows pending while the original dependency closure, supported configurations and equivalent providers are assessed. The normal real-floppy path separately requires attached-media read/write/format, media change, errors and provider retirement; existing no-media reset and fake-host tests cannot prove those behaviors.
