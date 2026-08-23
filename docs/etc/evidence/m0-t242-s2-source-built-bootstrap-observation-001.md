# M0 T242 S2 Source-Built Bootstrap Observation

## Purpose

Record the first bounded source-built guest observation after removing the
selector-only EXEC probe and correcting the machine-stage reset contract.  It
is integration evidence only: it neither implements a newly reached BOP nor
claims that guest `INT 21h/AH=4Bh` has been reached.

## Source Basis

- `dos/v86/doskrnl/bios/sysinit1.asm` emits `BOP 12h`, uses its AX result to
  establish conventional-memory state, and later sets `AH=88h` before
  emitting `BOP 15h`.
- `softpc.new/base/bios/bios.c` maps `BIOS[0x12]` to `memory_size` and
  `BIOS[0x15]` to `cassette_io`.
- `softpc.new/base/bios/reset.c` writes the conventional-memory word at BDA
  physical address `0x413`; the finite machine stage owns the corresponding
  fixed PC datum for its 640 KiB conventional-memory aperture.

The machine-stage implementation therefore publishes bytes `80 02` at
`0x413` before optional external staging bytes.  Its initial-state action is
now mechanically optional only when the request-clear all-zero action record
is supplied; any nonzero action still must satisfy the existing fixed action
ABI.  No DOS selector, service, provider or OpenNT business term enters
`bx-core` or `bx-mantle`.

## Reproduction

Generate and build the disposable formal graph outside the sandbox:

```powershell
powershell.exe -ExecutionPolicy Bypass -File tools/build/New-T225S7FullNinjaGraph.ps1 `
  -RepositoryRoot O:\repos.hobby\ntdos64 `
  -BuildRoot O:\repos.hobby\ntdos64\build\M0-T242-S2\formal-r8
cmd.exe /c "call build\M0-T242-S2\formal-r8\msvc-x64-mt.cmd && ninja -C build\M0-T242-S2\formal-r8 bin\t242-s2-machine-stage-reset-fixture.exe bin\ntdos64-native.exe"
build\M0-T242-S2\formal-r8\bin\t242-s2-machine-stage-reset-fixture.exe
build\M0-T242-S2\formal-r8\bin\ntdos64-native.exe --dos-root build\output\dos --wow16-root build\output\wow16 --instruction-tick-budget 100000 --observe-first-fault --observe-budget-terminal-position --observe-ud-sequence --observe-bop-sequence build\output\dos\share.exe
```

The 257-edge formal Ninja build and the machine-stage fixture both return
zero.  The fixture proves the `0x413` little-endian value and the empty
initial-state case without a selector/provider mock.

## Observed Native Route

The source-built bundle observes these accepted BOP windows, in order:

1. `50:11` at `0070:0475`;
2. machine `12` at `010D:0516`;
3. `50:3B` at `8E08:055C`;
4. `50:0F`, `50:1B`, `50:32`, `54:05`, and `50:46` in the original bootstrap
   continuation.

The next BOP is `15` at `8E08:0655`, with window `C4 C4 15 26` and AX
`88A7`.  It is not currently owned by the generic bridge and correctly
declines.  After that decline, the bounded run reaches its instruction budget
at `0000:018A`; it does not assert a successful guest EXEC, COMMAND child, or
parent-return result.

## Disposition

`BOP-DEPENDENCY-102` records BOP 15 as an original machine/BIOS prerequisite.
Its `AH=88h` edge belongs to the future selector-blind machine package, which
must preserve the source contract without moving BIOS/DOS semantics into
`bx-vdm`, `bx-core`, or `bx-mantle`.  T242 S2 has therefore advanced from
"no source-built BOP observed" to a reproducible owner-classified machine
frontier, but its own NTDOS EXEC observation exit criterion is still open.
