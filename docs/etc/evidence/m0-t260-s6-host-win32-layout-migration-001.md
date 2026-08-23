# M0 T260 S6 — host and Win32 facade layout migration

## Owner decision

The former generic `bx-vdm/bop/shim` directory did not represent one owner.
S6 splits its host-facing portion by interface role:

| Destination | Contents |
| --- | --- |
| `opennt-host/capability/` | 43 pre-existing COMMAND, DEM, XMS, Redirector and top-level host-capability files, plus six host drive/namespace files from top-level `bx-vdm`. |
| `adapter-win32/facade/` | `opennt_rtl_compat` and `opennt_error_dialog_facade`, whose public old-API shapes are implemented with public modern Win32. |
| `opennt-bop/route/` | `dem_ingress_shim`; its byte-level `50:xx` selector recognition proves it is BOP routing, not host capability. |

The remaining transitional shim files are exclusively the S7 candidates:
mapping-manager, DPMI descriptor/startup/xmem seams, and SoftPC device
mechanics. No BOP provider body or Win32 facade was left among them.

## Include transition

OpenNT DEM, COMMAND and XMS mirrors that formerly used `../../shim/...` now
name `opennt-host/capability/...` directly. This is a pathname-only component
boundary repair; it does not alter an original provider's algorithm, ABI,
failure disposition or BOP route. SoftPC/DPMI mechanical include paths remain
for S7, where their destination exists.

## Deferred build work

Formal Ninja source lists and include roots remain S8 work. S6 validates
source ownership and static paths, but cannot claim a fresh formal build until
that unified graph update occurs.
