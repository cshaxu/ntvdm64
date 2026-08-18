# T228 S1 P15 — Reproducible COMMAND Link-Map Recovery Result

## Result

A fresh disposable rebuild completed with the repository-local original
OpenNT source, OpenNT 4.5 historical `BUILDMSG`, MASM 5.NT.02, LINK16 5.60,
and RELOC closure.  It emitted a 50,384-byte `COMMAND.COM` whose SHA-256 is
exactly `908a77ac617c2d741f0aa1b73f73973dcf29adc91f092e5bcb02173c8c732c43`,
the identity of the immutable T228 guest input.  This is an independently
reproducible map for that exact artifact, not a map from a similar build.

The original `command.lnk` contains `/map +`; the rebuild retained the
linker's `command.map` (48,079 bytes, SHA-256
`7b5747267bc3f323844a81cd48acba5bf9cd62f1538b414f7e35ac0f1d516c53`).
The build manifest under
`build/M0-T228-S1/command-link-map-rebuild-20260818a/command-build-manifest.json`
records source revision, all historical tool hashes, all 34 command modules,
and the matching output identity.

## Map-Supported Lifecycle Attribution

`command.map` declares the resident `RESGROUP` at origin `0000:0000` and
publishes `EndInit` at `0000:0332`; it also reports the transient segment
layout as `TRANCODE 02490h..0AE41h` and `TRANSPACE 0B518h..0C5CFh`.

P13's two source-built COMMAND calls to `54:0F` occur at runtime
`0C41:03A6` and `0C41:03CB`.  Under the COM load-segment mapping, both are
inside the exact linked `EndInit` procedure (`+0074h` and `+0099h` from its
published entry), rather than merely somewhere in the COMMAND image.

The original `rdata.asm` definition of `EndInit` gives the source-level
contract for this same procedure:

1. allocate the initial environment block;
2. call `SVC_GETINITENVIRONMENT`;
3. when returned `BX` exceeds `EnvSiz`, free the old block, allocate the
   returned paragraph count, update `EnvirSeg` and `PDB_Environ`, then retry;
4. require the retry `BX == EnvSiz`;
5. allocate the largest NTDOS MCB block, position `TrnSeg` at its top, issue
   the reverse transient copy, and deallocate that block before `LodCom_Trap`.

Thus P13/P14's size-sensitive split reaches a map-supported original
**COMMAND `EndInit` / NTDOS MCB allocation-and-transient-placement package**.
It is not an adapter BOP-provider error, a generic Bochs string primitive, or
a source-label guess.

## Limitation And Next Boundary

The linker map publishes procedure/segment addresses, but it is not an
instruction listing.  It supports the `EndInit` procedure boundary and its
transient segments; it does not alone identify each instruction within that
procedure.  No heuristic disassembly has been used for that purpose.

The next repair admission may therefore inspect only the full map-supported
`EndInit` package and its original NTDOS allocation contract, including the
post-retry largest-block/`TrnSeg` handoff.  It must not introduce a BOP leaf,
truncate the host environment, alter the immutable guest image, or patch a
Bochs address.