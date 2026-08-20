# T231 S3 OpenNT `cmdmisc.c` registration witness

## Question

Can the original `cmdSetInfo` (`54:05`) register the guest CMDINFO state for
later COMMAND owners without retaining an unsafe guest pointer, while the
original default `cmdSaveWorld` (`54:03`) remains an ordinary no-op?

## Inputs and procedure

The admitted `cmdmisc.c` mirror retains both original function bodies.  The
minimal extension to `command_misc_shim` adds a versioned COMMAND session
whose packed `SCSINFO`, `SCS_ToSync`, `IsDosBinary` and `FDAccess` fields have
the original layouts.  On `54:05`, its three original `GetVDMAddr` calls map
to checked real-mode addresses in their original call order, seed the session
from guest memory, and retain only that session-owned copy.  The imported
globals point to the copy; no raw guest pointer escapes the boundary.

The focused x64 `/W4 /WX /MT` fixture compiled, linked and ran in
`build/M0-T231-S3/command-misc-registration-r1/`, producing:

```text
T231 S3 direct OpenNT cmdmisc registration: CMDINFO state and original SaveWorld no-op verified
```

## Observations

- all three registered real-mode addresses and their initial guest bytes were
  retained in the session;
- the original imported globals pointed at the session fields, matching the
  original relationship among `pSCSInfo`, `pSCS_ToSync`, `pIsDosBinary` and
  `pFDAccess`;
- `54:03` retained its disabled-by-default original `CHECK_IT_LATER` body and
  returned with no fabricated CPU delta;
- no v1 COMMAND object or provider was used.

## Limit

The session establishes the persistent mapping needed by future `cmdexec.c`,
`cmdpif.c` and `cmdmisc.c` owners.  Those later source bodies must explicitly
flush changed session fields through checked RAM; this record does not claim
their lifecycle or engine-route closure.
