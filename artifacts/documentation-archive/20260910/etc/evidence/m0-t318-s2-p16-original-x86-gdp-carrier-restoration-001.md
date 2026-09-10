# M0 T318 S2 P16 — Original x86 GDP carrier restoration

## Question

Does the selected Win32/x86 CPU40 runtime retain the original CCPU/CVIDC GDP
memory contract while the same source graph continues to compile and link on
x64?

## Source finding

The original `softpc.new/base/cvidc/ev_glue.c::setup_global_data_ptr` assigns
one contiguous `host_malloc(64 * 1024)` allocation to both `Gdp` and
`j_Gdp`. The generated CCPU rule sources use `jccc_gdp` as that raw base and
address fields by original offsets. They cannot observe independently
allocated native-width field slots.

The former overlay selected independent slots on both host widths. That was
an x64 compile/link carrier applied to the x86 runtime as well, and therefore
did not preserve the reached original x86 ABI.

## Change

`mvdm_gdp_create`/`mvdm_gdp_destroy` now allocate/free the original contiguous
64 KiB carrier for `_M_IX86`. The generated GDP-slot header leaves all
original GDP macros unchanged on x86. Native-width private slots remain only
for the x64 compile/link carrier, where the original fixed-width pointer slots
cannot be used safely.

This is the registered `MVDM-HOST-DIV-048` exception. It adds no guest token,
no low-address allocation, and no substitute video service.

## Verification

Both formal `cpu40-v7vga-r4` graphs rebuilt and normally linked after the
change:

```text
build/M0-T318/S2/cpu40-v7vga-r4/run-ninja-parallel.cmd original-softpc-process.exe
build/M0-T318/S2/cpu40-v7vga-r4-x64/run-ninja-parallel.cmd original-softpc-process.exe
```

The subsequent x86 incremental invocation reported `ninja: no work to do.`

A debugger-neutral staged x86 run no longer remained in the earlier startup
wait; it now terminates with `0xC0000005`. A bounded debugger capture places
the null transfer's saved return in the original VGA initialization region
near `base/video/vga_prts.c::do_chain_majority_decision`. The selected CVIDC
manifest already contains its generated `SINIT*`, `SEVID*`, `ev_glue.c`,
`evidfunc.c`, `j_c_lang.c`, `accessfn.c`, and `vglfunc.c` sources. This is
therefore an exposed original video/startup closure issue, not evidence to
insert a dummy video callback or to change DEM, COMMAND, NTDOS, or BOP logic.

## Disposition

The x86 GDP ABI correction is closed. The separately observed VGA null
transfer remains an in-scope original SoftPC startup prerequisite for source
attribution; it is not a guest-load, child-`EXEC`, or parent-return result.
