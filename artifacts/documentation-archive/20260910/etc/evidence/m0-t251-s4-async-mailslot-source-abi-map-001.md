# M0 T251 S4 — async named-pipe and mailslot source/ABI map

## Recovered original contracts

| Group | OpenNT evidence | Guest-visible contract | Composition consequence |
| --- | --- | --- | --- |
| Async pipe `57:23/24` | `redir/namepipe.asm:426+`, `vrnmpipe.h:DOS_ASYNC_NAMED_PIPE_STRUCT` | `DS:SI` points to packed 20/24-byte descriptor; a mapped `BP:BX` 32-bit opaque pipe handle identifies the native pipe. | Descriptor, buffer, bytes/error destinations and ANR/semaphore fields must cross only through bounded checked RAM. |
| Async completion | `vrnmpipe.h:DOS_ASYNC_NAMED_PIPE_INFO`, `vdmredir.h:VR_ASYNC_DISPOSITION`, `redir/int5c.asm:318+` | The original completion is serialized and reaches an ANR through the guest network interrupt path, not a host callback. | A bx-vdm queue may retain copied completion records, but activation of the guest ANR requires an already-declared selector-blind interrupt/device continuation. No worker gets a guest pointer, selector, or callback address. |
| Mailslot `57:09..0F` | `redir/mailslot.asm`, `vrmslot.h:VR_MAILSLOT_INFO`, `apistruc.h:DosWriteMailslotStruct` | `57:0B` invents a DOS 16-bit handle; records current PDB, DOS buffer address/selector, message size and native mailslot handle. `09` verifies PDB ownership before delete. | The existing 32-bit opaque host-handle manager remains the only host-HANDLE bridge. A Redirector-local 16-bit legacy-handle index maps to that manager token; it does not truncate or expose a host handle. |

## Decisions

1. Mailslot recovery can proceed in bx-vdm with public `CreateMailslotW`,
   `GetMailslotInfo`, `ReadFile`, `WriteFile` and checked guest spans.
2. Async dispatch admission can validate descriptor shape and create a
   session-owned pending record, but **must not claim completion** until a
   selector-blind guest interrupt delivery seam can deliver the original
   `int5c.asm` continuation.  It must return an explicit unavailable result
   for any operation requiring that missing delivery.
3. Neither group authorizes a Bochs/bx-core/bx-mantle semantic change, a raw
   guest pointer, raw host handle, native callback, CCPU thread, CSR, or
   BaseSrv dependency.
