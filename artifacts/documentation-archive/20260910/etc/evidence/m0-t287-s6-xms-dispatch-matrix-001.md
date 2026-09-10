# M0 T287 S6 — original XMS dispatcher local matrix evidence

## Result

S6 closes the local dispatcher regression rung. The exact original
`mvdm-host/xms.486/xms.c` initialization and `xmsdisp.c` dispatch table are
compiled and invoked on both x86 and x64. All twelve source table entries run
through the already-admitted same-shaped bindings; none is replaced with a
selector-specific provider and no external `52:xx` BOP ingress is enabled.

## Source table and disposition

- `XMSInit`: retains its i386 source order, including its empty `ReserveUMB`
  call and `SAInitialize` range selection.
- `0`: `xmsA20` retains source AX/BL results over the typed A20 operation.
- `1`: `xmsMoveBlock` retains the original stack descriptor body and uses the
  S4 bounded-lease move binding.
- `2`, `3`, and `11`: `xmsAllocBlock`, `xmsFreeBlock`, and
  `xmsReallocBlock` retain the original `suballoc.c` lifecycle.
- `4`, `5`, and `10`: page size, extended-memory size, and free-memory query
  retain their original local service bodies.
- `6`, `7`, and `8`: UMB initialization/request/release retain the original
  empty-physical-span `B1`/`B2` failure outcome. Successful span ownership
  remains transferred to the named monitor substrate owner.
- `9`: `xmsNotifyHookI15` retains `UpdateKbdInt15(CS, AX)` followed by the
  original `CX` result. The matrix uses only a test carrier; the production
  keyboard/IVT algorithm remains the named firmware prerequisite.

The fixture keeps one source allocation alive while separately allocating and
freeing a second block before testing reallocation. Its fixture reservation
starts at `0x110000`, deliberately avoiding the source allocator's legal zero
address so that the historical `AX=0` failure convention remains unambiguous
for this service-level test.

## Verification

`tools/build/New-T287S6XmsDispatchNinja.ps1` generated formal MSVC `/MT`
Ninja graphs with source SHA-256 manifests. They explicitly reject `src.old`,
the direct-pointer `i386/xmsmem86.c` backend, and prebuilt product archives.

- `build/M0-T287/S6/x86/bin/xms-family-matrix.exe`: exit `0`.
- `build/M0-T287/S6/x64/bin/xms-family-matrix.exe`: exit `0`.

Both executable runs construct only the minimal selector-blind Bochs machine
and a session-owned guest-memory lease context. They do not claim a native
guest trace, a successful UMB physical span, keyboard-controller behavior, or
an enabled BOP route.
