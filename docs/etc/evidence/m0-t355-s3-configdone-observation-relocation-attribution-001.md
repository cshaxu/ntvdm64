# M0 T355 S3 — ConfigDone observation relocation attribution

Date: 2026-09-01

## Question

S2 selected one source-positioned `ConfigDone` observation to distinguish a
missing `trys` write from later loss of `commnd`.  This record evaluates the
one admitted fixed-container observation without treating an observer byte
address as a guest conclusion.

## Build and focused proof

The isolated CPU40/x86 graph was generated at
`build/M0-T355/S3/formal-x86` and its generated MSVC-environment Ninja wrapper
compiled and linked `original-softpc-process.exe` successfully (425 actions).
The standalone observer fixture also passed:

- absent `MVDM_CONFIG_DONE_REPORT_PATH` caused zero guest reads, zero guest
  writes, and no report;
- a supplied report path caused exactly two synchronous read leases (one byte
  for the pass and one bounded 64-byte command record), zero writes, and a
  copied report;
- all leases ended inside the call.

## One admitted observation

The sole S3 non-debug fixed-container run used the unchanged `O:\ntvdm`
media manifest and the S3 formal product.  It timed out after the fixed eight
seconds but reached source-defined `BOP_NOTIFICATION` with `AL=0` and wrote:

```text
MVDM-CONFIG-DONE al=00 pass=02 command=?P?P&?= command-state=copied
```

Its concurrent durable BOP trace again reached original `54:05`, `54:0C`, and
the later `50:12 ... 50:3D` path.  The existing DEM record still observed
`C:\COMMAND.COM`.

## Static relocation check

The selected `ntio.map` names `multi_pass_id` as `009D:03D8` and `commnd` as
`009D:3466`.  Those are map/file-layout locations: the map's `SYSINITSEG`
starts at linear file offset `009D0h`.  The selected original loader
`nt_msscs.c::InitialiseDosEmulation` instead reads the whole immutable
`NTIO.SYS` file at `NTIO_LOAD_SEGMENT:NTIO_LOAD_OFFSET`, defined as `0070:0000`
(`nt_msscs.c:213-234`, `vint.h:51-52`).

Direct selected-binary inspection confirms the distinction:

```text
NTIO.SYS file offset 0x3e36: \COMMAND.COM\0
NTIO.SYS file offset 0x0da8: zero-initialized multi_pass_id storage
```

The S3 recorder incorrectly treated the map segment `009D` as the live
segment.  Its effective command read was linear `009D0h + 3466h = 03E36h`,
whereas the original loader makes the selected file offset live at
`00700h + 03E36h = 04536h`.  The observed text is consequently an address
translation artifact.  It neither proves nor disproves the `trys` write.

## Disposition

S3 is closed as a source-backed inability result for its first recorder
binding.  The next bounded S corrects only the recorder's map-to-loaded-image
translation by passing the already-live original `getCS()` value at
`MS_bop_E`; it preserves the original `UMBNotify`, `demDasdInit`, BOP return,
and all guest semantics.  It permits one new fixed-container run solely to
obtain the corrected copied state.
