# Current CCPU execution fixture repair

The selected `ccpu-high-linear-page-test` initially failed to link: its
generated target omitted the existing host fixture seams for three VDM client
entrypoints. The generator now links the same fail-closed seam object used
by adjacent CPU fixtures. An attempted link against the production client
library instead also failed for its process-owned CSR heap; it was not used
as a passing result.

The original PIC starts with delayed IRQs blocked until the keyboard-ready
path clears `DelayIrqLine`. This fixture tests the post-ready state but only
initialized it in the bit-matrix configuration. It now sets the same
test-owned prerequisite in both configurations. The original selected PIC
and CPU implementations are unchanged. The test emits its two acceptance
markers through the inherited Windows stdout handle: the linked legacy host
CRT previously swallowed the markers even on exit zero. Both marker writes
are checked for failure.

The cached x86 graph then built and passed the formal original USER handle
validator against the unchanged, hash-verified `USER.EXE`: all ten cases and
the high-linear-page boundary marker appeared. The no-USER control also
passed, exercising the original CPU wrapper and recursive entry. The focused
117-case port-permission witness passed after this repair; Ninja reported no
remaining work for the high-page target. The fixture retains the existing
`/force:multiple` link warnings and controlled device seams; it is not a
substitute for actual guest callback or complete product testing.

The adjacent 13-case bit-fault runner was also rebuilt and **did not pass**:
case 0 reports `#PF`, CR2 `9ffe0ffc`, rather than its expected valid access at
`7ffe0ffc`. The original selected CCPU's unsigned negative bit-index path
matches the previously documented, owner-excluded internal CPU finding; the
test was originally accepted with that now-held candidate. We have not
changed the expectation, restored the held patch, or claimed this runner
passes. It remains a controlled fixture result, not proof of a guest failure.
Exact identities, positive output, and this non-pass are retained here.
No product or guest binary was modified or released by this test repair.
