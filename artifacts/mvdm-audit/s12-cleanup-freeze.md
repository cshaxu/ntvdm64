# T473 S12 excluded-branch cleanup: bounded checkpoint

The owner requested a finite finish and then clarified that source shared by
multiple compilation units is excluded. The final candidate in three shared
headers was therefore discarded before the final build. Previously committed
header changes predate that clarification; this checkpoint does not assert
that all 102 changed headers are single-consumer, and does not silently undo
committed work. Their disposition against the clarified boundary remains open.
The subsequent [consumer join](s12-shared-header-check.md) found 83 headers
with multiple observed consumers; it does not determine branch-level status.

## Scope and evidence

- From `b90a0d900^` through `39847fc3e`, the recorded source diff under
  `src/mvdm` and `src/non-mvdm` is 333 files, 2,902 insertions and 49,211
  deletions: 229 C files, 102 headers, and two source registers. This is a
  Git diff count, not a count of independently compiled lines or a claim that
  all excluded branches in the tree were processed.
- The same interval changes 114 audit/status files by 7,219 insertions and
  three deletions. Per-batch condition evidence and retained declarations are
  in this directory; source markers and the mirror registers identify the
  exceptions. Conditional shells remain around removed branch bodies.
- Decisions used observed build entries and, where relevant, their actual
  include contexts; macro names alone are not proof of a global selection.
  `MONITOR` is selected for some units and not others. CCPU software V86 and
  the selected Video7 path are retained. No guest was modified or rebuilt.
- This checkpoint does not establish that each previously changed header had
  only one consumer, nor that unobserved build configurations are equivalent.
  A shared header's repeated inclusion is not counted as multiple independent
  code removals.

## Remaining boundaries

- The x86 `CallProc32` selection and its original assembly dependency remain
  unmodified pending behavioral ownership review: [evidence](s12-i386-held.md).
- `SPC486`, `TAKE_REAL_MODE_LIMIT_FAULT`, and other ambiguous CPU conditions
  remain unmodified; software V86 is not the discarded hardware monitor.
  See [CPU evidence](s12-cpu-mouse-held.md). The authored Video7 DAC fallback
  is retained; see [Video7 evidence](s12-non-v7vga.md).
- Shared code, mixed or unobserved configurations, and conditions without a
  proved entry are not an S12 completion claim. The existing queued original
  source recovery and mirror-diff review retain those broader questions;
  this checkpoint does not introduce a new feature or reorder that queue.

## Build and delivery evidence

After discarding the shared-header candidate, cached MSVC x86 and x64 builds
completed successfully; the selected x86 `VDMREDIR.dll` target was also
explicitly built and reported no work. With the existing package identity
applied, `ntmon.exe` is byte-identical to the published image and each of the
other nine images differs only in four PE linker timestamp bytes. This is not
raw SHA-256 equality between the fresh builds and the release. Every existing
asset and installed image separately matches the ten-component release
manifest SHA-256 exactly. The existing release was retained, not replaced by
the timestamp-different rebuild.

The prior ANSI-restoration candidate passed the Full suite and Win3.1 mode
matrix as recorded in [its evidence](condition-inventory/ansi-restoration/README.md).
Those are prior runtime results, not a newly executed Full test for this
checkpoint. The owner requested avoiding repeated tests for unchanged images;
the timestamp-only comparison is recorded as the practical limit of that
decision, not as proof of every runtime or preprocessor configuration.
No new production runtime testing or publication is claimed here.
