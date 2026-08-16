# T221 S3 P39: Private file-time contract

`src/opennt/base/mvdm/dos/dem/demhndl.c:demFileTimes` defines `BL=0` as
get time/date to `CX/DX` and `BL=1` as set time/date from `CX/DX`. The private
store/file-object now exposes a typed DOS time/date setter, and the private
handle path uses it for both Overlay and Virtual tokens after kind validation.

The whole-provider x64 `/MT` fixture at `build/M0-T221-S3/118` proves a
Virtual `BL=1` set of `1234h/5678h`, followed by a `BL=0` get returning those
same values. It performs no host-file mutation. Device-time (`BL=2`) remains
the pre-existing non-token route.
