# M0 T352 S4 — durable original COMMAND return observation

## Source-shaped diagnostic change

The existing `MVDM-HOST-DIV-164` return record remains after original
`CmdDispatch` and original `setIP(+1)`.  The adapter now accepts an optional
`MVDM_BOP_RETURN_REPORT_PATH` supplied only by the fixed observer and appends
that already-formatted fixed-width record to it.  Absence of the environment
variable or any report-file failure remains non-fatal.  The observer creates
the path only in the child environment and restores its own prior environment
after `CreateProcess`.

No BOP result, guest byte, guest-memory lease, mapping identity, CPU register,
or session state is changed.

## Verification

- The existing x86 observer rebuilt.
- Ninja incrementally rebuilt the modified adapter object, static library and
  `original-softpc-process.exe` (3 actions) within the already-generated
  selected CPU40/x86 formal graph.
- The linked product SHA-256 is
  `9962c05c8d75b37e584b39e665678601388a8e808008a39b6876c77fb9d81c00`.
- One new immutable-media stage manifest SHA-256 is
  `1dc4d4d35dde7a7721cea8d8e5c308921e3af06a4161303f4f43219d0e8cdf9e`.
- One fixed console-owning, non-debug child timed out at the unchanged
  `0x53504354` watchdog.  Its report SHA-256 is
  `d4ff08bad69831fc4f5794af0d1575e273be6aaecd1e7da9c64c01f03e9a9a09`;
  its final console snapshot SHA-256 is
  `28de9c8bd523e9d06ea52c48f5fc40b53c1e33285c2c257533046024c419cacb`.
- The durable path
  `startup-observation.txt.bop-return.txt` was absent after the child ended.

## Classification

Because the report path is append-only, child-only, and independent of the
final console screen, its absence conclusively establishes that this run did
not reach the existing record after original `CmdDispatch` and original
`setIP(+1)`.  It does not identify the particular edge inside the indirect
COMMAND call.  S5 is limited to that already-selected full COMMAND call
cohort; no CCPU, SAS, BIOS, NTDOS or BOP-provider repair is selected.
