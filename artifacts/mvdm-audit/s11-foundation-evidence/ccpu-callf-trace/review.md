# Original CCPU CALLF trace execution

The existing selected CCPU fixture now optionally installs a short synthetic
real-mode instruction sequence: CALLF to a target that changes AL and RETF,
then the existing fixture stop BOP. It executes through the original host
wrapper and CCPU `CALLF` owner with diagnostics disabled, enabled with a
nonmatching selector, and enabled with the matching selector. All three
preserved the expected CS:IP, stack pointer and AL. Only the matching case
produced one scalar far-call record. The link map identifies original
`call.obj` and the project diagnostic object; the output and hashes are here.

The normal fixture without this optional sequence still passed the ten-case
immutable original USER validator and high-page marker. The sequence is a
test-owned memory input, not a modified or rebuilt guest. This verifies a
real-mode selected-object execution path; it does not prove protected-mode
CALLF fault ordering, all nested callbacks, or an actual guest workload with
diagnostics enabled. Those remain separate from this witness and the product
Full gate. No product binary was changed for this test.
