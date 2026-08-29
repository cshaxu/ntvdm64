# M0 T310 S8 P4: video original-call declaration closure

## Scope

This closure covers all nine `C4013` rows in the complete selected
`softpc.new/base/video` owner build: the EGA/VGA port handlers, CGA selector,
EGA reader, and BIOS video service.

## Finding

Each unresolved call already had an original definition in the same SoftPC
video package:

- `setMarkPointers`, `setWritePointers`, and `setReadPointers` are the C_VID
  selector bodies;
- `Glue_set_vid_wrt_ptrs` is the write-handler selection body;
- `set_index_state` and `in_index_state` are EGA port-state bodies;
- `ega_read_dot` is the EGA BIOS video body.

NT4 C accepted those absent declarations as an implicit `int` call contract.
That loses both return and parameter checking in modern x86 and x64 builds.

## Resolution

The five original consumers now carry only the exact same-package
declarations. No definition, table order, selector algorithm, device state,
or host/guest address representation changed. `MVDM-HOST-DIV-067` registers
the minimal mirror differences.

## Verification

The full nine-object base/video owner group was forcibly rebuilt in the formal
Ninja x64 and x86 graphs. Neither architecture emitted `C4013` for these
calls. Other visible warnings remain classified separately and are not
suppressed by this change.
