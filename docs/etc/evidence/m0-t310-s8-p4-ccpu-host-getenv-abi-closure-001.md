# M0 T310 S8 P4 — CCPU host-getenv ABI closure

The original CCPU \`c_bsic.c\` uses the SoftPC portability alias
\`host_getenv\` to select the historical \`SHIFTROT_OF_UNDEF\` profile.
The selected host header already maps that alias to \`getenv\`, but did not
expose the standard pointer-return declaration. Modern x64 consequently
assumed an \`int\` result before comparing it with \`NULL\`.

\`MVDM-HOST-DIV-070\` adds only \`<stdlib.h>\` to the original portability
header. It preserves the alias, source call, environment name and predicate.
The returned value is a private host pointer, so it does not use the guest
mapping manager.

Clean formal Ninja rebuilds of \`obj/ccpu/c_bsic.obj\` succeeded for x64 and
x86. They no longer emit \`C4013\` or \`C4047\` at the original call; other
historical diagnostics remain visible.
