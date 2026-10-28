# C-VID vector image ownership

At base `27a9d2e63`, `evidfunc.c` remains byte-identical to both original
editions and appears in both frozen audit catalogs. Its selected production
object and valid dependency are recorded by the [observer](cvidc-vector-image-observer.py).
The [observation](cvidc-vector-image-observations.json) resolves every one of
692 COFF data relocations from 14 original read, write and mark tables to the
actual released image value and named original `SEVID` owner. This is stronger
than textual mentions: it establishes the initial linked pointer targets.

The original tables select algorithm implementations; video register and
mode logic determines which table is published, while emulated VGA state owns
planes and latches. The adapter must supply the underlying host resources.
This observation does not execute the algorithms, prove dynamic table
selection or updates, validate mode changes, bounds and aliases, or cover
teardown. The vector source is therefore **partial**. Its source text is
100% retained; the complete behavioral restoration percentage cannot be
measured from these bindings. Guest and product images are unchanged.

The selected-object [fixture](../../tests/mvdm-host/cvidc_vector_binding_fixture.c)
also verifies the exact five read pointers, all twelve write pointers, and the
256 unchained/chain-4 write-mode and state combinations against the original
table contract, plus dither, rotated, chain-2 and simple selections. It also
checks that a deliberately different table does not share the selected byte
writer. This exercises publication in the host fixture; it does not
execute the selected VRAM routines or prove guest rendering. The fixture is
linked with the existing `/force:multiple` test graph and its host seams.
The passing result, executable, map, object and source hashes are retained in
the [fixture evidence](cvidc-vector-fixture-evidence.json).
