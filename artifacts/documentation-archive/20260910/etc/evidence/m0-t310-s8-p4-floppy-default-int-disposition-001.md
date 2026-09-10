# M0 T310 S8 P4 floppy default-int disposition

## Scope

The complete 36-record C4431 cluster is confined to the original
`softpc.new/base/disks/floppy.c` source file.

## Original contract

The first group consists of file-local K&R forward declarations such as
`LOCAL cmos_type IPT2(...)`, `LOCAL wait_int IPT0()` and `LOCAL results
IPT0()`. The later corresponding definitions use the same unqualified
`LOCAL name IFN...` form, so both declaration and definition retain the
historical default `int` result contract. The remaining locations are the
corresponding selected function definitions and returns.

The operation jump table itself already has an explicit `void(int drive)`
type. It is not the source of this diagnostic cluster. No function pointer,
native pointer, guest address, handle, mapping identity, or calling convention
changes width between x86 and x64; the same C4431 records occur on both
targets.

## Disposition

This is a legacy C language-form diagnostic, not an x86/x64 ABI transition.
The mirror remains byte/logic-identical: adding modern return types would be
a broad source-style rewrite rather than a width repair. All 36 records are
classified `not-x86-x64-original-floppy-default-int-source-form` and remain
compiler-visible. Any later cleanup must be a dedicated original-controller
source-form decision, not an ABI workaround.
