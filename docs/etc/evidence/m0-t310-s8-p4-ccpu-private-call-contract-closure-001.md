# M0 T310 S8 P4 CCPU private call-contract closure

## Scope

This closure covers the reached original CCPU facade and exception source only:

- `softpc.new/base/ccpu386/ntstubs.c`
- `softpc.new/base/ccpu386/c_xcptn.c`

The full-tree warning baseline identified three implicit-call records in these
units: `c_cpu_calc_q_ev_inst_for_time`, `c_sas_overwrite_memory`, and
`force_yoda`.

## Source result

`c_main.c` defines `c_cpu_calc_q_ev_inst_for_time(IU32)` and `ccpusas4.c`
defines `c_sas_overwrite_memory(PHY_ADDR, PHY_ADDR)`.  The selected facade is
compiled outside the historical private-CCPU header branch, so its generated
header does not publish these existing providers.  `yoda.h` is the original
diagnostic interface for `force_yoda(void)`.

The facade now publishes only those exact original contracts; the exception
source includes `yoda.h`.  Its former macro-local `host_getenv(char *)`
declaration was removed because `host_def.h` already aliases it to CRT
`getenv(const char *)`; retaining both declarations produced C4028 and could
misdescribe the native pointer result.

No adapter, generated wrapper, pointer conversion, mapping-manager operation,
warning suppression, or call-order change was introduced.

## Validation

The formal Ninja `original-ccpu386.lib` target was rebuilt after cleaning both
changed objects on both host architectures:

- x64: clean `obj/ccpu/{ntstubs,c_xcptn}.obj` then `original-ccpu386.lib`;
- x86: the same clean-and-rebuild sequence.

Both builds reached the archive step with no compiler or linker error.  The
three reviewed C4013 records and the `host_getenv` C4028 record are absent.
Remaining visible diagnostics are separately classified non-width source
diagnostics or existing link ownership records; they are not hidden here.

## Boundary

This is declaration recovery for an existing original SoftPC/CCPU package. It
does not claim CPU execution, device composition, or runtime guest validation.
