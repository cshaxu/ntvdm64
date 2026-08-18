# T228 S1 P6 — Far-call base-pointer provenance result

## Result

The registered, default-off `BX-CORE-087` scalar derivative rebuilt the complete CPU5/Pentium-MMX MSVC x64 `/MT` provenance closure at `build/M0-T228-S1/far-call-bp-provenance-20260818a`.  Its focused terminal-position fixture passed.  The hash-locked Direct and Readonly observations in `build/M0-T228-S1/far-call-bp-provenance-observation-20260818b` each exited through the same finite budget terminal, with byte-identical stdout (`6e32994102f4dd2c0a714c59043710ae742078545d262d7d68534bea4583f451`) and stderr (`905ddf181613eb9b79dfeec0a507def81b0c26ffb1a1518ce10e4a19b0166406`).

The retained transition now carries the additional scalar:

```
previous=1797:3AB8 bp=CAE0
current =5A5F:5EF9 bp=CAE0
predecessor=FF9E4A971793109717000000000000
successor  =000000000000000000000000000000
```

The fresh default formal graph at `build/M0-T228-S1/formal-ninja-20260818g` completed all 381 targets. Its focused `t228-s1-budget-terminal-position-fixture.exe` passed, proving the non-diagnostic CPU5/Pentium-MMX MSVC x64 `/MT` closure after the scalar record layout change.

## Original COMMAND correlation

`FF 9E 4A 97` is interpreted only in this evidence record as the documented 16-bit indirect far-call form `call far SS:[BP+974Ah]`.  With the copied values, 16-bit offset arithmetic gives `CAE0h + 974Ah = 622Ah`; the operand is therefore at `0C41:622A`.

The hash-locked source-built `COMMAND.COM` maps a COM runtime offset to file offset minus `100h`.  At file offset `612Ah`, the bytes are `F9 5E 5F 5A`: the little-endian far pointer `5A5F:5EF9` observed at runtime.  The intended target maps to file offset `5DF9h`, where source-built bytes begin `51 52 56 57 55 9C`; it is not an all-zero static image region.  The linked `command.map` identifies that range as `TRANGROUP` / `TRANCODE` (origin `0249:0`), while `parseline` is within the same transient group.

This proves that the pointer producer and relocation are source-consistent: a resident COMMAND return/reload path selected the dynamically assigned transient segment `TrnSeg=5A5F`.  P5 proves the destination bytes were zero at the moment of the far transfer.

## Owner disposition

The demonstrated defect is the complete original COMMAND transient allocation/move/reload package, not a BOP leaf, host mutation profile difference, or bx-core execution defect.  The relevant original flow is `rdata.asm` EndInit (initial `ResGroup:TranStart` to `TrnSeg` move), `command2.asm` `LodCom`/`LodCom1` (largest-block allocation, `TrnSeg`, checksum, `LoadCom`/`ReadCom`, `TranVars` copy, `TJmp`), and the resident/transient trap tables in `uinit.asm`/`stub.asm`.

The next work must map this package's guest DOS allocation/read/close calls and its OpenNT DEM dependencies as one source/ABI/failure unit.  It must not seed `5A5F` with an address-specific image copy, change the far call, or create a trace-selected BOP provider.