# M0 T458 S1 — resource and accelerator owner matrix

**Status:** closed source/boundary audit. This records the selected original
owners and the finite S2 conversion acceptance boundary. It does not claim
that the residual W09 family has passed real Win16 application acceptance.

## Method and selected graph

The audit followed the formal x86 `wow32.dll` graph in
`build/.ninja/msvc-x86-release/build.ninja`, every reached dispatch/callback,
and the existing focused resource witnesses. It also checked the W09 proposal
and T447 transfer: MENU-01's graph, parser, mutation, owner-draw callbacks and
retirement remain T447-owned and are explicitly not recreated here. No guest
media was read as an implementation input, modified or rebuilt.

The formal graph selects original `wucursor.c`, `wcuricon.c`, `wcurcash.c`,
`waccel.c`, `wuser.c`, `wres32.c` and the accepted `clres.c`/`cldib.c` subset.
It also links the small output-slot bindings in `wow32-dll`; symbol presence is
not treated as runtime acceptance.

## Authority and lifecycle matrix

| Family / entry | Original owner and retained behavior | Modern-host boundary and authority | Normal / failure / retirement | S1 disposition |
| --- | --- | --- | --- | --- |
| Win16 resource lookup and byte lease | Original `wres16/wres32` lookup/lock/free callers; `WU32LoadBitmap` preserves the protected resource-byte lease before invoking its output slot. | The existing resource binding owns only a synchronous, read-only native view of the selected bytes. The original caller remains owner of guest selector access and result publication. | Missing/invalid bytes fail before output; the lease is released on every exit. No host pointer enters guest state. | Reuse W05; do not add a resource manager. |
| Bitmap, named and ID resource loading | Original `wuser.c::WU32LoadBitmap` and accepted original `clres.c::WOWLoadBitmapA`, `ConvertDIBBitmap` and `cldib.c` scaling chain. This preserves old-DIB/header, palette, monochrome and screen-format policy. | `wow_bitmap_load` supplies only the original static scratch/DC capability context. Public GDI owns `HBITMAP`; the original body owns parsing and conversion. | The context serializes the original shared scratch, nests by TLS, and releases DC/lock on success, failure and exception. An absent byte view follows original named/ID load rather than a fabricated DIB path. | Existing binding is source-shaped; S2 must prove its live Win16 caller and partial-load lifetime. |
| Icon / cursor resource loading | Original `wucursor.c::WU32LoadCursor` converts names, calls the output slot and then `wcuricon.c::SetupResCursorIconAlias`; `wcuricon.c` owns Win16/Win32 alias replacement and `wcurcash.c` owns task-local cache records. | `wow_private_user_load_create_cursor_icon` maps the inaccessible NT4 server helper to `CreateIconFromResourceEx` or `LoadImageW`. The host owns the native icon/cursor; original code owns aliases/cache. | Invalid type/zero byte span fails. Existing public route retains source-derived default sizing, cursor monochrome and hotspot scaling. Original task/module cleanup (`FreeCursorIconAlias`, `W32DeleteCursorIconCashForTask`) owns retirement. | Keep one bounded output-slot adaptation. S2 must exercise color/mono and old-header bytes through the actual WU32 caller, including task retirement. |
| Icon/cursor directory selection | Original `WU32WOWGetIdFromDirectory` determines when the lookup is made and returns its guest result. | The unavailable RTL resource-directory helper is projected once through public `LookupIconIdFromDirectoryEx`; it owns no resource storage. | Null/invalid data remains a caller failure; no persistent state. | Keep existing ADAPTER-WOW-015 seam; include malformed directory negative case in S2. |
| Accelerator table load / cross-width conversion | Original `waccel.c`: `WU32LoadAccelerators`, `CreateAccel32`, `CreateAccel16`, `GetAccelHandle32/16`, and alias lookup retain record conversion, last-key representation and task identity. | Public `CreateAcceleratorTable` and `CopyAcceleratorTable` own only native `HACCEL`; no adapter table or new ABI is selected. | Invalid/empty native table fails; original global allocation/lock exits own Win16 storage. `WK32WowCursorIconOp` releases a freed 16-bit alias; `wkman.c::WowOriginalFreeTask` calls `DestroyAccelAlias`. | No source gap identified. S3 will run dispatch/reentry/lifetime acceptance, not replace conversion. |
| MENU-01 consumers | Original T447-owned parser/graph/mutation/owner-draw and retirement remain the only owner. | Existing menu output slot and public USER parser are consumed as established; this task does not retain a second graph or mutation cache. | S2 does not change menus. S3 may only add accelerator dispatch evidence against the already owned menu route. | Explicitly excluded from S2 duplication. |

## Existing witness and omissions

The checked-in `verify-wow-bitmap-bindings.ps1` fixture compiles the actual
production bitmap binding and selected `clres/cldib` owner against synthetic
resource data. It checks named and ID loading, missing resources, nested
contexts, parallel invocation, failure and exception cleanup; it is executed
for this audit as `build/t458-s1/bitmap-r6`: its markers are
`WOW_BITMAP_BINDING_OK nested=30 parallel=4x50 named=2 missing=2` and
`WOW_BITMAP_BINDING_FAILURE_OK locks=2 screen=10/10`. The fixture had retained
an obsolete assumption that the stable CMake root was itself the component
Ninja graph. Its only change accepts that root by selecting its `wow32`
subgraph; it neither changes compile flags nor selects a different provider.
The checked-in cursor fixture supplies INFOHEADER/COREHEADER icon and cursor forms, 1/24-bpp,
monochrome output, default sizing/hotspots, shared stock resources and invalid
data/type rejection. It is a retained source witness, not an S1 runtime pass;
S2 must execute it through the selected original WU32 caller. These are native
boundary witnesses, not guest acceptance.

The audit found no second resource loader, bitmap converter, cursor alias
owner, accelerator table cache or MENU-01 graph. It also found no S1 source
defect to repair. A general USER/CSRSS import, a host-resource pointer in
guest state, an alternative DIB parser, or treating the modern host handle as
a Win16 alias would violate the one-owner boundary.

## Finite successor boundary

S2 owns a single integrated resource-conversion acceptance cohort:

1. drive actual Win16 bitmap/icon/cursor resource calls through their original
   WU32 callers and the registered output slots;
2. cover named/ordinal, color versus monochrome, old-format and malformed/
   partial byte inputs, and verify the original cleanup paths;
3. verify task/module retirement does not retain a stale native alias; and
4. retain the existing bitmap context as the only scratch/DC bridge.

It must not alter guest media, write a second parser, reproduce MENU-01, or
replace `wcuricon.c`/`waccel.c`. S3 owns accelerator dispatch plus the
execution-owned menu interaction boundary. S4 owns consumer/application
acceptance and package closure.
