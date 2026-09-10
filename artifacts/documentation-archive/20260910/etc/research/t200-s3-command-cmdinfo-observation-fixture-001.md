# T200 S3 — COMMAND CMDINFO observation fixture

## Result

The native research bridge now observes only `C4 C4 54 01`.  Before normal
composition dispatch it copies event register facts and performs one existing
mantle READ action over exactly `CMDINFO`'s source-defined 36 bytes at
real-mode `DS:DX`; it then decodes fixed-width fields.  After normal dispatch
it records acceptance/outcome and COMMAND lifecycle facts.  It does not write
guest RAM, alter the event/result, change provider selection, or enter product
sources.  The result remains fixture-only.

## Verification

`tools/probe/Invoke-T198S94SourceBuiltNormalReturnNativeProbe.ps1 -CompileOnly`
rebuilt the source-built COMMAND/SHARE inputs, full current x64 `/MT`
composition, and modified bridge at
`artifacts/build/t200-s3-cmdinfo-compile-r3/`.  Its record states `runs: 0`;
no guest execution occurred.  The compile-only switch also fixes its former
manifest-list/switch-name collision.

## Follow-up

The next admitted packet may make one native run to obtain this complete
COMMAND ABI observation.  That run is evidence only; its result must select a
complete COMMAND repair package rather than a per-service patch.
