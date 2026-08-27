# M0 T290 S3 P16 — Original COMMAND redirection identity composition

The formal x86/x64 `cmdredir.c` fixture now executes the original non-pipe
`cmdGetStdHandle` path with source-shaped session identities. The historical
`AX:BX` record pointer is resolved only through the existing session
host-resource mapping, and the source's returned `BX:CX` standard handle is
published as a second opaque host-resource identity. The fixture resolves that
identity back to the expected native standard handle, then invokes the
original `cmdCheckCopyForRedirection` cleanup.

The only mirror divergences (`MVDM-HOST-DIV-020`) replace x86 pointer/handle
width coincidence: record resolution before the source switch, per-branch
opaque handle publication in the existing register order, and identity
retirement at the source ownership/free point. Pipe worker branches, process
global standard-handle changes and a new broker remain excluded.

Both formal Ninja architectures exit `0`. This is direct original-body local
composition only: the complete `cmdmisc.c`/`cmdexec.c` producer-consumer
handoff of the record identity is explicitly owned by T290 S6 whole-family
reconciliation. No selector ingress, raw guest pointer, raw host handle,
second broker or mapping manager is introduced.
