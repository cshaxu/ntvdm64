# Real DOS DPMI resize and failure witness

The isolated current sealed package ran the existing `D36R.COM` test input,
SHA-256 `05B817EE7507CB20B0F8C27B0420F3B822B3B9C3CFDCD7368A705C183FADE43E`.
The prebuilt guest reports entering protected mode, allocation, failed growth
with preserved data, a subsequent forced relocation, data verification and
release. Both `S36_DPMI_FORCED_MOVE_FAILED_GROW_DATA_FREE_OK` and
`S36_DPMI_ALLOC_REALLOC_DATA_FREE_OK` appeared; the owning Run16 exited zero.
Package and test input identities, console output, result and the test-only
runner are retained here. The temporary Z: mapping was removed after the run.

This is a live DOS guest result for this prebuilt probe and package, not a
proof of every allocation, descriptor or failure path. It does not identify
every original source used to build the pre-existing probe. No guest was
rebuilt or modified, and no installed image or product source was changed.
