# Pending WRITE C source roles

The mixed inventory contains 82 pending `wow16/write/*.c` paths. The
[per-file census](write-pending-source-census.json) verifies each current
file against its OpenNT original; all 82 match. Their same-stem objects occur
in the original `write.lnk`; 76 also have explicit same-source object rules
in the original makefile. The other six use the makefile's `.c.obj` cl16
inference rule. The original makefile links with `link16` and names
`write.exe`; conditional source rules have not been evaluated as a guest
build. The makefile and response file hashes are retained with the census.

None of these exact paths appears in the selected x86 worker or WOW32 Ninja
commands or recorded dependencies. The recent DOS crosswalk established
positive controls for those graph queries. The [bounded host-source
dispositions](write-pending-host-dispositions.csv) therefore identify these
as historical guest WRITE sources, excluded from the selected host-source
denominator. This does not exclude WRITE behavior, shared host USER/GDI or
any dialog, message and lifecycle requirements, and does not establish
other configurations or guest runtime behavior. The original guest image
remains unchanged; the frozen mixed-source ledger has not been rewritten.
