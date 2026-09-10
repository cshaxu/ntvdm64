# M0 T372 S5 P3 — Original NetLib status conversion closure

`mvdm-host/vdmredir/vrnetapi.c` calls `NetpNtStatusToApiStatus` as part of
its original LAN Manager error direction.  Its owner is not Redirector or an
adapter: the complete retained function body is OpenNT
`opennt-host/netapi/netlib/ntstatus.c`.

The formal CPU40 generator now selects that existing original translation
unit into a dedicated `original-opennt-netlib.lib`.  It is a source-library
binding only: no table, error policy or status conversion was copied into an
adapter, and the original `vrnetapi.c` call is unchanged.

External validation regenerated the formal x86 graph and built:

```text
ninja -C build/M0-T371/S2/formal-x86 -j 8 original-opennt-netlib.lib
```

The original source compiled and the library was emitted.  The source retains
its historical inclusion of `ntstatus.h`; current SDK headers emit visible
macro-redefinition warnings for overlapping status constants.  They are not
suppressed and do not alter the original conversion body.

This closes the `NetpNtStatusToApiStatus` member of the S5 import ledger.  It
does not activate a Redirector selector or resolve the distinct
`rpcxlate`/`xactsrv` transport owner packages.
