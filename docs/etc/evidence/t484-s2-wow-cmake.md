# T484 S2 P5 WOW32 native CMake graph

The previous PowerShell graph generator is removed. Native CMake now reads
the original WOW sources manifest (72 C units) and retains explicit per-unit
adapter profiles, resource inputs, assembly, import libraries and link order.
Its six fixed ABI carriers are transferred from the former generator's literal
data to templates; only line endings change. The parent import library remains
an actual NTVDM producer dependency. No mirror or original guest is changed.

## Evidence and interpretation

- Initial native build PID2804 passed all235 compile/import/link steps.
- `test/build/test-wow32-build-migration.py` compares actual Ninja commands:
  all228 C compilation units preserve ordered defines, undefines, include and
  forced-header paths, and ABI/optimization options. The remaining units are
  original assembly and two resources. The linked image comparison checks
  every byte, not just section sizes.
- Final cached eleven-image build PID30252 passed,17:12:50Z--17:15:08Z on
  2026-11-22. Publication PID32848 passed after the migration comparison:
  package903798b5-7b13-4d59-b6c2-1e23e6c5babc at both assets/release and
  O:/winnt/system32,22 verified hashes.
- Four raw images are identical to P4; six differ only by PE timestamps.
  WOW32 remains708608 bytes and differs by27 bytes:4 timestamps and23 slash
  versus backslash characters in UTF-16 diagnostic source paths. These are
  not described as raw identity or timestamp-only identity. Code, imports,
  exports and all other bytes are identical.
- Post-publication PID15176 passed linked C-VID, nine-graph434-object A/B
  boundaries,545 authored macro profiles and22 release hashes.
- Actual Win16 message construction PID20100 passed two-task nested sends,
  replies, posts, string copyback, native peer directions and normal retirement;
  structured MINMAXINFO, WINDOWPOS and COPYDATA checks passed.

[Compact evidence](../../../artifacts/build-migration/t484/s2-wow-cmake/migration-review.json)
includes carrier identities and the retained pre-migration graph. The graph
is audit evidence only, never a build input. Reproduce the comparison with
`test/build/test-wow32-build-migration.py --tree <current-WOW-tree>
--baseline-graph <before-build.ninja> --baseline-image <P4-raw-WOW32.dll>`.

## Additional failed observation

An extra Clipboard transfer run timed out with inherited tool environment;
the historical shared-desktop invocation also timed out. The unchanged P4
package reproduced the timeout. A temporary reuse of the existing ordinary
Win16 user-environment helper allowed P4 to exit but produced an84-byte report
with a phase1 error instead of the required full transfer report. The optional
harness experiment was reverted; no product workaround is included. This is
not a Clipboard pass and does not establish a new product regression or a
fully diagnosed host cause. Final acceptance must retain this limitation.
The valid message test above does not substitute for a Clipboard pass.

No new Full run is claimed. Documentation/link/diff gates passed in PID1968.
The containing S2 P5 commit is the delivery identity. S2 remains active:
the original-worker and native-family graph
generators still require migration, followed by S3-S5. Mirror delta is zero.
