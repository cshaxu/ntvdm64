# M0 T310 S8 P4 XMS A20 call-contract closure

## Scope and source result

The selected keyboard controller and BIOS tape paths call the original
`xmsDisableA20Wrapping` and `xmsEnableA20Wrapping` providers in
`xms.486/xmsa20.c`. Their unconditional original declaration carrier is
`xms.486/xms.h`; the historical conditional declaration from `sas.h` did not
form an effective contract at these selected call sites.

`keyba.c` and `tape_io.c` now include `xms.h` in their top-level include
regions. The call ordering and the original XMS-mediated A20 semantics stay
unchanged. No mapping-manager operation is added: this is a private machine
state transition through the original XMS provider.

## Validation

After cleaning each changed object, formal Ninja rebuilt:

- `obj/keymouse/keyba.obj`, x64 and x86;
- `obj/bios/tape_io.obj`, x64 and x86.

All four units compile with no error and no reviewed XMS A20 C4013 record.
Other existing diagnostics remain visible.
