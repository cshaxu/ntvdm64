# M0 T310 S31 P1 — host-BOP source boundary

## Original selected carriers

The selected CCPU40 formal graph contains these original bodies without a
replacement dispatcher:

- `mvdm-host/softpc.new/host/src/nt_bop.c`: NT-specific `MS_bop_*` entry
  bodies and the original empty host-specific `host_bop_table` sentinel.
- `mvdm-host/softpc.new/base/bios/cntlbop.c`: original `control_bop` routing
  to `base_bop_table` or `host_bop_table` by the historical host type.
- `mvdm-host/softpc.new/host/src/nt_vdd.c`: original `ms_bop`,
  `get_ms_bop_index`, and `free_MS_bop_index` table contract used by
  `MS_bop_5`.

The empty host table is a source-defined result: the selected Win32 profile
does not invent host-specific control BOP entries.  It is not an incomplete
project dispatcher.

## Selector ownership boundary

`nt_bop.c` is a source-shaped cross-owner ingress, not the owner of the
services it calls.  The selected source body and its IP/flag sequencing stay
intact; its service consequences have these owners:

- `MS_bop_0`, `MS_bop_4`, and `MS_bop_E`: DEM/COMMAND/UMB lifecycle, S35.
- `MS_bop_2`: already closed original XMS.486 package, S15.
- `MS_bop_1`: WOW loader and dispatch, S39.
- `MS_bop_3`: DPMI monitor dispatcher, S37.
- `MS_bop_6` and `MS_bop_B`: debugger package, S42.
- `MS_bop_7`: VDMREDIR loader and dispatcher, S38.
- `MS_bop_5` and `MS_bop_A`: original VDD table/idle entry binding, S30;
  full VDD product behavior stays S40.
- `MS_bop_8`: original ISV dynamic-module extension mechanism; no module is
  enabled by this selected profile, so it retains its original unregistered
  table state until a separately admitted extension owner exists.
- `MS_bop_9` and `MS_bop_C`: direct-access-error and illegal-BOP contracts,
  respectively, retain their S28 source-shaped error owner.
- `MS_bop_D`: explicitly MONITOR-only IRET handling, not selected.
- `MS_bop_F`: original keyboard-vector handoff; controller behavior is S19.

This prevents a host-BOP binding packet from creating a second BOP dispatcher
or from changing any BOP service semantics.

## Formal evidence

The fresh 381-edge x86 CCPU40 formal graph at
`build/M0-T310/S30/formal-host-vdd-x86-r1` compiles and archives
`nt_bop.c`, `cntlbop.c`, and `nt_vdd.c`; forced-link inspection preserves their
original definitions.  The only host-BOP-object unresolved symbols are
`DpmiDispatch` and `DBGDispatch`, which are precisely the explicitly deferred
S37 and S42 owner-package calls.  No selected host-BOP table or VDD-table
binding symbol is unresolved.

Integrated selector execution is a mandatory S49 machine lifecycle proof.  It
cannot be faked by a project-defined direct dispatcher fixture in S31.
