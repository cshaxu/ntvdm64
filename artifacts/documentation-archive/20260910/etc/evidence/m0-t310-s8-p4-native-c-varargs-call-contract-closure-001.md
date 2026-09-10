# M0 T310 S8 P4 native C variadic call-contract closure

## Scope

This closure covers every selected `mvdm-host` C4013 record for the standard
C `printf`/`sprintf` family in the formal whole-tree baseline:

- `softpc.new/base/ccpu386/fpu.c`
- `softpc.new/base/ccpu386/popf.c`
- `softpc.new/base/ccpu386/ntstubs.c`
- `softpc.new/base/cvidc/j_c_lang.c`
- `softpc.new/base/cvidc/ev_glue.c`
- `softpc.new/host/src/nt_mem.c`

## Original contract and recovery

The historical source invokes `printf` or `sprintf` without a visible
declaration. Its selected host has an original `fprt.c` `printf` provider;
`sprintf` retains the standard CRT contract. The source-language extension
was not safe to preserve for x64 variadic calls.

`MVDM-HOST-DIV-076` adds `<stdio.h>` only at those six reached source call
sites. It does not replace a provider, change a format string, introduce an
adapter, suppress a warning, or alter guest/mapping-manager state.

## Verification

Using the actual Ninja CCPU target (`original-ccpu386.lib`) rather than an
invented archive name, formal x86/x64 builds selected the reached CCPU,
C-video and host-root archive closure:

- x64: `[99/186]`, zero errors, zero C4013 records for `printf` or `sprintf`.
- x86: `[44/186]`, zero errors, zero C4013 records for `printf` or `sprintf`.

The differing step counts reflect already-valid target-local objects, not a
different source selection. Both commands requested the same three archive
targets: `original-ccpu386.lib`, `original-softpc-cvidc.lib` and
`original-softpc-host-roots.lib`.

## Boundary

This closes the exact native C varargs declaration cluster only. It does not
close unrelated CRT safety/style diagnostics, custom host logging behavior,
or any remaining original SoftPC/NT interface declaration.
