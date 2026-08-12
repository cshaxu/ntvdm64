# T95 S7 Command-Envelope v1 Evidence

## Purpose

This record closes the adapter-only precursor admitted for the first reached
host-facing BOP, `BOP_CMD:01` / `SVC_CMDGETNEXTCMD`.  It does not dispatch a
BOP, inspect a `CMDINFO`, write guest RAM, change Bochs, or expose the CLI
runner's host target path.

## Inputs

* `src/opennt/base/mvdm/inc/cmdsvc.h` and the retained
  `t95-s7-bop-54-01-cli-command-delivery-audit-001.md`: source evidence that
  COMMAND.COM asks for an executable path and command tail through `CMDINFO`.
* The existing `bx_ntvdm_readonly_namespace_v1`: its first closed entry is the
  selected contained `\\COMMAND.COM` image and an explicitly chosen DOS drive.

## Implemented Contract

`bx_ntvdm_cmd_launch_envelope_v1` is a fixed-size copied session value.  It
accepts only an exactly terminated, ASCII `\\COMMAND.COM` namespace entry on
drive `A` through `Z`, derives an ASCII DOS path such as `C:\\COMMAND.COM`,
declares `COM`, has an empty tail, and transitions once from `PENDING` to
`DELIVERED` when copied out.  It rejects another path, non-ASCII text, and a
non-terminated full-width path without scanning beyond the fixed input array.

The value deliberately contains no host pathname, handle, pointer, CPU/BOP
record, guest address, or RAM-write request.  Therefore it is not yet the
future CLI target-command contract; that requires a separately admitted,
contained guest namespace mapping.

## Procedure And Observation

The fresh admitted root was
`artifacts/build/t95-s7-cmd-launch-envelope-c11-001`.  CMake 4.1.0 with
`C:\\msys64\\ucrt64\\bin\\gcc.exe` (GCC 16.1.0) configured the root and built
only `bx-ntvdm-cmd-launch-envelope-v1-test`.

The first process run exited `1`; source inspection identified only a fixture
constant error: it asserted twelve bytes for `C:\\COMMAND.COM`, whose ASCII
length is fourteen.  The assertion was corrected.  A bounded-path review then
replaced the preliminary `wcscmp` with an array-bounded comparison and added a
non-terminated-path rejection case.  The final rebuild and run printed:

```
bx-ntvdm-cmd-launch-envelope-v1-test: contained one-shot command contract verified
```

## Interpretation And Follow-up

Confidence is high for this narrow value contract only.  It supplies an
immutable first command envelope but does not prove OpenNT's `CMDINFO` layout,
capacity/retry behavior, or BOP dispatch.  The next source-led admission must
define a copied `CMDINFO` layout reader and a generic bounded response plan;
it must remain outside Bochs and must not consume a host target path until the
CLI's contained guest-namespace contract is explicitly selected.
