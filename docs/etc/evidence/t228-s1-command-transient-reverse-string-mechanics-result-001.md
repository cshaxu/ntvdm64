# T228 S1 P7 - COMMAND transient reverse-string mechanics result

## Question

Does the admitted CPU5/Pentium-MMX real/V86 compatibility profile correctly
execute the original COMMAND transient initialization form `STD; REP MOVSB`?
The question is deliberately mechanical. It does not assert that COMMAND's
`EndInit` path was reached in the native observation, and it does not select a
COMMAND or Bochs repair.

## Inputs

- Original owner/source reference: `src/opennt/base/mvdm/dos/v86/cmd/command/rdata.asm`,
  `EndInit`, which initializes the transient allocation using backward string
  movement (`std`, `rep movsb`, `cld`).
- Current fixture: `tests/bx-mantle/t225_s56_realmode_segment_limit_compatibility_fixture.cc`.
  Its existing real/V86 CPU compatibility grouping already covers scalar,
  string, stack and fetch/control mechanics.
- Formal current-source graph: `build/M0-T228-S1/formal-ninja-20260818g`,
  MSVC x64 `/MT`, CPU5/Pentium-MMX.

## Procedure

The fixture writes source bytes `34 12 78 56` at ordinary RAM `0800h..0803h`,
sets `SI=0803h`, `DI=0813h`, and `CX=4`, then executes
`STD; REP MOVSB; CLD`. It copies the destination `0810h..0813h` into its
existing terminal snapshot window and compares the complete expected byte
sequence. The formal Ninja target was rebuilt and executed:

```powershell
ninja -C build/M0-T228-S1/formal-ninja-20260818g bin/t225-s56-realmode-segment-limit-compatibility-fixture.exe
& .\build\M0-T228-S1\formal-ninja-20260818g\bin\t225-s56-realmode-segment-limit-compatibility-fixture.exe
```

## Observation

Ninja rebuilt the fixture object and executable (`[1/2]`, `[2/2]`). The
fixture exited `0`. Its terminal snapshot begins with `34 12 78 56`, proving
that the backward four-byte source range arrived at `0810h..0813h` in the
expected order. The pre-existing grouped checks also completed in that run.

## Interpretation and confidence

This is direct focused evidence that the current bx-core/bx-mantle CPU5
compatibility profile implements the relevant neutral mechanics: direction
flag handling, byte string repetition, descending SI/DI movement, and the
following `CLD`. It excludes a generic `STD; REP MOVSB` failure as grounds
for a bx-core intrusion or a guest-address-specific workaround.

It does **not** prove that native COMMAND executed `EndInit`, that its DOS
allocation succeeded, that the source bytes were present at the selected
transient destination, or that `LodCom`/`ReadCom`/reload lifecycle behavior
is correct. The retained P6 observation still transfers diagnosis to the whole
original COMMAND transient allocation/move/reload owner package.

## Follow-up

Map and test the original COMMAND package's allocation, segment selection,
initial copy/reload, and failure/cleanup paths as one unit. Do not prefill
`5A5F`, change the observed far call, add a BOP leaf implementation, or modify
Bochs from this mechanical result.