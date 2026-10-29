# Win95 offline patch utility

This directory produces the AMD64 offline patch95.exe for the
owner-approved recoverable-installed-tree workflow. The binary is not loaded
by Run16, NTSRV, NTVDM, NTVWM or a guest.

- `patch95.c` validates the exact expanded VMM32 input identity, applies only
  the byte ranges in `deltas.h`, validates the exact patched identity, and
  writes a fresh output file. It also prepares identity-checked kernel and
  SETVER copies; already-patched inputs produce the same output and unknown
  inputs are rejected. Its DS decompressor is a bounded adaptation of
  `JHRobotics/patcher9x` `pe/ds_decompress.c` revision
  `b6e30d4b5a396dcd453b6c8e6733fd5b5cbce59e`, under the retained MIT notice
  `DS-LICENSE.txt`.
`BUILD.CMD` is the sole producer entry point and compiles it with the local
MSVC Build Tools in `/W4 /WX /MT` mode. Installation and rollback ownership
resides in `tools/win95-launch` and `tools/win95-path`; do not add a runtime
consumer or treat these tools as guest inputs.

The separate src/addon/path95 component produces path95.exe for path repair.
