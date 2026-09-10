# T222 S2 Drive-View Free-Space Session Regression

## Question

Does `50:0E demGetDriveFreeSpace` consume the immutable admitted C: volume
snapshot through normal Direct/Readonly package-session dispatch?

## Observation

The normal fixture invokes the service with `AL=2` after binding its captured
C: volume snapshot.  Both Direct and Readonly resume at the post-BOP RIP,
write the five historical result registers (`AL/BX/CX/DX/SI`), and explicitly
clear CF.  MSVC x64 `/MT` build/link/run in
`build/M0-T222-S2/026-drive-view-free-space` exited zero.

## Interpretation

The test asserts source-shaped register layout and success terminal without
hard-coding mutable host free-space values.  Unadmitted-drive failure and the
remaining volume/DPB identities remain part of the full family matrix.
