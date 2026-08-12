# M59 Historical Provider Archive Purity

Status: completed isolated x86 source-object rebuild, 2026-08-08.

## Build

The fixed `src/opennt` island was rebuilt with its configured clang-cl x86 /
NMake toolchain:

```text
target: ntdos64-opennt-historical-provider-archive
result: success
```

Artifact:

```text
artifacts/build/current/opennt-r5-clang-x86/
  ntdos64-opennt-historical-provider-archive.lib
bytes:  2305382
sha256: 07ABDDC1F9AA09F5AFDC3DA763B6C7807EB2A0C02DB92E4A079D25C404C5EDEA
```

The archive compiles the selected BIOS, DEM, CCPU platform, system, support,
keyboard/mouse, disk, communications, video, XMS, suballocation, and UMB
provider object groups from the fixed historical input island.

## Isolation Correction

Before this rebuild, `ntdos64-opennt-host-stack-objects` also compiled the
frozen `local/shim/ccpu_register_abi_bridge.c`. That source was removed from
the general historical host-stack target. It remains explicitly linked only by
named bounded trace fixtures. A search of this archive's response file finds
no bridge object.

The archive still uses documented imported-source overlays where the isolated
historical build requires them. It is therefore evidence of the fixed
historical-input/overlay object closure, not a claim of pristine retail build
reproduction.

## Fresh Rebuild Revalidation

On 2026-08-08, the target was configured and built again in a fresh directory:

```text
generator: NMake Makefiles
compiler:  clang-cl 22.1.8, i686-pc-windows-msvc target
directory: artifacts/build/current/opennt-m59-fresh-clang-x86
target:    ntdos64-opennt-historical-provider-archive
result:    success
bytes:     2329118
sha256:    CE6C02905636EDE24B03D111F1664CD33B3E107A0556DD42F41A73FF22EB998B
```

The fresh archive response files contain no `ccpu_register_abi_bridge`
reference. The earlier successful archive has a different size and hash, so
this is configuration-level rebuild evidence, not a bit-for-bit
reproducibility claim. Any future reproducibility work must identify the
archive/object variation before asserting stronger equivalence.

## Non-Claim

This is a static library only. It neither links an executable host process nor
executes a monitor, V86 path, BIOS selector, BOP/SVC, DEM operation, guest
image, or DOS program. It does not relax M57/M58: the original x86 execution
profile requires V86, while an x64 research runtime needs a governed software
backend.
