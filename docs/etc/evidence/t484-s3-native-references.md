# T484 S3 P110 native query and queue references

B0179/B0180 retain separate, useful reference contracts. Query checks cover
eight live/dead full/short HWND forms, callbacks, properties and regions, plus
the current product's nine native USER imports. Queue checks cover ANSI/Wide
and both quit producers, ordinary-message ordering and the sentinel bound.
Neither is guest/product runtime acceptance. No active suite caller is
claimed merely because these opt-in references remain useful.

Remove manual CL/link producers and toolchain/provider parameters. Both
binaries are native CMD targets. Their C sources and assertion expectations
are unchanged. Query's stale provider/obj paths are replaced by a generated
list of the four actual CMake product objects; dependencies ensure they are
current before the existing dumpbin symbol assertion. Dumpbin remains a
read-only assertion tool, not an interpreted build producer. Product objects
are inspected, not linked into the native reference binary.

Owned PID19468 exits0: query231 checks/eight rows, all nine native import
bindings, and four A/W quit combinations pass. These are results on the
current Windows host, not an assertion of all USER versions.
Full cached host build, two hash/mtime-stable native outputs and x64
rejection exit2 pass. Eleven raw host and22 published hashes remain identical
to the current manifest. Existing publication verified, not republished.
No product, mirror, original guest or asset change.

[Receipt](../../../artifacts/build-migration/t484/s3-native-references/result.json)
contains process and cache identities. Raw logs and symbol output:
build/t484-s3-native-references.
Inventory211 complete/144 remaining of355. Ordered review resumes at B0182
because B0181 was delivered with its B0178 caller in P109.
B0166 native Idle and B0169 DLL-provider reconciliation remain open;
S3/retrospective review and S4/S5 acceptance remain incomplete.
