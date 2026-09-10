# T198 S102 post-handle-ABI native observation 001

## Observation

One rebuilt exact-input native run was retained at
`artifacts/build/t198-s102-post-handle-abi-native-r1`.  It again ends in the
bounded fixture stop at `next=50:42`, not a DOS result.

The copied fast-read observation changed from S97's `EAX=4005h` to
`EAX=4E53h` after the AX:BP correction.  This proves the corrected codec
affects the live guest route, but the retained fast-read log does not include
BP or flags.  It cannot prove whether the current namespace token matches or
whether the provider returned CF/error.

## Next

S103 may add only default-off copied-state observation fields for the existing
`50:42` marker (`BP` and flags), with no guest read, service routing, provider
or Bochs change.  A later separate run may use that expanded observation.
