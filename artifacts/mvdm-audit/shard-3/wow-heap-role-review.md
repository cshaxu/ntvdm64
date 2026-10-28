# Shard 3: WOW small heap ownership

Proposed status: **partial** for `wow32/wheap.c` and `wow32/wheap.h`.
The [image observer](wow-heap-image-observer.py) and
[observations](wow-heap-image-observations.json) recheck both original edition
hashes, both source catalogs, actual object/VALID header dependency, and the
three original heap functions against the released WOW32 image outside
relocation words. Five actual linked calls resolve: DLL attach -> heap create,
accelerator alias allocation/free, cursor/icon alias allocation, and DC cache
allocation. The checked cached and published WOW32 executable sections match.

The [preprocessor runner](wow-heap-preprocess.ps1) uses the actual WOW32
compiler command; [retained excerpts](wow-heap-header-excerpts.json) show
the header declarations from both original include contexts. The
[durable verifier](wow-heap-durable-check.py) checks those excerpts, image,
objects and caller edges without requiring temporary preprocessor output.

The [single-threaded original-object fixture](wow-small-heap-probe.c) was
rerun with the same pinned source and object hashes. It successfully creates
the heap, allocates/frees 1 and 8192 bytes, validates growth, and destroys
its **test-owned** heap; [receipt](wow-heap-fixture-r82.json). It does not
execute production DLL attach/detach or actual guest alias consumers.

Original WOW owns the small-heap handle and each consumer owns the lifetime
of its own entries; Windows Heap API owns allocation. The heap is created
with `HEAP_NO_SERIALIZE`, making actual WOW task scheduling/reentrancy and
external serialization necessary verification boundaries. Source shows
small-heap creation precedes the normal heap; failure of the latter returns
without explicit small-heap cleanup, and process detach destroys the normal
heap but not this one. This establishes source behavior only: process teardown
reclaims process memory, while explicit unload/reload, partially initialized
load, failing allocation and late callbacks still need targeted evidence.

Both source files retain 100% original bytes; complete behavioral recovery
percentages are unmeasurable. No product or guest binary changed.
