# T230 S15 P3 v2 Direct DEM host session

The v2 Direct DEM host session owns a 64-slot `HANDLE` to 32-bit token table,
attribute capability callbacks, and checked-RAM callback bindings.  It has no
`bop-v1` provider, dispatcher, namespace or result-bridge dependency.

Focused evidence:

- token publish, lookup, release and post-release rejection return `EXIT=0`;
- the Direct DEM native-session fixture binds this host session, accepts
  `C4 C4 50 1F` through the v2 bridge and imported OpenNT dispatcher, rejects
  non-DEM and unbound cases, and returns `EXIT=0`;
- the focused link includes only v2 Direct objects, OpenNT DEM v2 library,
  mantle/core and platform libraries; no `bop-v1` object is linked.

The generic bridge is not yet replaced in this record; that is S15 P4.
