# M0 T207 S2/S3 — passive normal-terminal sequence evidence

S2 added a default-off, bx-vdm-owned observer after the product generic bridge
has already accepted an outcome. It copies no pointer and makes no provider,
CPU, outcome, timer, engine or CLI change. Its only states are normal
`54:0B` resume seen, STOP before that resume, and first STOP after it.

The focused MSVC x64 `/MT /W4 /WX` regression compiled and ran:

```text
cl ... bx_ntvdm_normal_terminal_sequence_observation_v1.c
  bx_ntvdm_normal_terminal_sequence_observation_v1_test.c
```

It proves disabled/no-match/wrong-order and return-then-stop behavior.

S3 rebuilt the existing copied engine direct-composition fixture under MSVC
x64 `/MT` with the observer and ran it twice at
`artifacts/build/t207-s3-engine-terminal-sequence-r1/`. Both runs completed
engine installation, real-mode entry, CPU loop and reset. The first-run
output is:

```text
t200-s24 first terminal=5 detail=1
t200-s25 stop cs=8dc8 eip=00000984 bytes=15 c4 c4 50 3d
t207-s3 sequence normal=0 stop-before=1 stop-after=0
```

`50:3D` is original `SVC_DEMEXITVDM` in `src/opennt/base/mvdm/inc/dossvc.h`.
This run therefore proves a source-owned DEM exit STOP occurs before any
accepted final COMMAND `54:0B` resume in the current integrated profile.
It does not prove a normal COMMAND terminal result, and it authorizes neither
an engine result change nor a CLI exit-code mapping.

T207 closes as a passive disproof for the current fixture. The next owner
package is DEM lifecycle/termination as a whole, not a new `54:0B` endpoint.
