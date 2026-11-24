# T484 S3 P136 retired census tools and standalone collector

Source/caller inspection resolves four further inventory entries, without
converting obsolete research scripts into new producers.

- B0204 collect-host-mirror-build-evidence.py requires the frozen T480 CSV and
  exact current hashes matching that old census. It assumes old compiler/link
  command spelling when matching compdb rows and final operands. There are no
  current source/test callers. Its historical evidence remains; retire this
  task-specific reproduction tool instead of making a second current census.
- B0206 Export-MvdmWholeClosureInventory.mjs assumes `: lib` archive edges,
  old product output spelling and a literal original wholearchive linker rule.
  Current native CMake composition does not have that graph contract. Retire it;
  do not infer current source retention from this obsolete graph parser.
- B0208 Join-OpenNtRetainedGraphCoverage.ps1 recognizes only cc/cc_* and rc
  rules in T404-era graphs and requires complete source membership/hash equality
  with its T405 inventory. It is not a current product acceptance gate and has
  no current source/test callers. Retire its executable entry, retain reports.
- B0205 collect-mirror-header-consumers.py accepts an explicit plan and headers,
  queries only `ninja -t deps`, and writes a consumer JSON. It has no compiler,
  generator or publication call. Retain as standalone audit: source inspection
  establishes scope; --help verifies its interface only, not the completeness
  of any historical plan or current header census. Its historical output-root
  restriction remains explicit. It is not required by the native build.

B0207 Invoke-ProductVerification remains open because its interpreted package
and build helper edges require subsequent migration. B0209 and following marker
build tools also remain open; no blanket standalone classification applies.

Cached full host build and11 raw/22 publication identities are checked after
removal. No product, mirror, guest or release modification. This is not Full
runtime acceptance; S3/S4/S5 remain open. Raw evidence:
build/t484-s3-retire-census.

[Receipt](../../../artifacts/build-migration/t484/s3-retire-census/result.json).
