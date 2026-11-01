# S12: CPU and mouse selections held for behavioral ownership review

The observed MVDM command lines do not explicitly select `SPC486`,
`TAKE_REAL_MODE_LIMIT_FAULT` or `MOUSE_16_BIT`, and no project/generated C or
header definition or undefinition of those labels was found. This identifies
a current build selection; it does not determine whether each historical path
is an unwanted platform alternative or a missing supported behavior.

- `SPC486` gates original CPU branches. The owner excluded unrelated 486
  repairs, and the existing CPU work has not established a complete 486
  contract. Preserve these branches; do not claim a 486 fix or validation.
- `TAKE_REAL_MODE_LIMIT_FAULT` gates original real-mode instruction and
  address-limit checks in CCPU, including calls and interrupts. Its correct
  selection needs an explicit original CPU/guest semantics comparison. It is
  not equivalent to an unused hardware V86 monitor.
- `MOUSE_16_BIT` selects a historical 16-bit mouse-driver path in
  `mouse_io.c`, including initialization and pointer handling. Its relationship
  to the currently selected mouse implementation and supported Win3.1/DOS
  paths has not been established by this label inventory. Keep it intact
  rather than confuse it with the separately installed Win3.1 mouse driver.

No source, guest or product image changed in this review. These are bounded
ownership questions, not assertions of a failure or of runtime equivalence.
