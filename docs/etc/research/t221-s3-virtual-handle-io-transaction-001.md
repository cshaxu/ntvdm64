# T221 S3 P38: Virtual handle controlled-I/O transaction

## Question

Can a `VIRTUAL_FILE` token complete the normal DEM handle I/O direction without
exposing a host handle or bypassing checked guest-memory actions?

## Observations

The whole-provider fixture opens a private Virtual file, requests the bounded
guest read for `50:1E`, commits `"hi"` to the private file only, seeks, and
then receives a bounded guest write for `50:16` containing the same bytes.
It subsequently closes the token and rejects its stale reuse. The x64 `/MT`
fixture passed at `build/M0-T221-S3/115`.

## Interpretation

The two directional memory transactions use the existing typed private file
object and do not introduce a Direct fallback. File-time, commit and
zero-length truncate cases remain part of this subfamily's final sweep.
