# Shard 2 selected CCPU leaf declaration closure

Four original identical headers `lahf.h`, `cmc.h`, `neg.h`, `xor.h` each contain precisely one IMPORT declaration and no additional interface; names, frozen/live hashes, expanded prototypes and dependency consumers are in `header-abi-closure.csv`. The selected `ANSI` build makes `IMPORT` extern and `IPT0/2/3` typed prototypes (`host/inc/insignia.h`, `base/inc/base_def.h`); IU32 is unsigned long, IUM8 unsigned char. Current x86 CCPU command has `/DANSI` and no `/Gz`; the definitions and calls use the selected default cdecl.

The exact worker dependency snapshot `build/t473-s10/architecture-header-deps.txt` records both `obj/ccpu/c_main.obj` and each corresponding leaf object including that exact header, all VALID. The source-file leaf has independently recorded guest and actual-image decoder correspondence in `ccpu-sign-flags` (LAHF/CMC) or `ccpu-alu` (NEG/XOR). Those receipts support compatible selected signatures/call binding, but do not turn the dependency listing into a new runtime test.

These four headers are full/100 only for their complete single-declaration contracts in the selected x86 CPU40 ANSI/cdecl build. This is not evidence for other host architectures, old non-ANSI declarations, entire CPU/decoder, invalid prefixes or faults. No guest or product changes or runtime execution occurred in this review.
