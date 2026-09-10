# M0 T280 S3 — original host static-data build island

The first build island contains exactly two byte-identical original files:

- `softpc.new/host/src/nt_ertbl.c`, which defines the original host error
  classification table; and
- `softpc.new/host/src/nt_mess.c`, which defines original host error text.

Neither source contains BOP selection, CPU execution, guest-memory access or
host process control. They are linked only into `host-static-data.lib`, never
into the app or a provider route in this packet.

`host/inc/error.h` is byte-exact but supplies only documentation/references to
`ERROR_STRUCT`, `EH_*` and `EV_*`. The identical historical definitions remain
in `base/inc/error.h`. `adapter-softpc/include/error_abi.h` transcribes only
that declaration shape and is force-included by this build graph; it adds no
behavior and does not change an original header or source body.

The graph uses MSVC `/MT /W4` and Ninja `deps = msvc` on x86 and x64. No
warning suppression, original source rename, mapping-manager operation or
Bochs dependency is permitted.
