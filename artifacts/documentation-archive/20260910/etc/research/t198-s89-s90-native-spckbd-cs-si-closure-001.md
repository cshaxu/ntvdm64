# M0 T198 S89/S90: Native SPCKBD Stream Handoff

## Result

S89 rebuilt 56 current adapter/CLI objects from the registered composition
manifest and linked them only against the inherited CPU5/mantle/guest-image
closure.  The real guest reached `C4 C4 5F` with `AX=BEEF` at `8DC8:45A6`.

The first observation still transferred to `C000:014A`.  Source review then
found the S87 table-base error: OpenNT `kb_setup_vectors()` reads the kio table
from `effective_addr(getCS(), getSI())`; only the returned offset is relative
to `KbdSeg/getDS()`.  S90 corrects that exact split.

The corrected native r1 run no longer transfers to C000.  It reaches bounded
stop at `8DC8:0986`, with next BOP bytes `50:3D`.  This is evidence that the
original guest selected the stream branch; it is not an output claim.

## Evidence

- `src/opennt/base/mvdm/softpc.new/base/keymouse/keybd_io.c`: `CS:SI` table,
  `DS`-relative state write.
- `artifacts/build/t198-s89-current-composition-native-r3/run.log`: real BOP
  5F observed, pre-correction C000 path.
- `artifacts/build/t198-s90-cs-si-stream-r2/run.log`: corrected focused
  transaction test passes.
- `artifacts/build/t198-s90-cs-si-native-r1/run.log`: real BOP 5F observed,
  then bounded stop at `8DC8:0986` / next `50:3D`.

## Limits

No text sink, BOP 42, VGA, ROM, PIC, keyboard/timer device behavior, host
display API, or Bochs modification was added.  The next service is `DEM 50:3D`
and needs source ownership classification before any provider action.
