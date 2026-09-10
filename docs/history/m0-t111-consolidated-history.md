# m0-t111 historical record

Consolidated task record. Raw contemporaneous records are retained in the external documentation archive.

## m0-t111-closure-20260812.md

# M0 T111 closure — 2026-08-12

T111 establishes that selector-02's original unexpected-interrupt policy
belongs to SoftPC, while Bochs owns the PIC port behavior and RAM. Direct
linking is presently unavailable, and an adapter reimplementation is
architecturally forbidden.

T112 is admitted to specify, without implementation, the finite original
handler/shim closure and the smallest possible Bochs generic-port mechanics
ABI. Any future Bochs entry must be separately exception-registered.

