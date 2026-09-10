# M0 T179 guest teardown/control-state attribution closure - 2026-08-12

T179 closes the post-termination vector/stack question. Original source proves
spckbd deliberately installs INT 06 at the `BOP 06; iret` pseudo-ROM handler.
The earlier normal-return trace lacked its original SoftPC `illegal_op_int`
machine component and therefore recursively delivered #UD frames until normal
Bochs stack enforcement stopped it.

The one permitted existing-binary SHARE observation reaches `50:3C`, invokes
the linked original machine island 88 times, eliminates the prior stack fault,
and continues guest execution. It does not reach `54:11`; the next visible
COMMAND boundary is `54:01`. No CLI result, adapter service expansion, device
or vector synthesis is admitted.

Transfer: investigate the original COMMAND `54:01` post-target/no-next-command
lifecycle as a whole source composition, before any engine result transport.
