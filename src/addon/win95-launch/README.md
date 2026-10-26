# Win95 offline launch utilities

This directory contains two AMD64, offline-only producer utilities for the
owner-approved recoverable-installed-tree workflow. Neither binary is loaded
by Run16, NTSRV, NTVDM, NTVWM or a guest.

- `patch95.c` validates the exact expanded VMM32 input identity, applies only
  the byte ranges in `deltas.h`, validates the exact patched identity, and
  writes a fresh output file. Its DS decompressor is a bounded adaptation of
  `JHRobotics/patcher9x` `pe/ds_decompress.c` revision
  `b6e30d4b5a396dcd453b6c8e6733fd5b5cbce59e`, under the retained MIT notice
  `DS-LICENSE.txt`.
- `win95_path.c` reads a supported CREG hive to discover `SystemRoot`, or
  writes a fresh candidate with bounded path substitutions and rebuilt
  checksums. It uses public format documentation only; no libcreg code is
  linked or copied.

`BUILD.CMD` is the sole producer entry point and compiles both with the local
MSVC Build Tools in `/W4 /WX /MT` mode. Installation and rollback ownership
resides in `tools/win95-launch` and `tools/win95-path`; do not add a runtime
consumer or treat these tools as guest inputs.
