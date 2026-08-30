# M0 T313 S2 P6 — original CRT binding audit

Date: 2026-08-30  
Packet: M0 T313 S2  
Profile: selected Win32/x86 CCPU40.

## Finding

The selected original SoftPC graph contains two intentional historical CRT
contracts:

- `softpc.new/host/src/fprt.c` owns the original `printf` and `fprintf`
  interposition bodies;
- other unchanged original MVDM callers use the historic external `_sprintf`
  and `_sscanf` spellings.

Modern `/MT` UCRT inlines the first pair by default.  Disabling those inlines
for every selected original translation unit is necessary to retain one
observable `fprt.c` owner.  That mode makes the second pair unresolved, and
MSVC's vendor `legacy_stdio_definitions.lib` supplies those historical names.

## Disposition

The formal Ninja graph therefore:

1. keeps the graph-wide `_NO_CRT_STDIO_INLINE` declaration mode;
2. links the vendor `legacy_stdio_definitions.lib` after the selected original
   archives; and
3. records the forced-link `printf`/`fprintf` duplicate diagnostic rather
   than suppressing it.  The link order selects `fprt.c` first; the vendor
   object is reported as the ignored second provider.

An attempted per-file `fprt.c` switch was rejected: it allowed UCRT inline
providers in CCPU objects and created more competing original-path
definitions.  No MVDM source body, caller spelling, or project-owned CRT shim
was introduced.

## Verification

The fresh external r1 forced-link log has no unresolved `_sprintf` or
`_sscanf` record.  It records only:

```text
legacy_stdio_definitions.lib(...): warning LNK4006: _fprintf already defined
in original-softpc-host-roots.lib(fprt.obj); second definition ignored
legacy_stdio_definitions.lib(...): warning LNK4006: _printf already defined
in original-softpc-host-roots.lib(fprt.obj); second definition ignored
```

The remaining `LNK2019` records are distinct historical product boundaries:
EMS/LIM writes, module movement, debugger prompt/dispatch/init, start-glass,
and native virtual-memory/hard-error calls.  They remain visible for their
owner-package adapters; this forced-link audit DLL is deliberately non-runnable.
