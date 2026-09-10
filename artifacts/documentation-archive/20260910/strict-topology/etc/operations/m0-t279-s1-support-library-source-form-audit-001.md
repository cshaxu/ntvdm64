# M0 T279 S1 — original support-library source-form audit

The original build forms select exactly three host-library bodies:

- `oemuni/sources`: `file.c`, then `process.c`, producing `oemuni.lib`;
- `suballoc/sources`: `suballoc.c`, producing `suballoc.lib`.

The generated 71-row ledger is per original external symbol/caller form:
66 OEM path/process symbols and five suballocator runtime/callback symbols.
It establishes these source-first conclusions without compiling anything:

- public `...W`/environment/path/process APIs retain their original direct
  public Win32 call shape;
- old `NtCurrentTeb`/`NtCurrentPeb` plus RTL strings, heap and PEB locking
  require `adapter-win32` to preserve the historic declarations while keeping
  native pointers local on x86 and x64;
- `suballoc.c` retains numeric `ULONG` address arithmetic and its three
  original caller-supplied callback shapes. The caller, never suballoc,
  reaches `adapter-softpc` for a bounded guest-memory operation.

`toemuni.c`, `tsa.c` and `debug.c` remain non-runtime original test/diagnostic
inputs. The audit does not enable an adapter, a library target or a provider.
