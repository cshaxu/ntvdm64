# M0 T96 S4 Unified COMMAND Plane ABI 001

`bx_ntvdm_command_plane_v1` is a fixed-width, data-only classification record
behind common BOP ingress and provider selection. It maps all 17 callable
COMMAND services to original components: lifecycle 1, launch 4, config 2,
execution/redirection 5, console/VDD 2, environment 1, session 1, and the
original compile-disabled no-op 1. It rejects `SVC_CMDLASTSVC`.

No historical COMMAND code is linked, and no guest pointer, Win32 handle,
process, console, VDD, callback, or CPU result appears in the ABI. Services 8
and 10 are explicit-unavailable in the first CLI DOS profile; service 3 is
the original no-op; every other service remains deferred pending its complete
component contract.

Focused strict Clang C11 compilation/test covers services 0--16 through common
ingress/registry and rejects the sentinel. The default MinGW probe remains
unavailable because MSYS2 cannot create temporary files in `C:\\msys64\\tmp`.
