# M0 T396 S19 — WOWEXEC stack-cookie provenance 001

## Question

Does the actual current-binding `WOWEXEC.EXE` product route reach a guest or
WOW32 failure, and which immediate source owner causes its native fast-fail?

## Inputs and method

- Product: `build/M0-T396/S17/kernel-selector/ntvdm32-kernel-entry-v4.exe`,
  SHA-256 `35ab98611f7b7e793c422ed361228039cc10a05e931b1f7fa459f5d354aa4b3d`.
- Stage: `O:\t396`, with the S17 derived Kernel31 contract and the selected
  `system32\WOWEXEC.EXE` workload.
- Ordinary product observation: S18's console-owning run, which records
  `0xc0000409`, 15 modules, and no `WOW32.DLL`.
- Diagnostic contrast only: `tests/observation/fastfail_debug_observer.c`, a
  Console-owning `DEBUG_ONLY_THIS_PROCESS` observer.  It writes no product or
  guest memory.  A debug-register execution watch at image RVA `0x13e3` waits
  until `main` has established `EBP`; a four-byte write watch then follows the
  copied `main` stack-cookie address, ignoring the compiler's own cookie
  initialization store.

The observer was built as
`build/M0-T396/S19-fastfail-watch-observer.exe` and ran the staged product
with `-f -o --command system32\WOWEXEC.EXE`.  Its disposable report is
`build/M0-T396/S19-wowexec-cookie-watch.txt`.

## Observations

The product's actual terminal is `__report_gsfailure` at runtime
`0x01013a7c`, image RVA `0x53a7c`, whose mapped instruction is `int 29h` with
fast-fail code 2.  This is a stack-cookie failure, not the historical direct
KRNL386 `_abort`/code-7 observation.

The first write after `main` initializes the watched cookie is the CRT
`memset` loop at runtime `0x0101488c` (preferred image `0x0045488c`).  Its
return frame is runtime `0x00fc142d`, preferred image `0x0040142d`: the
return address immediately after `entry.c` calls `session_initialize(&owner,
1u)`.

The linked v4 map/disassembly proves the incompatible pair:

| Linked object | Encoded `session` size | ABI version |
| --- | ---: | ---: |
| `session:session.obj`, `session_initialize` | `0x1270` | 7 |
| `entry.obj`, stack storage from `&owner` to its cookie | `0x126c` | source layout at the time of that object |

`session_initialize` clears `0x1270` bytes starting at `&owner`; the caller
allocated only `0x126c` bytes below its cookie.  The four-byte excess reaches
the cookie.  This occurs before guest execution, so the preceding DOSX
`53:00` record is chronological context, not the writer or the failure
owner.

Recompiling each current source object with the declared formal x86 flags
proves their layouts agree: the new `session.obj` clears `0x1268` bytes,
writes ABI version 6 and `struct_bytes=0x1268`; the rebuilt current `entry.obj`
uses the corresponding caller layout.  The remaining required proof is a
fresh complete formal product link, not a replacement source behavior.

## Disposition

S19 closes the native terminal as a stale-object ABI-mixing defect in the
previous v4 archive/product, owned by the formal build composition boundary.
It is neither a WOW32 provider failure nor a CPU/BOP/guest repair.  S20 may
only make a fresh complete source-consistent x86 formal composition and rerun
the same product workload.  It may not retain a selectively replaced object
archive as product evidence.
